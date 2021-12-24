/*
 * Copyright © 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file iris_bufmgr.c
 *
 * The Iris buffer manager.
 *
 * XXX: write better comments
 * - BOs
 * - Explain BO cache
 * - main interface to GEM in the kernel
 */

#include <xf86drm.h>
#include <util/u_atomic.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "errno.h"
#include "common/intel_aux_map.h"
#include "common/intel_clflush.h"
#include "dev/intel_debug.h"
#include "common/intel_gem.h"
#include "dev/intel_device_info.h"
#include "isl/isl.h"
#include "main/macros.h"
#include "os/os_mman.h"
#include "util/debug.h"
#include "util/macros.h"
#include "util/hash_table.h"
#include "util/list.h"
#include "util/os_file.h"
#include "util/u_dynarray.h"
#include "util/vma.h"
#include "iris_bufmgr.h"
#include "iris_context.h"
#include "string.h"

#include "drm-uapi/i915_drm.h"

#ifdef HAVE_VALGRIND
#include <valgrind.h>
#include <memcheck.h>
#define VG(x) x
#else
#define VG(x)
#endif

/* VALGRIND_FREELIKE_BLOCK unfortunately does not actually undo the earlier
 * VALGRIND_MALLOCLIKE_BLOCK but instead leaves vg convinced the memory is
 * leaked. All because it does not call VG(cli_free) from its
 * VG_USERREQ__FREELIKE_BLOCK handler. Instead of treating the memory like
 * and allocation, we mark it available for use upon mmapping and remove
 * it upon unmapping.
 */
#define VG_DEFINED(ptr, size) VG(VALGRIND_MAKE_MEM_DEFINED(ptr, size))
#define VG_NOACCESS(ptr, size) VG(VALGRIND_MAKE_MEM_NOACCESS(ptr, size))

/* On FreeBSD PAGE_SIZE is already defined in
 * /usr/include/machine/param.h that is indirectly
 * included here.
 */
#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#define WARN_ONCE(cond, fmt...) do {                            \
   if (unlikely(cond)) {                                        \
      static bool _warned = false;                              \
      if (!_warned) {                                           \
         fprintf(stderr, "WARNING: ");                          \
         fprintf(stderr, fmt);                                  \
         _warned = true;                                        \
      }                                                         \
   }                                                            \
} while (0)

#define FILE_DEBUG_FLAG DEBUG_BUFMGR

/**
 * For debugging purposes, this returns a time in seconds.
 */
static double
get_time(void)
{
   struct timespec tp;

   clock_gettime(CLOCK_MONOTONIC, &tp);

   return tp.tv_sec + tp.tv_nsec / 1000000000.0;
}

static inline int
atomic_add_unless(int *v, int add, int unless)
{
   int c, old;
   c = p_atomic_read(v);
   while (c != unless && (old = p_atomic_cmpxchg(v, c, c + add)) != c)
      c = old;
   return c == unless;
}

static const char *
memzone_name(enum iris_memory_zone memzone)
{
   const char *names[] = {
      [IRIS_MEMZONE_SHADER]   = "shader",
      [IRIS_MEMZONE_BINDER]   = "binder",
      [IRIS_MEMZONE_BINDLESS] = "scratchsurf",
      [IRIS_MEMZONE_SURFACE]  = "surface",
      [IRIS_MEMZONE_DYNAMIC]  = "dynamic",
      [IRIS_MEMZONE_OTHER]    = "other",
      [IRIS_MEMZONE_BORDER_COLOR_POOL] = "bordercolor",
   };
   assert(memzone < ARRAY_SIZE(names));
   return names[memzone];
}

struct bo_cache_bucket {
   /** List of cached BOs. */
   struct list_head head;

   /** Size of this bucket, in bytes. */
   uint64_t size;
};

struct bo_export {
   /** File descriptor associated with a handle export. */
   int drm_fd;

   /** GEM handle in drm_fd */
   uint32_t gem_handle;

   struct list_head link;
};

struct iris_memregion {
   struct drm_i915_gem_memory_class_instance region;
   uint64_t size;
};

struct iris_bufmgr {
   /**
    * List into the list of bufmgr.
    */
   struct list_head link;

   uint32_t refcount;

   int fd;

   simple_mtx_t lock;

   /** Array of lists of cached gem objects of power-of-two sizes */
   struct bo_cache_bucket cache_bucket[14 * 4];
   int num_buckets;

   /** Same as cache_bucket, but for local memory gem objects */
   struct bo_cache_bucket local_cache_bucket[14 * 4];
   int num_local_buckets;

   time_t time;

   struct hash_table *name_table;
   struct hash_table *handle_table;

   /**
    * List of BOs which we've effectively freed, but are hanging on to
    * until they're idle before closing and returning the VMA.
    */
   struct list_head zombie_list;

   struct util_vma_heap vma_allocator[IRIS_MEMZONE_COUNT];

   uint64_t vma_min_align;
   struct iris_memregion vram, sys;

   bool has_llc:1;
   bool has_mmap_offset:1;
   bool has_tiling_uapi:1;
   bool bo_reuse:1;

   struct intel_aux_map_context *aux_map_ctx;
};

static simple_mtx_t global_bufmgr_list_mutex = _SIMPLE_MTX_INITIALIZER_NP;
static struct list_head global_bufmgr_list = {
   .next = &global_bufmgr_list,
   .prev = &global_bufmgr_list,
};

static void bo_free(struct iris_bo *bo);

static struct iris_bo *
find_and_ref_external_bo(struct hash_table *ht, unsigned int key)
{
   struct hash_entry *entry = _mesa_hash_table_search(ht, &key);
   struct iris_bo *bo = entry ? entry->data : NULL;

   if (bo) {
      assert(iris_bo_is_external(bo));
      assert(!bo->reusable);

      /* Being non-reusable, the BO cannot be in the cache lists, but it
       * may be in the zombie list if it had reached zero references, but
       * we hadn't yet closed it...and then reimported the same BO.  If it
       * is, then remove it since it's now been resurrected.
       */
      if (list_is_linked(&bo->head))
         list_del(&bo->head);

      iris_bo_reference(bo);
   }

   return bo;
}

/**
 * This function finds the correct bucket fit for the input size.
 * The function works with O(1) complexity when the requested size
 * was queried instead of iterating the size through all the buckets.
 */
static struct bo_cache_bucket *
bucket_for_size(struct iris_bufmgr *bufmgr, uint64_t size, bool local)
{
   /* Calculating the pages and rounding up to the page size. */
   const unsigned pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

   /* Row  Bucket sizes    clz((x-1) | 3)   Row    Column
    *        in pages                      stride   size
    *   0:   1  2  3  4 -> 30 30 30 30        4       1
    *   1:   5  6  7  8 -> 29 29 29 29        4       1
    *   2:  10 12 14 16 -> 28 28 28 28        8       2
    *   3:  20 24 28 32 -> 27 27 27 27       16       4
    */
   const unsigned row = 30 - __builtin_clz((pages - 1) | 3);
   const unsigned row_max_pages = 4 << row;

   /* The '& ~2' is the special case for row 1. In row 1, max pages /
    * 2 is 2, but the previous row maximum is zero (because there is
    * no previous row). All row maximum sizes are power of 2, so that
    * is the only case where that bit will be set.
    */
   const unsigned prev_row_max_pages = (row_max_pages / 2) & ~2;
   int col_size_log2 = row - 1;
   col_size_log2 += (col_size_log2 < 0);

   const unsigned col = (pages - prev_row_max_pages +
                        ((1 << col_size_log2) - 1)) >> col_size_log2;

   /* Calculating the index based on the row and column. */
   const unsigned index = (row * 4) + (col - 1);

   int num_buckets = local ? bufmgr->num_local_buckets : bufmgr->num_buckets;
   struct bo_cache_bucket *buckets = local ?
      bufmgr->local_cache_bucket : bufmgr->cache_bucket;

   return (index < num_buckets) ? &buckets[index] : NULL;
}

