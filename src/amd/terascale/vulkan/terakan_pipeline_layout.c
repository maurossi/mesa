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

#include "terakan_pipeline_layout.h"

#include "terakan_command_buffer.h"
#include "terakan_descriptor.h"
#include "terakan_descriptor_set.h"
#include "terakan_descriptor_set_layout.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_hw_state.h"

#include "compiler/shader_enums.h"
#include "gallium/drivers/r600/evergreend.h"
#include "util/bitscan.h"
#include "util/macros.h"
#include "util/u_math.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

VKAPI_ATTR void VKAPI_CALL
terakan_CmdBindDescriptorSets(VkCommandBuffer const commandBuffer,
                              VkPipelineBindPoint const pipelineBindPoint,
                              VkPipelineLayout const layoutHandle, uint32_t const firstSet,
                              uint32_t const descriptorSetCount,
                              VkDescriptorSet const * const pDescriptorSets,
                              UNUSED uint32_t const dynamicOffsetCount,
                              uint32_t const * const pDynamicOffsets)
{
   struct terakan_gfx_command_writer * const command_writer =
      terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx;

   struct terakan_pipeline_layout const * const layout =
      terakan_pipeline_layout_from_handle(layoutHandle);

   bool const is_compute = pipelineBindPoint == VK_PIPELINE_BIND_POINT_COMPUTE;
   gl_shader_stage const shader_stage_first = is_compute ? MESA_SHADER_COMPUTE : MESA_SHADER_VERTEX;
   gl_shader_stage const shader_stage_last =
      is_compute ? MESA_SHADER_COMPUTE : MESA_SHADER_FRAGMENT;
   /* TODO(Triang3l): Apply bindings to the compute stage. */

   uint32_t const * set_dynamic_offsets = pDynamicOffsets;

   for (uint32_t set_relative_index = 0; set_relative_index < descriptorSetCount;
        ++set_relative_index) {
      struct terakan_pipeline_layout_set const * const layout_set =
         &layout->sets[firstSet + set_relative_index];

      struct terakan_descriptor_set const * const set =
         terakan_descriptor_set_from_handle(pDescriptorSets[set_relative_index]);
      if (set == NULL) {
         continue;
      }
      struct terakan_descriptor_set_layout const * const set_layout = set->layout;

      struct terakan_descriptor_set_resource const * const set_resources =
         (struct terakan_descriptor_set_resource const *)set->descriptors;
      struct terakan_descriptor_set_sampler const * const set_samplers =
         (struct terakan_descriptor_set_sampler const *)(set->descriptors +
                                                         set_layout
                                                            ->pool_first_sampler_offset_bytes);

      for (gl_shader_stage shader_stage = shader_stage_first; shader_stage <= shader_stage_last;
           ++shader_stage) {
         struct terakan_descriptor_set_layout_shader const * const set_layout_shader =
            &set_layout->shaders[shader_stage];

         /* Resources. */

         terakan_hw_state_draw_set_sq_resource_function const graphics_resource_setter =
            is_compute ? NULL : terakan_hw_state_draw_set_sq_resource_for_stage[shader_stage];
         uint8_t const shader_resource_set_base = layout_set->first_shader_resources[shader_stage];
         struct terakan_descriptor_set_layout_shader_range const * const resource_ranges =
            set_layout->shader_ranges + set_layout_shader->first_resource_range;
         for (uint8_t range_index = 0; range_index < set_layout_shader->resource_range_count;
              ++range_index) {
            struct terakan_descriptor_set_layout_shader_range const * const range =
               &resource_ranges[range_index];
            struct terakan_descriptor_set_resource const * const range_set_resources =
               set_resources + range->first_set_descriptor;
            uint8_t const range_shader_base =
               shader_resource_set_base + range->first_shader_descriptor;
            if (range->first_dynamic_offset != UINT16_MAX) {
               uint32_t const * const range_dynamic_offsets =
                  set_dynamic_offsets + range->first_dynamic_offset;
               for (uint8_t resource_index = 0; resource_index < range->descriptor_count;
                    ++resource_index) {
                  struct terakan_descriptor_set_resource resource =
                     range_set_resources[resource_index];
                  /* Because descriptors for bindings not statically referenced by the pipeline can
                   * be undefined, the BO pointer must not be dereferenced here as it may be
                   * outdated.
                   */
                  if (resource.bo != NULL) {
                     assert(G_03001C_TYPE(resource.resource[7]) ==
                            V_03001C_SQ_TEX_VTX_VALID_BUFFER);
                     uint64_t const resource_address =
                        (resource.resource[0] |
                         ((uint64_t)G_030008_BASE_ADDRESS_HI(resource.resource[2]) << 32)) +
                        range_dynamic_offsets[resource_index];
                     resource.resource[0] = (uint32_t)resource_address;
                     resource.resource[2] = (resource.resource[2] & C_030008_BASE_ADDRESS_HI) |
                                            S_030008_BASE_ADDRESS_HI(resource_address >> 32);
                  }
                  graphics_resource_setter(&command_writer->hw_state_draw,
                                           range_shader_base + resource_index, resource.bo,
                                           resource.resource);
               }
            } else {
               for (uint8_t resource_index = 0; resource_index < range->descriptor_count;
                    ++resource_index) {
                  struct terakan_descriptor_set_resource const * const resource =
                     &range_set_resources[resource_index];
                  graphics_resource_setter(&command_writer->hw_state_draw,
                                           range_shader_base + resource_index, resource->bo,
                                           resource->resource);
               }
            }
         }

         /* Samplers. */

         terakan_hw_state_draw_set_sq_sampler_function const graphics_sampler_setter =
            is_compute ? NULL : terakan_hw_state_draw_set_sq_sampler_for_stage[shader_stage];
         uint8_t const shader_sampler_set_base = layout_set->first_shader_samplers[shader_stage];
         struct terakan_descriptor_set_layout_shader_range const * const sampler_ranges =
            set_layout->shader_ranges + set_layout_shader->first_sampler_range;
         for (uint8_t range_index = 0; range_index < set_layout_shader->sampler_range_count;
              ++range_index) {
            struct terakan_descriptor_set_layout_shader_range const * const range =
               &sampler_ranges[range_index];
            struct terakan_descriptor_set_sampler const * const range_set_samplers =
               set_samplers + range->first_set_descriptor;
            uint8_t const range_shader_base =
               shader_sampler_set_base + range->first_shader_descriptor;
            for (uint8_t sampler_index = 0; sampler_index < range->descriptor_count;
                 ++sampler_index) {
               struct terakan_descriptor_set_sampler const * const sampler =
                  &range_set_samplers[sampler_index];
               /* Skip uninitialized (zeroed in descriptor set allocation) samplers, as descriptors
                * may be left uninitialized if they're not statically referenced by the pipeline.
                */
               if (likely(G_03C008_TYPE(sampler->sampler[2]))) {
                  graphics_sampler_setter(&command_writer->hw_state_draw,
                                          range_shader_base + sampler_index, sampler->sampler,
                                          sampler->border_color);
               }
               /* TODO(Triang3l): Unnormalized coordinates on R8xx. */
            }
         }
      }

      set_dynamic_offsets += set_layout->dynamic_offset_count;
   }
}

