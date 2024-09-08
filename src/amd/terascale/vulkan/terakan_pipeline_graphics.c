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

#include "terakan_pipeline_graphics.h"

#include "terakan_command_buffer.h"
#include "terakan_state.h"
#include "terakan_state_input_assembly.h"
#include "terakan_state_rasterization.h"

#include "vk_graphics_state.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*terakan_pipeline_graphics_apply_state_function)(
   struct terakan_gfx_command_writer * command_writer,
   struct terakan_pipeline_graphics const * pipeline,
   enum terakan_pipeline_graphics_state_index state_index);

static void
terakan_pipeline_graphics_apply_vgt_primitive_type(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   command_writer->state_draw.vgt_primitive_type = pipeline->vertex_input.vgt_primitive_type;
   terakan_state_draw_written(&command_writer->state_draw, TERAKAN_STATE_DRAW_VGT_PRIMITIVE_TYPE);
}

static void
terakan_pipeline_graphics_apply_pa_cl_clip_cntl(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   terakan_state_draw_replace_fields(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL,
                                     &command_writer->state_draw.pa_cl_clip_cntl,
                                     pipeline->pre_rasterization.pa_cl_clip_cntl_clear,
                                     pipeline->pre_rasterization.pa_cl_clip_cntl);
}

static void
terakan_pipeline_graphics_apply_pa_su_sc_mode_cntl(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   terakan_state_draw_replace_fields(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL,
                                     &command_writer->state_draw.pa_su_sc_mode_cntl,
                                     pipeline->pre_rasterization.pa_su_sc_mode_cntl_clear,
                                     pipeline->pre_rasterization.pa_su_sc_mode_cntl);
}

static void
terakan_pipeline_graphics_apply_pa_sc_aa_mask(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   command_writer->state_draw.pa_sc_aa_mask = pipeline->multisample.pa_sc_aa_mask;
   terakan_state_draw_written(&command_writer->state_draw, TERAKAN_STATE_DRAW_PA_SC_AA_MASK);
}

static terakan_pipeline_graphics_apply_state_function const
   terakan_pipeline_graphics_apply_state_functions[TERAKAN_PIPELINE_GRAPHICS_STATE_COUNT] = {
      [TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE] =
         terakan_pipeline_graphics_apply_vgt_primitive_type,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL] =
         terakan_pipeline_graphics_apply_pa_cl_clip_cntl,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_SC_MODE_CNTL] =
         terakan_pipeline_graphics_apply_pa_su_sc_mode_cntl,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK] =
         terakan_pipeline_graphics_apply_pa_sc_aa_mask,
};

static void
terakan_pipeline_graphics_bind(struct terakan_gfx_command_writer * const command_writer,
                               struct terakan_pipeline_graphics const * const pipeline)
{
   command_writer->state_draw.cmd_set_depth_clamp_enable_sets_depth_clip_enable =
      pipeline->pre_rasterization.cmd_set_depth_clamp_enable_sets_depth_clip_enable;

   unsigned state_index;
   BITSET_FOREACH_SET(state_index, pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_COUNT)
   {
      terakan_pipeline_graphics_apply_state_functions[state_index](
         command_writer, pipeline, (enum terakan_state_draw_index)state_index);
   }
}

static VkResult
terakan_pipeline_graphics_vertex_input_init(struct terakan_pipeline_graphics * const pipeline,
                                            struct vk_graphics_pipeline_state const * const state)
{
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE */
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_IA_PRIMITIVE_TOPOLOGY)) {
      pipeline->vertex_input.vgt_primitive_type =
         terakan_state_draw_primitive_topology_vgt_primitive_type(state->ia->primitive_topology);
      BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE);
   }

   return VK_SUCCESS;
}

