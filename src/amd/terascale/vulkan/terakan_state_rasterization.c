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

#include "terakan_state_rasterization.h"

#include "terakan_command_buffer.h"
#include "terakan_entrypoints.h"
#include "terakan_image.h"
#include "terakan_state.h"

#include "util/macros.h"
#include "util/u_math.h"
#include "vk_device.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void
terakan_state_draw_viewport_translate(VkViewport const * const viewport,
                                      struct terakan_state_draw_viewport * const hw_viewport_out)
{
   float const width_half = viewport->width * 0.5f;
   float const height_half = viewport->height * 0.5f;

   hw_viewport_out->pa_cl_vport_xy_scale_offset[0][0] = width_half;
   hw_viewport_out->pa_cl_vport_xy_scale_offset[0][1] = viewport->x + width_half;
   hw_viewport_out->pa_cl_vport_xy_scale_offset[1][0] = height_half;
   hw_viewport_out->pa_cl_vport_xy_scale_offset[1][1] = viewport->y + height_half;
   float const z_scale_dx = viewport->maxDepth - viewport->minDepth;
   hw_viewport_out->pa_cl_vport_z_gl_dx_scale_offset[0][0] = z_scale_dx * 0.5f;
   hw_viewport_out->pa_cl_vport_z_gl_dx_scale_offset[0][1] =
      (viewport->maxDepth + viewport->minDepth) * 0.5f;
   hw_viewport_out->pa_cl_vport_z_gl_dx_scale_offset[1][0] = z_scale_dx;
   hw_viewport_out->pa_cl_vport_z_gl_dx_scale_offset[1][1] = viewport->minDepth;

   /* Vulkan viewport height can be negative, but not width. */
   assert(viewport->width >= 0.0f);
   float const right = viewport->x + viewport->width;
   float const y_1 = viewport->y + viewport->height;
   float const top = viewport->height < 0.0f ? y_1 : viewport->y;
   float const bottom = viewport->height < 0.0f ? viewport->y : y_1;

   float const height_half_abs = fabsf(height_half);
   hw_viewport_out->pa_cl_gb_vert_horz_clip_adj[0] =
      (0x1.0p15f + MIN2(top, -bottom) + height_half_abs) / height_half_abs;
   hw_viewport_out->pa_cl_gb_vert_horz_clip_adj[1] =
      (0x1.0p15f + MIN2(viewport->x, -right) + width_half) / width_half;

   /* Calculate the implicit viewport scissor conservatively, rounding the upper bound towards
    * positive infinity (unlike Direct3D, Vulkan doesn't have a concept of implicit viewport scissor
    * with both its bounds rounded towards negative infinity).
    */
   hw_viewport_out->pa_sc_vport_scissor_tl_br_xy[0][0] =
      (uint16_t)CLAMP(viewport->x, 0.0f, (float)TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES);
   hw_viewport_out->pa_sc_vport_scissor_tl_br_xy[0][1] =
      (uint16_t)CLAMP(top, 0.0f, (float)TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES);
   if (unlikely(hw_viewport_out->pa_sc_vport_scissor_tl_br_xy[0][0] >=
                   TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES ||
                hw_viewport_out->pa_sc_vport_scissor_tl_br_xy[0][1] >=
                   TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES)) {
      /* For top-left, the maximum value in the register is 2^n-1, not 2^n. */
      memset(hw_viewport_out->pa_sc_vport_scissor_tl_br_xy, 0,
             sizeof(hw_viewport_out->pa_sc_vport_scissor_tl_br_xy));
   } else {
      hw_viewport_out->pa_sc_vport_scissor_tl_br_xy[1][0] =
         (uint16_t)CLAMP(ceilf(right), 0.0f, (float)TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES);
      hw_viewport_out->pa_sc_vport_scissor_tl_br_xy[1][1] =
         (uint16_t)CLAMP(ceilf(bottom), 0.0f, (float)TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES);
   }

