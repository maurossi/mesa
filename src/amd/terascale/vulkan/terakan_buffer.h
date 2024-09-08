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

#ifndef TERAKAN_BUFFER_H
#define TERAKAN_BUFFER_H

#include "terakan_bo.h"
#include "terakan_descriptor.h"

#include "vk_buffer.h"
#include "vk_buffer_view.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_buffer {
   struct vk_buffer vk;

   struct terakan_bo const * bo;
   uint64_t va;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_buffer, vk.base, VkBuffer, VK_OBJECT_TYPE_BUFFER)

struct terakan_bo const *
terakan_buffer_create_uniform_buffer_descriptor(VkDescriptorBufferInfo const * buffer_info,
                                                uint32_t resource_out[8]);

struct terakan_bo const *
terakan_buffer_create_storage_buffer_descriptor(VkDescriptorBufferInfo const * buffer_info,
                                                uint32_t resource_out[8],
                                                struct terakan_color_descriptor * color_out);

struct terakan_buffer_view {
   struct vk_buffer_view vk;

   struct terakan_bo const * bo;

   uint32_t resource[8];

   struct terakan_color_descriptor color;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_buffer_view, vk.base, VkBufferView,
                               VK_OBJECT_TYPE_BUFFER_VIEW)

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_BUFFER_H */
