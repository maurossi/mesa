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

#ifndef TERAKAN_PIPELINE_GRAPHICS_H
#define TERAKAN_PIPELINE_GRAPHICS_H

#include "terakan_command_buffer.h"
#include "terakan_descriptor.h"
#include "terakan_hw_state.h"
#include "terakan_pipeline.h"
#include "terakan_shader.h"
#include "terakan_state_rasterization.h"

#include "compiler/shader_enums.h"
#include "util/bitset.h"

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

enum terakan_pipeline_graphics_state_index {
   /* Vertex input. */

   TERAKAN_PIPELINE_GRAPHICS_STATE_VERTEX_INPUT_START,

   TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE =
      TERAKAN_PIPELINE_GRAPHICS_STATE_VERTEX_INPUT_START,

   TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS,
   TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_RESOURCES_FS_STRIDE,
   /* SQ_PGM_FS_2048_STRIDE_WORKAROUND can be static only if SQ_PGM_FS and SQ_RESOURCES_FS_STRIDE
    * are static.
    */
   TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS_2048_STRIDE_WORKAROUND,

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

   TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_POLY_OFFSET,

   TERAKAN_PIPELINE_GRAPHICS_STATE_DB_RENDER_OVERRIDE_PRE_RASTERIZATION,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PRE_RASTERIZATION_END,

   /* Multisample. */

   TERAKAN_PIPELINE_GRAPHICS_STATE_MULTISAMPLE_START =
      TERAKAN_PIPELINE_GRAPHICS_STATE_PRE_RASTERIZATION_END,

   TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK,

   TERAKAN_PIPELINE_GRAPHICS_STATE_MULTISAMPLE_END,

   /* Fragment shader. */

   TERAKAN_PIPELINE_GRAPHICS_STATE_FRAGMENT_SHADER_START =
      TERAKAN_PIPELINE_GRAPHICS_STATE_MULTISAMPLE_END,

   TERAKAN_PIPELINE_GRAPHICS_STATE_DB_STENCILREFMASK,

   TERAKAN_PIPELINE_GRAPHICS_STATE_DB_DEPTH_CONTROL,

   TERAKAN_PIPELINE_GRAPHICS_STATE_FRAGMENT_SHADER_END,

   /* Fragment output. */

   TERAKAN_PIPELINE_GRAPHICS_STATE_FRAGMENT_OUTPUT_START =
      TERAKAN_PIPELINE_GRAPHICS_STATE_FRAGMENT_SHADER_END,

   TERAKAN_PIPELINE_GRAPHICS_STATE_LOGIC_OP_ENABLE,
   TERAKAN_PIPELINE_GRAPHICS_STATE_LOGIC_OP_ROP3,

   TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_RGBA,

   TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_ENABLE,
   TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_EQUATION,

   TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_MASK,
   TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_ENABLE,

   TERAKAN_PIPELINE_GRAPHICS_STATE_FRAGMENT_OUTPUT_END,

   TERAKAN_PIPELINE_GRAPHICS_STATE_COUNT = TERAKAN_PIPELINE_GRAPHICS_STATE_FRAGMENT_OUTPUT_END,
};

struct terakan_pipeline_graphics_vertex_input {
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_VGT_PRIMITIVE_TYPE */
   uint32_t vgt_primitive_type;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_PGM_FS
    * If the pipeline library contains only static vertex input, but no vertex shader, there's no
    * program BO in the vertex input state, and the static state contains info about all provided
    * attributes regardless of whether they're needed by the vertex shader.
    * In an executable pipeline, if program_bo is NULL, the empty (return-only) fetch shader must be
    * used instead.
    */
   struct terakan_vertex_input_static_state sq_pgm_fs;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_SQ_RESOURCES_FS_STRIDE */
   struct {
      uint32_t bindings_with_static_stride;
      /* The values are undefined for bindings not in bindings_with_static_stride. */
      uint16_t binding_strides[TERAKAN_RESOURCE_HW_COUNT_FETCH];
   } sq_resources_fs_stride;
};

