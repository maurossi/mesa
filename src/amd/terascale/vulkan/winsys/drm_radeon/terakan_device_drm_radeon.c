/*
 * Copyright © 2024 Vitaliy Triang3l Kuzmin
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

#include "terakan_device_drm_radeon.h"

#include "terakan_bo_drm_radeon.h"
#include "terakan_queue_drm_radeon.h"

#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <radeon_drm.h>

static void
terakan_device_drm_radeon_destroy(struct terakan_device * const device_base)
{
   struct terakan_device_drm_radeon * const device =
      container_of(device_base, struct terakan_device_drm_radeon, base);

   terakan_device_finish(&device->base);

   _mesa_hash_table_destroy(device->shared_bo_reference_counts, NULL);
   mtx_destroy(&device->shared_bo_mutex);

   close(device->render_node_fd);

   vk_free(&device->base.vk.alloc, device);
}

static struct terakan_device_winsys_fn const terakan_device_drm_radeon_fn = {
   .bo = &terakan_bo_drm_radeon_fn,
   .queue = &terakan_queue_drm_radeon_fn,
   .destroy = terakan_device_drm_radeon_destroy,
};

VkResult
terakan_device_drm_radeon_create(struct terakan_physical_device * const physical_device_base,
                                 VkDeviceCreateInfo const * const create_info,
                                 VkAllocationCallbacks const * const allocator,
                                 struct terakan_device ** const device_out)
{
   VkResult result;

   struct terakan_physical_device_drm_radeon * const physical_device =
      container_of(physical_device_base, struct terakan_physical_device_drm_radeon, base);

   struct vk_instance const * const instance = physical_device->base.vk.instance;

   struct terakan_device_drm_radeon * const device =
      vk_alloc2(&instance->alloc, allocator, sizeof(struct terakan_device_drm_radeon),
                alignof(struct terakan_device_drm_radeon), VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (device == NULL) {
      return vk_error(instance, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   /* Open a new render node file descriptor for this logical device, as independent as possible
    * from other logical devices created from this physical device (including having a separate
    * space for shared BO handles).
    */
   device->render_node_fd = open(physical_device->render_node_path, O_RDWR | O_CLOEXEC);
   if (device->render_node_fd < 0) {
      result = vk_errorf(
         instance, errno == ENOMEM ? VK_ERROR_OUT_OF_HOST_MEMORY : VK_ERROR_INITIALIZATION_FAILED,
         "Failed to open the DRM device '%s': %m", physical_device->render_node_path);
      goto fail_alloc;
   }

   /* Coarsely check that the render node file still belongs to the same physical device, by
    * verifying that the file descriptor opened for this render node for the lifetime of the
    * physical device is still valid. This is done after opening the logical device's file
    * descriptor, not before, as otherwise there would've been a period of time between the check
    * and the opening when the result of the check might have become outdated.
    */
   if (fcntl(physical_device->render_node_validation_fd, F_GETFD) == -1) {
      result = vk_errorf(
         instance, VK_ERROR_INITIALIZATION_FAILED,
         "Validation of the physical device's DRM render node file descriptor failed, not creating "
         "the logical device as it's not known if the render node path '%s' still points to a "
         "supported DRM device: %m",
         physical_device->render_node_path);
      goto fail_render_node_fd;
   }

   /* Initialize the device. */

   if (mtx_init(&device->shared_bo_mutex, mtx_plain) != thrd_success) {
      result = vk_errorf(instance, VK_ERROR_OUT_OF_HOST_MEMORY,
                         "Failed to initialize the shared buffer reference counting mutex");
      goto fail_render_node_fd;
   }
   device->shared_bo_reference_counts = _mesa_pointer_hash_table_create(NULL);
   if (device->shared_bo_reference_counts == NULL) {
      result = vk_errorf(instance, VK_ERROR_OUT_OF_HOST_MEMORY,
                         "Failed to create the shared buffer reference count table");
      goto fail_shared_bo_mutex;
   }

   /* TODO(Triang3l): With virtual memory, query RADEON_INFO_IB_VM_MAX_SIZE for the maximum indirect
    * buffer size in dwords.
    */
   terakan_queue_submission_context_init(
      &device->gfx_submission_context.base,
      (struct terakan_queue_submission_size){
         /* DRM Radeon accepts BO reference offsets in dwords in relocations. */
         .bo_references = UINT32_MAX / (sizeof(struct drm_radeon_cs_reloc) / sizeof(__u32)),
         .indirect_buffer_dwords = UINT32_MAX,
      });
   device->gfx_submission_context.device = device;
   device->gfx_submission_context.ring = RADEON_CS_RING_GFX;

   result = terakan_device_init(&device->base, &physical_device->base, create_info, allocator,
                                &terakan_device_drm_radeon_fn, sizeof(struct drm_radeon_cs_reloc),
                                alignof(struct drm_radeon_cs_reloc));
   if (result != VK_SUCCESS) {
      goto fail_shared_bo_reference_counts;
   }

   *device_out = &device->base;
   return VK_SUCCESS;

fail_shared_bo_reference_counts:
   _mesa_hash_table_destroy(device->shared_bo_reference_counts, NULL);
fail_shared_bo_mutex:
   mtx_destroy(&device->shared_bo_mutex);
fail_render_node_fd:
   close(device->render_node_fd);
fail_alloc:
   vk_free2(&instance->alloc, allocator, device);
   return result;
}
