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

#include "terakan_state.h"

#include "terakan_command_buffer.h"
#include "terakan_hw_state.h"
#include "terakan_image.h"
#include "terakan_state_rasterization.h"

#include "gallium/drivers/r600/evergreend.h"
#include "util/macros.h"
#include "util/u_endian.h"
#include "util/u_math.h"

#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void
terakan_state_translate_window_rectangle_unpacked(VkRect2D const * const rectangle,
                                                  uint16_t tl_br_xy_out[4])
{
   if (unlikely(rectangle->offset.x > TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES - 1 ||
                rectangle->offset.y > TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES - 1)) {
      /* For top-left, the maximum value in the register is 2^n-1, not 2^n. */
      memset(tl_br_xy_out, 0, sizeof(*tl_br_xy_out) * 4);
      return;
   }

   tl_br_xy_out[0] = CLAMP(rectangle->offset.x, 0, TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES - 1);
   tl_br_xy_out[1] = CLAMP(rectangle->offset.y, 0, TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES - 1);
   tl_br_xy_out[2] = CLAMP(rectangle->offset.x + rectangle->extent.width, 0,
                           TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES);
   tl_br_xy_out[3] = CLAMP(rectangle->offset.y + rectangle->extent.height, 0,
                           TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES);
}

typedef void (*terakan_state_draw_apply_function)(
   struct terakan_gfx_command_writer * command_writer, enum terakan_state_draw_index state_index);

static void
terakan_state_draw_apply_vgt_index_type(struct terakan_gfx_command_writer * const command_writer,
                                        UNUSED enum terakan_state_draw_index const state_index)
{
   bool const modified =
      command_writer->hw_state_draw.vgt_index_type != command_writer->state_draw.vgt_index_type;
   command_writer->hw_state_draw.vgt_index_type = command_writer->state_draw.vgt_index_type;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_VGT_INDEX_TYPE, modified);
}

static void
terakan_state_draw_apply_vgt_primitive_type(struct terakan_gfx_command_writer * const command_writer,
                                            UNUSED enum terakan_state_draw_index const state_index)
{
   bool const modified = command_writer->hw_state_draw.vgt_primitive_type !=
                         command_writer->state_draw.vgt_primitive_type;
   command_writer->hw_state_draw.vgt_primitive_type = command_writer->state_draw.vgt_primitive_type;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_VGT_PRIMITIVE_TYPE, modified);
}

static void
terakan_state_draw_apply_vgt_index_offset(struct terakan_gfx_command_writer * const command_writer,
                                          UNUSED enum terakan_state_draw_index const state_index)
{
   bool const modified =
      command_writer->hw_state_draw.vgt_index_offset != command_writer->state_draw.vgt_index_offset;
   command_writer->hw_state_draw.vgt_index_offset = command_writer->state_draw.vgt_index_offset;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_VGT_INDEX_OFFSET, modified);
}

static void
terakan_state_draw_apply_pa_cl_vport_xy_scale_offset(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_state_draw_index const state_index)
{
   terakan_hw_state_draw_ensure_viewport_count(&command_writer->hw_state_draw,
                                               command_writer->state_draw.viewport_count);
   for (uint32_t viewport_index = 0; viewport_index < command_writer->state_draw.viewport_count;
        ++viewport_index) {
      if (memcmp(
             command_writer->hw_state_draw.viewports[viewport_index].pa_cl_vport_xy_scale_offset,
             command_writer->state_draw.viewports[viewport_index].pa_cl_vport_xy_scale_offset,
             sizeof(command_writer->state_draw.viewports[viewport_index]
                       .pa_cl_vport_xy_scale_offset)) != 0) {
         memcpy(
            command_writer->hw_state_draw.viewports[viewport_index].pa_cl_vport_xy_scale_offset,
            command_writer->state_draw.viewports[viewport_index].pa_cl_vport_xy_scale_offset,
            sizeof(
               command_writer->state_draw.viewports[viewport_index].pa_cl_vport_xy_scale_offset));
         terakan_hw_state_draw_viewport_modified(
            &command_writer->hw_state_draw, viewport_index,
            TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_CL_VPORT_XY_SCALE_OFFSET);
      }
   }
}

