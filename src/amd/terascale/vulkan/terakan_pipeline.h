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

#ifndef TERAKAN_PIPELINE_H
#define TERAKAN_PIPELINE_H

#include "vk_object.h"
#include "vk_util.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_pipeline {
   struct vk_object_base base;

   bool is_compute;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_pipeline, base, VkPipeline, VK_OBJECT_TYPE_PIPELINE)

static inline VkPipelineCreateFlags2KHR
terakan_pipeline_create_flags(VkPipelineCreateFlags const base_flags, void const * const next)
{
   VkPipelineCreateFlags2CreateInfoKHR const * const flags_2_info =
      vk_find_struct_const(next, PIPELINE_CREATE_FLAGS_2_CREATE_INFO_KHR);
   if (flags_2_info != NULL) {
      return flags_2_info->flags;
   }
   return (VkPipelineCreateFlags2KHR)base_flags;
}

void terakan_pipeline_finish(struct terakan_pipeline * pipeline);

uint32_t const * terakan_pipeline_stage_spirv(VkPipelineShaderStageCreateInfo const * stage_info,
                                              size_t * size_bytes_out);

struct terakan_device;

void terakan_pipeline_init(struct terakan_pipeline * pipeline, struct terakan_device * device,
                           bool is_compute);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_PIPELINE_H */
