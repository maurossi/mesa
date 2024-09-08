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

#ifndef TERAKAN_HW_STATE_H
#define TERAKAN_HW_STATE_H

#include "winsys/terakan_winsys.h"
#include "terakan_descriptor.h"
#include "terakan_limits.h"
#include "terakan_shader.h"

#include "util/bitset.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

struct terakan_hw_state_sq_constant_cache_buffer {
   /* BO and base are undefined if the size is 0. */
   struct terakan_winsys_bo const * bo;
   uint32_t base_cache_lines;
   uint32_t size_cache_lines;
};

enum terakan_hw_state_draw_index {
   /* Generally ordered roughly by the location of the hardware unit in the pipeline, and within
    * each unit, by the register addresses.
    */

   TERAKAN_HW_STATE_DRAW_VGT_INDEX_TYPE,

   TERAKAN_HW_STATE_DRAW_VGT_INDEX_BUFFER,

   TERAKAN_HW_STATE_DRAW_VGT_PRIMITIVE_TYPE,

   TERAKAN_HW_STATE_DRAW_VGT_INDEX_OFFSET,

   TERAKAN_HW_STATE_DRAW_SQ_PGM_VS,
   TERAKAN_HW_STATE_DRAW_SQ_PGM_PS,

   TERAKAN_HW_STATE_DRAW_SQ_VTX_START_INST_LOC,

   TERAKAN_HW_STATE_DRAW_PA_CL_CLIP_CNTL,

   TERAKAN_HW_STATE_DRAW_PA_SU_SC_MODE_CNTL,

   TERAKAN_HW_STATE_DRAW_PA_CL_VTE_CNTL,

   TERAKAN_HW_STATE_DRAW_PA_SC_AA_SAMPLES,

   TERAKAN_HW_STATE_DRAW_PA_SC_AA_MASK,

   TERAKAN_HW_STATE_DRAW_CB_BLEND_RGBA,

   TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST,
   TERAKAN_HW_STATE_DRAW_CB_COLOR_LAST =
      TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + TERAKAN_LIMITS_HW_COLOR_RAT_COUNT - 1,

   /* State items starting from TERAKAN_HW_STATE_DRAW_SPECIAL_START have their modified flags set
    * via some method different from terakan_hw_state_draw_written.
    */
   TERAKAN_HW_STATE_DRAW_SPECIAL_FIRST,

   TERAKAN_HW_STATE_DRAW_SQ_CONSTANT_CACHE_VS = TERAKAN_HW_STATE_DRAW_SPECIAL_FIRST,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANT_CACHE_TCS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANT_CACHE_TES,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANT_CACHE_GS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANT_CACHE_FS,

   TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VI,
   TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VS,
   TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TCS,
   TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TES,
   TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_GS,
   TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_FS,

   TERAKAN_HW_STATE_DRAW_COUNT,
};

extern uint32_t const terakan_standard_sample_locs[5][16 / 4];
extern uint32_t const terakan_standard_sample_max_dists[5];

enum terakan_hw_state_draw_sq_constants_needed_stage {
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,

   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT,
};

enum terakan_hw_state_draw_sq_constants_modified_stage {
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TCS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_GS,
   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_FS,

   TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_COUNT,
};

struct terakan_hw_state_draw_vertex_constant_bits {
   uint16_t constant_cache;

   BITSET_DECLARE(resources, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
};

/* State applied before performing application's or internal draws, and reapplied when switching to
 * a new indirect buffer in the Vulkan command buffer.
 */
struct terakan_hw_state_draw {
   /* Whether each state item has ever been written, and thus has a value that's not complete junk,
    * and is potentially relevant to the current command buffer.
    */
   BITSET_DECLARE(state_ever_written, TERAKAN_HW_STATE_DRAW_COUNT);
   /* Whether each state item has been modified and needs to be emitted before the next draw. */
   BITSET_DECLARE(state_modified, TERAKAN_HW_STATE_DRAW_COUNT);

   /* TERAKAN_HW_STATE_DRAW_VGT_INDEX_TYPE */
   uint32_t vgt_index_type;

   /* TERAKAN_HW_STATE_DRAW_VGT_INDEX_BUFFER */
   struct {
      struct terakan_winsys_bo const * bo;
      uint64_t base;
      /* In units of indices. */
      uint32_t size;
   } vgt_index_buffer;

   /* TERAKAN_HW_STATE_DRAW_VGT_PRIMITIVE_TYPE */
   uint32_t vgt_primitive_type;

