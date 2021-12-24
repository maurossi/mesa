/*
 * Copyright © 2019 Red Hat.
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

/* use a gallium context to execute a command buffer */

#include "lvp_private.h"

#include "pipe/p_context.h"
#include "pipe/p_state.h"
#include "lvp_conv.h"

#include "pipe/p_shader_tokens.h"
#include "tgsi/tgsi_text.h"
#include "tgsi/tgsi_parse.h"

#include "util/format/u_format.h"
#include "util/u_surface.h"
#include "util/u_sampler.h"
#include "util/u_box.h"
#include "util/u_inlines.h"
#include "util/u_prim.h"
#include "util/u_prim_restart.h"
#include "util/format/u_format_zs.h"

#include "vk_util.h"

#define DOUBLE_EQ(a, b) (fabs((a) - (b)) < DBL_EPSILON)

enum gs_output {
  GS_OUTPUT_NONE,
  GS_OUTPUT_NOT_LINES,
  GS_OUTPUT_LINES,
};

struct rendering_state {
   struct pipe_context *pctx;
   struct cso_context *cso;

   bool blend_dirty;
   bool rs_dirty;
   bool dsa_dirty;
   bool stencil_ref_dirty;
   bool clip_state_dirty;
   bool blend_color_dirty;
   bool ve_dirty;
   bool vb_dirty;
   bool constbuf_dirty[PIPE_SHADER_TYPES];
   bool pcbuf_dirty[PIPE_SHADER_TYPES];
   bool vp_dirty;
   bool scissor_dirty;
   bool ib_dirty;
   bool sample_mask_dirty;
   bool min_samples_dirty;
   struct pipe_draw_indirect_info indirect_info;
   struct pipe_draw_info info;

   struct pipe_grid_info dispatch_info;
   struct pipe_framebuffer_state framebuffer;

   struct pipe_blend_state blend_state;
   struct {
      float offset_units;
      float offset_scale;
      float offset_clamp;
      bool enabled;
   } depth_bias;
   struct pipe_rasterizer_state rs_state;
   struct pipe_depth_stencil_alpha_state dsa_state;

   struct pipe_blend_color blend_color;
   struct pipe_stencil_ref stencil_ref;
   struct pipe_clip_state clip_state;

   int num_scissors;
   struct pipe_scissor_state scissors[16];

   int num_viewports;
   struct pipe_viewport_state viewports[16];

   ubyte index_size;
   unsigned index_offset;
   struct pipe_resource *index_buffer;
   struct pipe_constant_buffer pc_buffer[PIPE_SHADER_TYPES];
   struct pipe_constant_buffer const_buffer[PIPE_SHADER_TYPES][16];
   int num_const_bufs[PIPE_SHADER_TYPES];
   int num_vb;
   unsigned start_vb;
   struct pipe_vertex_buffer vb[PIPE_MAX_ATTRIBS];
   struct cso_velems_state velem;

   struct pipe_sampler_view *sv[PIPE_SHADER_TYPES][PIPE_MAX_SAMPLERS];
   int num_sampler_views[PIPE_SHADER_TYPES];
   struct pipe_sampler_state ss[PIPE_SHADER_TYPES][PIPE_MAX_SAMPLERS];
   /* cso_context api is stupid */
   const struct pipe_sampler_state *cso_ss_ptr[PIPE_SHADER_TYPES][PIPE_MAX_SAMPLERS];
   int num_sampler_states[PIPE_SHADER_TYPES];
   bool sv_dirty[PIPE_SHADER_TYPES];
   bool ss_dirty[PIPE_SHADER_TYPES];

   struct pipe_image_view iv[PIPE_SHADER_TYPES][PIPE_MAX_SHADER_IMAGES];
   int num_shader_images[PIPE_SHADER_TYPES];
   struct pipe_shader_buffer sb[PIPE_SHADER_TYPES][PIPE_MAX_SHADER_BUFFERS];
   int num_shader_buffers[PIPE_SHADER_TYPES];
   bool iv_dirty[PIPE_SHADER_TYPES];
   bool sb_dirty[PIPE_SHADER_TYPES];
   bool disable_multisample;
   enum gs_output gs_output_lines : 2;
   void *ss_cso[PIPE_SHADER_TYPES][PIPE_MAX_SAMPLERS];
   void *velems_cso;

   uint8_t push_constants[128 * 4];

   const struct lvp_render_pass *pass;
   uint32_t subpass;
   const struct lvp_framebuffer *vk_framebuffer;
   VkRect2D render_area;

   uint32_t sample_mask;
   unsigned min_samples;

   struct lvp_image_view **imageless_views;
   const struct lvp_attachment_state *attachments;
   VkImageAspectFlags *pending_clear_aspects;
   uint32_t *cleared_views;
   int num_pending_aspects;

   uint32_t num_so_targets;
   struct pipe_stream_output_target *so_targets[PIPE_MAX_SO_BUFFERS];
   uint32_t so_offsets[PIPE_MAX_SO_BUFFERS];
};

ALWAYS_INLINE static void
assert_subresource_layers(const struct pipe_resource *pres, const VkImageSubresourceLayers *layers, const VkOffset3D *offsets)
{
#ifndef NDEBUG
   if (pres->target == PIPE_TEXTURE_3D) {
      assert(layers->baseArrayLayer == 0);
      assert(layers->layerCount == 1);
      assert(offsets[0].z <= pres->depth0);
      assert(offsets[1].z <= pres->depth0);
   } else {
      assert(layers->baseArrayLayer < pres->array_size);
      assert(layers->baseArrayLayer + layers->layerCount <= pres->array_size);
      assert(offsets[0].z == 0);
      assert(offsets[1].z == 1);
   }
#endif
}

static void emit_compute_state(struct rendering_state *state)
{
   if (state->iv_dirty[PIPE_SHADER_COMPUTE]) {
      state->pctx->set_shader_images(state->pctx, PIPE_SHADER_COMPUTE,
                                     0, state->num_shader_images[PIPE_SHADER_COMPUTE],
                                     0, state->iv[PIPE_SHADER_COMPUTE]);
      state->iv_dirty[PIPE_SHADER_COMPUTE] = false;
   }

   if (state->pcbuf_dirty[PIPE_SHADER_COMPUTE]) {
      state->pctx->set_constant_buffer(state->pctx, PIPE_SHADER_COMPUTE,
                                       0, false, &state->pc_buffer[PIPE_SHADER_COMPUTE]);
      state->pcbuf_dirty[PIPE_SHADER_COMPUTE] = false;
   }

   if (state->constbuf_dirty[PIPE_SHADER_COMPUTE]) {
      for (unsigned i = 0; i < state->num_const_bufs[PIPE_SHADER_COMPUTE]; i++)
         state->pctx->set_constant_buffer(state->pctx, PIPE_SHADER_COMPUTE,
                                          i + 1, false, &state->const_buffer[PIPE_SHADER_COMPUTE][i]);
      state->constbuf_dirty[PIPE_SHADER_COMPUTE] = false;
   }

   if (state->sb_dirty[PIPE_SHADER_COMPUTE]) {
      state->pctx->set_shader_buffers(state->pctx, PIPE_SHADER_COMPUTE,
                                      0, state->num_shader_buffers[PIPE_SHADER_COMPUTE],
                                      state->sb[PIPE_SHADER_COMPUTE], 0);
      state->sb_dirty[PIPE_SHADER_COMPUTE] = false;
   }

   if (state->sv_dirty[PIPE_SHADER_COMPUTE]) {
      state->pctx->set_sampler_views(state->pctx, PIPE_SHADER_COMPUTE, 0, state->num_sampler_views[PIPE_SHADER_COMPUTE],
                                     0, state->sv[PIPE_SHADER_COMPUTE]);
      state->sv_dirty[PIPE_SHADER_COMPUTE] = false;
   }

   if (state->ss_dirty[PIPE_SHADER_COMPUTE]) {
      for (unsigned i = 0; i < state->num_sampler_states[PIPE_SHADER_COMPUTE]; i++) {
         if (state->ss_cso[PIPE_SHADER_COMPUTE][i])
            state->pctx->delete_sampler_state(state->pctx, state->ss_cso[PIPE_SHADER_COMPUTE][i]);
         state->ss_cso[PIPE_SHADER_COMPUTE][i] = state->pctx->create_sampler_state(state->pctx, &state->ss[PIPE_SHADER_COMPUTE][i]);
      }
      state->pctx->bind_sampler_states(state->pctx, PIPE_SHADER_COMPUTE, 0, state->num_sampler_states[PIPE_SHADER_COMPUTE], state->ss_cso[PIPE_SHADER_COMPUTE]);
      state->ss_dirty[PIPE_SHADER_COMPUTE] = false;
   }
}

static void emit_state(struct rendering_state *state)
{
   int sh;
   if (state->blend_dirty) {
      cso_set_blend(state->cso, &state->blend_state);
      state->blend_dirty = false;
   }

   if (state->rs_dirty) {
      bool ms = state->rs_state.multisample;
      if (state->disable_multisample &&
          (state->gs_output_lines == GS_OUTPUT_LINES ||
           (state->gs_output_lines == GS_OUTPUT_NONE && u_reduced_prim(state->info.mode) == PIPE_PRIM_LINES)))
         state->rs_state.multisample = false;
      assert(offsetof(struct pipe_rasterizer_state, offset_clamp) - offsetof(struct pipe_rasterizer_state, offset_units) == sizeof(float) * 2);
      if (state->depth_bias.enabled) {
         memcpy(&state->rs_state.offset_units, &state->depth_bias, sizeof(float) * 3);
      } else {
         memset(&state->rs_state.offset_units, 0, sizeof(float) * 3);
      }
      cso_set_rasterizer(state->cso, &state->rs_state);
      state->rs_dirty = false;
      state->rs_state.multisample = ms;
   }

   if (state->dsa_dirty) {
      cso_set_depth_stencil_alpha(state->cso, &state->dsa_state);
      state->dsa_dirty = false;
   }

   if (state->sample_mask_dirty) {
      cso_set_sample_mask(state->cso, state->sample_mask);
      state->sample_mask_dirty = false;
   }

   if (state->min_samples_dirty) {
      cso_set_min_samples(state->cso, state->min_samples);
      state->min_samples_dirty = false;
   }

   if (state->blend_color_dirty) {
      state->pctx->set_blend_color(state->pctx, &state->blend_color);
      state->blend_color_dirty = false;
   }

   if (state->stencil_ref_dirty) {
      cso_set_stencil_ref(state->cso, state->stencil_ref);
      state->stencil_ref_dirty = false;
   }

   if (state->vb_dirty) {
      cso_set_vertex_buffers(state->cso, state->start_vb, state->num_vb, state->vb);
      state->vb_dirty = false;
   }

   if (state->ve_dirty) {
      cso_set_vertex_elements(state->cso, &state->velem);
      state->ve_dirty = false;
   }
   

   for (sh = 0; sh < PIPE_SHADER_TYPES; sh++) {
      if (state->constbuf_dirty[sh]) {
         for (unsigned idx = 0; idx < state->num_const_bufs[sh]; idx++)
            state->pctx->set_constant_buffer(state->pctx, sh,
                                             idx + 1, false, &state->const_buffer[sh][idx]);
      }
      state->constbuf_dirty[sh] = false;
   }

   for (sh = 0; sh < PIPE_SHADER_TYPES; sh++) {
      if (state->pcbuf_dirty[sh]) {
         state->pctx->set_constant_buffer(state->pctx, sh,
                                          0, false, &state->pc_buffer[sh]);
      }
   }

   for (sh = 0; sh < PIPE_SHADER_TYPES; sh++) {
      if (state->sb_dirty[sh]) {
         state->pctx->set_shader_buffers(state->pctx, sh,
                                         0, state->num_shader_buffers[sh],
                                         state->sb[sh], 0);
      }
   }

   for (sh = 0; sh < PIPE_SHADER_TYPES; sh++) {
      if (state->iv_dirty[sh]) {
         state->pctx->set_shader_images(state->pctx, sh,
                                        0, state->num_shader_images[sh], 0,
                                        state->iv[sh]);
      }
   }

   for (sh = 0; sh < PIPE_SHADER_TYPES; sh++) {

      if (!state->sv_dirty[sh])
         continue;

      state->pctx->set_sampler_views(state->pctx, sh, 0, state->num_sampler_views[sh],
                                     0, state->sv[sh]);
      state->sv_dirty[sh] = false;
   }

   for (sh = 0; sh < PIPE_SHADER_TYPES; sh++) {
      if (!state->ss_dirty[sh])
         continue;

      cso_set_samplers(state->cso, sh, state->num_sampler_states[sh], state->cso_ss_ptr[sh]);
   }

   if (state->vp_dirty) {
      state->pctx->set_viewport_states(state->pctx, 0, state->num_viewports, state->viewports);
      state->vp_dirty = false;
   }

   if (state->scissor_dirty) {
      state->pctx->set_scissor_states(state->pctx, 0, state->num_scissors, state->scissors);
      state->scissor_dirty = false;
   }
}

static void handle_compute_pipeline(struct lvp_cmd_buffer_entry *cmd,
                                    struct rendering_state *state)
{
   struct lvp_pipeline *pipeline = cmd->u.pipeline.pipeline;

   state->dispatch_info.block[0] = pipeline->pipeline_nir[MESA_SHADER_COMPUTE]->info.workgroup_size[0];
   state->dispatch_info.block[1] = pipeline->pipeline_nir[MESA_SHADER_COMPUTE]->info.workgroup_size[1];
   state->dispatch_info.block[2] = pipeline->pipeline_nir[MESA_SHADER_COMPUTE]->info.workgroup_size[2];
   state->pctx->bind_compute_state(state->pctx, pipeline->shader_cso[PIPE_SHADER_COMPUTE]);
}

static void
get_viewport_xform(const VkViewport *viewport,
                   float scale[3], float translate[3])
{
   float x = viewport->x;
   float y = viewport->y;
   float half_width = 0.5f * viewport->width;
   float half_height = 0.5f * viewport->height;
   double n = viewport->minDepth;
   double f = viewport->maxDepth;

   scale[0] = half_width;
   translate[0] = half_width + x;
   scale[1] = half_height;
   translate[1] = half_height + y;

   scale[2] = (f - n);
   translate[2] = n;
}

/* enum re-indexing:

    VK_DYNAMIC_STATE_VIEWPORT
    VK_DYNAMIC_STATE_SCISSOR
    VK_DYNAMIC_STATE_LINE_WIDTH
    VK_DYNAMIC_STATE_DEPTH_BIAS
    VK_DYNAMIC_STATE_BLEND_CONSTANTS
    VK_DYNAMIC_STATE_DEPTH_BOUNDS
    VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK
    VK_DYNAMIC_STATE_STENCIL_WRITE_MASK
    VK_DYNAMIC_STATE_STENCIL_REFERENCE

    VK_DYNAMIC_STATE_LINE_STIPPLE_EXT

    VK_DYNAMIC_STATE_CULL_MODE_EXT
    VK_DYNAMIC_STATE_FRONT_FACE_EXT
    VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT
    VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT_EXT
    VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT_EXT
    VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT
    VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE_EXT
    VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE_EXT
    VK_DYNAMIC_STATE_DEPTH_COMPARE_OP_EXT
    VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE_EXT
    VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE_EXT
    VK_DYNAMIC_STATE_STENCIL_OP_EXT

    VK_DYNAMIC_STATE_VERTEX_INPUT_EXT

    VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT
    VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE_EXT
    VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE_EXT
    VK_DYNAMIC_STATE_LOGIC_OP_EXT
    VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE_EXT
*/
static int conv_dynamic_state_idx(VkDynamicState dyn_state)
{
   if (dyn_state <= VK_DYNAMIC_STATE_STENCIL_REFERENCE)
      return dyn_state;
   if (dyn_state == VK_DYNAMIC_STATE_LINE_STIPPLE_EXT)
      /* this one has a weird id, map after the normal dynamic state ones */
      return VK_DYNAMIC_STATE_STENCIL_REFERENCE + 1;
   if (dyn_state >= VK_DYNAMIC_STATE_CULL_MODE_EXT &&
       dyn_state <= VK_DYNAMIC_STATE_STENCIL_OP_EXT)
      return dyn_state - VK_DYNAMIC_STATE_CULL_MODE_EXT + VK_DYNAMIC_STATE_STENCIL_REFERENCE + 2;
   if (dyn_state == VK_DYNAMIC_STATE_VERTEX_INPUT_EXT)
      return (VK_DYNAMIC_STATE_STENCIL_OP_EXT - VK_DYNAMIC_STATE_CULL_MODE_EXT) + VK_DYNAMIC_STATE_STENCIL_REFERENCE + 2 + 1;
   if (dyn_state >= VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT &&
       dyn_state <= VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE_EXT)
      return dyn_state - VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT +
             VK_DYNAMIC_STATE_STENCIL_OP_EXT - VK_DYNAMIC_STATE_CULL_MODE_EXT +
             VK_DYNAMIC_STATE_STENCIL_REFERENCE + 2 + 1 + 1;
   assert(0);
   return -1;
}

static void handle_graphics_pipeline(struct lvp_cmd_buffer_entry *cmd,
                                     struct rendering_state *state)
{
   struct lvp_pipeline *pipeline = cmd->u.pipeline.pipeline;
   bool dynamic_states[VK_DYNAMIC_STATE_STENCIL_REFERENCE+32];
   unsigned fb_samples = 0;

   memset(dynamic_states, 0, sizeof(dynamic_states));
   if (pipeline->graphics_create_info.pDynamicState)
   {
      const VkPipelineDynamicStateCreateInfo *dyn = pipeline->graphics_create_info.pDynamicState;
      int i;
      for (i = 0; i < dyn->dynamicStateCount; i++) {
         int idx = conv_dynamic_state_idx(dyn->pDynamicStates[i]);
         if (idx == -1)
            continue;
         dynamic_states[idx] = true;
      }
   }

   bool has_stage[PIPE_SHADER_TYPES] = { false };

