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

#include "terakan_hw_state.h"

#include "terakan_command_buffer.h"
#include "terakan_physical_device.h"

#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600d_common.h"
#include "util/bitscan.h"
#include "util/macros.h"
#include "vk_device.h"

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define TERAKAN_MAKE_SAMPLE_LOCS(s0x, s0y, s1x, s1y, s2x, s2y, s3x, s3y)                           \
   (((uint32_t)(s0x)&0xF) | (((uint32_t)(s0y)&0xF) << 4) | (((uint32_t)(s1x)&0xF) << 8) |          \
    (((uint32_t)(s1y)&0xF) << 12) | (((uint32_t)(s2x)&0xF) << 16) |                                \
    (((uint32_t)(s2y)&0xF) << 20) | (((uint32_t)(s3x)&0xF) << 24) | (((uint32_t)(s3y)&0xF) << 28))

uint32_t const terakan_standard_sample_locs[5][16 / 4] = {
   {
      TERAKAN_MAKE_SAMPLE_LOCS(0, 0, 0, 0, 0, 0, 0, 0),
   },
   {
      TERAKAN_MAKE_SAMPLE_LOCS(4, 4, -4, -4, 0, 0, 0, 0),
   },
   {
      TERAKAN_MAKE_SAMPLE_LOCS(-2, -6, 6, -2, -6, 2, 2, 6),
   },
   {
      TERAKAN_MAKE_SAMPLE_LOCS(1, -3, -1, 3, 5, 1, -3, -5),
      TERAKAN_MAKE_SAMPLE_LOCS(-5, 5, -7, -1, 3, 7, 7, -7),
   },
   {
      TERAKAN_MAKE_SAMPLE_LOCS(1, 1, -1, -3, -3, 2, 4, -1),
      TERAKAN_MAKE_SAMPLE_LOCS(-5, -2, 2, 5, 5, 3, 3, -5),
      TERAKAN_MAKE_SAMPLE_LOCS(-2, 6, 0, -7, -4, -6, -6, 4),
      TERAKAN_MAKE_SAMPLE_LOCS(-8, 0, 7, -4, 6, 7, -7, -8),
   },
};

uint32_t const terakan_standard_sample_max_dists[5] = {0, 4, 6, 7, 8};

typedef void (*terakan_hw_state_draw_emit_function)(
   struct terakan_gfx_command_writer * command_writer,
   enum terakan_hw_state_draw_index state_index);

static void
terakan_hw_state_draw_emit_vgt_index_type(struct terakan_gfx_command_writer * const command_writer,
                                          UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_INDEX_TYPE, 1 - 1, 0);
   *packet++ = command_writer->hw_state_draw.vgt_index_type;
}

static void
terakan_hw_state_draw_emit_vgt_index_buffer(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 3 + 2, 1, 2, true);
   if (unlikely(packet == NULL)) {
      return;
   }

   *packet++ = PKT3(EG_PKT3_INDEX_BASE, 2 - 1, 0);
   *packet++ = (uint32_t)command_writer->hw_state_draw.vgt_index_buffer.base;
   *packet++ = (uint32_t)(command_writer->hw_state_draw.vgt_index_buffer.base >> 32);
   *packet++ = PKT3(PKT3_NOP, 0, 0);
   *packet++ = terakan_bo_reference_writer_add_reference(
      &command_writer->base.bo_reference_writer, command_writer->hw_state_draw.vgt_index_buffer.bo,
      true, false, TERAKAN_BO_PRIORITY_INDEX_BUFFER);

   *packet++ = PKT3(EG_PKT3_INDEX_BUFFER_SIZE, 1 - 1, 0);
   *packet++ = command_writer->hw_state_draw.vgt_index_buffer.size;
}

static void
terakan_hw_state_draw_emit_vgt_primitive_type(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONFIG_REG, 1, 0);
   *packet++ = TERAKAN_CONFIG_REG_OFFSET(R_008958_VGT_PRIMITIVE_TYPE);
   *packet++ = command_writer->hw_state_draw.vgt_primitive_type;
}

static void
terakan_hw_state_draw_emit_vgt_index_offset(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028408_VGT_INDX_OFFSET);
   *packet++ = command_writer->hw_state_draw.vgt_index_offset;
}

static void
terakan_hw_state_draw_emit_sq_pgm_vs(struct terakan_gfx_command_writer * const command_writer,
                                     UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_shader_static const * const shader = command_writer->hw_state_draw.sq_pgm_vs;

   /* VS_EXPORT_COUNT is the highest parameter export index. */
   uint32_t const spi_vs_out_id_count =
      G_0286C4_VS_EXPORT_COUNT(shader->stage.vs.spi_vs_out_config) / 4 + 1;

   uint32_t const packet_dwords =
      2 + ((R_028864_SQ_PGM_RESOURCES_2_VS - R_02885C_SQ_PGM_START_VS) / sizeof(uint32_t) + 1) +
      /* R_02861C_SPI_VS_OUT_ID_[0-9] */
      2 + spi_vs_out_id_count +
      /* R_0286C4_SPI_VS_OUT_CONFIG */
      2 + 1 +
      /* R_02881C_PA_CL_VS_OUT_CNTL */
      2 + 1;

   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, packet_dwords, 1, 2, true);
   if (unlikely(packet == NULL)) {
      return;
   }

   *packet++ =
      PKT3(PKT3_SET_CONTEXT_REG,
           (R_028864_SQ_PGM_RESOURCES_2_VS - R_02885C_SQ_PGM_START_VS) / sizeof(uint32_t) + 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_02885C_SQ_PGM_START_VS);
   *packet++ = shader->program_start;
   /* TODO(Triang3l): USE_LS_CONSTS. */
   *packet++ = shader->sq_pgm_resources[0];
   *packet++ = shader->sq_pgm_resources[1];
   *packet++ = PKT3(PKT3_NOP, 0, 0);
   *packet++ = terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer,
                                                         shader->program_bo, true, false,
                                                         TERAKAN_BO_PRIORITY_SHADER_BINARY);

   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, spi_vs_out_id_count, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_02861C_SPI_VS_OUT_ID_0);
   memcpy(packet, shader->stage.vs.spi_vs_out_id, sizeof(uint32_t) * spi_vs_out_id_count);
   packet += spi_vs_out_id_count;

   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_0286C4_SPI_VS_OUT_CONFIG);
   *packet++ = shader->stage.vs.spi_vs_out_config;

   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_02881C_PA_CL_VS_OUT_CNTL);
   *packet++ = shader->stage.vs.pa_cl_vs_out_cntl;
}

static void
terakan_hw_state_draw_emit_sq_pgm_ps(struct terakan_gfx_command_writer * const command_writer,
                                     UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_shader_static const * const shader = command_writer->hw_state_draw.sq_pgm_ps;

   uint32_t const interpolator_count = G_0286CC_NUM_INTERP(shader->stage.ps.spi_ps_in_control[0]);

   uint32_t const packet_dwords =
      2 + ((R_02884C_SQ_PGM_EXPORTS_PS - R_028840_SQ_PGM_START_PS) / sizeof(uint32_t) + 1) +
      /* R_028644_SPI_PS_INPUT_CNTL_[0-31] */
      (interpolator_count != 0 ? 2 + interpolator_count : 0) +
      /* R_0286CC_SPI_PS_IN_CONTROL_0, R_0286D0_SPI_PS_IN_CONTROL_1 */
      2 + 2 +
      /* R_0286D8_SPI_INPUT_Z */
      2 + 1 +
      /* R_0286E0_SPI_BARYC_CNTL */
      2 + 1 +
      /* R_02823C_CB_SHADER_MASK */
      2 + 1;

   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, packet_dwords, 1, 2, true);
   if (unlikely(packet == NULL)) {
      return;
   }

   *packet++ =
      PKT3(PKT3_SET_CONTEXT_REG,
           (R_02884C_SQ_PGM_EXPORTS_PS - R_028840_SQ_PGM_START_PS) / sizeof(uint32_t) + 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028840_SQ_PGM_START_PS);
   *packet++ = shader->program_start;
   *packet++ = shader->sq_pgm_resources[0];
   *packet++ = shader->sq_pgm_resources[1];
   *packet++ = shader->stage.ps.sq_pgm_exports_ps;
   *packet++ = PKT3(PKT3_NOP, 0, 0);
   *packet++ = terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer,
                                                         shader->program_bo, true, false,
                                                         TERAKAN_BO_PRIORITY_SHADER_BINARY);

   if (interpolator_count != 0) {
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, interpolator_count, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028644_SPI_PS_INPUT_CNTL_0);
      memcpy(packet, shader->stage.ps.spi_ps_input_cntl, sizeof(uint32_t) * interpolator_count);
      packet += interpolator_count;
   }

   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_0286CC_SPI_PS_IN_CONTROL_0);
   *packet++ = shader->stage.ps.spi_ps_in_control[0];
   *packet++ = shader->stage.ps.spi_ps_in_control[1];

   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_0286D8_SPI_INPUT_Z);
   *packet++ = shader->stage.ps.spi_input_z;

   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_0286E0_SPI_BARYC_CNTL);
   *packet++ = shader->stage.ps.spi_baryc_cntl;

   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_02823C_CB_SHADER_MASK);
   *packet++ = shader->stage.ps.cb_shader_mask;
}