   /* TERAKAN_HW_STATE_DRAW_VGT_INDEX_OFFSET */
   uint32_t vgt_index_offset;

   /* TERAKAN_HW_STATE_DRAW_SQ_PGM_VS */
   struct terakan_shader_static const * sq_pgm_vs;

   /* TERAKAN_HW_STATE_DRAW_SQ_PGM_PS */
   struct terakan_shader_static const * sq_pgm_ps;

   /* TERAKAN_HW_STATE_DRAW_SQ_VTX_START_INST_LOC */
   uint32_t sq_vtx_start_inst_loc;

   /* TERAKAN_HW_STATE_DRAW_PA_CL_CLIP_CNTL */
   uint32_t pa_cl_clip_cntl;

   /* TERAKAN_HW_STATE_DRAW_PA_SU_SC_MODE_CNTL */
   uint32_t pa_su_sc_mode_cntl;

   /* TERAKAN_HW_STATE_DRAW_PA_CL_VTE_CNTL */
   uint32_t pa_cl_vte_cntl;

   /* TERAKAN_HW_STATE_DRAW_PA_SC_AA_SAMPLES */
   struct {
      uint32_t num_samples_log2;
   } pa_sc_aa_samples;

   /* TERAKAN_HW_STATE_DRAW_PA_SC_AA_MASK */
   uint16_t pa_sc_aa_mask;

   /* TERAKAN_HW_STATE_DRAW_CB_BLEND_RGBA */
   float cb_blend_rgba[4];

   /* TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST...LAST */
   struct terakan_winsys_bo const * cb_color_bo[TERAKAN_LIMITS_HW_COLOR_RAT_COUNT];
   /* The values are undefined if the respective cb_color_bo is NULL. */
   struct terakan_color_descriptor cb_color[TERAKAN_LIMITS_HW_COLOR_RAT_COUNT];
   struct terakan_color_meta_descriptor cb_color_meta[TERAKAN_LIMITS_HW_COLOR_MRT_COUNT];

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

      uint16_t constant_cache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT];