   state->pctx->bind_gs_state(state->pctx, NULL);
   if (state->pctx->bind_tcs_state)
      state->pctx->bind_tcs_state(state->pctx, NULL);
   if (state->pctx->bind_tes_state)
      state->pctx->bind_tes_state(state->pctx, NULL);
   state->gs_output_lines = GS_OUTPUT_NONE;
   {
      int i;
      for (i = 0; i < pipeline->graphics_create_info.stageCount; i++) {
         const VkPipelineShaderStageCreateInfo *sh = &pipeline->graphics_create_info.pStages[i];
         switch (sh->stage) {
         case VK_SHADER_STAGE_FRAGMENT_BIT:
            state->pctx->bind_fs_state(state->pctx, pipeline->shader_cso[PIPE_SHADER_FRAGMENT]);
            has_stage[PIPE_SHADER_FRAGMENT] = true;
            break;
         case VK_SHADER_STAGE_VERTEX_BIT:
            state->pctx->bind_vs_state(state->pctx, pipeline->shader_cso[PIPE_SHADER_VERTEX]);
            has_stage[PIPE_SHADER_VERTEX] = true;
            break;
         case VK_SHADER_STAGE_GEOMETRY_BIT:
            state->pctx->bind_gs_state(state->pctx, pipeline->shader_cso[PIPE_SHADER_GEOMETRY]);
            state->gs_output_lines = pipeline->gs_output_lines ? GS_OUTPUT_LINES : GS_OUTPUT_NOT_LINES;
            has_stage[PIPE_SHADER_GEOMETRY] = true;
            break;
         case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            state->pctx->bind_tcs_state(state->pctx, pipeline->shader_cso[PIPE_SHADER_TESS_CTRL]);
            has_stage[PIPE_SHADER_TESS_CTRL] = true;
            break;
         case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            state->pctx->bind_tes_state(state->pctx, pipeline->shader_cso[PIPE_SHADER_TESS_EVAL]);
            has_stage[PIPE_SHADER_TESS_EVAL] = true;
            break;
         default:
            assert(0);
            break;
         }
      }
   }

   /* there should always be a dummy fs. */
   if (!has_stage[PIPE_SHADER_FRAGMENT])
      state->pctx->bind_fs_state(state->pctx, pipeline->shader_cso[PIPE_SHADER_FRAGMENT]);
   if (state->pctx->bind_gs_state && !has_stage[PIPE_SHADER_GEOMETRY])
      state->pctx->bind_gs_state(state->pctx, NULL);
   if (state->pctx->bind_tcs_state && !has_stage[PIPE_SHADER_TESS_CTRL])
      state->pctx->bind_tcs_state(state->pctx, NULL);
   if (state->pctx->bind_tes_state && !has_stage[PIPE_SHADER_TESS_EVAL])
      state->pctx->bind_tes_state(state->pctx, NULL);

   /* rasterization state */
   if (pipeline->graphics_create_info.pRasterizationState) {
      const VkPipelineRasterizationStateCreateInfo *rsc = pipeline->graphics_create_info.pRasterizationState;
      state->rs_state.depth_clip_near = state->rs_state.depth_clip_far = !rsc->depthClampEnable;
      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE_EXT)])
         state->rs_state.rasterizer_discard = rsc->rasterizerDiscardEnable;

      state->rs_state.line_smooth = pipeline->line_smooth;
      state->rs_state.line_stipple_enable = pipeline->line_stipple_enable;
      state->rs_state.fill_front = vk_polygon_mode_to_pipe(rsc->polygonMode);
      state->rs_state.fill_back = vk_polygon_mode_to_pipe(rsc->polygonMode);
      state->rs_state.point_size_per_vertex = true;
      state->rs_state.flatshade_first = !pipeline->provoking_vertex_last;
      state->rs_state.point_quad_rasterization = true;
      state->rs_state.clip_halfz = true;
      state->rs_state.half_pixel_center = true;
      state->rs_state.scissor = true;
      state->rs_state.no_ms_sample_mask_out = true;
      state->rs_state.line_rectangular = pipeline->line_rectangular;

      if (!dynamic_states[VK_DYNAMIC_STATE_LINE_WIDTH])
         state->rs_state.line_width = rsc->lineWidth;
      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_LINE_STIPPLE_EXT)]) {
         state->rs_state.line_stipple_factor = pipeline->line_stipple_factor;
         state->rs_state.line_stipple_pattern = pipeline->line_stipple_pattern;
      }

      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE_EXT)])
         state->depth_bias.enabled = pipeline->graphics_create_info.pRasterizationState->depthBiasEnable;
      if (!dynamic_states[VK_DYNAMIC_STATE_DEPTH_BIAS]) {
         state->depth_bias.offset_units = rsc->depthBiasConstantFactor;
         state->depth_bias.offset_scale = rsc->depthBiasSlopeFactor;
         state->depth_bias.offset_clamp = rsc->depthBiasClamp;
      }

      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_CULL_MODE_EXT)])
         state->rs_state.cull_face = vk_cull_to_pipe(rsc->cullMode);

      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_FRONT_FACE_EXT)])
         state->rs_state.front_ccw = (rsc->frontFace == VK_FRONT_FACE_COUNTER_CLOCKWISE);
      state->rs_dirty = true;
   }

   state->disable_multisample = pipeline->disable_multisample;
   if (pipeline->graphics_create_info.pMultisampleState) {
      const VkPipelineMultisampleStateCreateInfo *ms = pipeline->graphics_create_info.pMultisampleState;
      state->rs_state.multisample = ms->rasterizationSamples > 1;
      state->sample_mask = ms->pSampleMask ? ms->pSampleMask[0] : 0xffffffff;
      state->blend_state.alpha_to_coverage = ms->alphaToCoverageEnable;
      state->blend_state.alpha_to_one = ms->alphaToOneEnable;
      state->blend_dirty = true;
      state->rs_dirty = true;
      state->min_samples = 1;
      state->sample_mask_dirty = true;
      fb_samples = ms->rasterizationSamples;
      if (ms->sampleShadingEnable) {
         state->min_samples = ceil(ms->rasterizationSamples * ms->minSampleShading);
         if (state->min_samples > 1)
            state->min_samples = ms->rasterizationSamples;
         if (state->min_samples < 1)
            state->min_samples = 1;
      }
      if (pipeline->force_min_sample)
         state->min_samples = ms->rasterizationSamples;
      state->min_samples_dirty = true;
   } else {
      state->rs_state.multisample = false;
      state->sample_mask_dirty = state->sample_mask != 0xffffffff;
      state->sample_mask = 0xffffffff;
      state->min_samples_dirty = state->min_samples;
      state->min_samples = 0;
      state->blend_dirty |= state->blend_state.alpha_to_coverage || state->blend_state.alpha_to_one;
      state->blend_state.alpha_to_coverage = false;
      state->blend_state.alpha_to_one = false;
      state->rs_dirty = true;
   }

   if (pipeline->graphics_create_info.pDepthStencilState) {
      const VkPipelineDepthStencilStateCreateInfo *dsa = pipeline->graphics_create_info.pDepthStencilState;

      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE_EXT)])
         state->dsa_state.depth_enabled = dsa->depthTestEnable;
      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE_EXT)])
         state->dsa_state.depth_writemask = dsa->depthWriteEnable;
      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_DEPTH_COMPARE_OP_EXT)])
         state->dsa_state.depth_func = dsa->depthCompareOp;
      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE_EXT)])
         state->dsa_state.depth_bounds_test = dsa->depthBoundsTestEnable;

      if (!dynamic_states[VK_DYNAMIC_STATE_DEPTH_BOUNDS]) {
         state->dsa_state.depth_bounds_min = dsa->minDepthBounds;
         state->dsa_state.depth_bounds_max = dsa->maxDepthBounds;
      }

      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE_EXT)]) {
         state->dsa_state.stencil[0].enabled = dsa->stencilTestEnable;
         state->dsa_state.stencil[1].enabled = dsa->stencilTestEnable;
      }

      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_STENCIL_OP_EXT)]) {
         state->dsa_state.stencil[0].func = dsa->front.compareOp;
         state->dsa_state.stencil[0].fail_op = vk_conv_stencil_op(dsa->front.failOp);
         state->dsa_state.stencil[0].zpass_op = vk_conv_stencil_op(dsa->front.passOp);
         state->dsa_state.stencil[0].zfail_op = vk_conv_stencil_op(dsa->front.depthFailOp);

         state->dsa_state.stencil[1].func = dsa->back.compareOp;
         state->dsa_state.stencil[1].fail_op = vk_conv_stencil_op(dsa->back.failOp);
         state->dsa_state.stencil[1].zpass_op = vk_conv_stencil_op(dsa->back.passOp);
         state->dsa_state.stencil[1].zfail_op = vk_conv_stencil_op(dsa->back.depthFailOp);
      }

      if (!dynamic_states[VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK]) {
         state->dsa_state.stencil[0].valuemask = dsa->front.compareMask;
         state->dsa_state.stencil[1].valuemask = dsa->back.compareMask;
      }

      if (!dynamic_states[VK_DYNAMIC_STATE_STENCIL_WRITE_MASK]) {
         state->dsa_state.stencil[0].writemask = dsa->front.writeMask;
         state->dsa_state.stencil[1].writemask = dsa->back.writeMask;
      }

      if (dsa->stencilTestEnable) {
         if (!dynamic_states[VK_DYNAMIC_STATE_STENCIL_REFERENCE]) {
            state->stencil_ref.ref_value[0] = dsa->front.reference;
            state->stencil_ref.ref_value[1] = dsa->back.reference;
            state->stencil_ref_dirty = true;
         }
      }
   } else
      memset(&state->dsa_state, 0, sizeof(state->dsa_state));
   state->dsa_dirty = true;

   if (pipeline->graphics_create_info.pColorBlendState) {
      const VkPipelineColorBlendStateCreateInfo *cb = pipeline->graphics_create_info.pColorBlendState;
      int i;

      if (cb->logicOpEnable) {
         state->blend_state.logicop_enable = VK_TRUE;
         if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_LOGIC_OP_EXT)])
            state->blend_state.logicop_func = vk_conv_logic_op(cb->logicOp);
      }

      if (cb->attachmentCount > 1)
         state->blend_state.independent_blend_enable = true;
      for (i = 0; i < cb->attachmentCount; i++) {
         state->blend_state.rt[i].colormask = cb->pAttachments[i].colorWriteMask;
         state->blend_state.rt[i].blend_enable = cb->pAttachments[i].blendEnable;
         state->blend_state.rt[i].rgb_func = vk_conv_blend_func(cb->pAttachments[i].colorBlendOp);
         state->blend_state.rt[i].rgb_src_factor = vk_conv_blend_factor(cb->pAttachments[i].srcColorBlendFactor);
         state->blend_state.rt[i].rgb_dst_factor = vk_conv_blend_factor(cb->pAttachments[i].dstColorBlendFactor);
         state->blend_state.rt[i].alpha_func = vk_conv_blend_func(cb->pAttachments[i].alphaBlendOp);
         state->blend_state.rt[i].alpha_src_factor = vk_conv_blend_factor(cb->pAttachments[i].srcAlphaBlendFactor);
         state->blend_state.rt[i].alpha_dst_factor = vk_conv_blend_factor(cb->pAttachments[i].dstAlphaBlendFactor);

         /* At least llvmpipe applies the blend factor prior to the blend function,
          * regardless of what function is used. (like i965 hardware).
          * It means for MIN/MAX the blend factor has to be stomped to ONE.
          */
         if (cb->pAttachments[i].colorBlendOp == VK_BLEND_OP_MIN ||
             cb->pAttachments[i].colorBlendOp == VK_BLEND_OP_MAX) {
            state->blend_state.rt[i].rgb_src_factor = PIPE_BLENDFACTOR_ONE;
            state->blend_state.rt[i].rgb_dst_factor = PIPE_BLENDFACTOR_ONE;
         }

         if (cb->pAttachments[i].alphaBlendOp == VK_BLEND_OP_MIN ||
             cb->pAttachments[i].alphaBlendOp == VK_BLEND_OP_MAX) {
            state->blend_state.rt[i].alpha_src_factor = PIPE_BLENDFACTOR_ONE;
            state->blend_state.rt[i].alpha_dst_factor = PIPE_BLENDFACTOR_ONE;
         }
      }
      state->blend_dirty = true;
      if (!dynamic_states[VK_DYNAMIC_STATE_BLEND_CONSTANTS]) {
         memcpy(state->blend_color.color, cb->blendConstants, 4 * sizeof(float));
         state->blend_color_dirty = true;
      }
   } else {
      memset(&state->blend_state, 0, sizeof(state->blend_state));
      state->blend_dirty = true;
   }

   if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_VERTEX_INPUT_EXT)]) {
      const VkPipelineVertexInputStateCreateInfo *vi = pipeline->graphics_create_info.pVertexInputState;
      int i;
      const VkPipelineVertexInputDivisorStateCreateInfoEXT *div_state =
         vk_find_struct_const(vi->pNext,
                              PIPELINE_VERTEX_INPUT_DIVISOR_STATE_CREATE_INFO_EXT);

      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT)]) {
         for (i = 0; i < vi->vertexBindingDescriptionCount; i++) {
            state->vb[i].stride = vi->pVertexBindingDescriptions[i].stride;
         }
      }

      int max_location = -1;
      for (i = 0; i < vi->vertexAttributeDescriptionCount; i++) {
         unsigned location = vi->pVertexAttributeDescriptions[i].location;
         state->velem.velems[location].src_offset = vi->pVertexAttributeDescriptions[i].offset;
         state->velem.velems[location].vertex_buffer_index = vi->pVertexAttributeDescriptions[i].binding;
         state->velem.velems[location].src_format = lvp_vk_format_to_pipe_format(vi->pVertexAttributeDescriptions[i].format);

         switch (vi->pVertexBindingDescriptions[vi->pVertexAttributeDescriptions[i].binding].inputRate) {
         case VK_VERTEX_INPUT_RATE_VERTEX:
            state->velem.velems[location].instance_divisor = 0;
            break;
         case VK_VERTEX_INPUT_RATE_INSTANCE:
            if (div_state) {
               for (unsigned j = 0; j < div_state->vertexBindingDivisorCount; j++) {
                  const VkVertexInputBindingDivisorDescriptionEXT *desc =
                     &div_state->pVertexBindingDivisors[j];
                  if (desc->binding == state->velem.velems[location].vertex_buffer_index) {
                     state->velem.velems[location].instance_divisor = desc->divisor;
                     break;
                  }
               }
            } else
               state->velem.velems[location].instance_divisor = 1;
            break;
         default:
            assert(0);
            break;
         }

         if ((int)location > max_location)
            max_location = location;
      }
      state->velem.count = max_location + 1;
      state->vb_dirty = true;
      state->ve_dirty = true;
   }

   {
      const VkPipelineInputAssemblyStateCreateInfo *ia = pipeline->graphics_create_info.pInputAssemblyState;

      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT)]) {
         state->info.mode = vk_conv_topology(ia->topology);
         state->rs_dirty = true;
      }
      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE_EXT)])
         state->info.primitive_restart = ia->primitiveRestartEnable;
   }

   if (pipeline->graphics_create_info.pTessellationState) {
      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT)]) {
         const VkPipelineTessellationStateCreateInfo *ts = pipeline->graphics_create_info.pTessellationState;
         state->info.vertices_per_patch = ts->patchControlPoints;
      }
   } else
      state->info.vertices_per_patch = 0;

   if (pipeline->graphics_create_info.pViewportState) {
      const VkPipelineViewportStateCreateInfo *vpi= pipeline->graphics_create_info.pViewportState;
      int i;

      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT_EXT)]) {
         state->num_viewports = vpi->viewportCount;
         state->vp_dirty = true;
      }
      if (!dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT_EXT)]) {
         state->num_scissors = vpi->scissorCount;
         state->scissor_dirty = true;
      }

      if (!dynamic_states[VK_DYNAMIC_STATE_VIEWPORT] &&
          !dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT_EXT)]) {
         for (i = 0; i < vpi->viewportCount; i++)
            get_viewport_xform(&vpi->pViewports[i], state->viewports[i].scale, state->viewports[i].translate);
         state->vp_dirty = true;
      }
      if (!dynamic_states[VK_DYNAMIC_STATE_SCISSOR] &&
          !dynamic_states[conv_dynamic_state_idx(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT_EXT)]) {
         for (i = 0; i < vpi->scissorCount; i++) {
            const VkRect2D *ss = &vpi->pScissors[i];
            state->scissors[i].minx = ss->offset.x;
            state->scissors[i].miny = ss->offset.y;
            state->scissors[i].maxx = ss->offset.x + ss->extent.width;
            state->scissors[i].maxy = ss->offset.y + ss->extent.height;
            state->scissor_dirty = true;
         }

      }
   }

   if (fb_samples != state->framebuffer.samples) {
      state->framebuffer.samples = fb_samples;
      state->pctx->set_framebuffer_state(state->pctx, &state->framebuffer);
   }
}

static void handle_pipeline(struct lvp_cmd_buffer_entry *cmd,
                            struct rendering_state *state)
{
   struct lvp_pipeline *pipeline = cmd->u.pipeline.pipeline;
   if (pipeline->is_compute_pipeline)
      handle_compute_pipeline(cmd, state);
   else
      handle_graphics_pipeline(cmd, state);
}

static void handle_vertex_buffers(struct lvp_cmd_buffer_entry *cmd,
                                  struct rendering_state *state)
{
   int i;
   struct lvp_cmd_bind_vertex_buffers *vcb = &cmd->u.vertex_buffers;
   for (i = 0; i < vcb->binding_count; i++) {
      int idx = i + vcb->first;

      state->vb[idx].buffer_offset = vcb->offsets[i];
      state->vb[idx].buffer.resource = vcb->buffers[i] ? vcb->buffers[i]->bo : NULL;

      if (vcb->strides) {
         state->vb[idx].stride = vcb->strides[i];
      }
   }
   if (vcb->first < state->start_vb)
      state->start_vb = vcb->first;
   if (vcb->first + vcb->binding_count >= state->num_vb)
      state->num_vb = vcb->first + vcb->binding_count;
   state->vb_dirty = true;
}

struct dyn_info {
   struct {
      uint16_t const_buffer_count;
      uint16_t shader_buffer_count;
      uint16_t sampler_count;
      uint16_t sampler_view_count;
      uint16_t image_count;
   } stage[MESA_SHADER_STAGES];

   uint32_t dyn_index;
   const uint32_t *dynamic_offsets;
   uint32_t dynamic_offset_count;
};