static void
terakan_hw_state_draw_emit_sq_vtx_start_inst_loc(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CTL_CONST, 1, 0);
   *packet++ = TERAKAN_CTL_CONST_OFFSET(R_03CFF4_SQ_VTX_START_INST_LOC);
   *packet++ = command_writer->hw_state_draw.sq_vtx_start_inst_loc;
}

static void
terakan_hw_state_draw_emit_pa_cl_clip_cntl(struct terakan_gfx_command_writer * const command_writer,
                                           UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028810_PA_CL_CLIP_CNTL);
   *packet++ = command_writer->hw_state_draw.pa_cl_clip_cntl;
}

static void
terakan_hw_state_draw_emit_pa_su_sc_mode_cntl(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028814_PA_SU_SC_MODE_CNTL);
   *packet++ = command_writer->hw_state_draw.pa_su_sc_mode_cntl;
}

static void
terakan_hw_state_draw_emit_pa_cl_vte_cntl(struct terakan_gfx_command_writer * const command_writer,
                                          UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028818_PA_CL_VTE_CNTL);
   *packet++ = command_writer->hw_state_draw.pa_cl_vte_cntl;
}

static void
terakan_hw_state_draw_emit_pa_sc_mode_cntl_0(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028A48_PA_SC_MODE_CNTL_0);
   *packet++ = command_writer->hw_state_draw.pa_sc_mode_cntl_0;
}

static void
terakan_hw_state_draw_emit_pa_cl_gb(struct terakan_gfx_command_writer * const command_writer,
                                    UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 4, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   /* According to the Gallium R600 and RadeonSI drivers, if any guard band register is changed, all
    * must be emitted.
    */
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 4, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(
      container_of(command_writer->base.command_buffer->vk.base.device->physical,
                   struct terakan_physical_device const, vk)
            ->chip_family_info.is_r9xx
         ? CM_R_028BE8_PA_CL_GB_VERT_CLIP_ADJ
         : R_028C0C_PA_CL_GB_VERT_CLIP_ADJ);
   memcpy(packet, command_writer->hw_state_draw.pa_cl_gb_vert_horz_clip_disc_adj,
          sizeof(float) * 4);
}

static void
terakan_hw_state_draw_emit_pa_sc_aa_samples(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_hw_state_draw_index const state_index)
{
   bool const is_r9xx = container_of(command_writer->base.command_buffer->vk.base.device->physical,
                                     struct terakan_physical_device const, vk)
                           ->chip_family_info.is_r9xx;

   uint32_t const num_samples_log2 =
      command_writer->hw_state_draw.pa_sc_aa_samples.num_samples_log2;
   assert(num_samples_log2 <= (is_r9xx ? 4 : 3));

   uint32_t pa_sc_aa_config =
      S_028BE0_MSAA_NUM_SAMPLES(num_samples_log2) |
      S_028BE0_MAX_SAMPLE_DIST(terakan_standard_sample_max_dists[num_samples_log2]);

   uint32_t const num_sample_loc_dwords = (((uint32_t)1 << num_samples_log2) + 3) / 4;
   uint32_t const * const sample_locs = terakan_standard_sample_locs[num_samples_log2];

   if (is_r9xx) {
      pa_sc_aa_config |= S_028BE0_MSAA_EXPOSED_SAMPLES(num_samples_log2);

      uint32_t * packet = terakan_gfx_command_writer_emit(
         command_writer, (2 + 2) + (2 + 1) + (2 + num_sample_loc_dwords) * 4, 0, 0, true);
      if (unlikely(packet == NULL)) {
         return;
      }

      uint64_t const standard_centroid_priorities[] = {
         UINT64_C(0x0000000000000000), UINT64_C(0x1010101010101010), UINT64_C(0x3210321032103210),
         UINT64_C(0x7654321076543210), UINT64_C(0xFEDCBA9876543210),
      };
      uint64_t const centroid_priority = standard_centroid_priorities[num_samples_log2];
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_028BD4_PA_SC_CENTROID_PRIORITY_0);
      *packet++ = (uint32_t)centroid_priority;
      *packet++ = (uint32_t)(centroid_priority >> 32);

      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_028BE0_PA_SC_AA_CONFIG);
      *packet++ = pa_sc_aa_config;

      /* Pixels in the quad have a constant stride of 4 dwords. */
      for (uint32_t quad_pixel_index = 0; quad_pixel_index < 4; ++quad_pixel_index) {
         *packet++ = PKT3(PKT3_SET_CONTEXT_REG, num_sample_loc_dwords, 0);
         *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_028BF8_PA_SC_AA_SAMPLE_LOCS_PIXEL_X0Y0_0 +
                                                sizeof(uint32_t) * 4 * quad_pixel_index);
         memcpy(packet, sample_locs, sizeof(uint32_t) * num_sample_loc_dwords);
         packet += num_sample_loc_dwords;
      }
   } else {
      uint32_t * packet = terakan_gfx_command_writer_emit(
         command_writer, (2 + 1) + (2 + num_sample_loc_dwords * 4), 0, 0, true);
      if (unlikely(packet == NULL)) {
         return;
      }

      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028C04_PA_SC_AA_CONFIG);
      *packet++ = pa_sc_aa_config;

      /* Pixels in the quad have a stride of 1 dword at 1x/2x/4x, 2 dwords at 8x. */
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 4 * num_sample_loc_dwords, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028C1C_PA_SC_AA_SAMPLE_LOCS_0);
      for (uint32_t quad_pixel_index = 0; quad_pixel_index < 4; ++quad_pixel_index) {
         memcpy(packet, sample_locs, sizeof(uint32_t) * num_sample_loc_dwords);
         packet += num_sample_loc_dwords;
      }
   }
}

static void
terakan_hw_state_draw_emit_pa_sc_aa_mask(struct terakan_gfx_command_writer * const command_writer,
                                         UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t aa_mask = command_writer->hw_state_draw.pa_sc_aa_mask;
   aa_mask |= aa_mask << 16;

   if (container_of(command_writer->base.command_buffer->vk.base.device->physical,
                    struct terakan_physical_device const, vk)
          ->chip_family_info.is_r9xx) {
      uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 2, 0, 0, true);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_028C38_PA_SC_AA_MASK_X0Y0_X1Y0);
      *packet++ = aa_mask;
      *packet++ = aa_mask;
   } else {
      aa_mask &= (((uint32_t)1 << 8) - 1) | ((((uint32_t)1 << 8) - 1) << 16);
      aa_mask |= aa_mask << 8;

      uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028C3C_PA_SC_AA_MASK);
      *packet++ = aa_mask;
   }
}

static void
terakan_hw_state_draw_emit_db_render_override(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_02800C_DB_RENDER_OVERRIDE);
   *packet++ = command_writer->hw_state_draw.db_render_override;
}

static void
terakan_hw_state_draw_emit_cb_blend_rgba(struct terakan_gfx_command_writer * const command_writer,
                                         UNUSED enum terakan_hw_state_draw_index const state_index)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 4, 0, 0, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 4, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028414_CB_BLEND_RED);
   memcpy(packet, command_writer->hw_state_draw.cb_blend_rgba, sizeof(float) * 4);
}

static void
terakan_hw_state_draw_emit_cb_color(struct terakan_gfx_command_writer * const command_writer,
                                    enum terakan_hw_state_draw_index const state_index)
{
   uint32_t const color_index =
      (uint32_t)state_index - (uint32_t)TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST;
   uint32_t const color_register_offset =
      (R_028C9C_CB_COLOR1_BASE - R_028C60_CB_COLOR0_BASE) * color_index;

   struct terakan_bo const * const color_bo =
      command_writer->hw_state_draw.cb_color_bo[color_index];
   struct terakan_color_descriptor const * const color_descriptor =
      &command_writer->hw_state_draw.cb_color[color_index];
   if (color_bo != NULL && G_028C70_FORMAT(color_descriptor->info) != V_028C70_COLOR_INVALID) {
      uint32_t const cb_color_descriptor_dwords =
         sizeof(struct terakan_color_descriptor) / sizeof(uint32_t);
      uint32_t const cb_color_meta_descriptor_dwords =
         sizeof(struct terakan_color_meta_descriptor) / sizeof(uint32_t);

      /* Relocations needed for:
       * R_028C60_CB_COLOR0_BASE
       * R_028C74_CB_COLOR0_ATTRIB
       * R_028C7C_CB_COLOR0_CMASK
       * R_028C84_CB_COLOR0_FMASK
       */
      uint32_t const cb_color_relocation_count = 4;

      uint32_t * packet = terakan_gfx_command_writer_emit(
         command_writer, 2 + cb_color_descriptor_dwords + cb_color_meta_descriptor_dwords, 1,
         2 * cb_color_relocation_count, true);
      if (unlikely(packet == NULL)) {
         return;
      }

      *packet++ = PKT3(PKT3_SET_CONTEXT_REG,
                       cb_color_descriptor_dwords + cb_color_meta_descriptor_dwords, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028C60_CB_COLOR0_BASE + color_register_offset);

      /* TODO(Triang3l): Higher priority for multisampled color buffers (possibly pass the sample
       * count via the view not only on R9xx, but on R8xx too, but mask it away here on R8xx - using
       * the presence of FMask for this purpose is possibly more complicated and not always
       * reliable).
       */
      uint32_t const color_bo_reference = terakan_bo_reference_writer_add_reference(
         &command_writer->base.bo_reference_writer, color_bo, true, true,
         G_028C70_RAT(color_descriptor->info) ? TERAKAN_BO_PRIORITY_SHADER_RW_IMAGE
                                              : TERAKAN_BO_PRIORITY_COLOR_BUFFER);

      memcpy(packet, color_descriptor, sizeof(*color_descriptor));
      packet += sizeof(*color_descriptor) / sizeof(uint32_t);

      struct terakan_color_meta_descriptor const * const color_meta_descriptor =
         &command_writer->hw_state_draw.cb_color_meta[color_index];
      memcpy(packet, color_meta_descriptor, sizeof(*color_meta_descriptor));
      packet += sizeof(*color_meta_descriptor) / sizeof(uint32_t);

      for (uint32_t cb_color_relocation_index = 0;
           cb_color_relocation_index < cb_color_relocation_count; ++cb_color_relocation_index) {
         *packet++ = PKT3(PKT3_NOP, 0, 0);
         *packet++ = color_bo_reference;
      }
   } else {
      /* Set the format to invalid, not requiring any relocations. */
      uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028C70_CB_COLOR0_INFO + color_register_offset);
      *packet++ = 0;
   }
}

