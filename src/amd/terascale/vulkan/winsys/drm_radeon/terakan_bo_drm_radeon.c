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

#include "terakan_bo_drm_radeon.h"

#include "terakan_device_drm_radeon.h"
#include "terakan_image.h"
#include "terakan_physical_device.h"

#include "util/macros.h"
#include "util/os_mman.h"
#include "util/u_math.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <xf86drm.h>
#include <radeon_drm.h>

static bool
terakan_bo_drm_radeon_set_tiling_for_surface(struct terakan_bo * const bo_base,
                                             struct radeon_surf const * const surface)
{
   struct terakan_bo_drm_radeon const * const bo =
      container_of(bo_base, struct terakan_bo_drm_radeon const, base);
   struct terakan_device_drm_radeon const * const device =
      container_of(bo->base.device, struct terakan_device_drm_radeon const, base);

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
   return drmCommandWriteRead(device->render_node_fd, DRM_RADEON_GEM_SET_TILING,
                              &set_tiling_arguments, sizeof(set_tiling_arguments)) == 0;
}

static int
terakan_bo_drm_radeon_export_fd(struct terakan_bo * const bo_base, bool const writable)
{
   struct terakan_bo_drm_radeon const * const bo =
      container_of(bo_base, struct terakan_bo_drm_radeon const, base);

   if (!bo->handle_shareable) {
      return -1;
   }

   struct terakan_device_drm_radeon const * const device =
      container_of(bo->base.device, struct terakan_device_drm_radeon const, base);

   int fd;
   if (drmPrimeHandleToFD(device->render_node_fd, bo->handle,
                          DRM_CLOEXEC | (writable ? DRM_RDWR : 0), &fd) != 0) {
      return -1;
   }
   return fd;
}

static void *
terakan_bo_drm_radeon_map_impl(struct terakan_bo * const bo_base)
{
   struct terakan_bo_drm_radeon const * const bo =
      container_of(bo_base, struct terakan_bo_drm_radeon const, base);
   struct terakan_device_drm_radeon const * const device =
      container_of(bo->base.device, struct terakan_device_drm_radeon const, base);

   /* The offset and size fields are ignored by the Radeon kernel driver as of Radeon 2.50.0, but
    * for correctness, still passing the size.
    */
   struct drm_radeon_gem_mmap gem_mmap_arguments = {
      .handle = bo->handle,
      .size = bo->size,
   };
   int const gem_mmap_result = drmCommandWriteRead(device->render_node_fd, DRM_RADEON_GEM_MMAP,
                                                   &gem_mmap_arguments, sizeof(gem_mmap_arguments));
   if (gem_mmap_result != 0) {
      vk_loge(VK_LOG_OBJS(device), "Failed to map the buffer 0x%" PRIX32 " in GEM, error number %d",
              (uint32_t)bo->handle, gem_mmap_result);
      return NULL;
   }

   void * const mapping = os_mmap(NULL, (size_t)bo->size, PROT_READ | PROT_WRITE, MAP_SHARED,
                                  device->render_node_fd, (off_t)gem_mmap_arguments.addr_ptr);
   if (mapping == MAP_FAILED) {
      vk_loge(VK_LOG_OBJS(device), "Failed to map the buffer 0x%" PRIX32 " in the OS: %m",
              (uint32_t)bo->handle);
      return NULL;
   }

   return mapping;
}

static void
terakan_bo_drm_radeon_unmap_impl(struct terakan_bo * const bo_base)
{
   struct terakan_bo_drm_radeon const * const bo =
      container_of(bo_base, struct terakan_bo_drm_radeon const, base);

   os_munmap(bo_base->mapping, (size_t)bo->size);
}

static void
terakan_bo_drm_radeon_create_reference(void * const bo_reference_ptr,
                                       struct terakan_bo const * const bo_base,
                                       bool const is_reading, bool const is_writing,
                                       enum terakan_bo_priority const priority)
{
   struct drm_radeon_cs_reloc * const bo_reference = (struct drm_radeon_cs_reloc *)bo_reference_ptr;