static void fill_sampler(struct pipe_sampler_state *ss,
                         struct lvp_sampler *samp)
{
   ss->wrap_s = vk_conv_wrap_mode(samp->create_info.addressModeU);
   ss->wrap_t = vk_conv_wrap_mode(samp->create_info.addressModeV);
   ss->wrap_r = vk_conv_wrap_mode(samp->create_info.addressModeW);
   ss->min_img_filter = samp->create_info.minFilter == VK_FILTER_LINEAR ? PIPE_TEX_FILTER_LINEAR : PIPE_TEX_FILTER_NEAREST;
   ss->min_mip_filter = samp->create_info.mipmapMode == VK_SAMPLER_MIPMAP_MODE_LINEAR ? PIPE_TEX_MIPFILTER_LINEAR : PIPE_TEX_MIPFILTER_NEAREST;
   ss->mag_img_filter = samp->create_info.magFilter == VK_FILTER_LINEAR ? PIPE_TEX_FILTER_LINEAR : PIPE_TEX_FILTER_NEAREST;
   ss->min_lod = samp->create_info.minLod;
   ss->max_lod = samp->create_info.maxLod;
   ss->lod_bias = samp->create_info.mipLodBias;
   ss->max_anisotropy = samp->create_info.maxAnisotropy;
   ss->normalized_coords = !samp->create_info.unnormalizedCoordinates;
   ss->compare_mode = samp->create_info.compareEnable ? PIPE_TEX_COMPARE_R_TO_TEXTURE : PIPE_TEX_COMPARE_NONE;
   ss->compare_func = samp->create_info.compareOp;
   ss->seamless_cube_map = true;
   ss->reduction_mode = samp->reduction_mode;
   memcpy(&ss->border_color, &samp->border_color,
          sizeof(union pipe_color_union));
}

static void fill_sampler_stage(struct rendering_state *state,
                               struct dyn_info *dyn_info,
                               gl_shader_stage stage,
                               enum pipe_shader_type p_stage,
                               int array_idx,
                               const union lvp_descriptor_info *descriptor,
                               const struct lvp_descriptor_set_binding_layout *binding)
{
   int ss_idx = binding->stage[stage].sampler_index;
   if (ss_idx == -1)
      return;
   ss_idx += array_idx;
   ss_idx += dyn_info->stage[stage].sampler_count;
   fill_sampler(&state->ss[p_stage][ss_idx], binding->immutable_samplers ? binding->immutable_samplers[array_idx] : descriptor->sampler);
   if (state->num_sampler_states[p_stage] <= ss_idx)
      state->num_sampler_states[p_stage] = ss_idx + 1;
   state->ss_dirty[p_stage] = true;
}

#define fix_depth_swizzle(x) do { \
  if (x > PIPE_SWIZZLE_X && x < PIPE_SWIZZLE_0) \
    x = PIPE_SWIZZLE_0;				\
  } while (0)
#define fix_depth_swizzle_a(x) do { \
  if (x > PIPE_SWIZZLE_X && x < PIPE_SWIZZLE_0) \
    x = PIPE_SWIZZLE_1;				\
  } while (0)

static void fill_sampler_view_stage(struct rendering_state *state,
                                    struct dyn_info *dyn_info,
                                    gl_shader_stage stage,
                                    enum pipe_shader_type p_stage,
                                    int array_idx,
                                    const union lvp_descriptor_info *descriptor,
                                    const struct lvp_descriptor_set_binding_layout *binding)
{
   int sv_idx = binding->stage[stage].sampler_view_index;
   if (sv_idx == -1)
      return;
   sv_idx += array_idx;
   sv_idx += dyn_info->stage[stage].sampler_view_count;
   struct lvp_image_view *iv = descriptor->iview;
   struct pipe_sampler_view templ;

   enum pipe_format pformat;
   if (iv->subresourceRange.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT)
      pformat = lvp_vk_format_to_pipe_format(iv->format);
   else if (iv->subresourceRange.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT)
      pformat = util_format_stencil_only(lvp_vk_format_to_pipe_format(iv->format));
   else
      pformat = lvp_vk_format_to_pipe_format(iv->format);
   u_sampler_view_default_template(&templ,
                                   iv->image->bo,
                                   pformat);
   if (iv->view_type == VK_IMAGE_VIEW_TYPE_1D)
      templ.target = PIPE_TEXTURE_1D;
   if (iv->view_type == VK_IMAGE_VIEW_TYPE_2D)
      templ.target = PIPE_TEXTURE_2D;
   if (iv->view_type == VK_IMAGE_VIEW_TYPE_CUBE)
      templ.target = PIPE_TEXTURE_CUBE;
   if (iv->view_type == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY)
      templ.target = PIPE_TEXTURE_CUBE_ARRAY;
   templ.u.tex.first_layer = iv->subresourceRange.baseArrayLayer;
   templ.u.tex.last_layer = iv->subresourceRange.baseArrayLayer + lvp_get_layerCount(iv->image, &iv->subresourceRange) - 1;
   templ.u.tex.first_level = iv->subresourceRange.baseMipLevel;
   templ.u.tex.last_level = iv->subresourceRange.baseMipLevel + lvp_get_levelCount(iv->image, &iv->subresourceRange) - 1;
   if (iv->components.r != VK_COMPONENT_SWIZZLE_IDENTITY)
      templ.swizzle_r = vk_conv_swizzle(iv->components.r);
   if (iv->components.g != VK_COMPONENT_SWIZZLE_IDENTITY)
      templ.swizzle_g = vk_conv_swizzle(iv->components.g);
   if (iv->components.b != VK_COMPONENT_SWIZZLE_IDENTITY)
      templ.swizzle_b = vk_conv_swizzle(iv->components.b);
   if (iv->components.a != VK_COMPONENT_SWIZZLE_IDENTITY)
      templ.swizzle_a = vk_conv_swizzle(iv->components.a);

   /* depth stencil swizzles need special handling to pass VK CTS
    * but also for zink GL tests.
    * piping A swizzle into R fixes GL_ALPHA depth texture mode
    * only swizzling from R/0/1 (for alpha) fixes VK CTS tests
    * and a bunch of zink tests.
   */
   if (iv->subresourceRange.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT ||
       iv->subresourceRange.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT) {
      if (templ.swizzle_a == PIPE_SWIZZLE_X)
         templ.swizzle_r = PIPE_SWIZZLE_X;
      fix_depth_swizzle(templ.swizzle_r);
      fix_depth_swizzle(templ.swizzle_g);
      fix_depth_swizzle(templ.swizzle_b);
      fix_depth_swizzle_a(templ.swizzle_a);
   }

   if (state->sv[p_stage][sv_idx])
      pipe_sampler_view_reference(&state->sv[p_stage][sv_idx], NULL);
   state->sv[p_stage][sv_idx] = state->pctx->create_sampler_view(state->pctx, iv->image->bo, &templ);
   if (state->num_sampler_views[p_stage] <= sv_idx)
      state->num_sampler_views[p_stage] = sv_idx + 1;
   state->sv_dirty[p_stage] = true;
}

static void fill_sampler_buffer_view_stage(struct rendering_state *state,
                                           struct dyn_info *dyn_info,
                                           gl_shader_stage stage,
                                           enum pipe_shader_type p_stage,
                                           int array_idx,
                                           const union lvp_descriptor_info *descriptor,
                                           const struct lvp_descriptor_set_binding_layout *binding)
{
   int sv_idx = binding->stage[stage].sampler_view_index;
   if (sv_idx == -1)
      return;
   sv_idx += array_idx;
   sv_idx += dyn_info->stage[stage].sampler_view_count;
   struct lvp_buffer_view *bv = descriptor->buffer_view;
   struct pipe_sampler_view templ;
   memset(&templ, 0, sizeof(templ));
   templ.target = PIPE_BUFFER;
   templ.swizzle_r = PIPE_SWIZZLE_X;
   templ.swizzle_g = PIPE_SWIZZLE_Y;
   templ.swizzle_b = PIPE_SWIZZLE_Z;
   templ.swizzle_a = PIPE_SWIZZLE_W;
   templ.format = bv->pformat;
   templ.u.buf.offset = bv->offset + bv->buffer->offset;
   templ.u.buf.size = bv->range == VK_WHOLE_SIZE ? (bv->buffer->size - bv->offset) : bv->range;
   templ.texture = bv->buffer->bo;
   templ.context = state->pctx;

   if (state->sv[p_stage][sv_idx])
      pipe_sampler_view_reference(&state->sv[p_stage][sv_idx], NULL);
   state->sv[p_stage][sv_idx] = state->pctx->create_sampler_view(state->pctx, bv->buffer->bo, &templ);
   if (state->num_sampler_views[p_stage] <= sv_idx)
      state->num_sampler_views[p_stage] = sv_idx + 1;
   state->sv_dirty[p_stage] = true;
}

static void fill_image_view_stage(struct rendering_state *state,
                                  struct dyn_info *dyn_info,
                                  gl_shader_stage stage,
                                  enum pipe_shader_type p_stage,
                                  int array_idx,
                                  const union lvp_descriptor_info *descriptor,
                                  const struct lvp_descriptor_set_binding_layout *binding)
{
   struct lvp_image_view *iv = descriptor->iview;
   int idx = binding->stage[stage].image_index;
   if (idx == -1)
      return;
   idx += array_idx;
   idx += dyn_info->stage[stage].image_count;
   state->iv[p_stage][idx].resource = iv->image->bo;
   if (iv->subresourceRange.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT)
      state->iv[p_stage][idx].format = lvp_vk_format_to_pipe_format(iv->format);
   else if (iv->subresourceRange.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT)
      state->iv[p_stage][idx].format = util_format_stencil_only(lvp_vk_format_to_pipe_format(iv->format));
   else
      state->iv[p_stage][idx].format = lvp_vk_format_to_pipe_format(iv->format);

   if (iv->view_type == VK_IMAGE_VIEW_TYPE_3D) {
      state->iv[p_stage][idx].u.tex.first_layer = 0;
      state->iv[p_stage][idx].u.tex.last_layer = u_minify(iv->image->bo->depth0, iv->subresourceRange.baseMipLevel) - 1;
   } else {
      state->iv[p_stage][idx].u.tex.first_layer = iv->subresourceRange.baseArrayLayer;
      state->iv[p_stage][idx].u.tex.last_layer = iv->subresourceRange.baseArrayLayer + lvp_get_layerCount(iv->image, &iv->subresourceRange) - 1;
   }
   state->iv[p_stage][idx].u.tex.level = iv->subresourceRange.baseMipLevel;
   if (state->num_shader_images[p_stage] <= idx)
      state->num_shader_images[p_stage] = idx + 1;
   state->iv_dirty[p_stage] = true;
}

static void fill_image_buffer_view_stage(struct rendering_state *state,
                                         struct dyn_info *dyn_info,
                                         gl_shader_stage stage,
                                         enum pipe_shader_type p_stage,
                                         int array_idx,
                                         const union lvp_descriptor_info *descriptor,
                                         const struct lvp_descriptor_set_binding_layout *binding)
{
   struct lvp_buffer_view *bv = descriptor->buffer_view;
   int idx = binding->stage[stage].image_index;
   if (idx == -1)
      return;
   idx += array_idx;
   idx += dyn_info->stage[stage].image_count;
   state->iv[p_stage][idx].resource = bv->buffer->bo;
   state->iv[p_stage][idx].format = bv->pformat;
   state->iv[p_stage][idx].u.buf.offset = bv->offset + bv->buffer->offset;
   state->iv[p_stage][idx].u.buf.size = bv->range == VK_WHOLE_SIZE ? (bv->buffer->size - bv->offset): bv->range;
   if (state->num_shader_images[p_stage] <= idx)
      state->num_shader_images[p_stage] = idx + 1;
   state->iv_dirty[p_stage] = true;
}

static void handle_descriptor(struct rendering_state *state,
                              struct dyn_info *dyn_info,
                              const struct lvp_descriptor_set_binding_layout *binding,
                              gl_shader_stage stage,
                              enum pipe_shader_type p_stage,
                              int array_idx,
                              VkDescriptorType type,
                              const union lvp_descriptor_info *descriptor)
{
   bool is_dynamic = type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
      type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

   switch (type) {
   case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
   case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE: {
      fill_image_view_stage(state, dyn_info, stage, p_stage, array_idx, descriptor, binding);
      break;
   }
   case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
   case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: {
      int idx = binding->stage[stage].const_buffer_index;
      if (idx == -1)
         return;
      idx += array_idx;
      idx += dyn_info->stage[stage].const_buffer_count;
      state->const_buffer[p_stage][idx].buffer = descriptor->buffer->bo;
      state->const_buffer[p_stage][idx].buffer_offset = descriptor->offset + descriptor->buffer->offset;
      if (is_dynamic) {
         uint32_t offset = dyn_info->dynamic_offsets[dyn_info->dyn_index + binding->dynamic_index + array_idx];
         state->const_buffer[p_stage][idx].buffer_offset += offset;
      }
      if (descriptor->range == VK_WHOLE_SIZE)
         state->const_buffer[p_stage][idx].buffer_size = descriptor->buffer->bo->width0 - state->const_buffer[p_stage][idx].buffer_offset;
      else
         state->const_buffer[p_stage][idx].buffer_size = descriptor->range;
      if (state->num_const_bufs[p_stage] <= idx)
         state->num_const_bufs[p_stage] = idx + 1;
      state->constbuf_dirty[p_stage] = true;
      break;
   }
   case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
   case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: {
      int idx = binding->stage[stage].shader_buffer_index;
      if (idx == -1)
         return;
      idx += array_idx;
      idx += dyn_info->stage[stage].shader_buffer_count;
      state->sb[p_stage][idx].buffer = descriptor->buffer->bo;
      state->sb[p_stage][idx].buffer_offset = descriptor->offset + descriptor->buffer->offset;
      if (is_dynamic) {
         uint32_t offset = dyn_info->dynamic_offsets[dyn_info->dyn_index + binding->dynamic_index + array_idx];
         state->sb[p_stage][idx].buffer_offset += offset;
      }
      if (descriptor->range == VK_WHOLE_SIZE)
         state->sb[p_stage][idx].buffer_size = descriptor->buffer->bo->width0 - state->sb[p_stage][idx].buffer_offset;
      else
         state->sb[p_stage][idx].buffer_size = descriptor->range;
      if (state->num_shader_buffers[p_stage] <= idx)
         state->num_shader_buffers[p_stage] = idx + 1;
      state->sb_dirty[p_stage] = true;
      break;
   }
   case VK_DESCRIPTOR_TYPE_SAMPLER:
      if (!descriptor->sampler)
         return;
      fill_sampler_stage(state, dyn_info, stage, p_stage, array_idx, descriptor, binding);
      break;
   case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
      fill_sampler_view_stage(state, dyn_info, stage, p_stage, array_idx, descriptor, binding);
      break;
   case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
      fill_sampler_stage(state, dyn_info, stage, p_stage, array_idx, descriptor, binding);
      fill_sampler_view_stage(state, dyn_info, stage, p_stage, array_idx, descriptor, binding);
      break;
   case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
      fill_sampler_buffer_view_stage(state, dyn_info, stage, p_stage, array_idx, descriptor, binding);
      break;
   case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
      fill_image_buffer_view_stage(state, dyn_info, stage, p_stage, array_idx, descriptor, binding);
      break;
   default:
      fprintf(stderr, "Unhandled descriptor set %d\n", type);
      break;
   }
}

static void handle_set_stage(struct rendering_state *state,
                             struct dyn_info *dyn_info,
                             const struct lvp_descriptor_set *set,
                             gl_shader_stage stage,
                             enum pipe_shader_type p_stage)
{
   int j;
   for (j = 0; j < set->layout->binding_count; j++) {
      const struct lvp_descriptor_set_binding_layout *binding;
      const struct lvp_descriptor *descriptor;
      binding = &set->layout->binding[j];

      if (binding->valid) {
         for (int i = 0; i < binding->array_size; i++) {
            descriptor = &set->descriptors[binding->descriptor_index + i];
            handle_descriptor(state, dyn_info, binding, stage, p_stage, i, descriptor->type, &descriptor->info);
         }
      }
   }
}

static void increment_dyn_info(struct dyn_info *dyn_info,
                               struct lvp_descriptor_set_layout *layout, bool inc_dyn)
{
   for (gl_shader_stage stage = MESA_SHADER_VERTEX; stage < MESA_SHADER_STAGES; stage++) {
      dyn_info->stage[stage].const_buffer_count += layout->stage[stage].const_buffer_count;
      dyn_info->stage[stage].shader_buffer_count += layout->stage[stage].shader_buffer_count;
      dyn_info->stage[stage].sampler_count += layout->stage[stage].sampler_count;
      dyn_info->stage[stage].sampler_view_count += layout->stage[stage].sampler_view_count;
      dyn_info->stage[stage].image_count += layout->stage[stage].image_count;
   }
   if (inc_dyn)
      dyn_info->dyn_index += layout->dynamic_offset_count;
}

static void handle_compute_descriptor_sets(struct lvp_cmd_buffer_entry *cmd,
                                           struct dyn_info *dyn_info,
                                           struct rendering_state *state)
{
   struct lvp_cmd_bind_descriptor_sets *bds = &cmd->u.descriptor_sets;
   int i;

   for (i = 0; i < bds->first; i++) {
      increment_dyn_info(dyn_info, bds->set_layout[i], false);
   }
   for (i = 0; i < bds->count; i++) {
      const struct lvp_descriptor_set *set = bds->sets[i];

      if (set->layout->shader_stages & VK_SHADER_STAGE_COMPUTE_BIT)
         handle_set_stage(state, dyn_info, set, MESA_SHADER_COMPUTE, PIPE_SHADER_COMPUTE);
      increment_dyn_info(dyn_info, bds->set_layout[bds->first + i], true);
   }
}

static void handle_descriptor_sets(struct lvp_cmd_buffer_entry *cmd,
                                   struct rendering_state *state)
{
   struct lvp_cmd_bind_descriptor_sets *bds = &cmd->u.descriptor_sets;
   int i;
   struct dyn_info dyn_info;

   dyn_info.dyn_index = 0;
   dyn_info.dynamic_offsets = bds->dynamic_offsets;
   dyn_info.dynamic_offset_count = bds->dynamic_offset_count;

