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

#ifndef TERAKAN_STATE_H
#define TERAKAN_STATE_H

#include "terakan_bo.h"
#include "terakan_descriptor.h"
#include "terakan_hw_state.h"
#include "terakan_shader.h"
#include "terakan_state_rasterization.h"
#include "terakan_vertex_input.h"

#include "util/bitset.h"
#include "util/macros.h"

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
 * It can also modify terakan_push_constants_state.
 */

void terakan_state_translate_window_rect_unpacked(VkRect2D const * rect, uint16_t tl_br_xy_out[4]);

enum terakan_state_draw_index {
   TERAKAN_STATE_DRAW_INDEX_VGT_INDEX_TYPE,

   TERAKAN_STATE_DRAW_INDEX_VGT_PRIMITIVE_TYPE,

   TERAKAN_STATE_DRAW_INDEX_VGT_INDEX_OFFSET,

   TERAKAN_STATE_DRAW_INDEX_SQ_PGM_LS_ES_GS_VS,

   /* Depends on TERAKAN_STATE_DRAW_INDEX_SQ_PGM_LS_ES_GS_VS. */
   TERAKAN_STATE_DRAW_INDEX_SQ_PGM_FS,

   TERAKAN_STATE_DRAW_INDEX_SQ_RESOURCES_FS,

   TERAKAN_STATE_DRAW_INDEX_SQ_PGM_PS,

   TERAKAN_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL,

   /* Depends on TERAKAN_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL. */
   TERAKAN_STATE_DRAW_INDEX_VIEWPORT,

   TERAKAN_STATE_DRAW_INDEX_PA_SU_SC_MODE_CNTL,

   TERAKAN_STATE_DRAW_INDEX_PA_CL_VTE_CNTL,

   TERAKAN_STATE_DRAW_INDEX_PA_SC_MODE_CNTL_0,

   TERAKAN_STATE_DRAW_INDEX_PA_SC_AA_MASK,

   TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER,

   /* Depends on TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER. */
   TERAKAN_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_DB_FMT_CNTL,

   TERAKAN_STATE_DRAW_INDEX_DB_RENDER_OVERRIDE,

   TERAKAN_STATE_DRAW_INDEX_DB_STENCILREFMASK,

   TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_CONTROL,

   /* Depends on TERAKAN_STATE_DRAW_INDEX_SQ_PGM_PS. */
   TERAKAN_STATE_DRAW_INDEX_COLOR_ATTACHMENT_USAGE,
   TERAKAN_STATE_DRAW_INDEX_LOGIC_OP,
   /* Depends on TERAKAN_STATE_DRAW_INDEX_COLOR_ATTACHMENT_USAGE and
    * TERAKAN_STATE_DRAW_INDEX_LOGIC_OP.
    */
   TERAKAN_STATE_DRAW_INDEX_CB_BLEND_CONTROL,
   /* Depends on TERAKAN_STATE_DRAW_INDEX_COLOR_ATTACHMENT_USAGE and
    * TERAKAN_STATE_DRAW_INDEX_CB_BLEND_CONTROL.
    */
   TERAKAN_STATE_DRAW_INDEX_CB_COLOR_RTV,
   /* Depends on TERAKAN_STATE_DRAW_INDEX_COLOR_ATTACHMENT_USAGE and
    * TERAKAN_STATE_DRAW_INDEX_CB_COLOR_RTV.
    */
   TERAKAN_STATE_DRAW_INDEX_CB_TARGET_MASK,
   /* Depends on TERAKAN_STATE_DRAW_INDEX_CB_TARGET_MASK and TERAKAN_STATE_DRAW_INDEX_LOGIC_OP. */
   TERAKAN_STATE_DRAW_INDEX_CB_COLOR_CONTROL,

   /* Depends on TERAKAN_STATE_DRAW_INDEX_SQ_PGM_PS and TERAKAN_STATE_DRAW_INDEX_CB_COLOR_RTV. */
   TERAKAN_STATE_DRAW_INDEX_DB_SHADER_CONTROL,