static void
terakan_hw_state_draw_emit_cb_color_rat_only(
   struct terakan_gfx_command_writer * const command_writer,
   enum terakan_hw_state_draw_index const state_index)
{
   uint32_t const color_rat_only_index =
      (uint32_t)state_index - ((uint32_t)TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 8);
   uint32_t const color_register_offset =
      (R_028E5C_CB_COLOR9_BASE - R_028E40_CB_COLOR8_BASE) * color_rat_only_index;

   struct terakan_bo const * const color_bo =
      command_writer->hw_state_draw.cb_color_bo[8 + color_rat_only_index];
   struct terakan_color_descriptor const * const color_descriptor =
      &command_writer->hw_state_draw.cb_color[8 + color_rat_only_index];
   if (color_bo != NULL && G_028C70_FORMAT(color_descriptor->info) != V_028C70_COLOR_INVALID) {
      uint32_t const cb_color_descriptor_dwords =
         sizeof(struct terakan_color_descriptor) / sizeof(uint32_t);

      /* Relocations needed for:
       * R_028E40_CB_COLOR8_BASE
       * R_028E54_CB_COLOR8_ATTRIB
       */
      uint32_t const cb_color_relocation_count = 2;

      uint32_t * packet = terakan_gfx_command_writer_emit(
         command_writer, 2 + cb_color_descriptor_dwords, 1, 2 * cb_color_relocation_count, true);
      if (unlikely(packet == NULL)) {
         return;
      }

      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, cb_color_descriptor_dwords, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028E40_CB_COLOR8_BASE + color_register_offset);

      uint32_t const color_bo_reference = terakan_bo_reference_writer_add_reference(
         &command_writer->base.bo_reference_writer, color_bo, true, true,
         TERAKAN_BO_PRIORITY_SHADER_RW_IMAGE);

      memcpy(packet, color_descriptor, sizeof(*color_descriptor));
      packet += sizeof(*color_descriptor) / sizeof(uint32_t);

      for (uint32_t cb_color_relocation_index = 0;
           cb_color_relocation_index < cb_color_relocation_count; ++cb_color_relocation_index) {
         *packet++ = PKT3(PKT3_NOP, 0, 0);
         *packet++ = color_bo_reference;
      }
   } else {
      /* Set the format to invalid, not requiring any relocations. */
      uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028E50_CB_COLOR8_INFO + color_register_offset);
      *packet++ = 0;
   }
}

static void
terakan_hw_state_draw_emit_viewport(struct terakan_gfx_command_writer * const command_writer,
                                    UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   uint32_t * packet;

   while (state->viewports_modified) {
      uint32_t const viewport_index = (uint32_t)ffs((int)state->viewports_modified) - 1;
      assert(viewport_index < state->viewport_count_ever_written);
      struct terakan_hw_state_draw_viewport * const viewport = &state->viewports[viewport_index];

      bool const scale_offset_xy_modified = BITSET_TEST(
         viewport->state_modified, TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_CL_VPORT_XY_SCALE_OFFSET);
      bool const scale_offset_z_modified = BITSET_TEST(
         viewport->state_modified, TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_CL_VPORT_Z_SCALE_OFFSET);
      if (scale_offset_xy_modified || scale_offset_z_modified) {
         uint32_t const scale_offset_emit_dwords =
            (scale_offset_xy_modified ? 4 : 0) + (scale_offset_z_modified ? 2 : 0);
         packet = terakan_gfx_command_writer_emit(command_writer, 2 + scale_offset_emit_dwords, 0,
                                                  0, true);
         if (unlikely(packet == NULL)) {
            return;
         }
         *packet++ = PKT3(PKT3_SET_CONTEXT_REG, scale_offset_emit_dwords, 0);
         *packet++ =
            TERAKAN_CONTEXT_REG_OFFSET(scale_offset_xy_modified ? R_02843C_PA_CL_VPORT_XSCALE_0
                                                                : R_02844C_PA_CL_VPORT_ZSCALE_0) +
            6 * viewport_index;
         if (scale_offset_xy_modified) {
            memcpy(packet, viewport->pa_cl_vport_xy_scale_offset, sizeof(float) * 4);
            packet += 4;
         }
         if (scale_offset_z_modified) {
            memcpy(packet, viewport->pa_cl_vport_z_scale_offset, sizeof(float) * 2);
            packet += 2;
         }
      }

      if (BITSET_TEST(viewport->state_modified,
                      TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_SC_VPORT_SCISSOR)) {
         packet = terakan_gfx_command_writer_emit(command_writer, 2 + 2, 0, 0, true);
         if (unlikely(packet == NULL)) {
            return;
         }
         *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
         *packet++ =
            TERAKAN_CONTEXT_REG_OFFSET(R_028250_PA_SC_VPORT_SCISSOR_0_TL) + 2 * viewport_index;
         memcpy(packet, viewport->pa_sc_vport_scissor, sizeof(uint32_t) * 2);
         packet += 2;
      }

      if (BITSET_TEST(viewport->state_modified,
                      TERAKAN_HW_STATE_DRAW_VIEWPORT_PA_SC_VPORT_Z_MIN_MAX)) {
         packet = terakan_gfx_command_writer_emit(command_writer, 2 + 2, 0, 0, true);
         if (unlikely(packet == NULL)) {
            return;
         }
         *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
         *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_0282D0_PA_SC_VPORT_ZMIN_0) + 2 * viewport_index;
         memcpy(packet, viewport->pa_sc_vport_z_min_max, sizeof(float) * 2);
         packet += 2;
      }

      /* Keep state_modified of each viewport zero if the bit isn't set in viewports_modified so
       * terakan_hw_state_draw_viewport_modified doesn't have to check if the bit in
       * viewports_modified isn't set and clear state_modified.
       */
      BITSET_ZERO(viewport->state_modified);
      state->viewports_modified &= ~((uint16_t)1 << viewport_index);
   }
}

/* TODO(Triang3l): Fast paths for R9xx USE_LS_CONSTS and for R8xx when not tessellation is not
 * enabled on the device.
 */

/* Returns whether the setting was actually emitted, if false, it might have been aborted, for
 * instance, due to a new indirect buffer being started and all state having been re-emitted.
 */
static bool
terakan_hw_state_draw_emit_sq_kcache_buffer(
   struct terakan_gfx_command_writer * const command_writer, uint32_t const size_register_offset,
   uint32_t const base_register_offset,
   struct terakan_hw_state_sq_kcache_buffer const * const buffer)
{
   /* Both size and base must be emitted together if size is not 0 according to the register
    * reference.
    */
   bool const not_empty = buffer->size_lines != 0;

   uint32_t * packet = terakan_gfx_command_writer_emit(
      command_writer, 2 + 1 + (not_empty ? 2 + 1 : 0), not_empty ? 1 : 0, not_empty ? 2 : 0, true);
   if (unlikely(packet == NULL)) {
      return false;
   }

   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = size_register_offset;
   *packet++ = buffer->size_lines;

   if (not_empty) {
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = base_register_offset;
      *packet++ = buffer->base_lines;
      *packet++ = PKT3(PKT3_NOP, 0, 0);
      *packet++ = terakan_bo_reference_writer_add_reference(
         &command_writer->base.bo_reference_writer, buffer->bo, true, false,
         TERAKAN_BO_PRIORITY_UNIFORM_BUFFER);
   }

   return true;
}

static void
terakan_hw_state_draw_emit_sq_kcache_for_stage(
   struct terakan_gfx_command_writer * const command_writer, uint32_t const size_register_offset,
   uint32_t const base_register_offset,
   enum terakan_hw_state_draw_sq_constants_needed_stage const needed_stage,
   enum terakan_hw_state_draw_sq_constants_modified_stage const modified_stage)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   uint16_t * const buffers_modified = &state->sq_constants_modified.kcache[modified_stage];

   uint16_t buffers_remaining = *buffers_modified & state->sq_constants_needed.kcache[needed_stage];

   int buffer_index;
   while ((buffer_index = ffs(buffers_remaining) - 1) >= 0) {
      uint16_t const buffer_bit = (uint16_t)1 << buffer_index;

      buffers_remaining &= ~buffer_bit;

      if (!terakan_hw_state_draw_emit_sq_kcache_buffer(
             command_writer, size_register_offset + buffer_index,
             base_register_offset + buffer_index,
             &state->sq_kcache_buffers[needed_stage][buffer_index])) {
         return;
      }
      *buffers_modified &= ~buffer_bit;
   }
}

