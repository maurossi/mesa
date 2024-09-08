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

#ifndef TERAKAN_PIPELINE_LAYOUT_H
#define TERAKAN_PIPELINE_LAYOUT_H

#include "compiler/shader_enums.h"
#include "vk_pipeline_layout.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_pipeline_layout_set {
   uint8_t first_shader_resources[MESA_SHADER_STAGES];
   uint8_t first_shader_uniform_buffers[MESA_SHADER_STAGES];
   uint8_t first_shader_samplers[MESA_SHADER_STAGES];
};

struct terakan_pipeline_layout {
   struct vk_pipeline_layout vk;

   struct terakan_pipeline_layout_set * sets;

   uint32_t shader_non_immutable_samplers[MESA_SHADER_STAGES];
   uint32_t shader_immutable_samplers_unnormalized_coordinates[MESA_SHADER_STAGES];

   uint32_t shader_app_push_constants_extents_bytes[MESA_SHADER_STAGES];
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_pipeline_layout, vk.base, VkPipelineLayout,
                               VK_OBJECT_TYPE_PIPELINE_LAYOUT)

struct terakan_device;

VkResult terakan_pipeline_layout_create(struct terakan_device * device,
                                        VkPipelineLayoutCreateInfo const * create_info,
                                        VkShaderStageFlags stage_mask,
                                        struct terakan_pipeline_layout ** pipeline_layout_out);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_PIPELINE_LAYOUT_H */
