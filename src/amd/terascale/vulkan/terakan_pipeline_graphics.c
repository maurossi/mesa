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

#include "terakan_pipeline_graphics.h"

#include "terakan_bo.h"
#include "terakan_command_buffer.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_physical_device.h"
#include "terakan_pipeline_layout.h"
#include "terakan_shader.h"
#include "terakan_state.h"
#include "terakan_state_color.h"
#include "terakan_state_input_assembly.h"
#include "terakan_state_rasterization.h"

#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600_shader_common.h"
#include "util/bitscan.h"
#include "util/macros.h"
#include "util/ralloc.h"
#include "util/u_math.h"
#include "vk_alloc.h"
#include "vk_enum_to_str.h"
#include "vk_graphics_state.h"
#include "vk_log.h"
#include "vk_util.h"

#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef void (*terakan_pipeline_graphics_apply_state_function)(
   struct terakan_gfx_command_writer * command_writer,
   struct terakan_pipeline_graphics const * pipeline);

static void
terakan_pipeline_graphics_apply_vgt_primitive_type(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   command_writer->state_draw.vgt_primitive_type = pipeline->vertex_input.vgt_primitive_type;
   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_INDEX_VGT_PRIMITIVE_TYPE);
}

static void
terakan_pipeline_graphics_apply_sq_pgm_fs(struct terakan_gfx_command_writer * const command_writer,
                                          struct terakan_pipeline_graphics const * const pipeline)
{
   struct terakan_vertex_input_static_state const * const sq_pgm_fs =
      pipeline->vertex_input.sq_pgm_fs.program_bo != NULL
         ? &pipeline->vertex_input.sq_pgm_fs
         : &container_of(pipeline->base.base.device, struct terakan_device const, vk)
               ->empty_vertex_input;
   if (command_writer->state_draw.sq_pgm_fs.static_state != sq_pgm_fs) {
      command_writer->state_draw.sq_pgm_fs.static_state = sq_pgm_fs;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_SQ_PGM_FS);
   }
}

static void
terakan_pipeline_graphics_apply_sq_resources_fs_stride(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   {
      unsigned bindings_remaining =
         pipeline->vertex_input.sq_resources_fs_stride.bindings_with_static_stride;
      while (bindings_remaining) {
         int const binding_index = u_bit_scan(&bindings_remaining);
         command_writer->state_draw.sq_resources_fs[binding_index].stride =
            pipeline->vertex_input.sq_resources_fs_stride.binding_strides[binding_index];
      }
   }
   command_writer->state_draw.sq_resources_fs_pending |=
      pipeline->vertex_input.sq_resources_fs_stride.bindings_with_static_stride;
   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_INDEX_SQ_RESOURCES_FS);
}

static void
terakan_pipeline_graphics_apply_sq_pgm_fs_2048_stride_workaround(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   assert(BITSET_TEST(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS));
   assert(
      BITSET_TEST(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_RESOURCES_FS_STRIDE));
   assert(!(pipeline->vertex_input.sq_pgm_fs.bindings_with_2048_stride_workaround &
            ~pipeline->vertex_input.sq_resources_fs_stride.bindings_with_static_stride));
   uint32_t const new_bindings_with_2048_stride_workaround =
      (command_writer->state_draw.sq_pgm_fs.bindings_with_2048_stride_workaround &
       ~pipeline->vertex_input.sq_resources_fs_stride.bindings_with_static_stride) |
      pipeline->vertex_input.sq_pgm_fs.bindings_with_2048_stride_workaround;
   /* If whether the workaround needs to be applied to any currently needed bindings is changed,
    * update the fetch shader.
    */
   if ((command_writer->state_draw.sq_pgm_fs.bindings_with_2048_stride_workaround ^
        new_bindings_with_2048_stride_workaround) &
       pipeline->vertex_input.sq_pgm_fs.bindings_needed_by_attributes_and_provided) {
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_SQ_PGM_FS);
   }
   command_writer->state_draw.sq_pgm_fs.bindings_with_2048_stride_workaround =
      new_bindings_with_2048_stride_workaround;
}

static void
terakan_pipeline_graphics_apply_pa_sc_vport_z_min_0_max_1(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   if (command_writer->state_draw.viewport.pa_sc_vport_z_min_0_max_1 !=
       pipeline->pre_rasterization.pa_sc_vport_z_min_0_max_1) {
      command_writer->state_draw.viewport.pa_sc_vport_z_min_0_max_1 =
         pipeline->pre_rasterization.pa_sc_vport_z_min_0_max_1;
      command_writer->state_draw.viewport.viewports_pending.pa_sc_vport_z_min_max =
         BITFIELD_MASK(ARRAY_SIZE(command_writer->state_draw.viewport.viewports));
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_VIEWPORT);
   }
}

static void
terakan_pipeline_graphics_apply_viewport_count(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   terakan_state_draw_set_viewport_count(&command_writer->state_draw,
                                         pipeline->pre_rasterization.viewport_count);
}

