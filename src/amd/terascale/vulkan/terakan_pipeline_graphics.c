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

#include "terakan_bo.h"
#include "terakan_command_buffer.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_shader.h"
#include "terakan_state.h"
#include "terakan_state_input_assembly.h"
#include "terakan_state_rasterization.h"

#include "gallium/drivers/r600/r600_shader_common.h"
#include "util/bitscan.h"
#include "util/macros.h"
#include "util/ralloc.h"
#include "util/u_math.h"
#include "vk_alloc.h"
#include "vk_graphics_state.h"
#include "vk_log.h"
#include "vk_util.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

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
   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_VGT_PRIMITIVE_TYPE);
}

static void
terakan_pipeline_graphics_apply_pa_sc_vport_z_min_0_max_1(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   if (command_writer->state_draw.pa_sc_vport_z_min_0_max_1 !=
       pipeline->pre_rasterization.pa_sc_vport_z_min_0_max_1) {
      command_writer->state_draw.pa_sc_vport_z_min_0_max_1 =
         pipeline->pre_rasterization.pa_sc_vport_z_min_0_max_1;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_SC_VPORT_Z_MIN_MAX);
   }
}

static void
terakan_pipeline_graphics_apply_viewport_count(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   terakan_state_draw_set_viewport_count(&command_writer->state_draw,
                                         pipeline->pre_rasterization.viewport_count);
}

static void
terakan_pipeline_graphics_apply_viewport(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   struct terakan_state_draw * const state = &command_writer->state_draw;
   for (uint32_t viewport_index = 0; viewport_index < pipeline->pre_rasterization.viewport_count;
        ++viewport_index) {
      struct terakan_state_draw_viewport * const state_viewport = &state->viewports[viewport_index];
      struct terakan_state_draw_viewport const * const pipeline_viewport =
         &pipeline->pre_rasterization.viewports[viewport_index];
      if (memcmp(state_viewport->pa_cl_vport_xy_scale_offset,
                 pipeline_viewport->pa_cl_vport_xy_scale_offset,
                 sizeof(state_viewport->pa_cl_vport_xy_scale_offset)) != 0) {
         terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_PA_CL_VPORT_XY_SCALE_OFFSET);
      }
      if (memcmp(state_viewport->pa_cl_vport_z_gl_dx_scale_offset,
                 pipeline_viewport->pa_cl_vport_z_gl_dx_scale_offset,
                 sizeof(state_viewport->pa_cl_vport_z_gl_dx_scale_offset)) != 0) {
         terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_PA_CL_VPORT_Z_SCALE_OFFSET);
      }
      if (memcmp(state_viewport->pa_cl_gb_vert_horz_clip_adj,
                 pipeline_viewport->pa_cl_gb_vert_horz_clip_adj,
                 sizeof(state_viewport->pa_cl_gb_vert_horz_clip_adj)) != 0) {
         terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_PA_CL_GB);
      }
      if (memcmp(state_viewport->pa_sc_vport_scissor_tl_br_xy,
                 pipeline_viewport->pa_sc_vport_scissor_tl_br_xy,
                 sizeof(state_viewport->pa_sc_vport_scissor_tl_br_xy)) != 0) {
         terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_PA_SC_VPORT_SCISSOR);
      }
      if (memcmp(state_viewport->pa_sc_vport_z_min_max, pipeline_viewport->pa_sc_vport_z_min_max,
                 sizeof(state_viewport->pa_sc_vport_z_min_max)) != 0 &&
          !state->pa_sc_vport_z_min_0_max_1) {
         terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_PA_SC_VPORT_Z_MIN_MAX);
      }
      *state_viewport = *pipeline_viewport;
   }
}

static void
terakan_pipeline_graphics_apply_pa_sc_vport_generic_scissor(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   if (!terakan_state_draw_is_pending(&command_writer->state_draw,
                                      TERAKAN_STATE_DRAW_PA_SC_VPORT_SCISSOR) &&
       memcmp(command_writer->state_draw.pa_sc_vport_generic_scissor_tl_br_xy,
              pipeline->pre_rasterization.pa_sc_vport_generic_scissor_tl_br_xy,
              sizeof(uint16_t) * 4 *
                 MIN2(command_writer->state_draw.viewport_count,
                      pipeline->pre_rasterization.pa_sc_vport_generic_scissor_count)) != 0) {
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_PA_SC_VPORT_SCISSOR);
   }
   memcpy(command_writer->state_draw.pa_sc_vport_generic_scissor_tl_br_xy,
          pipeline->pre_rasterization.pa_sc_vport_generic_scissor_tl_br_xy,
          sizeof(uint16_t) * 4 * pipeline->pre_rasterization.pa_sc_vport_generic_scissor_count);
}

