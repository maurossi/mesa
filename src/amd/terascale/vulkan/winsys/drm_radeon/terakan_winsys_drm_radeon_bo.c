/*
 * Copyright © 2023 Vitaliy Triang3l Kuzmin
 *
 * Based on Gallium Radeon DRM winsys which is:
 * Copyright © 2008 Jérôme Glisse
 * Copyright © 2009 Corbin Simpson <MostAwesomeDude@gmail.com>
 * Copyright © 2011 Marek Olšák <maraeo@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "terakan_image.h"
#include "terakan_winsys_drm_radeon.h"

#include "util/macros.h"
#include "util/os_mman.h"
#include "util/u_memory.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <xf86drm.h>
#include <radeon_drm.h>

static bool
terakan_winsys_drm_radeon_bo_set_tiling_for_surface(struct terakan_winsys_bo * const bo_base,
                                                    struct radeon_surf const * const surface)
{
   struct terakan_winsys_drm_radeon_bo const * const bo =
      container_of(bo_base, struct terakan_winsys_drm_radeon_bo, base);
   struct terakan_winsys_drm_radeon const * const winsys =
      container_of(bo->base.winsys, struct terakan_winsys_drm_radeon const, base);

   struct drm_radeon_gem_set_tiling set_tiling_arguments = {
      .handle = bo->handle,
      .tiling_flags =
         (surface->u.legacy.level[0].mode >= RADEON_SURF_MODE_2D ? RADEON_TILING_MACRO : 0) |
         (surface->u.legacy.level[0].mode >= RADEON_SURF_MODE_1D ? RADEON_TILING_MICRO : 0) |
         (__u32)surface->u.legacy.bankw << RADEON_TILING_EG_BANKW_SHIFT |
         (__u32)surface->u.legacy.bankh << RADEON_TILING_EG_BANKH_SHIFT |
         (__u32)surface->u.legacy.mtilea << RADEON_TILING_EG_MACRO_TILE_ASPECT_SHIFT |
         terakan_image_tile_split_bytes_to_hw(surface->u.legacy.tile_split)
            << RADEON_TILING_EG_TILE_SPLIT_SHIFT |
         (surface->has_stencil
             ? terakan_image_tile_split_bytes_to_hw(surface->u.legacy.stencil_tile_split)
                  << RADEON_TILING_EG_STENCIL_TILE_SPLIT_SHIFT
             : 0),
      .pitch = surface->bpe * (__u32)surface->u.legacy.level[0].nblk_x,
   };
   return drmCommandWriteRead(winsys->fd, DRM_RADEON_GEM_SET_TILING, &set_tiling_arguments,
                              sizeof(set_tiling_arguments)) == 0;
}

static void *
terakan_winsys_drm_radeon_bo_map_impl(struct terakan_winsys_bo * const bo_base)
{
   struct terakan_winsys_drm_radeon_bo * const bo =
      container_of(bo_base, struct terakan_winsys_drm_radeon_bo, base);
   struct terakan_winsys_drm_radeon const * const winsys =
      container_of(bo->base.winsys, struct terakan_winsys_drm_radeon const, base);

   /* The offset and size fields are ignored by the Radeon kernel driver as of Radeon 2.50.0, but
    * for correctness, still passing the size.
    */
   struct drm_radeon_gem_mmap gem_mmap_arguments = {
      .handle = bo->handle,
      .size = bo->size,
   };
   int const gem_mmap_result = drmCommandWriteRead(winsys->fd, DRM_RADEON_GEM_MMAP,
                                                   &gem_mmap_arguments, sizeof(gem_mmap_arguments));
   if (gem_mmap_result != 0) {
      fprintf(stderr,
              "terakan/drm_radeon: Failed to map the buffer 0x%" PRIX32
              " in GEM, error number %d.\n",
              (uint32_t)bo->handle, gem_mmap_result);
      return NULL;
   }

   void * const mapping = os_mmap(NULL, (size_t)bo->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                                  winsys->fd, (off_t)gem_mmap_arguments.addr_ptr);
   if (mapping == MAP_FAILED) {
      fprintf(stderr,
              "terakan/drm_radeon: Failed to map the buffer 0x%" PRIX32
              " in the OS, error number %d.\n",
              (uint32_t)bo->handle, errno);
      return NULL;
   }

   return mapping;
}

static void
terakan_winsys_drm_radeon_bo_unmap_impl(struct terakan_winsys_bo * const bo_base)
{
   struct terakan_winsys_drm_radeon_bo * const bo =
      container_of(bo_base, struct terakan_winsys_drm_radeon_bo, base);

   os_munmap(bo_base->mapping, (size_t)bo->size);
}

