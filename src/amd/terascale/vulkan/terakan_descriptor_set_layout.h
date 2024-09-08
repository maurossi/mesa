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

#ifndef TERAKAN_DESCRIPTOR_SET_LAYOUT_H
#define TERAKAN_DESCRIPTOR_SET_LAYOUT_H

#include "compiler/shader_enums.h"
#include "vk_descriptor_set_layout.h"
#include "vk_sampler.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_descriptor_set_layout_binding {
   VkDescriptorType descriptor_type;

   /* If descriptor_count is 0, all other fields are invalid.
    * vkUpdateDescriptorSets should start the update at the next binding with nonzero
    * descriptor_count.
    */
   uint16_t descriptor_count;

   /* UINT16_MAX if not using an immutable sampler or dynamic offset. */
   uint16_t first_immutable_sampler_or_dynamic_offset;

   uint32_t immutable_samplers_unnormalized_coordinates;

   /* Primarily for writing.
    * Descriptors of each type for consecutive bindings are laid out in descriptor memory
    * sequentially so descriptor counts in writes can span both array elements and consecutive
    * bindings.
    * UAVs are allocated regardless of the stage flags for the simplicity of writing and copying.
    */
   uint16_t first_set_uav;
   uint16_t first_set_resource;
   uint8_t first_set_sampler;

   uint8_t stage_flags;

   /* Primarily for shader compilation and pushing. */
   uint8_t first_shader_resources[MESA_SHADER_STAGES];
   uint8_t first_shader_uniform_buffers[MESA_SHADER_STAGES];
   uint8_t first_shader_samplers[MESA_SHADER_STAGES];
};

struct terakan_descriptor_set_layout_shader_range {
   /* Of the range's type. */
   uint16_t first_set_descriptor;
   /* UINT16_MAX if not using a dynamic offset. */
   uint16_t first_dynamic_offset;
   /* Of the range's type. */
   uint8_t first_shader_descriptor;
   uint8_t descriptor_count;
};

struct terakan_descriptor_set_layout_shader {
   uint32_t non_immutable_samplers;
   uint32_t immutable_samplers_unnormalized_coordinates;

   uint16_t first_resource_range;
   uint16_t first_sampler_range;
   uint8_t resource_range_count;
   uint8_t sampler_range_count;

   uint8_t resource_count;
   uint8_t uniform_buffer_count;
   uint8_t sampler_count;
};

struct terakan_descriptor_set_layout {
   struct vk_descriptor_set_layout vk;

   /* Resources are the in the beginning of the set's descriptor memory. */
   uint32_t pool_first_sampler_offset_bytes;
   /* For simplicity of copying descriptors (the destination must not be immutable, but the source
    * may be - VUID-VkCopyDescriptorSet-dstBinding-02753), immutable samplers are stored in the set
    * memory, but are initialized when allocating a set.
    * This also simplifies binding, which is done frequently. Note that immutable samplers must be
    * bound at vkCmdBindDescriptorSets time, while vkCmdBindPipeline (or draws / dispatches) must
    * not destructively overwrite bindings from previously bound sets, to maintain pipeline layout
    * compatibility - binding a pipeline or drawing / dispatching doesn't disturb descriptor sets,
    * binding descriptor sets does.
    */
   uint32_t pool_first_uav_offset_bytes;
   uint32_t pool_size_bytes;

   uint16_t dynamic_offset_count;
   uint8_t immutable_sampler_count;

   uint8_t * immutable_sampler_indices_in_set;
   struct terakan_sampler const ** immutable_samplers;

   /* Primarily for binding. */
   struct terakan_descriptor_set_layout_shader_range * shader_ranges;
   struct terakan_descriptor_set_layout_shader shaders[MESA_SHADER_STAGES];

   size_t binding_count;
   struct terakan_descriptor_set_layout_binding * bindings;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_descriptor_set_layout, vk.base, VkDescriptorSetLayout,
                               VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT)

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_DESCRIPTOR_SET_LAYOUT_H */