VkResult
terakan_pipeline_layout_create(struct terakan_device * const device,
                               VkPipelineLayoutCreateInfo const * const create_info,
                               VkShaderStageFlags const stage_mask,
                               struct terakan_pipeline_layout ** const pipeline_layout_out)
{
   assert(!(stage_mask & ~(VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT)));

   VK_MULTIALLOC(multialloc);
   VK_MULTIALLOC_DECL(&multialloc, struct terakan_pipeline_layout, layout, 1);
   VK_MULTIALLOC_DECL(&multialloc, struct terakan_pipeline_layout_set, sets,
                      create_info->setLayoutCount);
   /* Mesa pipeline layout has a different lifetime than the corresponding VkPipelineLayout since
    * other objects hold additional references to them, allocation must be done in the device scope.
    */
   if (vk_pipeline_layout_multizalloc(&device->vk, &multialloc, create_info) == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }
   layout->sets = sets;

   uint8_t next_first_mutable_shader_resources[MESA_SHADER_STAGES] = {};
   uint8_t next_first_shader_uniform_buffers[MESA_SHADER_STAGES] = {};
   uint8_t next_first_shader_samplers[MESA_SHADER_STAGES] = {};

   for (uint32_t set_index = 0; set_index < layout->vk.set_count; ++set_index) {
      struct vk_descriptor_set_layout const * const set_layout_base =
         layout->vk.set_layouts[set_index];
      if (set_layout_base == NULL) {
         continue;
      }
      struct terakan_descriptor_set_layout const * const set_layout =
         container_of(set_layout_base, struct terakan_descriptor_set_layout const, vk);
      struct terakan_pipeline_layout_set * const set = &layout->sets[set_index];

      unsigned remaining_stages = (unsigned)stage_mask;
      while (remaining_stages) {
         unsigned const stage_index = u_bit_scan(&remaining_stages);
         struct terakan_descriptor_set_layout_shader const * const set_layout_shader =
            &set_layout->shaders[stage_index];

         uint8_t const first_mutable_shader_resource =
            next_first_mutable_shader_resources[stage_index];
         if (((VkShaderStageFlags)1 << stage_index == VK_SHADER_STAGE_FRAGMENT_BIT
                 ? TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL
                 : TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL) -
                first_mutable_shader_resource <
             set_layout_shader->resource_count) {
            goto too_many_descriptors;
         }
         set->first_shader_resources[stage_index] =
            TERAKAN_RESOURCE_RANGE_MUTABLE_BASE + first_mutable_shader_resource;
         next_first_mutable_shader_resources[stage_index] += set_layout_shader->resource_count;

         set->first_shader_uniform_buffers[stage_index] =
            next_first_shader_uniform_buffers[stage_index];
         next_first_shader_uniform_buffers[stage_index] += set_layout_shader->uniform_buffer_count;

         uint8_t const first_shader_sampler = next_first_shader_samplers[stage_index];
         if (TERAKAN_SAMPLER_HW_COUNT_PER_STAGE - first_shader_sampler <
             set_layout_shader->sampler_count) {
            goto too_many_descriptors;
         }
         set->first_shader_samplers[stage_index] = first_shader_sampler;
         layout->shader_non_immutable_samplers[stage_index] |=
            set_layout_shader->non_immutable_samplers << first_shader_sampler;
         layout->shader_immutable_samplers_unnormalized_coordinates[stage_index] |=
            set_layout_shader->immutable_samplers_unnormalized_coordinates << first_shader_sampler;
         next_first_shader_samplers[stage_index] += set_layout_shader->sampler_count;
      }
   }