   memset(dyn_info.stage, 0, sizeof(dyn_info.stage));
   if (bds->bind_point == VK_PIPELINE_BIND_POINT_COMPUTE) {
      handle_compute_descriptor_sets(cmd, &dyn_info, state);
      return;
   }

   for (i = 0; i < bds->first; i++) {
      increment_dyn_info(&dyn_info, bds->set_layout[i], false);
   }

   for (i = 0; i < bds->count; i++) {
      const struct lvp_descriptor_set *set = bds->sets[i];

      if (set->layout->shader_stages & VK_SHADER_STAGE_VERTEX_BIT)
         handle_set_stage(state, &dyn_info, set, MESA_SHADER_VERTEX, PIPE_SHADER_VERTEX);

      if (set->layout->shader_stages & VK_SHADER_STAGE_GEOMETRY_BIT)
         handle_set_stage(state, &dyn_info, set, MESA_SHADER_GEOMETRY, PIPE_SHADER_GEOMETRY);

      if (set->layout->shader_stages & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
         handle_set_stage(state, &dyn_info, set, MESA_SHADER_TESS_CTRL, PIPE_SHADER_TESS_CTRL);

      if (set->layout->shader_stages & VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
         handle_set_stage(state, &dyn_info, set, MESA_SHADER_TESS_EVAL, PIPE_SHADER_TESS_EVAL);

      if (set->layout->shader_stages & VK_SHADER_STAGE_FRAGMENT_BIT)
	 handle_set_stage(state, &dyn_info, set, MESA_SHADER_FRAGMENT, PIPE_SHADER_FRAGMENT);
      increment_dyn_info(&dyn_info, bds->set_layout[bds->first + i], true);
   }
}

static struct pipe_surface *create_img_surface_bo(struct rendering_state *state,
                                                  VkImageSubresourceRange *range,
                                                  struct pipe_resource *bo,
                                                  enum pipe_format pformat,
                                                  int width,
                                                  int height,
                                                  int base_layer, int layer_count,
                                                  int level)
{
   struct pipe_surface template;

   memset(&template, 0, sizeof(struct pipe_surface));

   template.format = pformat;
   template.width = width;
   template.height = height;
   template.u.tex.first_layer = range->baseArrayLayer + base_layer;
   template.u.tex.last_layer = range->baseArrayLayer + layer_count;
   template.u.tex.level = range->baseMipLevel + level;

   if (template.format == PIPE_FORMAT_NONE)
      return NULL;
   return state->pctx->create_surface(state->pctx,
                                      bo, &template);

}
static struct pipe_surface *create_img_surface(struct rendering_state *state,
                                               struct lvp_image_view *imgv,
                                               VkFormat format, int width,
                                               int height,
                                               int base_layer, int layer_count)
{
   return create_img_surface_bo(state, &imgv->subresourceRange, imgv->image->bo,
                                lvp_vk_format_to_pipe_format(format), width, height, base_layer, layer_count, 0);
}

static void add_img_view_surface(struct rendering_state *state,
                                 struct lvp_image_view *imgv, VkFormat format, int width, int height)
{
   if (!imgv->surface) {
      imgv->surface = create_img_surface(state, imgv, format,
                                         width, height,
                                         0, lvp_get_layerCount(imgv->image, &imgv->subresourceRange) - 1);
   }
}

static inline bool
attachment_needs_clear(struct rendering_state *state,
                       uint32_t a)
{
   const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];
   uint32_t view_mask = subpass->view_mask;
   return (a != VK_ATTACHMENT_UNUSED &&
           state->pending_clear_aspects[a] &&
           (!view_mask || (view_mask & ~state->cleared_views[a])));
}

static bool
subpass_needs_clear(struct rendering_state *state)
{
   uint32_t a;
   const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];
   for (uint32_t i = 0; i < subpass->color_count; i++) {
      a = subpass->color_attachments[i].attachment;
      if (attachment_needs_clear(state, a))
         return true;
   }
   if (subpass->depth_stencil_attachment) {
      a = subpass->depth_stencil_attachment->attachment;
      if (attachment_needs_clear(state, a))
         return true;
   }
   return false;
}

static void clear_attachment_layers(struct rendering_state *state,
                                    struct lvp_image_view *imgv,
                                    VkRect2D *rect,
                                    unsigned base_layer, unsigned layer_count,
                                    unsigned ds_clear_flags, double dclear_val,
                                    uint32_t sclear_val,
                                    union pipe_color_union *col_val)
{
   struct pipe_surface *clear_surf = create_img_surface(state,
                                                        imgv,
                                                        imgv->format,
                                                        state->framebuffer.width,
                                                        state->framebuffer.height,
                                                        base_layer,
                                                        base_layer + layer_count - 1);

   if (ds_clear_flags) {
      state->pctx->clear_depth_stencil(state->pctx,
                                       clear_surf,
                                       ds_clear_flags,
                                       dclear_val, sclear_val,
                                       rect->offset.x, rect->offset.y,
                                       rect->extent.width, rect->extent.height,
                                       true);
   } else {
      state->pctx->clear_render_target(state->pctx, clear_surf,
                                       col_val,
                                       rect->offset.x, rect->offset.y,
                                       rect->extent.width, rect->extent.height,
                                       true);
   }
   state->pctx->surface_destroy(state->pctx, clear_surf);
}

static struct lvp_image_view *
get_attachment(struct rendering_state *state,
               unsigned idx)
{
   if (state->imageless_views)
      return state->imageless_views[idx];
   else
      return state->vk_framebuffer->attachments[idx];
}

static void render_subpass_clear(struct rendering_state *state)
{
   const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];

   for (unsigned i = 0; i < subpass->color_count; i++) {
      uint32_t a = subpass->color_attachments[i].attachment;

      if (!attachment_needs_clear(state, a))
         continue;

      union pipe_color_union color_clear_val = { 0 };
      const VkClearValue value = state->attachments[a].clear_value;
      color_clear_val.ui[0] = value.color.uint32[0];
      color_clear_val.ui[1] = value.color.uint32[1];
      color_clear_val.ui[2] = value.color.uint32[2];
      color_clear_val.ui[3] = value.color.uint32[3];

      struct lvp_image_view *imgv = get_attachment(state, a);

      assert(imgv->surface);

      if (subpass->view_mask) {
         u_foreach_bit(i, subpass->view_mask)
            clear_attachment_layers(state, imgv, &state->render_area,
                                    i, 1, 0, 0, 0, &color_clear_val);
         state->cleared_views[a] |= subpass->view_mask;
      } else {
         state->pctx->clear_render_target(state->pctx,
                                          imgv->surface,
                                          &color_clear_val,
                                          state->render_area.offset.x, state->render_area.offset.y,
                                          state->render_area.extent.width, state->render_area.extent.height,
                                          false);
         state->pending_clear_aspects[a] = 0;
      }
   }

   if (subpass->depth_stencil_attachment) {
      uint32_t ds = subpass->depth_stencil_attachment->attachment;

      if (!attachment_needs_clear(state, ds))
         return;

      struct lvp_render_pass_attachment *att = &state->pass->attachments[ds];
      struct lvp_image_view *imgv = get_attachment(state, ds);

      assert (util_format_is_depth_or_stencil(imgv->surface->format));

      const struct util_format_description *desc = util_format_description(imgv->surface->format);
      double dclear_val = 0;
      uint32_t sclear_val = 0;
      uint32_t ds_clear_flags = 0;

      if ((util_format_has_stencil(desc) && att->stencil_load_op == VK_ATTACHMENT_LOAD_OP_CLEAR) ||
          (util_format_is_depth_and_stencil(imgv->surface->format) && att->stencil_load_op == VK_ATTACHMENT_LOAD_OP_DONT_CARE)) {
         ds_clear_flags |= PIPE_CLEAR_STENCIL;
         if (att->stencil_load_op == VK_ATTACHMENT_LOAD_OP_CLEAR)
            sclear_val = state->attachments[ds].clear_value.depthStencil.stencil;
      }
      if ((util_format_has_depth(desc) && att->load_op == VK_ATTACHMENT_LOAD_OP_CLEAR) ||
          (util_format_is_depth_and_stencil(imgv->surface->format) && att->load_op == VK_ATTACHMENT_LOAD_OP_DONT_CARE)) {
         ds_clear_flags |= PIPE_CLEAR_DEPTH;
         if (att->load_op == VK_ATTACHMENT_LOAD_OP_CLEAR)
            dclear_val = state->attachments[ds].clear_value.depthStencil.depth;
      }

      assert(imgv->surface);
      if (ds_clear_flags) {
         if (subpass->view_mask) {
            u_foreach_bit(i, subpass->view_mask)
               clear_attachment_layers(state, imgv, &state->render_area,
                                       i, 1, ds_clear_flags, dclear_val, sclear_val, NULL);
            state->cleared_views[ds] |= subpass->view_mask;
         } else {
            state->pctx->clear_depth_stencil(state->pctx,
                                             imgv->surface,
                                             ds_clear_flags,
                                             dclear_val, sclear_val,
                                             state->render_area.offset.x, state->render_area.offset.y,
                                             state->render_area.extent.width, state->render_area.extent.height,
                                             false);
            state->pending_clear_aspects[ds] = 0;
         }
      }

   }

}

static void render_subpass_clear_fast(struct rendering_state *state)
{
   /* attempt to use the clear interface first, then fallback to per-attchment clears */
   const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];
   bool has_color_value = false;
   uint32_t buffers = 0;
   VkClearValue color_value = {0};
   double dclear_val = 0;
   uint32_t sclear_val = 0;

   /*
    * the state tracker clear interface only works if all the attachments have the same
    * clear color.
    */
   /* llvmpipe doesn't support scissored clears yet */
   if (state->render_area.offset.x || state->render_area.offset.y)
      goto slow_clear;

   if (state->render_area.extent.width != state->framebuffer.width ||
       state->render_area.extent.height != state->framebuffer.height)
      goto slow_clear;

   if (subpass->view_mask)
      goto slow_clear;
   for (unsigned i = 0; i < subpass->color_count; i++) {
      uint32_t a = subpass->color_attachments[i].attachment;

      if (!attachment_needs_clear(state, a))
         continue;

      if (has_color_value) {
         if (memcmp(&color_value, &state->attachments[a].clear_value, sizeof(VkClearValue)))
            goto slow_clear;
      } else {
         memcpy(&color_value, &state->attachments[a].clear_value, sizeof(VkClearValue));
         has_color_value = true;
      }
   }

   for (unsigned i = 0; i < subpass->color_count; i++) {
      uint32_t a = subpass->color_attachments[i].attachment;

      if (!attachment_needs_clear(state, a))
         continue;
      buffers |= (PIPE_CLEAR_COLOR0 << i);
      state->pending_clear_aspects[a] = 0;
   }

   if (subpass->depth_stencil_attachment &&
       attachment_needs_clear(state, subpass->depth_stencil_attachment->attachment)) {
      uint32_t ds = subpass->depth_stencil_attachment->attachment;

      struct lvp_render_pass_attachment *att = &state->pass->attachments[ds];
      struct lvp_image_view *imgv = get_attachment(state, ds);
      const struct util_format_description *desc = util_format_description(imgv->surface->format);

      /* also clear stencil for don't care to avoid RMW */
      if ((util_format_has_stencil(desc) && att->stencil_load_op == VK_ATTACHMENT_LOAD_OP_CLEAR) ||
          (util_format_is_depth_and_stencil(imgv->surface->format) && att->stencil_load_op == VK_ATTACHMENT_LOAD_OP_DONT_CARE))
         buffers |= PIPE_CLEAR_STENCIL;
      if (util_format_has_depth(desc) && att->load_op == VK_ATTACHMENT_LOAD_OP_CLEAR)
         buffers |= PIPE_CLEAR_DEPTH;

      dclear_val = state->attachments[ds].clear_value.depthStencil.depth;
      sclear_val = state->attachments[ds].clear_value.depthStencil.stencil;
      state->pending_clear_aspects[ds] = 0;
   }

   union pipe_color_union col_val;
   for (unsigned i = 0; i < 4; i++)
      col_val.ui[i] = color_value.color.uint32[i];

   state->pctx->clear(state->pctx, buffers,
                      NULL, &col_val,
                      dclear_val, sclear_val);
   return;
slow_clear:
   render_subpass_clear(state);
}

static void render_pass_resolve(struct rendering_state *state)
{
   const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];
   if (!subpass->has_color_resolve)
      return;
   for (uint32_t i = 0; i < subpass->color_count; i++) {
      struct lvp_subpass_attachment src_att = subpass->color_attachments[i];
      struct lvp_subpass_attachment dst_att = subpass->resolve_attachments[i];

      if (dst_att.attachment == VK_ATTACHMENT_UNUSED)
         continue;

      struct lvp_image_view *src_imgv = get_attachment(state, src_att.attachment);
      struct lvp_image_view *dst_imgv = get_attachment(state, dst_att.attachment);

      struct pipe_blit_info info;
      memset(&info, 0, sizeof(info));

      info.src.resource = src_imgv->image->bo;
      info.dst.resource = dst_imgv->image->bo;
      info.src.format = src_imgv->pformat;
      info.dst.format = dst_imgv->pformat;
      info.filter = PIPE_TEX_FILTER_NEAREST;
      info.mask = PIPE_MASK_RGBA;
      info.src.box.x = state->render_area.offset.x;
      info.src.box.y = state->render_area.offset.y;
      info.src.box.width = state->render_area.extent.width;
      info.src.box.height = state->render_area.extent.height;
      info.src.box.depth = state->vk_framebuffer->layers;

      info.dst.box = info.src.box;

      info.src.level = src_imgv->subresourceRange.baseMipLevel;
      info.dst.level = dst_imgv->subresourceRange.baseMipLevel;

      state->pctx->blit(state->pctx, &info);
   }
}

static void begin_render_subpass(struct rendering_state *state,
                                 int subpass_idx)
{
   state->subpass = subpass_idx;

   state->framebuffer.nr_cbufs = 0;

   const struct lvp_subpass *subpass = &state->pass->subpasses[subpass_idx];
   for (unsigned i = 0; i < subpass->color_count; i++) {
      struct lvp_subpass_attachment *color_att = &subpass->color_attachments[i];
      if (color_att->attachment != VK_ATTACHMENT_UNUSED) {
         struct lvp_image_view *imgv = get_attachment(state, color_att->attachment);
         add_img_view_surface(state, imgv, state->pass->attachments[color_att->attachment].format, state->framebuffer.width, state->framebuffer.height);
         state->framebuffer.cbufs[state->framebuffer.nr_cbufs] = imgv->surface;
      } else
         state->framebuffer.cbufs[state->framebuffer.nr_cbufs] = NULL;
      state->framebuffer.nr_cbufs++;
   }

   if (subpass->depth_stencil_attachment) {
      struct lvp_subpass_attachment *ds_att = subpass->depth_stencil_attachment;

      if (ds_att->attachment != VK_ATTACHMENT_UNUSED) {
         struct lvp_image_view *imgv = get_attachment(state, ds_att->attachment);
         add_img_view_surface(state, imgv, state->pass->attachments[ds_att->attachment].format, state->framebuffer.width, state->framebuffer.height);
         state->framebuffer.zsbuf = imgv->surface;
      }
   }

   state->pctx->set_framebuffer_state(state->pctx,
                                      &state->framebuffer);

   if (subpass_needs_clear(state))
      render_subpass_clear_fast(state);
}

static void handle_begin_render_pass(struct lvp_cmd_buffer_entry *cmd,
                                     struct rendering_state *state)
{
   state->pass = cmd->u.begin_render_pass.render_pass;
   state->vk_framebuffer = cmd->u.begin_render_pass.framebuffer;
   state->render_area = cmd->u.begin_render_pass.render_area;

   state->attachments = cmd->u.begin_render_pass.attachments;

   state->imageless_views = cmd->u.begin_render_pass.imageless_views;
   state->framebuffer.width = state->vk_framebuffer->width;
   state->framebuffer.height = state->vk_framebuffer->height;
   state->framebuffer.layers = state->vk_framebuffer->layers;

   if (state->num_pending_aspects < state->pass->attachment_count) {
      state->pending_clear_aspects = realloc(state->pending_clear_aspects, sizeof(VkImageAspectFlags) * state->pass->attachment_count);
      state->cleared_views = realloc(state->cleared_views, sizeof(uint32_t) * state->pass->attachment_count);
      state->num_pending_aspects = state->pass->attachment_count;
   }

   for (unsigned a = 0; a < state->pass->attachment_count; a++) {
      state->pending_clear_aspects[a] = state->attachments[a].pending_clear_aspects;
      state->cleared_views[a] = 0;
   }
   begin_render_subpass(state, 0);
}

static void handle_end_render_pass(struct lvp_cmd_buffer_entry *cmd,
                                   struct rendering_state *state)
{
   state->pctx->flush(state->pctx, NULL, 0);

   render_pass_resolve(state);

   state->attachments = NULL;
   state->pass = NULL;
   state->subpass = 0;
}

static void handle_next_subpass(struct lvp_cmd_buffer_entry *cmd,
                                struct rendering_state *state)
{
   state->pctx->flush(state->pctx, NULL, 0);
   render_pass_resolve(state);
   state->subpass++;
   begin_render_subpass(state, state->subpass);
}

static void handle_draw(struct lvp_cmd_buffer_entry *cmd,
                        struct rendering_state *state)
{
   const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];
   state->info.index_size = 0;
   state->info.index.resource = NULL;
   state->info.start_instance = cmd->u.draw.first_instance;
   state->info.instance_count = cmd->u.draw.instance_count;
   state->info.view_mask = subpass->view_mask;
   state->info.increment_draw_id = true;

   state->pctx->draw_vbo(state->pctx, &state->info, 0, NULL, cmd->u.draw.draws, cmd->u.draw.draw_count);
}

static void handle_set_viewport(struct lvp_cmd_buffer_entry *cmd,
                                struct rendering_state *state)
{
   int i;
   unsigned base = 0;
   if (cmd->u.set_viewport.first_viewport == UINT32_MAX)
      state->num_viewports = cmd->u.set_viewport.viewport_count;
   else
      base = cmd->u.set_viewport.first_viewport;

   for (i = 0; i < cmd->u.set_viewport.viewport_count; i++) {
      int idx = i + base;
      const VkViewport *vp = &cmd->u.set_viewport.viewports[i];
      get_viewport_xform(vp, state->viewports[idx].scale, state->viewports[idx].translate);
   }
   state->vp_dirty = true;
}

