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

#ifndef TERAKAN_PHYSICAL_DEVICE_DRM_RADEON_H
#define TERAKAN_PHYSICAL_DEVICE_DRM_RADEON_H

#include "terakan_physical_device.h"

#include "vk_sync.h"
#include "vk_sync_binary.h"

#include <stdbool.h>
#include <stdint.h>
#include <xf86drm.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_physical_device_drm_radeon {
   struct terakan_physical_device base;

   drmPciBusInfo pci_bus_info;

   bool has_primary_node;
   int64_t primary_node_device_id_major;
   int64_t primary_node_device_id_minor;

   int64_t render_node_device_id_major;
   int64_t render_node_device_id_minor;
   char * render_node_path;
   int render_node_validation_fd;

   struct vk_sync_binary_type sync_type_binary;
   struct vk_sync_type const * sync_types[3];
};

VkResult terakan_physical_device_drm_radeon_try_create(struct vk_instance * instance,
                                                       struct _drmDevice * drm_device,
                                                       struct vk_physical_device ** device_out);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_PHYSICAL_DEVICE_DRM_RADEON_H */