static bool
terakan_winsys_drm_radeon_bo_wait_idle(struct terakan_winsys_bo * const bo_base)
{
   struct terakan_winsys_drm_radeon_bo const * const bo =
      container_of(bo_base, struct terakan_winsys_drm_radeon_bo const, base);
   struct terakan_winsys_drm_radeon const * const winsys =
      container_of(bo->base.winsys, struct terakan_winsys_drm_radeon const, base);

   struct drm_radeon_gem_wait_idle gem_wait_idle_arguments = {
      .handle = bo->handle,
   };
   /* Returns -EBUSY in finite time in case of a hang (30-second timeout in Linux Radeon 2.50.0). */
   return drmCommandWrite(winsys->fd, DRM_RADEON_GEM_WAIT_IDLE, &gem_wait_idle_arguments,
                          sizeof(gem_wait_idle_arguments)) == 0;
}

static int
terakan_winsys_drm_radeon_bo_export_fd(struct terakan_winsys_bo * const bo_base,
                                       bool const writable)
{
   struct terakan_winsys_drm_radeon_bo * const bo =
      container_of(bo_base, struct terakan_winsys_drm_radeon_bo, base);

   if (!bo->handle_shareable) {
      return -1;
   }

   struct terakan_winsys_drm_radeon const * const winsys =
      container_of(bo->base.winsys, struct terakan_winsys_drm_radeon const, base);

   int fd;
   if (drmPrimeHandleToFD(winsys->fd, bo->handle, DRM_CLOEXEC | (writable ? DRM_RDWR : 0), &fd) !=
       0) {
      return -1;
   }
   return fd;
}

static void
terakan_winsys_drm_radeon_bo_free_impl(struct terakan_winsys_bo * const bo_base)
{
   struct terakan_winsys_drm_radeon_bo * const bo =
      container_of(bo_base, struct terakan_winsys_drm_radeon_bo, base);
   struct terakan_winsys_drm_radeon * const winsys =
      container_of(bo->base.winsys, struct terakan_winsys_drm_radeon, base);

   bool close_bo;
   if (bo->handle_shareable) {
      /* Close the handle atomically with reference count management to make sure the handle isn't
       * closed while another thread is importing the same GEM BO or destroying another winsys BO
       * for this GEM BO.
       */
      mtx_lock(&winsys->shared_bo_mutex);
      struct hash_entry * const reference_count_entry =
         _mesa_hash_table_search(winsys->shared_bo_reference_counts, (void *)(uintptr_t)bo->handle);
      assert(reference_count_entry != NULL && "All shareable BOs must be reference-counted");
      uintptr_t const old_reference_count = (uintptr_t)reference_count_entry->data;
      assert(old_reference_count > 0);
      if (old_reference_count > 1) {
         reference_count_entry->data = (void *)(old_reference_count - 1);
         close_bo = false;
      } else {
         _mesa_hash_table_remove(winsys->shared_bo_reference_counts, reference_count_entry);
         close_bo = true;
      }
   } else {
      close_bo = true;
   }

   if (close_bo) {
      struct drm_gem_close gem_close_arguments = {.handle = bo->handle};
      drmIoctl(winsys->fd, DRM_IOCTL_GEM_CLOSE, &gem_close_arguments);
   }

   if (bo->handle_shareable) {
      mtx_unlock(&winsys->shared_bo_mutex);
   }

   FREE(bo);
}