   hw_viewport_out->pa_sc_vport_z_min_max[0] = MIN2(viewport->minDepth, viewport->maxDepth);
   hw_viewport_out->pa_sc_vport_z_min_max[1] = MAX2(viewport->minDepth, viewport->maxDepth);
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetViewport(VkCommandBuffer const commandBuffer, uint32_t const firstViewport,
                       uint32_t const viewportCount, VkViewport const * const pViewports)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   for (uint32_t viewport_relative_index = 0; viewport_relative_index < viewportCount;
        ++viewport_relative_index) {
      uint32_t const viewport_index = firstViewport + viewport_relative_index;
      assert(viewport_index < ARRAY_SIZE(state_draw->viewports));
      struct terakan_state_draw_viewport * const viewport = &state_draw->viewports[viewport_index];
      bool const viewport_is_active = viewport_index < state_draw->viewport_count;
      struct terakan_state_draw_viewport old_viewport;
      if (viewport_is_active) {
         old_viewport = *viewport;
      }
      terakan_state_draw_viewport_translate(&pViewports[viewport_relative_index], viewport);
      if (!viewport_is_active) {
         continue;
      }
      if (memcmp(viewport->pa_cl_vport_xy_scale_offset, old_viewport.pa_cl_vport_xy_scale_offset,
                 sizeof(viewport->pa_cl_vport_xy_scale_offset)) != 0) {
         terakan_state_draw_set_pending(state_draw, TERAKAN_STATE_DRAW_PA_CL_VPORT_XY_SCALE_OFFSET);
      }
      if (memcmp(viewport->pa_cl_vport_z_gl_dx_scale_offset,
                 old_viewport.pa_cl_vport_z_gl_dx_scale_offset,
                 sizeof(viewport->pa_cl_vport_z_gl_dx_scale_offset)) != 0) {
         terakan_state_draw_set_pending(state_draw, TERAKAN_STATE_DRAW_PA_CL_VPORT_Z_SCALE_OFFSET);
      }
      if (memcmp(viewport->pa_cl_gb_vert_horz_clip_adj, old_viewport.pa_cl_gb_vert_horz_clip_adj,
                 sizeof(viewport->pa_cl_gb_vert_horz_clip_adj)) != 0) {
         terakan_state_draw_set_pending(state_draw, TERAKAN_STATE_DRAW_PA_CL_GB);
      }
      if (memcmp(viewport->pa_sc_vport_scissor_tl_br_xy, old_viewport.pa_sc_vport_scissor_tl_br_xy,
                 sizeof(viewport->pa_sc_vport_scissor_tl_br_xy)) != 0) {
         terakan_state_draw_set_pending(state_draw, TERAKAN_STATE_DRAW_PA_SC_VPORT_SCISSOR);
      }
      if (memcmp(viewport->pa_sc_vport_z_min_max, old_viewport.pa_sc_vport_z_min_max,
                 sizeof(viewport->pa_sc_vport_z_min_max)) != 0 &&
          !state_draw->pa_sc_vport_z_min_0_max_1) {
         terakan_state_draw_set_pending(state_draw, TERAKAN_STATE_DRAW_PA_SC_VPORT_Z_MIN_MAX);
      }
   }
}