static void
terakan_hw_state_draw_emit_sq_kcache_vs(struct terakan_gfx_command_writer * const command_writer,
                                        UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (state->sq_constants_needed.tcs_tes) {
      terakan_hw_state_draw_emit_sq_kcache_for_stage(
         command_writer, TERAKAN_CONTEXT_REG_OFFSET(R_028FC0_ALU_CONST_BUFFER_SIZE_LS_0),
         TERAKAN_CONTEXT_REG_OFFSET(R_028F40_ALU_CONST_CACHE_LS_0),
         TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
         TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS);
      return;
   }

   /* TODO(Triang3l): Fast path for tessellation not enabled on the device. */

   uint16_t * const buffers_modified =
      &state->sq_constants_modified
          .kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES];

   uint16_t buffers_remaining =
      (*buffers_modified | state->sq_constants_for_vs_overwritten_in_vses_by_tes.kcache) &
      state->sq_constants_needed.kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS];

   int buffer_index;
   while ((buffer_index = ffs(buffers_remaining) - 1) >= 0) {
      uint16_t const buffer_bit = (uint16_t)1 << buffer_index;

      buffers_remaining &= ~buffer_bit;

      bool emit_buffer = false;

      struct terakan_hw_state_sq_kcache_buffer const * const buffer =
         &state->sq_kcache_buffers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS][buffer_index];

      if (*buffers_modified & buffer_bit) {
         /* Completely new buffer needed for the Vulkan VS stage. */
         emit_buffer = true;
         *buffers_modified &= ~buffer_bit;
         /* Next time the hardware binding is needed for TES, make sure the TES one is applied.
          * Simply mark as overwritten for now to avoid overhead within non-tesellated draws, defer
          * all more complex logic until tessellation is enabled.
          */
         state->sq_constants_for_tes_overwritten_in_vses_by_vs.kcache |= buffer_bit;
      } else {
         /* The previous VS buffer is still up to date, but VSES bindings now include the TES
          * binding instead, which may be different, or the same.
          */
         struct terakan_hw_state_sq_kcache_buffer const * const tes_buffer =
            &state->sq_kcache_buffers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES]
                                     [buffer_index];
         if (buffer->size_lines != tes_buffer->size_lines ||
             (buffer->size_lines &&
              (buffer->bo != tes_buffer->bo || buffer->base_lines != tes_buffer->base_lines))) {
            /* VS and TES buffers for this binding are different, emit the VS buffer now, and emit
             * the TES buffer next time it's needed.
             */
            emit_buffer = true;
            state->sq_constants_for_tes_overwritten_in_vses_by_vs.kcache |= buffer_bit;
         } else {
            state->sq_constants_for_tes_overwritten_in_vses_by_vs.kcache &= ~buffer_bit;
         }
      }

      if (emit_buffer &&
          !terakan_hw_state_draw_emit_sq_kcache_buffer(
             command_writer,
             TERAKAN_CONTEXT_REG_OFFSET(R_028180_ALU_CONST_BUFFER_SIZE_VS_0) + buffer_index,
             TERAKAN_CONTEXT_REG_OFFSET(R_028980_ALU_CONST_CACHE_VS_0) + buffer_index, buffer)) {
         return;
      }

      state->sq_constants_for_vs_overwritten_in_vses_by_tes.kcache &= ~buffer_bit;
   }
}

static void
terakan_hw_state_draw_emit_sq_kcache_tcs(struct terakan_gfx_command_writer * const command_writer,
                                         UNUSED enum terakan_hw_state_draw_index const state_index)
{
   if (!command_writer->hw_state_draw.sq_constants_needed.tcs_tes) {
      return;
   }
   terakan_hw_state_draw_emit_sq_kcache_for_stage(
      command_writer, TERAKAN_CONTEXT_REG_OFFSET(R_028F80_ALU_CONST_BUFFER_SIZE_HS_0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028F00_ALU_CONST_CACHE_HS_0),
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TCS);
}

static void
terakan_hw_state_draw_emit_sq_kcache_tes(struct terakan_gfx_command_writer * const command_writer,
                                         UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (!state->sq_constants_needed.tcs_tes) {
      return;
   }

   /* TODO(Triang3l): Fast path for R9xx USE_LS_CONSTS. */

   uint16_t * const buffers_modified =
      &state->sq_constants_modified
          .kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES];

   uint16_t buffers_remaining =
      (*buffers_modified | state->sq_constants_for_tes_overwritten_in_vses_by_vs.kcache) &
      state->sq_constants_needed.kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES];

   int buffer_index;
   while ((buffer_index = ffs(buffers_remaining) - 1) >= 0) {
      uint16_t const buffer_bit = (uint16_t)1 << buffer_index;

      buffers_remaining &= ~buffer_bit;

      bool emit_buffer = false;

      struct terakan_hw_state_sq_kcache_buffer const * const buffer =
         &state
             ->sq_kcache_buffers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES][buffer_index];

      if (*buffers_modified & buffer_bit) {
         /* Completely new buffer needed for the Vulkan TES stage. */
         emit_buffer = true;
         *buffers_modified &= ~buffer_bit;
         /* Next time the hardware binding is needed for VS, make sure the VS one is applied.
          * Simply mark as overwritten for now to avoid overhead within tesellated draws, defer all
          * more complex logic until tessellation is disabled.
          */
         state->sq_constants_for_vs_overwritten_in_vses_by_tes.kcache |= buffer_bit;
      } else {
         /* The previous TES buffer is still up to date, but VSES bindings now include the VS
          * binding instead, which may be different, or the same.
          */
         struct terakan_hw_state_sq_kcache_buffer const * const vs_buffer =
            &state->sq_kcache_buffers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS]
                                     [buffer_index];
         if (buffer->size_lines != vs_buffer->size_lines ||
             (buffer->size_lines &&
              (buffer->bo != vs_buffer->bo || buffer->base_lines != vs_buffer->base_lines))) {
            /* VS and TES buffers for this binding are different, emit the TES buffer now, and emit
             * the VS buffer next time it's needed.
             */
            emit_buffer = true;
            state->sq_constants_for_vs_overwritten_in_vses_by_tes.kcache |= buffer_bit;
         } else {
            state->sq_constants_for_vs_overwritten_in_vses_by_tes.kcache &= ~buffer_bit;
         }
      }

      if (emit_buffer &&
          !terakan_hw_state_draw_emit_sq_kcache_buffer(
             command_writer,
             TERAKAN_CONTEXT_REG_OFFSET(R_028180_ALU_CONST_BUFFER_SIZE_VS_0) + buffer_index,
             TERAKAN_CONTEXT_REG_OFFSET(R_028980_ALU_CONST_CACHE_VS_0) + buffer_index, buffer)) {
         return;
      }

      state->sq_constants_for_tes_overwritten_in_vses_by_vs.kcache &= ~buffer_bit;
   }
}

static void
terakan_hw_state_draw_emit_sq_kcache_gs(struct terakan_gfx_command_writer * const command_writer,
                                        UNUSED enum terakan_hw_state_draw_index const state_index)
{
   if (!terakan_hw_state_draw_sq_constants_needed_by_gs(&command_writer->hw_state_draw)) {
      return;
   }
   terakan_hw_state_draw_emit_sq_kcache_for_stage(
      command_writer, TERAKAN_CONTEXT_REG_OFFSET(R_0281C0_ALU_CONST_BUFFER_SIZE_GS_0),
      TERAKAN_CONTEXT_REG_OFFSET(R_0289C0_ALU_CONST_CACHE_GS_0),
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_GS);
}

static void
terakan_hw_state_draw_emit_sq_kcache_fs(struct terakan_gfx_command_writer * const command_writer,
                                        UNUSED enum terakan_hw_state_draw_index const state_index)
{
   terakan_hw_state_draw_emit_sq_kcache_for_stage(
      command_writer, TERAKAN_CONTEXT_REG_OFFSET(R_028140_ALU_CONST_BUFFER_SIZE_PS_0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028940_ALU_CONST_CACHE_PS_0),
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_FS);
}

/* bo and descriptor are undefined if not_null == false.
 * Returns whether the setting was actually emitted, if false, it might have been aborted, for
 * instance, due to a new indirect buffer being started and all state having been re-emitted.
 */
