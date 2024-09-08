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

#include "terakan_meta.h"

#include "terakan_device.h"

#include "util/macros.h"

#include <assert.h>
#include <stdbool.h>

struct terakan_meta_shader const * const terakan_meta_shaders[TERAKAN_META_SHADER_COUNT] = {
   [TERAKAN_META_SHADER_POSITION_FROM_INDEX_VS] = &terakan_meta_position_from_index_vs,
   [TERAKAN_META_SHADER_COPY_BUFFER_TO_IMAGE_PS] = &terakan_meta_copy_buffer_to_image_ps,
};

void
terakan_meta_modify_state_draw_dword(struct terakan_gfx_command_writer * const command_writer,
                                     enum terakan_state_draw_index const invalidate_state_index,
                                     enum terakan_hw_state_draw_index const hw_state_index,
                                     uint32_t * const hw_state_item, uint32_t const value)
{
   terakan_state_draw_set_pending(&command_writer->state_draw, invalidate_state_index);
   bool const modified = *hw_state_item != value;
   *hw_state_item = value;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw, hw_state_index, modified);
}

void
terakan_meta_set_vs(struct terakan_gfx_command_writer * const command_writer,
                    enum terakan_meta_shader_index const shader_index)
{
   struct terakan_device const * const device = container_of(
      command_writer->base.command_buffer->vk.base.device, struct terakan_device const, vk);

   struct terakan_shader_static const * const shader_static = &device->meta_shaders[shader_index];

   if (BITSET_TEST(command_writer->hw_state_draw.state_ever_written,
                   TERAKAN_HW_STATE_DRAW_SQ_PGM_VS) &&
       command_writer->hw_state_draw.sq_pgm_vs == shader_static) {
      /* If this shader was set via this function previously, everything else set by this function
       * must still be up to date.
       */
      return;
   }

   command_writer->hw_state_draw.sq_pgm_vs = shader_static;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw, TERAKAN_HW_STATE_DRAW_SQ_PGM_VS,
                                 true);

   struct terakan_meta_shader const * const shader = terakan_meta_shaders[shader_index];

   terakan_hw_state_draw_set_sq_constants_needed_by_vs(
      &command_writer->hw_state_draw, shader->constant_cache_needed, shader->resources_needed,
      VK_SHADER_STAGE_FRAGMENT_BIT);
}

void
terakan_meta_set_ps(struct terakan_gfx_command_writer * const command_writer,
                    enum terakan_meta_shader_index const shader_index)
{
   struct terakan_device const * const device = container_of(
      command_writer->base.command_buffer->vk.base.device, struct terakan_device const, vk);

   struct terakan_shader_static const * const shader_static = &device->meta_shaders[shader_index];

   if (BITSET_TEST(command_writer->hw_state_draw.state_ever_written,
                   TERAKAN_HW_STATE_DRAW_SQ_PGM_PS) &&
       command_writer->hw_state_draw.sq_pgm_ps == shader_static) {
      /* If this shader was set via this function previously, everything else set by this function
       * must still be up to date.
       */
      return;
   }

   command_writer->hw_state_draw.sq_pgm_ps = shader_static;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw, TERAKAN_HW_STATE_DRAW_SQ_PGM_PS,
                                 true);

   struct terakan_meta_shader const * const shader = terakan_meta_shaders[shader_index];

   terakan_hw_state_draw_set_sq_constants_needed_by_fs(
      &command_writer->hw_state_draw, shader->constant_cache_needed, shader->resources_needed);
}