      struct {
         BITSET_DECLARE(vi, TERAKAN_RESOURCE_HW_COUNT_FETCH);
         BITSET_DECLARE(vs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(tcs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(tes, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(gs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(fs, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE);
      } resources;
   } sq_constants_needed;

   /* If the modified bit for the constant index is set, the new constant is considered to be
    * completely new, and must be emitted for the specific stage next time it's needed.
    * Otherwise, the constant may already be set to the needed value in the hardware, or it might
    * have been set to it previously, but now has been evicted in the VSES constant by the other
    * Vulkan VS or TES stage.
    */
   struct {
      uint16_t constant_cache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_COUNT];

      struct {
         BITSET_DECLARE(vi, TERAKAN_RESOURCE_HW_COUNT_FETCH);
         BITSET_DECLARE(vs_in_vses, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(vs_in_ls, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(tcs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(tes_in_vses, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(gs, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
         BITSET_DECLARE(fs, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE);
      } resources;
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

   struct terakan_hw_state_sq_constant_cache_buffer
      sq_constant_cache_buffers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT]
                               [TERAKAN_CONSTANT_CACHE_BUFFERS_PER_STAGE];

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
      struct terakan_winsys_bo const * vi[TERAKAN_RESOURCE_HW_COUNT_FETCH];
      struct terakan_winsys_bo const * vs[TERAKAN_RESOURCE_HW_COUNT_VERTEX];
      struct terakan_winsys_bo const * tcs[TERAKAN_RESOURCE_HW_COUNT_VERTEX];
      struct terakan_winsys_bo const * tes[TERAKAN_RESOURCE_HW_COUNT_VERTEX];
      struct terakan_winsys_bo const * gs[TERAKAN_RESOURCE_HW_COUNT_VERTEX];
      struct terakan_winsys_bo const * fs[TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE];
   } sq_resource_bos;
   struct {
      uint32_t vi[TERAKAN_RESOURCE_HW_COUNT_FETCH][8];
      uint32_t vs[TERAKAN_RESOURCE_HW_COUNT_VERTEX][8];
      uint32_t tcs[TERAKAN_RESOURCE_HW_COUNT_VERTEX][8];
      uint32_t tes[TERAKAN_RESOURCE_HW_COUNT_VERTEX][8];
      uint32_t gs[TERAKAN_RESOURCE_HW_COUNT_VERTEX][8];
      uint32_t fs[TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE][8];
   } sq_resource_descriptors;
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
   assert(state_index < TERAKAN_HW_STATE_DRAW_SPECIAL_FIRST);
   if (!BITSET_TEST(state->state_ever_written, state_index)) {
      BITSET_SET(state->state_ever_written, state_index);
      modified = true;
   }
   if (modified) {
      BITSET_SET(state->state_modified, state_index);
   }
}

void terakan_hw_state_draw_set_sq_constant_cache_vs(struct terakan_hw_state_draw * state,
                                                    uint32_t buffer_index,
                                                    uint32_t size_cache_lines,
                                                    struct terakan_winsys_bo const * bo,
                                                    uint32_t base_cache_lines);
void terakan_hw_state_draw_set_sq_constant_cache_tcs(struct terakan_hw_state_draw * state,
                                                     uint32_t buffer_index,
                                                     uint32_t size_cache_lines,
                                                     struct terakan_winsys_bo const * bo,
                                                     uint32_t base_cache_lines);
void terakan_hw_state_draw_set_sq_constant_cache_tes(struct terakan_hw_state_draw * state,
                                                     uint32_t buffer_index,
                                                     uint32_t size_cache_lines,
                                                     struct terakan_winsys_bo const * bo,
                                                     uint32_t base_cache_lines);
void terakan_hw_state_draw_set_sq_constant_cache_gs(struct terakan_hw_state_draw * state,
                                                    uint32_t buffer_index,
                                                    uint32_t size_cache_lines,
                                                    struct terakan_winsys_bo const * bo,
                                                    uint32_t base_cache_lines);
void terakan_hw_state_draw_set_sq_constant_cache_fs(struct terakan_hw_state_draw * state,
                                                    uint32_t buffer_index,
                                                    uint32_t size_cache_lines,
                                                    struct terakan_winsys_bo const * bo,
                                                    uint32_t base_cache_lines);

void terakan_hw_state_draw_set_sq_resource_vi(struct terakan_hw_state_draw * state, uint32_t index,
                                              struct terakan_winsys_bo const * bo,
                                              uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_vs(struct terakan_hw_state_draw * state, uint32_t index,
                                              struct terakan_winsys_bo const * bo,
                                              uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_tcs(struct terakan_hw_state_draw * state, uint32_t index,
                                               struct terakan_winsys_bo const * bo,
                                               uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_tes(struct terakan_hw_state_draw * state, uint32_t index,
                                               struct terakan_winsys_bo const * bo,
                                               uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_gs(struct terakan_hw_state_draw * state, uint32_t index,
                                              struct terakan_winsys_bo const * bo,
                                              uint32_t const descriptor[8]);
void terakan_hw_state_draw_set_sq_resource_fs(struct terakan_hw_state_draw * state, uint32_t index,
                                              struct terakan_winsys_bo const * bo,
                                              uint32_t const descriptor[8]);

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
                                                         uint16_t constant_cache,
                                                         BITSET_WORD const * resources_opt,
                                                         VkShaderStageFlags next_stage);
void terakan_hw_state_draw_set_sq_constants_needed_by_tcs(struct terakan_hw_state_draw * state,
                                                          uint16_t constant_cache,
                                                          BITSET_WORD const * resources_opt);
void terakan_hw_state_draw_set_sq_constants_needed_by_tes(struct terakan_hw_state_draw * state,
                                                          uint16_t constant_cache,
                                                          BITSET_WORD const * resources_opt,
                                                          bool next_stage_is_gs);
void terakan_hw_state_draw_set_sq_constants_needed_by_gs(struct terakan_hw_state_draw * state,
                                                         uint16_t constant_cache,
                                                         BITSET_WORD const * resources_opt);
void terakan_hw_state_draw_set_sq_constants_needed_by_fs(struct terakan_hw_state_draw * state,
                                                         uint16_t constant_cache,
                                                         BITSET_WORD const * resources_opt);

void terakan_hw_state_draw_indirect_buffer_begun_and_sq_resources_cleared(
   struct terakan_hw_state_draw * state);

struct terakan_gfx_command_writer;

void terakan_hw_state_draw_emit_modified(struct terakan_gfx_command_writer * command_writer);

void terakan_hw_state_draw_emit_all(struct terakan_gfx_command_writer * command_writer);

void terakan_hw_state_draw_reset(struct terakan_hw_state_draw * state);

#endif /* TERAKAN_HW_STATE_H */
