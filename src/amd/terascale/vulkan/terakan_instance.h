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

#ifndef TERAKAN_INSTANCE_H
#define TERAKAN_INSTANCE_H

#include "vk_instance.h"

#include <stdint.h>

#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
#define TERAKAN_USE_WSI_PLATFORM
#endif

#define TERAKAN_API_VERSION VK_MAKE_API_VERSION(0, 1, 0, VK_HEADER_VERSION)

enum {
   TERAKAN_DEBUG_STARTUP = (uint64_t)1 << 0,
};

struct terakan_instance {
   struct vk_instance vk;

   uint64_t debug_flags;

   /* Binding allocation in the physical device limits. */
   /* From 4 to 8. The rest of RAT bindings will be used for storage images. */
   uint32_t max_per_stage_storage_buffers;
   /* Uniform buffers, sampled images and input attachments are allocated from one range. */
   uint32_t max_per_stage_uniform_buffers;
   uint32_t max_per_stage_sampled_images;
   uint32_t max_per_stage_input_attachments;
};

VK_DEFINE_HANDLE_CASTS(terakan_instance, vk.base, VkInstance, VK_OBJECT_TYPE_INSTANCE)

#endif /* TERAKAN_INSTANCE_H */