static void
terakan_pipeline_graphics_apply_viewport(struct terakan_gfx_command_writer * const command_writer,
                                         struct terakan_pipeline_graphics const * const pipeline)
{
   struct terakan_state_draw * const state = &command_writer->state_draw;
   bool any_pending = false;
   for (uint8_t viewport_index = 0; viewport_index < pipeline->pre_rasterization.viewport_count;
        ++viewport_index) {
      struct terakan_state_draw_viewport * const state_viewport =
         &state->viewport.viewports[viewport_index];
      struct terakan_state_draw_viewport const * const pipeline_viewport =
         &pipeline->pre_rasterization.viewports[viewport_index];
      uint16_t const viewport_bit = (uint16_t)BITFIELD_BIT(viewport_index);
      if (memcmp(state_viewport->pa_cl_vport_xy_scale_offset,
                 pipeline_viewport->pa_cl_vport_xy_scale_offset,
                 sizeof(state_viewport->pa_cl_vport_xy_scale_offset)) != 0) {
         any_pending = true;
         state->viewport.viewports_pending.pa_cl_vport_xy_scale_offset |= viewport_bit;
      }
      if (memcmp(state_viewport->pa_cl_vport_z_gl_dx_scale_offset,
                 pipeline_viewport->pa_cl_vport_z_gl_dx_scale_offset,
                 sizeof(state_viewport->pa_cl_vport_z_gl_dx_scale_offset)) != 0) {
         any_pending = true;
         state->viewport.viewports_pending.pa_cl_vport_z_scale_offset |= viewport_bit;
      }
      if (memcmp(state_viewport->pa_cl_gb_vert_horz_clip_adj,
                 pipeline_viewport->pa_cl_gb_vert_horz_clip_adj,
                 sizeof(state_viewport->pa_cl_gb_vert_horz_clip_adj)) != 0) {
         any_pending = true;
         state->viewport.pa_cl_gb_pending = true;
      }
      if (memcmp(state_viewport->pa_sc_vport_scissor_tl_br_xy,
                 pipeline_viewport->pa_sc_vport_scissor_tl_br_xy,
                 sizeof(state_viewport->pa_sc_vport_scissor_tl_br_xy)) != 0) {
         any_pending = true;
         state->viewport.viewports_pending.pa_sc_vport_scissor |= viewport_bit;
      }
      if (memcmp(state_viewport->pa_sc_vport_z_min_max, pipeline_viewport->pa_sc_vport_z_min_max,
                 sizeof(state_viewport->pa_sc_vport_z_min_max)) != 0 &&
          !state->viewport.pa_sc_vport_z_min_0_max_1) {
         any_pending = true;
         state->viewport.viewports_pending.pa_sc_vport_z_min_max |= viewport_bit;
      }
      *state_viewport = *pipeline_viewport;
   }
   if (any_pending) {
      terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_VIEWPORT);
   }
}

static void
terakan_pipeline_graphics_apply_pa_sc_vport_generic_scissor(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   for (uint32_t scissor_index = 0;
        scissor_index < pipeline->pre_rasterization.pa_sc_vport_generic_scissor_count;
        ++scissor_index) {
      uint16_t * const viewport_scissor =
         command_writer->state_draw.viewport.pa_sc_vport_generic_scissor_tl_br_xy[scissor_index][0];
      uint16_t const * const pipeline_scissor =
         pipeline->pre_rasterization.pa_sc_vport_generic_scissor_tl_br_xy[scissor_index][0];
      if (memcmp(viewport_scissor, pipeline_scissor, sizeof(uint16_t) * 4) != 0) {
         memcpy(viewport_scissor, pipeline_scissor, sizeof(uint16_t) * 4);
         command_writer->state_draw.viewport.viewports_pending.pa_sc_vport_scissor |=
            (uint16_t)BITFIELD_BIT(scissor_index);
         terakan_state_draw_set_pending(&command_writer->state_draw,
                                        TERAKAN_STATE_DRAW_INDEX_VIEWPORT);
      }
   }
}

static void
terakan_pipeline_graphics_apply_pa_cl_clip_cntl(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   terakan_state_draw_replace_fields(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL,
                                     &command_writer->state_draw.pa_cl_clip_cntl,
                                     pipeline->pre_rasterization.pa_cl_clip_cntl_clear,
                                     pipeline->pre_rasterization.pa_cl_clip_cntl);
}

static void
terakan_pipeline_graphics_apply_pa_su_sc_mode_cntl(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   terakan_state_draw_replace_fields(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_PA_SU_SC_MODE_CNTL,
                                     &command_writer->state_draw.pa_su_sc_mode_cntl,
                                     pipeline->pre_rasterization.pa_su_sc_mode_cntl_clear,
                                     pipeline->pre_rasterization.pa_su_sc_mode_cntl);
}

static void
terakan_pipeline_graphics_apply_pa_su_poly_offset(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   /* These values are not needed by internal draws, modify hw_state_draw directly. */
   bool const clamp_scale_offset_modified =
      memcmp(&command_writer->hw_state_draw.pa_su_poly_offset_clamp,
             &pipeline->pre_rasterization.pa_su_poly_offset.clamp, sizeof(float)) != 0 ||
      memcmp(&command_writer->hw_state_draw.pa_su_poly_offset_subpixel_slope_scale,
             &pipeline->pre_rasterization.pa_su_poly_offset.subpixel_slope_scale,
             sizeof(float)) != 0 ||
      memcmp(&command_writer->hw_state_draw.pa_su_poly_offset_offset,
             &pipeline->pre_rasterization.pa_su_poly_offset.offset, sizeof(float)) != 0;
   command_writer->hw_state_draw.pa_su_poly_offset_clamp =
      pipeline->pre_rasterization.pa_su_poly_offset.clamp;
   command_writer->hw_state_draw.pa_su_poly_offset_subpixel_slope_scale =
      pipeline->pre_rasterization.pa_su_poly_offset.subpixel_slope_scale;
   command_writer->hw_state_draw.pa_su_poly_offset_offset =
      pipeline->pre_rasterization.pa_su_poly_offset.offset;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_CLAMP_SCALE_OFFSET,
                                 clamp_scale_offset_modified);

   if (command_writer->state_draw.pa_su_poly_offset_db_fmt_cntl.representation !=
          pipeline->pre_rasterization.pa_su_poly_offset.representation ||
       command_writer->state_draw.pa_su_poly_offset_db_fmt_cntl.representation_exact !=
          pipeline->pre_rasterization.pa_su_poly_offset.representation_exact) {
      command_writer->state_draw.pa_su_poly_offset_db_fmt_cntl.representation =
         pipeline->pre_rasterization.pa_su_poly_offset.representation;
      command_writer->state_draw.pa_su_poly_offset_db_fmt_cntl.representation_exact =
         pipeline->pre_rasterization.pa_su_poly_offset.representation_exact;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_DB_FMT_CNTL);
   }
}

static void
terakan_pipeline_graphics_apply_db_render_override_pre_rasterization(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   terakan_state_draw_replace_fields(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_DB_RENDER_OVERRIDE,
                                     &command_writer->state_draw.db_render_override,
                                     pipeline->pre_rasterization.db_render_override_clear,
                                     pipeline->pre_rasterization.db_render_override);
}

static void
terakan_pipeline_graphics_apply_pa_sc_aa_mask(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   command_writer->state_draw.pa_sc_aa_mask = pipeline->multisample.pa_sc_aa_mask;
   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_INDEX_PA_SC_AA_MASK);
}

