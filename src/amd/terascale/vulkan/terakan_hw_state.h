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

#ifndef TERAKAN_HW_STATE_H
#define TERAKAN_HW_STATE_H

#include "terakan_bo.h"
#include "terakan_descriptor.h"
#include "terakan_shader.h"

#include "compiler/shader_enums.h"
#include "util/bitset.h"
#include "util/macros.h"
#include "vk_limits.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_hw_state_sq_kcache_buffer {
   /* BO and base are undefined if the size is 0. */
   struct terakan_bo const * bo;
   uint32_t va_lines;
   uint32_t size_lines;
};

enum terakan_hw_state_draw_sq_constants_needed_stage {
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,

   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT,
};

enum terakan_hw_state_draw_sq_constants_modified_stage {
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TCS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_GS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_FS,

   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_COUNT,
};

struct terakan_hw_state_draw_vertex_constant_bits {
   uint16_t kcache;

   BITSET_DECLARE(resources, TERAKAN_RESOURCE_HW_COUNT_VERTEX);

   uint32_t samplers;
   uint32_t sampler_border_colors;
};

#define TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS 16

struct terakan_hw_state_draw_viewport {
   float pa_cl_vport_xyz_scale_offset[3][2];
   float pa_sc_vport_z_min_max[2];
   uint32_t pa_sc_vport_scissor[2];
};

/* PA_SU_POLY_OFFSET_FRONT/BACK_SCALE is applied to the slope between subpixels that is computed by
 * the hardware. To convert a pixel slope scale from Vulkan to a subpixel slope scale in the
 * registers, multiply it by TERAKAN_HW_STATE_DRAW_POLY_OFFSET_SLOPE_SUBPIXELS_IN_PIXEL.
 */
#define TERAKAN_HW_STATE_DRAW_POLY_OFFSET_SLOPE_SUBPIXELS_IN_PIXEL 16.0f

extern uint32_t const terakan_standard_sample_locs[5][16 / 4];
extern uint32_t const terakan_standard_sample_max_dists[5];

enum terakan_hw_state_draw_index {
   /* Generally ordered roughly by the location of the hardware unit in the pipeline, and within
    * each unit, by the register addresses.
    */

   TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_TYPE,

   TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_BUFFER,

   TERAKAN_HW_STATE_DRAW_INDEX_VGT_PRIMITIVE_TYPE,

   TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_OFFSET,

   TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_FS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_VS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_PS,

   TERAKAN_HW_STATE_DRAW_INDEX_SQ_VTX_START_INST_LOC,

   TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL,

   TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_SC_MODE_CNTL,

   TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_VTE_CNTL,

   TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_MODE_CNTL_0,

   TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_DB_FMT_CNTL,

   TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_CLAMP_SCALE_OFFSET,

   TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_GB,

   TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_AA_SAMPLES,

   TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_AA_MASK,

   TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER,

   TERAKAN_HW_STATE_DRAW_INDEX_DB_RENDER_OVERRIDE,

   TERAKAN_HW_STATE_DRAW_INDEX_DB_STENCILREFMASK,

   TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_CONTROL,

   TERAKAN_HW_STATE_DRAW_INDEX_DB_SHADER_CONTROL,

   TERAKAN_HW_STATE_DRAW_INDEX_CB_TARGET_MASK,

   TERAKAN_HW_STATE_DRAW_INDEX_CB_BLEND_RGBA,

   TERAKAN_HW_STATE_DRAW_INDEX_CB_COLOR_CONTROL,

   /* State items starting from TERAKAN_HW_STATE_DRAW_INDEX_SPECIAL_FIRST have their modified flags
    * set via some method different from terakan_hw_state_draw_written.
    */
   TERAKAN_HW_STATE_DRAW_INDEX_SPECIAL_FIRST,

   TERAKAN_HW_STATE_DRAW_INDEX_VIEWPORT = TERAKAN_HW_STATE_DRAW_INDEX_SPECIAL_FIRST,

   TERAKAN_HW_STATE_DRAW_INDEX_CB_BLEND_CONTROL,

   TERAKAN_HW_STATE_DRAW_INDEX_CB_COLOR,

   /* Matching the sequence in terakan_hw_state_draw_sq_constants_needed_stage. */
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_VS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_TCS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_TES,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_GS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_FS,

   TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VI,
   /* Matching the sequence in terakan_hw_state_draw_sq_constants_needed_stage. */
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_TCS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_TES,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_GS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_FS,