struct terakan_pipeline_graphics_pre_rasterization {
   bool cmd_set_depth_clamp_enable_sets_depth_clip_enable;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_VPORT_Z_MIN_0_MAX_1 */
   bool pa_sc_vport_z_min_0_max_1;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT_COUNT and TERAKAN_PIPELINE_GRAPHICS_STATE_VIEWPORT */
   uint8_t viewport_count;
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

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SU_POLY_OFFSET */
   struct {
      float clamp;
      float subpixel_slope_scale;
      float offset;
      VkDepthBiasRepresentationEXT representation;
      bool representation_exact;
   } pa_su_poly_offset;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_DB_RENDER_OVERRIDE_PRE_RASTERIZATION */
   uint32_t db_render_override_clear;
   uint32_t db_render_override;
};

struct terakan_pipeline_graphics_multisample {
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_PA_SC_AA_MASK */
   uint16_t pa_sc_aa_mask;
};

struct terakan_pipeline_graphics_fragment_shader {
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_DB_STENCILREFMASK */
   uint32_t db_stencilrefmask_clear;
   uint32_t db_stencilrefmask_front_back[2];

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_DB_DEPTH_CONTROL */
   uint32_t db_depth_control_clear;
   uint32_t db_depth_control;
};

struct terakan_pipeline_graphics_fragment_output {
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_LOGIC_OP_ENABLE */
   bool logic_op_enable;
   /* TERAKAN_PIPELINE_GRAPHICS_STATE_LOGIC_OP_ROP3 */
   uint8_t logic_op_rop3;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_RGBA */
   float cb_blend_rgba[4];

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_ENABLE,
    * TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_EQUATION,
    * TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_MASK
    */
   uint32_t color_blend_attachment_count;

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_ENABLE: BLEND_CONTROL_ENABLE
    * TERAKAN_PIPELINE_GRAPHICS_STATE_CB_BLEND_CONTROL_EQUATION: all other fields
    * color_blend_attachment_count elements are valid.
    */
   uint32_t cb_blend_control[TERAKAN_COLOR_HW_RTV_COUNT];

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_MASK
    * color_blend_attachment_count elements are valid.
    */
   uint8_t color_attachment_write_masks[TERAKAN_COLOR_HW_RTV_COUNT];

   /* TERAKAN_PIPELINE_GRAPHICS_STATE_COLOR_ATTACHMENT_WRITE_ENABLE */
   uint8_t color_attachment_write_enable;
};

struct terakan_pipeline_graphics {
   struct terakan_pipeline base;

   VkShaderStageFlags shader_stages;
   struct terakan_shader_impl shaders[MESA_SHADER_FRAGMENT + 1];

   BITSET_DECLARE(static_state, TERAKAN_PIPELINE_GRAPHICS_STATE_COUNT);

   /* Precompiling the state into register values rather than simply layering pipeline binding over
    * dynamic state setters to perform less work at bind time.
    *
    * State fields not in static_state are undefined.
    */

   struct terakan_pipeline_graphics_vertex_input vertex_input;

   struct terakan_pipeline_graphics_pre_rasterization pre_rasterization;

   /* Part of the fragment shader state if sample shading is enabled or the render pass is static,
    * and of the fragment output state.
    */
   struct terakan_pipeline_graphics_multisample multisample;

   struct terakan_pipeline_graphics_fragment_shader fragment_shader;

   struct terakan_pipeline_graphics_fragment_output fragment_output;
};

void terakan_pipeline_graphics_bind(struct terakan_gfx_command_writer * command_writer,
                                    struct terakan_pipeline_graphics const * pipeline);

void terakan_pipeline_graphics_destroy(struct terakan_pipeline_graphics * pipeline,
                                       VkAllocationCallbacks const * allocator);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_PIPELINE_GRAPHICS_H */