static void
terakan_pipeline_graphics_apply_db_stencilrefmask(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   for (unsigned face_index = 0; face_index < 2; ++face_index) {
      terakan_state_draw_replace_fields(
         &command_writer->state_draw, TERAKAN_STATE_DRAW_INDEX_DB_STENCILREFMASK,
         &command_writer->state_draw.db_stencilrefmask_front_back[face_index],
         pipeline->fragment_shader.db_stencilrefmask_clear,
         pipeline->fragment_shader.db_stencilrefmask_front_back[face_index]);
   }
}

static void
terakan_pipeline_graphics_apply_db_depth_control(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   terakan_state_draw_replace_fields(
      &command_writer->state_draw, TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_CONTROL,
      &command_writer->state_draw.db_depth_control,
      pipeline->fragment_shader.db_depth_control_clear, pipeline->fragment_shader.db_depth_control);
}

static void
terakan_pipeline_graphics_apply_logic_op_enable(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   /* Make TERAKAN_STATE_DRAW_INDEX_LOGIC_OP pending only if needed due to the complexity of
    * applying it.
    */
   if (command_writer->state_draw.logic_op.enable != pipeline->fragment_output.logic_op_enable) {
      command_writer->state_draw.logic_op.enable = pipeline->fragment_output.logic_op_enable;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_LOGIC_OP);
   }
}

static void
terakan_pipeline_graphics_apply_logic_op_rop3(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   /* Make TERAKAN_STATE_DRAW_INDEX_LOGIC_OP pending only if needed due to the complexity of
    * applying it.
    */
   if (command_writer->state_draw.logic_op.enable &&
       command_writer->state_draw.logic_op.rop3 != pipeline->fragment_output.logic_op_rop3) {
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_LOGIC_OP);
   }
   command_writer->state_draw.logic_op.rop3 = pipeline->fragment_output.logic_op_rop3;
}

static void
terakan_pipeline_graphics_apply_cb_blend_rgba(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   /* The blend constant is not needed by internal draws, modify hw_state_draw directly. */
   bool const modified = memcmp(command_writer->hw_state_draw.cb_blend_rgba,
                                pipeline->fragment_output.cb_blend_rgba, sizeof(float) * 4) != 0;
   memcpy(command_writer->hw_state_draw.cb_blend_rgba, pipeline->fragment_output.cb_blend_rgba,
          sizeof(float) * 4);
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_CB_BLEND_RGBA, modified);
}

static void
terakan_pipeline_graphics_apply_cb_blend_control_enable(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   for (uint32_t attachment_index = 0;
        attachment_index < pipeline->fragment_output.color_blend_attachment_count;
        ++attachment_index) {
      uint32_t * const cb_blend_control_ptr =
         &command_writer->state_draw.cb_blend_control.attachments[attachment_index];
      bool const attachment_enable = G_028780_BLEND_CONTROL_ENABLE(
         pipeline->fragment_output.cb_blend_control[attachment_index]);
      if (G_028780_BLEND_CONTROL_ENABLE(*cb_blend_control_ptr) != attachment_enable) {
         *cb_blend_control_ptr = (*cb_blend_control_ptr & C_028780_BLEND_CONTROL_ENABLE) |
                                 S_028780_BLEND_CONTROL_ENABLE(attachment_enable);
         terakan_state_draw_set_pending(&command_writer->state_draw,
                                        TERAKAN_STATE_DRAW_INDEX_CB_BLEND_CONTROL);
      }
   }
}

static void
terakan_pipeline_graphics_apply_cb_blend_control_equation(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   for (uint32_t attachment_index = 0;
        attachment_index < pipeline->fragment_output.color_blend_attachment_count;
        ++attachment_index) {
      uint32_t * const cb_blend_control_ptr =
         &command_writer->state_draw.cb_blend_control.attachments[attachment_index];
      bool const attachment_enable = G_028780_BLEND_CONTROL_ENABLE(*cb_blend_control_ptr);
      uint32_t const cb_blend_control =
         S_028780_BLEND_CONTROL_ENABLE(attachment_enable) |
         (pipeline->fragment_output.cb_blend_control[attachment_index] &
          C_028780_BLEND_CONTROL_ENABLE);
      if (*cb_blend_control_ptr != cb_blend_control) {
         *cb_blend_control_ptr = cb_blend_control;
         if (attachment_enable) {
            terakan_state_draw_set_pending(&command_writer->state_draw,
                                           TERAKAN_STATE_DRAW_INDEX_CB_BLEND_CONTROL);
         }
      }
   }
}

static void
terakan_pipeline_graphics_apply_color_attachment_write_mask(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   for (uint32_t attachment_index = 0;
        attachment_index < pipeline->fragment_output.color_blend_attachment_count;
        ++attachment_index) {
      uint8_t * const attachment_write_mask_ptr =
         &command_writer->state_draw.cb_target_mask.attachment_write_masks[attachment_index];
      uint8_t const attachment_write_mask =
         pipeline->fragment_output.color_attachment_write_masks[attachment_index];
      if (*attachment_write_mask_ptr != attachment_write_mask) {
         *attachment_write_mask_ptr = attachment_write_mask;
         terakan_state_draw_set_pending(&command_writer->state_draw,
                                        TERAKAN_STATE_DRAW_INDEX_CB_TARGET_MASK);
      }
   }
}

