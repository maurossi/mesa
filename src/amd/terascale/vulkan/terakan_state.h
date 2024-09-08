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

#ifndef TERAKAN_STATE_H
#define TERAKAN_STATE_H

#include "terakan_bo.h"
#include "terakan_descriptor.h"
#include "terakan_hw_state.h"
#include "terakan_limits.h"
#include "terakan_state_rasterization.h"

#include "util/bitset.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* terakan_state is a layer on top of terakan_hw_state that specifically tracks the state desired by
 * the application. It's not reapplied when switching to a new indirect buffer - terakan_hw_state is
 * instead, for both application's work and internal draws / dispatches. Thus, no register writes
 * must be done directly from terakan_state application - it must first go to terakan_hw_state.
 */

void terakan_state_translate_window_rectangle_unpacked(VkRect2D const * rectangle,
                                                       uint16_t tl_br_xy_out[4]);

enum terakan_state_draw_index {
   TERAKAN_STATE_DRAW_VGT_INDEX_TYPE,

   TERAKAN_STATE_DRAW_VGT_PRIMITIVE_TYPE,

   TERAKAN_STATE_DRAW_VGT_INDEX_OFFSET,

   TERAKAN_STATE_DRAW_PA_CL_VPORT_XY_SCALE_OFFSET,
   TERAKAN_STATE_DRAW_PA_CL_VPORT_Z_SCALE_OFFSET,
   TERAKAN_STATE_DRAW_PA_CL_GB,
   TERAKAN_STATE_DRAW_PA_SC_VPORT_SCISSOR,
   TERAKAN_STATE_DRAW_PA_SC_VPORT_Z_MIN_MAX,

   TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL,

   TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL,

   TERAKAN_STATE_DRAW_PA_CL_VTE_CNTL,

   TERAKAN_STATE_DRAW_PA_SC_MODE_CNTL_0,

   TERAKAN_STATE_DRAW_PA_SC_AA_MASK,

   TERAKAN_STATE_DRAW_DB_RENDER_OVERRIDE,

   TERAKAN_STATE_DRAW_CB_COLOR_FIRST,
   TERAKAN_STATE_DRAW_CB_COLOR_LAST =
      TERAKAN_STATE_DRAW_CB_COLOR_FIRST + TERAKAN_LIMITS_HW_COLOR_RAT_COUNT - 1,

   TERAKAN_STATE_DRAW_COUNT,
};

static_assert(MESA_VK_MAX_VIEWPORTS >= TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS,
              "Expecting that the Mesa Vulkan runtime can expose all hardware viewports.");

/* State applied before performing application's draws, and marked for reapplication after internal
 * draws.
 */
struct terakan_state_draw {
   /* Whether each state item has been modified, overridden by an internal draw, or had its
    * dependencies changed, and needs to be applied before the next draw.
    */
   BITSET_DECLARE(state_pending, TERAKAN_STATE_DRAW_COUNT);

   /* Configuration of state setting commands themselves. */
   bool cmd_set_depth_clamp_enable_sets_depth_clip_enable;

   /* TERAKAN_STATE_DRAW_VGT_INDEX_TYPE */
   uint32_t vgt_index_type;

   /* TERAKAN_STATE_DRAW_VGT_PRIMITIVE_TYPE */
   uint32_t vgt_primitive_type;

   /* TERAKAN_STATE_DRAW_VGT_INDEX_OFFSET */
   uint32_t vgt_index_offset;

   /* Set via terakan_state_draw_set_viewport_count. */
   uint32_t viewport_count;

   /* TERAKAN_STATE_DRAW_PA_CL_VPORT_XY_SCALE_OFFSET:
    * - pa_cl_vport_xy_scale_offset
    * TERAKAN_STATE_DRAW_PA_CL_VPORT_Z_SCALE_OFFSET:
    * - pa_cl_vport_z_gl_dx_scale_offset
    * TERAKAN_STATE_DRAW_PA_CL_GB:
    * - pa_cl_gb_vert_horz_clip_adj
    * TERAKAN_STATE_DRAW_PA_SC_VPORT_SCISSOR:
    * - pa_sc_vport_scissor_tl_br_xy
    * - pa_sc_vport_generic_scissor_tl_br_xy
    * TERAKAN_STATE_DRAW_PA_SC_VPORT_Z_MIN_MAX:
    * - pa_sc_vport_z_min_max
    * - pa_sc_vport_z_min_0_max_1
    */
   struct terakan_state_draw_viewport viewports[TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS];
   uint16_t pa_sc_vport_generic_scissor_tl_br_xy[TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS][2][2];
   /* Whether PA_SC_VPORT_ZMIN / ZMAX must be 0 to 1 regardless of the values in the viewport.
    * According to PAL, for optimal performance, depth clamping should be enabled, so without
    * unrestricted depth range, the preferred way to disable depth clamping is to clamp depth to
    * 0 to 1 instead of not clamping it completely by setting DISABLE_VIEWPORT_CLAMP in
    * DB_RENDER_OVERRIDE.
    */
   bool pa_sc_vport_z_min_0_max_1;

   /* All: TERAKAN_STATE_DRAW_PA_CL_CLIP_CNTL
    * DX_CLIP_SPACE_DEF: additionally TERAKAN_STATE_DRAW_PA_CL_VPORT_Z_SCALE_OFFSET
    */
   uint32_t pa_cl_clip_cntl;

   /* TERAKAN_STATE_DRAW_PA_SU_SC_MODE_CNTL */
   uint32_t pa_su_sc_mode_cntl;

   /* TERAKAN_STATE_DRAW_PA_SC_AA_MASK */
   uint16_t pa_sc_aa_mask;

   /* TERAKAN_STATE_DRAW_DB_RENDER_OVERRIDE */
   uint32_t db_render_override;

   /* TERAKAN_STATE_DRAW_CB_COLOR_FIRST...LAST */
   struct terakan_bo const * cb_color_bo[TERAKAN_LIMITS_HW_COLOR_RAT_COUNT];
   /* The values are undefined if the respective cb_color_bo is NULL. */
   struct terakan_color_descriptor cb_color[TERAKAN_LIMITS_HW_COLOR_RAT_COUNT];
   struct terakan_color_meta_descriptor cb_color_meta[TERAKAN_LIMITS_HW_COLOR_MRT_COUNT];
};

struct terakan_gfx_command_writer;

static inline void
terakan_state_draw_set_pending(struct terakan_state_draw * const state,
                               enum terakan_state_draw_index const state_index)
{
   BITSET_SET(state->state_pending, state_index);
}

static inline bool
terakan_state_draw_is_pending(struct terakan_state_draw const * const state,
                              enum terakan_state_draw_index const state_index)
{
   return BITSET_TEST(state->state_pending, state_index);
}

static inline void
terakan_state_draw_replace_fields(struct terakan_state_draw * const state,
                                  enum terakan_state_draw_index const state_index,
                                  uint32_t * const value, uint32_t const keep_fields,
                                  uint32_t const set_fields)
{
   assert(!(set_fields & keep_fields));
   uint32_t const new_value = (*value & keep_fields) | set_fields;
   if (*value == new_value) {
      return;
   }
   *value = new_value;
   terakan_state_draw_set_pending(state, state_index);
}

void terakan_state_draw_set_viewport_count(struct terakan_state_draw * state,
                                           uint32_t viewport_count);

void terakan_state_draw_apply_pending(struct terakan_gfx_command_writer * command_writer);

void terakan_state_draw_reset(struct terakan_state_draw * state, bool depth_range_unrestricted);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_STATE_H */
