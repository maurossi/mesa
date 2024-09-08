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

#include "terakan_push_constants.h"

#include "terakan_command_buffer.h"
#include "terakan_descriptor.h"
#include "terakan_entrypoints.h"

#include "util/bitscan.h"
#include "util/macros.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

void
terakan_push_constants_state_reset(struct terakan_push_constants_state * const state)
{
   state->allocation.mapping_if_up_to_date = NULL;

   state->graphics_stages_using_push_constants = 0;
   state->usage_pre_rasterization = (struct terakan_push_constants_usage){};
   state->usage_fragment = (struct terakan_push_constants_usage){};
   state->usage_compute = (struct terakan_push_constants_usage){};

   state->up_to_date_push_constants_bound_to_stages = 0;

   state->driver_constants_modified = BITFIELD_MASK(TERAKAN_PUSH_CONSTANT_DRIVER_INDEX_COUNT);

   /* For more stable comparisons. */
   memset(&state->driver_constants, 0, sizeof(state->driver_constants));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdPushConstants2KHR(VkCommandBuffer const commandBuffer,
                             VkPushConstantsInfoKHR const * const pPushConstantsInfo)
{
   struct terakan_gfx_command_writer * const command_writer =
      terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx;

   assert(pPushConstantsInfo->offset <= TERAKAN_PUSH_CONSTANTS_APP_SIZE_BYTES);
   assert(pPushConstantsInfo->size <=
          TERAKAN_PUSH_CONSTANTS_APP_SIZE_BYTES - pPushConstantsInfo->offset);

   if (command_writer->push_constants_state.allocation.app_up_to_date_bytes_written >
       pPushConstantsInfo->offset) {
      command_writer->push_constants_state.allocation.mapping_if_up_to_date = NULL;
   }

   memcpy(command_writer->push_constants_state.app_constants + pPushConstantsInfo->offset,
          pPushConstantsInfo->pValues, pPushConstantsInfo->size);
}

void
terakan_push_constants_apply(struct terakan_gfx_command_writer * const command_writer,
                             bool const is_compute)
{
   struct terakan_push_constants_state * const state = &command_writer->push_constants_state;

   struct terakan_push_constants_usage const usage =
      is_compute ? state->usage_compute
                 : terakan_push_constants_usage_union(state->usage_pre_rasterization,
                                                      state->usage_fragment);
   if (terakan_push_constants_usage_empty(usage)) {
      return;
   }
   uint32_t const bytes_used = TERAKAN_PUSH_CONSTANTS_APP_BASE_BYTES + usage.app_extent_bytes;
   uint32_t const kcache_lines_used = DIV_ROUND_UP(bytes_used, TERAKAN_KCACHE_HW_LINE_BYTES);

   if ((state->driver_constants_modified & usage.driver_constants) ||
       state->allocation.size_kcache_lines < kcache_lines_used) {
      state->allocation.mapping_if_up_to_date = NULL;
   }

   if (state->allocation.mapping_if_up_to_date == NULL) {
      state->allocation.mapping_if_up_to_date = terakan_push_buffer_allocate_kcache(
         command_writer->base.command_buffer, TERAKAN_KCACHE_HW_LINE_BYTES * kcache_lines_used,
         &state->allocation.bo, &state->allocation.va_kcache_lines);
      if (unlikely(state->allocation.mapping_if_up_to_date == NULL)) {
         return;
      }
      state->allocation.size_kcache_lines = kcache_lines_used;
      state->allocation.app_up_to_date_bytes_written = 0;

      state->up_to_date_push_constants_bound_to_stages = 0;

      memcpy(state->allocation.mapping_if_up_to_date, &state->driver_constants,
             sizeof(state->driver_constants));
      state->driver_constants_modified = 0;
   }

   /* If merely expanding the used range (binding a pipeline that has a shader stage that previously
    * wasn't used, for instance), but not changing the push constant data itself themselves, it's
    * possible to append the newly used constants to the existing allocation without creating a new
    * one.
    */
   if (usage.app_extent_bytes > state->allocation.app_up_to_date_bytes_written) {
      memcpy(state->allocation.mapping_if_up_to_date + TERAKAN_PUSH_CONSTANTS_APP_BASE_BYTES +
                state->allocation.app_up_to_date_bytes_written,
             state->app_constants + state->allocation.app_up_to_date_bytes_written,
             usage.app_extent_bytes - state->allocation.app_up_to_date_bytes_written);
      state->allocation.app_up_to_date_bytes_written = usage.app_extent_bytes;
   }

   VkShaderStageFlags const bind_to_stages =
      (is_compute ? VK_SHADER_STAGE_COMPUTE_BIT : state->graphics_stages_using_push_constants) &
      ~state->up_to_date_push_constants_bound_to_stages;
   if (bind_to_stages) {
      uint32_t resource[8];
      terakan_descriptor_create_for_uniform_buffer(
         state->allocation.bo,
         TERAKAN_KCACHE_HW_LINE_BYTES * (VkDeviceSize)state->allocation.va_kcache_lines,
         TERAKAN_KCACHE_HW_LINE_BYTES * state->allocation.size_kcache_lines, resource);
      if (is_compute) {
         /* TODO(Triang3l): Bind to the compute stage. */
      } else {
         unsigned remaining_stages = (unsigned)bind_to_stages;
         while (remaining_stages) {
            gl_shader_stage const stage_index = vk_to_mesa_shader_stage(
               (VkShaderStageFlagBits)((VkShaderStageFlags)1 << u_bit_scan(&remaining_stages)));
            terakan_hw_state_draw_set_sq_kcache_for_stage[stage_index](
               &command_writer->hw_state_draw, TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS,
               state->allocation.size_kcache_lines, state->allocation.bo,
               state->allocation.va_kcache_lines);
            terakan_hw_state_draw_set_sq_resource_for_stage[stage_index](
               &command_writer->hw_state_draw, TERAKAN_RESOURCE_RANGE_PUSH_CONSTANTS,
               state->allocation.bo, resource);
         }
      }
      state->up_to_date_push_constants_bound_to_stages |= bind_to_stages;
   }
}