static void
terakan_state_draw_apply_pa_cl_vport_z_scale_offset(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_state_draw_index const state_index)
{
   terakan_hw_state_draw_ensure_viewport_count(&command_writer->hw_state_draw,
                                               command_writer->state_draw.viewport_count);
   bool const dx_clip_space_def =
      G_028810_DX_CLIP_SPACE_DEF(command_writer->hw_state_draw.pa_cl_clip_cntl) != 0;
   for (uint32_t viewport_index = 0; viewport_index < command_writer->state_draw.viewport_count;
        ++viewport_index) {
      struct terakan_state_draw_viewport const * const viewport =
         &command_writer->state_draw.viewports[viewport_index];
      float const * const z_scale_offset =
         &viewport->pa_cl_vport_z_gl_dx_scale_offset[(int)dx_clip_space_def][0];
      if (memcmp(command_writer->hw_state_draw.viewports[viewport_index].pa_cl_vport_z_scale_offset,
                 z_scale_offset, sizeof(float) * 2) != 0) {
         memcpy(command_writer->hw_state_draw.viewports[viewport_index].pa_cl_vport_z_scale_offset,
                z_scale_offset, sizeof(float) * 2);
         terakan_hw_state_draw_viewport_modified(
            &command_writer->hw_state_draw, viewport_index,
            TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_CL_VPORT_Z_SCALE_OFFSET);
      }
   }
}

static void
terakan_state_draw_apply_pa_cl_gb(struct terakan_gfx_command_writer * const command_writer,
                                  UNUSED enum terakan_state_draw_index const state_index)
{
   float pa_cl_gb_vert_horz_clip_disc_adj[][2] = {{FLT_MAX, 1.0f}, {FLT_MAX, 1.0f}};
   for (uint32_t viewport_index = 0; viewport_index < command_writer->state_draw.viewport_count;
        ++viewport_index) {
      for (int axis = 0; axis < 2; ++axis) {
         pa_cl_gb_vert_horz_clip_disc_adj[axis][0] = MIN2(
            command_writer->state_draw.viewports[viewport_index].pa_cl_gb_vert_horz_clip_adj[axis],
            pa_cl_gb_vert_horz_clip_disc_adj[axis][0]);
      }
   }
   /* TODO(Triang3l): Discard rectangle ratio for points and lines. */
   bool const modified =
      memcmp(command_writer->hw_state_draw.pa_cl_gb_vert_horz_clip_disc_adj,
             pa_cl_gb_vert_horz_clip_disc_adj, sizeof(pa_cl_gb_vert_horz_clip_disc_adj)) != 0;
   memcpy(command_writer->hw_state_draw.pa_cl_gb_vert_horz_clip_disc_adj,
          pa_cl_gb_vert_horz_clip_disc_adj, sizeof(pa_cl_gb_vert_horz_clip_disc_adj));
   terakan_hw_state_draw_written(&command_writer->hw_state_draw, TERAKAN_HW_STATE_DRAW_PA_CL_GB,
                                 modified);
}

static void
terakan_state_draw_apply_pa_sc_vport_scissor(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_state_draw_index const state_index)
{
   terakan_hw_state_draw_ensure_viewport_count(&command_writer->hw_state_draw,
                                               command_writer->state_draw.viewport_count);
   for (uint32_t viewport_index = 0; viewport_index < command_writer->state_draw.viewport_count;
        ++viewport_index) {
      uint16_t const * const viewport_scissor =
         command_writer->state_draw.viewports[viewport_index].pa_sc_vport_scissor_tl_br_xy[0];
      uint16_t const * const generic_scissor =
         command_writer->state_draw.pa_sc_vport_generic_scissor_tl_br_xy[viewport_index][0];
      uint16_t scissor[] = {
         MAX2(viewport_scissor[0], generic_scissor[0]),
         MAX2(viewport_scissor[1], generic_scissor[1]),
         MIN2(viewport_scissor[2], generic_scissor[2]),
         MIN2(viewport_scissor[3], generic_scissor[3]),
      };
      terakan_state_draw_finalize_scissor(scissor);
      uint32_t const pa_sc_vport_scissor[] = {
         S_028250_TL_X(scissor[0]) | S_028250_TL_Y(scissor[1]) | S_028250_WINDOW_OFFSET_DISABLE(1),
         S_028254_BR_X(scissor[2]) | S_028254_BR_Y(scissor[3]),
      };
      if (memcmp(command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_scissor,
                 pa_sc_vport_scissor, sizeof(pa_sc_vport_scissor)) != 0) {
         memcpy(command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_scissor,
                pa_sc_vport_scissor, sizeof(pa_sc_vport_scissor));
         terakan_hw_state_draw_viewport_modified(
            &command_writer->hw_state_draw, viewport_index,
            TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_SC_VPORT_SCISSOR);
      }
   }
}