enum iris_memory_zone
iris_memzone_for_address(uint64_t address)
{
   STATIC_ASSERT(IRIS_MEMZONE_OTHER_START    > IRIS_MEMZONE_DYNAMIC_START);
   STATIC_ASSERT(IRIS_MEMZONE_DYNAMIC_START  > IRIS_MEMZONE_SURFACE_START);
   STATIC_ASSERT(IRIS_MEMZONE_SURFACE_START  > IRIS_MEMZONE_BINDLESS_START);
   STATIC_ASSERT(IRIS_MEMZONE_BINDLESS_START > IRIS_MEMZONE_BINDER_START);
   STATIC_ASSERT(IRIS_MEMZONE_BINDER_START   > IRIS_MEMZONE_SHADER_START);
   STATIC_ASSERT(IRIS_BORDER_COLOR_POOL_ADDRESS == IRIS_MEMZONE_DYNAMIC_START);

   if (address >= IRIS_MEMZONE_OTHER_START)
      return IRIS_MEMZONE_OTHER;

   if (address == IRIS_BORDER_COLOR_POOL_ADDRESS)
      return IRIS_MEMZONE_BORDER_COLOR_POOL;

   if (address > IRIS_MEMZONE_DYNAMIC_START)
      return IRIS_MEMZONE_DYNAMIC;

   if (address >= IRIS_MEMZONE_SURFACE_START)
      return IRIS_MEMZONE_SURFACE;

   if (address >= IRIS_MEMZONE_BINDLESS_START)
      return IRIS_MEMZONE_BINDLESS;

   if (address >= IRIS_MEMZONE_BINDER_START)
      return IRIS_MEMZONE_BINDER;

   return IRIS_MEMZONE_SHADER;
}

/**
 * Allocate a section of virtual memory for a buffer, assigning an address.
 *
 * This uses either the bucket allocator for the given size, or the large
 * object allocator (util_vma).
 */
static uint64_t
vma_alloc(struct iris_bufmgr *bufmgr,
          enum iris_memory_zone memzone,
          uint64_t size,
          uint64_t alignment)
{
   /* Force minimum alignment based on device requirements */
   assert((alignment & (alignment - 1)) == 0);
   alignment = MAX2(alignment, bufmgr->vma_min_align);

   if (memzone == IRIS_MEMZONE_BORDER_COLOR_POOL)
      return IRIS_BORDER_COLOR_POOL_ADDRESS;

   /* The binder handles its own allocations.  Return non-zero here. */
   if (memzone == IRIS_MEMZONE_BINDER)
      return IRIS_MEMZONE_BINDER_START;

   uint64_t addr =
      util_vma_heap_alloc(&bufmgr->vma_allocator[memzone], size, alignment);

   assert((addr >> 48ull) == 0);
   assert((addr % alignment) == 0);

   return intel_canonical_address(addr);
}

static void
vma_free(struct iris_bufmgr *bufmgr,
         uint64_t address,
         uint64_t size)
{
   if (address == IRIS_BORDER_COLOR_POOL_ADDRESS)
      return;

   /* Un-canonicalize the address. */
   address = intel_48b_address(address);

   if (address == 0ull)
      return;

   enum iris_memory_zone memzone = iris_memzone_for_address(address);

   /* The binder handles its own allocations. */
   if (memzone == IRIS_MEMZONE_BINDER)
      return;

   assert(memzone < ARRAY_SIZE(bufmgr->vma_allocator));

   util_vma_heap_free(&bufmgr->vma_allocator[memzone], address, size);
}

int
iris_bo_busy(struct iris_bo *bo)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;
   struct drm_i915_gem_busy busy = { .handle = bo->gem_handle };

   int ret = intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_BUSY, &busy);
   if (ret == 0) {
      bo->idle = !busy.busy;
      return busy.busy;
   }
   return false;
}

int
iris_bo_madvise(struct iris_bo *bo, int state)
{
   struct drm_i915_gem_madvise madv = {
      .handle = bo->gem_handle,
      .madv = state,
      .retained = 1,
   };

   intel_ioctl(bo->bufmgr->fd, DRM_IOCTL_I915_GEM_MADVISE, &madv);

   return madv.retained;
}

static struct iris_bo *
bo_calloc(void)
{
   struct iris_bo *bo = calloc(1, sizeof(*bo));
   if (!bo)
      return NULL;

   list_inithead(&bo->exports);

   bo->hash = _mesa_hash_pointer(bo);

   return bo;
}

static void
bo_unmap(struct iris_bo *bo)
{
   VG_NOACCESS(bo->map, bo->size);
   os_munmap(bo->map, bo->size);
   bo->map = NULL;
}

static struct iris_bo *
alloc_bo_from_cache(struct iris_bufmgr *bufmgr,
                    struct bo_cache_bucket *bucket,
                    uint32_t alignment,
                    enum iris_memory_zone memzone,
                    enum iris_mmap_mode mmap_mode,
                    unsigned flags,
                    bool match_zone)
{
   if (!bucket)
      return NULL;

   struct iris_bo *bo = NULL;

   list_for_each_entry_safe(struct iris_bo, cur, &bucket->head, head) {
      /* Find one that's got the right mapping type.  We used to swap maps
       * around but the kernel doesn't allow this on discrete GPUs.
       */
      if (mmap_mode != cur->mmap_mode)
         continue;

      /* Try a little harder to find one that's already in the right memzone */
      if (match_zone && memzone != iris_memzone_for_address(cur->gtt_offset))
         continue;

      /* If the last BO in the cache is busy, there are no idle BOs.  Bail,
       * either falling back to a non-matching memzone, or if that fails,
       * allocating a fresh buffer.
       */
      if (iris_bo_busy(cur))
         return NULL;

      list_del(&cur->head);

      /* Tell the kernel we need this BO.  If it still exists, we're done! */
      if (iris_bo_madvise(cur, I915_MADV_WILLNEED)) {
         bo = cur;
         break;
      }

      /* This BO was purged, throw it out and keep looking. */
      bo_free(cur);
   }

   if (!bo)
      return NULL;

   if (bo->aux_map_address) {
      /* This buffer was associated with an aux-buffer range. We make sure
       * that buffers are not reused from the cache while the buffer is (busy)
       * being used by an executing batch. Since we are here, the buffer is no
       * longer being used by a batch and the buffer was deleted (in order to
       * end up in the cache). Therefore its old aux-buffer range can be
       * removed from the aux-map.
       */
      if (bo->bufmgr->aux_map_ctx)
         intel_aux_map_unmap_range(bo->bufmgr->aux_map_ctx, bo->gtt_offset,
                                   bo->size);
      bo->aux_map_address = 0;
   }

   /* If the cached BO isn't in the right memory zone, or the alignment
    * isn't sufficient, free the old memory and assign it a new address.
    */
   if (memzone != iris_memzone_for_address(bo->gtt_offset) ||
       bo->gtt_offset % alignment != 0) {
      vma_free(bufmgr, bo->gtt_offset, bo->size);
      bo->gtt_offset = 0ull;
   }

   /* Zero the contents if necessary.  If this fails, fall back to
    * allocating a fresh BO, which will always be zeroed by the kernel.
    */
   if (flags & BO_ALLOC_ZEROED) {
      void *map = iris_bo_map(NULL, bo, MAP_WRITE | MAP_RAW);
      if (map) {
         memset(map, 0, bo->size);
      } else {
         bo_free(bo);
         return NULL;
      }
   }

   return bo;
}