static void handle_set_scissor(struct lvp_cmd_buffer_entry *cmd,
                               struct rendering_state *state)
{
   int i;
   unsigned base = 0;
   if (cmd->u.set_scissor.first_scissor == UINT32_MAX)
      state->num_scissors = cmd->u.set_scissor.scissor_count;
   else
      base = cmd->u.set_scissor.first_scissor;

   for (i = 0; i < cmd->u.set_scissor.scissor_count; i++) {
      int idx = i + base;
      const VkRect2D *ss = &cmd->u.set_scissor.scissors[i];
      state->scissors[idx].minx = ss->offset.x;
      state->scissors[idx].miny = ss->offset.y;
      state->scissors[idx].maxx = ss->offset.x + ss->extent.width;
      state->scissors[idx].maxy = ss->offset.y + ss->extent.height;
   }
   state->scissor_dirty = true;
}

static void handle_set_line_width(struct lvp_cmd_buffer_entry *cmd,
                                  struct rendering_state *state)
{
   state->rs_state.line_width = cmd->u.set_line_width.line_width;
   state->rs_dirty = true;
}

static void handle_set_depth_bias(struct lvp_cmd_buffer_entry *cmd,
                                  struct rendering_state *state)
{
   state->depth_bias.offset_units = cmd->u.set_depth_bias.constant_factor;
   state->depth_bias.offset_scale = cmd->u.set_depth_bias.slope_factor;
   state->depth_bias.offset_clamp = cmd->u.set_depth_bias.clamp;
   state->rs_dirty = true;
}

static void handle_set_blend_constants(struct lvp_cmd_buffer_entry *cmd,
                                       struct rendering_state *state)
{
   memcpy(state->blend_color.color, cmd->u.set_blend_constants.blend_constants, 4 * sizeof(float));
   state->blend_color_dirty = true;
}

static void handle_set_depth_bounds(struct lvp_cmd_buffer_entry *cmd,
                                    struct rendering_state *state)
{
   state->dsa_dirty |= !DOUBLE_EQ(state->dsa_state.depth_bounds_min, cmd->u.set_depth_bounds.min_depth);
   state->dsa_dirty |= !DOUBLE_EQ(state->dsa_state.depth_bounds_max, cmd->u.set_depth_bounds.max_depth);
   state->dsa_state.depth_bounds_min = cmd->u.set_depth_bounds.min_depth;
   state->dsa_state.depth_bounds_max = cmd->u.set_depth_bounds.max_depth;
}

static void handle_set_stencil_compare_mask(struct lvp_cmd_buffer_entry *cmd,
                                            struct rendering_state *state)
{
   if (cmd->u.stencil_vals.face_mask & VK_STENCIL_FACE_FRONT_BIT)
      state->dsa_state.stencil[0].valuemask = cmd->u.stencil_vals.value;
   if (cmd->u.stencil_vals.face_mask & VK_STENCIL_FACE_BACK_BIT)
      state->dsa_state.stencil[1].valuemask = cmd->u.stencil_vals.value;
   state->dsa_dirty = true;
}

static void handle_set_stencil_write_mask(struct lvp_cmd_buffer_entry *cmd,
                                          struct rendering_state *state)
{
   if (cmd->u.stencil_vals.face_mask & VK_STENCIL_FACE_FRONT_BIT)
      state->dsa_state.stencil[0].writemask = cmd->u.stencil_vals.value;
   if (cmd->u.stencil_vals.face_mask & VK_STENCIL_FACE_BACK_BIT)
      state->dsa_state.stencil[1].writemask = cmd->u.stencil_vals.value;
   state->dsa_dirty = true;
}

static void handle_set_stencil_reference(struct lvp_cmd_buffer_entry *cmd,
                                         struct rendering_state *state)
{
   if (cmd->u.stencil_vals.face_mask & VK_STENCIL_FACE_FRONT_BIT)
      state->stencil_ref.ref_value[0] = cmd->u.stencil_vals.value;
   if (cmd->u.stencil_vals.face_mask & VK_STENCIL_FACE_BACK_BIT)
      state->stencil_ref.ref_value[1] = cmd->u.stencil_vals.value;
   state->stencil_ref_dirty = true;
}

static void
copy_depth_rect(ubyte * dst,
                enum pipe_format dst_format,
                unsigned dst_stride,
                unsigned dst_x,
                unsigned dst_y,
                unsigned width,
                unsigned height,
                const ubyte * src,
                enum pipe_format src_format,
                int src_stride,
                unsigned src_x,
                unsigned src_y)
{
   int src_stride_pos = src_stride < 0 ? -src_stride : src_stride;
   int src_blocksize = util_format_get_blocksize(src_format);
   int src_blockwidth = util_format_get_blockwidth(src_format);
   int src_blockheight = util_format_get_blockheight(src_format);
   int dst_blocksize = util_format_get_blocksize(dst_format);
   int dst_blockwidth = util_format_get_blockwidth(dst_format);
   int dst_blockheight = util_format_get_blockheight(dst_format);

   assert(src_blocksize > 0);
   assert(src_blockwidth > 0);
   assert(src_blockheight > 0);

   dst_x /= dst_blockwidth;
   dst_y /= dst_blockheight;
   width = (width + src_blockwidth - 1)/src_blockwidth;
   height = (height + src_blockheight - 1)/src_blockheight;
   src_x /= src_blockwidth;
   src_y /= src_blockheight;

   dst += dst_x * dst_blocksize;
   src += src_x * src_blocksize;
   dst += dst_y * dst_stride;
   src += src_y * src_stride_pos;

   if (dst_format == PIPE_FORMAT_S8_UINT) {
      if (src_format == PIPE_FORMAT_Z32_FLOAT_S8X24_UINT) {
         util_format_z32_float_s8x24_uint_unpack_s_8uint(dst, dst_stride,
                                                         src, src_stride,
                                                         width, height);
      } else if (src_format == PIPE_FORMAT_Z24_UNORM_S8_UINT) {
         util_format_z24_unorm_s8_uint_unpack_s_8uint(dst, dst_stride,
                                                      src, src_stride,
                                                      width, height);
      } else {
      }
   } else if (dst_format == PIPE_FORMAT_Z24X8_UNORM) {
      util_format_z24_unorm_s8_uint_unpack_z24(dst, dst_stride,
                                               src, src_stride,
                                               width, height);
   } else if (dst_format == PIPE_FORMAT_Z32_FLOAT) {
      if (src_format == PIPE_FORMAT_Z32_FLOAT_S8X24_UINT) {
         util_format_z32_float_s8x24_uint_unpack_z_float((float *)dst, dst_stride,
                                                         src, src_stride,
                                                         width, height);
      }
   } else if (dst_format == PIPE_FORMAT_Z32_FLOAT_S8X24_UINT) {
      if (src_format == PIPE_FORMAT_Z32_FLOAT)
         util_format_z32_float_s8x24_uint_pack_z_float(dst, dst_stride,
                                                       (float *)src, src_stride,
                                                       width, height);
      else if (src_format == PIPE_FORMAT_S8_UINT)
         util_format_z32_float_s8x24_uint_pack_s_8uint(dst, dst_stride,
                                                       src, src_stride,
                                                       width, height);
   } else if (dst_format == PIPE_FORMAT_Z24_UNORM_S8_UINT) {
      if (src_format == PIPE_FORMAT_S8_UINT)
         util_format_z24_unorm_s8_uint_pack_s_8uint(dst, dst_stride,
                                                    src, src_stride,
                                                    width, height);
      if (src_format == PIPE_FORMAT_Z24X8_UNORM)
         util_format_z24_unorm_s8_uint_pack_z24(dst, dst_stride,
                                                src, src_stride,
                                                width, height);
   }
}

static void
copy_depth_box(ubyte *dst,
               enum pipe_format dst_format,
               unsigned dst_stride, unsigned dst_slice_stride,
               unsigned dst_x, unsigned dst_y, unsigned dst_z,
               unsigned width, unsigned height, unsigned depth,
               const ubyte * src,
               enum pipe_format src_format,
               int src_stride, unsigned src_slice_stride,
               unsigned src_x, unsigned src_y, unsigned src_z)
{
   unsigned z;
   dst += dst_z * dst_slice_stride;
   src += src_z * src_slice_stride;
   for (z = 0; z < depth; ++z) {
      copy_depth_rect(dst,
                      dst_format,
                      dst_stride,
                      dst_x, dst_y,
                      width, height,
                      src,
                      src_format,
                      src_stride,
                      src_x, src_y);

      dst += dst_slice_stride;
      src += src_slice_stride;
   }
}

static void handle_copy_image_to_buffer(struct lvp_cmd_buffer_entry *cmd,
                                        struct rendering_state *state)
{
   int i;
   struct lvp_cmd_copy_image_to_buffer *copycmd = &cmd->u.img_to_buffer;
   struct pipe_box box, dbox;
   struct pipe_transfer *src_t, *dst_t;
   ubyte *src_data, *dst_data;

   state->pctx->flush(state->pctx, NULL, 0);

   for (i = 0; i < copycmd->region_count; i++) {

      box.x = copycmd->regions[i].imageOffset.x;
      box.y = copycmd->regions[i].imageOffset.y;
      box.z = copycmd->src->type == VK_IMAGE_TYPE_3D ? copycmd->regions[i].imageOffset.z : copycmd->regions[i].imageSubresource.baseArrayLayer;
      box.width = copycmd->regions[i].imageExtent.width;
      box.height = copycmd->regions[i].imageExtent.height;
      box.depth = copycmd->src->type == VK_IMAGE_TYPE_3D ? copycmd->regions[i].imageExtent.depth : copycmd->regions[i].imageSubresource.layerCount;

      src_data = state->pctx->texture_map(state->pctx,
                                           copycmd->src->bo,
                                           copycmd->regions[i].imageSubresource.mipLevel,
                                           PIPE_MAP_READ,
                                           &box,
                                           &src_t);

      dbox.x = copycmd->regions[i].bufferOffset;
      dbox.y = 0;
      dbox.z = 0;
      dbox.width = copycmd->dst->bo->width0;
      dbox.height = 1;
      dbox.depth = 1;
      dst_data = state->pctx->buffer_map(state->pctx,
                                           copycmd->dst->bo,
                                           0,
                                           PIPE_MAP_WRITE,
                                           &dbox,
                                           &dst_t);

      enum pipe_format src_format = copycmd->src->bo->format;
      enum pipe_format dst_format = src_format;
      if (util_format_is_depth_or_stencil(src_format)) {
         if (copycmd->regions[i].imageSubresource.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) {
            dst_format = util_format_get_depth_only(src_format);
         } else if (copycmd->regions[i].imageSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT) {
            dst_format = PIPE_FORMAT_S8_UINT;
         }
      }

      unsigned buffer_row_len = util_format_get_stride(dst_format, copycmd->regions[i].bufferRowLength);
      if (buffer_row_len == 0)
         buffer_row_len = util_format_get_stride(dst_format, copycmd->regions[i].imageExtent.width);
      unsigned buffer_image_height = copycmd->regions[i].bufferImageHeight;
      if (buffer_image_height == 0)
         buffer_image_height = copycmd->regions[i].imageExtent.height;

      unsigned img_stride = util_format_get_2d_size(dst_format, buffer_row_len, buffer_image_height);
      if (src_format != dst_format) {
         copy_depth_box(dst_data, dst_format,
                        buffer_row_len, img_stride,
                        0, 0, 0,
                        copycmd->regions[i].imageExtent.width,
                        copycmd->regions[i].imageExtent.height,
                        box.depth,
                        src_data, src_format, src_t->stride, src_t->layer_stride, 0, 0, 0);
      } else {
         util_copy_box((ubyte *)dst_data, src_format,
                       buffer_row_len, img_stride,
                       0, 0, 0,
                       copycmd->regions[i].imageExtent.width,
                       copycmd->regions[i].imageExtent.height,
                       box.depth,
                       src_data, src_t->stride, src_t->layer_stride, 0, 0, 0);
      }
      state->pctx->texture_unmap(state->pctx, src_t);
      state->pctx->buffer_unmap(state->pctx, dst_t);
   }
}

static void handle_copy_buffer_to_image(struct lvp_cmd_buffer_entry *cmd,
                                        struct rendering_state *state)
{
   int i;
   struct lvp_cmd_copy_buffer_to_image *copycmd = &cmd->u.buffer_to_img;
   struct pipe_box box, sbox;
   struct pipe_transfer *src_t, *dst_t;
   void *src_data, *dst_data;

   state->pctx->flush(state->pctx, NULL, 0);

   for (i = 0; i < copycmd->region_count; i++) {

      sbox.x = copycmd->regions[i].bufferOffset;
      sbox.y = 0;
      sbox.z = 0;
      sbox.width = copycmd->src->bo->width0;
      sbox.height = 1;
      sbox.depth = 1;
      src_data = state->pctx->buffer_map(state->pctx,
                                           copycmd->src->bo,
                                           0,
                                           PIPE_MAP_READ,
                                           &sbox,
                                           &src_t);


      box.x = copycmd->regions[i].imageOffset.x;
      box.y = copycmd->regions[i].imageOffset.y;
      box.z = copycmd->dst->type == VK_IMAGE_TYPE_3D ? copycmd->regions[i].imageOffset.z : copycmd->regions[i].imageSubresource.baseArrayLayer;
      box.width = copycmd->regions[i].imageExtent.width;
      box.height = copycmd->regions[i].imageExtent.height;
      box.depth = copycmd->dst->type == VK_IMAGE_TYPE_3D ? copycmd->regions[i].imageExtent.depth : copycmd->regions[i].imageSubresource.layerCount;

      dst_data = state->pctx->texture_map(state->pctx,
                                           copycmd->dst->bo,
                                           copycmd->regions[i].imageSubresource.mipLevel,
                                           PIPE_MAP_WRITE,
                                           &box,
                                           &dst_t);

      enum pipe_format dst_format = copycmd->dst->bo->format;
      enum pipe_format src_format = dst_format;
      if (util_format_is_depth_or_stencil(dst_format)) {
         if (copycmd->regions[i].imageSubresource.aspectMask == VK_IMAGE_ASPECT_DEPTH_BIT) {
            src_format = util_format_get_depth_only(copycmd->dst->bo->format);
         } else if (copycmd->regions[i].imageSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT) {
            src_format = PIPE_FORMAT_S8_UINT;
         }
      }

      unsigned buffer_row_len = util_format_get_stride(src_format, copycmd->regions[i].bufferRowLength);
      if (buffer_row_len == 0)
         buffer_row_len = util_format_get_stride(src_format, copycmd->regions[i].imageExtent.width);
      unsigned buffer_image_height = copycmd->regions[i].bufferImageHeight;
      if (buffer_image_height == 0)
         buffer_image_height = copycmd->regions[i].imageExtent.height;

      unsigned img_stride = util_format_get_2d_size(src_format, buffer_row_len, buffer_image_height);
      if (src_format != dst_format) {
         copy_depth_box(dst_data, dst_format,
                        dst_t->stride, dst_t->layer_stride,
                        0, 0, 0,
                        copycmd->regions[i].imageExtent.width,
                        copycmd->regions[i].imageExtent.height,
                        box.depth,
                        src_data, src_format,
                        buffer_row_len, img_stride, 0, 0, 0);
      } else {
         util_copy_box(dst_data, dst_format,
                       dst_t->stride, dst_t->layer_stride,
                       0, 0, 0,
                       copycmd->regions[i].imageExtent.width,
                       copycmd->regions[i].imageExtent.height,
                       box.depth,
                       src_data,
                       buffer_row_len, img_stride, 0, 0, 0);
      }
      state->pctx->buffer_unmap(state->pctx, src_t);
      state->pctx->texture_unmap(state->pctx, dst_t);
   }
}

static void handle_copy_image(struct lvp_cmd_buffer_entry *cmd,
                              struct rendering_state *state)
{
   int i;
   struct lvp_cmd_copy_image *copycmd = &cmd->u.copy_image;

   state->pctx->flush(state->pctx, NULL, 0);

   for (i = 0; i < copycmd->region_count; i++) {
      struct pipe_box src_box;
      src_box.x = copycmd->regions[i].srcOffset.x;
      src_box.y = copycmd->regions[i].srcOffset.y;
      src_box.width = copycmd->regions[i].extent.width;
      src_box.height = copycmd->regions[i].extent.height;
      if (copycmd->src->bo->target == PIPE_TEXTURE_3D) {
         src_box.depth = copycmd->regions[i].extent.depth;
         src_box.z = copycmd->regions[i].srcOffset.z;
      } else {
         src_box.depth = copycmd->regions[i].srcSubresource.layerCount;
         src_box.z = copycmd->regions[i].srcSubresource.baseArrayLayer;
      }

      unsigned dstz = copycmd->dst->bo->target == PIPE_TEXTURE_3D ?
                      copycmd->regions[i].dstOffset.z :
                      copycmd->regions[i].dstSubresource.baseArrayLayer;
      state->pctx->resource_copy_region(state->pctx, copycmd->dst->bo,
                                        copycmd->regions[i].dstSubresource.mipLevel,
                                        copycmd->regions[i].dstOffset.x,
                                        copycmd->regions[i].dstOffset.y,
                                        dstz,
                                        copycmd->src->bo,
                                        copycmd->regions[i].srcSubresource.mipLevel,
                                        &src_box);
   }
}

static void handle_copy_buffer(struct lvp_cmd_buffer_entry *cmd,
                               struct rendering_state *state)
{
   int i;
   struct lvp_cmd_copy_buffer *copycmd = &cmd->u.copy_buffer;

   for (i = 0; i < copycmd->region_count; i++) {
      struct pipe_box box = { 0 };
      u_box_1d(copycmd->regions[i].srcOffset, copycmd->regions[i].size, &box);
      state->pctx->resource_copy_region(state->pctx, copycmd->dst->bo, 0,
                                        copycmd->regions[i].dstOffset, 0, 0,
                                        copycmd->src->bo, 0, &box);
   }
}

static void handle_blit_image(struct lvp_cmd_buffer_entry *cmd,
                              struct rendering_state *state)
{
   int i;
   struct lvp_cmd_blit_image *blitcmd = &cmd->u.blit_image;
   struct pipe_blit_info info;

