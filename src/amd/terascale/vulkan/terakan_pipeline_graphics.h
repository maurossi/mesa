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

#ifndef TERAKAN_PIPELINE_GRAPHICS_H
#define TERAKAN_PIPELINE_GRAPHICS_H

#include "terakan_hw_state.h"
#include "terakan_state_rasterization.h"

#include "util/bitset.h"
#include "vk_object.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum terakan_pipeline_graphics_state_index {
   /* Vertex input. */

   TERAKAN_PIPELINE_GRAPHICS_STATE_VERTEX_INPUT_START,

   TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE =
      TERAKAN_PIPELINE_GRAPHICS_STATE_VERTEX_INPUT_START,

   TERAKAN_PIPELINE_GRAPHICS_STATE_VERTEX_INPUT_END,

   /* Pre-rasterization. */

   TERAKAN_PIPELINE_GRAPHICS_STATE_PRE_RASTERIZATION_START =
      TERAKAN_PIPELINE_GRAPHICS_STATE_VERTEX_INPUT_END,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_Z_MIN_0_MAX_1,
   TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT_COUNT,
   TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_GENERIC_SCISSOR,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_SC_MODE_CNTL,

   TERAKAN_PIPELINE_GRAPHICS_STATE_DB_RENDER_OVERRIDE_PRE_RASTERIZATION,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PRE_RASTERIZATION_END,

   /* Multisample. */

   TERAKAN_PIPELINE_GRAPHICS_STATE_MULTISAMPLE_START =
      TERAKAN_PIPELINE_GRAPHICS_STATE_PRE_RASTERIZATION_END,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK,

   TERAKAN_PIPELINE_GRAPHICS_STATE_MULTISAMPLE_END,

   TERAKAN_PIPELINE_GRAPHICS_STATE_COUNT = TERAKAN_PIPELINE_GRAPHICS_STATE_MULTISAMPLE_END,
};

struct terakan_pipeline_graphics_vertex_input {
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE */
   uint32_t vgt_primitive_type;
};

struct terakan_pipeline_graphics_pre_rasterization {
   bool cmd_set_depth_clamp_enable_sets_depth_clip_enable;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_Z_MIN_0_MAX_1 */
   bool pa_sc_vport_z_min_0_max_1;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT_COUNT and TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT */
   uint32_t viewport_count;
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT */
   struct terakan_state_draw_viewport viewports[TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS];

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_GENERIC_SCISSOR */
   uint32_t pa_sc_vport_generic_scissor_count;
   uint16_t pa_sc_vport_generic_scissor_tl_br_xy[TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS][2][2];

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL */
   uint32_t pa_cl_clip_cntl_clear;
   uint32_t pa_cl_clip_cntl;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_SC_MODE_CNTL */
   uint32_t pa_su_sc_mode_cntl_clear;
   uint32_t pa_su_sc_mode_cntl;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_DB_RENDER_OVERRIDE_PRE_RASTERIZATION */
   uint32_t db_render_override_clear;
   uint32_t db_render_override;
};

struct terakan_pipeline_graphics_multisample {
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK */
   uint16_t pa_sc_aa_mask;
};

struct terakan_pipeline_graphics {
   struct vk_object_base base;

   BITSET_DECLARE(static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_COUNT);

   /* State fields not in static_state are undefined. */

   struct terakan_pipeline_graphics_vertex_input vertex_input;

   struct terakan_pipeline_graphics_pre_rasterization pre_rasterization;

   /* Part of the fragment shader state if sample shading is enabled or the render pass is static,
    * and of the fragment output state.
    */
   struct terakan_pipeline_graphics_multisample multisample;
};

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_PIPELINE_GRAPHICS_H */