static void
terakan_state_draw_apply_pa_sc_vport_z_min_max(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_state_draw_index const state_index)
{
   terakan_hw_state_draw_ensure_viewport_count(&command_writer->hw_state_draw,
                                               command_writer->state_draw.viewport_count);
   if (command_writer->state_draw.pa_sc_vport_z_min_0_max_1) {
      float const zero_one[] = {0.0f, 1.0f};
      for (uint32_t viewport_index = 0; viewport_index < command_writer->state_draw.viewport_count;
           ++viewport_index) {
         if (memcmp(command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_z_min_max,
                    &zero_one, sizeof(float) * 2) != 0) {
            memcpy(command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_z_min_max,
                   &zero_one, sizeof(float) * 2);
            terakan_hw_state_draw_viewport_modified(
               &command_writer->hw_state_draw, viewport_index,
               TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_SC_VPORT_Z_MIN_MAX);
         }
      }
   } else {
      for (uint32_t viewport_index = 0; viewport_index < command_writer->state_draw.viewport_count;
           ++viewport_index) {
         if (memcmp(command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_z_min_max,
                    command_writer->state_draw.viewports[viewport_index].pa_sc_vport_z_min_max,
                    sizeof(float) * 2) != 0) {
            memcpy(command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_z_min_max,
                   command_writer->state_draw.viewports[viewport_index].pa_sc_vport_z_min_max,
                   sizeof(float) * 2);
            terakan_hw_state_draw_viewport_modified(
               &command_writer->hw_state_draw, viewport_index,
               TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_SC_VPORT_Z_MIN_MAX);
         }
      }
   }
}

static void
terakan_state_draw_apply_pa_cl_clip_cntl(struct terakan_gfx_command_writer * const command_writer,
                                         UNUSED enum terakan_state_draw_index const state_index)
{
   bool const modified =
      command_writer->hw_state_draw.pa_cl_clip_cntl != command_writer->state_draw.pa_cl_clip_cntl;
   command_writer->hw_state_draw.pa_cl_clip_cntl = command_writer->state_draw.pa_cl_clip_cntl;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_PA_CL_CLIP_CNTL, modified);
}

static void
terakan_state_draw_apply_pa_su_sc_mode_cntl(struct terakan_gfx_command_writer * const command_writer,
                                            UNUSED enum terakan_state_draw_index const state_index)
{
   bool const modified = command_writer->hw_state_draw.pa_su_sc_mode_cntl !=
                         command_writer->state_draw.pa_su_sc_mode_cntl;
   command_writer->hw_state_draw.pa_su_sc_mode_cntl = command_writer->state_draw.pa_su_sc_mode_cntl;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_PA_SU_SC_MODE_CNTL, modified);
}

static void
terakan_state_draw_apply_pa_cl_vte_cntl(struct terakan_gfx_command_writer * const command_writer,
                                        UNUSED enum terakan_state_draw_index const state_index)
{
   uint32_t const pa_cl_vte_cntl = S_028818_VPORT_X_SCALE_ENA(1) | S_028818_VPORT_X_OFFSET_ENA(1) |
                                   S_028818_VPORT_Y_SCALE_ENA(1) | S_028818_VPORT_Y_OFFSET_ENA(1) |
                                   S_028818_VPORT_Z_SCALE_ENA(1) | S_028818_VPORT_Z_OFFSET_ENA(1) |
                                   S_028818_VTX_W0_FMT(1);
   bool const modified = command_writer->hw_state_draw.pa_cl_vte_cntl != pa_cl_vte_cntl;
   command_writer->hw_state_draw.pa_cl_vte_cntl = pa_cl_vte_cntl;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_PA_CL_VTE_CNTL, modified);
}