static struct iris_bo *
alloc_fresh_bo(struct iris_bufmgr *bufmgr, uint64_t bo_size, bool local)
{
   struct iris_bo *bo = bo_calloc();
   if (!bo)
      return NULL;

   /* If we have vram size, we have multiple memory regions and should choose
    * one of them.
    */
   if (bufmgr->vram.size > 0) {
      /* All new BOs we get from the kernel are zeroed, so we don't need to
       * worry about that here.
       */
      struct drm_i915_gem_memory_class_instance regions[2];
      uint32_t nregions = 0;
      if (local) {
         /* For vram allocations, still use system memory as a fallback. */
         regions[nregions++] = bufmgr->vram.region;
         regions[nregions++] = bufmgr->sys.region;
      } else {
         regions[nregions++] = bufmgr->sys.region;
      }

      struct drm_i915_gem_create_ext_memory_regions ext_regions = {
         .base = { .name = I915_GEM_CREATE_EXT_MEMORY_REGIONS },
         .num_regions = nregions,
         .regions = (uintptr_t)regions,
      };

      struct drm_i915_gem_create_ext create = {
         .size = bo_size,
         .extensions = (uintptr_t)&ext_regions,
      };

      /* It should be safe to use GEM_CREATE_EXT without checking, since we are
       * in the side of the branch where discrete memory is available. So we
       * can assume GEM_CREATE_EXT is supported already.
       */
      if (intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_CREATE_EXT, &create) != 0) {
         free(bo);
         return NULL;
      }
      bo->gem_handle = create.handle;
   } else {
      struct drm_i915_gem_create create = { .size = bo_size };

      /* All new BOs we get from the kernel are zeroed, so we don't need to
       * worry about that here.
       */
      if (intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_CREATE, &create) != 0) {
         free(bo);
         return NULL;
      }
      bo->gem_handle = create.handle;
   }

   bo->bufmgr = bufmgr;
   bo->size = bo_size;
   bo->idle = true;
   bo->local = local;

   /* Calling set_domain() will allocate pages for the BO outside of the
    * struct mutex lock in the kernel, which is more efficient than waiting
    * to create them during the first execbuf that uses the BO.
    */
   struct drm_i915_gem_set_domain sd = {
      .handle = bo->gem_handle,
      .read_domains = I915_GEM_DOMAIN_CPU,
      .write_domain = 0,
   };

   if (intel_ioctl(bo->bufmgr->fd, DRM_IOCTL_I915_GEM_SET_DOMAIN, &sd) != 0) {
      bo_free(bo);
      return NULL;
   }

   return bo;
}

struct iris_bo *
iris_bo_alloc(struct iris_bufmgr *bufmgr,
              const char *name,
              uint64_t size,
              uint32_t alignment,
              enum iris_memory_zone memzone,
              unsigned flags)
{
   struct iris_bo *bo;
   unsigned int page_size = getpagesize();
   bool local = bufmgr->vram.size > 0 &&
      !(flags & BO_ALLOC_COHERENT || flags & BO_ALLOC_SMEM);
   struct bo_cache_bucket *bucket = bucket_for_size(bufmgr, size, local);

   /* Round the size up to the bucket size, or if we don't have caching
    * at this size, a multiple of the page size.
    */
   uint64_t bo_size =
      bucket ? bucket->size : MAX2(ALIGN(size, page_size), page_size);

   bool is_coherent = bufmgr->has_llc || (flags & BO_ALLOC_COHERENT);
   enum iris_mmap_mode mmap_mode =
      !local && is_coherent ? IRIS_MMAP_WB : IRIS_MMAP_WC;

   simple_mtx_lock(&bufmgr->lock);

   /* Get a buffer out of the cache if available.  First, we try to find
    * one with a matching memory zone so we can avoid reallocating VMA.
    */
   bo = alloc_bo_from_cache(bufmgr, bucket, alignment, memzone, mmap_mode,
                            flags, true);

   /* If that fails, we try for any cached BO, without matching memzone. */
   if (!bo) {
      bo = alloc_bo_from_cache(bufmgr, bucket, alignment, memzone, mmap_mode,
                               flags, false);
   }

   simple_mtx_unlock(&bufmgr->lock);

   if (!bo) {
      bo = alloc_fresh_bo(bufmgr, bo_size, local);
      if (!bo)
         return NULL;
   }

   if (bo->gtt_offset == 0ull) {
      simple_mtx_lock(&bufmgr->lock);
      bo->gtt_offset = vma_alloc(bufmgr, memzone, bo->size, alignment);
      simple_mtx_unlock(&bufmgr->lock);

      if (bo->gtt_offset == 0ull)
         goto err_free;
   }

   bo->name = name;
   p_atomic_set(&bo->refcount, 1);
   bo->reusable = bucket && bufmgr->bo_reuse;
   bo->index = -1;
   bo->kflags = EXEC_OBJECT_SUPPORTS_48B_ADDRESS | EXEC_OBJECT_PINNED;

   /* By default, capture all driver-internal buffers like shader kernels,
    * surface states, dynamic states, border colors, and so on.
    */
   if (memzone < IRIS_MEMZONE_OTHER)
      bo->kflags |= EXEC_OBJECT_CAPTURE;

   assert(bo->map == NULL || bo->mmap_mode == mmap_mode);
   bo->mmap_mode = mmap_mode;

   /* On integrated GPUs, enable snooping to ensure coherency if needed.
    * For discrete, we instead use SMEM and avoid WB maps for coherency.
    */
   if ((flags & BO_ALLOC_COHERENT) &&
       !bufmgr->has_llc && bufmgr->vram.size == 0) {
      struct drm_i915_gem_caching arg = {
         .handle = bo->gem_handle,
         .caching = 1,
      };
      if (intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_SET_CACHING, &arg) != 0)
         goto err_free;

      bo->reusable = false;
   }

   DBG("bo_create: buf %d (%s) (%s memzone) (%s) %llub\n", bo->gem_handle,
       bo->name, memzone_name(memzone), bo->local ? "local" : "system",
       (unsigned long long) size);

   return bo;

err_free:
   bo_free(bo);
   return NULL;
}

struct iris_bo *
iris_bo_create_userptr(struct iris_bufmgr *bufmgr, const char *name,
                       void *ptr, size_t size,
                       enum iris_memory_zone memzone)
{
   struct drm_gem_close close = { 0, };
   struct iris_bo *bo;

   bo = bo_calloc();
   if (!bo)
      return NULL;

   struct drm_i915_gem_userptr arg = {
      .user_ptr = (uintptr_t)ptr,
      .user_size = size,
   };
   if (intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_USERPTR, &arg))
      goto err_free;
   bo->gem_handle = arg.handle;

   /* Check the buffer for validity before we try and use it in a batch */
   struct drm_i915_gem_set_domain sd = {
      .handle = bo->gem_handle,
      .read_domains = I915_GEM_DOMAIN_CPU,
   };
   if (intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_SET_DOMAIN, &sd))
      goto err_close;

   bo->name = name;
   bo->size = size;
   bo->map = ptr;

   bo->bufmgr = bufmgr;
   bo->kflags = EXEC_OBJECT_SUPPORTS_48B_ADDRESS | EXEC_OBJECT_PINNED;

   simple_mtx_lock(&bufmgr->lock);
   bo->gtt_offset = vma_alloc(bufmgr, memzone, size, 1);
   simple_mtx_unlock(&bufmgr->lock);

   if (bo->gtt_offset == 0ull)
      goto err_close;

   p_atomic_set(&bo->refcount, 1);
   bo->userptr = true;
   bo->index = -1;
   bo->idle = true;
   bo->mmap_mode = IRIS_MMAP_WB;

   return bo;