static void
terakan_pipeline_graphics_apply_pa_cl_clip_cntl(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   uint32_t const old_pa_cl_clip_cntl = command_writer->state_draw.pa_cl_clip_cntl;
   terakan_state_draw_replace_fields(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL,
                                     &command_writer->state_draw.pa_cl_clip_cntl,
                                     pipeline->pre_rasterization.pa_cl_clip_cntl_clear,
                                     pipeline->pre_rasterization.pa_cl_clip_cntl);
   uint32_t const new_pa_cl_clip_cntl = command_writer->state_draw.pa_cl_clip_cntl;
   if (G_028810_DX_CLIP_SPACE_DEF(old_pa_cl_clip_cntl) !=
       G_028810_DX_CLIP_SPACE_DEF(new_pa_cl_clip_cntl)) {
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_PA_CL_VPORT_Z_SCALE_OFFSET);
   }
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
terakan_pipeline_graphics_apply_db_render_override_pre_rasterization(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   terakan_state_draw_replace_fields(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_DB_RENDER_OVERRIDE,
                                     &command_writer->state_draw.db_render_override,
                                     pipeline->pre_rasterization.db_render_override_clear,
                                     pipeline->pre_rasterization.db_render_override);
}

static void
terakan_pipeline_graphics_apply_pa_sc_aa_mask(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline,
   UNUSED enum terakan_pipeline_graphics_state_index const state_index)
{
   command_writer->state_draw.pa_sc_aa_mask = pipeline->multisample.pa_sc_aa_mask;
   terakan_state_draw_set_pending(&command_writer->state_draw, TERAKAN_STATE_DRAW_PA_SC_AA_MASK);
}

static terakan_pipeline_graphics_apply_state_function const
   terakan_pipeline_graphics_apply_state_functions[TERAKAN_PIPELINE_GRAPHICS_STATE_COUNT] = {
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_Z_MIN_0_MAX_1] =
         terakan_pipeline_graphics_apply_pa_sc_vport_z_min_0_max_1,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT_COUNT] =
         terakan_pipeline_graphics_apply_viewport_count,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT] = terakan_pipeline_graphics_apply_viewport,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_GENERIC_SCISSOR] =
         terakan_pipeline_graphics_apply_pa_sc_vport_generic_scissor,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE] =
         terakan_pipeline_graphics_apply_vgt_primitive_type,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL] =
         terakan_pipeline_graphics_apply_pa_cl_clip_cntl,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_SC_MODE_CNTL] =
         terakan_pipeline_graphics_apply_pa_su_sc_mode_cntl,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_DB_RENDER_OVERRIDE_PRE_RASTERIZATION] =
         terakan_pipeline_graphics_apply_db_render_override_pre_rasterization,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK] =
         terakan_pipeline_graphics_apply_pa_sc_aa_mask,
};

void
terakan_pipeline_graphics_bind(struct terakan_gfx_command_writer * const command_writer,
                               struct terakan_pipeline_graphics const * const pipeline)
{
   command_writer->state_draw.cmd_set_depth_clamp_enable_sets_depth_clip_enable =
      pipeline->pre_rasterization.cmd_set_depth_clamp_enable_sets_depth_clip_enable;

   unsigned state_index;
   BITSET_FOREACH_SET (state_index, pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_COUNT) {
      terakan_pipeline_graphics_apply_state_functions[state_index](
         command_writer, pipeline, (enum terakan_pipeline_graphics_state_index)state_index);
   }

   /* TODO(Triang3l): All vertex pipeline stages. */

   bool const sq_pgm_vs_modified = command_writer->hw_state_draw.sq_pgm_vs !=
                                   &pipeline->shaders[MESA_SHADER_VERTEX].static_state;
   command_writer->hw_state_draw.sq_pgm_vs = &pipeline->shaders[MESA_SHADER_VERTEX].static_state;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw, TERAKAN_HW_STATE_DRAW_SQ_PGM_VS,
                                 sq_pgm_vs_modified);

   if (pipeline->shader_stages & VK_SHADER_STAGE_FRAGMENT_BIT) {
      bool const sq_pgm_ps_modified = command_writer->hw_state_draw.sq_pgm_ps !=
                                      &pipeline->shaders[MESA_SHADER_FRAGMENT].static_state;
      command_writer->hw_state_draw.sq_pgm_ps =
         &pipeline->shaders[MESA_SHADER_FRAGMENT].static_state;
      terakan_hw_state_draw_written(&command_writer->hw_state_draw, TERAKAN_HW_STATE_DRAW_SQ_PGM_PS,
                                    sq_pgm_ps_modified);
   }
}

