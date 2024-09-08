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
#include "terakan_state.h"

#include <stdint.h>

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetDepthClipNegativeOneToOneEXT(VkCommandBuffer const commandBuffer,
                                           VkBool32 const negativeOneToOne)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(
      state_draw, TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL, &state_draw->pa_cl_clip_cntl,
      TERAKAN_STATE_DRAW_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_PA_CL_CLIP_CNTL_CLEAR,
      terakan_state_draw_depth_clip_negative_one_to_one_pa_cl_clip_cntl(negativeOneToOne));
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
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   if (state_draw->cmd_set_depth_clamp_enable_sets_depth_clip_enable) {
      terakan_state_draw_replace_fields(
         state_draw, TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL, &state_draw->pa_cl_clip_cntl,
         TERAKAN_STATE_DRAW_DEPTH_CLIP_ENABLE_PA_CL_CLIP_CNTL_CLEAR,
         terakan_state_draw_depth_clip_enable_pa_cl_clip_cntl(!depthClampEnable));
   }
   /* TODO(Triang3l): DB_RENDER_OVERRIDE.DISABLE_VIEWPORT_CLAMP. */
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