err_close:
   close.handle = bo->gem_handle;
   intel_ioctl(bufmgr->fd, DRM_IOCTL_GEM_CLOSE, &close);
err_free:
   free(bo);
   return NULL;
}

/**
 * Returns a iris_bo wrapping the given buffer object handle.
 *
 * This can be used when one application needs to pass a buffer object
 * to another.
 */
struct iris_bo *
iris_bo_gem_create_from_name(struct iris_bufmgr *bufmgr,
                             const char *name, unsigned int handle)
{
   struct iris_bo *bo;

   /* At the moment most applications only have a few named bo.
    * For instance, in a DRI client only the render buffers passed
    * between X and the client are named. And since X returns the
    * alternating names for the front/back buffer a linear search
    * provides a sufficiently fast match.
    */
   simple_mtx_lock(&bufmgr->lock);
   bo = find_and_ref_external_bo(bufmgr->name_table, handle);
   if (bo)
      goto out;

   struct drm_gem_open open_arg = { .name = handle };
   int ret = intel_ioctl(bufmgr->fd, DRM_IOCTL_GEM_OPEN, &open_arg);
   if (ret != 0) {
      DBG("Couldn't reference %s handle 0x%08x: %s\n",
          name, handle, strerror(errno));
      bo = NULL;
      goto out;
   }
   /* Now see if someone has used a prime handle to get this
    * object from the kernel before by looking through the list
    * again for a matching gem_handle
    */
   bo = find_and_ref_external_bo(bufmgr->handle_table, open_arg.handle);
   if (bo)
      goto out;

   bo = bo_calloc();
   if (!bo)
      goto out;

   p_atomic_set(&bo->refcount, 1);

   bo->size = open_arg.size;
   bo->bufmgr = bufmgr;
   bo->gem_handle = open_arg.handle;
   bo->name = name;
   bo->global_name = handle;
   bo->reusable = false;
   bo->imported = true;
   bo->mmap_mode = IRIS_MMAP_WC;
   bo->kflags = EXEC_OBJECT_SUPPORTS_48B_ADDRESS | EXEC_OBJECT_PINNED;
   bo->gtt_offset = vma_alloc(bufmgr, IRIS_MEMZONE_OTHER, bo->size, 1);

   _mesa_hash_table_insert(bufmgr->handle_table, &bo->gem_handle, bo);
   _mesa_hash_table_insert(bufmgr->name_table, &bo->global_name, bo);

   DBG("bo_create_from_handle: %d (%s)\n", handle, bo->name);

out:
   simple_mtx_unlock(&bufmgr->lock);
   return bo;
}

static void
bo_close(struct iris_bo *bo)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;

   if (iris_bo_is_external(bo)) {
      struct hash_entry *entry;

      if (bo->global_name) {
         entry = _mesa_hash_table_search(bufmgr->name_table, &bo->global_name);
         _mesa_hash_table_remove(bufmgr->name_table, entry);
      }

      entry = _mesa_hash_table_search(bufmgr->handle_table, &bo->gem_handle);
      _mesa_hash_table_remove(bufmgr->handle_table, entry);

      list_for_each_entry_safe(struct bo_export, export, &bo->exports, link) {
         struct drm_gem_close close = { .handle = export->gem_handle };
         intel_ioctl(export->drm_fd, DRM_IOCTL_GEM_CLOSE, &close);

         list_del(&export->link);
         free(export);
      }
   } else {
      assert(list_is_empty(&bo->exports));
   }

   /* Close this object */
   struct drm_gem_close close = { .handle = bo->gem_handle };
   int ret = intel_ioctl(bufmgr->fd, DRM_IOCTL_GEM_CLOSE, &close);
   if (ret != 0) {
      DBG("DRM_IOCTL_GEM_CLOSE %d failed (%s): %s\n",
          bo->gem_handle, bo->name, strerror(errno));
   }

   if (bo->aux_map_address && bo->bufmgr->aux_map_ctx) {
      intel_aux_map_unmap_range(bo->bufmgr->aux_map_ctx, bo->gtt_offset,
                                bo->size);
   }

   /* Return the VMA for reuse */
   vma_free(bo->bufmgr, bo->gtt_offset, bo->size);

   free(bo);
}

static void
bo_free(struct iris_bo *bo)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;

   if (!bo->userptr && bo->map)
      bo_unmap(bo);

   if (bo->idle) {
      bo_close(bo);
   } else {
      /* Defer closing the GEM BO and returning the VMA for reuse until the
       * BO is idle.  Just move it to the dead list for now.
       */
      list_addtail(&bo->head, &bufmgr->zombie_list);
   }
}

/** Frees all cached buffers significantly older than @time. */
static void
cleanup_bo_cache(struct iris_bufmgr *bufmgr, time_t time)
{
   int i;

   if (bufmgr->time == time)
      return;

   for (i = 0; i < bufmgr->num_buckets; i++) {
      struct bo_cache_bucket *bucket = &bufmgr->cache_bucket[i];

      list_for_each_entry_safe(struct iris_bo, bo, &bucket->head, head) {
         if (time - bo->free_time <= 1)
            break;

         list_del(&bo->head);

         bo_free(bo);
      }
   }

   for (i = 0; i < bufmgr->num_local_buckets; i++) {
      struct bo_cache_bucket *bucket = &bufmgr->local_cache_bucket[i];

      list_for_each_entry_safe(struct iris_bo, bo, &bucket->head, head) {
         if (time - bo->free_time <= 1)
            break;

         list_del(&bo->head);

         bo_free(bo);
      }
   }

   list_for_each_entry_safe(struct iris_bo, bo, &bufmgr->zombie_list, head) {
      /* Stop once we reach a busy BO - all others past this point were
       * freed more recently so are likely also busy.
       */
      if (!bo->idle && iris_bo_busy(bo))
         break;

      list_del(&bo->head);
      bo_close(bo);
   }

   bufmgr->time = time;
}

static void
bo_unreference_final(struct iris_bo *bo, time_t time)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;
   struct bo_cache_bucket *bucket;

   DBG("bo_unreference final: %d (%s)\n", bo->gem_handle, bo->name);

   bucket = NULL;
   if (bo->reusable)
      bucket = bucket_for_size(bufmgr, bo->size, bo->local);
   /* Put the buffer into our internal cache for reuse if we can. */
   if (bucket && iris_bo_madvise(bo, I915_MADV_DONTNEED)) {
      bo->free_time = time;
      bo->name = NULL;

      list_addtail(&bo->head, &bucket->head);
   } else {
      bo_free(bo);
   }
}

void
iris_bo_unreference(struct iris_bo *bo)
{
   if (bo == NULL)
      return;

   assert(p_atomic_read(&bo->refcount) > 0);

   if (atomic_add_unless(&bo->refcount, -1, 1)) {
      struct iris_bufmgr *bufmgr = bo->bufmgr;
      struct timespec time;

      clock_gettime(CLOCK_MONOTONIC, &time);

      simple_mtx_lock(&bufmgr->lock);

      if (p_atomic_dec_zero(&bo->refcount)) {
         bo_unreference_final(bo, time.tv_sec);
         cleanup_bo_cache(bufmgr, time.tv_sec);
      }

      simple_mtx_unlock(&bufmgr->lock);
   }
}

