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

#include "gallium/drivers/r600/evergreend.h"
#include "util/macros.h"
#include "util/u_endian.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

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
terakan_state_draw_apply_color(struct terakan_gfx_command_writer * const command_writer,
                               enum terakan_state_draw_index const state_index)
{
   uint32_t const color_index = (uint32_t)state_index - (uint32_t)TERAKAN_STATE_DRAW_CB_COLOR_FIRST;
   struct terakan_winsys_bo const * const color_bo =
      command_writer->state_draw.cb_color_bo[color_index];
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
terakan_state_draw_apply_color_rat_only(struct terakan_gfx_command_writer * const command_writer,
                                        enum terakan_state_draw_index const state_index)
{
   uint32_t const color_index = (uint32_t)state_index - (uint32_t)TERAKAN_STATE_DRAW_CB_COLOR_FIRST;
   struct terakan_winsys_bo const * const color_bo =
      command_writer->state_draw.cb_color_bo[color_index];
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
      [TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL] = terakan_state_draw_apply_pa_cl_clip_cntl,
      [TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL] = terakan_state_draw_apply_pa_su_sc_mode_cntl,
      [TERAKAN_STATE_DRAW_PA_CL_VTE_CNTL] = terakan_state_draw_apply_pa_cl_vte_cntl,
      [TERAKAN_STATE_DRAW_PA_SC_AA_MASK] = terakan_state_draw_apply_pa_sc_aa_mask,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST] = terakan_state_draw_apply_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 1] = terakan_state_draw_apply_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 2] = terakan_state_draw_apply_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 3] = terakan_state_draw_apply_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 4] = terakan_state_draw_apply_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 5] = terakan_state_draw_apply_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 6] = terakan_state_draw_apply_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 7] = terakan_state_draw_apply_color,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 8] = terakan_state_draw_apply_color_rat_only,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 9] = terakan_state_draw_apply_color_rat_only,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 10] = terakan_state_draw_apply_color_rat_only,
      [TERAKAN_STATE_DRAW_CB_COLOR_FIRST + 11] = terakan_state_draw_apply_color_rat_only,
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
terakan_state_draw_reset(struct terakan_state_draw * const state)
{
   BITSET_ZERO(state->state_ever_written);

   /* Initialize state setters themselves, and state that setters modify partially (such as via
    * terakan_state_draw_replace_fields), as well as state that other state depends on, to values
    * corresponding to their original Vulkan state values being zero (like zeroed via memset), or,
    * for state items from extensions, to their values without the structure containing the field
    * being in the pNext chain.
    */

   /* No VK_DYNAMIC_STATE_DEPTH_CLIP_ENABLE_EXT,
    * no VkPipelineRasterizationDepthClipStateCreateInfoEXT */
   state->cmd_set_depth_clamp_enable_sets_depth_clip_enable = true;

#if UTIL_ARCH_BIG_ENDIAN
   state->vgt_index_type = VGT_INDEX_16 | VGT_DMA_SWAP_16_BIT;
#else
   state->vgt_index_type = VGT_INDEX_16;
#endif
   BITSET_SET(state->state_ever_written, TERAKAN_STATE_DRAW_VGT_INDEX_TYPE);

   state->pa_cl_clip_cntl =
      /* negativeOneToOne = VK_FALSE */
      S_028810_DX_CLIP_SPACE_DEF(1) |
      /* rasterizerDiscardEnable = VK_FALSE */
      S_028810_DX_RASTERIZATION_KILL(0) |
      /* Always enabled. */
      S_028810_DX_LINEAR_ATTR_CLIP_ENA(1) |
      /* depthClampEnable = VK_FALSE, no VkPipelineRasterizationDepthClipStateCreateInfoEXT */
      S_028810_ZCLIP_NEAR_DISABLE(0) | S_028810_ZCLIP_FAR_DISABLE(0);
   BITSET_SET(state->state_ever_written, TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL);

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
   BITSET_SET(state->state_ever_written, TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL);

   /* Clear modified bits, and make sure the defaults are applied before the first draw. */
   BITSET_COPY(state->state_pending, state->state_ever_written);
}