   memset(&info, 0, sizeof(info));

   state->pctx->flush(state->pctx, NULL, 0);
   info.src.resource = blitcmd->src->bo;
   info.dst.resource = blitcmd->dst->bo;
   info.src.format = blitcmd->src->bo->format;
   info.dst.format = blitcmd->dst->bo->format;
   info.mask = util_format_is_depth_or_stencil(info.src.format) ? PIPE_MASK_ZS : PIPE_MASK_RGBA;
   info.filter = blitcmd->filter == VK_FILTER_NEAREST ? PIPE_TEX_FILTER_NEAREST : PIPE_TEX_FILTER_LINEAR;
   for (i = 0; i < blitcmd->region_count; i++) {
      int srcX0, srcX1, srcY0, srcY1, srcZ0, srcZ1;
      unsigned dstX0, dstX1, dstY0, dstY1, dstZ0, dstZ1;

      srcX0 = blitcmd->regions[i].srcOffsets[0].x;
      srcX1 = blitcmd->regions[i].srcOffsets[1].x;
      srcY0 = blitcmd->regions[i].srcOffsets[0].y;
      srcY1 = blitcmd->regions[i].srcOffsets[1].y;
      srcZ0 = blitcmd->regions[i].srcOffsets[0].z;
      srcZ1 = blitcmd->regions[i].srcOffsets[1].z;

      dstX0 = blitcmd->regions[i].dstOffsets[0].x;
      dstX1 = blitcmd->regions[i].dstOffsets[1].x;
      dstY0 = blitcmd->regions[i].dstOffsets[0].y;
      dstY1 = blitcmd->regions[i].dstOffsets[1].y;
      dstZ0 = blitcmd->regions[i].dstOffsets[0].z;
      dstZ1 = blitcmd->regions[i].dstOffsets[1].z;

      if (dstX0 < dstX1) {
         info.dst.box.x = dstX0;
         info.src.box.x = srcX0;
         info.dst.box.width = dstX1 - dstX0;
         info.src.box.width = srcX1 - srcX0;
      } else {
         info.dst.box.x = dstX1;
         info.src.box.x = srcX1;
         info.dst.box.width = dstX0 - dstX1;
         info.src.box.width = srcX0 - srcX1;
      }

      if (dstY0 < dstY1) {
         info.dst.box.y = dstY0;
         info.src.box.y = srcY0;
         info.dst.box.height = dstY1 - dstY0;
         info.src.box.height = srcY1 - srcY0;
      } else {
         info.dst.box.y = dstY1;
         info.src.box.y = srcY1;
         info.dst.box.height = dstY0 - dstY1;
         info.src.box.height = srcY0 - srcY1;
      }

      assert_subresource_layers(info.src.resource, &blitcmd->regions[i].srcSubresource, blitcmd->regions[i].srcOffsets);
      assert_subresource_layers(info.dst.resource, &blitcmd->regions[i].dstSubresource, blitcmd->regions[i].dstOffsets);
      if (blitcmd->src->bo->target == PIPE_TEXTURE_3D) {
         if (dstZ0 < dstZ1) {
            info.dst.box.z = dstZ0;
            info.src.box.z = srcZ0;
            info.dst.box.depth = dstZ1 - dstZ0;
            info.src.box.depth = srcZ1 - srcZ0;
         } else {
            info.dst.box.z = dstZ1;
            info.src.box.z = srcZ1;
            info.dst.box.depth = dstZ0 - dstZ1;
            info.src.box.depth = srcZ0 - srcZ1;
         }
      } else {
         info.src.box.z = blitcmd->regions[i].srcSubresource.baseArrayLayer;
         info.dst.box.z = blitcmd->regions[i].dstSubresource.baseArrayLayer;
         info.src.box.depth = blitcmd->regions[i].srcSubresource.layerCount;
         info.dst.box.depth = blitcmd->regions[i].dstSubresource.layerCount;
      }

      info.src.level = blitcmd->regions[i].srcSubresource.mipLevel;
      info.dst.level = blitcmd->regions[i].dstSubresource.mipLevel;
      state->pctx->blit(state->pctx, &info);
   }
}

static void handle_fill_buffer(struct lvp_cmd_buffer_entry *cmd,
                               struct rendering_state *state)
{
   struct lvp_cmd_fill_buffer *fillcmd = &cmd->u.fill_buffer;
   uint32_t size = fillcmd->fill_size;

   if (fillcmd->fill_size == VK_WHOLE_SIZE) {
      size = fillcmd->buffer->bo->width0 - fillcmd->offset;
      size = ROUND_DOWN_TO(size, 4);
   }

   state->pctx->clear_buffer(state->pctx,
                             fillcmd->buffer->bo,
                             fillcmd->offset,
                             size,
                             &fillcmd->data,
                             4);
}

static void handle_update_buffer(struct lvp_cmd_buffer_entry *cmd,
                                 struct rendering_state *state)
{
   struct lvp_cmd_update_buffer *updcmd = &cmd->u.update_buffer;
   uint32_t *dst;
   struct pipe_transfer *dst_t;
   struct pipe_box box;

   u_box_1d(updcmd->offset, updcmd->data_size, &box);
   dst = state->pctx->buffer_map(state->pctx,
                                   updcmd->buffer->bo,
                                   0,
                                   PIPE_MAP_WRITE,
                                   &box,
                                   &dst_t);

   memcpy(dst, updcmd->data, updcmd->data_size);
   state->pctx->buffer_unmap(state->pctx, dst_t);
}

static void handle_draw_indexed(struct lvp_cmd_buffer_entry *cmd,
                                struct rendering_state *state)
{
   const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];
   state->info.index_bounds_valid = false;
   state->info.min_index = 0;
   state->info.max_index = ~0;
   state->info.index_size = state->index_size;
   state->info.index.resource = state->index_buffer;
   state->info.start_instance = cmd->u.draw_indexed.first_instance;
   state->info.instance_count = cmd->u.draw_indexed.instance_count;
   state->info.view_mask = subpass->view_mask;
   state->info.increment_draw_id = true;

   if (state->info.primitive_restart)
      state->info.restart_index = util_prim_restart_index_from_size(state->info.index_size);
   /* avoid calculating multiple times if cmdbuf is submitted again */
   if (cmd->u.draw_indexed.calc_start) {
      for (unsigned i = 0; i < cmd->u.draw_indexed.draw_count; i++)
         cmd->u.draw_indexed.draws[i].start = (state->index_offset / state->index_size) + cmd->u.draw_indexed.draws[i].start;
      cmd->u.draw_indexed.calc_start = false;
   }
   state->info.index_bias_varies = cmd->u.draw_indexed.vertex_offset_changes;
   state->pctx->draw_vbo(state->pctx, &state->info, 0, NULL, cmd->u.draw_indexed.draws, cmd->u.draw_indexed.draw_count);
}

static void handle_draw_indirect(struct lvp_cmd_buffer_entry *cmd,
                                 struct rendering_state *state, bool indexed)
{
   const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];
   struct pipe_draw_start_count_bias draw = {0};
   if (indexed) {
      state->info.index_bounds_valid = false;
      state->info.index_size = state->index_size;
      state->info.index.resource = state->index_buffer;
      state->info.max_index = ~0;
   } else
      state->info.index_size = 0;
   state->indirect_info.offset = cmd->u.draw_indirect.offset;
   state->indirect_info.stride = cmd->u.draw_indirect.stride;
   state->indirect_info.draw_count = cmd->u.draw_indirect.draw_count;
   state->indirect_info.buffer = cmd->u.draw_indirect.buffer->bo;
   state->info.view_mask = subpass->view_mask;

   state->pctx->draw_vbo(state->pctx, &state->info, 0, &state->indirect_info, &draw, 1);
}

static void handle_index_buffer(struct lvp_cmd_buffer_entry *cmd,
                                struct rendering_state *state)
{
   struct lvp_cmd_bind_index_buffer *ib = &cmd->u.index_buffer;
   switch (ib->index_type) {
   case VK_INDEX_TYPE_UINT8_EXT:
      state->index_size = 1;
      break;
   case VK_INDEX_TYPE_UINT16:
      state->index_size = 2;
      break;
   case VK_INDEX_TYPE_UINT32:
      state->index_size = 4;
      break;
   default:
      break;
   }
   state->index_offset = ib->offset;
   if (ib->buffer)
      state->index_buffer = ib->buffer->bo;
   else
      state->index_buffer = NULL;

   state->ib_dirty = true;
}

static void handle_dispatch(struct lvp_cmd_buffer_entry *cmd,
                            struct rendering_state *state)
{
   state->dispatch_info.grid[0] = cmd->u.dispatch.x;
   state->dispatch_info.grid[1] = cmd->u.dispatch.y;
   state->dispatch_info.grid[2] = cmd->u.dispatch.z;
   state->dispatch_info.grid_base[0] = cmd->u.dispatch.base_x;
   state->dispatch_info.grid_base[1] = cmd->u.dispatch.base_y;
   state->dispatch_info.grid_base[2] = cmd->u.dispatch.base_z;
   state->dispatch_info.indirect = NULL;
   state->pctx->launch_grid(state->pctx, &state->dispatch_info);
}

static void handle_dispatch_indirect(struct lvp_cmd_buffer_entry *cmd,
                                     struct rendering_state *state)
{
   state->dispatch_info.indirect = cmd->u.dispatch_indirect.buffer->bo;
   state->dispatch_info.indirect_offset = cmd->u.dispatch_indirect.offset;
   state->pctx->launch_grid(state->pctx, &state->dispatch_info);
}

static void handle_push_constants(struct lvp_cmd_buffer_entry *cmd,
                                  struct rendering_state *state)
{
   memcpy(state->push_constants + cmd->u.push_constants.offset, cmd->u.push_constants.val, cmd->u.push_constants.size);

   state->pc_buffer[PIPE_SHADER_VERTEX].buffer_size = 128 * 4;
   state->pc_buffer[PIPE_SHADER_VERTEX].buffer_offset = 0;
   state->pc_buffer[PIPE_SHADER_VERTEX].user_buffer = state->push_constants;
   state->pcbuf_dirty[PIPE_SHADER_VERTEX] = true;
   state->pc_buffer[PIPE_SHADER_FRAGMENT].buffer_size = 128 * 4;
   state->pc_buffer[PIPE_SHADER_FRAGMENT].buffer_offset = 0;
   state->pc_buffer[PIPE_SHADER_FRAGMENT].user_buffer = state->push_constants;
   state->pcbuf_dirty[PIPE_SHADER_FRAGMENT] = true;
   state->pc_buffer[PIPE_SHADER_GEOMETRY].buffer_size = 128 * 4;
   state->pc_buffer[PIPE_SHADER_GEOMETRY].buffer_offset = 0;
   state->pc_buffer[PIPE_SHADER_GEOMETRY].user_buffer = state->push_constants;
   state->pcbuf_dirty[PIPE_SHADER_GEOMETRY] = true;
   state->pc_buffer[PIPE_SHADER_TESS_CTRL].buffer_size = 128 * 4;
   state->pc_buffer[PIPE_SHADER_TESS_CTRL].buffer_offset = 0;
   state->pc_buffer[PIPE_SHADER_TESS_CTRL].user_buffer = state->push_constants;
   state->pcbuf_dirty[PIPE_SHADER_TESS_CTRL] = true;
   state->pc_buffer[PIPE_SHADER_TESS_EVAL].buffer_size = 128 * 4;
   state->pc_buffer[PIPE_SHADER_TESS_EVAL].buffer_offset = 0;
   state->pc_buffer[PIPE_SHADER_TESS_EVAL].user_buffer = state->push_constants;
   state->pcbuf_dirty[PIPE_SHADER_TESS_EVAL] = true;
   state->pc_buffer[PIPE_SHADER_COMPUTE].buffer_size = 128 * 4;
   state->pc_buffer[PIPE_SHADER_COMPUTE].buffer_offset = 0;
   state->pc_buffer[PIPE_SHADER_COMPUTE].user_buffer = state->push_constants;
   state->pcbuf_dirty[PIPE_SHADER_COMPUTE] = true;
}

static void lvp_execute_cmd_buffer(struct lvp_cmd_buffer *cmd_buffer,
                                   struct rendering_state *state);

static void handle_execute_commands(struct lvp_cmd_buffer_entry *cmd,
                                    struct rendering_state *state)
{
   for (unsigned i = 0; i < cmd->u.execute_commands.command_buffer_count; i++) {
      struct lvp_cmd_buffer *secondary_buf = cmd->u.execute_commands.cmd_buffers[i];
      lvp_execute_cmd_buffer(secondary_buf, state);
   }
}

static void handle_event_set(struct lvp_cmd_buffer_entry *cmd,
                             struct rendering_state *state)
{
   struct lvp_event *event = cmd->u.event_set.event;

   if (cmd->u.event_set.flush)
      state->pctx->flush(state->pctx, NULL, 0);
   event->event_storage = (cmd->u.event_set.value == true) ? 1 : 0;
}

static void handle_wait_events(struct lvp_cmd_buffer_entry *cmd,
                               struct rendering_state *state)
{
   for (unsigned i = 0; i < cmd->u.wait_events.event_count; i++) {
      struct lvp_event *event = cmd->u.wait_events.events[i];

      while (event->event_storage != true);
   }
}

static void handle_pipeline_barrier(struct lvp_cmd_buffer_entry *cmd,
                                    struct rendering_state *state)
{
   /* why hello nail, I'm a hammer. - TODO */
   state->pctx->flush(state->pctx, NULL, 0);
}

static void maybe_emit_state_for_begin_query(struct lvp_cmd_buffer_entry *cmd,
                                             struct rendering_state *state)
{
   struct lvp_cmd_query_cmd *qcmd = &cmd->u.query;
   struct lvp_query_pool *pool = qcmd->pool;

   if (pool->type == VK_QUERY_TYPE_PIPELINE_STATISTICS &&
       pool->pipeline_stats & VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT)
      emit_compute_state(state);
   emit_state(state);
}

static void handle_begin_query(struct lvp_cmd_buffer_entry *cmd,
                               struct rendering_state *state)
{
   struct lvp_cmd_query_cmd *qcmd = &cmd->u.query;
   struct lvp_query_pool *pool = qcmd->pool;

   if (!pool->queries[qcmd->query]) {
      enum pipe_query_type qtype = pool->base_type;
      if (qtype == PIPE_QUERY_OCCLUSION_COUNTER && !qcmd->precise)
         qtype = PIPE_QUERY_OCCLUSION_PREDICATE;
      pool->queries[qcmd->query] = state->pctx->create_query(state->pctx,
                                                             qtype, qcmd->index);
   }

   state->pctx->begin_query(state->pctx, pool->queries[qcmd->query]);
}

static void handle_end_query(struct lvp_cmd_buffer_entry *cmd,
                             struct rendering_state *state)
{
   struct lvp_cmd_query_cmd *qcmd = &cmd->u.query;
   struct lvp_query_pool *pool = qcmd->pool;
   assert(pool->queries[qcmd->query]);

   state->pctx->end_query(state->pctx, pool->queries[qcmd->query]);
}

static void handle_reset_query_pool(struct lvp_cmd_buffer_entry *cmd,
                                    struct rendering_state *state)
{
   struct lvp_cmd_query_cmd *qcmd = &cmd->u.query;
   struct lvp_query_pool *pool = qcmd->pool;
   for (unsigned i = qcmd->query; i < qcmd->query + qcmd->index; i++) {
      if (pool->queries[i]) {
         state->pctx->destroy_query(state->pctx, pool->queries[i]);
         pool->queries[i] = NULL;
      }
   }
}

static void handle_write_timestamp(struct lvp_cmd_buffer_entry *cmd,
                                   struct rendering_state *state)
{
   struct lvp_cmd_query_cmd *qcmd = &cmd->u.query;
   struct lvp_query_pool *pool = qcmd->pool;
   if (!pool->queries[qcmd->query]) {
      pool->queries[qcmd->query] = state->pctx->create_query(state->pctx,
                                                             PIPE_QUERY_TIMESTAMP, 0);
   }

   if (qcmd->flush)
      state->pctx->flush(state->pctx, NULL, 0);
   state->pctx->end_query(state->pctx, pool->queries[qcmd->query]);

}

static void handle_copy_query_pool_results(struct lvp_cmd_buffer_entry *cmd,
                                           struct rendering_state *state)
{
   struct lvp_cmd_copy_query_pool_results *copycmd = &cmd->u.copy_query_pool_results;
   struct lvp_query_pool *pool = copycmd->pool;

   for (unsigned i = copycmd->first_query; i < copycmd->first_query + copycmd->query_count; i++) {
      unsigned offset = copycmd->dst_offset + copycmd->dst->offset + (copycmd->stride * (i - copycmd->first_query));
      if (pool->queries[i]) {
         if (copycmd->flags & VK_QUERY_RESULT_WITH_AVAILABILITY_BIT)
            state->pctx->get_query_result_resource(state->pctx,
                                                   pool->queries[i],
                                                   copycmd->flags & VK_QUERY_RESULT_WAIT_BIT,
                                                   copycmd->flags & VK_QUERY_RESULT_64_BIT ? PIPE_QUERY_TYPE_U64 : PIPE_QUERY_TYPE_U32,
                                                   -1,
                                                   copycmd->dst->bo,
                                                   offset + (copycmd->flags & VK_QUERY_RESULT_64_BIT ? 8 : 4));
         if (pool->type == VK_QUERY_TYPE_PIPELINE_STATISTICS) {
            unsigned num_results = 0;
            unsigned result_size = copycmd->flags & VK_QUERY_RESULT_64_BIT ? 8 : 4;
            u_foreach_bit(bit, pool->pipeline_stats)
               state->pctx->get_query_result_resource(state->pctx,
                                                      pool->queries[i],
                                                      copycmd->flags & VK_QUERY_RESULT_WAIT_BIT,
                                                      copycmd->flags & VK_QUERY_RESULT_64_BIT ? PIPE_QUERY_TYPE_U64 : PIPE_QUERY_TYPE_U32,
                                                      bit,
                                                      copycmd->dst->bo,
                                                      offset + num_results++ * result_size);
         } else {
            state->pctx->get_query_result_resource(state->pctx,
                                                   pool->queries[i],
                                                   copycmd->flags & VK_QUERY_RESULT_WAIT_BIT,
                                                   copycmd->flags & VK_QUERY_RESULT_64_BIT ? PIPE_QUERY_TYPE_U64 : PIPE_QUERY_TYPE_U32,
                                                   0,
                                                   copycmd->dst->bo,
                                                   offset);
         }
      } else {
         /* if no queries emitted yet, just reset the buffer to 0 so avail is reported correctly */
         if (copycmd->flags & VK_QUERY_RESULT_WITH_AVAILABILITY_BIT) {
            struct pipe_transfer *src_t;
            uint32_t *map;

            struct pipe_box box = {0};
            box.x = offset;
            box.width = copycmd->stride;
            box.height = 1;
            box.depth = 1;
            map = state->pctx->buffer_map(state->pctx,
                                            copycmd->dst->bo, 0, PIPE_MAP_READ, &box,
                                            &src_t);

            memset(map, 0, box.width);
            state->pctx->buffer_unmap(state->pctx, src_t);
         }
      }
   }
}