static void
terakan_state_draw_apply_pa_sc_mode_cntl_0(struct terakan_gfx_command_writer * const command_writer,
                                           UNUSED enum terakan_state_draw_index const state_index)
{
   /* TODO(Triang3l): MSAA_ENABLE, LINE_STIPPLE_ENABLE from a variable. */
   uint32_t const pa_sc_mode_cntl_0 = S_028A48_VPORT_SCISSOR_ENABLE(1);
   bool const modified = command_writer->hw_state_draw.pa_sc_mode_cntl_0 != pa_sc_mode_cntl_0;
   command_writer->hw_state_draw.pa_sc_mode_cntl_0 = pa_sc_mode_cntl_0;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_PA_SC_MODE_CNTL_0, modified);
}

static void
terakan_state_draw_apply_pa_sc_aa_mask(struct terakan_gfx_command_writer * const command_writer,
                                       UNUSED enum terakan_state_draw_index const state_index)
{
   bool const modified =
      command_writer->hw_state_draw.pa_sc_aa_mask != command_writer->state_draw.pa_sc_aa_mask;
   command_writer->hw_state_draw.pa_sc_aa_mask = command_writer->state_draw.pa_sc_aa_mask;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_PA_SC_AA_MASK, modified);
}

static void
terakan_state_draw_apply_db_render_override(struct terakan_gfx_command_writer * const command_writer,
                                            UNUSED enum terakan_state_draw_index const state_index)
{
   bool const modified = command_writer->hw_state_draw.db_render_override !=
                         command_writer->state_draw.db_render_override;
   command_writer->hw_state_draw.db_render_override = command_writer->state_draw.db_render_override;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_DB_RENDER_OVERRIDE, modified);
}

static void
terakan_state_draw_apply_cb_color(struct terakan_gfx_command_writer * const command_writer,
                                  enum terakan_state_draw_index const state_index)
{
   uint32_t const color_index = (uint32_t)state_index - (uint32_t)TERAKAN_STATE_DRAW_CB_COLOR_FIRST;
   struct terakan_bo const * const color_bo = command_writer->state_draw.cb_color_bo[color_index];
   bool modified = command_writer->hw_state_draw.cb_color_bo[color_index] != color_bo;
   command_writer->hw_state_draw.cb_color_bo[color_index] = color_bo;
   if (color_bo != NULL) {
      if (modified ||
          memcmp(&command_writer->hw_state_draw.cb_color[color_index],
                 &command_writer->state_draw.cb_color[color_index],
                 sizeof(struct terakan_color_descriptor)) != 0 ||
          memcmp(&command_writer->hw_state_draw.cb_color_meta[color_index],
                 &command_writer->state_draw.cb_color_meta[color_index],
                 sizeof(struct terakan_color_meta_descriptor)) != 0) {
         modified = true;
         memcpy(&command_writer->hw_state_draw.cb_color[color_index],
                &command_writer->state_draw.cb_color[color_index],
                sizeof(struct terakan_color_descriptor));
         memcpy(&command_writer->hw_state_draw.cb_color_meta[color_index],
                &command_writer->state_draw.cb_color_meta[color_index],
                sizeof(struct terakan_color_meta_descriptor));
      }
   }
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 (enum terakan_hw_state_draw_index)(
                                    (uint32_t)TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + color_index),
                                 modified);
}

static void
terakan_state_draw_apply_cb_color_rat_only(struct terakan_gfx_command_writer * const command_writer,
                                           enum terakan_state_draw_index const state_index)
{
   uint32_t const color_index = (uint32_t)state_index - (uint32_t)TERAKAN_STATE_DRAW_CB_COLOR_FIRST;
   struct terakan_bo const * const color_bo = command_writer->state_draw.cb_color_bo[color_index];
   bool modified = command_writer->hw_state_draw.cb_color_bo[color_index] != color_bo;
   command_writer->hw_state_draw.cb_color_bo[color_index] = color_bo;
   if (color_bo != NULL) {
      if (!modified && memcmp(&command_writer->hw_state_draw.cb_color[color_index],
                              &command_writer->state_draw.cb_color[color_index],
                              sizeof(struct terakan_color_descriptor)) != 0) {
         modified = true;
      }
      memcpy(&command_writer->hw_state_draw.cb_color[color_index],
             &command_writer->state_draw.cb_color[color_index],
             sizeof(struct terakan_color_descriptor));
   }
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 (enum terakan_hw_state_draw_index)(
                                    (uint32_t)TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + color_index),
                                 modified);
}