   TERAKAN_STATE_DRAW_INDEX_COUNT,
};

#define TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(dependent, dependency)                                \
   static_assert(TERAKAN_STATE_DRAW_INDEX_##dependent > TERAKAN_STATE_DRAW_INDEX_##dependency,     \
                 #dependent " depends on " #dependency " and thus must have a higher index.")

struct terakan_state_draw_sq_resource_fs {
   uint64_t va;
   /* If `bo` is NULL, no buffer is bound. */
   struct terakan_bo const * bo;
   uint32_t size_bytes_minus_1;
   uint16_t stride;
};

static_assert(MESA_VK_MAX_VIEWPORTS >= TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS,
              "Expecting that the Mesa Vulkan runtime can expose all hardware viewports.");

struct terakan_state_draw_cb_color {
   struct terakan_bo const * bo;
   struct terakan_color_descriptor color;
   struct terakan_color_meta_descriptor meta;
};

/* State applied before performing application's draws, and marked for reapplication after internal
 * draws.
 */
struct terakan_state_draw {
   /* Whether each state item has been modified, overridden by an internal draw, or had its
    * dependencies changed, and needs to be applied before the next draw.
    */
   BITSET_DECLARE(state_pending, TERAKAN_STATE_DRAW_INDEX_COUNT);

   /* Configuration of state setting commands themselves. */
   bool cmd_set_depth_clamp_enable_sets_depth_clip_enable;

   /* TERAKAN_STATE_DRAW_INDEX_VGT_INDEX_TYPE */
   uint32_t vgt_index_type;

   /* TERAKAN_STATE_DRAW_INDEX_VGT_PRIMITIVE_TYPE */
   uint32_t vgt_primitive_type;

   /* TERAKAN_STATE_DRAW_INDEX_VGT_INDEX_OFFSET */
   uint32_t vgt_index_offset;

   /* TERAKAN_STATE_DRAW_INDEX_SQ_PGM_LS_ES_GS_VS */
   struct {
      struct terakan_shader_impl const * vs_as_vs;
   } sq_pgm_ls_es_gs_vs;

   /* TERAKAN_STATE_DRAW_INDEX_SQ_PGM_FS */
   struct {
      /* Vertex input state can be either static (from a pipeline with non-dynamic vertex input
       * state and a vertex shader) or dynamic (from vkCmdSetVertexInputEXT).
       *
       * For static vertex input state, normally the fetch shader program generated at pipeline
       * creation time is used. However, some state can be overridden, in which case a transient
       * fetch shader will be generated.
       *
       * For dynamic vertex input state, transient fetch shader programs are generated when the
       * state is changed. They're allocated similarly to push constants.
       */

      /* If the static state pointer is NULL, dynamic state is used instead. */
      struct terakan_vertex_input_static_state const * static_state;

      struct {
         BITSET_DECLARE(attributes_provided, TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES);
         /* The values are undefined for attributes not in attributes_provided. */
         struct terakan_vertex_input_attribute attributes[TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES];

         uint32_t bindings_needed_by_attributes_and_provided;
         /* For bindings not in bindings_needed_by_attributes_and_provided, the bits must be 0. */
         uint32_t instance_bindings;
         /* The values are undefined for bindings not in
          * bindings_needed_by_attributes_and_provided.
          */
         uint32_t instance_binding_divisors[TERAKAN_RESOURCE_HW_COUNT_FETCH];

         struct {
            BITSET_DECLARE(attributes_needed_by_vs, TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES);
         } from_apply_sq_pgm_ls_es_gs_vs;
      } dynamic_state;

      /* When using static vertex input state, if this value doesn't match which bindings have the
       * 2048 stride workaround in the static fetch shader (among the bindings actually used by it),
       * a transient fetch shader program will be generated.
       * This mask is also used for dynamic vertex input state.
       */
      uint32_t bindings_with_2048_stride_workaround;
   } sq_pgm_fs;

   /* TERAKAN_STATE_DRAW_INDEX_SQ_RESOURCES_FS */
   uint32_t sq_resources_fs_pending;
   struct terakan_state_draw_sq_resource_fs sq_resources_fs[TERAKAN_RESOURCE_HW_COUNT_FETCH];

   /* TERAKAN_STATE_DRAW_INDEX_SQ_PGM_PS */
   struct {
      /* Can be NULL, in which case an empty pixel shader exporting a color with an alpha of 1 will
       * be used.
       */
      struct terakan_shader_impl const * fs;
   } sq_pgm_ps;

   /* TERAKAN_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL */
   uint32_t pa_cl_clip_cntl;

   /* TERAKAN_STATE_DRAW_INDEX_VIEWPORT */
   struct {
      uint8_t count;

      /* viewports_pending contains bits indicating whether each viewport state item is pending for
       * each of the viewports (one bit per viewport).
       *
       * Note that these pending flags (both per-viewport and global) must not be cleared without
       * actually applying the state first if they are set, as they may be set from meta draws
       * changing the viewport state in hw_state_draw, making it out of sync with state_draw.
       */
      struct {
         /* pa_cl_vport_xy_scale_offset pending. */
         uint16_t pa_cl_vport_xy_scale_offset;
         /* pa_cl_vport_z_gl_dx_scale_offset, dx_clip_space_def pending. */
         uint16_t pa_cl_vport_z_scale_offset;
         /* pa_sc_vport_scissor_tl_br_xy, pa_sc_vport_generic_scissor_tl_br_xy pending. */
         uint16_t pa_sc_vport_scissor;
         /* pa_sc_vport_z_min_max, pa_sc_vport_z_min_0_max_1 pending. */
         uint16_t pa_sc_vport_z_min_max;
      } viewports_pending;
      /* pa_cl_gb_vert_horz_clip_adj pending. */
      bool pa_cl_gb_pending;

      struct {
         /* If changing, make pa_cl_vport_z_scale_offset pending for all `ARRAY_SIZE(viewports)`
          * viewports.
          */
         bool dx_clip_space_def;
      } from_apply_pa_cl_clip_cntl;

      /* Whether PA_SC_VPORT_ZMIN/MAX must be 0 to 1 regardless of the values in the viewport.
       * According to PAL, for optimal performance, depth clamping should be enabled, so without
       * unrestricted depth range, the preferred way to disable depth clamping is to clamp depth to
       * 0 to 1 instead of not clamping it completely by setting DISABLE_VIEWPORT_CLAMP in
       * DB_RENDER_OVERRIDE.
       *
       * If changing, make pa_sc_vport_z_min_max pending for all `ARRAY_SIZE(viewports)` viewports.
       */
      bool pa_sc_vport_z_min_0_max_1;

      struct terakan_state_draw_viewport viewports[TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS];

      uint16_t pa_sc_vport_generic_scissor_tl_br_xy[TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS][2][2];
   } viewport;

   /* TERAKAN_STATE_DRAW_INDEX_PA_SU_SC_MODE_CNTL */
   uint32_t pa_su_sc_mode_cntl;

   /* TERAKAN_STATE_DRAW_INDEX_PA_SC_AA_MASK */
   uint16_t pa_sc_aa_mask;

   /* TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER */
   struct {
      struct terakan_bo const * bo;
      /* The descriptor is undefined if the BO is NULL. */
      struct terakan_depth_stencil_descriptor descriptor;
   } db_depth_stencil_buffer;

   /* TERAKAN_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_DB_FMT_CNTL */
   struct {
      VkDepthBiasRepresentationEXT representation;
      bool representation_exact;
      struct {
         /* A S_028040_FORMAT value. V_028040_Z_INVALID is not allowed, specify
          * TERAKAN_STATE_DRAW_PA_SU_POLY_OFFSET_DB_FMT_CNTL_NO_ATTACHMENT_FORMAT when a depth
          * attachment is not present instead.
          */
         uint32_t poly_offset_z_format;
      } from_apply_db_depth_stencil_buffer;
   } pa_su_poly_offset_db_fmt_cntl;

   /* TERAKAN_STATE_DRAW_INDEX_DB_RENDER_OVERRIDE */
   uint32_t db_render_override;

   /* TERAKAN_STATE_DRAW_INDEX_DB_STENCILREFMASK */
   uint32_t db_stencilrefmask_front_back[2];

   /* TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_CONTROL
    * BACKFACE_ENABLE is always enabled implicitly.
    */
   uint32_t db_depth_control;

   /* Color target state is indexed by render pass color attachment index (hence the word
    * "attachment" in the names).
    * However, color exports in shaders are compacted to make sure all UAVs are placed above all
    * RTVs (the Programming Guide states that it's required, plus that makes it possible to allocate
    * arrays of UAVs without fragmentation), and so CB_SHADER_MASK doesn't have holes (that's needed
    * to avoid hangs according to RadeonSI, but this appears to apply to earlier hardware too).
    * Therefore, color target registers are reindexed when they're written to the hardware state,
    * skipping targets not in `color_attachment_usage.written_by_shader`.
    */

   /* TERAKAN_STATE_DRAW_INDEX_COLOR_ATTACHMENT_USAGE */
   struct {
      struct {
         /* The mask used for compaction of render pass color attachment indices in this state
          * structure into hardware state color attachment indices.
          */
         uint8_t written_by_shader;
      } from_apply_sq_pgm_ps;
   } color_attachment_usage;

   /* TERAKAN_STATE_DRAW_INDEX_LOGIC_OP */
   struct {
      /* If not enabled, ROP3 must be assumed to be 0xCC. */
      bool enable;
      uint8_t rop3;
   } logic_op;

   /* TERAKAN_STATE_DRAW_INDEX_CB_BLEND_CONTROL
    * The CB_BLEND#_CONTROL register values stored here are directly the application's values for
    * VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT (BLEND_CONTROL_ENABLE) and
    * VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT (all other fields). Any needed overrides are
    * resolved when applying, without modifying the stored values.
    */
   struct {
      uint32_t attachments[TERAKAN_COLOR_HW_RTV_COUNT];
   } cb_blend_control;

   /* TERAKAN_STATE_DRAW_INDEX_CB_COLOR_RTV
    * The descriptors are undefined for targets with the BO being NULL.
    */
   struct {
      struct terakan_state_draw_cb_color attachments[TERAKAN_COLOR_HW_RTV_COUNT];
      struct {
         bool dual_source_blend;
      } from_apply_cb_blend_control;
   } cb_color_rtv;

   /* TERAKAN_STATE_DRAW_INDEX_CB_TARGET_MASK */
   struct {
      /* If the attachment_write_enable bit for the attachment is not set, the write mask must be
       * assumed to be 0.
       */
      uint8_t attachment_write_enable;
      uint8_t attachment_write_masks[TERAKAN_COLOR_HW_RTV_COUNT];
      struct {
         uint32_t attachment_format_masks;
      } from_apply_cb_color_rtv;
   } cb_target_mask;

   /* TERAKAN_STATE_DRAW_INDEX_CB_COLOR_CONTROL */
   struct {
      struct {
         bool any_rtv_enabled;
      } from_apply_cb_target_mask;
   } cb_color_control;

   /* TERAKAN_STATE_DRAW_INDEX_DB_SHADER_CONTROL */
   struct {
      struct {
         bool cb_dual_export_allowed;
      } from_apply_cb_color_rtv;
   } db_shader_control;
};

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

static inline void
terakan_state_draw_set_viewport_count(struct terakan_state_draw * const state,
                                      uint32_t const viewport_count)
{
   assert(viewport_count <= ARRAY_SIZE(state->viewport.viewports));
   if (state->viewport.count == viewport_count) {
      return;
   }
   state->viewport.count = viewport_count;
   state->viewport.pa_cl_gb_pending = true;
   terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_VIEWPORT);
}

struct terakan_gfx_command_writer;

void terakan_state_draw_apply_pending(struct terakan_gfx_command_writer * command_writer);

struct terakan_device;

void terakan_state_draw_reset(struct terakan_state_draw * state,
                              struct terakan_device const * device);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_STATE_H */
