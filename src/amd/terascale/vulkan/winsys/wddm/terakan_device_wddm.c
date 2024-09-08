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

#include "terakan_device_wddm.h"

#include "terakan_bo_wddm.h"
#include "terakan_physical_device_wddm.h"
#include "terakan_queue_wddm.h"
#include "terakan_wddm_d3dkmthk.h"

#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <stddef.h>

static void
terakan_device_wddm_destroy(struct terakan_device * const device_base)
{
   struct terakan_device_wddm * const device =
      container_of(device_base, struct terakan_device_wddm, base);

   terakan_device_finish(&device->base);

   D3DKMT_DESTROYDEVICE const destroy_device_arguments = {.hDevice = device->d3dkmt_device};
   D3DKMTDestroyDevice(&destroy_device_arguments);

   vk_free(&device->base.vk.alloc, device);
}

static struct terakan_device_winsys_fn const terakan_device_wddm_fn = {
   .bo = &terakan_bo_wddm_fn,
   .queue = &terakan_queue_wddm_fn,
   .destroy = terakan_device_wddm_destroy,
};

VkResult
terakan_device_wddm_create(struct terakan_physical_device * const physical_device_base,
                           VkDeviceCreateInfo const * const create_info,
                           VkAllocationCallbacks const * const allocator,
                           struct terakan_device ** const device_out)
{
   VkResult result;

   struct terakan_physical_device_wddm * const physical_device =
      container_of(physical_device_base, struct terakan_physical_device_wddm, base);

   struct vk_instance const * const instance = physical_device->base.vk.instance;

   struct terakan_device_wddm * const device =
      vk_alloc2(&instance->alloc, allocator, sizeof(struct terakan_device_wddm),
                alignof(struct terakan_device_wddm), VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (device == NULL) {
      return vk_error(instance, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   D3DKMT_CREATEDEVICE create_device_arguments = {
      .hAdapter = physical_device->d3dkmt_adapter,
   };
   NTSTATUS const create_device_status = D3DKMTCreateDevice(&create_device_arguments);
   if (!NT_SUCCESS(create_device_status)) {
      result =
         vk_errorf(instance, VK_ERROR_INITIALIZATION_FAILED,
                   "Failed to create the D3DKMT device, status 0x%08lX", create_device_status);
      goto fail_alloc;
   }
   device->d3dkmt_device = create_device_arguments.hDevice;

   result = terakan_device_init(&device->base, &physical_device->base, create_info, allocator,
                                &terakan_device_wddm_fn, sizeof(D3DDDI_ALLOCATIONLIST),
                                alignof(D3DDDI_ALLOCATIONLIST));
   if (result != VK_SUCCESS) {
      goto fail_d3dkmt_device;
   }

   *device_out = &device->base;
   return VK_SUCCESS;

fail_d3dkmt_device:
   D3DKMT_DESTROYDEVICE const destroy_device_arguments = {.hDevice = device->d3dkmt_device};
   D3DKMTDestroyDevice(&destroy_device_arguments);
fail_alloc:
   vk_free2(&instance->alloc, allocator, device);
   return result;
}