static terakan_state_draw_apply_function const
   terakan_state_draw_apply_functions[TERAKAN_STATE_DRAW_COUNT] = {
      [TERAKAN_STATE_DRAW_VGT_INDEX_TYPE] = terakan_state_draw_apply_vgt_index_type,
      [TERAKAN_STATE_DRAW_VGT_PRIMITIVE_TYPE] = terakan_state_draw_apply_vgt_primitive_type,
      [TERAKAN_STATE_DRAW_VGT_INDEX_OFFSET] = terakan_state_draw_apply_vgt_index_offset,
      [TERAKAN_STATE_DRAW_PA_CL_VPORT_XY_SCALE_OFFSET] =
         terakan_state_draw_apply_pa_cl_vport_xy_scale_offset,
      [TERAKAN_STATE_DRAW_PA_CL_VPORT_Z_SCALE_OFFSET] =
         terakan_state_draw_apply_pa_cl_vport_z_scale_offset,
      [TERAKAN_STATE_DRAW_PA_CL_GB] = terakan_state_draw_apply_pa_cl_gb,
      [TERAKAN_STATE_DRAW_PA_SC_VPORT_SCISSOR] = terakan_state_draw_apply_pa_sc_vport_scissor,
      [TERAKAN_STATE_DRAW_PA_SC_VPORT_Z_MIN_MAX] = terakan_state_draw_apply_pa_sc_vport_z_min_max,
      [TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL] = terakan_state_draw_apply_pa_cl_clip_cntl,
      [TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL] = terakan_state_draw_apply_pa_su_sc_mode_cntl,
      [TERAKAN_STATE_DRAW_PA_CL_VTE_CNTL] = terakan_state_draw_apply_pa_cl_vte_cntl,
      [TERAKAN_STATE_DRAW_PA_SC_MODE_CNTL_0] = terakan_state_draw_apply_pa_sc_mode_cntl_0,
      [TERAKAN_STATE_DRAW_PA_SC_AA_MASK] = terakan_state_draw_apply_pa_sc_aa_mask,
      [TERAKAN_STATE_DRAW_DB_RENDER_OVERRIDE] = terakan_state_draw_apply_db_render_override,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST] = terakan_state_draw_apply_cb_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 1] = terakan_state_draw_apply_cb_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 2] = terakan_state_draw_apply_cb_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 3] = terakan_state_draw_apply_cb_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 4] = terakan_state_draw_apply_cb_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 5] = terakan_state_draw_apply_cb_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 6] = terakan_state_draw_apply_cb_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 7] = terakan_state_draw_apply_cb_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 8] = terakan_state_draw_apply_cb_color_rat_only,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 9] = terakan_state_draw_apply_cb_color_rat_only,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 10] = terakan_state_draw_apply_cb_color_rat_only,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 11] = terakan_state_draw_apply_cb_color_rat_only,
};

void
terakan_state_draw_apply_pending(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_state_draw * const state = &command_writer->state_draw;
   /* Manual loop instead of BITSET_FOREACH_SET, reading the pending bits at each iteration so
    * application of state items with lower indices can make state items with higher indices pending
    * if the latter depend on the former.
    */
   unsigned next_state_index = 0;
   while (next_state_index < TERAKAN_STATE_DRAW_COUNT) {
      unsigned const next_state_word_index = BITSET_BITWORD(next_state_index);
      BITSET_WORD const next_state_word_remaining =
         state->state_pending[next_state_word_index] & ~(BITSET_BIT(next_state_index) - 1);
      if (!next_state_word_remaining) {
         next_state_index = BITSET_WORDBITS * (next_state_word_index + 1);
         continue;
      }
      unsigned const state_index =
         BITSET_WORDBITS * next_state_word_index + (ffs(next_state_word_remaining) - 1);
      if (state_index >= TERAKAN_STATE_DRAW_COUNT) {
         /* Ignore the bits beyond the end of the bitset. */
         break;
      }
      terakan_state_draw_apply_functions[state_index](command_writer,
                                                      (enum terakan_state_draw_index)state_index);
      BITSET_CLEAR(state->state_pending, state_index);
      next_state_index = state_index + 1;
   }
}