static bool
terakan_hw_state_draw_emit_resource(struct terakan_gfx_command_writer * const command_writer,
                                    uint32_t const global_index, bool const not_null,
                                    struct terakan_bo const * const bo,
                                    uint32_t const descriptor[8])
{
   uint32_t * packet;

   if (!not_null) {
      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, true);
      if (unlikely(packet == NULL)) {
         return false;
      }
      *packet++ = PKT3(PKT3_SET_CTL_CONST, 1, 0);
      *packet++ = TERAKAN_CTL_CONST_OFFSET(R_03FF04_SQ_TEX_RESOURCE_CLEAR);
      *packet++ = global_index;
      return true;
   }

   assert(bo != NULL);

   bool const is_texture = G_03001C_TYPE(descriptor[7]) == V_03001C_SQ_TEX_VTX_VALID_TEXTURE;
   bool const is_multisampled =
      is_texture && (G_030000_DIM(descriptor[0]) == V_030000_SQ_TEX_DIM_2D_MSAA ||
                     G_030000_DIM(descriptor[0]) == V_030000_SQ_TEX_DIM_2D_ARRAY_MSAA);
   bool const relocate_mips_or_fmask =
      is_texture && (!is_multisampled || G_03000C_MIP_ADDRESS(descriptor[3]) != 0);

   packet = terakan_gfx_command_writer_emit(command_writer, 2 + 8, 1,
                                            relocate_mips_or_fmask ? 4 : 2, true);
   if (unlikely(packet == NULL)) {
      return false;
   }

   *packet++ = PKT3(PKT3_SET_RESOURCE, 8, 0);
   *packet++ = 8 * global_index;
   memcpy(packet, descriptor, sizeof(uint32_t) * 8);
   if (!is_texture) {
      packet[TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD] = 0;
   }
   packet += 8;

   uint32_t const bo_reference = terakan_bo_reference_writer_add_reference(
      &command_writer->base.bo_reference_writer, bo, true, false,
      is_texture ? (is_multisampled ? TERAKAN_BO_PRIORITY_SHADER_READ_IMAGE_MS
                                    : TERAKAN_BO_PRIORITY_SHADER_READ_IMAGE)
                 : descriptor[TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD]);
   *packet++ = PKT3(PKT3_NOP, 0, 0);
   *packet++ = bo_reference;
   if (relocate_mips_or_fmask) {
      *packet++ = PKT3(PKT3_NOP, 0, 0);
      *packet++ = bo_reference;
   }

   return true;
}

static void
terakan_hw_state_draw_emit_sq_resources_for_stage(
   struct terakan_gfx_command_writer * const command_writer, uint32_t const global_offset,
   uint32_t const count, BITSET_WORD const * const not_null_bitset,
   struct terakan_bo const * const * const bos, uint32_t const * const descriptors,
   BITSET_WORD const * const needed_bitset, BITSET_WORD * const modified_bitset)
{
   unsigned const word_count = BITSET_WORDS(count);

   BITSET_DECLARE(resources_needed, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE);
   for (unsigned word_index = 0; word_index < word_count; ++word_index) {
      resources_needed[word_index] = modified_bitset[word_index] & needed_bitset[word_index];
   }

   unsigned resource_index;
   BITSET_FOREACH_SET (resource_index, resources_needed, count) {
      if (!terakan_hw_state_draw_emit_resource(command_writer, global_offset + resource_index,
                                               BITSET_TEST(not_null_bitset, resource_index),
                                               bos[resource_index],
                                               descriptors + 8 * resource_index)) {
         return;
      }
      BITSET_CLEAR(modified_bitset, resource_index);
   }
}

static void
terakan_hw_state_draw_emit_sq_resources_vi(struct terakan_gfx_command_writer * const command_writer,
                                           UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;
   terakan_hw_state_draw_emit_sq_resources_for_stage(
      command_writer, TERAKAN_RESOURCE_HW_OFFSET_FS, TERAKAN_RESOURCE_HW_COUNT_FETCH,
      state->sq_resources_not_null.vi, state->sq_resource_bos.vi,
      state->sq_resource_descriptors.vi[0], state->sq_constants_needed.resources.vi,
      state->sq_constants_modified.resources.vi);
}