   /* Matching the sequence in terakan_hw_state_draw_sq_constants_needed_stage. */
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_VS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_TCS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_TES,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_GS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_FS,

   /* Matching the sequence in terakan_hw_state_draw_sq_constants_needed_stage. */
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_VS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_TCS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_TES,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_GS,
   TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_FS,

   TERAKAN_HW_STATE_DRAW_INDEX_COUNT,
};

/* State applied before performing application's or internal draws, and reapplied when switching to
 * a new indirect buffer in the Vulkan command buffer.
 */
struct terakan_hw_state_draw {
   /* Whether each state item has ever been written, and thus has a value that's not complete junk,
    * and is potentially relevant to the current command buffer.
    */
   BITSET_DECLARE(state_ever_written, TERAKAN_HW_STATE_DRAW_INDEX_COUNT);
   /* Whether each state item has been modified and needs to be emitted before the next draw. */
   BITSET_DECLARE(state_modified, TERAKAN_HW_STATE_DRAW_INDEX_COUNT);

   /* TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_TYPE */
   uint32_t vgt_index_type;

   /* TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_BUFFER */
   struct {
      struct terakan_bo const * bo;
      uint64_t va;
      /* In units of indices. */
      uint32_t size;
   } vgt_index_buffer;

   /* TERAKAN_HW_STATE_DRAW_INDEX_VGT_PRIMITIVE_TYPE */
   uint32_t vgt_primitive_type;

   /* TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_OFFSET */
   uint32_t vgt_index_offset;

   /* TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_FS */
   struct {
      struct terakan_bo const * bo;
      uint32_t va_shr8;
   } sq_pgm_fs;

   /* TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_VS */
   struct terakan_shader_static const * sq_pgm_vs;

   /* TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_PS */
   struct terakan_shader_static const * sq_pgm_ps;

   /* TERAKAN_HW_STATE_DRAW_INDEX_SQ_VTX_START_INST_LOC */
   uint32_t sq_vtx_start_inst_loc;

   /* TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL */
   uint32_t pa_cl_clip_cntl;

   /* TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_SC_MODE_CNTL */
   uint32_t pa_su_sc_mode_cntl;

   /* TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_VTE_CNTL */
   uint32_t pa_cl_vte_cntl;

   /* TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_MODE_CNTL_0 */
   uint32_t pa_sc_mode_cntl_0;

   /* TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_DB_FMT_CNTL */
   uint32_t pa_su_poly_offset_db_fmt_cntl;

   /* TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_CLAMP_SCALE_OFFSET */
   float pa_su_poly_offset_clamp;
   float pa_su_poly_offset_subpixel_slope_scale;
   float pa_su_poly_offset_offset;

   /* TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_GB */
   float pa_cl_gb_vert_horz_clip_disc_adj[2][2];

   /* TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_AA_SAMPLES */
   struct {
      uint32_t num_samples_log2;
   } pa_sc_aa_samples;

   /* TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_AA_MASK */
   uint16_t pa_sc_aa_mask;

   /* TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER */
   struct {
      struct terakan_bo const * bo;
      /* The descriptor is undefined if the BO is NULL. */
      struct terakan_depth_stencil_descriptor descriptor;
   } db_depth_stencil_buffer;

   /* TERAKAN_HW_STATE_DRAW_INDEX_DB_RENDER_OVERRIDE */
   uint32_t db_render_override;

   /* TERAKAN_HW_STATE_DRAW_INDEX_DB_STENCILREFMASK */
   uint32_t db_stencilrefmask_front_back[2];

   /* TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_CONTROL */
   uint32_t db_depth_control;

   /* TERAKAN_HW_STATE_DRAW_INDEX_DB_SHADER_CONTROL */
   uint32_t db_shader_control;

   /* TERAKAN_HW_STATE_DRAW_INDEX_CB_TARGET_MASK */
   uint32_t cb_target_mask;

   /* TERAKAN_HW_STATE_DRAW_INDEX_CB_BLEND_RGBA */
   float cb_blend_rgba[4];

   /* TERAKAN_HW_STATE_DRAW_INDEX_CB_COLOR_CONTROL */
   uint32_t cb_color_control;