void
terakan_state_draw_reset(struct terakan_state_draw * const state,
                         bool const depth_range_unrestricted)
{
   BITSET_ZERO(state->state_pending);

   /* Initialize the state to the default values, corresponding to one of the following that's
    * applicable:
    * - For optional features, the feature is not enabled.
    * - For state configured via a structure in a pNext chain, the structure is missing.
    * - The field in the original structure is zero (like if it was zeroed via memset or {}).
    *
    * While Vulkan drivers are not required to do this because complete state for all enabled
    * features must be configured before a draw (via a pipeline object or dynamic state), do this
    * for simplicity:
    * - Certain state items correspond to hardware registers containing multiple fields, and those
    *   fields are configured individually separately by state setters. In this case the rest of the
    *   fields (the ones not exposed to the application) in those registers must be set to their
    *   default values.
    * - Optional features not enabled on the device don't need to be handled explicitly, especially
    *   when the application uses only dynamic state and never calls the setters for certain state
    *   items.
    * - There may be conditional logic based on the latest values of the state items in some
    *   places, make sure it doesn't read uninitialized variables.
    */

   /* No VK_DYNAMIC_STATE_DEPTH_CLIP_ENABLE_EXT,
    * no VkPipelineRasterizationDepthClipStateCreateInfoEXT */
   state->cmd_set_depth_clamp_enable_sets_depth_clip_enable = true;

   /* indexType = VK_INDEX_TYPE_UINT16 */
#if UTIL_ARCH_BIG_ENDIAN
   state->vgt_index_type = VGT_INDEX_16 | VGT_DMA_SWAP_16_BIT;
#else
   state->vgt_index_type = VGT_INDEX_16;
#endif

   /* topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST */
   state->vgt_primitive_type = S_008958_PRIM_TYPE(V_008958_DI_PT_POINTLIST);

   /* firstVertex or vertexOffset = 0 */
   state->vgt_index_offset = 0;

   state->viewport_count = 0;
   memset(state->viewports, 0, sizeof(state->viewports));
   memset(state->pa_sc_vport_generic_scissor_tl_br_xy, 0,
          sizeof(state->pa_sc_vport_generic_scissor_tl_br_xy));

   /* depthClampEnable = VK_FALSE, must be initialized accurately because depthClamp is an optional
    * feature, when it's disabled, VK_FALSE must be assumed if never set by the application.
    */
   state->pa_sc_vport_z_min_0_max_1 = !depth_range_unrestricted;
   /* depthClampEnable = VK_FALSE */
   state->db_render_override = S_02800C_DISABLE_VIEWPORT_CLAMP(depth_range_unrestricted);

   state->pa_cl_clip_cntl =
      /* negativeOneToOne = VK_FALSE */
      S_028810_DX_CLIP_SPACE_DEF(1) |
      /* rasterizerDiscardEnable = VK_FALSE */
      S_028810_DX_RASTERIZATION_KILL(0) |
      /* Always enabled. */
      S_028810_DX_LINEAR_ATTR_CLIP_ENA(1) |
      /* depthClampEnable = VK_FALSE, no VkPipelineRasterizationDepthClipStateCreateInfoEXT */
      S_028810_ZCLIP_NEAR_DISABLE(0) | S_028810_ZCLIP_FAR_DISABLE(0);

   state->pa_su_sc_mode_cntl =
      /* cullMode = VK_CULL_MODE_NONE */
      S_028814_CULL_FRONT(0) | S_028814_CULL_BACK(0) |
      /* frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE */
      S_028814_FACE(0) |
      /* polygonMode = VK_POLYGON_MODE_FILL */
      S_028814_POLY_MODE(V_028814_X_DISABLE_POLY_MODE) |
      S_028814_POLYMODE_FRONT_PTYPE(V_028814_X_DRAW_TRIANGLES) |
      S_028814_POLYMODE_BACK_PTYPE(V_028814_X_DRAW_TRIANGLES) |
      /* depthBiasEnable = VK_FALSE */
      S_028814_POLY_OFFSET_FRONT_ENABLE(0) | S_028814_POLY_OFFSET_BACK_ENABLE(0) |
      S_028814_POLY_OFFSET_PARA_ENABLE(0) |
      /* provokingVertexMode = VK_PROVOKING_VERTEX_MODE_FIRST_VERTEX_EXT */
      S_028814_PROVOKING_VTX_LAST(0);

   /* pSampleMask = NULL */
   state->pa_sc_aa_mask = UINT16_MAX;

   memset(state->cb_color_bo, 0, sizeof(state->cb_color_bo));

   /* Make all state items pending so the defaults are applied before the first draw, even for
    * unsupported features.
    */
   BITSET_ONES(state->state_pending);
}