static void
bo_wait_with_stall_warning(struct pipe_debug_callback *dbg,
                           struct iris_bo *bo,
                           const char *action)
{
   bool busy = dbg && !bo->idle;
   double elapsed = unlikely(busy) ? -get_time() : 0.0;

   iris_bo_wait_rendering(bo);

   if (unlikely(busy)) {
      elapsed += get_time();
      if (elapsed > 1e-5) /* 0.01ms */ {
         perf_debug(dbg, "%s a busy \"%s\" BO stalled and took %.03f ms.\n",
                    action, bo->name, elapsed * 1000);
      }
   }
}

static void
print_flags(unsigned flags)
{
   if (flags & MAP_READ)
      DBG("READ ");
   if (flags & MAP_WRITE)
      DBG("WRITE ");
   if (flags & MAP_ASYNC)
      DBG("ASYNC ");
   if (flags & MAP_PERSISTENT)
      DBG("PERSISTENT ");
   if (flags & MAP_COHERENT)
      DBG("COHERENT ");
   if (flags & MAP_RAW)
      DBG("RAW ");
   DBG("\n");
}

static void *
iris_bo_gem_mmap_legacy(struct pipe_debug_callback *dbg, struct iris_bo *bo)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;

   struct drm_i915_gem_mmap mmap_arg = {
      .handle = bo->gem_handle,
      .size = bo->size,
      .flags = bo->mmap_mode == IRIS_MMAP_WC ? I915_MMAP_WC : 0,
   };

   int ret = intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_MMAP, &mmap_arg);
   if (ret != 0) {
      DBG("%s:%d: Error mapping buffer %d (%s): %s .\n",
          __FILE__, __LINE__, bo->gem_handle, bo->name, strerror(errno));
      return NULL;
   }
   void *map = (void *) (uintptr_t) mmap_arg.addr_ptr;

   return map;
}

static void *
iris_bo_gem_mmap_offset(struct pipe_debug_callback *dbg, struct iris_bo *bo)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;

   struct drm_i915_gem_mmap_offset mmap_arg = {
      .handle = bo->gem_handle,
   };

   if (bo->mmap_mode == IRIS_MMAP_WB)
      mmap_arg.flags = I915_MMAP_OFFSET_WB;
   else if (bo->mmap_mode == IRIS_MMAP_WC)
      mmap_arg.flags = I915_MMAP_OFFSET_WC;
   else
      mmap_arg.flags = I915_MMAP_OFFSET_UC;

   /* Get the fake offset back */
   int ret = intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_MMAP_OFFSET, &mmap_arg);
   if (ret != 0) {
      DBG("%s:%d: Error preparing buffer %d (%s): %s .\n",
          __FILE__, __LINE__, bo->gem_handle, bo->name, strerror(errno));
      return NULL;
   }

   /* And map it */
   void *map = mmap(0, bo->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                    bufmgr->fd, mmap_arg.offset);
   if (map == MAP_FAILED) {
      DBG("%s:%d: Error mapping buffer %d (%s): %s .\n",
          __FILE__, __LINE__, bo->gem_handle, bo->name, strerror(errno));
      return NULL;
   }

   return map;
}

void *
iris_bo_map(struct pipe_debug_callback *dbg,
            struct iris_bo *bo, unsigned flags)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;

   if (!bo->map) {
      DBG("iris_bo_map: %d (%s)\n", bo->gem_handle, bo->name);
      void *map = bufmgr->has_mmap_offset ? iris_bo_gem_mmap_offset(dbg, bo)
                                          : iris_bo_gem_mmap_legacy(dbg, bo);
      if (!map) {
         return NULL;
      }

      VG_DEFINED(map, bo->size);

      if (p_atomic_cmpxchg(&bo->map, NULL, map)) {
         VG_NOACCESS(map, bo->size);
         os_munmap(map, bo->size);
      }
   }
   assert(bo->map);

   DBG("iris_bo_map: %d (%s) -> %p\n", bo->gem_handle, bo->name, bo->map);
   print_flags(flags);

   if (!(flags & MAP_ASYNC)) {
      bo_wait_with_stall_warning(dbg, bo, "memory mapping");
   }

   return bo->map;
}

/** Waits for all GPU rendering with the object to have completed. */
void
iris_bo_wait_rendering(struct iris_bo *bo)
{
   /* We require a kernel recent enough for WAIT_IOCTL support.
    * See intel_init_bufmgr()
    */
   iris_bo_wait(bo, -1);
}

/**
 * Waits on a BO for the given amount of time.
 *
 * @bo: buffer object to wait for
 * @timeout_ns: amount of time to wait in nanoseconds.
 *   If value is less than 0, an infinite wait will occur.
 *
 * Returns 0 if the wait was successful ie. the last batch referencing the
 * object has completed within the allotted time. Otherwise some negative return
 * value describes the error. Of particular interest is -ETIME when the wait has
 * failed to yield the desired result.
 *
 * Similar to iris_bo_wait_rendering except a timeout parameter allows
 * the operation to give up after a certain amount of time. Another subtle
 * difference is the internal locking semantics are different (this variant does
 * not hold the lock for the duration of the wait). This makes the wait subject
 * to a larger userspace race window.
 *
 * The implementation shall wait until the object is no longer actively
 * referenced within a batch buffer at the time of the call. The wait will
 * not guarantee that the buffer is re-issued via another thread, or an flinked
 * handle. Userspace must make sure this race does not occur if such precision
 * is important.
 *
 * Note that some kernels have broken the infinite wait for negative values
 * promise, upgrade to latest stable kernels if this is the case.
 */
int
iris_bo_wait(struct iris_bo *bo, int64_t timeout_ns)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;

   /* If we know it's idle, don't bother with the kernel round trip */
   if (bo->idle && !iris_bo_is_external(bo))
      return 0;

   struct drm_i915_gem_wait wait = {
      .bo_handle = bo->gem_handle,
      .timeout_ns = timeout_ns,
   };
   int ret = intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_WAIT, &wait);
   if (ret != 0)
      return -errno;

   bo->idle = true;

   return ret;
}

static void
iris_bufmgr_destroy(struct iris_bufmgr *bufmgr)
{
   /* Free aux-map buffers */
   intel_aux_map_finish(bufmgr->aux_map_ctx);

   /* bufmgr will no longer try to free VMA entries in the aux-map */
   bufmgr->aux_map_ctx = NULL;

   simple_mtx_destroy(&bufmgr->lock);

   /* Free any cached buffer objects we were going to reuse */
   for (int i = 0; i < bufmgr->num_buckets; i++) {
      struct bo_cache_bucket *bucket = &bufmgr->cache_bucket[i];

      list_for_each_entry_safe(struct iris_bo, bo, &bucket->head, head) {
         list_del(&bo->head);

         bo_free(bo);
      }
   }

   /* Close any buffer objects on the dead list. */
   list_for_each_entry_safe(struct iris_bo, bo, &bufmgr->zombie_list, head) {
      list_del(&bo->head);
      bo_close(bo);
   }

   _mesa_hash_table_destroy(bufmgr->name_table, NULL);
   _mesa_hash_table_destroy(bufmgr->handle_table, NULL);

   for (int z = 0; z < IRIS_MEMZONE_COUNT; z++) {
      if (z != IRIS_MEMZONE_BINDER)
         util_vma_heap_finish(&bufmgr->vma_allocator[z]);
   }

   close(bufmgr->fd);

   free(bufmgr);
}

