/*
 * Copyright © 2023 Vitaliy Triang3l Kuzmin
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

#ifndef TERAKAN_PHYSICAL_DEVICE_H
#define TERAKAN_PHYSICAL_DEVICE_H

#include "winsys/terakan_winsys.h"
#include "terakan_instance.h"
#include "wsi_common.h"

#include "vk_physical_device.h"

#if !defined(_WIN32)
#include <xf86drm.h>
#include <sys/types.h>
#endif

struct terakan_physical_device {
   struct vk_physical_device vk;

#if !defined(_WIN32)
   /* TODO(Triang3l): master_fd. */
   int local_fd;
#endif

   struct terakan_winsys * winsys;

#if !defined(_WIN32)
   int drm_available_nodes;
   drmPciBusInfo drm_bus_info;
   dev_t drm_primary_devid;
   dev_t drm_render_devid;
#endif

   uint8_t device_uuid[VK_UUID_SIZE];

   VkPhysicalDeviceMemoryProperties memory_properties;

   struct wsi_device wsi_device;
};

VK_DEFINE_HANDLE_CASTS(terakan_physical_device, vk.base, VkPhysicalDevice,
                       VK_OBJECT_TYPE_PHYSICAL_DEVICE)

VkResult terakan_physical_device_try_create_for_drm(struct vk_instance * instance,
                                                    struct _drmDevice * drm_device,
                                                    struct vk_physical_device ** device_out);

void terakan_physical_device_destroy(struct vk_physical_device * device);

#endif /* TERAKAN_PHYSICAL_DEVICE_H */