static VkResult
terakan_pipeline_graphics_pre_rasterization_init(
   struct terakan_pipeline_graphics * const pipeline,
   struct vk_graphics_pipeline_state const * const state)
{
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL */
   pipeline->pre_rasterization.pa_cl_clip_cntl_clear = UINT32_MAX;
   pipeline->pre_rasterization.pa_cl_clip_cntl = 0;
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_VP_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE)) {
      pipeline->pre_rasterization.pa_cl_clip_cntl_clear &=
         TERAKAN_STATE_DRAW_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_PA_CL_CLIP_CNTL_CLEAR;
      pipeline->pre_rasterization.pa_cl_clip_cntl |=
         terakan_state_draw_depth_clip_negative_one_to_one_pa_cl_clip_cntl(
            state->vp->depth_clip_negative_one_to_one);
   }
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_RASTERIZER_DISCARD_ENABLE)) {
      pipeline->pre_rasterization.pa_cl_clip_cntl_clear &=
         TERAKAN_STATE_DRAW_RASTERIZER_DISCARD_ENABLE_PA_CL_CLIP_CNTL_CLEAR;
      pipeline->pre_rasterization.pa_cl_clip_cntl |=
         terakan_state_draw_rasterizer_discard_enable_pa_cl_clip_cntl(
            state->rs->rasterizer_discard_enable);
   }
   pipeline->pre_rasterization.cmd_set_depth_clamp_enable_sets_depth_clip_enable = false;
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_DEPTH_CLIP_ENABLE)) {
      if (state->rs->depth_clip_enable == VK_MESA_DEPTH_CLIP_ENABLE_NOT_CLAMP) {
         pipeline->pre_rasterization.cmd_set_depth_clamp_enable_sets_depth_clip_enable = true;
         if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_DEPTH_CLAMP_ENABLE)) {
            pipeline->pre_rasterization.pa_cl_clip_cntl_clear &=
               TERAKAN_STATE_DRAW_DEPTH_CLIP_ENABLE_PA_CL_CLIP_CNTL_CLEAR;
            pipeline->pre_rasterization.pa_cl_clip_cntl |=
               terakan_state_draw_depth_clip_enable_pa_cl_clip_cntl(!state->rs->depth_clamp_enable);
         }
      } else {
         pipeline->pre_rasterization.pa_cl_clip_cntl_clear &=
            TERAKAN_STATE_DRAW_DEPTH_CLIP_ENABLE_PA_CL_CLIP_CNTL_CLEAR;
         pipeline->pre_rasterization.pa_cl_clip_cntl |=
            terakan_state_draw_depth_clip_enable_pa_cl_clip_cntl(state->rs->depth_clip_enable ==
                                                                 VK_MESA_DEPTH_CLIP_ENABLE_TRUE);
      }
   }
   assert(!(pipeline->pre_rasterization.pa_cl_clip_cntl &
            pipeline->pre_rasterization.pa_cl_clip_cntl_clear));
   if (pipeline->pre_rasterization.pa_cl_clip_cntl_clear != UINT32_MAX) {
      BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL);
   }

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_SC_MODE_CNTL */
   pipeline->pre_rasterization.pa_su_sc_mode_cntl_clear = UINT32_MAX;
   pipeline->pre_rasterization.pa_su_sc_mode_cntl = 0;
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_POLYGON_MODE)) {
      pipeline->pre_rasterization.pa_su_sc_mode_cntl_clear &=
         TERAKAN_STATE_DRAW_POLYGON_MODE_PA_SU_SC_MODE_CNTL_CLEAR;
      pipeline->pre_rasterization.pa_su_sc_mode_cntl |=
         terakan_state_draw_polygon_mode_pa_su_sc_mode_cntl(state->rs->polygon_mode);
   }
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_CULL_MODE)) {
      pipeline->pre_rasterization.pa_su_sc_mode_cntl_clear &=
         TERAKAN_STATE_DRAW_CULL_MODE_PA_SU_SC_MODE_CNTL_CLEAR;
      pipeline->pre_rasterization.pa_su_sc_mode_cntl |=
         terakan_state_draw_cull_mode_pa_su_sc_mode_cntl(state->rs->cull_mode);
   }
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_FRONT_FACE)) {
      pipeline->pre_rasterization.pa_su_sc_mode_cntl_clear &=
         TERAKAN_STATE_DRAW_FRONT_FACE_PA_SU_SC_MODE_CNTL_CLEAR;
      pipeline->pre_rasterization.pa_su_sc_mode_cntl |=
         terakan_state_draw_front_face_pa_su_sc_mode_cntl(state->rs->front_face);
   }
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_PROVOKING_VERTEX)) {
      pipeline->pre_rasterization.pa_su_sc_mode_cntl_clear &=
         TERAKAN_STATE_DRAW_PROVOKING_VERTEX_MODE_PA_SU_SC_MODE_CNTL_CLEAR;
      pipeline->pre_rasterization.pa_su_sc_mode_cntl |=
         terakan_state_draw_provoking_vertex_mode_pa_su_sc_mode_cntl(state->rs->provoking_vertex);
   }
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_DEPTH_BIAS_ENABLE)) {
      pipeline->pre_rasterization.pa_su_sc_mode_cntl_clear &=
         TERAKAN_STATE_DRAW_DEPTH_BIAS_ENABLE_PA_SU_SC_MODE_CNTL_CLEAR;
      pipeline->pre_rasterization.pa_su_sc_mode_cntl |=
         terakan_state_draw_depth_bias_enable_pa_su_sc_mode_cntl(state->rs->depth_bias.enable);
   }
   assert(!(pipeline->pre_rasterization.pa_su_sc_mode_cntl &
            pipeline->pre_rasterization.pa_su_sc_mode_cntl_clear));
   if (pipeline->pre_rasterization.pa_su_sc_mode_cntl_clear != UINT32_MAX) {
      BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_SC_MODE_CNTL);
   }

   return VK_SUCCESS;
}

static void
terakan_pipeline_graphics_multisample_init(struct terakan_pipeline_graphics * const pipeline,
                                           struct vk_graphics_pipeline_state const * const state)
{
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK */
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_MS_SAMPLE_MASK)) {
      pipeline->multisample.pa_sc_aa_mask = (uint16_t)state->ms->sample_mask;
      BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK);
   }
}