static void
terakan_pipeline_graphics_apply_color_attachment_write_enable(
   struct terakan_gfx_command_writer * const command_writer,
   struct terakan_pipeline_graphics const * const pipeline)
{
   if (command_writer->state_draw.cb_target_mask.attachment_write_enable !=
       pipeline->fragment_output.color_attachment_write_enable) {
      command_writer->state_draw.cb_target_mask.attachment_write_enable =
         pipeline->fragment_output.color_attachment_write_enable;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_CB_TARGET_MASK);
   }
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
      [TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS] = terakan_pipeline_graphics_apply_sq_pgm_fs,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_RESOURCES_FS_STRIDE] =
         terakan_pipeline_graphics_apply_sq_resources_fs_stride,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS_2048_STRIDE_WORKAROUND] =
         terakan_pipeline_graphics_apply_sq_pgm_fs_2048_stride_workaround,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL] =
         terakan_pipeline_graphics_apply_pa_cl_clip_cntl,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_SC_MODE_CNTL] =
         terakan_pipeline_graphics_apply_pa_su_sc_mode_cntl,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_POLY_OFFSET] =
         terakan_pipeline_graphics_apply_pa_su_poly_offset,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_DB_RENDER_OVERRIDE_PRE_RASTERIZATION] =
         terakan_pipeline_graphics_apply_db_render_override_pre_rasterization,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK] =
         terakan_pipeline_graphics_apply_pa_sc_aa_mask,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_DB_STENCILREFMASK] =
         terakan_pipeline_graphics_apply_db_stencilrefmask,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_DB_DEPTH_CONTROL] =
         terakan_pipeline_graphics_apply_db_depth_control,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_LOGIC_OP_ENABLE] =
         terakan_pipeline_graphics_apply_logic_op_enable,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_LOGIC_OP_ROP3] =
         terakan_pipeline_graphics_apply_logic_op_rop3,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_RGBA] =
         terakan_pipeline_graphics_apply_cb_blend_rgba,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_ENABLE] =
         terakan_pipeline_graphics_apply_cb_blend_control_enable,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_EQUATION] =
         terakan_pipeline_graphics_apply_cb_blend_control_equation,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_MASK] =
         terakan_pipeline_graphics_apply_color_attachment_write_mask,
      [TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_ENABLE] =
         terakan_pipeline_graphics_apply_color_attachment_write_enable,
};

void
terakan_pipeline_graphics_bind(struct terakan_gfx_command_writer * const command_writer,
                               struct terakan_pipeline_graphics const * const pipeline)
{
   /* Vertex shader. */

   struct terakan_shader_impl const * const vs = &pipeline->shaders[MESA_SHADER_VERTEX];
   if (command_writer->state_draw.sq_pgm_ls_es_gs_vs.vs_as_vs != vs) {
      command_writer->state_draw.sq_pgm_ls_es_gs_vs.vs_as_vs = vs;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_SQ_PGM_LS_ES_GS_VS);
   }

   /* Fragment shader. */

   struct terakan_shader_impl const * const fs =
      pipeline->shader_stages & VK_SHADER_STAGE_FRAGMENT_BIT
         ? &pipeline->shaders[MESA_SHADER_FRAGMENT]
         : NULL;
   if (command_writer->state_draw.sq_pgm_ps.fs != fs) {
      command_writer->state_draw.sq_pgm_ps.fs = fs;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_SQ_PGM_PS);
   }

   /* Static state. */

   command_writer->state_draw.cmd_set_depth_clamp_enable_sets_depth_clip_enable =
      pipeline->pre_rasterization.cmd_set_depth_clamp_enable_sets_depth_clip_enable;

   unsigned state_index;
   BITSET_FOREACH_SET (state_index, pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_COUNT) {
      terakan_pipeline_graphics_apply_state_functions[state_index](command_writer, pipeline);
   }
}

