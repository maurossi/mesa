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

#include "terakan_descriptor_set_layout.h"

#include "terakan_descriptor.h"
#include "terakan_descriptor_set.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_sampler.h"

#include "util/bitscan.h"
#include "util/macros.h"
#include "util/u_math.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int
terakan_descriptor_set_layout_compare_binding_create_infos(void const * const a,
                                                           void const * const b)
{
   VkDescriptorSetLayoutBinding const * const a_binding = (VkDescriptorSetLayoutBinding const *)a;
   VkDescriptorSetLayoutBinding const * const b_binding = (VkDescriptorSetLayoutBinding const *)b;
   /* Move all empty bindings to the end to easily ignore them. */
   if (a_binding->descriptorCount != 0 && b_binding->descriptorCount == 0) {
      return -1;
   }
   if (b_binding->descriptorCount != 0 && a_binding->descriptorCount == 0) {
      return 1;
   }
   if (a_binding->binding < b_binding->binding) {
      return -1;
   }
   if (b_binding->binding < a_binding->binding) {
      return 1;
   }
   return 0;
}

/* Try to combine the previous range and the new one to make binding slightly faster. */
static bool
terakan_descriptor_set_layout_shader_range_try_extend(
   struct terakan_descriptor_set_layout_shader_range * const range,
   struct terakan_descriptor_set_layout_shader_range const * const extension)
{
   if (range->first_set_descriptor + range->descriptor_count != extension->first_set_descriptor ||
       range->first_shader_descriptor + range->descriptor_count !=
          extension->first_shader_descriptor ||
       ((range->first_immutable_sampler_or_dynamic_offset != UINT16_MAX) !=
        (extension->first_immutable_sampler_or_dynamic_offset != UINT16_MAX)) ||
       (range->first_immutable_sampler_or_dynamic_offset != UINT16_MAX &&
        range->first_immutable_sampler_or_dynamic_offset + range->descriptor_count !=
           extension->first_immutable_sampler_or_dynamic_offset)) {
      return false;
   }
   range->descriptor_count += extension->descriptor_count;
   return true;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreateDescriptorSetLayout(VkDevice const deviceHandle,
                                  VkDescriptorSetLayoutCreateInfo const * const pCreateInfo,
                                  UNUSED VkAllocationCallbacks const * const pAllocator,
                                  VkDescriptorSetLayout * const pSetLayout)
{
   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   /* Sort bindings by their numbers for pipeline layout compatibility and dynamic offset indexing
    * purposes, and also use the sorting to move empty bindings to the end.
    */
   VkDescriptorSetLayoutBinding * sorted_create_info_bindings = NULL;
   if (pCreateInfo->bindingCount != 0) {
      size_t const sorted_create_info_bindings_size =
         sizeof(VkDescriptorSetLayoutBinding) * pCreateInfo->bindingCount;
      sorted_create_info_bindings =
         vk_alloc2(&device->vk.alloc, pAllocator, sorted_create_info_bindings_size,
                   alignof(VkDescriptorSetLayoutBinding), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
      if (sorted_create_info_bindings == NULL) {
         return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
      }
      memcpy(sorted_create_info_bindings, pCreateInfo->pBindings, sorted_create_info_bindings_size);
      qsort(sorted_create_info_bindings, pCreateInfo->bindingCount,
            sizeof(VkDescriptorSetLayoutBinding),
            terakan_descriptor_set_layout_compare_binding_create_infos);
   }

   /* Skip empty bindings. */
   uint32_t non_empty_create_info_binding_count = pCreateInfo->bindingCount;
   while (non_empty_create_info_binding_count != 0 &&
          sorted_create_info_bindings[non_empty_create_info_binding_count - 1].descriptorCount ==
             0) {
      --non_empty_create_info_binding_count;
   }

   /* VK_SHADER_STAGE_ALL includes bits other than the actually supported stages, mask them out.
    * Also skip binding logic for stages never needed by the application if it uses
    * VK_SHADER_STAGE_ALL.
    */
   VkShaderStageFlags stage_mask =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
   if (device->vk.enabled_features.geometryShader) {
      stage_mask |= VK_SHADER_STAGE_GEOMETRY_BIT;
   }
   if (device->vk.enabled_features.tessellationShader) {
      stage_mask |=
         VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
   }

   /* Calculate the sizes of the suballocations, as well as masks of shader stages requiring any
    * descriptors of a given type.
    */
   size_t binding_count = 0;
   uint8_t immutable_sampler_count = 0;
   uint16_t shader_range_count = 0;
   VkShaderStageFlags stages_with_resources = 0, stages_with_samplers = 0;
   for (uint32_t create_info_binding_index = 0;
        create_info_binding_index < non_empty_create_info_binding_count;
        ++create_info_binding_index) {
      VkDescriptorSetLayoutBinding const * const binding =
         &sorted_create_info_bindings[create_info_binding_index];
      if (create_info_binding_index != 0 &&
          binding->binding == sorted_create_info_bindings[create_info_binding_index - 1].binding) {
         vk_free2(&device->vk.alloc, pAllocator, sorted_create_info_bindings);
         return vk_errorf(
            device, VK_ERROR_VALIDATION_FAILED_EXT,
            "Descriptor set layout has multiple create infos for the same binding number");
      }
      binding_count = (size_t)binding->binding + 1;
      /* Coarsely validate the binding count against the numeric limit. */
      if (binding->descriptorCount >= MAX3(TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL,
                                           TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL,
                                           TERAKAN_SAMPLERS_PER_STAGE)) {
         goto too_many_descriptors;
      }
      uint8_t binding_shader_range_count = 0;
      VkDescriptorType const binding_type = binding->descriptorType;
      VkShaderStageFlagBits const binding_stages = binding->stageFlags & stage_mask;
      if (terakan_descriptor_type_has_resource(binding_type)) {
         ++binding_shader_range_count;
         stages_with_resources |= binding_stages;
      }
      if (terakan_descriptor_type_has_sampler(binding_type)) {
         if (binding->pImmutableSamplers != NULL) {
            if (TERAKAN_SAMPLERS_PER_STAGE * MESA_SHADER_STAGES - immutable_sampler_count <
                binding->descriptorCount) {
               goto too_many_descriptors;
            }
            immutable_sampler_count += binding->descriptorCount;
         }
         ++binding_shader_range_count;
         stages_with_samplers |= binding_stages;
      }
      shader_range_count += binding_shader_range_count * util_bitcount((unsigned)binding_stages);
   }

   /* Ordered by access frequency in the allocation:
    * - Shader ranges - primarily for binding.
    * - Immutable samplers - primarily for binding, needed by shader ranges conditionally.
    * - Bindings - primarily for shader compilation and writing.
    */
   VK_MULTIALLOC(multialloc);
   VK_MULTIALLOC_DECL(&multialloc, struct terakan_descriptor_set_layout, layout, 1);
   VK_MULTIALLOC_DECL(&multialloc, struct terakan_descriptor_set_layout_shader_range, shader_ranges,
                      shader_range_count);
   VK_MULTIALLOC_DECL(&multialloc, struct terakan_sampler const *, immutable_samplers,
                      immutable_sampler_count);
   VK_MULTIALLOC_DECL(&multialloc, struct terakan_descriptor_set_layout_binding, bindings,
                      binding_count);
   /* Mesa descriptor set layout has a different lifetime than the corresponding
    * VkDescriptorSetLayout since other objects hold additional references to them, allocation must
    * be done in the device scope.
    */
   if (vk_descriptor_set_layout_multizalloc(&device->vk, &multialloc) == NULL) {
      vk_free2(&device->vk.alloc, pAllocator, sorted_create_info_bindings);
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }
   layout->immutable_samplers = immutable_samplers;
   layout->shader_ranges = shader_ranges;
   layout->binding_count = binding_count;
   layout->bindings = bindings;

   /* Set up the layout of the bindings inside the set in descriptor pools, and also immutable
    * samplers.
    */
   uint8_t next_immutable_sampler_index = 0;
   uint16_t dynamic_offset_count = 0;
   uint16_t set_rat_count = 0;
   uint16_t set_resource_count = 0;
   uint8_t set_sampler_count = 0;
   for (uint32_t create_info_binding_index = 0;
        create_info_binding_index < non_empty_create_info_binding_count;
        ++create_info_binding_index) {
      VkDescriptorSetLayoutBinding const * const create_info_binding =
         &sorted_create_info_bindings[create_info_binding_index];
      struct terakan_descriptor_set_layout_binding * const layout_binding =
         &layout->bindings[create_info_binding->binding];

      VkDescriptorType const binding_type = create_info_binding->descriptorType;
      layout_binding->descriptor_type = binding_type;

      uint32_t const binding_descriptor_count = create_info_binding->descriptorCount;
      layout_binding->descriptor_count = binding_descriptor_count;

      layout_binding->first_immutable_sampler_or_dynamic_offset = UINT16_MAX;

      bool const binding_has_samplers = terakan_descriptor_type_has_sampler(binding_type);
      if (binding_has_samplers) {
         assert(immutable_sampler_count - next_immutable_sampler_index >= binding_descriptor_count);
         layout_binding->first_immutable_sampler_or_dynamic_offset = next_immutable_sampler_index;
         next_immutable_sampler_index += binding_descriptor_count;
         for (uint32_t immutable_sampler_index = 0;
              immutable_sampler_index < binding_descriptor_count; ++immutable_sampler_index) {
            struct terakan_sampler const * const immutable_sampler = terakan_sampler_from_handle(
               create_info_binding->pImmutableSamplers[immutable_sampler_index]);
            layout->immutable_samplers[layout_binding->first_immutable_sampler_or_dynamic_offset +
                                       immutable_sampler_index] = immutable_sampler;
            if (immutable_sampler->unnormalized_coordinates) {
               layout_binding->immutable_samplers_unnormalized_coordinates |=
                  (uint32_t)1 << immutable_sampler_index;
            }
         }
      } else if (binding_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
                 binding_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
         layout_binding->first_immutable_sampler_or_dynamic_offset = dynamic_offset_count;
         dynamic_offset_count += binding_descriptor_count;
      }

      /* Write the offsets used in descriptor updating regardless of the descriptor type.
       * This isn't necessary due to the requirements for consecutive bindings in
       * vkUpdateDescriptorSets, but trivially allows for more graceful handling of invalid usage.
       */
      layout_binding->first_set_rat = set_rat_count;
      layout_binding->first_set_resource = set_resource_count;
      layout_binding->first_set_sampler = set_sampler_count;

      /* Add to the counts, validating against the numeric limits. */
      if (terakan_descriptor_type_has_rat(binding_type)) {
         if (TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_IN_PIPELINE - set_rat_count <
             binding_descriptor_count) {
            goto too_many_descriptors_destroy;
         }
         set_rat_count += binding_descriptor_count;
      }
      if (terakan_descriptor_type_has_resource(binding_type)) {
         if (TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_IN_PIPELINE - set_resource_count <
             binding_descriptor_count) {
            goto too_many_descriptors_destroy;
         }
         set_resource_count += binding_descriptor_count;
      }
      if (binding_has_samplers &&
          layout_binding->first_immutable_sampler_or_dynamic_offset == UINT16_MAX) {
         if (TERAKAN_SAMPLERS_PER_STAGE * MESA_SHADER_STAGES - set_sampler_count <
             binding_descriptor_count) {
            goto too_many_descriptors_destroy;
         }
         set_sampler_count += binding_descriptor_count;
      }

      layout_binding->stage_flags = (uint8_t)(create_info_binding->stageFlags & stage_mask);
   }
   assert(next_immutable_sampler_index == immutable_sampler_count);

   layout->pool_first_sampler_offset_bytes =
      sizeof(struct terakan_descriptor_set_resource) * set_resource_count;
   layout->pool_first_rat_offset_bytes =
      layout->pool_first_sampler_offset_bytes +
      sizeof(struct terakan_descriptor_set_sampler) * set_sampler_count;
   layout->pool_size_bytes = layout->pool_first_rat_offset_bytes +
                             sizeof(struct terakan_descriptor_set_rat) * set_rat_count;

   layout->dynamic_offset_count = dynamic_offset_count;

   /* Set up the layout for each shader stage. */

   uint16_t next_shader_range_index = 0;

   unsigned remaining_stages = (unsigned)stages_with_resources;
   while (remaining_stages) {
      int const stage_index = u_bit_scan(&remaining_stages);

      struct terakan_descriptor_set_layout_shader * const layout_shader =
         &layout->shaders[stage_index];
      layout_shader->first_resource_range = next_shader_range_index;

      uint8_t stage_resource_count = 0;

      uint8_t const stage_flag = (uint8_t)1 << stage_index;

      for (uint32_t create_info_binding_index = 0;
           create_info_binding_index < non_empty_create_info_binding_count;
           ++create_info_binding_index) {
         struct terakan_descriptor_set_layout_binding * const binding =
            &layout->bindings[sorted_create_info_bindings[create_info_binding_index].binding];

         if (!(binding->stage_flags & stage_flag) ||
             !terakan_descriptor_type_has_resource(binding->descriptor_type)) {
            continue;
         }

         if ((stage_flag == VK_SHADER_STAGE_FRAGMENT_BIT
                 ? TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL
                 : TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL) -
                stage_resource_count <
             binding->descriptor_count) {
            goto too_many_descriptors_destroy;
         }

         binding->first_shader_resources[stage_index] = stage_resource_count;

         assert(next_shader_range_index < shader_range_count);
         struct terakan_descriptor_set_layout_shader_range * const shader_range =
            &layout->shader_ranges[next_shader_range_index];
         shader_range->first_set_descriptor = binding->first_set_resource;
         shader_range->first_immutable_sampler_or_dynamic_offset =
            binding->first_immutable_sampler_or_dynamic_offset;
         shader_range->first_shader_descriptor = stage_resource_count;
         shader_range->descriptor_count = binding->descriptor_count;
         if (next_shader_range_index == layout_shader->first_resource_range ||
             !terakan_descriptor_set_layout_shader_range_try_extend(
                &layout->shader_ranges[next_shader_range_index - 1], shader_range)) {
            ++next_shader_range_index;
         }

         stage_resource_count += binding->descriptor_count;
      }

      layout_shader->resource_range_count =
         next_shader_range_index - layout_shader->first_resource_range;

      layout_shader->resource_count = stage_resource_count;
   }

   remaining_stages = (unsigned)stages_with_samplers;
   while (remaining_stages) {
      int const stage_index = u_bit_scan(&remaining_stages);

      struct terakan_descriptor_set_layout_shader * const layout_shader =
         &layout->shaders[stage_index];
      layout_shader->first_sampler_range = next_shader_range_index;

      uint8_t stage_sampler_count = 0;

      uint8_t const stage_flag = (uint8_t)1 << stage_index;

      for (uint32_t create_info_binding_index = 0;
           create_info_binding_index < non_empty_create_info_binding_count;
           ++create_info_binding_index) {
         struct terakan_descriptor_set_layout_binding * const binding =
            &layout->bindings[sorted_create_info_bindings[create_info_binding_index].binding];

         if (!(binding->stage_flags & stage_flag) ||
             !terakan_descriptor_type_has_sampler(binding->descriptor_type)) {
            continue;
         }

         if (TERAKAN_SAMPLERS_PER_STAGE - stage_sampler_count < binding->descriptor_count) {
            goto too_many_descriptors_destroy;
         }

         binding->first_shader_samplers[stage_index] = stage_sampler_count;

         assert(next_shader_range_index < shader_range_count);
         struct terakan_descriptor_set_layout_shader_range * const shader_range =
            &layout->shader_ranges[next_shader_range_index];
         shader_range->first_set_descriptor = binding->first_set_sampler;
         shader_range->first_immutable_sampler_or_dynamic_offset =
            binding->first_immutable_sampler_or_dynamic_offset;
         shader_range->first_shader_descriptor = stage_sampler_count;
         shader_range->descriptor_count = binding->descriptor_count;
         if (next_shader_range_index == layout_shader->first_sampler_range ||
             !terakan_descriptor_set_layout_shader_range_try_extend(
                &layout->shader_ranges[next_shader_range_index - 1], shader_range)) {
            ++next_shader_range_index;
         }

         if (binding->first_immutable_sampler_or_dynamic_offset != UINT16_MAX) {
            layout_shader->immutable_samplers_unnormalized_coordinates |=
               binding->immutable_samplers_unnormalized_coordinates << stage_sampler_count;
         } else {
            layout_shader->non_immutable_samplers |=
               (((uint32_t)1 << binding->descriptor_count) - 1) << stage_sampler_count;
         }

         stage_sampler_count += binding->descriptor_count;
      }

      layout_shader->sampler_range_count =
         next_shader_range_index - layout_shader->first_sampler_range;

      layout_shader->sampler_count = stage_sampler_count;
   }

   assert(next_shader_range_index == shader_range_count);

   vk_free2(&device->vk.alloc, pAllocator, sorted_create_info_bindings);

   *pSetLayout = terakan_descriptor_set_layout_to_handle(layout);
   return VK_SUCCESS;

   /* While Vulkan implementations generally shouldn't perform validation, TeraScale has very low
    * binding count limits, while modern games demand many more. If they're launched on Terakan,
    * catch that early and report that instead of proceeding with invalid state.
    */
too_many_descriptors_destroy:
   vk_descriptor_set_layout_unref(&device->vk, &layout->vk);
too_many_descriptors:
   vk_free2(&device->vk.alloc, pAllocator, sorted_create_info_bindings);
   return vk_errorf(
      device, VK_ERROR_VALIDATION_FAILED_EXT,
      "The application creates a descriptor set layout that is too large to fit into the hardware "
      "binding register spaces");
}
