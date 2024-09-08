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

#include "util/bitset.h"
#include "vk_object.h"

#include <stdint.h>

enum terakan_pipeline_graphics_state_index {
   TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_SC_MODE_CNTL,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK,

   TERAKAN_PIPELINE_GRAPHICS_STATE_COUNT,
};

struct terakan_pipeline_graphics_vertex_input {
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE */
   uint32_t vgt_primitive_type;
};

struct terakan_pipeline_graphics_pre_rasterization {
   bool cmd_set_depth_clamp_enable_sets_depth_clip_enable;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_CL_CLIP_CNTL */
   uint32_t pa_cl_clip_cntl_clear;
   uint32_t pa_cl_clip_cntl;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_SC_MODE_CNTL */
   uint32_t pa_su_sc_mode_cntl_clear;
   uint32_t pa_su_sc_mode_cntl;
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

#endif /* TERAKAN_PIPELINE_GRAPHICS_H */