void
terakan_pipeline_graphics_destroy(struct terakan_pipeline_graphics * const pipeline,
                                  VkAllocationCallbacks const * allocator)
{
   if (allocator == NULL) {
      allocator = &pipeline->base.base.device->alloc;
   }

   if (BITSET_TEST(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS) &&
       pipeline->vertex_input.sq_pgm_fs.program_bo != NULL) {
      terakan_bo_free(pipeline->vertex_input.sq_pgm_fs.program_bo, allocator);
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

/* If attributes_needed_by_vs is NULL (in case of a library with vertex input state, but no
 * pre-rasterization state), all provided inputs are assumed to be needed, but the fetch shader
 * isn't created.
 */
static VkResult
terakan_pipeline_graphics_vertex_input_init(struct terakan_pipeline_graphics * const pipeline,
                                            struct vk_graphics_pipeline_state const * const state,
                                            struct terakan_device * const device,
                                            BITSET_WORD const * const attributes_needed_by_vs,
                                            VkAllocationCallbacks const * const allocator)
{
   VkResult result;

   if (state->ia != NULL) {
      /* TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE */
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_IA_PRIMITIVE_TOPOLOGY)) {
         pipeline->vertex_input.vgt_primitive_type =
            terakan_state_draw_primitive_topology_vgt_primitive_type(state->ia->primitive_topology);
         BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE);
      }
   }

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS,
    * TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_RESOURCES_FS_STRIDE,
    * TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS_2048_STRIDE_WORKAROUND
    */
   if (state->vi != NULL && !BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_VI)) {
      struct terakan_vertex_input_static_state * const fs_state = &pipeline->vertex_input.sq_pgm_fs;

      bool const is_r9xx = terakan_device_physical_device(device)->chip_family_info.is_r9xx;

      uint32_t const bindings_provided = (uint32_t)state->vi->bindings_valid;

      BITSET_ZERO(fs_state->attributes_needed_and_provided);
      static_assert(
         sizeof(BITSET_WORD) >= sizeof(uint32_t),
         "Assuming that vk_vertex_input_state::attributes_valid can fit into one bitset word, as "
         "the maximum attribute count inside Terakan is more flexible than in the Mesa Vulkan "
         "runtime, with the possibility to expose more than 32 reserved for future.");
      fs_state->attributes_needed_and_provided[0] = state->vi->attributes_valid;
      if (attributes_needed_by_vs != NULL) {
         fs_state->attributes_needed_and_provided[0] = attributes_needed_by_vs[0];
      }
      fs_state->bindings_needed_by_attributes_and_provided = 0b0;
      unsigned attribute_index;
      BITSET_FOREACH_SET (attribute_index, fs_state->attributes_needed_and_provided,
                          TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES) {
         struct vk_vertex_attribute_state const * const attribute =
            &state->vi->attributes[attribute_index];
         uint32_t const attribute_binding_bit = BITFIELD_BIT(attribute->binding);
         if (unlikely(!terakan_vertex_input_attribute_translate(
                attribute_index, attribute->binding, attribute->format, attribute->offset,
                &fs_state->attributes[attribute_index]))) {
            return vk_errorf(
               device, VK_ERROR_VALIDATION_FAILED_EXT,
               "Failed to translate vertex attribute %d: binding %" PRIu32 ", format %s "
               "(%" PRIu32 "), offset %" PRIu32,
               attribute_index, attribute->binding, vk_Format_to_str(attribute->format),
               (uint32_t)attribute->format, attribute->offset);
         }
         assert(bindings_provided & attribute_binding_bit);
         if (unlikely(!(bindings_provided & attribute_binding_bit))) {
            return vk_errorf(device, VK_ERROR_VALIDATION_FAILED_EXT,
                             "Vertex attribute %d uses binding %" PRIu32 ", which is not "
                             "provided",
                             attribute_index, attribute->binding);
         }
         fs_state->bindings_needed_by_attributes_and_provided |= attribute_binding_bit;
      }

      fs_state->instance_bindings = 0b0;
      {
         unsigned bindings_remaining = fs_state->bindings_needed_by_attributes_and_provided;
         while (bindings_remaining) {
            int const binding_index = u_bit_scan(&bindings_remaining);
            struct vk_vertex_binding_state const * const binding =
               &state->vi->bindings[binding_index];
            if (binding->input_rate == VK_VERTEX_INPUT_RATE_INSTANCE) {
               fs_state->instance_bindings |= BITFIELD_BIT(binding_index);
               fs_state->instance_binding_divisors[binding_index] = binding->divisor;
            }
         }
      }

      fs_state->bindings_with_2048_stride_workaround = 0b0;
      if (bindings_provided && !BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_VI_BINDING_STRIDES)) {
         pipeline->vertex_input.sq_resources_fs_stride.bindings_with_static_stride =
            bindings_provided;
         {
            unsigned bindings_remaining =
               pipeline->vertex_input.sq_resources_fs_stride.bindings_with_static_stride;
            while (bindings_remaining) {
               int const binding_index = u_bit_scan(&bindings_remaining);
               uint16_t const binding_stride = state->vi->bindings[binding_index].stride;
               pipeline->vertex_input.sq_resources_fs_stride.binding_strides[binding_index] =
                  binding_stride;
               if (!is_r9xx && binding_stride >= 2048) {
                  fs_state->bindings_with_2048_stride_workaround |= BITFIELD_BIT(binding_index);
               }
            }
         }
         BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_RESOURCES_FS_STRIDE);
         if (!is_r9xx) {
            BITSET_SET(pipeline->static_state,
                       TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS_2048_STRIDE_WORKAROUND);
         }
      }

      /* If attributes_needed_by_vs is NULL, this is a pipeline library with a vertex input state,
       * but no pre-rasterization state (no vertex shader). The fetch shader will be created when
       * they're linked for only the needed attributes.
       * If there are no valid needed attributes (thus no used bindings either), don't create a
       * fetch shader, instead use the empty one.
       */
      if (attributes_needed_by_vs == NULL ||
          !fs_state->bindings_needed_by_attributes_and_provided) {
         fs_state->program_bo = NULL;
         fs_state->program_va_shr8 = 0;
      } else {
         uint32_t fs_alu_qword_count, fs_alu_clause_count, fs_fetch_count;
         uint32_t fs_alu[2 * TERAKAN_VERTEX_INPUT_FS_MAX_ALU_QWORDS];
         uint8_t fs_alu_clause_qwords[TERAKAN_VERTEX_INPUT_FS_MAX_ALU_CLAUSES];
         uint32_t fs_fetch[4 * TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES];
         terakan_vertex_input_create_fs_alu_and_fetches(
            is_r9xx, fs_state->attributes_needed_and_provided, fs_state->attributes,
            fs_state->instance_bindings, fs_state->instance_binding_divisors,
            fs_state->bindings_with_2048_stride_workaround, &fs_alu_qword_count, fs_alu,
            &fs_alu_clause_count, fs_alu_clause_qwords, &fs_fetch_count, fs_fetch);
         /* TODO(Triang3l): Suballocate the fetch shader as well as other shaders. */
         result = device->winsys_fn->bo->allocate_device_memory(
            device,
            terakan_vertex_input_fs_byte_count(fs_alu_qword_count, fs_alu_clause_count,
                                               fs_fetch_count),
            TERAKAN_SHADER_PROGRAM_ALIGNMENT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            0, allocator, VK_SYSTEM_ALLOCATION_SCOPE_OBJECT, &fs_state->program_bo);
         if (result != VK_SUCCESS) {
            return vk_error(device, result);
         }
         fs_state->program_va_shr8 = 0;
         void * const fs_mapping = terakan_bo_map(fs_state->program_bo);
         if (fs_mapping == NULL) {
            terakan_bo_free(fs_state->program_bo, allocator);
            return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
         }
         terakan_vertex_input_create_fs_program(is_r9xx, fs_alu_qword_count, fs_alu,
                                                fs_alu_clause_count, fs_alu_clause_qwords,
                                                fs_fetch_count, fs_fetch, fs_mapping);
         terakan_bo_unmap(fs_state->program_bo);
      }

      BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS);
   }

   return VK_SUCCESS;
}

static void
terakan_pipeline_graphics_pre_rasterization_init(
   struct terakan_pipeline_graphics * const pipeline,
   struct vk_graphics_pipeline_state const * const state, bool const depth_range_unrestricted)
{
   if (state->rs != NULL) {
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
   }

