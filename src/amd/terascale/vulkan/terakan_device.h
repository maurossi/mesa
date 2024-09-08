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

#ifndef TERAKAN_DEVICE_H
#define TERAKAN_DEVICE_H

#include "meta/terakan_meta.h"
#include "terakan_bo.h"
#include "terakan_image.h"
#include "terakan_physical_device.h"
#include "terakan_queue.h"
#include "terakan_shader.h"

#include "c11/threads.h"
#include "ac_surface.h"
#include "amd_family.h"
#include "vk_device.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_device_winsys_fn {
   struct terakan_image_winsys_fn const * image;

   struct terakan_bo_winsys_fn const * bo;

   struct terakan_queue_winsys_fn const * queue;

   void (*destroy)(struct terakan_device * device);
};

/* Partially implemented by the winsys. */
struct terakan_device {
   struct vk_device vk;

   struct terakan_device_winsys_fn const * winsys_fn;

   size_t bo_reference_size;
   size_t bo_reference_alignment;

   uint32_t last_bo_creation_number;

   struct terakan_bo * meta_shaders_bo;
   struct terakan_shader_static meta_shaders[TERAKAN_META_SHADER_COUNT];

   /* Mutex and condition variable for terakan_sync_completion timeline semaphore value updates and
    * controlling submission completion waits.
    */
   mtx_t completion_mutex;
   cnd_t completion_condition;

   /* Whether awaiting submission completion isn't possible anymore, as a result of device
    * destruction or submission-related device loss.
    * Protected by completion_mutex, broadcast completion_condition when setting.
    */
   bool completion_lost;

   /* Each queue is optional, if requested by the application. */
   struct terakan_queue * queue_graphics;
};

VK_DEFINE_HANDLE_CASTS(terakan_device, vk.base, VkDevice, VK_OBJECT_TYPE_DEVICE)

void terakan_device_finish(struct terakan_device * device);

VkResult terakan_device_init(struct terakan_device * device,
                             struct terakan_physical_device * physical_device,
                             VkDeviceCreateInfo const * create_info,
                             VkAllocationCallbacks const * allocator,
                             struct terakan_device_winsys_fn const * winsys_fn_static,
                             size_t bo_reference_size, size_t bo_reference_alignment);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_DEVICE_H */