   struct terakan_bo_drm_radeon const * const bo =
      container_of(bo_base, struct terakan_bo_drm_radeon const, base);

   bo_reference->handle = bo->handle;

   bo_reference->read_domains = is_reading ? bo->domains : 0;
   bo_reference->write_domain = is_writing ? bo->domains : 0;

   assert(((__u32)priority & ~(__u32)RADEON_RELOC_PRIO_MASK) == 0);
   bo_reference->flags = (__u32)priority;
}

static void
terakan_bo_drm_radeon_update_reference(void * const bo_reference_ptr,
                                       struct terakan_bo const * const bo_base,
                                       bool const is_reading, bool const is_writing,
                                       enum terakan_bo_priority const priority)
{
   struct drm_radeon_cs_reloc * const bo_reference = (struct drm_radeon_cs_reloc *)bo_reference_ptr;

   struct terakan_bo_drm_radeon const * const bo =
      container_of(bo_base, struct terakan_bo_drm_radeon const, base);

   assert(bo_reference->handle == bo->handle);

   if (is_reading) {
      bo_reference->read_domains |= bo->domains;
   }
   if (is_writing) {
      bo_reference->write_domain |= bo->domains;
   }

   assert(((__u32)priority & ~(__u32)RADEON_RELOC_PRIO_MASK) == 0);
   /* The flags only contain the priority. */
   bo_reference->flags = MAX2((__u32)priority, bo_reference->flags);
}

static void
terakan_bo_drm_radeon_free_impl(struct terakan_bo * const bo_base,
                                VkAllocationCallbacks const * const allocator)
{
   struct terakan_bo_drm_radeon * const bo =
      container_of(bo_base, struct terakan_bo_drm_radeon, base);
   struct terakan_device_drm_radeon * const device =
      container_of(bo->base.device, struct terakan_device_drm_radeon, base);

   bool close_bo;
   if (bo->handle_shareable) {
      /* Close the handle atomically with reference count management to make sure the handle isn't
       * closed while another thread is importing the same GEM BO or destroying another winsys BO
       * for this GEM BO.
       */
      mtx_lock(&device->shared_bo_mutex);
      struct hash_entry * const reference_count_entry =
         _mesa_hash_table_search(device->shared_bo_reference_counts, (void *)(uintptr_t)bo->handle);
      assert(reference_count_entry != NULL && "All shareable BOs must be reference-counted");
      uintptr_t const old_reference_count = (uintptr_t)reference_count_entry->data;
      assert(old_reference_count > 0);
      if (old_reference_count > 1) {
         reference_count_entry->data = (void *)(old_reference_count - 1);
         close_bo = false;
      } else {
         _mesa_hash_table_remove(device->shared_bo_reference_counts, reference_count_entry);
         close_bo = true;
      }
   } else {
      close_bo = true;
   }

   if (close_bo) {
      struct drm_gem_close gem_close_arguments = {.handle = bo->handle};
      drmIoctl(device->render_node_fd, DRM_IOCTL_GEM_CLOSE, &gem_close_arguments);
   }

   if (bo->handle_shareable) {
      mtx_unlock(&device->shared_bo_mutex);
   }

   vk_free2(&device->base.vk.alloc, allocator, bo);
}

static VkResult
terakan_bo_drm_radeon_allocate_device_memory(
   struct terakan_device * const device_base, VkDeviceSize const size, VkDeviceSize const alignment,
   VkMemoryPropertyFlags const flags, VkExternalMemoryHandleTypeFlags const export_handle_types,
   VkAllocationCallbacks const * const allocator, VkSystemAllocationScope const scope,
   struct terakan_bo ** const bo_out)
{
   struct terakan_device_drm_radeon * const device =
      container_of(device_base, struct terakan_device_drm_radeon, base);