static void
terakan_hw_state_draw_emit_sq_resources_vs(struct terakan_gfx_command_writer * const command_writer,
                                           UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (state->sq_constants_needed.tcs_tes) {
      terakan_hw_state_draw_emit_sq_resources_for_stage(
         command_writer, TERAKAN_RESOURCE_HW_OFFSET_LS, TERAKAN_RESOURCE_HW_COUNT_VERTEX,
         state->sq_resources_not_null.vs, state->sq_resource_bos.vs,
         state->sq_resource_descriptors.vs[0], state->sq_constants_needed.resources.vs,
         state->sq_constants_modified.resources.vs_in_ls);
      return;
   }

   /* TODO(Triang3l): Fast path for tessellation not enabled on the device. */

   BITSET_DECLARE(resources_needed, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
   for (unsigned word_index = 0; word_index < BITSET_WORDS(TERAKAN_RESOURCE_HW_COUNT_VERTEX);
        ++word_index) {
      resources_needed[word_index] =
         (state->sq_constants_modified.resources.vs_in_vses[word_index] |
          state->sq_constants_for_vs_overwritten_in_vses_by_tes.resources[word_index]) &
         state->sq_constants_needed.resources.vs[word_index];
   }

   unsigned resource_index;
   BITSET_FOREACH_SET (resource_index, resources_needed, TERAKAN_RESOURCE_HW_COUNT_VERTEX) {
      bool emit_resource = false;

      bool const resource_not_null = BITSET_TEST(state->sq_resources_not_null.vs, resource_index);

      if (BITSET_TEST(state->sq_constants_modified.resources.vs_in_vses, resource_index)) {
         /* Completely new resource needed for the Vulkan VS stage. */
         emit_resource = true;
         BITSET_CLEAR(state->sq_constants_modified.resources.vs_in_vses, resource_index);
         /* Next time the hardware binding is needed for TES, make sure the TES one is applied.
          * Simply mark as overwritten for now to avoid overhead within non-tesellated draws, defer
          * all more complex logic until tessellation is enabled.
          */
         BITSET_SET(state->sq_constants_for_tes_overwritten_in_vses_by_vs.resources,
                    resource_index);
      } else {
         /* The previous VS resource is still up to date, but VSES bindings now include the TES
          * binding instead, which may be different, or the same (VS | TES bindings in the
          * beginning of the pipeline layout, may happen in applications having a common descriptor
          * set referenced by all shaders).
          */
         if ((resource_not_null != BITSET_TEST(state->sq_resources_not_null.tes, resource_index)) ||
             (resource_not_null && (state->sq_resource_bos.vs[resource_index] !=
                                       state->sq_resource_bos.tes[resource_index] ||
                                    memcmp(state->sq_resource_descriptors.vs[resource_index],
                                           state->sq_resource_descriptors.tes[resource_index],
                                           sizeof(uint32_t) * 8) != 0))) {
            /* VS and TES resources for this binding are different, emit the VS resource now, and
             * emit the TES resource next time it's needed.
             */
            emit_resource = true;
            BITSET_SET(state->sq_constants_for_tes_overwritten_in_vses_by_vs.resources,
                       resource_index);
         } else {
            /* VS and TES resources at this index are the same, continue using the resource for both
             * VS for this emission and TES later.
             */
            BITSET_CLEAR(state->sq_constants_for_tes_overwritten_in_vses_by_vs.resources,
                         resource_index);
         }
      }

      if (emit_resource && !terakan_hw_state_draw_emit_resource(
                              command_writer, TERAKAN_RESOURCE_HW_OFFSET_VSES + resource_index,
                              resource_not_null, state->sq_resource_bos.vs[resource_index],
                              state->sq_resource_descriptors.vs[resource_index])) {
         return;
      }

      BITSET_CLEAR(state->sq_constants_for_vs_overwritten_in_vses_by_tes.resources, resource_index);
   }
}

static void
terakan_hw_state_draw_emit_sq_resources_tcs(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;
   if (!state->sq_constants_needed.tcs_tes) {
      return;
   }
   terakan_hw_state_draw_emit_sq_resources_for_stage(
      command_writer, TERAKAN_RESOURCE_HW_OFFSET_HS, TERAKAN_RESOURCE_HW_COUNT_VERTEX,
      state->sq_resources_not_null.tcs, state->sq_resource_bos.tcs,
      state->sq_resource_descriptors.tcs[0], state->sq_constants_needed.resources.tcs,
      state->sq_constants_modified.resources.tcs);
}

static void
terakan_hw_state_draw_emit_sq_resources_tes(
   struct terakan_gfx_command_writer * const command_writer,
   UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (!state->sq_constants_needed.tcs_tes) {
      return;
   }

   /* TODO(Triang3l): Fast path for R9xx USE_LS_CONSTS. */

   BITSET_DECLARE(resources_needed, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
   for (unsigned word_index = 0; word_index < BITSET_WORDS(TERAKAN_RESOURCE_HW_COUNT_VERTEX);
        ++word_index) {
      resources_needed[word_index] =
         (state->sq_constants_modified.resources.tes_in_vses[word_index] |
          state->sq_constants_for_tes_overwritten_in_vses_by_vs.resources[word_index]) &
         state->sq_constants_needed.resources.tes[word_index];
   }

   unsigned resource_index;
   BITSET_FOREACH_SET (resource_index, resources_needed, TERAKAN_RESOURCE_HW_COUNT_VERTEX) {
      bool emit_resource = false;

      bool const resource_not_null = BITSET_TEST(state->sq_resources_not_null.tes, resource_index);

      if (BITSET_TEST(state->sq_constants_modified.resources.tes_in_vses, resource_index)) {
         /* Completely new resource needed for the Vulkan TES stage. */
         emit_resource = true;
         BITSET_CLEAR(state->sq_constants_modified.resources.tes_in_vses, resource_index);
         /* Next time the hardware binding is needed for VS, make sure the VS one is applied.
          * Simply mark as overwritten for now to avoid overhead within tesellated draws, defer all
          * more complex logic until tessellation is disabled.
          */
         BITSET_SET(state->sq_constants_for_vs_overwritten_in_vses_by_tes.resources,
                    resource_index);
      } else {
         /* The previous TES resource is still up to date, but VSES bindings now include the VS
          * binding instead, which may be different, or the same (VS | TES bindings in the
          * beginning of the pipeline layout, may happen in applications having a common descriptor
          * set referenced by all shaders).
          */
         if ((resource_not_null != BITSET_TEST(state->sq_resources_not_null.vs, resource_index)) ||
             (resource_not_null && (state->sq_resource_bos.tes[resource_index] !=
                                       state->sq_resource_bos.vs[resource_index] ||
                                    memcmp(state->sq_resource_descriptors.tes[resource_index],
                                           state->sq_resource_descriptors.vs[resource_index],
                                           sizeof(uint32_t) * 8) != 0))) {
            /* VS and TES resources for this binding are different, emit the TES resource now, and
             * emit the VS resource next time it's needed.
             */
            emit_resource = true;
            BITSET_SET(state->sq_constants_for_vs_overwritten_in_vses_by_tes.resources,
                       resource_index);
         } else {
            /* VS and TES resources at this index are the same, continue using the resource for both
             * TES for this emission and VS later.
             */
            BITSET_CLEAR(state->sq_constants_for_vs_overwritten_in_vses_by_tes.resources,
                         resource_index);
         }
      }

      if (emit_resource && !terakan_hw_state_draw_emit_resource(
                              command_writer, TERAKAN_RESOURCE_HW_OFFSET_VSES + resource_index,
                              resource_not_null, state->sq_resource_bos.tes[resource_index],
                              state->sq_resource_descriptors.tes[resource_index])) {
         return;
      }

      BITSET_CLEAR(state->sq_constants_for_tes_overwritten_in_vses_by_vs.resources, resource_index);
   }
}

static void
terakan_hw_state_draw_emit_sq_resources_gs(struct terakan_gfx_command_writer * const command_writer,
                                           UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;
   if (!terakan_hw_state_draw_sq_constants_needed_by_gs(state)) {
      return;
   }
   terakan_hw_state_draw_emit_sq_resources_for_stage(
      command_writer, TERAKAN_RESOURCE_HW_OFFSET_GS, TERAKAN_RESOURCE_HW_COUNT_VERTEX,
      state->sq_resources_not_null.gs, state->sq_resource_bos.gs,
      state->sq_resource_descriptors.gs[0], state->sq_constants_needed.resources.gs,
      state->sq_constants_modified.resources.gs);
}

static void
terakan_hw_state_draw_emit_sq_resources_fs(struct terakan_gfx_command_writer * const command_writer,
                                           UNUSED enum terakan_hw_state_draw_index const state_index)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;
   terakan_hw_state_draw_emit_sq_resources_for_stage(
      command_writer, TERAKAN_RESOURCE_HW_OFFSET_PS, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE,
      state->sq_resources_not_null.fs, state->sq_resource_bos.fs,
      state->sq_resource_descriptors.fs[0], state->sq_constants_needed.resources.fs,
      state->sq_constants_modified.resources.fs);
}

static terakan_hw_state_draw_emit_function const
   terakan_hw_state_draw_emit_functions[TERAKAN_HW_STATE_DRAW_COUNT] = {
      [TERAKAN_HW_STATE_DRAW_VGT_INDEX_TYPE] = terakan_hw_state_draw_emit_vgt_index_type,
      [TERAKAN_HW_STATE_DRAW_VGT_INDEX_BUFFER] = terakan_hw_state_draw_emit_vgt_index_buffer,
      [TERAKAN_HW_STATE_DRAW_VGT_PRIMITIVE_TYPE] = terakan_hw_state_draw_emit_vgt_primitive_type,
      [TERAKAN_HW_STATE_DRAW_VGT_INDEX_OFFSET] = terakan_hw_state_draw_emit_vgt_index_offset,
      [TERAKAN_HW_STATE_DRAW_SQ_PGM_VS] = terakan_hw_state_draw_emit_sq_pgm_vs,
      [TERAKAN_HW_STATE_DRAW_SQ_PGM_PS] = terakan_hw_state_draw_emit_sq_pgm_ps,
      [TERAKAN_HW_STATE_DRAW_SQ_VTX_START_INST_LOC] =
         terakan_hw_state_draw_emit_sq_vtx_start_inst_loc,
      [TERAKAN_HW_STATE_DRAW_PA_CL_CLIP_CNTL] = terakan_hw_state_draw_emit_pa_cl_clip_cntl,
      [TERAKAN_HW_STATE_DRAW_PA_SU_SC_MODE_CNTL] = terakan_hw_state_draw_emit_pa_su_sc_mode_cntl,
      [TERAKAN_HW_STATE_DRAW_PA_CL_VTE_CNTL] = terakan_hw_state_draw_emit_pa_cl_vte_cntl,
      [TERAKAN_HW_STATE_DRAW_PA_SC_MODE_CNTL_0] = terakan_hw_state_draw_emit_pa_sc_mode_cntl_0,
      [TERAKAN_HW_STATE_DRAW_PA_CL_GB] = terakan_hw_state_draw_emit_pa_cl_gb,
      [TERAKAN_HW_STATE_DRAW_PA_SC_AA_SAMPLES] = terakan_hw_state_draw_emit_pa_sc_aa_samples,
      [TERAKAN_HW_STATE_DRAW_PA_SC_AA_MASK] = terakan_hw_state_draw_emit_pa_sc_aa_mask,
      [TERAKAN_HW_STATE_DRAW_DB_RENDER_OVERRIDE] = terakan_hw_state_draw_emit_db_render_override,
      [TERAKAN_HW_STATE_DRAW_CB_BLEND_RGBA] = terakan_hw_state_draw_emit_cb_blend_rgba,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST] = terakan_hw_state_draw_emit_cb_color,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 1] = terakan_hw_state_draw_emit_cb_color,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 2] = terakan_hw_state_draw_emit_cb_color,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 3] = terakan_hw_state_draw_emit_cb_color,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 4] = terakan_hw_state_draw_emit_cb_color,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 5] = terakan_hw_state_draw_emit_cb_color,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 6] = terakan_hw_state_draw_emit_cb_color,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 7] = terakan_hw_state_draw_emit_cb_color,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 8] = terakan_hw_state_draw_emit_cb_color_rat_only,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 9] = terakan_hw_state_draw_emit_cb_color_rat_only,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 10] = terakan_hw_state_draw_emit_cb_color_rat_only,
      [TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST + 11] = terakan_hw_state_draw_emit_cb_color_rat_only,
      [TERAKAN_HW_STATE_DRAW_VIEWPORT] = terakan_hw_state_draw_emit_viewport,
      [TERAKAN_HW_STATE_DRAW_SQ_KCACHE_VS] = terakan_hw_state_draw_emit_sq_kcache_vs,
      [TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TCS] = terakan_hw_state_draw_emit_sq_kcache_tcs,
      [TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TES] = terakan_hw_state_draw_emit_sq_kcache_tes,
      [TERAKAN_HW_STATE_DRAW_SQ_KCACHE_GS] = terakan_hw_state_draw_emit_sq_kcache_gs,
      [TERAKAN_HW_STATE_DRAW_SQ_KCACHE_FS] = terakan_hw_state_draw_emit_sq_kcache_fs,
      [TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VI] = terakan_hw_state_draw_emit_sq_resources_vi,
      [TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VS] = terakan_hw_state_draw_emit_sq_resources_vs,
      [TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TCS] = terakan_hw_state_draw_emit_sq_resources_tcs,
      [TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TES] = terakan_hw_state_draw_emit_sq_resources_tes,
      [TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_GS] = terakan_hw_state_draw_emit_sq_resources_gs,
      [TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_FS] = terakan_hw_state_draw_emit_sq_resources_fs,
};

void
terakan_hw_state_draw_ensure_viewport_count(struct terakan_hw_state_draw * const state,
                                            uint32_t const viewport_count)
{
   assert(viewport_count <= ARRAY_SIZE(state->viewports));

   uint32_t const old_viewport_count = state->viewport_count_ever_written;
   if (viewport_count <= old_viewport_count) {
      return;
   }
   uint32_t const viewports_added = viewport_count - old_viewport_count;

   /* There's no (and no need for because everything is mostly always applicable) tracking of
    * whether each individual part of the state of a viewport has ever been written, so clear it to
    * something safe.
    */
   memset(state->viewports + old_viewport_count, 0, sizeof(*state->viewports) * viewports_added);

   /* Mark everything in the new viewports as modified, as state->viewports has been initialized,
    * but the state on the GPU is still undefined, and thus an equality comparison can't be used to
    * check if a new value of some viewport state is still the same as what has already been
    * emitted.
    */
   for (uint32_t viewport_index = old_viewport_count; viewport_index < viewport_count;
        ++viewport_index) {
      BITSET_ONES(state->viewports[viewport_index].state_modified);
   }
   state->viewports_modified |= (uint16_t)(((uint32_t)1 << viewports_added) - 1)
                                << old_viewport_count;

   state->viewport_count_ever_written = viewport_count;

   BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_VIEWPORT);
}

/* Returns whether the buffer is now modified. */
static bool
terakan_hw_state_draw_set_sq_kcache_buffer(
   struct terakan_hw_state_draw * const state, uint32_t const buffer_index,
   uint32_t const size_lines, struct terakan_bo const * const bo, uint32_t const base_lines,
   enum terakan_hw_state_draw_sq_constants_needed_stage const needed_stage,
   enum terakan_hw_state_draw_sq_constants_modified_stage const modified_stage,
   enum terakan_hw_state_draw_index const state_index)
{
   assert(!(size_lines != 0 && bo == NULL));