static struct terakan_winsys_bo *
terakan_winsys_drm_radeon_bo_allocate_device_memory(
   struct terakan_winsys * const winsys_base, VkDeviceSize const size, VkDeviceSize const alignment,
   VkMemoryPropertyFlags const flags, VkExternalMemoryHandleTypeFlags const export_handle_types)
{
   struct terakan_winsys_drm_radeon * const winsys =
      container_of(winsys_base, struct terakan_winsys_drm_radeon, base);

   struct terakan_winsys_drm_radeon_bo * const bo = MALLOC_STRUCT(terakan_winsys_drm_radeon_bo);
   if (bo == NULL) {
      fputs("terakan/drm_radeon: Failed to allocate memory for the winsys buffer structure.\n",
            stderr);
      return NULL;
   }

   struct drm_radeon_gem_create gem_create_arguments = {};

   gem_create_arguments.size = size;
   gem_create_arguments.alignment = alignment;

   __u32 initial_domains = 0;
   if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
      initial_domains |= RADEON_GEM_DOMAIN_VRAM;
      /* If VRAM is just stolen system memory, allow both VRAM and GTT, whichever has free space.
       * If a buffer is evicted from VRAM to GTT, it will stay there.
       */
      if (!winsys->base.gpu_info.has_dedicated_vram) {
         initial_domains |= RADEON_GEM_DOMAIN_GTT;
      }
   } else {
      initial_domains |= RADEON_GEM_DOMAIN_GTT;
   }
   gem_create_arguments.initial_domain = initial_domains;

   if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
      gem_create_arguments.flags |= RADEON_GEM_CPU_ACCESS;
      if (!(flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)) {
         gem_create_arguments.flags |= RADEON_GEM_GTT_WC;
      }
   } else {
      gem_create_arguments.flags |= RADEON_GEM_NO_CPU_ACCESS;
   }

   int const gem_create_result = drmCommandWriteRead(
      winsys->fd, DRM_RADEON_GEM_CREATE, &gem_create_arguments, sizeof(gem_create_arguments));
   if (gem_create_result != 0) {
      fprintf(stderr, "terakan/drm_radeon: Failed to allocate a buffer, error number %d:\n",
              gem_create_result);
      fprintf(stderr, "terakan/drm_radeon:    Size: %" PRIu64 " bytes\n", size);
      fprintf(stderr, "terakan/drm_radeon:    Alignment: %" PRIu64 " bytes\n", alignment);
      fprintf(stderr, "terakan/drm_radeon:    Domains: 0x%" PRIX32 "\n", (uint32_t)initial_domains);
      fprintf(stderr, "terakan/drm_radeon:    Flags: 0x%" PRIX32 "\n",
              (uint32_t)gem_create_arguments.flags);
      FREE(bo);
      return NULL;
   }

   terakan_winsys_bo_base_init(&bo->base, &winsys->base);

   bo->size = size;

   bo->domains = initial_domains;

   bo->handle = gem_create_arguments.handle;

   if (export_handle_types & (VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT |
                              VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT)) {
      bo->handle_shareable = true;

      mtx_lock(&winsys->shared_bo_mutex);
      bool const shared_handle_reference_count_inserted =
         _mesa_hash_table_insert(winsys->shared_bo_reference_counts,
                                 (void *)(uintptr_t)gem_create_arguments.handle, (void *)1) != NULL;
      mtx_unlock(&winsys->shared_bo_mutex);
      if (!shared_handle_reference_count_inserted) {
         fputs("terakan/drm_radeon: Failed to insert the buffer reference count into the table.\n",
               stderr);
         struct drm_gem_close gem_close_arguments = {.handle = gem_create_arguments.handle};
         drmIoctl(winsys->fd, DRM_IOCTL_GEM_CLOSE, &gem_close_arguments);
         FREE(bo);
         return NULL;
      }
   } else {
      bo->handle_shareable = false;
   }

   return &bo->base;
}

static bool
terakan_winsys_drm_radeon_bo_get_fd_vram_preference(struct terakan_winsys * const winsys_base,
                                                    int const fd, bool * const vram_preferred_out)
{
   struct terakan_winsys_drm_radeon * const winsys =
      container_of(winsys_base, struct terakan_winsys_drm_radeon, base);

   /* Get the handle atomically with reference count management to make sure the handle isn't closed
    * by another thread while it's being imported.
    */
   mtx_lock(&winsys->shared_bo_mutex);

   __u32 handle;
   if (drmPrimeFDToHandle(winsys->fd, fd, &handle) != 0) {
      return false;
   }

   struct drm_radeon_gem_op gem_op_arguments = {
      .handle = handle,
      .op = RADEON_GEM_OP_GET_INITIAL_DOMAIN,
   };
   int const gem_op_result = drmCommandWriteRead(winsys->fd, DRM_RADEON_GEM_OP, &gem_op_arguments,
                                                 sizeof(gem_op_arguments));

   /* Close the handle if there are no winsys BOs backed by the same GEM BO. */
   struct hash_entry * const reference_count_entry =
      _mesa_hash_table_search(winsys->shared_bo_reference_counts, (void *)(uintptr_t)handle);
   assert(reference_count_entry == NULL || (uintptr_t)reference_count_entry->data > 0);
   if (reference_count_entry == NULL) {
      struct drm_gem_close gem_close_arguments = {.handle = handle};
      drmIoctl(winsys->fd, DRM_IOCTL_GEM_CLOSE, &gem_close_arguments);
   }

   mtx_unlock(&winsys->shared_bo_mutex);

   if (gem_op_result != 0) {
      return false;
   }

   *vram_preferred_out = (gem_op_arguments.value & RADEON_GEM_DOMAIN_VRAM) != 0;

   return true;
}