   struct terakan_bo_drm_radeon * const bo =
      vk_alloc2(&device->base.vk.alloc, allocator, sizeof(struct terakan_bo_drm_radeon),
                alignof(struct terakan_bo_drm_radeon), scope);
   if (bo == NULL) {
      return VK_ERROR_OUT_OF_HOST_MEMORY;
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
      if (!container_of(device->base.vk.physical, struct terakan_physical_device const, vk)
              ->chip_family_info.has_dedicated_vram) {
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

   int const gem_create_result =
      drmCommandWriteRead(device->render_node_fd, DRM_RADEON_GEM_CREATE, &gem_create_arguments,
                          sizeof(gem_create_arguments));
   if (gem_create_result != 0) {
      vk_free2(&device->base.vk.alloc, allocator, bo);
      vk_loge(VK_LOG_OBJS(device),
              "Failed to allocate a buffer, size: %" PRIu64 " bytes, alignment: %" PRIu64 " bytes, "
              "domains: 0x%" PRIX32 ", flags: 0x%" PRIX32 ", error number %d",
              size, alignment, (uint32_t)initial_domains, (uint32_t)gem_create_arguments.flags,
              gem_create_result);
      return VK_ERROR_OUT_OF_DEVICE_MEMORY;
   }

   terakan_bo_init(&bo->base, &device->base);

   bo->size = size;

   bo->domains = initial_domains;

   bo->handle = gem_create_arguments.handle;

   if (export_handle_types & (VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT |
                              VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT)) {
      bo->handle_shareable = true;

      mtx_lock(&device->shared_bo_mutex);
      bool const shared_handle_reference_count_inserted =
         _mesa_hash_table_insert(device->shared_bo_reference_counts,
                                 (void *)(uintptr_t)gem_create_arguments.handle, (void *)1) != NULL;
      mtx_unlock(&device->shared_bo_mutex);
      if (!shared_handle_reference_count_inserted) {
         struct drm_gem_close gem_close_arguments = {.handle = gem_create_arguments.handle};
         drmIoctl(device->render_node_fd, DRM_IOCTL_GEM_CLOSE, &gem_close_arguments);
         vk_free2(&device->base.vk.alloc, allocator, bo);
         return VK_ERROR_OUT_OF_HOST_MEMORY;
      }
   } else {
      bo->handle_shareable = false;
   }

   *bo_out = &bo->base;
   return VK_SUCCESS;
}

static bool
terakan_bo_drm_radeon_get_fd_vram_preference(struct terakan_device * const device_base,
                                             int const fd, bool * const vram_preferred_out)
{
   struct terakan_device_drm_radeon * const device =
      container_of(device_base, struct terakan_device_drm_radeon, base);

   /* Get the handle atomically with reference count management to make sure the handle isn't closed
    * by another thread while it's being imported.
    */
   mtx_lock(&device->shared_bo_mutex);

   __u32 handle;
   if (drmPrimeFDToHandle(device->render_node_fd, fd, &handle) != 0) {
      return false;
   }

   struct drm_radeon_gem_op gem_op_arguments = {
      .handle = handle,
      .op = RADEON_GEM_OP_GET_INITIAL_DOMAIN,
   };
   int const gem_op_result = drmCommandWriteRead(device->render_node_fd, DRM_RADEON_GEM_OP,
                                                 &gem_op_arguments, sizeof(gem_op_arguments));

   /* Close the handle if there are no driver BOs backed by the same GEM BO. */
   struct hash_entry * const reference_count_entry =
      _mesa_hash_table_search(device->shared_bo_reference_counts, (void *)(uintptr_t)handle);
   assert(reference_count_entry == NULL || (uintptr_t)reference_count_entry->data > 0);
   if (reference_count_entry == NULL) {
      struct drm_gem_close gem_close_arguments = {.handle = handle};
      drmIoctl(device->render_node_fd, DRM_IOCTL_GEM_CLOSE, &gem_close_arguments);
   }

   mtx_unlock(&device->shared_bo_mutex);

   if (gem_op_result != 0) {
      return false;
   }

   *vram_preferred_out = (gem_op_arguments.value & RADEON_GEM_DOMAIN_VRAM) != 0;

   return true;
}

static VkResult
terakan_bo_drm_radeon_import_fd(struct terakan_device * const device_base, int const fd,
                                VkDeviceSize const size, bool const prefer_vram,
                                VkAllocationCallbacks const * const allocator,
                                VkSystemAllocationScope const scope,
                                struct terakan_bo ** const bo_out)
{
   struct terakan_device_drm_radeon * const device =
      container_of(device_base, struct terakan_device_drm_radeon, base);

   struct terakan_bo_drm_radeon * const bo =
      vk_alloc2(&device->base.vk.alloc, allocator, sizeof(struct terakan_bo_drm_radeon),
                alignof(struct terakan_bo_drm_radeon), scope);
   if (bo == NULL) {
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   /* Get the handle atomically with reference count management to make sure the handle isn't closed
    * by another thread while it's being imported.
    */
   mtx_lock(&device->shared_bo_mutex);

   __u32 handle;
   int const prime_fd_to_handle_result = drmPrimeFDToHandle(device->render_node_fd, fd, &handle);
   if (prime_fd_to_handle_result != 0) {
      mtx_unlock(&device->shared_bo_mutex);
      vk_free2(&device->base.vk.alloc, allocator, bo);
      vk_loge(VK_LOG_OBJS(device),
              "Failed to import a file descriptor as a buffer, size: %" PRIu64 " bytes, preferred "
              "domain: %s, error number %d",
              size, prefer_vram ? "VRAM" : "GTT", prime_fd_to_handle_result);
      return VK_ERROR_INVALID_EXTERNAL_HANDLE;
   }

   uint32_t reference_count_hash = _mesa_hash_pointer((void *)(uintptr_t)handle);
   struct hash_entry * const existing_reference_count_entry = _mesa_hash_table_search_pre_hashed(
      device->shared_bo_reference_counts, reference_count_hash, (void *)(uintptr_t)handle);
   if (existing_reference_count_entry != NULL) {
      assert((uintptr_t)existing_reference_count_entry->data > 0);
      existing_reference_count_entry->data =
         (void *)((uintptr_t)existing_reference_count_entry->data + 1);
   } else {
      if (_mesa_hash_table_insert_pre_hashed(device->shared_bo_reference_counts,
                                             reference_count_hash, (void *)(uintptr_t)handle,
                                             (void *)1) == NULL) {
         /* Close the BO inside the critical section so it's not closed while other threads may be
          * trying to import the BO with the same handle.
          */
         struct drm_gem_close gem_close_arguments = {.handle = handle};
         drmIoctl(device->render_node_fd, DRM_IOCTL_GEM_CLOSE, &gem_close_arguments);
         mtx_unlock(&device->shared_bo_mutex);
         vk_free2(&device->base.vk.alloc, allocator, bo);
         return VK_ERROR_OUT_OF_HOST_MEMORY;
      }
   }

   mtx_unlock(&device->shared_bo_mutex);

   terakan_bo_init(&bo->base, &device->base);

   bo->size = size;

   bo->domains = prefer_vram ? RADEON_GEM_DOMAIN_VRAM : RADEON_GEM_DOMAIN_GTT;

   bo->handle = handle;

   bo->handle_shareable = true;

   *bo_out = &bo->base;
   return VK_SUCCESS;
}

struct terakan_bo_winsys_fn const terakan_bo_drm_radeon_fn = {
   .set_tiling_for_surface = terakan_bo_drm_radeon_set_tiling_for_surface,
   .export_fd = terakan_bo_drm_radeon_export_fd,
   .map_impl = terakan_bo_drm_radeon_map_impl,
   .unmap_impl = terakan_bo_drm_radeon_unmap_impl,
   .create_reference = terakan_bo_drm_radeon_create_reference,
   .update_reference = terakan_bo_drm_radeon_update_reference,
   .free_impl = terakan_bo_drm_radeon_free_impl,
   .allocate_device_memory = terakan_bo_drm_radeon_allocate_device_memory,
   .get_fd_vram_preference = terakan_bo_drm_radeon_get_fd_vram_preference,
   .import_fd = terakan_bo_drm_radeon_import_fd,
};