   /* TERAKAN_HW_STATE_DRAW_INDEX_VIEWPORT
    * Modify `viewports` and call terakan_hw_state_draw_update_viewports (ever-written is not
    * tracked).
    * Don't access viewport_counts externally.
    */
   struct {
      /* Upper bound of viewports needed by the next draw. */
      uint8_t needed;
      /* Upper bound of viewports for which the last emitted PA_CL_VPORT_X/Y/ZSCALE/OFFSET and
       * PA_SC_VPORT_ZMIN/MAX are currently up to date.
       */
      uint8_t scale_offset_z_min_max_emitted;
      /* Upper bound of viewports for which the last emitted PA_SC_VPORT_SCISSOR is currently up to
       * date.
       */
      uint8_t scissor_emitted;
   } viewport_counts;
   struct terakan_hw_state_draw_viewport viewports[TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS];

   /* TERAKAN_HW_STATE_DRAW_INDEX_CB_BLEND_CONTROL
    * Don't access externally directly, instead set via terakan_hw_state_draw_set_cb_blend_control.
    */
   struct {
      uint8_t ever_written;
      uint8_t modified;
      uint32_t cb_blend_control[TERAKAN_COLOR_HW_RTV_COUNT];
   } cb_blend_control;

   /* TERAKAN_HW_STATE_DRAW_INDEX_CB_COLOR
    * Don't access externally directly, instead set via terakan_hw_state_draw_set_cb_color or
    * terakan_hw_state_draw_set_cb_color1_dual_source.
    */
   struct {
      uint16_t ever_written;
      uint16_t modified;
      struct terakan_bo const * bo[TERAKAN_COLOR_HW_RTV_AND_UAV_COUNT];
      /* The descriptor fields other than the INFO register are undefined if the respective BO is
       * NULL.
       */
      struct terakan_color_descriptor color[TERAKAN_COLOR_HW_RTV_AND_UAV_COUNT];
      struct terakan_color_meta_descriptor meta[TERAKAN_COLOR_HW_RTV_COUNT];
   } cb_color;

   /* Sequencer constants.
    * Don't access externally directly, use the respective setters.
    */

   /* Constants demand of the shaders used in the next state emission. */
   struct {
      /* Whether each shader stage is used in the pipeline, and thus constants are needed for it.
       * These are independent from the actual bits of the needed constants for these stages, based
       * on how shaders are chained.
       * For simplicity (and to avoid checking these while setting every single constant, which can
       * be done many times before the first draw that happens afterwards with the respective
       * state_modified bits set), the respective emit calls may be done regardless of whether these
       * are true.
       */
      bool tcs_tes;
      bool gs_after_vs;
      bool gs_after_tes;

      uint16_t kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT];