void
terakan_state_draw_set_viewport_count(struct terakan_state_draw * const state,
                                      uint32_t const viewport_count)
{
   assert(viewport_count <= ARRAY_SIZE(state->viewports));
   if (viewport_count == state->viewport_count) {
      return;
   }
   if (viewport_count > state->viewport_count) {
      terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_PA_CL_VPORT_XY_SCALE_OFFSET);
      terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_PA_CL_VPORT_Z_SCALE_OFFSET);
      terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_PA_SC_VPORT_SCISSOR);
      /* Make sure either the actual Z min / max or 0 to 1 is applied to the new viewports depending
       * on the pa_sc_vport_z_min_0_max_1 setting.
       */
      terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_PA_SC_VPORT_Z_MIN_MAX);
   }
   terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_PA_CL_GB);
   state->viewport_count = viewport_count;
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetViewportWithCount(VkCommandBuffer const commandBuffer, uint32_t const viewportCount,
                                VkViewport const * const pViewports)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_set_viewport_count(state_draw, viewportCount);
   terakan_CmdSetViewport(commandBuffer, 0, viewportCount, pViewports);
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetScissor(VkCommandBuffer const commandBuffer, uint32_t const firstScissor,
                      uint32_t const scissorCount, VkRect2D const * const pScissors)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   for (uint32_t scissor_index = 0; scissor_index < scissorCount; ++scissor_index) {
      uint32_t const viewport_index = firstScissor + scissor_index;
      assert(viewport_index < ARRAY_SIZE(state_draw->viewports));
      uint16_t * const viewport_scissor =
         state_draw->pa_sc_vport_generic_scissor_tl_br_xy[viewport_index][0];
      uint16_t scissor[4];
      terakan_state_translate_window_rectangle_unpacked(&pScissors[scissor_index], scissor);
      if (viewport_index < state_draw->viewport_count &&
          memcmp(viewport_scissor, scissor, sizeof(scissor)) != 0) {
         terakan_state_draw_set_pending(state_draw, TERAKAN_STATE_DRAW_PA_SC_VPORT_SCISSOR);
      }
      memcpy(viewport_scissor, scissor, sizeof(scissor));
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetScissorWithCount(VkCommandBuffer const commandBuffer, uint32_t const scissorCount,
                               VkRect2D const * const pScissors)
{
   terakan_CmdSetScissor(commandBuffer, 0, scissorCount, pScissors);
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetDepthClipNegativeOneToOneEXT(VkCommandBuffer const commandBuffer,
                                           VkBool32 const negativeOneToOne)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   uint32_t const dx_clip_space_def = !negativeOneToOne;
   if (G_028810_DX_CLIP_SPACE_DEF(state_draw->pa_cl_clip_cntl) != dx_clip_space_def) {
      terakan_state_draw_set_pending(state_draw, TERAKAN_STATE_DRAW_PA_CL_VPORT_Z_SCALE_OFFSET);
   }
   terakan_state_draw_replace_fields(state_draw, TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL,
                                     &state_draw->pa_cl_clip_cntl, C_028810_DX_CLIP_SPACE_DEF,
                                     S_028810_DX_CLIP_SPACE_DEF(dx_clip_space_def));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetRasterizerDiscardEnable(VkCommandBuffer const commandBuffer,
                                      VkBool32 const rasterizerDiscardEnable)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(
      state_draw, TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL, &state_draw->pa_cl_clip_cntl,
      TERAKAN_STATE_DRAW_RASTERIZER_DISCARD_ENABLE_PA_CL_CLIP_CNTL_CLEAR,
      terakan_state_draw_rasterizer_discard_enable_pa_cl_clip_cntl(rasterizerDiscardEnable));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetDepthClampEnableEXT(VkCommandBuffer const commandBuffer,
                                  VkBool32 const depthClampEnable)
{
   struct terakan_command_buffer const * const command_buffer =
      terakan_command_buffer_from_handle(commandBuffer);
   struct terakan_state_draw * const state_draw = &command_buffer->command_writer.gfx->state_draw;
   bool const depth_clamp_disable = !depthClampEnable;
   if (state_draw->cmd_set_depth_clamp_enable_sets_depth_clip_enable) {
      terakan_state_draw_replace_fields(
         state_draw, TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL, &state_draw->pa_cl_clip_cntl,
         TERAKAN_STATE_DRAW_DEPTH_CLIP_ENABLE_PA_CL_CLIP_CNTL_CLEAR,
         terakan_state_draw_depth_clip_enable_pa_cl_clip_cntl(depth_clamp_disable));
   }
   if (command_buffer->vk.pool->base.device->enabled_extensions.EXT_depth_range_unrestricted) {
      terakan_state_draw_replace_fields(
         state_draw, TERAKAN_STATE_DRAW_DB_RENDER_OVERRIDE, &state_draw->db_render_override,
         C_02800C_DISABLE_VIEWPORT_CLAMP, S_02800C_DISABLE_VIEWPORT_CLAMP(depth_clamp_disable));
   } else {
      if (state_draw->pa_sc_vport_z_min_0_max_1 != depth_clamp_disable) {
         state_draw->pa_sc_vport_z_min_0_max_1 = depth_clamp_disable;
         terakan_state_draw_set_pending(state_draw,
                                        TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_SC_VPORT_Z_MIN_MAX);
      }
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetDepthClipEnableEXT(VkCommandBuffer const commandBuffer,
                                 VkBool32 const depthClipEnable)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(
      state_draw, TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL, &state_draw->pa_cl_clip_cntl,
      TERAKAN_STATE_DRAW_DEPTH_CLIP_ENABLE_PA_CL_CLIP_CNTL_CLEAR,
      terakan_state_draw_depth_clip_enable_pa_cl_clip_cntl(depthClipEnable));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetPolygonModeEXT(VkCommandBuffer const commandBuffer, VkPolygonMode const polygonMode)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(
      state_draw, TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL, &state_draw->pa_su_sc_mode_cntl,
      TERAKAN_STATE_DRAW_POLYGON_MODE_PA_SU_SC_MODE_CNTL_CLEAR,
      terakan_state_draw_polygon_mode_pa_su_sc_mode_cntl(polygonMode));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetCullMode(VkCommandBuffer const commandBuffer, VkCullModeFlags const cullMode)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(state_draw, TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL,
                                     &state_draw->pa_su_sc_mode_cntl,
                                     TERAKAN_STATE_DRAW_CULL_MODE_PA_SU_SC_MODE_CNTL_CLEAR,
                                     terakan_state_draw_cull_mode_pa_su_sc_mode_cntl(cullMode));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetFrontFace(VkCommandBuffer const commandBuffer, VkFrontFace const frontFace)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(state_draw, TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL,
                                     &state_draw->pa_su_sc_mode_cntl,
                                     TERAKAN_STATE_DRAW_FRONT_FACE_PA_SU_SC_MODE_CNTL_CLEAR,
                                     terakan_state_draw_front_face_pa_su_sc_mode_cntl(frontFace));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetProvokingVertexModeEXT(VkCommandBuffer const commandBuffer,
                                     VkProvokingVertexModeEXT const provokingVertexMode)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(
      state_draw, TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL, &state_draw->pa_su_sc_mode_cntl,
      TERAKAN_STATE_DRAW_PROVOKING_VERTEX_MODE_PA_SU_SC_MODE_CNTL_CLEAR,
      terakan_state_draw_provoking_vertex_mode_pa_su_sc_mode_cntl(provokingVertexMode));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetDepthBiasEnable(VkCommandBuffer const commandBuffer, VkBool32 const depthBiasEnable)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(
      state_draw, TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL, &state_draw->pa_su_sc_mode_cntl,
      TERAKAN_STATE_DRAW_DEPTH_BIAS_ENABLE_PA_SU_SC_MODE_CNTL_CLEAR,
      terakan_state_draw_depth_bias_enable_pa_su_sc_mode_cntl(depthBiasEnable));
}
