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

#include "terakan_wsi.h"

#include "vk_instance.h"
#include "wsi_common.h"

static VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
terakan_wsi_proc_addr(VkPhysicalDevice const physicalDevice, char const * const pName)
{
   struct terakan_physical_device const * const physical_device =
      terakan_physical_device_from_handle(physicalDevice);
   return vk_instance_get_proc_addr_unchecked(physical_device->vk.instance, pName);
}

void
terakan_wsi_finish(struct terakan_physical_device * const physical_device)
{
   physical_device->vk.wsi_device = NULL;
   wsi_device_finish(&physical_device->wsi_device, &physical_device->vk.instance->alloc);
}

VkResult
terakan_wsi_init(struct terakan_physical_device * const physical_device)
{
   struct wsi_device_options const device_options = {.sw_device = false};
   VkResult result = wsi_device_init(
      &physical_device->wsi_device, terakan_physical_device_to_handle(physical_device),
      terakan_wsi_proc_addr, &physical_device->vk.instance->alloc, -1, NULL, &device_options);
   if (result != VK_SUCCESS) {
      return result;
   }

   physical_device->vk.wsi_device = &physical_device->wsi_device;

#if defined(VK_USE_PLATFORM_WIN32_KHR)
   /* As of April 2024, the DXGI path uses Direct3D 12, which is not available on TeraScale.
    * Until a more direct solution is implemented, present via copying to the host.
    */
   physical_device->wsi_device.sw = true;
#endif

   return VK_SUCCESS;
}