      struct {
         BITSET_DECLARE(vi, TERAKAN_RESOURCE_HW_COUNT_FETCH);
         BITSET_DECLARE(vs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(tcs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(tes, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(gs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(fs, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE);
      } resources;

      uint32_t samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT];
   } sq_constants_needed;

   /* If the modified bit for the constant index is set, the new constant is considered to be
    * completely new, and must be emitted for the specific stage next time it's needed.
    * Otherwise, the constant may already be set to the needed value in the hardware, or it might
    * have been set to it previously, but now has been evicted in the VSES constant by the other
    * Vulkan VS or TES stage.
    */
   struct {
      uint16_t kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_COUNT];

      struct {
         BITSET_DECLARE(vi, TERAKAN_RESOURCE_HW_COUNT_FETCH);
         BITSET_DECLARE(vs_in_ls, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(vs_in_vses, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(tcs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(tes_in_vses, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(gs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(fs, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE);
      } resources;

      uint32_t samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_COUNT];
      uint32_t sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_COUNT];
   } sq_constants_modified;

   /* Whether the constant emitted into VSES for VS or TES might have potentially been overwritten
    * in the hardware by an emission for the other Vulkan stage, and thus when the constant is
    * needed for that other stage later, it may need to be re-emitted.
    * If the modified bit for the VS in VSES constant is true, the "VS constant overwritten by TES"
    * bit for it should be considered out of date and thus ignored, same for the "TES constant
    * overwritten by VS" bit if the corresponding TES constant was modified.
    */
   struct terakan_hw_state_draw_vertex_constant_bits sq_constants_for_vs_overwritten_in_vses_by_tes;
   struct terakan_hw_state_draw_vertex_constant_bits sq_constants_for_tes_overwritten_in_vses_by_vs;

   struct terakan_hw_state_sq_kcache_buffer
      sq_kcache_buffers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT]
                       [TERAKAN_KCACHE_HW_BUFFERS_PER_STAGE];

   /* The BOs and descriptors are undefined if the sq_resources_not_null bit for the resource index
    * is not set.
    */
   struct {
      BITSET_DECLARE(vi, TERAKAN_RESOURCE_HW_COUNT_FETCH);
      BITSET_DECLARE(vs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
      BITSET_DECLARE(tcs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
      BITSET_DECLARE(tes, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
      BITSET_DECLARE(gs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
      BITSET_DECLARE(fs, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE);
   } sq_resources_not_null;
   struct {
      struct terakan_bo const * vi[TERAKAN_RESOURCE_HW_COUNT_FETCH];
      struct terakan_bo const * vs[TERAKAN_RESOURCE_HW_COUNT_VERTEX];
      struct terakan_bo const * tcs[TERAKAN_RESOURCE_HW_COUNT_VERTEX];
      struct terakan_bo const * tes[TERAKAN_RESOURCE_HW_COUNT_VERTEX];
      struct terakan_bo const * gs[TERAKAN_RESOURCE_HW_COUNT_VERTEX];
      struct terakan_bo const * fs[TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE];
   } sq_resource_bos;
   struct {
      uint32_t vi[TERAKAN_RESOURCE_HW_COUNT_FETCH][8];
      uint32_t vs[TERAKAN_RESOURCE_HW_COUNT_VERTEX][8];
      uint32_t tcs[TERAKAN_RESOURCE_HW_COUNT_VERTEX][8];
      uint32_t tes[TERAKAN_RESOURCE_HW_COUNT_VERTEX][8];
      uint32_t gs[TERAKAN_RESOURCE_HW_COUNT_VERTEX][8];
      uint32_t fs[TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE][8];
   } sq_resource_descriptors;

   uint32_t sq_samplers_ever_written[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT];
   /* The bits are 0 for samplers not in sq_samplers_ever_written (the only interface exposed for
    * setting the border color is setting the sampler itself).
    */
   uint32_t
      sq_sampler_border_colors_ever_written[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT];
   /* The bits are 0 for samplers not in sq_samplers_ever_written. */
   uint32_t sq_samplers_with_border_color[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT];
   uint32_t sq_samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT]
                       [TERAKAN_SAMPLER_HW_COUNT_PER_STAGE][3];
   float sq_sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT]
                                 [TERAKAN_SAMPLER_HW_COUNT_PER_STAGE][4];
};

/* Pass the result of the external comparison to reduce the amount of state setting packets if the
 * state was not modified if needed (especially recommended when using static state in pipeline
 * objects). Floating-point state must be compared using memcmp to distinguish between the signs of
 * zero.
 * The function must still be called even if the state wasn't changed, however, to mark the state
 * item as needed in the current command buffer and also to make sure it's emitted for the first
 * time before the next draw.
 */
static inline void
terakan_hw_state_draw_written(struct terakan_hw_state_draw * const state,
                              enum terakan_hw_state_draw_index const state_index, bool modified)
{
   assert(state_index < TERAKAN_HW_STATE_DRAW_INDEX_SPECIAL_FIRST);
   if (!BITSET_TEST(state->state_ever_written, state_index)) {
      BITSET_SET(state->state_ever_written, state_index);
      modified = true;
   }
   if (modified) {
      BITSET_SET(state->state_modified, state_index);
   }
}

/* `lowest_modified` are the lowest indices of the viewports for which the corresponding states were
 * modified. If not modifying those states, pass ARRAY_SIZE(state->viewports).
 */
static inline void
terakan_hw_state_draw_update_viewports(struct terakan_hw_state_draw * const state,
                                       uint8_t const viewport_count_needed,
                                       uint8_t const scale_offset_z_min_max_lowest_modified,
                                       uint8_t const scissor_lowest_modified)
{
   assert(viewport_count_needed <= ARRAY_SIZE(state->viewports));
   state->viewport_counts.needed = viewport_count_needed;

   state->viewport_counts.scale_offset_z_min_max_emitted =
      MIN2(scale_offset_z_min_max_lowest_modified,
           state->viewport_counts.scale_offset_z_min_max_emitted);
   state->viewport_counts.scissor_emitted =
      MIN2(scissor_lowest_modified, state->viewport_counts.scissor_emitted);

   if (state->viewport_counts.scale_offset_z_min_max_emitted < state->viewport_counts.needed ||
       state->viewport_counts.scissor_emitted < state->viewport_counts.needed) {
      BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_VIEWPORT);
   } else {
      BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_VIEWPORT);
   }
}

static inline void
terakan_hw_state_draw_set_cb_blend_control(struct terakan_hw_state_draw * const state,
                                           uint32_t const color_index, uint32_t cb_blend_control)
{
   bool modified = state->cb_blend_control.cb_blend_control[color_index] != cb_blend_control;
   state->cb_blend_control.cb_blend_control[color_index] = cb_blend_control;
   uint8_t const color_bit = (uint8_t)1 << color_index;
   if (!(state->cb_blend_control.ever_written & color_bit)) {
      state->cb_blend_control.ever_written |= color_bit;
      modified = true;
   }
   if (modified) {
      state->cb_blend_control.modified |= color_bit;
      BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_CB_BLEND_CONTROL);
   }
}

/* Setting `bo` to NULL disables the color target, the descriptor pointers are ignored if `bo` is
 * NULL.
 * If `meta` is NULL for a non-NULL target, the target will be bound without metadata.
 * Note that all unbound targets that the pixel shader exports to must be disabled explicitly, as
 * CB_COLOR# (at least INFO::SOURCE_FORMAT) also has effect on shader export.
 */
void terakan_hw_state_draw_set_cb_color(struct terakan_hw_state_draw * state, uint32_t color_index,
                                        struct terakan_bo const * bo,
                                        struct terakan_color_descriptor const * color,
                                        struct terakan_color_meta_descriptor const * meta,
                                        bool is_uav);
/* For dual-source blending, unbinds the target 1, and sets its SOURCE_FORMAT to the specified
 * format, which must match the MRT 0 according to Radeon Evergreen / Northern Islands Acceleration,
 * so 2 exported quads come to blending in the same format.
 */
void terakan_hw_state_draw_set_cb_color1_dual_source(struct terakan_hw_state_draw * state,
                                                     uint32_t source_format);

/* Section 14.2.3. "Allocation of Descriptor Sets" of the Vulkan 1.3.275 specification says:
 *
 *     "For descriptor set bindings created without the VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
 *     bit set, all descriptors in that binding that are statically used must have been populated
 *     before the descriptor set is consumed."
 *
 *     "Entries that are not used by a pipeline can have undefined descriptors."
 *
 * Therefore:
 * - The setters (and their callers binding descriptor sets) must not dereference the BO pointers
 *   passed to them because they may be pointing to an already freed BO.
 * - Bindings not statically used by shaders must not be emitted.
 */

typedef void (*terakan_hw_state_draw_set_sq_kcache_function)(struct terakan_hw_state_draw * state,
                                                             uint32_t index, uint32_t size_lines,
                                                             struct terakan_bo const * bo,
                                                             uint32_t va_lines);
void terakan_hw_state_draw_set_sq_kcache_vs(struct terakan_hw_state_draw * state, uint32_t index,
                                            uint32_t size_lines, struct terakan_bo const * bo,
                                            uint32_t va_lines);
void terakan_hw_state_draw_set_sq_kcache_tcs(struct terakan_hw_state_draw * state, uint32_t index,
                                             uint32_t size_lines, struct terakan_bo const * bo,
                                             uint32_t va_lines);
void terakan_hw_state_draw_set_sq_kcache_tes(struct terakan_hw_state_draw * state, uint32_t index,
                                             uint32_t size_lines, struct terakan_bo const * bo,
                                             uint32_t va_lines);
void terakan_hw_state_draw_set_sq_kcache_gs(struct terakan_hw_state_draw * state, uint32_t index,
                                            uint32_t size_lines, struct terakan_bo const * bo,
                                            uint32_t va_lines);
void terakan_hw_state_draw_set_sq_kcache_fs(struct terakan_hw_state_draw * state, uint32_t index,
                                            uint32_t size_lines, struct terakan_bo const * bo,
                                            uint32_t va_lines);
extern terakan_hw_state_draw_set_sq_kcache_function const
   terakan_hw_state_draw_set_sq_kcache_for_stage[MESA_SHADER_FRAGMENT + 1];

typedef void (*terakan_hw_state_draw_set_sq_resource_function)(struct terakan_hw_state_draw * state,
                                                               uint32_t index,
                                                               struct terakan_bo const * bo,
                                                               uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_vi(struct terakan_hw_state_draw * state, uint32_t index,
                                              struct terakan_bo const * bo,
                                              uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_vs(struct terakan_hw_state_draw * state, uint32_t index,
                                              struct terakan_bo const * bo,
                                              uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_tcs(struct terakan_hw_state_draw * state, uint32_t index,
                                               struct terakan_bo const * bo,
                                               uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_tes(struct terakan_hw_state_draw * state, uint32_t index,
                                               struct terakan_bo const * bo,
                                               uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_gs(struct terakan_hw_state_draw * state, uint32_t index,
                                              struct terakan_bo const * bo,
                                              uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_fs(struct terakan_hw_state_draw * state, uint32_t index,
                                              struct terakan_bo const * bo,
                                              uint32_t const descriptor[8]);
extern terakan_hw_state_draw_set_sq_resource_function const
   terakan_hw_state_draw_set_sq_resource_for_stage[MESA_SHADER_FRAGMENT + 1];

/* The border color can be NULL if BORDER_COLOR_TYPE is not REGISTER. */
typedef void (*terakan_hw_state_draw_set_sq_sampler_function)(struct terakan_hw_state_draw * state,
                                                              uint32_t index,
                                                              uint32_t const sampler[3],
                                                              float const border_color[4]);
void terakan_hw_state_draw_set_sq_sampler_vs(struct terakan_hw_state_draw * state, uint32_t index,
                                             uint32_t const sampler[3],
                                             float const border_color[4]);
void terakan_hw_state_draw_set_sq_sampler_tcs(struct terakan_hw_state_draw * state, uint32_t index,
                                              uint32_t const sampler[3],
                                              float const border_color[4]);
void terakan_hw_state_draw_set_sq_sampler_tes(struct terakan_hw_state_draw * state, uint32_t index,
                                              uint32_t const sampler[3],
                                              float const border_color[4]);
void terakan_hw_state_draw_set_sq_sampler_gs(struct terakan_hw_state_draw * state, uint32_t index,
                                             uint32_t const sampler[3],
                                             float const border_color[4]);
void terakan_hw_state_draw_set_sq_sampler_fs(struct terakan_hw_state_draw * state, uint32_t index,
                                             uint32_t const sampler[3],
                                             float const border_color[4]);
extern terakan_hw_state_draw_set_sq_sampler_function const
   terakan_hw_state_draw_set_sq_sampler_for_stage[MESA_SHADER_FRAGMENT + 1];

static inline bool
terakan_hw_state_draw_sq_constants_needed_by_gs(struct terakan_hw_state_draw const * const state)
{
   return state->sq_constants_needed.tcs_tes ? state->sq_constants_needed.gs_after_tes
                                             : state->sq_constants_needed.gs_after_vs;
}

/* resources_opt is optional, passing NULL disables all constants for this stage (for instance, if
 * no shader is bound to the stage).
 */
void terakan_hw_state_draw_set_sq_constants_needed_by_vi(struct terakan_hw_state_draw * state,
                                                         BITSET_WORD const * resources_opt);
void terakan_hw_state_draw_set_sq_constants_needed_by_vs(struct terakan_hw_state_draw * state,
                                                         uint16_t kcache,
                                                         BITSET_WORD const * resources_opt,
                                                         uint32_t samplers,
                                                         VkShaderStageFlags next_stage);
void terakan_hw_state_draw_set_sq_constants_needed_by_tcs(struct terakan_hw_state_draw * state,
                                                          uint16_t kcache,
                                                          BITSET_WORD const * resources_opt,
                                                          uint32_t samplers);
void terakan_hw_state_draw_set_sq_constants_needed_by_tes(struct terakan_hw_state_draw * state,
                                                          uint16_t kcache,
                                                          BITSET_WORD const * resources_opt,
                                                          uint32_t samplers, bool next_stage_is_gs);
void terakan_hw_state_draw_set_sq_constants_needed_by_gs(struct terakan_hw_state_draw * state,
                                                         uint16_t kcache,
                                                         BITSET_WORD const * resources_opt,
                                                         uint32_t samplers);
void terakan_hw_state_draw_set_sq_constants_needed_by_fs(struct terakan_hw_state_draw * state,
                                                         uint16_t kcache,
                                                         BITSET_WORD const * resources_opt,
                                                         uint32_t samplers);

void terakan_hw_state_draw_indirect_buffer_begun_and_sq_resources_cleared(
   struct terakan_hw_state_draw * state);

struct terakan_gfx_command_writer;

void terakan_hw_state_draw_emit_modified(struct terakan_gfx_command_writer * command_writer);

void terakan_hw_state_draw_emit_all(struct terakan_gfx_command_writer * command_writer);

void terakan_hw_state_draw_reset(struct terakan_hw_state_draw * state);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_HW_STATE_H */