   uint16_t const buffer_bit = (uint16_t)1 << buffer_index;

   uint16_t * const buffers_modified = &state->sq_constants_modified.kcache[modified_stage];

   struct terakan_hw_state_sq_kcache_buffer * const buffer_ptr =
      &state->sq_kcache_buffers[needed_stage][buffer_index];

   if (!(*buffers_modified & buffer_bit) && buffer_ptr->size_lines == size_lines &&
       (size_lines == 0 || (buffer_ptr->bo == bo && buffer_ptr->base_lines == base_lines))) {
      /* Not modified. */
      return false;
   }

   buffer_ptr->bo = bo;
   buffer_ptr->base_lines = base_lines;
   buffer_ptr->size_lines = size_lines;

   *buffers_modified |= buffer_bit;

   if (state->sq_constants_needed.kcache[needed_stage] & buffer_bit) {
      /* Emit before the next draw. */
      BITSET_SET(state->state_modified, state_index);
   }

   return true;
}

void
terakan_hw_state_draw_set_sq_kcache_vs(struct terakan_hw_state_draw * const state,
                                       uint32_t const buffer_index, uint32_t const size_lines,
                                       struct terakan_bo const * const bo,
                                       uint32_t const base_lines)
{
   if (terakan_hw_state_draw_set_sq_kcache_buffer(
          state, buffer_index, size_lines, bo, base_lines,
          TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
          TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES,
          TERAKAN_HW_STATE_DRAW_SQ_KCACHE_VS)) {
      state->sq_constants_modified
         .kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS] |= (uint16_t)1
                                                                                << buffer_index;
   }
}

void
terakan_hw_state_draw_set_sq_kcache_tcs(struct terakan_hw_state_draw * const state,
                                        uint32_t const buffer_index, uint32_t const size_lines,
                                        struct terakan_bo const * const bo,
                                        uint32_t const base_lines)
{
   terakan_hw_state_draw_set_sq_kcache_buffer(state, buffer_index, size_lines, bo, base_lines,
                                              TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS,
                                              TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TCS,
                                              TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TCS);
}

void
terakan_hw_state_draw_set_sq_kcache_tes(struct terakan_hw_state_draw * const state,
                                        uint32_t const buffer_index, uint32_t const size_lines,
                                        struct terakan_bo const * const bo,
                                        uint32_t const base_lines)
{
   terakan_hw_state_draw_set_sq_kcache_buffer(
      state, buffer_index, size_lines, bo, base_lines,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES,
      TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TES);
}

void
terakan_hw_state_draw_set_sq_kcache_gs(struct terakan_hw_state_draw * const state,
                                       uint32_t const buffer_index, uint32_t const size_lines,
                                       struct terakan_bo const * const bo,
                                       uint32_t const base_lines)
{
   terakan_hw_state_draw_set_sq_kcache_buffer(state, buffer_index, size_lines, bo, base_lines,
                                              TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS,
                                              TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_GS,
                                              TERAKAN_HW_STATE_DRAW_SQ_KCACHE_GS);
}

void
terakan_hw_state_draw_set_sq_kcache_fs(struct terakan_hw_state_draw * const state,
                                       uint32_t const buffer_index, uint32_t const size_lines,
                                       struct terakan_bo const * const bo,
                                       uint32_t const base_lines)
{
   terakan_hw_state_draw_set_sq_kcache_buffer(state, buffer_index, size_lines, bo, base_lines,
                                              TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,
                                              TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_FS,
                                              TERAKAN_HW_STATE_DRAW_SQ_KCACHE_FS);
}

/* Returns whether the resource is now modified. */
static bool
terakan_hw_state_draw_set_sq_resource(
   struct terakan_hw_state_draw * const state, uint32_t const index,
   struct terakan_bo const * const bo, uint32_t const descriptor[8],
   BITSET_WORD * const not_null_bitset, struct terakan_bo const ** const bos,
   uint32_t * const descriptors, BITSET_WORD const * const needed_bitset,
   BITSET_WORD * const modified_bitset, enum terakan_hw_state_draw_index const state_index)
{
   bool const not_null = bo != NULL;

   if (!BITSET_TEST(modified_bitset, index) && BITSET_TEST(not_null_bitset, index) == not_null &&
       (!not_null || (bos[index] == bo &&
                      memcmp(descriptors + 8 * index, descriptor, sizeof(uint32_t) * 8) == 0))) {
      /* Not modified. */
      return false;
   }

   if (not_null) {
      bos[index] = bo;
      memcpy(descriptors + 8 * index, descriptor, sizeof(uint32_t) * 8);
      BITSET_SET(not_null_bitset, index);
   } else {
      BITSET_CLEAR(not_null_bitset, index);
   }

   BITSET_SET(modified_bitset, index);

   if (BITSET_TEST(needed_bitset, index)) {
      /* Emit before the next draw. */
      BITSET_SET(state->state_modified, state_index);
   }

   return true;
}

void
terakan_hw_state_draw_set_sq_resource_vi(struct terakan_hw_state_draw * const state,
                                         uint32_t const index, struct terakan_bo const * const bo,
                                         uint32_t const descriptor[8])
{
   terakan_hw_state_draw_set_sq_resource(
      state, index, bo, descriptor, state->sq_resources_not_null.vi, state->sq_resource_bos.vi,
      state->sq_resource_descriptors.vi[0], state->sq_constants_needed.resources.vi,
      state->sq_constants_modified.resources.vi, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VI);
}

void
terakan_hw_state_draw_set_sq_resource_vs(struct terakan_hw_state_draw * const state,
                                         uint32_t const index, struct terakan_bo const * const bo,
                                         uint32_t const descriptor[8])
{
   if (terakan_hw_state_draw_set_sq_resource(
          state, index, bo, descriptor, state->sq_resources_not_null.vs, state->sq_resource_bos.vs,
          state->sq_resource_descriptors.vs[0], state->sq_constants_needed.resources.vs,
          state->sq_constants_modified.resources.vs_in_vses,
          TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VS)) {
      BITSET_SET(state->sq_constants_modified.resources.vs_in_ls, index);
   }
}

void
terakan_hw_state_draw_set_sq_resource_tcs(struct terakan_hw_state_draw * const state,
                                          uint32_t const index, struct terakan_bo const * const bo,
                                          uint32_t const descriptor[8])
{
   terakan_hw_state_draw_set_sq_resource(
      state, index, bo, descriptor, state->sq_resources_not_null.tcs, state->sq_resource_bos.tcs,
      state->sq_resource_descriptors.tcs[0], state->sq_constants_needed.resources.tcs,
      state->sq_constants_modified.resources.tcs, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TCS);
}

void
terakan_hw_state_draw_set_sq_resource_tes(struct terakan_hw_state_draw * const state,
                                          uint32_t const index, struct terakan_bo const * const bo,
                                          uint32_t const descriptor[8])
{
   terakan_hw_state_draw_set_sq_resource(
      state, index, bo, descriptor, state->sq_resources_not_null.tes, state->sq_resource_bos.tes,
      state->sq_resource_descriptors.tes[0], state->sq_constants_needed.resources.tes,
      state->sq_constants_modified.resources.tes_in_vses, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TES);
}

void
terakan_hw_state_draw_set_sq_resource_gs(struct terakan_hw_state_draw * const state,
                                         uint32_t const index, struct terakan_bo const * const bo,
                                         uint32_t const descriptor[8])
{
   terakan_hw_state_draw_set_sq_resource(
      state, index, bo, descriptor, state->sq_resources_not_null.gs, state->sq_resource_bos.gs,
      state->sq_resource_descriptors.gs[0], state->sq_constants_needed.resources.gs,
      state->sq_constants_modified.resources.gs, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_GS);
}

void
terakan_hw_state_draw_set_sq_resource_fs(struct terakan_hw_state_draw * const state,
                                         uint32_t const index, struct terakan_bo const * const bo,
                                         uint32_t const descriptor[8])
{
   terakan_hw_state_draw_set_sq_resource(
      state, index, bo, descriptor, state->sq_resources_not_null.fs, state->sq_resource_bos.fs,
      state->sq_resource_descriptors.fs[0], state->sq_constants_needed.resources.fs,
      state->sq_constants_modified.resources.fs, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_FS);
}

static void
terakan_hw_state_draw_set_sq_constants_needed(
   struct terakan_hw_state_draw * const state, uint16_t const kcache,
   uint32_t const * const resources_opt, bool const stage_needed,
   enum terakan_hw_state_draw_sq_constants_needed_stage const constants_needed_stage,
   enum terakan_hw_state_draw_index const kcache_state_index, uint32_t const resource_count,
   BITSET_WORD * const resources_needed_bitset,
   enum terakan_hw_state_draw_index const resources_state_index)
{
   size_t const resource_bitset_size = sizeof(BITSET_WORD) * BITSET_WORDS(resource_count);

   if (stage_needed) {
      if (state->sq_constants_needed.kcache[constants_needed_stage] != kcache) {
         BITSET_SET(state->state_modified, kcache_state_index);
      }

      if (resources_opt != NULL && !BITSET_TEST(state->state_modified, resources_state_index) &&
          memcmp(resources_needed_bitset, resources_opt, resource_bitset_size) != 0) {
         BITSET_SET(state->state_modified, resources_state_index);
      }
   }