void
terakan_pipeline_graphics_destroy(struct terakan_pipeline_graphics * const pipeline,
                                  VkAllocationCallbacks const * allocator)
{
   if (allocator == NULL) {
      allocator = &pipeline->base.base.device->alloc;
   }

   unsigned remaining_shader_stages = (unsigned)pipeline->shader_stages;
   while (remaining_shader_stages) {
      terakan_shader_impl_finish(
         &pipeline->shaders[vk_to_mesa_shader_stage((
            VkShaderStageFlagBits)((VkShaderStageFlags)1 << u_bit_scan(&remaining_shader_stages)))],
         allocator);
   }

   terakan_pipeline_finish(&pipeline->base);

   vk_free(allocator, pipeline);
}

static void
terakan_pipeline_graphics_vertex_input_init(struct terakan_pipeline_graphics * const pipeline,
                                            struct vk_graphics_pipeline_state const * const state)
{
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE */
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_IA_PRIMITIVE_TOPOLOGY)) {
      pipeline->vertex_input.vgt_primitive_type =
         terakan_state_draw_primitive_topology_vgt_primitive_type(state->ia->primitive_topology);
      BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE);
   }
}

static void
terakan_pipeline_graphics_pre_rasterization_init(
   struct terakan_pipeline_graphics * const pipeline,
   struct vk_graphics_pipeline_state const * const state, bool const depth_range_unrestricted)
{
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_Z_MIN_0_MAX_1,
    * TERAKAN_PIPELINE_GRAPHICS_STATE_DB_RENDER_OVERRIDE_PRE_RASTERIZATION
    */
   pipeline->pre_rasterization.db_render_override_clear = UINT32_MAX;
   pipeline->pre_rasterization.db_render_override = 0;
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_DEPTH_CLAMP_ENABLE)) {
      if (depth_range_unrestricted) {
         pipeline->pre_rasterization.db_render_override_clear &= C_02800C_DISABLE_VIEWPORT_CLAMP;
         pipeline->pre_rasterization.db_render_override |=
            S_02800C_DISABLE_VIEWPORT_CLAMP(!state->rs->depth_clamp_enable);
      } else {
         pipeline->pre_rasterization.pa_sc_vport_z_min_0_max_1 = !state->rs->depth_clamp_enable;
         BITSET_SET(pipeline->static_state,
                    TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_Z_MIN_0_MAX_1);
      }
   }
   assert(!(pipeline->pre_rasterization.db_render_override &
            pipeline->pre_rasterization.db_render_override_clear));
   if (pipeline->pre_rasterization.db_render_override_clear != UINT32_MAX) {
      BITSET_SET(pipeline->static_state,
                 TERAKAN_PIPELINE_GRAPHICS_STATE_DB_RENDER_OVERRIDE_PRE_RASTERIZATION);
   }

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT_COUNT */
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_VP_VIEWPORT_COUNT)) {
      assert(state->vp->viewport_count <= ARRAY_SIZE(pipeline->pre_rasterization.viewports));
      pipeline->pre_rasterization.viewport_count = state->vp->viewport_count;
      BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT_COUNT);
      /* TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT */
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_VP_VIEWPORTS)) {
         for (uint32_t viewport_index = 0; viewport_index < state->vp->viewport_count;
              ++viewport_index) {
            terakan_state_draw_viewport_translate(
               &state->vp->viewports[viewport_index],
               &pipeline->pre_rasterization.viewports[viewport_index]);
         }
         BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT);
      }
   }

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_GENERIC_SCISSOR */
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_VP_SCISSORS)) {
      assert(state->vp->scissor_count <=
             ARRAY_SIZE(pipeline->pre_rasterization.pa_sc_vport_generic_scissor_tl_br_xy));
      for (uint32_t scissor_index = 0; scissor_index < state->vp->scissor_count; ++scissor_index) {
         terakan_state_translate_window_rectangle_unpacked(
            &state->vp->scissors[scissor_index],
            pipeline->pre_rasterization.pa_sc_vport_generic_scissor_tl_br_xy[scissor_index][0]);
      }
      BITSET_SET(pipeline->static_state,
                 TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_GENERIC_SCISSOR);
   }

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL */
   pipeline->pre_rasterization.pa_cl_clip_cntl_clear = UINT32_MAX;
   pipeline->pre_rasterization.pa_cl_clip_cntl = 0;
   if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_VP_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE)) {
      pipeline->pre_rasterization.pa_cl_clip_cntl_clear &= C_028810_DX_CLIP_SPACE_DEF;
      pipeline->pre_rasterization.pa_cl_clip_cntl |=
         S_028810_DX_CLIP_SPACE_DEF(!state->vp->depth_clip_negative_one_to_one);
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

static VkResult
terakan_pipeline_graphics_create(struct terakan_device * const device,
                                 VkGraphicsPipelineCreateInfo const * const create_info,
                                 VkAllocationCallbacks const * const allocator,
                                 struct terakan_pipeline_graphics ** const pipeline_out)
{
   VkResult result;

   struct terakan_pipeline_graphics * const pipeline =
      vk_alloc2(&device->vk.alloc, allocator, sizeof(struct terakan_pipeline_graphics),
                alignof(struct terakan_pipeline_graphics), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (pipeline == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   terakan_pipeline_init(&pipeline->base, device, false);

   BITSET_ZERO(pipeline->static_state);
   struct vk_graphics_pipeline_all_state all_state;
   struct vk_graphics_pipeline_state state = {};
   result = vk_graphics_pipeline_state_fill(&device->vk, &state, create_info, NULL, 0, &all_state,
                                            NULL, VK_SYSTEM_ALLOCATION_SCOPE_OBJECT, NULL);
   if (result != VK_SUCCESS) {
      goto fail_pipeline;
   }
   terakan_pipeline_graphics_vertex_input_init(pipeline, &state);
   terakan_pipeline_graphics_pre_rasterization_init(
      pipeline, &state, device->vk.enabled_extensions.EXT_depth_range_unrestricted);
   terakan_pipeline_graphics_multisample_init(pipeline, &state);

   pipeline->shader_stages = 0;
   for (uint32_t stage_info_index = 0; stage_info_index < create_info->stageCount;
        ++stage_info_index) {
      VkPipelineShaderStageCreateInfo const * const stage_info =
         &create_info->pStages[stage_info_index];

      size_t spirv_size_bytes;
      uint32_t const * spirv = terakan_pipeline_stage_spirv(stage_info, &spirv_size_bytes);
      assert(spirv != NULL);

      gl_shader_stage const stage_index = vk_to_mesa_shader_stage(stage_info->stage);
      nir_shader * nir =
         terakan_shader_spirv_to_nir(device, spirv_size_bytes, spirv, stage_index,
                                     stage_info->pName, stage_info->pSpecializationInfo);
      if (nir == NULL) {
         result = vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
         goto fail_shaders;
      }

      /* TODO(Triang3l): Construct the shader info from the NIR and, when available, the pipeline
       * state.
       */
      union r600_shader_key shader_key = {};
      /* TODO(Triang3l): Remove this color buffer count test. */
      if (stage_index == MESA_SHADER_FRAGMENT) {
         shader_key.ps.nr_cbufs = 1;
      }

      result = terakan_shader_impl_init_from_nir(&pipeline->shaders[stage_index], device,
                                                 &shader_key, nir, allocator);
      ralloc_free(nir);
      if (result != VK_SUCCESS) {
         goto fail_shaders;
      }

      /* Fully initialized now, make sure it's fully cleaned up in case of failure. */
      pipeline->shader_stages |= stage_info->stage;
   }

   *pipeline_out = pipeline;
   return VK_SUCCESS;

fail_shaders : {
   unsigned remaining_shader_stages = (unsigned)pipeline->shader_stages;
   while (remaining_shader_stages) {
      terakan_shader_impl_finish(
         &pipeline->shaders[vk_to_mesa_shader_stage((
            VkShaderStageFlagBits)((VkShaderStageFlags)1 << u_bit_scan(&remaining_shader_stages)))],
         allocator);
   }
}
fail_pipeline:
   terakan_pipeline_finish(&pipeline->base);
   vk_free2(&device->vk.alloc, allocator, pipeline);
   return result;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreateGraphicsPipelines(VkDevice const deviceHandle, VkPipelineCache const pipelineCache,
                                uint32_t const createInfoCount,
                                VkGraphicsPipelineCreateInfo const * const pCreateInfos,
                                VkAllocationCallbacks const * const pAllocator,
                                VkPipeline * const pPipelines)
{
   VkResult result = VK_SUCCESS;

   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   uint32_t pipeline_index;

   for (pipeline_index = 0; pipeline_index < createInfoCount; ++pipeline_index) {
      struct terakan_pipeline_graphics * pipeline;
      VkGraphicsPipelineCreateInfo const * const create_info = &pCreateInfos[pipeline_index];
      VkResult const pipeline_result =
         terakan_pipeline_graphics_create(device, create_info, pAllocator, &pipeline);
      if (pipeline_result != VK_SUCCESS) {
         result = pipeline_result;
         if (terakan_pipeline_create_flags(create_info->flags, create_info->pNext) &
             VK_PIPELINE_CREATE_2_EARLY_RETURN_ON_FAILURE_BIT_KHR) {
            break;
         }
         pPipelines[pipeline_index] = VK_NULL_HANDLE;
         continue;
      }
      pPipelines[pipeline_index] = terakan_pipeline_to_handle(&pipeline->base);
   }

   for (; pipeline_index < createInfoCount; ++pipeline_index) {
      pPipelines[pipeline_index] = VK_NULL_HANDLE;
   }

   return result;
}