   if (state->vp != NULL) {
      /* TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT_COUNT */
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_VP_VIEWPORT_COUNT)) {
         assert(state->vp->viewport_count <= ARRAY_SIZE(pipeline->pre_rasterization.viewports));
         pipeline->pre_rasterization.viewport_count = (uint8_t)state->vp->viewport_count;
         BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT_COUNT);
         /* TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT */
         if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_VP_VIEWPORTS)) {
            for (uint8_t viewport_index = 0;
                 viewport_index < pipeline->pre_rasterization.viewport_count; ++viewport_index) {
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
         pipeline->pre_rasterization.pa_sc_vport_generic_scissor_count = state->vp->scissor_count;
         for (uint32_t scissor_index = 0; scissor_index < state->vp->scissor_count;
              ++scissor_index) {
            terakan_state_translate_window_rect_unpacked(
               &state->vp->scissors[scissor_index],
               pipeline->pre_rasterization.pa_sc_vport_generic_scissor_tl_br_xy[scissor_index][0]);
         }
         BITSET_SET(pipeline->static_state,
                    TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_GENERIC_SCISSOR);
      }
   }

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL */
   pipeline->pre_rasterization.pa_cl_clip_cntl_clear = UINT32_MAX;
   pipeline->pre_rasterization.pa_cl_clip_cntl = 0;
   if (state->vp != NULL) {
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_VP_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE)) {
         pipeline->pre_rasterization.pa_cl_clip_cntl_clear &= C_028810_DX_CLIP_SPACE_DEF;
         pipeline->pre_rasterization.pa_cl_clip_cntl |=
            S_028810_DX_CLIP_SPACE_DEF(!state->vp->depth_clip_negative_one_to_one);
      }
   }
   if (state->rs != NULL) {
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_RASTERIZER_DISCARD_ENABLE)) {
         pipeline->pre_rasterization.pa_cl_clip_cntl_clear &=
            TERAKAN_STATE_DRAW_RASTERIZER_DISCARD_ENABLE_PA_CL_CLIP_CNTL_CLEAR;
         pipeline->pre_rasterization.pa_cl_clip_cntl |=
            terakan_state_draw_rasterizer_discard_enable_pa_cl_clip_cntl(
               state->rs->rasterizer_discard_enable);
      }
   }
   pipeline->pre_rasterization.cmd_set_depth_clamp_enable_sets_depth_clip_enable = false;
   if (state->rs != NULL) {
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_DEPTH_CLIP_ENABLE)) {
         if (state->rs->depth_clip_enable == VK_MESA_DEPTH_CLIP_ENABLE_NOT_CLAMP) {
            pipeline->pre_rasterization.cmd_set_depth_clamp_enable_sets_depth_clip_enable = true;
            if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_DEPTH_CLAMP_ENABLE)) {
               pipeline->pre_rasterization.pa_cl_clip_cntl_clear &=
                  TERAKAN_STATE_DRAW_DEPTH_CLIP_ENABLE_PA_CL_CLIP_CNTL_CLEAR;
               pipeline->pre_rasterization.pa_cl_clip_cntl |=
                  terakan_state_draw_depth_clip_enable_pa_cl_clip_cntl(
                     !state->rs->depth_clamp_enable);
            }
         } else {
            pipeline->pre_rasterization.pa_cl_clip_cntl_clear &=
               TERAKAN_STATE_DRAW_DEPTH_CLIP_ENABLE_PA_CL_CLIP_CNTL_CLEAR;
            pipeline->pre_rasterization.pa_cl_clip_cntl |=
               terakan_state_draw_depth_clip_enable_pa_cl_clip_cntl(state->rs->depth_clip_enable ==
                                                                    VK_MESA_DEPTH_CLIP_ENABLE_TRUE);
         }
      }
   }
   assert(!(pipeline->pre_rasterization.pa_cl_clip_cntl &
            pipeline->pre_rasterization.pa_cl_clip_cntl_clear));
   if (pipeline->pre_rasterization.pa_cl_clip_cntl_clear != UINT32_MAX) {
      BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL);
   }

   if (state->rs != NULL) {
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
            terakan_state_draw_provoking_vertex_mode_pa_su_sc_mode_cntl(
               state->rs->provoking_vertex);
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

      /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_POLY_OFFSET */
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_RS_DEPTH_BIAS_FACTORS)) {
         pipeline->pre_rasterization.pa_su_poly_offset.clamp = state->rs->depth_bias.clamp;
         pipeline->pre_rasterization.pa_su_poly_offset.subpixel_slope_scale =
            TERAKAN_HW_STATE_DRAW_POLY_OFFSET_SLOPE_SUBPIXELS_IN_PIXEL *
            state->rs->depth_bias.slope;
         pipeline->pre_rasterization.pa_su_poly_offset.offset = state->rs->depth_bias.constant;
         pipeline->pre_rasterization.pa_su_poly_offset.representation =
            state->rs->depth_bias.representation;
         pipeline->pre_rasterization.pa_su_poly_offset.representation_exact =
            state->rs->depth_bias.exact;
         BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_POLY_OFFSET);
      }
   }
}

static void
terakan_pipeline_graphics_multisample_init(struct terakan_pipeline_graphics * const pipeline,
                                           struct vk_graphics_pipeline_state const * const state)
{
   if (state->ms != NULL) {
      /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK */
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_MS_SAMPLE_MASK)) {
         pipeline->multisample.pa_sc_aa_mask = (uint16_t)state->ms->sample_mask;
         BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK);
      }
   }
}