int
iris_gem_get_tiling(struct iris_bo *bo, uint32_t *tiling)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;

   if (!bufmgr->has_tiling_uapi) {
      *tiling = I915_TILING_NONE;
      return 0;
   }

   struct drm_i915_gem_get_tiling ti = { .handle = bo->gem_handle };
   int ret = intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_GET_TILING, &ti);

   if (ret) {
      DBG("gem_get_tiling failed for BO %u: %s\n",
          bo->gem_handle, strerror(errno));
   }

   *tiling = ti.tiling_mode;

   return ret;
}

int
iris_gem_set_tiling(struct iris_bo *bo, const struct isl_surf *surf)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;
   uint32_t tiling_mode = isl_tiling_to_i915_tiling(surf->tiling);
   int ret;

   /* If we can't do map_gtt, the set/get_tiling API isn't useful. And it's
    * actually not supported by the kernel in those cases.
    */
   if (!bufmgr->has_tiling_uapi)
      return 0;

   /* GEM_SET_TILING is slightly broken and overwrites the input on the
    * error path, so we have to open code intel_ioctl().
    */
   do {
      struct drm_i915_gem_set_tiling set_tiling = {
         .handle = bo->gem_handle,
         .tiling_mode = tiling_mode,
         .stride = surf->row_pitch_B,
      };
      ret = ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_SET_TILING, &set_tiling);
   } while (ret == -1 && (errno == EINTR || errno == EAGAIN));

   if (ret) {
      DBG("gem_set_tiling failed for BO %u: %s\n",
          bo->gem_handle, strerror(errno));
   }

   return ret;
}

struct iris_bo *
iris_bo_import_dmabuf(struct iris_bufmgr *bufmgr, int prime_fd)
{
   uint32_t handle;
   struct iris_bo *bo;

   simple_mtx_lock(&bufmgr->lock);
   int ret = drmPrimeFDToHandle(bufmgr->fd, prime_fd, &handle);
   if (ret) {
      DBG("import_dmabuf: failed to obtain handle from fd: %s\n",
          strerror(errno));
      simple_mtx_unlock(&bufmgr->lock);
      return NULL;
   }

   /*
    * See if the kernel has already returned this buffer to us. Just as
    * for named buffers, we must not create two bo's pointing at the same
    * kernel object
    */
   bo = find_and_ref_external_bo(bufmgr->handle_table, handle);
   if (bo)
      goto out;

   bo = bo_calloc();
   if (!bo)
      goto out;

   p_atomic_set(&bo->refcount, 1);

   /* Determine size of bo.  The fd-to-handle ioctl really should
    * return the size, but it doesn't.  If we have kernel 3.12 or
    * later, we can lseek on the prime fd to get the size.  Older
    * kernels will just fail, in which case we fall back to the
    * provided (estimated or guess size). */
   ret = lseek(prime_fd, 0, SEEK_END);
   if (ret != -1)
      bo->size = ret;

   bo->bufmgr = bufmgr;
   bo->name = "prime";
   bo->reusable = false;
   bo->imported = true;
   bo->mmap_mode = IRIS_MMAP_WC;
   bo->kflags = EXEC_OBJECT_SUPPORTS_48B_ADDRESS | EXEC_OBJECT_PINNED;

   /* From the Bspec, Memory Compression - Gfx12:
    *
    *    The base address for the surface has to be 64K page aligned and the
    *    surface is expected to be padded in the virtual domain to be 4 4K
    *    pages.
    *
    * The dmabuf may contain a compressed surface. Align the BO to 64KB just
    * in case. We always align to 64KB even on platforms where we don't need
    * to, because it's a fairly reasonable thing to do anyway.
    */
   bo->gtt_offset =
      vma_alloc(bufmgr, IRIS_MEMZONE_OTHER, bo->size, 64 * 1024);

   bo->gem_handle = handle;
   _mesa_hash_table_insert(bufmgr->handle_table, &bo->gem_handle, bo);

out:
   simple_mtx_unlock(&bufmgr->lock);
   return bo;
}

static void
iris_bo_mark_exported_locked(struct iris_bo *bo)
{
   if (!iris_bo_is_external(bo))
      _mesa_hash_table_insert(bo->bufmgr->handle_table, &bo->gem_handle, bo);

   if (!bo->exported) {
      /* If a BO is going to be used externally, it could be sent to the
       * display HW. So make sure our CPU mappings don't assume cache
       * coherency since display is outside that cache.
       */
      bo->exported = true;
      bo->reusable = false;
   }
}

void
iris_bo_mark_exported(struct iris_bo *bo)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;

   if (bo->exported) {
      assert(!bo->reusable);
      return;
   }

   simple_mtx_lock(&bufmgr->lock);
   iris_bo_mark_exported_locked(bo);
   simple_mtx_unlock(&bufmgr->lock);
}

int
iris_bo_export_dmabuf(struct iris_bo *bo, int *prime_fd)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;

   iris_bo_mark_exported(bo);

   if (drmPrimeHandleToFD(bufmgr->fd, bo->gem_handle,
                          DRM_CLOEXEC | DRM_RDWR, prime_fd) != 0)
      return -errno;

   return 0;
}

uint32_t
iris_bo_export_gem_handle(struct iris_bo *bo)
{
   iris_bo_mark_exported(bo);

   return bo->gem_handle;
}

int
iris_bo_flink(struct iris_bo *bo, uint32_t *name)
{
   struct iris_bufmgr *bufmgr = bo->bufmgr;

   if (!bo->global_name) {
      struct drm_gem_flink flink = { .handle = bo->gem_handle };

      if (intel_ioctl(bufmgr->fd, DRM_IOCTL_GEM_FLINK, &flink))
         return -errno;

      simple_mtx_lock(&bufmgr->lock);
      if (!bo->global_name) {
         iris_bo_mark_exported_locked(bo);
         bo->global_name = flink.name;
         _mesa_hash_table_insert(bufmgr->name_table, &bo->global_name, bo);
      }
      simple_mtx_unlock(&bufmgr->lock);
   }

   *name = bo->global_name;
   return 0;
}

int
iris_bo_export_gem_handle_for_device(struct iris_bo *bo, int drm_fd,
                                     uint32_t *out_handle)
{
   /* Only add the new GEM handle to the list of export if it belongs to a
    * different GEM device. Otherwise we might close the same buffer multiple
    * times.
    */
   struct iris_bufmgr *bufmgr = bo->bufmgr;
   int ret = os_same_file_description(drm_fd, bufmgr->fd);
   WARN_ONCE(ret < 0,
             "Kernel has no file descriptor comparison support: %s\n",
             strerror(errno));
   if (ret == 0) {
      *out_handle = iris_bo_export_gem_handle(bo);
      return 0;
   }

   struct bo_export *export = calloc(1, sizeof(*export));
   if (!export)
      return -ENOMEM;

   export->drm_fd = drm_fd;

   int dmabuf_fd = -1;
   int err = iris_bo_export_dmabuf(bo, &dmabuf_fd);
   if (err) {
      free(export);
      return err;
   }

   simple_mtx_lock(&bufmgr->lock);
   err = drmPrimeFDToHandle(drm_fd, dmabuf_fd, &export->gem_handle);
   close(dmabuf_fd);
   if (err) {
      simple_mtx_unlock(&bufmgr->lock);
      free(export);
      return err;
   }

   bool found = false;
   list_for_each_entry(struct bo_export, iter, &bo->exports, link) {
      if (iter->drm_fd != drm_fd)
         continue;
      /* Here we assume that for a given DRM fd, we'll always get back the
       * same GEM handle for a given buffer.
       */
      assert(iter->gem_handle == export->gem_handle);
      free(export);
      export = iter;
      found = true;
      break;
   }
   if (!found)
      list_addtail(&export->link, &bo->exports);

   simple_mtx_unlock(&bufmgr->lock);

   *out_handle = export->gem_handle;

   return 0;
}