   for (uint32_t push_constant_range_index = 0;
        push_constant_range_index < create_info->pushConstantRangeCount;
        ++push_constant_range_index) {
      VkPushConstantRange const * const push_constant_range =
         &create_info->pPushConstantRanges[push_constant_range_index];
      uint32_t const push_constant_range_extent =
         push_constant_range->offset + push_constant_range->size;
      unsigned remaining_stages = (unsigned)(push_constant_range->stageFlags & stage_mask);
      while (remaining_stages) {
         uint32_t * const shader_app_push_constants_extent =
            &layout->shader_app_push_constants_extents_bytes[u_bit_scan(&remaining_stages)];
         *shader_app_push_constants_extent =
            MAX2(push_constant_range_extent, *shader_app_push_constants_extent);
      }
   }

   *pipeline_layout_out = layout;
   return VK_SUCCESS;

   /* While Vulkan implementations generally shouldn't perform validation, TeraScale has very low
    * binding count limits, while modern games demand many more. If they're launched on Terakan,
    * catch that early and report that instead of proceeding with invalid state. Doing the same for
    * push constants isn't needed as Terakan provides a much larger amount than most other drivers
    * (accurate validation of their limit also would be more complex due to cube array layer counts
    * being passed alongside push constants).
    */
too_many_descriptors:
   vk_pipeline_layout_unref(&device->vk, &layout->vk);
   return vk_errorf(
      device, VK_ERROR_VALIDATION_FAILED_EXT,
      "The application creates a pipeline layout that is too large to fit into the hardware "
      "binding register spaces");
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreatePipelineLayout(VkDevice const deviceHandle,
                             VkPipelineLayoutCreateInfo const * const pCreateInfo,
                             UNUSED VkAllocationCallbacks const * const pAllocator,
                             VkPipelineLayout * const pPipelineLayout)
{
   struct terakan_pipeline_layout * layout;
   VkResult const result = terakan_pipeline_layout_create(
      terakan_device_from_handle(deviceHandle), pCreateInfo,
      VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT, &layout);
   if (result != VK_SUCCESS) {
      return result;
   }
   *pPipelineLayout = terakan_pipeline_layout_to_handle(layout);
   return VK_SUCCESS;
}