static void pack_clear_color(enum pipe_format pformat, VkClearColorValue *in_val, uint32_t col_val[4])
{
   const struct util_format_description *desc = util_format_description(pformat);
   col_val[0] = col_val[1] = col_val[2] = col_val[3] = 0;
   for (unsigned c = 0; c < 4; c++) {
      if (desc->swizzle[c] >= 4)
         continue;
      const struct util_format_channel_description *channel = &desc->channel[desc->swizzle[c]];
      if (channel->size == 32) {
         col_val[c] = in_val->uint32[c];
         continue;
      }
      if (channel->pure_integer) {
         uint64_t v = in_val->uint32[c] & ((1u << channel->size) - 1);
         switch (channel->size) {
         case 2:
         case 8:
         case 10:
            col_val[0] |= (v << channel->shift);
            break;
         case 16:
            col_val[c / 2] |= (v << (16 * (c % 2)));
            break;
         }
      } else {
         util_pack_color(in_val->float32, pformat, (union util_color *)col_val);
         break;
      }
   }
}

static void handle_clear_color_image(struct lvp_cmd_buffer_entry *cmd,
                                     struct rendering_state *state)
{
   struct lvp_image *image = cmd->u.clear_color_image.image;
   uint32_t col_val[4];
   pack_clear_color(image->bo->format, &cmd->u.clear_color_image.clear_val, col_val);
   for (unsigned i = 0; i < cmd->u.clear_color_image.range_count; i++) {
      VkImageSubresourceRange *range = &cmd->u.clear_color_image.ranges[i];
      struct pipe_box box;
      box.x = 0;
      box.y = 0;
      box.z = 0;

      uint32_t level_count = lvp_get_levelCount(image, range);
      for (unsigned j = range->baseMipLevel; j < range->baseMipLevel + level_count; j++) {
         box.width = u_minify(image->bo->width0, j);
         box.height = u_minify(image->bo->height0, j);
         box.depth = 1;
         if (image->bo->target == PIPE_TEXTURE_3D)
            box.depth = u_minify(image->bo->depth0, j);
         else if (image->bo->target == PIPE_TEXTURE_1D_ARRAY) {
            box.y = range->baseArrayLayer;
            box.height = lvp_get_layerCount(image, range);
            box.depth = 1;
         } else {
            box.z = range->baseArrayLayer;
            box.depth = lvp_get_layerCount(image, range);
         }

         state->pctx->clear_texture(state->pctx, image->bo,
                                    j, &box, (void *)col_val);
      }
   }
}

static void handle_clear_ds_image(struct lvp_cmd_buffer_entry *cmd,
                                  struct rendering_state *state)
{
   struct lvp_image *image = cmd->u.clear_ds_image.image;
   for (unsigned i = 0; i < cmd->u.clear_ds_image.range_count; i++) {
      VkImageSubresourceRange *range = &cmd->u.clear_ds_image.ranges[i];
      uint32_t ds_clear_flags = 0;
      if (range->aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT)
         ds_clear_flags |= PIPE_CLEAR_DEPTH;
      if (range->aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT)
         ds_clear_flags |= PIPE_CLEAR_STENCIL;

      uint32_t level_count = lvp_get_levelCount(image, range);
      for (unsigned j = 0; j < level_count; j++) {
         struct pipe_surface *surf;
         unsigned width, height;

         width = u_minify(image->bo->width0, range->baseMipLevel + j);
         height = u_minify(image->bo->height0, range->baseMipLevel + j);

         surf = create_img_surface_bo(state, range,
                                      image->bo, image->bo->format,
                                      width, height,
                                      0, lvp_get_layerCount(image, range) - 1, j);

         state->pctx->clear_depth_stencil(state->pctx,
                                          surf,
                                          ds_clear_flags,
                                          cmd->u.clear_ds_image.clear_val.depth,
                                          cmd->u.clear_ds_image.clear_val.stencil,
                                          0, 0,
                                          width, height, true);
         state->pctx->surface_destroy(state->pctx, surf);
      }
   }
}

static void handle_clear_attachments(struct lvp_cmd_buffer_entry *cmd,
                                     struct rendering_state *state)
{
   for (uint32_t a = 0; a < cmd->u.clear_attachments.attachment_count; a++) {
      VkClearAttachment *att = &cmd->u.clear_attachments.attachments[a];
      const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];
      struct lvp_image_view *imgv;

      if (att->aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) {
         struct lvp_subpass_attachment *color_att = &subpass->color_attachments[att->colorAttachment];
         if (!color_att || color_att->attachment == VK_ATTACHMENT_UNUSED)
            continue;
         imgv = get_attachment(state, color_att->attachment);
      } else {
         struct lvp_subpass_attachment *ds_att = subpass->depth_stencil_attachment;
         if (!ds_att || ds_att->attachment == VK_ATTACHMENT_UNUSED)
            continue;
         imgv = get_attachment(state, ds_att->attachment);
      }
      union pipe_color_union col_val;
      double dclear_val = 0;
      uint32_t sclear_val = 0;
      uint32_t ds_clear_flags = 0;
      if (att->aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT) {
         ds_clear_flags |= PIPE_CLEAR_DEPTH;
         dclear_val = att->clearValue.depthStencil.depth;
      }
      if (att->aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT) {
         ds_clear_flags |= PIPE_CLEAR_STENCIL;
         sclear_val = att->clearValue.depthStencil.stencil;
      }
      if (att->aspectMask & VK_IMAGE_ASPECT_COLOR_BIT) {
         for (unsigned i = 0; i < 4; i++)
            col_val.ui[i] = att->clearValue.color.uint32[i];
      }

      for (uint32_t r = 0; r < cmd->u.clear_attachments.rect_count; r++) {

         VkClearRect *rect = &cmd->u.clear_attachments.rects[r];
         if (subpass->view_mask) {
            u_foreach_bit(i, subpass->view_mask)
               clear_attachment_layers(state, imgv, &rect->rect,
                                       i, 1,
                                       ds_clear_flags, dclear_val, sclear_val,
                                       &col_val);
         } else
            clear_attachment_layers(state, imgv, &rect->rect,
                                    rect->baseArrayLayer, rect->layerCount,
                                    ds_clear_flags, dclear_val, sclear_val,
                                    &col_val);
      }
   }
}

static void handle_resolve_image(struct lvp_cmd_buffer_entry *cmd,
                                 struct rendering_state *state)
{
   int i;
   struct lvp_cmd_resolve_image *resolvecmd = &cmd->u.resolve_image;
   struct pipe_blit_info info;

   memset(&info, 0, sizeof(info));

   state->pctx->flush(state->pctx, NULL, 0);
   info.src.resource = resolvecmd->src->bo;
   info.dst.resource = resolvecmd->dst->bo;
   info.src.format = resolvecmd->src->bo->format;
   info.dst.format = resolvecmd->dst->bo->format;
   info.mask = util_format_is_depth_or_stencil(info.src.format) ? PIPE_MASK_ZS : PIPE_MASK_RGBA;
   info.filter = PIPE_TEX_FILTER_NEAREST;
   for (i = 0; i < resolvecmd->region_count; i++) {
      int srcX0, srcY0;
      unsigned dstX0, dstY0;

      srcX0 = resolvecmd->regions[i].srcOffset.x;
      srcY0 = resolvecmd->regions[i].srcOffset.y;

      dstX0 = resolvecmd->regions[i].dstOffset.x;
      dstY0 = resolvecmd->regions[i].dstOffset.y;

      info.dst.box.x = dstX0;
      info.dst.box.y = dstY0;
      info.src.box.x = srcX0;
      info.src.box.y = srcY0;

      info.dst.box.width = resolvecmd->regions[i].extent.width;
      info.src.box.width = resolvecmd->regions[i].extent.width;
      info.dst.box.height = resolvecmd->regions[i].extent.height;
      info.src.box.height = resolvecmd->regions[i].extent.height;

      info.dst.box.depth = resolvecmd->regions[i].dstSubresource.layerCount;
      info.src.box.depth = resolvecmd->regions[i].srcSubresource.layerCount;

      info.src.level = resolvecmd->regions[i].srcSubresource.mipLevel;
      info.src.box.z = resolvecmd->regions[i].srcOffset.z + resolvecmd->regions[i].srcSubresource.baseArrayLayer;

      info.dst.level = resolvecmd->regions[i].dstSubresource.mipLevel;
      info.dst.box.z = resolvecmd->regions[i].dstOffset.z + resolvecmd->regions[i].dstSubresource.baseArrayLayer;

      state->pctx->blit(state->pctx, &info);
   }
}

static void handle_draw_indirect_count(struct lvp_cmd_buffer_entry *cmd,
                                       struct rendering_state *state, bool indexed)
{
   const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];
   struct pipe_draw_start_count_bias draw = {0};
   if (indexed) {
      state->info.index_bounds_valid = false;
      state->info.index_size = state->index_size;
      state->info.index.resource = state->index_buffer;
      state->info.max_index = ~0;
   } else
      state->info.index_size = 0;
   state->indirect_info.offset = cmd->u.draw_indirect_count.offset;
   state->indirect_info.stride = cmd->u.draw_indirect_count.stride;
   state->indirect_info.draw_count = cmd->u.draw_indirect_count.max_draw_count;
   state->indirect_info.buffer = cmd->u.draw_indirect_count.buffer->bo;
   state->indirect_info.indirect_draw_count_offset = cmd->u.draw_indirect_count.count_buffer_offset;
   state->indirect_info.indirect_draw_count = cmd->u.draw_indirect_count.count_buffer->bo;
   state->info.view_mask = subpass->view_mask;

   state->pctx->draw_vbo(state->pctx, &state->info, 0, &state->indirect_info, &draw, 1);
}

static void handle_compute_push_descriptor_set(struct lvp_cmd_buffer_entry *cmd,
                                               struct dyn_info *dyn_info,
                                               struct rendering_state *state)
{
   struct lvp_cmd_push_descriptor_set *pds = &cmd->u.push_descriptor_set;
   struct lvp_descriptor_set_layout *layout = pds->layout->set[pds->set].layout;

   if (!(layout->shader_stages & VK_SHADER_STAGE_COMPUTE_BIT))
      return;
   for (unsigned i = 0; i < pds->set; i++) {
      increment_dyn_info(dyn_info, pds->layout->set[i].layout, false);
   }
   unsigned info_idx = 0;
   for (unsigned i = 0; i < pds->descriptor_write_count; i++) {
      struct lvp_write_descriptor *desc = &pds->descriptors[i];
      struct lvp_descriptor_set_binding_layout *binding = &layout->binding[desc->dst_binding];

      if (!binding->valid)
         continue;

      for (unsigned j = 0; j < desc->descriptor_count; j++) {
         union lvp_descriptor_info *info = &pds->infos[info_idx + j];

         handle_descriptor(state, dyn_info, binding,
                           MESA_SHADER_COMPUTE, PIPE_SHADER_COMPUTE,
                           j, desc->descriptor_type,
                           info);
      }
      info_idx += desc->descriptor_count;
   }
}

static void handle_push_descriptor_set(struct lvp_cmd_buffer_entry *cmd,
                                       struct rendering_state *state)
{
   struct lvp_cmd_push_descriptor_set *pds = &cmd->u.push_descriptor_set;
   struct lvp_descriptor_set_layout *layout = pds->layout->set[pds->set].layout;
   struct dyn_info dyn_info;

   memset(&dyn_info.stage, 0, sizeof(dyn_info.stage));
   dyn_info.dyn_index = 0;
   if (pds->bind_point == VK_PIPELINE_BIND_POINT_COMPUTE) {
      handle_compute_push_descriptor_set(cmd, &dyn_info, state);
   }

   for (unsigned i = 0; i < pds->set; i++) {
      increment_dyn_info(&dyn_info, pds->layout->set[i].layout, false);
   }

