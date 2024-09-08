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

#include "terakan_pipeline_layout.h"

#include "terakan_descriptor.h"
#include "terakan_descriptor_set_layout.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"

#include "util/bitscan.h"
#include "util/macros.h"
#include "util/u_math.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

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

         uint8_t const first_shader_sampler = next_first_shader_samplers[stage_index];
         if (TERAKAN_SAMPLERS_PER_STAGE - first_shader_sampler < set_layout_shader->sampler_count) {
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
         uint32_t * const shader_push_constant_extent =
            &layout->shader_push_constant_extents_bytes[u_bit_scan(&remaining_stages)];
         *shader_push_constant_extent =
            MAX2(push_constant_range_extent, *shader_push_constant_extent);
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