static void
terakan_pipeline_graphics_fragment_shader_state_init(
   struct terakan_pipeline_graphics * const pipeline,
   struct vk_graphics_pipeline_state const * const state)
{
   if (state->ds != NULL) {
      /* TERAKAN_PIPELINE_GRAPHICS_STATE_DB_STENCILREFMASK */
      if (BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_DS_STENCIL_TEST_ENABLE) ||
          state->ds->stencil.test_enable) {
         pipeline->fragment_shader.db_stencilrefmask_clear = UINT32_MAX;
         memset(pipeline->fragment_shader.db_stencilrefmask_front_back, 0,
                sizeof(pipeline->fragment_shader.db_stencilrefmask_front_back));
         if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_DS_STENCIL_COMPARE_MASK)) {
            pipeline->fragment_shader.db_stencilrefmask_clear &= C_028430_STENCILMASK;
            pipeline->fragment_shader.db_stencilrefmask_front_back[0] |=
               S_028430_STENCILMASK(state->ds->stencil.front.compare_mask);
            pipeline->fragment_shader.db_stencilrefmask_front_back[1] |=
               S_028430_STENCILMASK(state->ds->stencil.back.compare_mask);
         }
         if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_DS_STENCIL_WRITE_MASK)) {
            pipeline->fragment_shader.db_stencilrefmask_clear &= C_028430_STENCILWRITEMASK;
            pipeline->fragment_shader.db_stencilrefmask_front_back[0] |=
               S_028430_STENCILWRITEMASK(state->ds->stencil.front.write_mask);
            pipeline->fragment_shader.db_stencilrefmask_front_back[1] |=
               S_028430_STENCILWRITEMASK(state->ds->stencil.back.write_mask);
         }
         if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_DS_STENCIL_REFERENCE)) {
            pipeline->fragment_shader.db_stencilrefmask_clear &= C_028430_STENCILREF;
            pipeline->fragment_shader.db_stencilrefmask_front_back[0] |=
               S_028430_STENCILREF(state->ds->stencil.front.reference);
            pipeline->fragment_shader.db_stencilrefmask_front_back[1] |=
               S_028430_STENCILREF(state->ds->stencil.back.reference);
         }
         assert(!((pipeline->fragment_shader.db_stencilrefmask_front_back[0] |
                   pipeline->fragment_shader.db_stencilrefmask_front_back[1]) &
                  pipeline->fragment_shader.db_stencilrefmask_clear));
         if (pipeline->fragment_shader.db_stencilrefmask_clear != UINT32_MAX) {
            BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_DB_STENCILREFMASK);
         }
      }

      /* TERAKAN_PIPELINE_GRAPHICS_STATE_DB_DEPTH_CONTROL */
      pipeline->fragment_shader.db_depth_control_clear = UINT32_MAX;
      pipeline->fragment_shader.db_depth_control = 0;
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_DS_DEPTH_TEST_ENABLE)) {
         pipeline->fragment_shader.db_depth_control_clear &= C_028800_Z_ENABLE;
         pipeline->fragment_shader.db_depth_control |=
            S_028800_Z_ENABLE(state->ds->depth.test_enable);
      }
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_DS_DEPTH_WRITE_ENABLE)) {
         pipeline->fragment_shader.db_depth_control_clear &= C_028800_Z_WRITE_ENABLE;
         pipeline->fragment_shader.db_depth_control |=
            S_028800_Z_WRITE_ENABLE(state->ds->depth.write_enable);
      }
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_DS_DEPTH_COMPARE_OP)) {
         pipeline->fragment_shader.db_depth_control_clear &= C_028800_ZFUNC;
         pipeline->fragment_shader.db_depth_control |=
            S_028800_ZFUNC((uint32_t)state->ds->depth.compare_op);
      }
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_DS_STENCIL_TEST_ENABLE)) {
         pipeline->fragment_shader.db_depth_control_clear &= C_028800_STENCIL_ENABLE;
         pipeline->fragment_shader.db_depth_control |=
            S_028800_STENCIL_ENABLE(state->ds->stencil.test_enable);
      }
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_DS_STENCIL_OP)) {
         pipeline->fragment_shader.db_depth_control_clear &=
            C_028800_STENCILFAIL & C_028800_STENCILZPASS & C_028800_STENCILZFAIL &
            C_028800_STENCILFUNC & C_028800_STENCILFAIL_BF & C_028800_STENCILZPASS_BF &
            C_028800_STENCILZFAIL_BF & C_028800_STENCILFUNC_BF;
         pipeline->fragment_shader.db_depth_control |=
            S_028800_STENCILFAIL((uint32_t)state->ds->stencil.front.op.fail) |
            S_028800_STENCILZPASS((uint32_t)state->ds->stencil.front.op.pass) |
            S_028800_STENCILZFAIL((uint32_t)state->ds->stencil.front.op.depth_fail) |
            S_028800_STENCILFUNC((uint32_t)state->ds->stencil.front.op.compare) |
            S_028800_STENCILFAIL_BF((uint32_t)state->ds->stencil.back.op.fail) |
            S_028800_STENCILZPASS_BF((uint32_t)state->ds->stencil.back.op.pass) |
            S_028800_STENCILZFAIL_BF((uint32_t)state->ds->stencil.back.op.depth_fail) |
            S_028800_STENCILFUNC_BF((uint32_t)state->ds->stencil.back.op.compare);
      }
      assert(!(pipeline->fragment_shader.db_depth_control &
               pipeline->fragment_shader.db_depth_control_clear));
      if (pipeline->fragment_shader.db_depth_control_clear != UINT32_MAX) {
         BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_DB_DEPTH_CONTROL);
      }
   }
}