static void
add_bucket(struct iris_bufmgr *bufmgr, int size, bool local)
{
   unsigned int i = local ?
      bufmgr->num_local_buckets : bufmgr->num_buckets;

   struct bo_cache_bucket *buckets = local ?
      bufmgr->local_cache_bucket : bufmgr->cache_bucket;

   assert(i < ARRAY_SIZE(bufmgr->cache_bucket));

   list_inithead(&buckets[i].head);
   buckets[i].size = size;

   if (local)
      bufmgr->num_local_buckets++;
   else
      bufmgr->num_buckets++;

   assert(bucket_for_size(bufmgr, size, local) == &buckets[i]);
   assert(bucket_for_size(bufmgr, size - 2048, local) == &buckets[i]);
   assert(bucket_for_size(bufmgr, size + 1, local) != &buckets[i]);
}

static void
init_cache_buckets(struct iris_bufmgr *bufmgr, bool local)
{
   uint64_t size, cache_max_size = 64 * 1024 * 1024;

   /* OK, so power of two buckets was too wasteful of memory.
    * Give 3 other sizes between each power of two, to hopefully
    * cover things accurately enough.  (The alternative is
    * probably to just go for exact matching of sizes, and assume
    * that for things like composited window resize the tiled
    * width/height alignment and rounding of sizes to pages will
    * get us useful cache hit rates anyway)
    */
   add_bucket(bufmgr, PAGE_SIZE, local);
   add_bucket(bufmgr, PAGE_SIZE * 2, local);
   add_bucket(bufmgr, PAGE_SIZE * 3, local);

   /* Initialize the linked lists for BO reuse cache. */
   for (size = 4 * PAGE_SIZE; size <= cache_max_size; size *= 2) {
      add_bucket(bufmgr, size, local);

      add_bucket(bufmgr, size + size * 1 / 4, local);
      add_bucket(bufmgr, size + size * 2 / 4, local);
      add_bucket(bufmgr, size + size * 3 / 4, local);
   }
}

uint32_t
iris_create_hw_context(struct iris_bufmgr *bufmgr)
{
   struct drm_i915_gem_context_create create = { };
   int ret = intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_CONTEXT_CREATE, &create);
   if (ret != 0) {
      DBG("DRM_IOCTL_I915_GEM_CONTEXT_CREATE failed: %s\n", strerror(errno));
      return 0;
   }

   /* Upon declaring a GPU hang, the kernel will zap the guilty context
    * back to the default logical HW state and attempt to continue on to
    * our next submitted batchbuffer.  However, our render batches assume
    * the previous GPU state is preserved, and only emit commands needed
    * to incrementally change that state.  In particular, we inherit the
    * STATE_BASE_ADDRESS and PIPELINE_SELECT settings, which are critical.
    * With default base addresses, our next batches will almost certainly
    * cause more GPU hangs, leading to repeated hangs until we're banned
    * or the machine is dead.
    *
    * Here we tell the kernel not to attempt to recover our context but
    * immediately (on the next batchbuffer submission) report that the
    * context is lost, and we will do the recovery ourselves.  Ideally,
    * we'll have two lost batches instead of a continual stream of hangs.
    */
   struct drm_i915_gem_context_param p = {
      .ctx_id = create.ctx_id,
      .param = I915_CONTEXT_PARAM_RECOVERABLE,
      .value = false,
   };
   intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &p);

   return create.ctx_id;
}

static int
iris_hw_context_get_priority(struct iris_bufmgr *bufmgr, uint32_t ctx_id)
{
   struct drm_i915_gem_context_param p = {
      .ctx_id = ctx_id,
      .param = I915_CONTEXT_PARAM_PRIORITY,
   };
   intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_CONTEXT_GETPARAM, &p);
   return p.value; /* on error, return 0 i.e. default priority */
}

int
iris_hw_context_set_priority(struct iris_bufmgr *bufmgr,
                            uint32_t ctx_id,
                            int priority)
{
   struct drm_i915_gem_context_param p = {
      .ctx_id = ctx_id,
      .param = I915_CONTEXT_PARAM_PRIORITY,
      .value = priority,
   };
   int err;

   err = 0;
   if (intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_CONTEXT_SETPARAM, &p))
      err = -errno;

   return err;
}

uint32_t
iris_clone_hw_context(struct iris_bufmgr *bufmgr, uint32_t ctx_id)
{
   uint32_t new_ctx = iris_create_hw_context(bufmgr);

   if (new_ctx) {
      int priority = iris_hw_context_get_priority(bufmgr, ctx_id);
      iris_hw_context_set_priority(bufmgr, new_ctx, priority);
   }

   return new_ctx;
}

void
iris_destroy_hw_context(struct iris_bufmgr *bufmgr, uint32_t ctx_id)
{
   struct drm_i915_gem_context_destroy d = { .ctx_id = ctx_id };

   if (ctx_id != 0 &&
       intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_GEM_CONTEXT_DESTROY, &d) != 0) {
      fprintf(stderr, "DRM_IOCTL_I915_GEM_CONTEXT_DESTROY failed: %s\n",
              strerror(errno));
   }
}

int
iris_reg_read(struct iris_bufmgr *bufmgr, uint32_t offset, uint64_t *result)
{
   struct drm_i915_reg_read reg_read = { .offset = offset };
   int ret = intel_ioctl(bufmgr->fd, DRM_IOCTL_I915_REG_READ, &reg_read);

   *result = reg_read.val;
   return ret;
}

static uint64_t
iris_gtt_size(int fd)
{
   /* We use the default (already allocated) context to determine
    * the default configuration of the virtual address space.
    */
   struct drm_i915_gem_context_param p = {
      .param = I915_CONTEXT_PARAM_GTT_SIZE,
   };
   if (!intel_ioctl(fd, DRM_IOCTL_I915_GEM_CONTEXT_GETPARAM, &p))
      return p.value;

   return 0;
}

static struct intel_buffer *
intel_aux_map_buffer_alloc(void *driver_ctx, uint32_t size)
{
   struct intel_buffer *buf = malloc(sizeof(struct intel_buffer));
   if (!buf)
      return NULL;

   struct iris_bufmgr *bufmgr = (struct iris_bufmgr *)driver_ctx;

   struct iris_bo *bo =
      iris_bo_alloc(bufmgr, "aux-map", size, 64 * 1024,
                    IRIS_MEMZONE_OTHER, 0);

   buf->driver_bo = bo;
   buf->gpu = bo->gtt_offset;
   buf->gpu_end = buf->gpu + bo->size;
   buf->map = iris_bo_map(NULL, bo, MAP_WRITE | MAP_RAW);
   return buf;
}

static void
intel_aux_map_buffer_free(void *driver_ctx, struct intel_buffer *buffer)
{
   iris_bo_unreference((struct iris_bo*)buffer->driver_bo);
   free(buffer);
}

static struct intel_mapped_pinned_buffer_alloc aux_map_allocator = {
   .alloc = intel_aux_map_buffer_alloc,
   .free = intel_aux_map_buffer_free,
};

static int
gem_param(int fd, int name)
{
   int v = -1; /* No param uses (yet) the sign bit, reserve it for errors */

   struct drm_i915_getparam gp = { .param = name, .value = &v };
   if (intel_ioctl(fd, DRM_IOCTL_I915_GETPARAM, &gp))
      return -1;

   return v;
}