static struct terakan_winsys_bo *
terakan_winsys_drm_radeon_bo_import_fd(struct terakan_winsys * const winsys_base, int const fd,
                                       VkDeviceSize const size, bool const prefer_vram)
{
   struct terakan_winsys_drm_radeon * const winsys =
      container_of(winsys_base, struct terakan_winsys_drm_radeon, base);

   struct terakan_winsys_drm_radeon_bo * const bo = MALLOC_STRUCT(terakan_winsys_drm_radeon_bo);
   if (bo == NULL) {
      fputs("terakan/drm_radeon: Failed to allocate memory for the winsys buffer structure.\n",
            stderr);
      return NULL;
   }

   /* Get the handle atomically with reference count management to make sure the handle isn't closed
    * by another thread while it's being imported.
    */
   mtx_lock(&winsys->shared_bo_mutex);

   __u32 handle;
   int const prime_fd_to_handle_result = drmPrimeFDToHandle(winsys->fd, fd, &handle);
   if (prime_fd_to_handle_result != 0) {
      mtx_unlock(&winsys->shared_bo_mutex);
      fprintf(
         stderr,
         "terakan/drm_radeon: Failed to import a file descriptor as a buffer, error number %d:\n",
         prime_fd_to_handle_result);
      fprintf(stderr, "terakan/drm_radeon:    Size: %" PRIu64 " bytes\n", size);
      fprintf(stderr, "terakan/drm_radeon:    Preferred domain: %s\n",
              prefer_vram ? "VRAM" : "GTT");
      FREE(bo);
      return false;
   }

   uint32_t reference_count_hash = _mesa_hash_pointer((void *)(uintptr_t)handle);
   struct hash_entry * const existing_reference_count_entry = _mesa_hash_table_search_pre_hashed(
      winsys->shared_bo_reference_counts, reference_count_hash, (void *)(uintptr_t)handle);
   if (existing_reference_count_entry != NULL) {
      assert((uintptr_t)existing_reference_count_entry->data > 0);
      existing_reference_count_entry->data =
         (void *)((uintptr_t)existing_reference_count_entry->data + 1);
   } else {
      if (_mesa_hash_table_insert_pre_hashed(winsys->shared_bo_reference_counts,
                                             reference_count_hash, (void *)(uintptr_t)handle,
                                             (void *)1) == NULL) {
         fputs("terakan/drm_radeon: Failed to insert the buffer reference count into the table.\n",
               stderr);
         /* Close the BO inside the critical section so it's not closed while other threads may be
          * trying to import the BO with the same handle.
          */
         struct drm_gem_close gem_close_arguments = {.handle = handle};
         drmIoctl(winsys->fd, DRM_IOCTL_GEM_CLOSE, &gem_close_arguments);
         mtx_unlock(&winsys->shared_bo_mutex);
         FREE(bo);
         return false;
      }
   }

   mtx_unlock(&winsys->shared_bo_mutex);

   terakan_winsys_bo_base_init(&bo->base, &winsys->base);

   bo->size = size;

   bo->domains = prefer_vram ? RADEON_GEM_DOMAIN_VRAM : RADEON_GEM_DOMAIN_GTT;

   bo->handle = handle;

   bo->handle_shareable = true;

   return &bo->base;
}

struct terakan_winsys_bo_fn const terakan_winsys_drm_radeon_bo_fn = {
   .set_tiling_for_surface = terakan_winsys_drm_radeon_bo_set_tiling_for_surface,
   .map_impl = terakan_winsys_drm_radeon_bo_map_impl,
   .unmap_impl = terakan_winsys_drm_radeon_bo_unmap_impl,
   .wait_idle = terakan_winsys_drm_radeon_bo_wait_idle,
   .free_impl = terakan_winsys_drm_radeon_bo_free_impl,
   .allocate_device_memory = terakan_winsys_drm_radeon_bo_allocate_device_memory,
   .export_fd = terakan_winsys_drm_radeon_bo_export_fd,
   .get_fd_vram_preference = terakan_winsys_drm_radeon_bo_get_fd_vram_preference,
   .import_fd = terakan_winsys_drm_radeon_bo_import_fd,
};