static void
terakan_pipeline_graphics_fragment_output_init(struct terakan_pipeline_graphics * const pipeline,
                                               struct vk_graphics_pipeline_state const * const state)
{
   if (state->cb != NULL) {
      /* TERAKAN_PIPELINE_GRAPHICS_STATE_LOGIC_OP_ENABLE */
      bool logic_op_potentially_enabled = true;
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_CB_LOGIC_OP_ENABLE)) {
         bool const logic_op_enable = state->cb->logic_op_enable;
         pipeline->fragment_output.logic_op_enable = logic_op_enable;
         BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_LOGIC_OP_ENABLE);
         logic_op_potentially_enabled = logic_op_enable;
      }

      /* TERAKAN_PIPELINE_GRAPHICS_STATE_LOGIC_OP_ROP3
       * Optimize out if the logical operation is known to be disabled.
       */
      if (logic_op_potentially_enabled &&
          !BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_CB_LOGIC_OP)) {
         pipeline->fragment_output.logic_op_rop3 =
            terakan_state_draw_logic_op_rop3((VkLogicOp)state->cb->logic_op);
         BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_LOGIC_OP_ROP3);
      }

      /* TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_RGBA */
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_CB_BLEND_CONSTANTS)) {
         memcpy(pipeline->fragment_output.cb_blend_rgba, state->cb->blend_constants,
                sizeof(float) * 4);
         BITSET_SET(pipeline->static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_RGBA);
      }

      /* Must be ignored if all states using it are dynamic, don't insert assertions unless it's
       * actually used.
       */
      pipeline->fragment_output.color_blend_attachment_count = state->cb->attachment_count;

      memset(pipeline->fragment_output.cb_blend_control, 0,
             sizeof(pipeline->fragment_output.cb_blend_control));

      /* TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_ENABLE */
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_CB_BLEND_ENABLES)) {
         for (uint32_t attachment_index = 0;
              attachment_index < pipeline->fragment_output.color_blend_attachment_count;
              ++attachment_index) {
            pipeline->fragment_output.cb_blend_control[attachment_index] |=
               S_028780_BLEND_CONTROL_ENABLE(state->cb->attachments[attachment_index].blend_enable);
         }
         BITSET_SET(pipeline->static_state,
                    TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_ENABLE);
      }

      /* TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_EQUATION */
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_CB_BLEND_EQUATIONS)) {
         for (uint32_t attachment_index = 0;
              attachment_index < pipeline->fragment_output.color_blend_attachment_count;
              ++attachment_index) {
            struct vk_color_blend_attachment_state const * const attachment =
               &state->cb->attachments[attachment_index];
            bool const allow_dual_source = attachment_index == 0;
            /* According to VkPipelineColorBlendAttachmentState VUIDs, all factors and operations
             * must be valid values regardless of blendEnable, so there's no need to handle
             * blendEnable being static here.
             */
            pipeline->fragment_output.cb_blend_control[attachment_index] |=
               S_028780_COLOR_SRCBLEND(terakan_state_draw_blend_factor_translate(
                  attachment->src_color_blend_factor, allow_dual_source)) |
               S_028780_COLOR_DESTBLEND(terakan_state_draw_blend_factor_translate(
                  attachment->dst_color_blend_factor, allow_dual_source)) |
               S_028780_COLOR_COMB_FCN(
                  terakan_state_draw_blend_op_translate(attachment->color_blend_op));
            if (attachment->src_alpha_blend_factor != attachment->src_color_blend_factor ||
                attachment->dst_alpha_blend_factor != attachment->dst_color_blend_factor ||
                attachment->alpha_blend_op != attachment->color_blend_op) {
               pipeline->fragment_output.cb_blend_control[attachment_index] |=
                  S_028780_SEPARATE_ALPHA_BLEND(1) |
                  S_028780_ALPHA_SRCBLEND(terakan_state_draw_blend_factor_translate(
                     attachment->src_alpha_blend_factor, allow_dual_source)) |
                  S_028780_ALPHA_DESTBLEND(terakan_state_draw_blend_factor_translate(
                     attachment->dst_alpha_blend_factor, allow_dual_source)) |
                  S_028780_ALPHA_COMB_FCN(
                     terakan_state_draw_blend_op_translate(attachment->alpha_blend_op));
            }
         }
         BITSET_SET(pipeline->static_state,
                    TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_EQUATION);
      }

      /* TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_MASK */
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_CB_WRITE_MASKS)) {
         for (uint32_t attachment_index = 0;
              attachment_index < pipeline->fragment_output.color_blend_attachment_count;
              ++attachment_index) {
            pipeline->fragment_output.color_attachment_write_masks[attachment_index] =
               state->cb->attachments[attachment_index].write_mask & 0b1111;
         }
         BITSET_SET(pipeline->static_state,
                    TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_MASK);
      }

      /* TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_ENABLE */
      if (!BITSET_TEST(state->dynamic, MESA_VK_DYNAMIC_CB_COLOR_WRITE_ENABLES)) {
         pipeline->fragment_output.color_attachment_write_enable =
            state->cb->color_write_enables & BITFIELD_MASK(TERAKAN_COLOR_HW_RTV_COUNT);
         BITSET_SET(pipeline->static_state,
                    TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_ENABLE);
      }
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

   struct terakan_pipeline_layout const * const pipeline_layout =
      terakan_pipeline_layout_from_handle(create_info->layout);

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

      struct terakan_shader_impl * const shader = &pipeline->shaders[stage_index];
      memset(shader, 0, sizeof(*shader));

      shader->push_constants_usage.app_extent_bytes =
         pipeline_layout->shader_app_push_constants_extents_bytes[stage_index];

      terakan_shader_lower_and_optimize_post_link(nir, pipeline_layout, shader->resources_needed,
                                                  &shader->samplers_needed,
                                                  &shader->fs.fragment_data_uncompacted_locations);

      /* TODO(Triang3l): Construct the shader key from the NIR and, when available, the pipeline
       * state.
       */
      union r600_shader_key shader_key = {0};
      if (stage_index == MESA_SHADER_FRAGMENT) {
         shader_key.ps.nr_cbufs = util_bitcount(shader->fs.fragment_data_uncompacted_locations);
      }

      result = terakan_shader_impl_compile(shader, device, &shader_key, nir, allocator);
      ralloc_free(nir);
      if (result != VK_SUCCESS) {
         goto fail_shaders;
      }

      /* Fully initialized now, make sure it's fully cleaned up in case of failure. */
      pipeline->shader_stages |= stage_info->stage;
   }

   /* Vertex shader is mandatory if the pre-rasterization part is present. Fail to create the
    * pipeline to let other places in the pipeline code assume this more consistently.
    */
   /* TODO(Triang3l): Don't do this for graphics pipeline libraries without the pre-rasterization
    * part.
    */
   assert(pipeline->shader_stages & VK_SHADER_STAGE_VERTEX_BIT);
   if (unlikely(!(pipeline->shader_stages & VK_SHADER_STAGE_VERTEX_BIT))) {
      result = vk_errorf(device, VK_ERROR_VALIDATION_FAILED_EXT,
                         "No vertex shader in the graphics pipeline");
      goto fail_shaders;
   }

   BITSET_ZERO(pipeline->static_state);
   struct vk_graphics_pipeline_all_state all_state;
   struct vk_graphics_pipeline_state state = {0};
   result = vk_graphics_pipeline_state_fill(&device->vk, &state, create_info, NULL, 0, &all_state,
                                            NULL, VK_SYSTEM_ALLOCATION_SCOPE_OBJECT, NULL);
   if (result != VK_SUCCESS) {
      goto fail_shaders;
   }
   result = terakan_pipeline_graphics_vertex_input_init(
      pipeline, &state, device,
      pipeline->shader_stages & VK_SHADER_STAGE_VERTEX_BIT
         ? pipeline->shaders[MESA_SHADER_VERTEX].vs.vertex_attributes_needed
         : NULL,
      allocator);
   if (result != VK_SUCCESS) {
      goto fail_shaders;
   }
   terakan_pipeline_graphics_pre_rasterization_init(
      pipeline, &state, device->vk.enabled_extensions.EXT_depth_range_unrestricted);
   terakan_pipeline_graphics_multisample_init(pipeline, &state);
   terakan_pipeline_graphics_fragment_shader_state_init(pipeline, &state);
   terakan_pipeline_graphics_fragment_output_init(pipeline, &state);

   *pipeline_out = pipeline;
   return VK_SUCCESS;

fail_shaders: {
   unsigned remaining_shader_stages = (unsigned)pipeline->shader_stages;
   while (remaining_shader_stages) {
      terakan_shader_impl_finish(
         &pipeline->shaders[vk_to_mesa_shader_stage((
            VkShaderStageFlagBits)((VkShaderStageFlags)1 << u_bit_scan(&remaining_shader_stages)))],
         allocator);
   }
}
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
      struct terakan_pipeline_graphics * pipeline = NULL;
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