   unsigned info_idx = 0;
   for (unsigned i = 0; i < pds->descriptor_write_count; i++) {
      struct lvp_write_descriptor *desc = &pds->descriptors[i];
      struct lvp_descriptor_set_binding_layout *binding = &layout->binding[desc->dst_binding];

      if (!binding->valid)
         continue;

      for (unsigned j = 0; j < desc->descriptor_count; j++) {
         union lvp_descriptor_info *info = &pds->infos[info_idx + j];

         if (layout->shader_stages & VK_SHADER_STAGE_VERTEX_BIT)
            handle_descriptor(state, &dyn_info, binding,
                              MESA_SHADER_VERTEX, PIPE_SHADER_VERTEX,
                              j, desc->descriptor_type,
                              info);
         if (layout->shader_stages & VK_SHADER_STAGE_FRAGMENT_BIT)
            handle_descriptor(state, &dyn_info, binding,
                              MESA_SHADER_FRAGMENT, PIPE_SHADER_FRAGMENT,
                              j, desc->descriptor_type,
                              info);
         if (layout->shader_stages & VK_SHADER_STAGE_GEOMETRY_BIT)
            handle_descriptor(state, &dyn_info, binding,
                              MESA_SHADER_GEOMETRY, PIPE_SHADER_GEOMETRY,
                              j, desc->descriptor_type,
                              info);
         if (layout->shader_stages & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
            handle_descriptor(state, &dyn_info, binding,
                              MESA_SHADER_TESS_CTRL, PIPE_SHADER_TESS_CTRL,
                              j, desc->descriptor_type,
                              info);
         if (layout->shader_stages & VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
            handle_descriptor(state, &dyn_info, binding,
                              MESA_SHADER_TESS_EVAL, PIPE_SHADER_TESS_EVAL,
                              j, desc->descriptor_type,
                              info);
      }
      info_idx += desc->descriptor_count;
   }
}

static void handle_bind_transform_feedback_buffers(struct lvp_cmd_buffer_entry *cmd,
                                                   struct rendering_state *state)
{
   struct lvp_cmd_bind_transform_feedback_buffers *btfb = &cmd->u.bind_transform_feedback_buffers;

   for (unsigned i = 0; i < btfb->binding_count; i++) {
      int idx = i + btfb->first_binding;
      if (state->so_targets[idx])
         state->pctx->stream_output_target_destroy(state->pctx, state->so_targets[idx]);

      state->so_targets[idx] = state->pctx->create_stream_output_target(state->pctx,
                                                                        btfb->buffers[i]->bo,
                                                                        btfb->offsets[i],
                                                                        btfb->sizes[i]);
   }
   state->num_so_targets = btfb->first_binding + btfb->binding_count;
}

static void handle_begin_transform_feedback(struct lvp_cmd_buffer_entry *cmd,
                                            struct rendering_state *state)
{
   struct lvp_cmd_begin_transform_feedback *btf = &cmd->u.begin_transform_feedback;
   uint32_t offsets[4];

   memset(offsets, 0, sizeof(uint32_t)*4);

   for (unsigned i = 0; i < btf->counter_buffer_count; i++) {
      if (!btf->counter_buffers[i])
         continue;

      pipe_buffer_read(state->pctx,
                       btf->counter_buffers[i]->bo,
                       btf->counter_buffer_offsets[i],
                       4,
                       &offsets[i]);
   }
   state->pctx->set_stream_output_targets(state->pctx, state->num_so_targets,
                                          state->so_targets, offsets);
}

static void handle_end_transform_feedback(struct lvp_cmd_buffer_entry *cmd,
                                          struct rendering_state *state)
{
   struct lvp_cmd_end_transform_feedback *etf = &cmd->u.end_transform_feedback;

   if (etf->counter_buffer_count) {
      for (unsigned i = 0; i < etf->counter_buffer_count; i++) {
         if (!etf->counter_buffers[i])
            continue;

         uint32_t offset;
         offset = state->pctx->stream_output_target_offset(state->so_targets[i]);

         pipe_buffer_write(state->pctx,
                           etf->counter_buffers[i]->bo,
                           etf->counter_buffer_offsets[i],
                           4,
                           &offset);
      }
   }
   state->pctx->set_stream_output_targets(state->pctx, 0, NULL, NULL);
}

static void handle_draw_indirect_byte_count(struct lvp_cmd_buffer_entry *cmd,
                                            struct rendering_state *state)
{
   struct lvp_cmd_draw_indirect_byte_count *dibc = &cmd->u.draw_indirect_byte_count;
   const struct lvp_subpass *subpass = &state->pass->subpasses[state->subpass];
   struct pipe_draw_start_count_bias draw = {0};

   pipe_buffer_read(state->pctx,
                    dibc->counter_buffer->bo,
                    dibc->counter_buffer->offset + dibc->counter_buffer_offset,
                    4, &draw.count);

   state->info.start_instance = cmd->u.draw_indirect_byte_count.first_instance;
   state->info.instance_count = cmd->u.draw_indirect_byte_count.instance_count;
   state->info.index_size = 0;

   draw.count /= cmd->u.draw_indirect_byte_count.vertex_stride;
   state->info.view_mask = subpass->view_mask;
   state->pctx->draw_vbo(state->pctx, &state->info, 0, &state->indirect_info, &draw, 1);
}

static void handle_begin_conditional_rendering(struct lvp_cmd_buffer_entry *cmd,
                                               struct rendering_state *state)
{
   struct lvp_cmd_begin_conditional_rendering *bcr = &cmd->u.begin_conditional_rendering;
   state->pctx->render_condition_mem(state->pctx,
                                     bcr->buffer->bo,
                                     bcr->buffer->offset + bcr->offset,
                                     bcr->inverted);
}

static void handle_end_conditional_rendering(struct rendering_state *state)
{
   state->pctx->render_condition_mem(state->pctx, NULL, 0, false);
}

static void handle_set_vertex_input(struct lvp_cmd_buffer_entry *cmd,
                                    struct rendering_state *state)
{
   const struct lvp_cmd_set_vertex_input *vertex_input = &cmd->u.set_vertex_input;
   const struct VkVertexInputBindingDescription2EXT *bindings = (void*)vertex_input->data;
   const struct VkVertexInputAttributeDescription2EXT *attrs = (void*)(vertex_input->data +
                                                               vertex_input->binding_count *
                                                               sizeof(struct VkVertexInputBindingDescription2EXT));
   int max_location = -1;
   for (unsigned i = 0; i < vertex_input->attr_count; i++) {
      const struct VkVertexInputBindingDescription2EXT *binding = &bindings[attrs[i].binding];
      unsigned location = attrs[i].location;
      state->velem.velems[location].src_offset = attrs[i].offset;
      state->velem.velems[location].vertex_buffer_index = attrs[i].binding;
      state->velem.velems[location].src_format = lvp_vk_format_to_pipe_format(attrs[i].format);
      state->vb[attrs[i].binding].stride = binding->stride;

      switch (binding->inputRate) {
      case VK_VERTEX_INPUT_RATE_VERTEX:
         state->velem.velems[location].instance_divisor = 0;
         break;
      case VK_VERTEX_INPUT_RATE_INSTANCE:
         state->velem.velems[location].instance_divisor = binding->divisor;
         break;
      default:
         assert(0);
         break;
      }

      if ((int)location > max_location)
         max_location = location;
   }
   state->velem.count = max_location + 1;
   state->vb_dirty = true;
   state->ve_dirty = true;
}

static void handle_set_cull_mode(struct lvp_cmd_buffer_entry *cmd,
                                 struct rendering_state *state)
{
   state->rs_state.cull_face = vk_cull_to_pipe(cmd->u.set_cull_mode.cull_mode);
   state->rs_dirty = true;
}

static void handle_set_front_face(struct lvp_cmd_buffer_entry *cmd,
                                  struct rendering_state *state)
{
   state->rs_state.front_ccw = (cmd->u.set_front_face.front_face == VK_FRONT_FACE_COUNTER_CLOCKWISE);
   state->rs_dirty = true;
}

static void handle_set_primitive_topology(struct lvp_cmd_buffer_entry *cmd,
                                          struct rendering_state *state)
{
   state->info.mode = vk_conv_topology(cmd->u.set_primitive_topology.prim);
   state->rs_dirty = true;
}


static void handle_set_depth_test_enable(struct lvp_cmd_buffer_entry *cmd,
                                         struct rendering_state *state)
{
   state->dsa_dirty |= state->dsa_state.depth_enabled != cmd->u.set_depth_test_enable.depth_test_enable;
   state->dsa_state.depth_enabled = cmd->u.set_depth_test_enable.depth_test_enable;
}

static void handle_set_depth_write_enable(struct lvp_cmd_buffer_entry *cmd,
                                          struct rendering_state *state)
{
   state->dsa_dirty |= state->dsa_state.depth_writemask != cmd->u.set_depth_write_enable.depth_write_enable;
   state->dsa_state.depth_writemask = cmd->u.set_depth_write_enable.depth_write_enable;
}

static void handle_set_depth_compare_op(struct lvp_cmd_buffer_entry *cmd,
                                        struct rendering_state *state)
{
   state->dsa_dirty |= state->dsa_state.depth_func != cmd->u.set_depth_compare_op.depth_op;
   state->dsa_state.depth_func = cmd->u.set_depth_compare_op.depth_op;
}

static void handle_set_depth_bounds_test_enable(struct lvp_cmd_buffer_entry *cmd,
                                                struct rendering_state *state)
{
   state->dsa_dirty |= state->dsa_state.depth_bounds_test != cmd->u.set_depth_bounds_test_enable.depth_bounds_test_enable;
   state->dsa_state.depth_bounds_test = cmd->u.set_depth_bounds_test_enable.depth_bounds_test_enable;
}

static void handle_set_stencil_test_enable(struct lvp_cmd_buffer_entry *cmd,
                                           struct rendering_state *state)
{
   state->dsa_dirty |= state->dsa_state.stencil[0].enabled != cmd->u.set_stencil_test_enable.stencil_test_enable ||
                       state->dsa_state.stencil[1].enabled != cmd->u.set_stencil_test_enable.stencil_test_enable;
   state->dsa_state.stencil[0].enabled = cmd->u.set_stencil_test_enable.stencil_test_enable;
   state->dsa_state.stencil[1].enabled = cmd->u.set_stencil_test_enable.stencil_test_enable;
}

static void handle_set_stencil_op(struct lvp_cmd_buffer_entry *cmd,
                                  struct rendering_state *state)
{
   if (cmd->u.set_stencil_op.face_mask & VK_STENCIL_FACE_FRONT_BIT) {
      state->dsa_state.stencil[0].func = cmd->u.set_stencil_op.compare_op;
      state->dsa_state.stencil[0].fail_op = vk_conv_stencil_op(cmd->u.set_stencil_op.fail_op);
      state->dsa_state.stencil[0].zpass_op = vk_conv_stencil_op(cmd->u.set_stencil_op.pass_op);
      state->dsa_state.stencil[0].zfail_op = vk_conv_stencil_op(cmd->u.set_stencil_op.depth_fail_op);
   }

   if (cmd->u.set_stencil_op.face_mask & VK_STENCIL_FACE_BACK_BIT) {
      state->dsa_state.stencil[1].func = cmd->u.set_stencil_op.compare_op;
      state->dsa_state.stencil[1].fail_op = vk_conv_stencil_op(cmd->u.set_stencil_op.fail_op);
      state->dsa_state.stencil[1].zpass_op = vk_conv_stencil_op(cmd->u.set_stencil_op.pass_op);
      state->dsa_state.stencil[1].zfail_op = vk_conv_stencil_op(cmd->u.set_stencil_op.depth_fail_op);
   }
   state->dsa_dirty = true;
}

static void handle_set_line_stipple(struct lvp_cmd_buffer_entry *cmd,
                                    struct rendering_state *state)
{
   state->rs_state.line_stipple_factor = cmd->u.set_line_stipple.line_stipple_factor - 1;
   state->rs_state.line_stipple_pattern = cmd->u.set_line_stipple.line_stipple_pattern;
   state->rs_dirty = true;
}

static void handle_set_depth_bias_enable(struct lvp_cmd_buffer_entry *cmd,
                                         struct rendering_state *state)
{
   state->rs_dirty |= state->depth_bias.enabled != cmd->u.set_depth_bias_enable.enable;
   state->depth_bias.enabled = cmd->u.set_depth_bias_enable.enable;
}

static void handle_set_logic_op(struct lvp_cmd_buffer_entry *cmd,
                                struct rendering_state *state)
{
   unsigned op = vk_conv_logic_op(cmd->u.set_logic_op.op);
   state->rs_dirty |= state->blend_state.logicop_func != op;
   state->blend_state.logicop_func = op;
}

static void handle_set_patch_control_points(struct lvp_cmd_buffer_entry *cmd,
                                            struct rendering_state *state)
{
   state->info.vertices_per_patch = cmd->u.set_patch_control_points.vertices_per_patch;
}

static void handle_set_primitive_restart_enable(struct lvp_cmd_buffer_entry *cmd,
                                                struct rendering_state *state)
{
   state->info.primitive_restart = cmd->u.set_primitive_restart_enable.enable;
}

static void handle_set_rasterizer_discard_enable(struct lvp_cmd_buffer_entry *cmd,
                                                 struct rendering_state *state)
{
   state->rs_dirty |= state->rs_state.rasterizer_discard != cmd->u.set_rasterizer_discard_enable.enable;
   state->rs_state.rasterizer_discard = cmd->u.set_rasterizer_discard_enable.enable;
}

static void lvp_execute_cmd_buffer(struct lvp_cmd_buffer *cmd_buffer,
                                   struct rendering_state *state)
{
   struct lvp_cmd_buffer_entry *cmd;
   bool first = true;
   bool did_flush = false;

   LIST_FOR_EACH_ENTRY(cmd, &cmd_buffer->cmds, cmd_link) {
      switch (cmd->cmd_type) {
      case LVP_CMD_BIND_PIPELINE:
         handle_pipeline(cmd, state);
         break;
      case LVP_CMD_SET_VIEWPORT:
         handle_set_viewport(cmd, state);
         break;
      case LVP_CMD_SET_SCISSOR:
         handle_set_scissor(cmd, state);
         break;
      case LVP_CMD_SET_LINE_WIDTH:
         handle_set_line_width(cmd, state);
         break;
      case LVP_CMD_SET_DEPTH_BIAS:
         handle_set_depth_bias(cmd, state);
         break;
      case LVP_CMD_SET_BLEND_CONSTANTS:
         handle_set_blend_constants(cmd, state);
         break;
      case LVP_CMD_SET_DEPTH_BOUNDS:
         handle_set_depth_bounds(cmd, state);
         break;
      case LVP_CMD_SET_STENCIL_COMPARE_MASK:
         handle_set_stencil_compare_mask(cmd, state);
         break;
      case LVP_CMD_SET_STENCIL_WRITE_MASK:
         handle_set_stencil_write_mask(cmd, state);
         break;
      case LVP_CMD_SET_STENCIL_REFERENCE:
         handle_set_stencil_reference(cmd, state);
         break;
      case LVP_CMD_BIND_DESCRIPTOR_SETS:
         handle_descriptor_sets(cmd, state);
         break;
      case LVP_CMD_BIND_INDEX_BUFFER:
         handle_index_buffer(cmd, state);
         break;
      case LVP_CMD_BIND_VERTEX_BUFFERS:
         handle_vertex_buffers(cmd, state);
         break;
      case LVP_CMD_DRAW:
         emit_state(state);
         handle_draw(cmd, state);
         break;
      case LVP_CMD_DRAW_INDEXED:
         emit_state(state);
         handle_draw_indexed(cmd, state);
         break;
      case LVP_CMD_DRAW_INDIRECT:
         emit_state(state);
         handle_draw_indirect(cmd, state, false);
         break;
      case LVP_CMD_DRAW_INDEXED_INDIRECT:
         emit_state(state);
         handle_draw_indirect(cmd, state, true);
         break;
      case LVP_CMD_DISPATCH:
         emit_compute_state(state);
         handle_dispatch(cmd, state);
         break;
      case LVP_CMD_DISPATCH_INDIRECT:
         emit_compute_state(state);
         handle_dispatch_indirect(cmd, state);
         break;
      case LVP_CMD_COPY_BUFFER:
         handle_copy_buffer(cmd, state);
         break;
      case LVP_CMD_COPY_IMAGE:
         handle_copy_image(cmd, state);
         break;
      case LVP_CMD_BLIT_IMAGE:
         handle_blit_image(cmd, state);
         break;
      case LVP_CMD_COPY_BUFFER_TO_IMAGE:
         handle_copy_buffer_to_image(cmd, state);
         break;
      case LVP_CMD_COPY_IMAGE_TO_BUFFER:
         handle_copy_image_to_buffer(cmd, state);
         break;
      case LVP_CMD_UPDATE_BUFFER:
         handle_update_buffer(cmd, state);
         break;
      case LVP_CMD_FILL_BUFFER:
         handle_fill_buffer(cmd, state);
         break;
      case LVP_CMD_CLEAR_COLOR_IMAGE:
         handle_clear_color_image(cmd, state);
         break;
      case LVP_CMD_CLEAR_DEPTH_STENCIL_IMAGE:
         handle_clear_ds_image(cmd, state);
         break;
      case LVP_CMD_CLEAR_ATTACHMENTS:
         handle_clear_attachments(cmd, state);
         break;
      case LVP_CMD_RESOLVE_IMAGE:
         handle_resolve_image(cmd, state);
         break;
      case LVP_CMD_SET_EVENT:
      case LVP_CMD_RESET_EVENT:
         handle_event_set(cmd, state);
         break;
      case LVP_CMD_WAIT_EVENTS:
         handle_wait_events(cmd, state);
         break;
      case LVP_CMD_PIPELINE_BARRIER:
         /* skip flushes since every cmdbuf does a flush
            after iterating its cmds and so this is redundant
          */
         if (first || did_flush || cmd->cmd_link.next == &cmd_buffer->cmds)
            continue;
         handle_pipeline_barrier(cmd, state);
         did_flush = true;
         continue;
      case LVP_CMD_BEGIN_QUERY:
         maybe_emit_state_for_begin_query(cmd, state);
         handle_begin_query(cmd, state);
         break;
      case LVP_CMD_END_QUERY:
         handle_end_query(cmd, state);
         break;
      case LVP_CMD_RESET_QUERY_POOL:
         handle_reset_query_pool(cmd, state);
         break;
      case LVP_CMD_WRITE_TIMESTAMP:
         handle_write_timestamp(cmd, state);
         break;
      case LVP_CMD_COPY_QUERY_POOL_RESULTS:
         handle_copy_query_pool_results(cmd, state);
         break;
      case LVP_CMD_PUSH_CONSTANTS:
         handle_push_constants(cmd, state);
         break;
      case LVP_CMD_BEGIN_RENDER_PASS:
         handle_begin_render_pass(cmd, state);
         break;
      case LVP_CMD_NEXT_SUBPASS:
         handle_next_subpass(cmd, state);
         break;
      case LVP_CMD_END_RENDER_PASS:
         handle_end_render_pass(cmd, state);
         break;
      case LVP_CMD_EXECUTE_COMMANDS:
         handle_execute_commands(cmd, state);
         break;
      case LVP_CMD_DRAW_INDIRECT_COUNT:
         emit_state(state);
         handle_draw_indirect_count(cmd, state, false);
         break;
      case LVP_CMD_DRAW_INDEXED_INDIRECT_COUNT:
         emit_state(state);
         handle_draw_indirect_count(cmd, state, true);
         break;
      case LVP_CMD_PUSH_DESCRIPTOR_SET:
         handle_push_descriptor_set(cmd, state);
         break;
      case LVP_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS:
         handle_bind_transform_feedback_buffers(cmd, state);
         break;
      case LVP_CMD_BEGIN_TRANSFORM_FEEDBACK:
         handle_begin_transform_feedback(cmd, state);
         break;
      case LVP_CMD_END_TRANSFORM_FEEDBACK:
         handle_end_transform_feedback(cmd, state);
         break;
      case LVP_CMD_DRAW_INDIRECT_BYTE_COUNT:
         emit_state(state);
         handle_draw_indirect_byte_count(cmd, state);
	 break;
      case LVP_CMD_BEGIN_CONDITIONAL_RENDERING:
         handle_begin_conditional_rendering(cmd, state);
         break;
      case LVP_CMD_END_CONDITIONAL_RENDERING:
         handle_end_conditional_rendering(state);
         break;
      case LVP_CMD_SET_VERTEX_INPUT:
         handle_set_vertex_input(cmd, state);
         break;
      case LVP_CMD_SET_CULL_MODE:
         handle_set_cull_mode(cmd, state);
         break;
      case LVP_CMD_SET_FRONT_FACE:
         handle_set_front_face(cmd, state);
         break;
      case LVP_CMD_SET_PRIMITIVE_TOPOLOGY:
         handle_set_primitive_topology(cmd, state);
         break;
      case LVP_CMD_SET_DEPTH_TEST_ENABLE:
         handle_set_depth_test_enable(cmd, state);
         break;
      case LVP_CMD_SET_DEPTH_WRITE_ENABLE:
         handle_set_depth_write_enable(cmd, state);
         break;
      case LVP_CMD_SET_DEPTH_COMPARE_OP:
         handle_set_depth_compare_op(cmd, state);
         break;
      case LVP_CMD_SET_DEPTH_BOUNDS_TEST_ENABLE:
         handle_set_depth_bounds_test_enable(cmd, state);
         break;
      case LVP_CMD_SET_STENCIL_TEST_ENABLE:
         handle_set_stencil_test_enable(cmd, state);
         break;
      case LVP_CMD_SET_STENCIL_OP:
         handle_set_stencil_op(cmd, state);
         break;
      case LVP_CMD_SET_LINE_STIPPLE:
         handle_set_line_stipple(cmd, state);
         break;
      case LVP_CMD_SET_DEPTH_BIAS_ENABLE:
         handle_set_depth_bias_enable(cmd, state);
         break;
      case LVP_CMD_SET_LOGIC_OP:
         handle_set_logic_op(cmd, state);
         break;
      case LVP_CMD_SET_PATCH_CONTROL_POINTS:
         handle_set_patch_control_points(cmd, state);
         break;
      case LVP_CMD_SET_PRIMITIVE_RESTART_ENABLE:
         handle_set_primitive_restart_enable(cmd, state);
         break;
      case LVP_CMD_SET_RASTERIZER_DISCARD_ENABLE:
         handle_set_rasterizer_discard_enable(cmd, state);
         break;
      }
      first = false;
      did_flush = false;
   }
}

VkResult lvp_execute_cmds(struct lvp_device *device,
                          struct lvp_queue *queue,
                          struct lvp_cmd_buffer *cmd_buffer)
{
   struct rendering_state state;
   memset(&state, 0, sizeof(state));
   state.pctx = queue->ctx;
   state.cso = queue->cso;
   state.blend_dirty = true;
   state.dsa_dirty = true;
   state.rs_dirty = true;
   state.vp_dirty = true;
   for (enum pipe_shader_type s = PIPE_SHADER_VERTEX; s < PIPE_SHADER_TYPES; s++) {
      for (unsigned i = 0; i < PIPE_MAX_SAMPLERS; i++)
         state.cso_ss_ptr[s][i] = &state.ss[s][i];
   }
   /* create a gallium context */
   lvp_execute_cmd_buffer(cmd_buffer, &state);

   state.start_vb = -1;
   state.num_vb = 0;
   cso_unbind_context(queue->cso);
   for (unsigned i = 0; i < PIPE_MAX_SO_BUFFERS; i++) {
      if (state.so_targets[i]) {
         state.pctx->stream_output_target_destroy(state.pctx, state.so_targets[i]);
      }
   }

   free(state.pending_clear_aspects);
   free(state.cleared_views);
   return VK_SUCCESS;
}