   state->sq_constants_needed.kcache[constants_needed_stage] = kcache;
   if (!kcache) {
      BITSET_CLEAR(state->state_modified, kcache_state_index);
   }

   if (resources_opt != NULL) {
      memcpy(resources_needed_bitset, resources_opt, resource_bitset_size);
   } else {
      BITSET_CLEAR(state->state_modified, resources_state_index);
      memset(resources_needed_bitset, 0, resource_bitset_size);
   }
}

static inline void
terakan_hw_state_draw_update_sq_constants_gs_needed(struct terakan_hw_state_draw * const state,
                                                    bool const gs_was_needed)
{
   bool const gs_needed = terakan_hw_state_draw_sq_constants_needed_by_gs(state);
   if (gs_was_needed == gs_needed) {
      return;
   }
   if (gs_needed) {
      BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_GS);
      BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_GS);
   } else {
      BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_GS);
      BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_GS);
   }
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_vi(struct terakan_hw_state_draw * const state,
                                                    BITSET_WORD const * const resources_opt)
{
   size_t const resource_bitset_size =
      sizeof(BITSET_WORD) * BITSET_WORDS(TERAKAN_RESOURCE_HW_COUNT_FETCH);
   if (resources_opt != NULL) {
      if (!BITSET_TEST(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VI) &&
          memcmp(state->sq_constants_needed.resources.vi, resources_opt, resource_bitset_size) !=
             0) {
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VI);
      }
      memcpy(state->sq_constants_needed.resources.vi, resources_opt, resource_bitset_size);
   } else {
      BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VI);
      memset(state->sq_constants_needed.resources.vi, 0, resource_bitset_size);
   }
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_vs(struct terakan_hw_state_draw * const state,
                                                    uint16_t const kcache,
                                                    BITSET_WORD const * const resources_opt,
                                                    VkShaderStageFlags const next_stage)
{
   terakan_hw_state_draw_set_sq_constants_needed(
      state, kcache, resources_opt, true, TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
      TERAKAN_HW_STATE_DRAW_SQ_KCACHE_VS, TERAKAN_RESOURCE_HW_COUNT_VERTEX,
      state->sq_constants_needed.resources.vs, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VS);

   bool const tcs_tes_needed = next_stage == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
   if (state->sq_constants_needed.tcs_tes != tcs_tes_needed) {
      state->sq_constants_needed.tcs_tes = tcs_tes_needed;
      if (tcs_tes_needed) {
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TCS);
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TES);
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TCS);
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TES);
      } else {
         BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TCS);
         BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TES);
         BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TCS);
         BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TES);
      }
   }

   bool const gs_was_needed = terakan_hw_state_draw_sq_constants_needed_by_gs(state);
   state->sq_constants_needed.gs_after_vs = next_stage == VK_SHADER_STAGE_GEOMETRY_BIT;
   terakan_hw_state_draw_update_sq_constants_gs_needed(state, gs_was_needed);
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_tcs(struct terakan_hw_state_draw * const state,
                                                     uint16_t const kcache,
                                                     BITSET_WORD const * const resources_opt)
{
   terakan_hw_state_draw_set_sq_constants_needed(
      state, kcache, resources_opt, state->sq_constants_needed.tcs_tes,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TCS,
      TERAKAN_RESOURCE_HW_COUNT_VERTEX, state->sq_constants_needed.resources.tcs,
      TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TCS);
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_tes(struct terakan_hw_state_draw * const state,
                                                     uint16_t const kcache,
                                                     BITSET_WORD const * const resources_opt,
                                                     bool const next_stage_is_gs)
{
   terakan_hw_state_draw_set_sq_constants_needed(
      state, kcache, resources_opt, state->sq_constants_needed.tcs_tes,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TES,
      TERAKAN_RESOURCE_HW_COUNT_VERTEX, state->sq_constants_needed.resources.tes,
      TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TES);

   bool const gs_was_needed = terakan_hw_state_draw_sq_constants_needed_by_gs(state);
   state->sq_constants_needed.gs_after_tes = next_stage_is_gs;
   terakan_hw_state_draw_update_sq_constants_gs_needed(state, gs_was_needed);
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_gs(struct terakan_hw_state_draw * const state,
                                                    uint16_t const kcache,
                                                    BITSET_WORD const * const resources_opt)
{
   terakan_hw_state_draw_set_sq_constants_needed(
      state, kcache, resources_opt, terakan_hw_state_draw_sq_constants_needed_by_gs(state),
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_GS,
      TERAKAN_RESOURCE_HW_COUNT_VERTEX, state->sq_constants_needed.resources.gs,
      TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_GS);
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_fs(struct terakan_hw_state_draw * const state,
                                                    uint16_t const kcache,
                                                    BITSET_WORD const * const resources_opt)
{
   terakan_hw_state_draw_set_sq_constants_needed(
      state, kcache, resources_opt, true, TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,
      TERAKAN_HW_STATE_DRAW_SQ_KCACHE_FS, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE,
      state->sq_constants_needed.resources.fs, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_FS);
}

void
terakan_hw_state_draw_indirect_buffer_begun_and_sq_resources_cleared(
   struct terakan_hw_state_draw * const state)
{
   /* Kcache registers are undefined in the hardware. */
   memset(state->sq_constants_modified.kcache, UCHAR_MAX,
          sizeof(state->sq_constants_modified.kcache));

   /* All resources are null in the hardware.
    * Make sure those that shouldn't be null are emitted next time they're needed.
    */
   BITSET_COPY(state->sq_constants_modified.resources.vi, state->sq_resources_not_null.vi);
   BITSET_COPY(state->sq_constants_modified.resources.vs_in_vses, state->sq_resources_not_null.vs);
   BITSET_COPY(state->sq_constants_modified.resources.vs_in_ls, state->sq_resources_not_null.vs);
   BITSET_COPY(state->sq_constants_modified.resources.tcs, state->sq_resources_not_null.tcs);
   BITSET_COPY(state->sq_constants_modified.resources.tes_in_vses,
               state->sq_resources_not_null.tes);
   BITSET_COPY(state->sq_constants_modified.resources.gs, state->sq_resources_not_null.gs);
   BITSET_COPY(state->sq_constants_modified.resources.fs, state->sq_resources_not_null.fs);

   /* Constants for VS or TES have not been emitted in the current indirect buffer yet. */
   memset(&state->sq_constants_for_vs_overwritten_in_vses_by_tes, 0,
          sizeof(state->sq_constants_for_vs_overwritten_in_vses_by_tes));
   memset(&state->sq_constants_for_tes_overwritten_in_vses_by_vs, 0,
          sizeof(state->sq_constants_for_tes_overwritten_in_vses_by_vs));
}

void
terakan_hw_state_draw_emit_modified(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;
   unsigned state_index;
   BITSET_FOREACH_SET (state_index, state->state_modified, TERAKAN_HW_STATE_DRAW_COUNT) {
      terakan_hw_state_draw_emit_functions[state_index](
         command_writer, (enum terakan_hw_state_draw_index)state_index);
      if (unlikely(!BITSET_TEST(state->state_modified, state_index))) {
         /* If state_modified was zeroed during an emit call, switched to another indirect buffer,
          * and all state has been applied.
          */
         return;
      }
      BITSET_CLEAR(state->state_modified, state_index);
   }
}

void
terakan_hw_state_draw_emit_all(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   BITSET_ZERO(state->state_modified);

   /* Make sure the viewport emission callback emits the state for all viewports. */
   state->viewports_modified = (uint16_t)((uint32_t)1 << state->viewport_count_ever_written);
   for (uint32_t viewport_index = 0; viewport_index < state->viewport_count_ever_written;
        ++viewport_index) {
      BITSET_ONES(state->viewports[viewport_index].state_modified);
   }

   unsigned state_index;
   BITSET_FOREACH_SET (state_index, state->state_ever_written, TERAKAN_HW_STATE_DRAW_COUNT) {
      terakan_hw_state_draw_emit_functions[state_index](
         command_writer, (enum terakan_hw_state_draw_index)state_index);
   }
}

void
terakan_hw_state_draw_reset(struct terakan_hw_state_draw * const state)
{
   BITSET_ZERO(state->state_ever_written);
   BITSET_ZERO(state->state_modified);

   state->viewport_count_ever_written = 0;
   state->viewports_modified = 0;
   /* For simplicity, consider the viewport state always valid (starting from 0 viewports). */
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_VIEWPORT);

   memset(&state->sq_constants_needed, 0, sizeof(state->sq_constants_needed));

   /* For simplicity, don't track whether constants have actually ever been set, reset them to null.
    */
   memset(&state->sq_kcache_buffers, 0, sizeof(state->sq_kcache_buffers));
   memset(&state->sq_resources_not_null, 0, sizeof(state->sq_resources_not_null));

   /* state_modified is set specially for sequencer constants, and state_ever_written is not
    * updated for them, but make sure they are emitted next full state emission - they've also just
    * been initialized to a valid state (to all being null).
    */

   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_VS);
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TCS);
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_TES);
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_GS);
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_KCACHE_FS);

   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VI);
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_VS);
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TCS);
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_TES);
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_GS);
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_SQ_RESOURCES_FS);
}