static bool
iris_bufmgr_query_meminfo(struct iris_bufmgr *bufmgr)
{
   struct drm_i915_query_memory_regions *meminfo =
      intel_i915_query_alloc(bufmgr->fd, DRM_I915_QUERY_MEMORY_REGIONS);
   if (meminfo == NULL)
      return false;

   for (int i = 0; i < meminfo->num_regions; i++) {
      const struct drm_i915_memory_region_info *mem = &meminfo->regions[i];
      switch (mem->region.memory_class) {
      case I915_MEMORY_CLASS_SYSTEM:
         bufmgr->sys.region = mem->region;
         bufmgr->sys.size = mem->probed_size;
         break;
      case I915_MEMORY_CLASS_DEVICE:
         bufmgr->vram.region = mem->region;
         bufmgr->vram.size = mem->probed_size;
         break;
      default:
         break;
      }
   }

   free(meminfo);

   return true;
}

/**
 * Initializes the GEM buffer manager, which uses the kernel to allocate, map,
 * and manage map buffer objections.
 *
 * \param fd File descriptor of the opened DRM device.
 */
static struct iris_bufmgr *
iris_bufmgr_create(struct intel_device_info *devinfo, int fd, bool bo_reuse)
{
   uint64_t gtt_size = iris_gtt_size(fd);
   if (gtt_size <= IRIS_MEMZONE_OTHER_START)
      return NULL;

   struct iris_bufmgr *bufmgr = calloc(1, sizeof(*bufmgr));
   if (bufmgr == NULL)
      return NULL;

   /* Handles to buffer objects belong to the device fd and are not
    * reference counted by the kernel.  If the same fd is used by
    * multiple parties (threads sharing the same screen bufmgr, or
    * even worse the same device fd passed to multiple libraries)
    * ownership of those handles is shared by those independent parties.
    *
    * Don't do this! Ensure that each library/bufmgr has its own device
    * fd so that its namespace does not clash with another.
    */
   bufmgr->fd = os_dupfd_cloexec(fd);

   p_atomic_set(&bufmgr->refcount, 1);

   simple_mtx_init(&bufmgr->lock, mtx_plain);

   list_inithead(&bufmgr->zombie_list);

   bufmgr->has_llc = devinfo->has_llc;
   bufmgr->has_tiling_uapi = devinfo->has_tiling_uapi;
   bufmgr->bo_reuse = bo_reuse;
#ifdef ANDROID
   /* Android kernel still has some issues to support
    * mmap_offset on 32 bit, hardcode to legacy mode
    */
   bufmgr->has_mmap_offset = false;
#else
   bufmgr->has_mmap_offset = gem_param(fd, I915_PARAM_MMAP_GTT_VERSION) >= 4;
#endif
   iris_bufmgr_query_meminfo(bufmgr);

   STATIC_ASSERT(IRIS_MEMZONE_SHADER_START == 0ull);
   const uint64_t _4GB = 1ull << 32;
   const uint64_t _2GB = 1ul << 31;

   /* The STATE_BASE_ADDRESS size field can only hold 1 page shy of 4GB */
   const uint64_t _4GB_minus_1 = _4GB - PAGE_SIZE;

   util_vma_heap_init(&bufmgr->vma_allocator[IRIS_MEMZONE_SHADER],
                      PAGE_SIZE, _4GB_minus_1 - PAGE_SIZE);
   util_vma_heap_init(&bufmgr->vma_allocator[IRIS_MEMZONE_BINDLESS],
                      IRIS_MEMZONE_BINDLESS_START, IRIS_BINDLESS_SIZE);
   util_vma_heap_init(&bufmgr->vma_allocator[IRIS_MEMZONE_SURFACE],
                      IRIS_MEMZONE_SURFACE_START,
                      _4GB_minus_1 - IRIS_MAX_BINDERS * IRIS_BINDER_SIZE -
                     IRIS_BINDLESS_SIZE);
   /* TODO: Why does limiting to 2GB help some state items on gfx12?
    *  - CC Viewport Pointer
    *  - Blend State Pointer
    *  - Color Calc State Pointer
    */
   const uint64_t dynamic_pool_size =
      (devinfo->ver >= 12 ? _2GB : _4GB_minus_1) - IRIS_BORDER_COLOR_POOL_SIZE;
   util_vma_heap_init(&bufmgr->vma_allocator[IRIS_MEMZONE_DYNAMIC],
                      IRIS_MEMZONE_DYNAMIC_START + IRIS_BORDER_COLOR_POOL_SIZE,
                      dynamic_pool_size);

   /* Leave the last 4GB out of the high vma range, so that no state
    * base address + size can overflow 48 bits.
    */
   util_vma_heap_init(&bufmgr->vma_allocator[IRIS_MEMZONE_OTHER],
                      IRIS_MEMZONE_OTHER_START,
                      (gtt_size - _4GB) - IRIS_MEMZONE_OTHER_START);

   init_cache_buckets(bufmgr, false);
   init_cache_buckets(bufmgr, true);

   bufmgr->name_table =
      _mesa_hash_table_create(NULL, _mesa_hash_uint, _mesa_key_uint_equal);
   bufmgr->handle_table =
      _mesa_hash_table_create(NULL, _mesa_hash_uint, _mesa_key_uint_equal);

   bufmgr->vma_min_align = devinfo->has_local_mem ? 64 * 1024 : PAGE_SIZE;

   if (devinfo->has_aux_map) {
      bufmgr->aux_map_ctx = intel_aux_map_init(bufmgr, &aux_map_allocator,
                                               devinfo);
      assert(bufmgr->aux_map_ctx);
   }

   return bufmgr;
}

static struct iris_bufmgr *
iris_bufmgr_ref(struct iris_bufmgr *bufmgr)
{
   p_atomic_inc(&bufmgr->refcount);
   return bufmgr;
}

void
iris_bufmgr_unref(struct iris_bufmgr *bufmgr)
{
   simple_mtx_lock(&global_bufmgr_list_mutex);
   if (p_atomic_dec_zero(&bufmgr->refcount)) {
      list_del(&bufmgr->link);
      iris_bufmgr_destroy(bufmgr);
   }
   simple_mtx_unlock(&global_bufmgr_list_mutex);
}

/**
 * Gets an already existing GEM buffer manager or create a new one.
 *
 * \param fd File descriptor of the opened DRM device.
 */
struct iris_bufmgr *
iris_bufmgr_get_for_fd(struct intel_device_info *devinfo, int fd, bool bo_reuse)
{
   struct stat st;

   if (fstat(fd, &st))
      return NULL;

   struct iris_bufmgr *bufmgr = NULL;

   simple_mtx_lock(&global_bufmgr_list_mutex);
   list_for_each_entry(struct iris_bufmgr, iter_bufmgr, &global_bufmgr_list, link) {
      struct stat iter_st;
      if (fstat(iter_bufmgr->fd, &iter_st))
         continue;

      if (st.st_rdev == iter_st.st_rdev) {
         assert(iter_bufmgr->bo_reuse == bo_reuse);
         bufmgr = iris_bufmgr_ref(iter_bufmgr);
         goto unlock;
      }
   }

   bufmgr = iris_bufmgr_create(devinfo, fd, bo_reuse);
   if (bufmgr)
      list_addtail(&bufmgr->link, &global_bufmgr_list);

 unlock:
   simple_mtx_unlock(&global_bufmgr_list_mutex);

   return bufmgr;
}

int
iris_bufmgr_get_fd(struct iris_bufmgr *bufmgr)
{
   return bufmgr->fd;
}

void*
iris_bufmgr_get_aux_map_context(struct iris_bufmgr *bufmgr)
{
   return bufmgr->aux_map_ctx;
}
