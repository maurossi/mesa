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

#ifndef TERAKAN_DEVICE_DRM_RADEON_H
#define TERAKAN_DEVICE_DRM_RADEON_H

#include "terakan_device.h"
#include "terakan_physical_device_drm_radeon.h"
#include "terakan_queue_drm_radeon.h"

#include "c11/threads.h"
#include "util/hash_table.h"

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_device_drm_radeon {
   struct terakan_device base;

   int render_node_fd;

   /* Implementing reference counting for shared BO handles, since drmPrimeFDToHandle returns the
    * same handle when importing the same BO multiple times even with different file descriptors,
    * and GEM has no implicit reference counting for BO handles.
    * Reference count updates must be done atomically with drmPrimeFDToHandle and
    * DRM_IOCTL_GEM_CLOSE using the mutex.
    */
   mtx_t shared_bo_mutex;
   struct hash_table * shared_bo_reference_counts;

   struct terakan_queue_submission_context_drm_radeon gfx_submission_context;
};

VkResult terakan_device_drm_radeon_create(struct terakan_physical_device * physical_device,
                                          VkDeviceCreateInfo const * create_info,
                                          VkAllocationCallbacks const * allocator,
                                          struct terakan_device ** device_out);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_DEVICE_DRM_RADEON_H */
