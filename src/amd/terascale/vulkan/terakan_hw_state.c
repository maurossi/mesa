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

#include "terakan_hw_state.h"

#include "terakan_command_buffer.h"
#include "terakan_physical_device.h"

#include "amd/terascale/common/terascale_format.h"
#include "amd/terascale/common/terascale_wddm.h"
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
   (((uint32_t)(s0x) & 0xF) | (((uint32_t)(s0y) & 0xF) << 4) | (((uint32_t)(s1x) & 0xF) << 8) |    \
    (((uint32_t)(s1y) & 0xF) << 12) | (((uint32_t)(s2x) & 0xF) << 16) |                            \
    (((uint32_t)(s2y) & 0xF) << 20) | (((uint32_t)(s3x) & 0xF) << 24) |                            \
    (((uint32_t)(s3y) & 0xF) << 28))

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
   struct terakan_gfx_command_writer * command_writer);

static void
terakan_hw_state_draw_emit_vgt_index_type(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_INDEX_TYPE, 1 - 1, 0);
   *packet++ = command_writer->hw_state_draw.vgt_index_type;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_vgt_index_buffer(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet =
      terakan_gfx_command_writer_emit_with_bo(command_writer, 3 + 2, true, 1, 0, 1);
   if (unlikely(packet == NULL)) {
      return;
   }

   *packet++ = PKT3(EG_PKT3_INDEX_BASE, 2 - 1, 0);
   uint32_t const * const packet_index_base = packet;
   *packet++ = (uint32_t)command_writer->hw_state_draw.vgt_index_buffer.va;
   *packet++ = (command_writer->hw_state_draw.vgt_index_buffer.va >> 32) & 0xFF;
   terakan_gfx_command_writer_add_relocation_for_40_bits(
      command_writer, &packet, packet_index_base, packet_index_base + 1,
      TERASCALE_WDDM_PATCH_IDS_INDEX_BASE_LO, TERASCALE_WDDM_PATCH_IDS_INDEX_BASE_HI,
      terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer,
                                                command_writer->hw_state_draw.vgt_index_buffer.bo,
                                                true, false, TERAKAN_BO_PRIORITY_INDEX_BUFFER));

   *packet++ = PKT3(EG_PKT3_INDEX_BUFFER_SIZE, 1 - 1, 0);
   *packet++ = command_writer->hw_state_draw.vgt_index_buffer.size;

   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_vgt_primitive_type(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONFIG_REG, 1, 0);
   *packet++ = TERAKAN_CONFIG_REG_OFFSET(R_008958_VGT_PRIMITIVE_TYPE);
   *packet++ = command_writer->hw_state_draw.vgt_primitive_type;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_vgt_index_offset(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028408_VGT_INDX_OFFSET);
   *packet++ = command_writer->hw_state_draw.vgt_index_offset;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_sq_pgm_fs(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet =
      terakan_gfx_command_writer_emit_with_bo(command_writer, 2 + 1, true, 1, 1, 0);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_0288A4_SQ_PGM_START_FS);
   uint32_t const * const packet_pgm_start = packet;
   *packet++ = command_writer->hw_state_draw.sq_pgm_fs.va_shr8;
   terakan_gfx_command_writer_add_relocation(
      command_writer, &packet, packet_pgm_start, *packet_pgm_start,
      TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_FS,
      terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer,
                                                command_writer->hw_state_draw.sq_pgm_fs.bo, true,
                                                false, TERAKAN_BO_PRIORITY_SHADER_BINARY));
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_sq_pgm_vs(struct terakan_gfx_command_writer * const command_writer)
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

   uint32_t * packet =
      terakan_gfx_command_writer_emit_with_bo(command_writer, packet_dwords, true, 1, 1, 0);
   if (unlikely(packet == NULL)) {
      return;
   }

   *packet++ =
      PKT3(PKT3_SET_CONTEXT_REG,
           (R_028864_SQ_PGM_RESOURCES_2_VS - R_02885C_SQ_PGM_START_VS) / sizeof(uint32_t) + 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_02885C_SQ_PGM_START_VS);
   uint32_t const * const packet_pgm_start = packet;
   *packet++ = shader->program_va_shr8;
   /* TODO(Triang3l): USE_LS_CONSTS. */
   *packet++ = shader->sq_pgm_resources[0];
   *packet++ = shader->sq_pgm_resources[1];
   terakan_gfx_command_writer_add_relocation(
      command_writer, &packet, packet_pgm_start, *packet_pgm_start,
      TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_VS,
      terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer,
                                                shader->program_bo, true, false,
                                                TERAKAN_BO_PRIORITY_SHADER_BINARY));

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

   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_sq_pgm_ps(struct terakan_gfx_command_writer * const command_writer)
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

   uint32_t * packet =
      terakan_gfx_command_writer_emit_with_bo(command_writer, packet_dwords, true, 1, 1, 0);
   if (unlikely(packet == NULL)) {
      return;
   }

   *packet++ =
      PKT3(PKT3_SET_CONTEXT_REG,
           (R_02884C_SQ_PGM_EXPORTS_PS - R_028840_SQ_PGM_START_PS) / sizeof(uint32_t) + 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028840_SQ_PGM_START_PS);
   uint32_t const * const packet_pgm_start = packet;
   *packet++ = shader->program_va_shr8;
   *packet++ = shader->sq_pgm_resources[0];
   *packet++ = shader->sq_pgm_resources[1];
   *packet++ = shader->stage.ps.sq_pgm_exports_ps;
   terakan_gfx_command_writer_add_relocation(
      command_writer, &packet, packet_pgm_start, *packet_pgm_start,
      TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_PS,
      terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer,
                                                shader->program_bo, true, false,
                                                TERAKAN_BO_PRIORITY_SHADER_BINARY));

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

   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_sq_vtx_start_inst_loc(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CTL_CONST, 1, 0);
   *packet++ = TERAKAN_CTL_CONST_OFFSET(R_03CFF4_SQ_VTX_START_INST_LOC);
   *packet++ = command_writer->hw_state_draw.sq_vtx_start_inst_loc;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_pa_cl_clip_cntl(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028810_PA_CL_CLIP_CNTL);
   *packet++ = command_writer->hw_state_draw.pa_cl_clip_cntl;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_pa_su_sc_mode_cntl(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028814_PA_SU_SC_MODE_CNTL);
   *packet++ = command_writer->hw_state_draw.pa_su_sc_mode_cntl;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_pa_cl_vte_cntl(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028818_PA_CL_VTE_CNTL);
   *packet++ = command_writer->hw_state_draw.pa_cl_vte_cntl;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_pa_sc_mode_cntl_0(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028A48_PA_SC_MODE_CNTL_0);
   *packet++ = command_writer->hw_state_draw.pa_sc_mode_cntl_0;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_pa_su_poly_offset_db_fmt_cntl(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028B78_PA_SU_POLY_OFFSET_DB_FMT_CNTL);
   *packet++ = command_writer->hw_state_draw.pa_su_poly_offset_db_fmt_cntl;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_pa_su_poly_offset_clamp_scale_offset(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t const register_count =
      (R_028B8C_PA_SU_POLY_OFFSET_BACK_OFFSET - R_028B7C_PA_SU_POLY_OFFSET_CLAMP) /
         sizeof(uint32_t) +
      1;
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + register_count, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, register_count, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028B7C_PA_SU_POLY_OFFSET_CLAMP);
   memcpy(packet++, &command_writer->hw_state_draw.pa_su_poly_offset_clamp, sizeof(float));
   /* Front face. */
   memcpy(packet++, &command_writer->hw_state_draw.pa_su_poly_offset_subpixel_slope_scale,
          sizeof(float));
   memcpy(packet++, &command_writer->hw_state_draw.pa_su_poly_offset_offset, sizeof(float));
   /* Back face. */
   memcpy(packet++, &command_writer->hw_state_draw.pa_su_poly_offset_subpixel_slope_scale,
          sizeof(float));
   memcpy(packet++, &command_writer->hw_state_draw.pa_su_poly_offset_offset, sizeof(float));
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_pa_cl_gb(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 4, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   /* According to the Gallium R600 and RadeonSI drivers and to PAL's Pm4Optimizer, all the four
    * guard band discard / clip adjust registers are a single register "vector", and if any value in
    * the vector is changed, the hardware requires that the entire vector is emitted.
    */
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 4, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(
      terakan_gfx_command_writer_physical_device(command_writer)->chip_family_info.is_r9xx
         ? CM_R_028BE8_PA_CL_GB_VERT_CLIP_ADJ
         : R_028C0C_PA_CL_GB_VERT_CLIP_ADJ);
   memcpy(packet, command_writer->hw_state_draw.pa_cl_gb_vert_horz_clip_disc_adj,
          sizeof(float) * 4);
   packet += 4;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_pa_sc_aa_samples(struct terakan_gfx_command_writer * const command_writer)
{
   bool const is_r9xx =
      terakan_gfx_command_writer_physical_device(command_writer)->chip_family_info.is_r9xx;

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
         command_writer, (2 + 2) + (2 + 1) + (2 + num_sample_loc_dwords) * 4, true);
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

      terakan_gfx_command_writer_emit_done(command_writer, packet);
   } else {
      uint32_t * packet = terakan_gfx_command_writer_emit(
         command_writer, (2 + 1) + (2 + num_sample_loc_dwords * 4), true);
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

      terakan_gfx_command_writer_emit_done(command_writer, packet);
   }
}

static void
terakan_hw_state_draw_emit_pa_sc_aa_mask(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t aa_mask = command_writer->hw_state_draw.pa_sc_aa_mask;
   aa_mask |= aa_mask << 16;

   if (terakan_gfx_command_writer_physical_device(command_writer)->chip_family_info.is_r9xx) {
      uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 2, true);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_028C38_PA_SC_AA_MASK_X0Y0_X1Y0);
      *packet++ = aa_mask;
      *packet++ = aa_mask;
      terakan_gfx_command_writer_emit_done(command_writer, packet);
   } else {
      aa_mask &= (((uint32_t)1 << 8) - 1) | ((((uint32_t)1 << 8) - 1) << 16);
      aa_mask |= aa_mask << 8;

      uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028C3C_PA_SC_AA_MASK);
      *packet++ = aa_mask;
      terakan_gfx_command_writer_emit_done(command_writer, packet);
   }
}

static void
terakan_hw_state_draw_emit_db_depth_stencil_buffer(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet;
   uint32_t bo_reference;

   struct terakan_bo const * const bo = command_writer->hw_state_draw.db_depth_stencil_buffer.bo;
   if (bo == NULL) {
      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 2, true);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028040_DB_Z_INFO);
      *packet++ = S_028040_FORMAT(V_028040_Z_INVALID);
      *packet++ = S_028044_FORMAT(V_028044_STENCIL_INVALID);
      terakan_gfx_command_writer_emit_done(command_writer, packet);
      return;
   }

   struct terakan_depth_stencil_descriptor const * const descriptor =
      &command_writer->hw_state_draw.db_depth_stencil_buffer.descriptor;

   bool const z_bound = G_028040_FORMAT(descriptor->z_info) != V_028040_Z_INVALID;
   bool const stencil_bound = G_028044_FORMAT(descriptor->stencil_info) != V_028044_STENCIL_INVALID;

   if (!z_bound && !stencil_bound) {
      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 2, true);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028040_DB_Z_INFO);
      *packet++ = S_028040_FORMAT(V_028040_Z_INVALID);
      *packet++ = S_028044_FORMAT(V_028044_STENCIL_INVALID);
      terakan_gfx_command_writer_emit_done(command_writer, packet);
      return;
   }

   /* R_028008_DB_DEPTH_VIEW */
   packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028008_DB_DEPTH_VIEW);
   *packet++ = descriptor->view;
   terakan_gfx_command_writer_emit_done(command_writer, packet);

   if (z_bound && stencil_bound) {
      /* Single sequence for both depth and stencil. */
      uint32_t const combined_depth_stencil_register_count =
         (R_02805C_DB_DEPTH_SLICE - R_028040_DB_Z_INFO) / sizeof(uint32_t) + 1;
      packet = terakan_gfx_command_writer_emit_with_bo(
         command_writer, 2 + combined_depth_stencil_register_count, true, 1, 4, 0);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, combined_depth_stencil_register_count, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028040_DB_Z_INFO);
      *packet++ = descriptor->z_info;
      *packet++ = descriptor->stencil_info;
      uint32_t const * const packet_bases = packet;
      /* Read bases. */
      *packet++ = descriptor->z_base;
      *packet++ = descriptor->stencil_base;
      /* Write bases. */
      *packet++ = descriptor->z_base;
      *packet++ = descriptor->stencil_base;
      *packet++ = descriptor->size;
      *packet++ = descriptor->slice;
      /* BO references are valid only for the last terakan_gfx_command_writer_emit. */
      bo_reference =
         terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer, bo,
                                                   true, true, TERAKAN_BO_PRIORITY_DEPTH_BUFFER);
      for (uint32_t relocation_index = 0; relocation_index < 4; ++relocation_index) {
         terakan_gfx_command_writer_add_relocation(
            command_writer, &packet, &packet_bases[relocation_index],
            packet_bases[relocation_index], TERASCALE_WDDM_PATCH_IDS_DB_Z_STENCIL_BASE,
            bo_reference);
      }
      terakan_gfx_command_writer_emit_done(command_writer, packet);
      return;
   }

   /* Either depth or stencil bound. */
   assert(z_bound != stencil_bound);
   {
      uint32_t const packet_dwords =
         /* R_028040_DB_Z_INFO, R_028044_DB_STENCIL_INFO */
         2 + 2 +
         /* R_028048_DB_Z_READ_BASE or R_02804C_DB_STENCIL_READ_BASE */
         2 + 1 +
         /* R_028050_DB_Z_WRITE_BASE or R_028054_DB_STENCIL_WRITE_BASE */
         2 + 1 +
         /* R_028058_DB_DEPTH_SIZE, R_02805C_DB_DEPTH_SLICE */
         2 + 2;
      packet =
         terakan_gfx_command_writer_emit_with_bo(command_writer, packet_dwords, true, 1, 2, 0);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028040_DB_Z_INFO);
      *packet++ = descriptor->z_info;
      *packet++ = descriptor->stencil_info;
      /* BO references are valid only for the last terakan_gfx_command_writer_emit. */
      bo_reference =
         terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer, bo,
                                                   true, true, TERAKAN_BO_PRIORITY_DEPTH_BUFFER);
      /* Read and write bases. */
      uint32_t const base = z_bound ? descriptor->z_base : descriptor->stencil_base;
      uint32_t const base_register_offset =
         z_bound ? TERAKAN_CONTEXT_REG_OFFSET(R_028048_DB_Z_READ_BASE)
                 : TERAKAN_CONTEXT_REG_OFFSET(R_02804C_DB_STENCIL_READ_BASE);
      for (uint32_t base_index = 0; base_index < 2; ++base_index) {
         *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
         *packet++ = base_register_offset + 2 * base_index;
         uint32_t const * const packet_base = packet;
         *packet++ = base;
         terakan_gfx_command_writer_add_relocation(
            command_writer, &packet, packet_base, *packet_base,
            TERASCALE_WDDM_PATCH_IDS_DB_Z_STENCIL_BASE, bo_reference);
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028058_DB_DEPTH_SIZE);
      *packet++ = descriptor->size;
      *packet++ = descriptor->slice;
      terakan_gfx_command_writer_emit_done(command_writer, packet);
   }

   /* TODO(Triang3l): Higher priority for multisampled depth / stencil buffers (possibly pass the
    * sample count via the view not only on R9xx, but on R8xx too, but mask it away here on R8xx).
    */
}

static void
terakan_hw_state_draw_emit_db_render_override(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_02800C_DB_RENDER_OVERRIDE);
   *packet++ = command_writer->hw_state_draw.db_render_override;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_db_stencilrefmask(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 2, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028430_DB_STENCILREFMASK);
   memcpy(packet, command_writer->hw_state_draw.db_stencilrefmask_front_back, sizeof(uint32_t) * 2);
   packet += 2;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_db_depth_control(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028800_DB_DEPTH_CONTROL);
   *packet++ = command_writer->hw_state_draw.db_depth_control;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_db_shader_control(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_02880C_DB_SHADER_CONTROL);
   *packet++ = command_writer->hw_state_draw.db_shader_control;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_cb_target_mask(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028238_CB_TARGET_MASK);
   *packet++ = command_writer->hw_state_draw.cb_target_mask;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_cb_blend_rgba(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 4, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 4, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028414_CB_BLEND_RED);
   memcpy(packet, command_writer->hw_state_draw.cb_blend_rgba, sizeof(float) * 4);
   packet += 4;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_cb_color_control(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028808_CB_COLOR_CONTROL);
   *packet++ = command_writer->hw_state_draw.cb_color_control;

   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

static void
terakan_hw_state_draw_emit_viewport(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (unlikely(state->viewport_counts.needed == 0)) {
      return;
   }

   /* According to RadeonSI and to PAL's Pm4Optimizer, PA_CL_VPORT_X/Y/ZSCALE/OFFSET,
    * PA_SC_VPORT_SCISSOR and PA_SC_VPORT_ZMIN/MAX are "vector" registers, and if any value in the
    * vector is changed, the hardware requires that the entire vector, for all viewports that are
    * used, is emitted.
    *
    * This applies to TeraScale too. Particularly, during the development of Terakan, there was
    * incorrect rendering on Barts when the Z scale/offset was changed and emitted independently of
    * the XY scales/offsets - that was triggered in Sascha Willems's bloom example by implementing
    * depth clear as a draw that has the Z offset set to the clear value, while not modifying the XY
    * transform and not emitting it alongside the updated Z transform, which caused some of the
    * passes to be rendered with incorrect coordinates sometimes, and also occasionally the drawn
    * mesh to have incorrect depth and thus not properly functioning depth test.
    *
    * It's safe to update PA_SC_VPORT_SCISSOR at a frequency different from the rest of the viewport
    * state according to other drivers.
    *
    * Whether PA_CL_VPORT_X/Y/ZSCALE/OFFSET and PA_SC_VPORT_ZMIN/MAX can be updated separately from
    * each other is untested, and no precedent of that has been found in other drivers as of this
    * writing, so for safety, always emitting them together.
    */

   uint32_t * packet;

   if (state->viewport_counts.scale_offset_z_min_max_emitted < state->viewport_counts.needed) {
      packet = terakan_gfx_command_writer_emit(
         command_writer, (2 * 2) + (2 * 3 + 2) * state->viewport_counts.needed, true);
      if (unlikely(packet == NULL)) {
         return;
      }

      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2 * 3 * state->viewport_counts.needed, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_02843C_PA_CL_VPORT_XSCALE_0);
      for (uint8_t viewport_index = 0; viewport_index < state->viewport_counts.needed;
           ++viewport_index) {
         memcpy(packet, state->viewports[viewport_index].pa_cl_vport_xyz_scale_offset,
                sizeof(float) * 2 * 3);
         packet += 2 * 3;
      }

      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2 * state->viewport_counts.needed, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_0282D0_PA_SC_VPORT_ZMIN_0);
      for (uint8_t viewport_index = 0; viewport_index < state->viewport_counts.needed;
           ++viewport_index) {
         memcpy(packet, state->viewports[viewport_index].pa_sc_vport_z_min_max, sizeof(float) * 2);
         packet += 2;
      }

      terakan_gfx_command_writer_emit_done(command_writer, packet);
      state->viewport_counts.scale_offset_z_min_max_emitted = state->viewport_counts.needed;
   }

   if (state->viewport_counts.scissor_emitted < state->viewport_counts.needed) {
      packet = terakan_gfx_command_writer_emit(command_writer,
                                               2 + 2 * state->viewport_counts.needed, true);
      if (unlikely(packet == NULL)) {
         return;
      }

      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 2 * state->viewport_counts.needed, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028250_PA_SC_VPORT_SCISSOR_0_TL);
      for (uint8_t viewport_index = 0; viewport_index < state->viewport_counts.needed;
           ++viewport_index) {
         memcpy(packet, state->viewports[viewport_index].pa_sc_vport_scissor, sizeof(uint32_t) * 2);
         packet += 2;
      }

      terakan_gfx_command_writer_emit_done(command_writer, packet);
      state->viewport_counts.scissor_emitted = state->viewport_counts.needed;
   }
}

static void
terakan_hw_state_draw_emit_cb_blend_control(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   assert(!(state->cb_blend_control.modified & ~state->cb_blend_control.ever_written));
   while (state->cb_blend_control.modified) {
      uint32_t const range_start = (uint32_t)ffs((int)state->cb_blend_control.modified) - 1;
      uint32_t const range_end =
         (uint32_t)ffs((int)(~state->cb_blend_control.modified & ~BITFIELD_MASK(range_start)) |
                       (1 << TERAKAN_COLOR_HW_RTV_COUNT)) -
         1;
      uint32_t const range_length = range_end - range_start;

      uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + range_length, true);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, range_length, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028780_CB_BLEND0_CONTROL) + range_start;
      memcpy(packet, &state->cb_blend_control.cb_blend_control[range_start],
             sizeof(uint32_t) * range_length);
      packet += range_length;
      terakan_gfx_command_writer_emit_done(command_writer, packet);

      state->cb_blend_control.modified &= (uint8_t)~BITFIELD_RANGE(range_start, range_length);
   }
}

static void
terakan_hw_state_draw_emit_cb_color(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   /* DRM Radeon requires ATTRIB relocations regardless of RADEON_CS_KEEP_TILING_FLAGS. */
   bool const need_attrib_relocation =
      terakan_gfx_command_writer_physical_device(command_writer)
         ->submission_info_gfx.base.relocation_type == TERAKAN_QUEUE_RELOCATION_TYPE_DRM_NOP;

   assert(!(state->cb_color.modified & ~state->cb_color.ever_written));
   while (state->cb_color.modified) {
      uint32_t const color_index = (uint32_t)ffs((int)state->cb_color.modified) - 1;

      bool const has_meta = color_index < TERAKAN_COLOR_HW_RTV_COUNT;

      uint32_t const register_offset =
         has_meta ? (R_028C9C_CB_COLOR1_BASE - R_028C60_CB_COLOR0_BASE) * color_index
                  : (R_028E40_CB_COLOR8_BASE - R_028C60_CB_COLOR0_BASE) +
                       (R_028E5C_CB_COLOR9_BASE - R_028E40_CB_COLOR8_BASE) *
                          (color_index - TERAKAN_COLOR_HW_RTV_COUNT);

      struct terakan_bo const * const bo = command_writer->hw_state_draw.cb_color.bo[color_index];
      struct terakan_color_descriptor const * const descriptor =
         &command_writer->hw_state_draw.cb_color.color[color_index];
      if (bo != NULL) {
         uint32_t const register_count =
            (sizeof(struct terakan_color_descriptor) +
             (has_meta ? sizeof(struct terakan_color_meta_descriptor) : 0)) /
            sizeof(uint32_t);

         uint32_t * packet = terakan_gfx_command_writer_emit_with_bo(
            command_writer, 2 + register_count, true, 1,
            1 + (need_attrib_relocation ? 1 : 0) + (has_meta ? 2 : 0), 0);
         if (unlikely(packet == NULL)) {
            return;
         }

         *packet++ = PKT3(PKT3_SET_CONTEXT_REG, register_count, 0);
         *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028C60_CB_COLOR0_BASE + register_offset);

         struct terakan_color_meta_descriptor const * const meta_descriptor =
            has_meta ? &command_writer->hw_state_draw.cb_color.meta[color_index] : NULL;
         /* For single-sampled images, FMASK must be equal to BASE. */
         bool const has_fmask = has_meta && meta_descriptor->fmask != descriptor->base;

         uint32_t const bo_reference = terakan_bo_reference_writer_add_reference(
            &command_writer->base.bo_reference_writer, bo, true, true,
            G_028C70_RAT(descriptor->info) ? TERAKAN_BO_PRIORITY_SHADER_RW_IMAGE
                                           : (has_fmask ? TERAKAN_BO_PRIORITY_COLOR_BUFFER_MS
                                                        : TERAKAN_BO_PRIORITY_COLOR_BUFFER));

         uint32_t const * const packet_descriptor = packet;
         memcpy(packet, descriptor, sizeof(*descriptor));
         packet += sizeof(*descriptor) / sizeof(uint32_t);
         uint32_t const * const packet_meta_descriptor = packet;
         if (has_meta) {
            memcpy(packet, meta_descriptor, sizeof(*meta_descriptor));
            packet += sizeof(*meta_descriptor) / sizeof(uint32_t);
         }

         terakan_gfx_command_writer_add_relocation(
            command_writer, &packet,
            &packet_descriptor[offsetof(struct terakan_color_descriptor, base) / sizeof(uint32_t)],
            packet_descriptor[offsetof(struct terakan_color_descriptor, base) / sizeof(uint32_t)],
            TERASCALE_WDDM_PATCH_IDS_CB_COLOR_BASE | color_index, bo_reference);
         if (need_attrib_relocation) {
            terakan_gfx_command_writer_add_relocation(
               command_writer, &packet,
               &packet_descriptor[offsetof(struct terakan_color_descriptor, attrib) /
                                  sizeof(uint32_t)],
               packet_descriptor[offsetof(struct terakan_color_descriptor, attrib) /
                                 sizeof(uint32_t)],
               0, bo_reference);
         }
         if (has_meta) {
            terakan_gfx_command_writer_add_relocation(
               command_writer, &packet,
               &packet_meta_descriptor[offsetof(struct terakan_color_meta_descriptor, cmask) /
                                       sizeof(uint32_t)],
               packet_meta_descriptor[offsetof(struct terakan_color_meta_descriptor, cmask) /
                                      sizeof(uint32_t)],
               TERASCALE_WDDM_PATCH_IDS_CB_COLOR_CMASK | color_index, bo_reference);
            terakan_gfx_command_writer_add_relocation(
               command_writer, &packet,
               &packet_meta_descriptor[offsetof(struct terakan_color_meta_descriptor, fmask) /
                                       sizeof(uint32_t)],
               packet_meta_descriptor[offsetof(struct terakan_color_meta_descriptor, fmask) /
                                      sizeof(uint32_t)],
               (has_fmask ? TERASCALE_WDDM_PATCH_IDS_CB_COLOR_FMASK
                          : TERASCALE_WDDM_PATCH_IDS_CB_COLOR_BASE) |
                  color_index,
               bo_reference);
         }

         terakan_gfx_command_writer_emit_done(command_writer, packet);
      } else {
         /* Set only the INFO of the unbound target, including the MRT 1 export format for
          * dual-source blending, to the specified value, not requiring any relocations.
          */
         assert(G_028C70_FORMAT(descriptor->info) == TERASCALE_FORMAT_INDEX_INVALID);
         uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
         if (unlikely(packet == NULL)) {
            return;
         }
         *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
         *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028C70_CB_COLOR0_INFO + register_offset);
         *packet++ = descriptor->info;
         terakan_gfx_command_writer_emit_done(command_writer, packet);
      }

      state->cb_color.modified &= ~((uint16_t)1 << color_index);
   }
}

/* TODO(Triang3l): Fast paths for R9xx USE_LS_CONSTS and for R8xx when not tessellation is not
 * enabled on the device.
 */

/* The individual binding emission functions return whether the setting was actually emitted.
 * If false, emission might have been aborted, for instance, due to a new indirect buffer being
 * started and all state having been re-emitted.
 */

static bool
terakan_hw_state_draw_emit_sq_kcache_buffer(
   struct terakan_gfx_command_writer * const command_writer, uint32_t const size_register_offset,
   uint32_t const base_register_offset, uint64_t const wddm_patch_ids,
   struct terakan_hw_state_sq_kcache_buffer const * const buffer)
{
   /* Both size and base must be emitted together if size is not 0 according to the register
    * reference.
    */
   bool const not_empty = buffer->size_lines != 0;

   uint32_t * packet =
      terakan_gfx_command_writer_emit_with_bo(command_writer, 2 + 1 + (not_empty ? 2 + 1 : 0), true,
                                              not_empty ? 1 : 0, not_empty ? 1 : 0, 0);
   if (unlikely(packet == NULL)) {
      return false;
   }

   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = size_register_offset;
   *packet++ = buffer->size_lines;

   if (not_empty) {
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = base_register_offset;
      uint32_t const * const packet_base = packet;
      *packet++ = buffer->va_lines;
      terakan_gfx_command_writer_add_relocation(
         command_writer, &packet, packet_base, *packet_base, wddm_patch_ids,
         terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer,
                                                   buffer->bo, true, false,
                                                   TERAKAN_BO_PRIORITY_UNIFORM_BUFFER));
   }

   terakan_gfx_command_writer_emit_done(command_writer, packet);

   return true;
}

/* bo and descriptor are undefined if not_null == false. */
static bool
terakan_hw_state_draw_emit_resource(struct terakan_gfx_command_writer * const command_writer,
                                    uint32_t const global_index, bool const not_null,
                                    struct terakan_bo const * const bo,
                                    uint32_t const descriptor[8])
{
   uint32_t * packet;

   if (!not_null) {
      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, true);
      if (unlikely(packet == NULL)) {
         return false;
      }
      *packet++ = PKT3(PKT3_SET_CTL_CONST, 1, 0);
      *packet++ = TERAKAN_CTL_CONST_OFFSET(R_03FF04_SQ_TEX_RESOURCE_CLEAR);
      *packet++ = global_index;
      terakan_gfx_command_writer_emit_done(command_writer, packet);
      return true;
   }

   assert(bo != NULL);

   bool const is_texture = G_03001C_TYPE(descriptor[7]) == V_03001C_SQ_TEX_VTX_VALID_TEXTURE;
   bool const is_multisampled =
      is_texture && (G_030000_DIM(descriptor[0]) == V_030000_SQ_TEX_DIM_2D_MSAA ||
                     G_030000_DIM(descriptor[0]) == V_030000_SQ_TEX_DIM_2D_ARRAY_MSAA);
   bool const relocate_mips_or_fmask =
      is_texture && (!is_multisampled || G_03000C_MIP_ADDRESS(descriptor[3]) != 0);

   packet = terakan_gfx_command_writer_emit_with_bo(
      command_writer, 2 + 8, true, 1, is_texture ? 1 + (uint32_t)relocate_mips_or_fmask : 0,
      is_texture ? 0 : 1);
   if (unlikely(packet == NULL)) {
      return false;
   }

   *packet++ = PKT3(PKT3_SET_RESOURCE, 8, 0);
   *packet++ = 8 * global_index;
   uint32_t const * const packet_descriptor = packet;
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
   if (is_texture) {
      terakan_gfx_command_writer_add_relocation(
         command_writer, &packet, &packet_descriptor[2], packet_descriptor[2],
         TERASCALE_WDDM_PATCH_IDS_SQ_TEX_RESOURCE_BASE, bo_reference);
      if (relocate_mips_or_fmask) {
         terakan_gfx_command_writer_add_relocation(
            command_writer, &packet, &packet_descriptor[3], packet_descriptor[3],
            TERASCALE_WDDM_PATCH_IDS_SQ_TEX_RESOURCE_MIP, bo_reference);
      }
   } else {
      terakan_gfx_command_writer_add_relocation_for_40_bits(
         command_writer, &packet, &packet_descriptor[0], &packet_descriptor[2],
         TERASCALE_WDDM_PATCH_IDS_SQ_VTX_CONSTANT_BASE_LO,
         TERASCALE_WDDM_PATCH_IDS_SQ_VTX_CONSTANT_BASE_HI, bo_reference);
   }

   terakan_gfx_command_writer_emit_done(command_writer, packet);

   return true;
}

static bool
terakan_hw_state_draw_emit_sq_sampler(struct terakan_gfx_command_writer * const command_writer,
                                      uint32_t const global_index, uint32_t const sampler[3])
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 3, true);
   if (unlikely(packet == NULL)) {
      return false;
   }
   *packet++ = PKT3(PKT3_SET_SAMPLER, 3, 0);
   *packet++ = 3 * global_index;
   memcpy(packet, sampler, sizeof(uint32_t) * 3);
   packet += 3;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
   return true;
}

static bool
terakan_hw_state_draw_emit_sq_sampler_border_color(
   struct terakan_gfx_command_writer * const command_writer, uint32_t const index_register_offset,
   uint32_t const stage_local_index, float const border_color[4])
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 5, true);
   if (unlikely(packet == NULL)) {
      return false;
   }
   *packet++ = PKT3(PKT3_SET_CONFIG_REG, 5, 0);
   *packet++ = index_register_offset;
   *packet++ = stage_local_index;
   memcpy(packet, border_color, sizeof(float) * 4);
   packet += 4;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
   return true;
}

static void
terakan_hw_state_draw_emit_sq_kcache_for_stage(
   struct terakan_gfx_command_writer * const command_writer, uint32_t const size_register_offset,
   uint32_t const base_register_offset, uint64_t const wddm_patch_ids,
   enum terakan_hw_state_draw_sq_constants_needed_stage const needed_stage,
   enum terakan_hw_state_draw_sq_constants_modified_stage const modified_stage)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   uint16_t * const bindings_modified = &state->sq_constants_modified.kcache[modified_stage];
   uint16_t update_bindings = *bindings_modified & state->sq_constants_needed.kcache[needed_stage];

   uint16_t bindings_remaining = update_bindings;
   int binding_index;
   while ((binding_index = ffs(bindings_remaining) - 1) >= 0) {
      uint16_t const binding_bit = (uint16_t)1 << binding_index;
      bindings_remaining &= ~binding_bit;
      if (!terakan_hw_state_draw_emit_sq_kcache_buffer(
             command_writer, size_register_offset + binding_index,
             base_register_offset + binding_index, wddm_patch_ids | binding_index,
             &state->sq_kcache_buffers[needed_stage][binding_index])) {
         return;
      }
   }

   *bindings_modified &= ~update_bindings;
}

static void
terakan_hw_state_draw_emit_sq_resources_for_stage(
   struct terakan_gfx_command_writer * const command_writer, uint32_t const global_offset,
   uint32_t const count, BITSET_WORD const * const not_null_bitset,
   struct terakan_bo const * const * const bos, uint32_t const * const descriptors,
   BITSET_WORD const * const needed_bitset, BITSET_WORD * const modified_bitset)
{
   unsigned const word_count = BITSET_WORDS(count);

   BITSET_DECLARE(update_bindings, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE);
   for (unsigned word_index = 0; word_index < word_count; ++word_index) {
      update_bindings[word_index] = modified_bitset[word_index] & needed_bitset[word_index];
   }

   unsigned binding_index;
   BITSET_FOREACH_SET (binding_index, update_bindings, count) {
      if (!terakan_hw_state_draw_emit_resource(command_writer, global_offset + binding_index,
                                               BITSET_TEST(not_null_bitset, binding_index),
                                               bos[binding_index],
                                               descriptors + 8 * binding_index)) {
         return;
      }
      BITSET_CLEAR(modified_bitset, binding_index);
   }
}

static void
terakan_hw_state_draw_emit_sq_samplers_for_stage(
   struct terakan_gfx_command_writer * const command_writer, uint32_t const global_offset,
   enum terakan_hw_state_draw_sq_constants_needed_stage const needed_stage,
   enum terakan_hw_state_draw_sq_constants_modified_stage const modified_stage)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   uint32_t * const bindings_modified = &state->sq_constants_modified.samplers[modified_stage];
   uint32_t update_bindings =
      *bindings_modified & state->sq_constants_needed.samplers[needed_stage];

   uint32_t bindings_remaining = update_bindings;
   int binding_index;
   while ((binding_index = ffs(bindings_remaining) - 1) >= 0) {
      uint32_t const binding_bit = (uint16_t)1 << binding_index;
      bindings_remaining &= ~binding_bit;
      if (!terakan_hw_state_draw_emit_sq_sampler(command_writer, global_offset + binding_index,
                                                 state->sq_samplers[needed_stage][binding_index])) {
         return;
      }
   }

   *bindings_modified &= ~update_bindings;
}

static void
terakan_hw_state_draw_emit_sq_sampler_border_colors_for_stage(
   struct terakan_gfx_command_writer * const command_writer, uint32_t const index_register_offset,
   enum terakan_hw_state_draw_sq_constants_needed_stage const needed_stage,
   enum terakan_hw_state_draw_sq_constants_modified_stage const modified_stage)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   uint32_t * const bindings_modified =
      &state->sq_constants_modified.sampler_border_colors[modified_stage];
   uint32_t update_bindings = *bindings_modified &
                              state->sq_samplers_with_border_color[needed_stage] &
                              state->sq_constants_needed.samplers[needed_stage];

   uint32_t bindings_remaining = update_bindings;
   int binding_index;
   while ((binding_index = ffs(bindings_remaining) - 1) >= 0) {
      uint32_t const binding_bit = (uint16_t)1 << binding_index;
      bindings_remaining &= ~binding_bit;
      if (!terakan_hw_state_draw_emit_sq_sampler_border_color(
             command_writer, index_register_offset, binding_index,
             state->sq_sampler_border_colors[needed_stage][binding_index])) {
         return;
      }
   }

   *bindings_modified &= ~update_bindings;
}

static void
terakan_hw_state_draw_emit_sq_kcache_vs(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (state->sq_constants_needed.tcs_tes) {
      terakan_hw_state_draw_emit_sq_kcache_for_stage(
         command_writer, TERAKAN_CONTEXT_REG_OFFSET(R_028FC0_ALU_CONST_BUFFER_SIZE_LS_0),
         TERAKAN_CONTEXT_REG_OFFSET(R_028F40_ALU_CONST_CACHE_LS_0),
         TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_LS_VS,
         TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
         TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS);
      return;
   }

   /* TODO(Triang3l): Fast path for tessellation not enabled on the device. */

   uint16_t const bindings_modified =
      state->sq_constants_modified
         .kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES];
   uint16_t const update_bindings =
      (bindings_modified | state->sq_constants_for_vs_overwritten_in_vses_by_tes.kcache) &
      state->sq_constants_needed.kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS];

   uint16_t bindings_remaining = update_bindings;
   int binding_index;
   while ((binding_index = ffs(bindings_remaining) - 1) >= 0) {
      uint16_t const binding_bit = (uint16_t)1 << binding_index;
      bindings_remaining &= ~binding_bit;

      bool emit_binding = false;

      struct terakan_hw_state_sq_kcache_buffer const * const buffer =
         &state
             ->sq_kcache_buffers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS][binding_index];

      if (bindings_modified & binding_bit) {
         /* Completely new binding needed for the Vulkan VS stage. */
         emit_binding = true;
      } else {
         /* The previous VS binding is still up to date, but VSES bindings now include the TES
          * binding instead, which may be different, or the same (VS | TES bindings in the
          * beginning of the pipeline layout, may happen in applications having a common descriptor
          * set referenced by all shaders).
          */
         struct terakan_hw_state_sq_kcache_buffer const * const tes_buffer =
            &state->sq_kcache_buffers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES]
                                     [binding_index];
         if (buffer->size_lines != tes_buffer->size_lines ||
             (buffer->size_lines &&
              (buffer->bo != tes_buffer->bo || buffer->va_lines != tes_buffer->va_lines))) {
            /* VS and TES bindings at this index are different, emit the VS binding now, and emit
             * the TES binding next time it's needed.
             */
            emit_binding = true;
            state->sq_constants_for_tes_overwritten_in_vses_by_vs.kcache |= binding_bit;
         } else {
            /* VS and TES bindings at this index are the same, continue using the binding for both
             * VS for this emission and TES later.
             */
            state->sq_constants_for_tes_overwritten_in_vses_by_vs.kcache &= ~binding_bit;
         }
      }

      if (emit_binding &&
          !terakan_hw_state_draw_emit_sq_kcache_buffer(
             command_writer,
             TERAKAN_CONTEXT_REG_OFFSET(R_028180_ALU_CONST_BUFFER_SIZE_VS_0) + binding_index,
             TERAKAN_CONTEXT_REG_OFFSET(R_028980_ALU_CONST_CACHE_VS_0) + binding_index,
             TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_LS_VS | binding_index, buffer)) {
         return;
      }
   }

   state->sq_constants_modified
      .kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES] &= ~update_bindings;
   state->sq_constants_for_vs_overwritten_in_vses_by_tes.kcache &= ~update_bindings;
   /* Next time the hardware constants now used for new VS bindings are needed for TES, make sure
    * the TES ones are applied.
    * Simply mark as overwritten for now to avoid overhead within non-tesellated draws, defer all
    * more complex logic until tessellation is enabled.
    */
   state->sq_constants_for_tes_overwritten_in_vses_by_vs.kcache |=
      update_bindings & bindings_modified;
}

static void
terakan_hw_state_draw_emit_sq_resources_vs(struct terakan_gfx_command_writer * const command_writer)
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

   BITSET_DECLARE(update_bindings, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
   for (unsigned word_index = 0; word_index < BITSET_WORDS(TERAKAN_RESOURCE_HW_COUNT_VERTEX);
        ++word_index) {
      update_bindings[word_index] =
         (state->sq_constants_modified.resources.vs_in_vses[word_index] |
          state->sq_constants_for_vs_overwritten_in_vses_by_tes.resources[word_index]) &
         state->sq_constants_needed.resources.vs[word_index];
   }

   unsigned binding_index;
   BITSET_FOREACH_SET (binding_index, update_bindings, TERAKAN_RESOURCE_HW_COUNT_VERTEX) {
      bool emit_binding = false;

      bool const resource_not_null = BITSET_TEST(state->sq_resources_not_null.vs, binding_index);

      if (BITSET_TEST(state->sq_constants_modified.resources.vs_in_vses, binding_index)) {
         emit_binding = true;
         BITSET_CLEAR(state->sq_constants_modified.resources.vs_in_vses, binding_index);
         BITSET_SET(state->sq_constants_for_tes_overwritten_in_vses_by_vs.resources, binding_index);
      } else {
         if ((resource_not_null != BITSET_TEST(state->sq_resources_not_null.tes, binding_index)) ||
             (resource_not_null && (state->sq_resource_bos.vs[binding_index] !=
                                       state->sq_resource_bos.tes[binding_index] ||
                                    memcmp(state->sq_resource_descriptors.vs[binding_index],
                                           state->sq_resource_descriptors.tes[binding_index],
                                           sizeof(uint32_t) * 8) != 0))) {
            emit_binding = true;
            BITSET_SET(state->sq_constants_for_tes_overwritten_in_vses_by_vs.resources,
                       binding_index);
         } else {
            BITSET_CLEAR(state->sq_constants_for_tes_overwritten_in_vses_by_vs.resources,
                         binding_index);
         }
      }

      if (emit_binding && !terakan_hw_state_draw_emit_resource(
                             command_writer, TERAKAN_RESOURCE_HW_OFFSET_VSES + binding_index,
                             resource_not_null, state->sq_resource_bos.vs[binding_index],
                             state->sq_resource_descriptors.vs[binding_index])) {
         return;
      }

      BITSET_CLEAR(state->sq_constants_for_vs_overwritten_in_vses_by_tes.resources, binding_index);
   }
}

static void
terakan_hw_state_draw_emit_sq_samplers_vs(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (state->sq_constants_needed.tcs_tes) {
      terakan_hw_state_draw_emit_sq_samplers_for_stage(
         command_writer, TERAKAN_SAMPLER_HW_OFFSET_LS,
         TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
         TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS);
      return;
   }

   /* TODO(Triang3l): Fast path for tessellation not enabled on the device. */

   uint32_t const bindings_modified =
      state->sq_constants_modified
         .samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES];
   uint32_t const update_bindings =
      (bindings_modified | state->sq_constants_for_vs_overwritten_in_vses_by_tes.samplers) &
      state->sq_constants_needed.samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS];

   uint32_t bindings_remaining = update_bindings;
   int binding_index;
   while ((binding_index = ffs(bindings_remaining) - 1) >= 0) {
      uint32_t const binding_bit = (uint32_t)1 << binding_index;
      bindings_remaining &= ~binding_bit;

      bool emit_binding = false;

      uint32_t const * const sampler =
         state->sq_samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS][binding_index];

      if (bindings_modified & binding_bit) {
         emit_binding = true;
      } else {
         if (memcmp(sampler,
                    state->sq_samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES]
                                      [binding_index],
                    sizeof(uint32_t) * 3) != 0) {
            emit_binding = true;
            state->sq_constants_for_tes_overwritten_in_vses_by_vs.samplers |= binding_bit;
         } else {
            state->sq_constants_for_tes_overwritten_in_vses_by_vs.samplers &= ~binding_bit;
         }
      }

      if (emit_binding &&
          !terakan_hw_state_draw_emit_sq_sampler(
             command_writer, TERAKAN_SAMPLER_HW_OFFSET_VSES + binding_index, sampler)) {
         return;
      }
   }

   state->sq_constants_modified
      .samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES] &= ~update_bindings;
   state->sq_constants_for_vs_overwritten_in_vses_by_tes.samplers &= ~update_bindings;
   state->sq_constants_for_tes_overwritten_in_vses_by_vs.samplers |=
      update_bindings & bindings_modified;
}

static void
terakan_hw_state_draw_emit_sq_sampler_border_colors_vs(
   struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (state->sq_constants_needed.tcs_tes) {
      terakan_hw_state_draw_emit_sq_sampler_border_colors_for_stage(
         command_writer, TERAKAN_CONFIG_REG_OFFSET(R_00A450_TD_LS_SAMPLER0_BORDER_COLOR_INDEX),
         TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
         TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS);
      return;
   }

   /* TODO(Triang3l): Fast path for tessellation not enabled on the device. */

   uint32_t const bindings_modified =
      state->sq_constants_modified
         .sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES];
   uint32_t const update_bindings =
      (bindings_modified |
       state->sq_constants_for_vs_overwritten_in_vses_by_tes.sampler_border_colors) &
      state->sq_samplers_with_border_color[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS] &
      state->sq_constants_needed.samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS];

   uint32_t bindings_remaining = update_bindings;
   int binding_index;
   while ((binding_index = ffs(bindings_remaining) - 1) >= 0) {
      uint32_t const binding_bit = (uint32_t)1 << binding_index;
      bindings_remaining &= ~binding_bit;

      bool emit_binding = false;

      float const * const border_color =
         state->sq_sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS]
                                        [binding_index];

      if (bindings_modified & binding_bit) {
         emit_binding = true;
      } else {
         if (memcmp(
                border_color,
                state->sq_sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES]
                                               [binding_index],
                sizeof(float) * 4) != 0) {
            emit_binding = true;
            state->sq_constants_for_tes_overwritten_in_vses_by_vs.sampler_border_colors |=
               binding_bit;
         } else {
            state->sq_constants_for_tes_overwritten_in_vses_by_vs.sampler_border_colors &=
               ~binding_bit;
         }
      }

      if (emit_binding &&
          !terakan_hw_state_draw_emit_sq_sampler_border_color(
             command_writer, TERAKAN_CONFIG_REG_OFFSET(R_00A414_TD_VS_SAMPLER0_BORDER_INDEX),
             binding_index, border_color)) {
         return;
      }
   }

   state->sq_constants_modified
      .sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES] &=
      ~update_bindings;
   state->sq_constants_for_vs_overwritten_in_vses_by_tes.sampler_border_colors &= ~update_bindings;
   state->sq_constants_for_tes_overwritten_in_vses_by_vs.sampler_border_colors |=
      update_bindings & bindings_modified;
}

static void
terakan_hw_state_draw_emit_sq_kcache_tcs(struct terakan_gfx_command_writer * const command_writer)
{
   if (!command_writer->hw_state_draw.sq_constants_needed.tcs_tes) {
      return;
   }
   terakan_hw_state_draw_emit_sq_kcache_for_stage(
      command_writer, TERAKAN_CONTEXT_REG_OFFSET(R_028F80_ALU_CONST_BUFFER_SIZE_HS_0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028F00_ALU_CONST_CACHE_HS_0),
      TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_HS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TCS);
}

static void
terakan_hw_state_draw_emit_sq_resources_tcs(struct terakan_gfx_command_writer * const command_writer)
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
terakan_hw_state_draw_emit_sq_samplers_tcs(struct terakan_gfx_command_writer * const command_writer)
{
   if (!command_writer->hw_state_draw.sq_constants_needed.tcs_tes) {
      return;
   }
   terakan_hw_state_draw_emit_sq_samplers_for_stage(
      command_writer, TERAKAN_SAMPLER_HW_OFFSET_HS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TCS);
}

static void
terakan_hw_state_draw_emit_sq_sampler_border_colors_tcs(
   struct terakan_gfx_command_writer * const command_writer)
{
   if (!command_writer->hw_state_draw.sq_constants_needed.tcs_tes) {
      return;
   }
   terakan_hw_state_draw_emit_sq_sampler_border_colors_for_stage(
      command_writer, TERAKAN_CONFIG_REG_OFFSET(R_00A43C_TD_HS_SAMPLER0_BORDER_COLOR_INDEX),
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TCS);
}

static void
terakan_hw_state_draw_emit_sq_kcache_tes(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (!state->sq_constants_needed.tcs_tes) {
      return;
   }

   /* TODO(Triang3l): Fast path for R9xx USE_LS_CONSTS. */

   uint16_t const bindings_modified =
      state->sq_constants_modified
         .kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES];
   uint16_t const update_bindings =
      (bindings_modified | state->sq_constants_for_tes_overwritten_in_vses_by_vs.kcache) &
      state->sq_constants_needed.kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES];

   uint16_t bindings_remaining = update_bindings;
   int binding_index;
   while ((binding_index = ffs(bindings_remaining) - 1) >= 0) {
      uint16_t const binding_bit = (uint16_t)1 << binding_index;
      bindings_remaining &= ~binding_bit;

      bool emit_binding = false;

      struct terakan_hw_state_sq_kcache_buffer const * const buffer =
         &state->sq_kcache_buffers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES]
                                  [binding_index];

      if (bindings_modified & binding_bit) {
         /* Completely new binding needed for the Vulkan TES stage. */
         emit_binding = true;
      } else {
         /* The previous TES binding is still up to date, but VSES bindings now include the VS
          * binding instead, which may be different, or the same (VS | TES bindings in the
          * beginning of the pipeline layout, may happen in applications having a common descriptor
          * set referenced by all shaders).
          */
         struct terakan_hw_state_sq_kcache_buffer const * const vs_buffer =
            &state->sq_kcache_buffers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS]
                                     [binding_index];
         if (buffer->size_lines != vs_buffer->size_lines ||
             (buffer->size_lines &&
              (buffer->bo != vs_buffer->bo || buffer->va_lines != vs_buffer->va_lines))) {
            /* VS and TES bindings at this index are different, emit the TES buffer now, and emit
             * the VS binding next time it's needed.
             */
            emit_binding = true;
            state->sq_constants_for_vs_overwritten_in_vses_by_tes.kcache |= binding_bit;
         } else {
            /* VS and TES bindings at this index are the same, continue using the binding for both
             * TES for this emission and VS later.
             */
            state->sq_constants_for_vs_overwritten_in_vses_by_tes.kcache &= ~binding_bit;
         }
      }

      if (emit_binding &&
          !terakan_hw_state_draw_emit_sq_kcache_buffer(
             command_writer,
             TERAKAN_CONTEXT_REG_OFFSET(R_028180_ALU_CONST_BUFFER_SIZE_VS_0) + binding_index,
             TERAKAN_CONTEXT_REG_OFFSET(R_028980_ALU_CONST_CACHE_VS_0) + binding_index,
             TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_LS_VS | binding_index, buffer)) {
         return;
      }
   }

   state->sq_constants_modified
      .kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES] &= ~update_bindings;
   state->sq_constants_for_tes_overwritten_in_vses_by_vs.kcache &= ~update_bindings;
   /* Next time the hardware constants now used for new TES bindings are needed for VS, make sure
    * the VS ones are applied.
    * Simply mark as overwritten for now to avoid overhead within tesellated draws, defer all more
    * complex logic until tessellation is disabled.
    */
   state->sq_constants_for_vs_overwritten_in_vses_by_tes.kcache |=
      update_bindings & bindings_modified;
}

static void
terakan_hw_state_draw_emit_sq_resources_tes(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (!state->sq_constants_needed.tcs_tes) {
      return;
   }

   /* TODO(Triang3l): Fast path for R9xx USE_LS_CONSTS. */

   BITSET_DECLARE(update_bindings, TERAKAN_RESOURCE_HW_COUNT_VERTEX);
   for (unsigned word_index = 0; word_index < BITSET_WORDS(TERAKAN_RESOURCE_HW_COUNT_VERTEX);
        ++word_index) {
      update_bindings[word_index] =
         (state->sq_constants_modified.resources.tes_in_vses[word_index] |
          state->sq_constants_for_tes_overwritten_in_vses_by_vs.resources[word_index]) &
         state->sq_constants_needed.resources.tes[word_index];
   }

   unsigned binding_index;
   BITSET_FOREACH_SET (binding_index, update_bindings, TERAKAN_RESOURCE_HW_COUNT_VERTEX) {
      bool emit_binding = false;

      bool const resource_not_null = BITSET_TEST(state->sq_resources_not_null.tes, binding_index);

      if (BITSET_TEST(state->sq_constants_modified.resources.tes_in_vses, binding_index)) {
         emit_binding = true;
         BITSET_CLEAR(state->sq_constants_modified.resources.tes_in_vses, binding_index);
         BITSET_SET(state->sq_constants_for_vs_overwritten_in_vses_by_tes.resources, binding_index);
      } else {
         if ((resource_not_null != BITSET_TEST(state->sq_resources_not_null.vs, binding_index)) ||
             (resource_not_null && (state->sq_resource_bos.tes[binding_index] !=
                                       state->sq_resource_bos.vs[binding_index] ||
                                    memcmp(state->sq_resource_descriptors.tes[binding_index],
                                           state->sq_resource_descriptors.vs[binding_index],
                                           sizeof(uint32_t) * 8) != 0))) {
            emit_binding = true;
            BITSET_SET(state->sq_constants_for_vs_overwritten_in_vses_by_tes.resources,
                       binding_index);
         } else {
            BITSET_CLEAR(state->sq_constants_for_vs_overwritten_in_vses_by_tes.resources,
                         binding_index);
         }
      }

      if (emit_binding && !terakan_hw_state_draw_emit_resource(
                             command_writer, TERAKAN_RESOURCE_HW_OFFSET_VSES + binding_index,
                             resource_not_null, state->sq_resource_bos.tes[binding_index],
                             state->sq_resource_descriptors.tes[binding_index])) {
         return;
      }

      BITSET_CLEAR(state->sq_constants_for_tes_overwritten_in_vses_by_vs.resources, binding_index);
   }
}

static void
terakan_hw_state_draw_emit_sq_samplers_tes(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (!state->sq_constants_needed.tcs_tes) {
      return;
   }

   /* TODO(Triang3l): Fast path for R9xx USE_LS_CONSTS. */

   uint32_t const bindings_modified =
      state->sq_constants_modified
         .samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES];
   uint32_t const update_bindings =
      (bindings_modified | state->sq_constants_for_tes_overwritten_in_vses_by_vs.samplers) &
      state->sq_constants_needed.samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES];

   uint32_t bindings_remaining = update_bindings;
   int binding_index;
   while ((binding_index = ffs(bindings_remaining) - 1) >= 0) {
      uint32_t const binding_bit = (uint32_t)1 << binding_index;
      bindings_remaining &= ~binding_bit;

      bool emit_binding = false;

      uint32_t const * const sampler =
         state->sq_samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES][binding_index];

      if (bindings_modified & binding_bit) {
         emit_binding = true;
      } else {
         if (memcmp(sampler,
                    state->sq_samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS]
                                      [binding_index],
                    sizeof(uint32_t) * 3) != 0) {
            emit_binding = true;
            state->sq_constants_for_vs_overwritten_in_vses_by_tes.samplers |= binding_bit;
         } else {
            state->sq_constants_for_vs_overwritten_in_vses_by_tes.samplers &= ~binding_bit;
         }
      }

      if (emit_binding &&
          !terakan_hw_state_draw_emit_sq_sampler(
             command_writer, TERAKAN_SAMPLER_HW_OFFSET_VSES + binding_index, sampler)) {
         return;
      }
   }

   state->sq_constants_modified
      .samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES] &= ~update_bindings;
   state->sq_constants_for_tes_overwritten_in_vses_by_vs.samplers &= ~update_bindings;
   state->sq_constants_for_vs_overwritten_in_vses_by_tes.samplers |=
      update_bindings & bindings_modified;
}

static void
terakan_hw_state_draw_emit_sq_sampler_border_colors_tes(
   struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;

   if (!state->sq_constants_needed.tcs_tes) {
      return;
   }

   /* TODO(Triang3l): Fast path for R9xx USE_LS_CONSTS. */

   uint32_t const bindings_modified =
      state->sq_constants_modified
         .sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES];
   uint32_t const update_bindings =
      (bindings_modified |
       state->sq_constants_for_tes_overwritten_in_vses_by_vs.sampler_border_colors) &
      state->sq_samplers_with_border_color[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES] &
      state->sq_constants_needed.samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES];

   uint32_t bindings_remaining = update_bindings;
   int binding_index;
   while ((binding_index = ffs(bindings_remaining) - 1) >= 0) {
      uint32_t const binding_bit = (uint32_t)1 << binding_index;
      bindings_remaining &= ~binding_bit;

      bool emit_binding = false;

      float const * const border_color =
         state->sq_sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES]
                                        [binding_index];

      if (bindings_modified & binding_bit) {
         emit_binding = true;
      } else {
         if (memcmp(
                border_color,
                state->sq_sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS]
                                               [binding_index],
                sizeof(float) * 4) != 0) {
            emit_binding = true;
            state->sq_constants_for_vs_overwritten_in_vses_by_tes.sampler_border_colors |=
               binding_bit;
         } else {
            state->sq_constants_for_vs_overwritten_in_vses_by_tes.sampler_border_colors &=
               ~binding_bit;
         }
      }

      if (emit_binding &&
          !terakan_hw_state_draw_emit_sq_sampler_border_color(
             command_writer, TERAKAN_CONFIG_REG_OFFSET(R_00A414_TD_VS_SAMPLER0_BORDER_INDEX),
             binding_index, border_color)) {
         return;
      }
   }

   state->sq_constants_modified
      .sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES] &=
      ~update_bindings;
   state->sq_constants_for_tes_overwritten_in_vses_by_vs.sampler_border_colors &= ~update_bindings;
   state->sq_constants_for_vs_overwritten_in_vses_by_tes.sampler_border_colors |=
      update_bindings & bindings_modified;
}

static void
terakan_hw_state_draw_emit_sq_kcache_gs(struct terakan_gfx_command_writer * const command_writer)
{
   if (!terakan_hw_state_draw_sq_constants_needed_by_gs(&command_writer->hw_state_draw)) {
      return;
   }
   terakan_hw_state_draw_emit_sq_kcache_for_stage(
      command_writer, TERAKAN_CONTEXT_REG_OFFSET(R_0281C0_ALU_CONST_BUFFER_SIZE_GS_0),
      TERAKAN_CONTEXT_REG_OFFSET(R_0289C0_ALU_CONST_CACHE_GS_0),
      TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_GS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_GS);
}

static void
terakan_hw_state_draw_emit_sq_resources_gs(struct terakan_gfx_command_writer * const command_writer)
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
terakan_hw_state_draw_emit_sq_samplers_gs(struct terakan_gfx_command_writer * const command_writer)
{
   if (!terakan_hw_state_draw_sq_constants_needed_by_gs(&command_writer->hw_state_draw)) {
      return;
   }
   terakan_hw_state_draw_emit_sq_samplers_for_stage(
      command_writer, TERAKAN_SAMPLER_HW_OFFSET_GS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_GS);
}

static void
terakan_hw_state_draw_emit_sq_sampler_border_colors_gs(
   struct terakan_gfx_command_writer * const command_writer)
{
   if (!terakan_hw_state_draw_sq_constants_needed_by_gs(&command_writer->hw_state_draw)) {
      return;
   }
   terakan_hw_state_draw_emit_sq_sampler_border_colors_for_stage(
      command_writer, TERAKAN_CONFIG_REG_OFFSET(R_00A428_TD_GS_SAMPLER0_BORDER_INDEX),
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_GS);
}

static void
terakan_hw_state_draw_emit_sq_kcache_fs(struct terakan_gfx_command_writer * const command_writer)
{
   terakan_hw_state_draw_emit_sq_kcache_for_stage(
      command_writer, TERAKAN_CONTEXT_REG_OFFSET(R_028140_ALU_CONST_BUFFER_SIZE_PS_0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028940_ALU_CONST_CACHE_PS_0),
      TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_PS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_FS);
}

static void
terakan_hw_state_draw_emit_sq_resources_fs(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;
   terakan_hw_state_draw_emit_sq_resources_for_stage(
      command_writer, TERAKAN_RESOURCE_HW_OFFSET_PS, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE,
      state->sq_resources_not_null.fs, state->sq_resource_bos.fs,
      state->sq_resource_descriptors.fs[0], state->sq_constants_needed.resources.fs,
      state->sq_constants_modified.resources.fs);
}

static void
terakan_hw_state_draw_emit_sq_samplers_fs(struct terakan_gfx_command_writer * const command_writer)
{
   terakan_hw_state_draw_emit_sq_samplers_for_stage(
      command_writer, TERAKAN_SAMPLER_HW_OFFSET_PS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_FS);
}

static void
terakan_hw_state_draw_emit_sq_sampler_border_colors_fs(
   struct terakan_gfx_command_writer * const command_writer)
{
   terakan_hw_state_draw_emit_sq_sampler_border_colors_for_stage(
      command_writer, TERAKAN_CONFIG_REG_OFFSET(R_00A400_TD_PS_SAMPLER0_BORDER_INDEX),
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_FS);
}

static void
terakan_hw_state_draw_emit_sq_resources_vi(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_hw_state_draw * const state = &command_writer->hw_state_draw;
   terakan_hw_state_draw_emit_sq_resources_for_stage(
      command_writer, TERAKAN_RESOURCE_HW_OFFSET_FS, TERAKAN_RESOURCE_HW_COUNT_FETCH,
      state->sq_resources_not_null.vi, state->sq_resource_bos.vi,
      state->sq_resource_descriptors.vi[0], state->sq_constants_needed.resources.vi,
      state->sq_constants_modified.resources.vi);
}

static terakan_hw_state_draw_emit_function const
   terakan_hw_state_draw_emit_functions[TERAKAN_HW_STATE_DRAW_INDEX_COUNT] = {
      [TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_TYPE] = terakan_hw_state_draw_emit_vgt_index_type,
      [TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_BUFFER] = terakan_hw_state_draw_emit_vgt_index_buffer,
      [TERAKAN_HW_STATE_DRAW_INDEX_VGT_PRIMITIVE_TYPE] =
         terakan_hw_state_draw_emit_vgt_primitive_type,
      [TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_OFFSET] = terakan_hw_state_draw_emit_vgt_index_offset,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_FS] = terakan_hw_state_draw_emit_sq_pgm_fs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_VS] = terakan_hw_state_draw_emit_sq_pgm_vs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_PS] = terakan_hw_state_draw_emit_sq_pgm_ps,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_VTX_START_INST_LOC] =
         terakan_hw_state_draw_emit_sq_vtx_start_inst_loc,
      [TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL] = terakan_hw_state_draw_emit_pa_cl_clip_cntl,
      [TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_SC_MODE_CNTL] =
         terakan_hw_state_draw_emit_pa_su_sc_mode_cntl,
      [TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_VTE_CNTL] = terakan_hw_state_draw_emit_pa_cl_vte_cntl,
      [TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_MODE_CNTL_0] =
         terakan_hw_state_draw_emit_pa_sc_mode_cntl_0,
      [TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_DB_FMT_CNTL] =
         terakan_hw_state_draw_emit_pa_su_poly_offset_db_fmt_cntl,
      [TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_CLAMP_SCALE_OFFSET] =
         terakan_hw_state_draw_emit_pa_su_poly_offset_clamp_scale_offset,
      [TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_GB] = terakan_hw_state_draw_emit_pa_cl_gb,
      [TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_AA_SAMPLES] = terakan_hw_state_draw_emit_pa_sc_aa_samples,
      [TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_AA_MASK] = terakan_hw_state_draw_emit_pa_sc_aa_mask,
      [TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER] =
         terakan_hw_state_draw_emit_db_depth_stencil_buffer,
      [TERAKAN_HW_STATE_DRAW_INDEX_DB_RENDER_OVERRIDE] =
         terakan_hw_state_draw_emit_db_render_override,
      [TERAKAN_HW_STATE_DRAW_INDEX_DB_STENCILREFMASK] =
         terakan_hw_state_draw_emit_db_stencilrefmask,
      [TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_CONTROL] = terakan_hw_state_draw_emit_db_depth_control,
      [TERAKAN_HW_STATE_DRAW_INDEX_DB_SHADER_CONTROL] =
         terakan_hw_state_draw_emit_db_shader_control,
      [TERAKAN_HW_STATE_DRAW_INDEX_CB_TARGET_MASK] = terakan_hw_state_draw_emit_cb_target_mask,
      [TERAKAN_HW_STATE_DRAW_INDEX_CB_BLEND_RGBA] = terakan_hw_state_draw_emit_cb_blend_rgba,
      [TERAKAN_HW_STATE_DRAW_INDEX_CB_COLOR_CONTROL] = terakan_hw_state_draw_emit_cb_color_control,
      [TERAKAN_HW_STATE_DRAW_INDEX_VIEWPORT] = terakan_hw_state_draw_emit_viewport,
      [TERAKAN_HW_STATE_DRAW_INDEX_CB_BLEND_CONTROL] = terakan_hw_state_draw_emit_cb_blend_control,
      [TERAKAN_HW_STATE_DRAW_INDEX_CB_COLOR] = terakan_hw_state_draw_emit_cb_color,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_VS] = terakan_hw_state_draw_emit_sq_kcache_vs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_TCS] = terakan_hw_state_draw_emit_sq_kcache_tcs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_TES] = terakan_hw_state_draw_emit_sq_kcache_tes,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_GS] = terakan_hw_state_draw_emit_sq_kcache_gs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_FS] = terakan_hw_state_draw_emit_sq_kcache_fs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VI] = terakan_hw_state_draw_emit_sq_resources_vi,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VS] = terakan_hw_state_draw_emit_sq_resources_vs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_TCS] = terakan_hw_state_draw_emit_sq_resources_tcs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_TES] = terakan_hw_state_draw_emit_sq_resources_tes,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_GS] = terakan_hw_state_draw_emit_sq_resources_gs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_FS] = terakan_hw_state_draw_emit_sq_resources_fs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_VS] = terakan_hw_state_draw_emit_sq_samplers_vs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_TCS] = terakan_hw_state_draw_emit_sq_samplers_tcs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_TES] = terakan_hw_state_draw_emit_sq_samplers_tes,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_GS] = terakan_hw_state_draw_emit_sq_samplers_gs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_FS] = terakan_hw_state_draw_emit_sq_samplers_fs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_VS] =
         terakan_hw_state_draw_emit_sq_sampler_border_colors_vs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_TCS] =
         terakan_hw_state_draw_emit_sq_sampler_border_colors_tcs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_TES] =
         terakan_hw_state_draw_emit_sq_sampler_border_colors_tes,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_GS] =
         terakan_hw_state_draw_emit_sq_sampler_border_colors_gs,
      [TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_FS] =
         terakan_hw_state_draw_emit_sq_sampler_border_colors_fs,
};

static void
terakan_hw_state_draw_set_cb_color_impl(struct terakan_hw_state_draw * const state,
                                        uint32_t const color_index,
                                        struct terakan_bo const * const bo,
                                        struct terakan_color_descriptor const * const color,
                                        struct terakan_color_meta_descriptor const * meta,
                                        uint32_t const unbound_info)
{
   assert(color_index < ARRAY_SIZE(state->cb_color.color));
   uint16_t const color_bit = (uint16_t)1 << color_index;

   assert(G_028C70_FORMAT(unbound_info) == TERASCALE_FORMAT_INDEX_INVALID);

   struct terakan_color_meta_descriptor disabled_meta;
   if (meta == NULL && bo != NULL) {
      disabled_meta = terakan_color_meta_descriptor_create_disabled(color);
      meta = &disabled_meta;
   }

   bool modified =
      !(state->cb_color.ever_written & color_bit) || state->cb_color.bo[color_index] != bo;
   if (!modified) {
      if (bo != NULL) {
         modified = memcmp(&state->cb_color.color[color_index], color,
                           sizeof(struct terakan_color_descriptor)) != 0;
         if (!modified && color_index < TERAKAN_COLOR_HW_RTV_COUNT) {
            modified = memcmp(&state->cb_color.meta[color_index], meta,
                              sizeof(struct terakan_color_meta_descriptor)) != 0;
         }
      } else {
         modified = state->cb_color.color[color_index].info != unbound_info;
      }
      if (!modified) {
         return;
      }
   }

   state->cb_color.bo[color_index] = bo;
   if (bo != NULL) {
      memcpy(&state->cb_color.color[color_index], color, sizeof(struct terakan_color_descriptor));
      if (color_index < TERAKAN_COLOR_HW_RTV_COUNT) {
         memcpy(&state->cb_color.meta[color_index], meta,
                sizeof(struct terakan_color_meta_descriptor));
      }
   } else {
      state->cb_color.color[color_index].info = unbound_info;
   }

   state->cb_color.ever_written |= color_bit;
   state->cb_color.modified |= color_bit;
   BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_CB_COLOR);
}

void
terakan_hw_state_draw_set_cb_color(struct terakan_hw_state_draw * const state,
                                   uint32_t const color_index, struct terakan_bo const * const bo,
                                   struct terakan_color_descriptor const * const color,
                                   struct terakan_color_meta_descriptor const * const meta,
                                   bool const is_uav)
{
   assert(bo == NULL || G_028C70_RAT(color->info) == is_uav);
   terakan_hw_state_draw_set_cb_color_impl(
      state, color_index, bo, color, meta,
      is_uav ? S_028C70_BLEND_BYPASS(1) | S_028C70_SOURCE_FORMAT(V_028C70_EXPORT_4C_32BPC) |
                  S_028C70_RAT(1)
             : S_028C70_SOURCE_FORMAT(V_028C70_EXPORT_4C_16BPC));
}

void
terakan_hw_state_draw_set_cb_color1_dual_source(struct terakan_hw_state_draw * const state,
                                                uint32_t const source_format)
{
   terakan_hw_state_draw_set_cb_color_impl(state, 1, NULL, NULL, NULL,
                                           S_028C70_SOURCE_FORMAT(source_format));
}

static void
terakan_hw_state_draw_set_sq_kcache_buffer(
   struct terakan_hw_state_draw * const state, uint32_t const index, uint32_t const size_lines,
   struct terakan_bo const * const bo, uint32_t const va_lines,
   enum terakan_hw_state_draw_sq_constants_needed_stage const needed_stage,
   enum terakan_hw_state_draw_sq_constants_modified_stage const modified_stage)
{
   assert(!(size_lines != 0 && bo == NULL));

   uint16_t const buffer_bit = (uint16_t)1 << index;

   uint16_t * const buffers_modified = &state->sq_constants_modified.kcache[modified_stage];

   struct terakan_hw_state_sq_kcache_buffer * const buffer_ptr =
      &state->sq_kcache_buffers[needed_stage][index];

   if (!(*buffers_modified & buffer_bit) && buffer_ptr->size_lines == size_lines &&
       (size_lines == 0 || (buffer_ptr->bo == bo && buffer_ptr->va_lines == va_lines))) {
      /* Not modified. */
   }

   buffer_ptr->bo = bo;
   buffer_ptr->va_lines = va_lines;
   buffer_ptr->size_lines = size_lines;

   *buffers_modified |= buffer_bit;

   if (state->sq_constants_needed.kcache[needed_stage] & buffer_bit) {
      /* Emit before the next draw. */
      BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_VS + needed_stage);
   }
}

void
terakan_hw_state_draw_set_sq_kcache_vs(struct terakan_hw_state_draw * const state,
                                       uint32_t const index, uint32_t const size_lines,
                                       struct terakan_bo const * const bo, uint32_t const va_lines)
{
   terakan_hw_state_draw_set_sq_kcache_buffer(
      state, index, size_lines, bo, va_lines, TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS);
   state->sq_constants_modified
      .kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES] |=
      state->sq_constants_modified
         .kcache[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS] &
      ((uint16_t)1 << index);
}

void
terakan_hw_state_draw_set_sq_kcache_tcs(struct terakan_hw_state_draw * const state,
                                        uint32_t const index, uint32_t const size_lines,
                                        struct terakan_bo const * const bo, uint32_t const va_lines)
{
   terakan_hw_state_draw_set_sq_kcache_buffer(
      state, index, size_lines, bo, va_lines, TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TCS);
}

void
terakan_hw_state_draw_set_sq_kcache_tes(struct terakan_hw_state_draw * const state,
                                        uint32_t const index, uint32_t const size_lines,
                                        struct terakan_bo const * const bo, uint32_t const va_lines)
{
   terakan_hw_state_draw_set_sq_kcache_buffer(
      state, index, size_lines, bo, va_lines, TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES);
}

void
terakan_hw_state_draw_set_sq_kcache_gs(struct terakan_hw_state_draw * const state,
                                       uint32_t const index, uint32_t const size_lines,
                                       struct terakan_bo const * const bo, uint32_t const va_lines)
{
   terakan_hw_state_draw_set_sq_kcache_buffer(state, index, size_lines, bo, va_lines,
                                              TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS,
                                              TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_GS);
}

void
terakan_hw_state_draw_set_sq_kcache_fs(struct terakan_hw_state_draw * const state,
                                       uint32_t const index, uint32_t const size_lines,
                                       struct terakan_bo const * const bo, uint32_t const va_lines)
{
   terakan_hw_state_draw_set_sq_kcache_buffer(state, index, size_lines, bo, va_lines,
                                              TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,
                                              TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_FS);
}

terakan_hw_state_draw_set_sq_kcache_function const
   terakan_hw_state_draw_set_sq_kcache_for_stage[MESA_SHADER_FRAGMENT + 1] = {
      [MESA_SHADER_VERTEX] = terakan_hw_state_draw_set_sq_kcache_vs,
      [MESA_SHADER_TESS_CTRL] = terakan_hw_state_draw_set_sq_kcache_tcs,
      [MESA_SHADER_TESS_EVAL] = terakan_hw_state_draw_set_sq_kcache_tes,
      [MESA_SHADER_GEOMETRY] = terakan_hw_state_draw_set_sq_kcache_gs,
      [MESA_SHADER_FRAGMENT] = terakan_hw_state_draw_set_sq_kcache_fs,
};

static void
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
      return;
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
}

void
terakan_hw_state_draw_set_sq_resource_vi(struct terakan_hw_state_draw * const state,
                                         uint32_t const index, struct terakan_bo const * const bo,
                                         uint32_t const descriptor[8])
{
   terakan_hw_state_draw_set_sq_resource(
      state, index, bo, descriptor, state->sq_resources_not_null.vi, state->sq_resource_bos.vi,
      state->sq_resource_descriptors.vi[0], state->sq_constants_needed.resources.vi,
      state->sq_constants_modified.resources.vi, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VI);
}

void
terakan_hw_state_draw_set_sq_resource_vs(struct terakan_hw_state_draw * const state,
                                         uint32_t const index, struct terakan_bo const * const bo,
                                         uint32_t const descriptor[8])
{
   terakan_hw_state_draw_set_sq_resource(
      state, index, bo, descriptor, state->sq_resources_not_null.vs, state->sq_resource_bos.vs,
      state->sq_resource_descriptors.vs[0], state->sq_constants_needed.resources.vs,
      state->sq_constants_modified.resources.vs_in_ls, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VS);
   if (BITSET_TEST(state->sq_constants_modified.resources.vs_in_ls, index)) {
      BITSET_SET(state->sq_constants_modified.resources.vs_in_vses, index);
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
      state->sq_constants_modified.resources.tcs, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_TCS);
}

void
terakan_hw_state_draw_set_sq_resource_tes(struct terakan_hw_state_draw * const state,
                                          uint32_t const index, struct terakan_bo const * const bo,
                                          uint32_t const descriptor[8])
{
   terakan_hw_state_draw_set_sq_resource(
      state, index, bo, descriptor, state->sq_resources_not_null.tes, state->sq_resource_bos.tes,
      state->sq_resource_descriptors.tes[0], state->sq_constants_needed.resources.tes,
      state->sq_constants_modified.resources.tes_in_vses,
      TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_TES);
}

void
terakan_hw_state_draw_set_sq_resource_gs(struct terakan_hw_state_draw * const state,
                                         uint32_t const index, struct terakan_bo const * const bo,
                                         uint32_t const descriptor[8])
{
   terakan_hw_state_draw_set_sq_resource(
      state, index, bo, descriptor, state->sq_resources_not_null.gs, state->sq_resource_bos.gs,
      state->sq_resource_descriptors.gs[0], state->sq_constants_needed.resources.gs,
      state->sq_constants_modified.resources.gs, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_GS);
}

void
terakan_hw_state_draw_set_sq_resource_fs(struct terakan_hw_state_draw * const state,
                                         uint32_t const index, struct terakan_bo const * const bo,
                                         uint32_t const descriptor[8])
{
   terakan_hw_state_draw_set_sq_resource(
      state, index, bo, descriptor, state->sq_resources_not_null.fs, state->sq_resource_bos.fs,
      state->sq_resource_descriptors.fs[0], state->sq_constants_needed.resources.fs,
      state->sq_constants_modified.resources.fs, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_FS);
}

terakan_hw_state_draw_set_sq_resource_function const
   terakan_hw_state_draw_set_sq_resource_for_stage[MESA_SHADER_FRAGMENT + 1] = {
      [MESA_SHADER_VERTEX] = terakan_hw_state_draw_set_sq_resource_vs,
      [MESA_SHADER_TESS_CTRL] = terakan_hw_state_draw_set_sq_resource_tcs,
      [MESA_SHADER_TESS_EVAL] = terakan_hw_state_draw_set_sq_resource_tes,
      [MESA_SHADER_GEOMETRY] = terakan_hw_state_draw_set_sq_resource_gs,
      [MESA_SHADER_FRAGMENT] = terakan_hw_state_draw_set_sq_resource_fs,
};

static void
terakan_hw_state_draw_set_sq_sampler(
   struct terakan_hw_state_draw * const state, uint32_t const index, uint32_t const sampler[3],
   float const border_color[4],
   enum terakan_hw_state_draw_sq_constants_needed_stage const needed_stage,
   enum terakan_hw_state_draw_sq_constants_modified_stage const modified_stage)
{
   assert(G_03C008_TYPE(sampler[2]));

   uint32_t const sampler_bit = (uint32_t)1 << index;

   bool const sampler_needed =
      (state->sq_constants_needed.samplers[needed_stage] & sampler_bit) != 0;

   uint32_t * const samplers_ever_written = &state->sq_samplers_ever_written[needed_stage];
   uint32_t * const samplers_modified = &state->sq_constants_modified.samplers[modified_stage];
   uint32_t * const state_sampler = state->sq_samplers[needed_stage][index];
   if (!(*samplers_ever_written & sampler_bit) ||
       memcmp(state_sampler, sampler, sizeof(uint32_t) * 3) != 0) {
      memcpy(state_sampler, sampler, sizeof(uint32_t) * 3);
      *samplers_ever_written |= sampler_bit;
      *samplers_modified |= sampler_bit;
      if (sampler_needed) {
         /* Emit before the next draw. */
         BITSET_SET(state->state_modified,
                    TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_VS + needed_stage);
      }
   }

   if (G_03C000_BORDER_COLOR_TYPE(sampler[0]) == V_03C000_SQ_TEX_BORDER_COLOR_REGISTER) {
      state->sq_samplers_with_border_color[needed_stage] |= sampler_bit;
      uint32_t * const border_colors_ever_written =
         &state->sq_sampler_border_colors_ever_written[needed_stage];
      uint32_t * const border_colors_modified =
         &state->sq_constants_modified.sampler_border_colors[modified_stage];
      float * const state_border_color = state->sq_sampler_border_colors[needed_stage][index];
      if (!(*border_colors_ever_written & sampler_bit) ||
          memcmp(state_border_color, border_color, sizeof(float) * 4) != 0) {
         memcpy(state_border_color, border_color, sizeof(float) * 4);
         *border_colors_ever_written |= sampler_bit;
         *border_colors_modified |= sampler_bit;
      }
      /* Make sure the border color is emitted not only if it was modified by this call, but also if
       * it was modified previously, however hasn't been yet emitted because the sampler at this
       * index previously didn't need it.
       */
      if (sampler_needed && (*border_colors_modified & sampler_bit)) {
         BITSET_SET(state->state_modified,
                    TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_VS + needed_stage);
      }
   } else {
      state->sq_samplers_with_border_color[needed_stage] &= ~sampler_bit;
   }
}

void
terakan_hw_state_draw_set_sq_sampler_vs(struct terakan_hw_state_draw * const state,
                                        uint32_t const index, uint32_t const sampler[3],
                                        float const border_color[4])
{
   terakan_hw_state_draw_set_sq_sampler(state, index, sampler, border_color,
                                        TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
                                        TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS);
   uint32_t const sampler_bit = (uint32_t)1 << index;
   state->sq_constants_modified
      .samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES] |=
      state->sq_constants_modified
         .samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS] &
      sampler_bit;
   state->sq_constants_modified
      .sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES] |=
      state->sq_constants_modified
         .sampler_border_colors[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS] &
      sampler_bit;
}

void
terakan_hw_state_draw_set_sq_sampler_tcs(struct terakan_hw_state_draw * const state,
                                         uint32_t const index, uint32_t const sampler[3],
                                         float const border_color[4])
{
   terakan_hw_state_draw_set_sq_sampler(state, index, sampler, border_color,
                                        TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS,
                                        TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TCS);
}

void
terakan_hw_state_draw_set_sq_sampler_tes(struct terakan_hw_state_draw * const state,
                                         uint32_t const index, uint32_t const sampler[3],
                                         float const border_color[4])
{
   terakan_hw_state_draw_set_sq_sampler(
      state, index, sampler, border_color, TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES);
}

void
terakan_hw_state_draw_set_sq_sampler_gs(struct terakan_hw_state_draw * const state,
                                        uint32_t const index, uint32_t const sampler[3],
                                        float const border_color[4])
{
   terakan_hw_state_draw_set_sq_sampler(state, index, sampler, border_color,
                                        TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS,
                                        TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_GS);
}

void
terakan_hw_state_draw_set_sq_sampler_fs(struct terakan_hw_state_draw * const state,
                                        uint32_t const index, uint32_t const sampler[3],
                                        float const border_color[4])
{
   terakan_hw_state_draw_set_sq_sampler(state, index, sampler, border_color,
                                        TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,
                                        TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_FS);
}

terakan_hw_state_draw_set_sq_sampler_function const
   terakan_hw_state_draw_set_sq_sampler_for_stage[MESA_SHADER_FRAGMENT + 1] = {
      [MESA_SHADER_VERTEX] = terakan_hw_state_draw_set_sq_sampler_vs,
      [MESA_SHADER_TESS_CTRL] = terakan_hw_state_draw_set_sq_sampler_tcs,
      [MESA_SHADER_TESS_EVAL] = terakan_hw_state_draw_set_sq_sampler_tes,
      [MESA_SHADER_GEOMETRY] = terakan_hw_state_draw_set_sq_sampler_gs,
      [MESA_SHADER_FRAGMENT] = terakan_hw_state_draw_set_sq_sampler_fs,
};

static void
terakan_hw_state_draw_set_sq_constants_needed(
   struct terakan_hw_state_draw * const state, uint16_t const kcache,
   uint32_t const * const resources_opt, uint32_t const samplers, bool const stage_needed,
   enum terakan_hw_state_draw_sq_constants_needed_stage const constants_needed_stage,
   uint32_t const resource_count, BITSET_WORD * const resources_needed_bitset)
{
   enum terakan_hw_state_draw_index const kcache_state_index =
      TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_VS + constants_needed_stage;
   enum terakan_hw_state_draw_index const resources_state_index =
      TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VS + constants_needed_stage;
   enum terakan_hw_state_draw_index const samplers_state_index =
      TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_VS + constants_needed_stage;
   enum terakan_hw_state_draw_index const sampler_border_colors_state_index =
      TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_VS + constants_needed_stage;

   size_t const resource_bitset_size = sizeof(BITSET_WORD) * BITSET_WORDS(resource_count);

   if (stage_needed) {
      if (state->sq_constants_needed.kcache[constants_needed_stage] != kcache) {
         BITSET_SET(state->state_modified, kcache_state_index);
      }

      if (resources_opt != NULL && !BITSET_TEST(state->state_modified, resources_state_index) &&
          memcmp(resources_needed_bitset, resources_opt, resource_bitset_size) != 0) {
         BITSET_SET(state->state_modified, resources_state_index);
      }

      uint32_t const samplers_needed_different =
         state->sq_constants_needed.samplers[constants_needed_stage] ^ samplers;
      if (samplers_needed_different) {
         BITSET_SET(state->state_modified, samplers_state_index);
         if (samplers_needed_different &
             state->sq_samplers_with_border_color[constants_needed_stage]) {
            BITSET_SET(state->state_modified, sampler_border_colors_state_index);
         }
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

   state->sq_constants_needed.samplers[constants_needed_stage] = samplers;
   if (!(samplers & state->sq_samplers_with_border_color[constants_needed_stage])) {
      BITSET_CLEAR(state->state_modified, sampler_border_colors_state_index);
      if (!samplers) {
         BITSET_CLEAR(state->state_modified, samplers_state_index);
      }
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
      BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_GS);
      BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_GS);
      BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_GS);
      BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_GS);
   } else {
      BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_GS);
      BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_GS);
      BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_GS);
      BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_GS);
   }
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_vi(struct terakan_hw_state_draw * const state,
                                                    BITSET_WORD const * const resources_opt)
{
   size_t const resource_bitset_size =
      sizeof(BITSET_WORD) * BITSET_WORDS(TERAKAN_RESOURCE_HW_COUNT_FETCH);
   if (resources_opt != NULL) {
      if (!BITSET_TEST(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VI) &&
          memcmp(state->sq_constants_needed.resources.vi, resources_opt, resource_bitset_size) !=
             0) {
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VI);
      }
      memcpy(state->sq_constants_needed.resources.vi, resources_opt, resource_bitset_size);
   } else {
      BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VI);
      memset(state->sq_constants_needed.resources.vi, 0, resource_bitset_size);
   }
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_vs(struct terakan_hw_state_draw * const state,
                                                    uint16_t const kcache,
                                                    BITSET_WORD const * const resources_opt,
                                                    uint32_t const samplers,
                                                    VkShaderStageFlags const next_stage)
{
   terakan_hw_state_draw_set_sq_constants_needed(state, kcache, resources_opt, samplers, true,
                                                 TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS,
                                                 TERAKAN_RESOURCE_HW_COUNT_VERTEX,
                                                 state->sq_constants_needed.resources.vs);

   bool const tcs_tes_needed = next_stage == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
   if (state->sq_constants_needed.tcs_tes != tcs_tes_needed) {
      state->sq_constants_needed.tcs_tes = tcs_tes_needed;
      if (tcs_tes_needed) {
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_TCS);
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_TES);
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_TCS);
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_TES);
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_TCS);
         BITSET_SET(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_TES);
         BITSET_SET(state->state_modified,
                    TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_TCS);
         BITSET_SET(state->state_modified,
                    TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_TES);
      } else {
         BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_TCS);
         BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_TES);
         BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_TCS);
         BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_TES);
         BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_TCS);
         BITSET_CLEAR(state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_TES);
         BITSET_CLEAR(state->state_modified,
                      TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_TCS);
         BITSET_CLEAR(state->state_modified,
                      TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_TES);
      }
   }

   bool const gs_was_needed = terakan_hw_state_draw_sq_constants_needed_by_gs(state);
   state->sq_constants_needed.gs_after_vs = next_stage == VK_SHADER_STAGE_GEOMETRY_BIT;
   terakan_hw_state_draw_update_sq_constants_gs_needed(state, gs_was_needed);
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_tcs(struct terakan_hw_state_draw * const state,
                                                     uint16_t const kcache,
                                                     BITSET_WORD const * const resources_opt,
                                                     uint32_t const samplers)
{
   terakan_hw_state_draw_set_sq_constants_needed(
      state, kcache, resources_opt, samplers, state->sq_constants_needed.tcs_tes,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS, TERAKAN_RESOURCE_HW_COUNT_VERTEX,
      state->sq_constants_needed.resources.tcs);
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_tes(struct terakan_hw_state_draw * const state,
                                                     uint16_t const kcache,
                                                     BITSET_WORD const * const resources_opt,
                                                     uint32_t const samplers,
                                                     bool const next_stage_is_gs)
{
   terakan_hw_state_draw_set_sq_constants_needed(
      state, kcache, resources_opt, samplers, state->sq_constants_needed.tcs_tes,
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES, TERAKAN_RESOURCE_HW_COUNT_VERTEX,
      state->sq_constants_needed.resources.tes);

   bool const gs_was_needed = terakan_hw_state_draw_sq_constants_needed_by_gs(state);
   state->sq_constants_needed.gs_after_tes = next_stage_is_gs;
   terakan_hw_state_draw_update_sq_constants_gs_needed(state, gs_was_needed);
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_gs(struct terakan_hw_state_draw * const state,
                                                    uint16_t const kcache,
                                                    BITSET_WORD const * const resources_opt,
                                                    uint32_t const samplers)
{
   terakan_hw_state_draw_set_sq_constants_needed(
      state, kcache, resources_opt, samplers,
      terakan_hw_state_draw_sq_constants_needed_by_gs(state),
      TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS, TERAKAN_RESOURCE_HW_COUNT_VERTEX,
      state->sq_constants_needed.resources.gs);
}

void
terakan_hw_state_draw_set_sq_constants_needed_by_fs(struct terakan_hw_state_draw * const state,
                                                    uint16_t const kcache,
                                                    BITSET_WORD const * const resources_opt,
                                                    uint32_t const samplers)
{
   terakan_hw_state_draw_set_sq_constants_needed(state, kcache, resources_opt, samplers, true,
                                                 TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS,
                                                 TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE,
                                                 state->sq_constants_needed.resources.fs);
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
   BITSET_COPY(state->sq_constants_modified.resources.vs_in_ls, state->sq_resources_not_null.vs);
   BITSET_COPY(state->sq_constants_modified.resources.vs_in_vses, state->sq_resources_not_null.vs);
   BITSET_COPY(state->sq_constants_modified.resources.tcs, state->sq_resources_not_null.tcs);
   BITSET_COPY(state->sq_constants_modified.resources.tes_in_vses,
               state->sq_resources_not_null.tes);
   BITSET_COPY(state->sq_constants_modified.resources.gs, state->sq_resources_not_null.gs);
   BITSET_COPY(state->sq_constants_modified.resources.fs, state->sq_resources_not_null.fs);

   /* Samplers are undefined in the hardware. */
   state->sq_constants_modified
      .samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_LS] =
      state->sq_samplers_ever_written[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS];
   state->sq_constants_modified
      .samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_VS_IN_VSES] =
      state->sq_samplers_ever_written[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_VS];
   state->sq_constants_modified.samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TCS] =
      state->sq_samplers_ever_written[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TCS];
   state->sq_constants_modified
      .samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_TES_IN_VSES] =
      state->sq_samplers_ever_written[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_TES];
   state->sq_constants_modified.samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_GS] =
      state->sq_samplers_ever_written[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_GS];
   state->sq_constants_modified.samplers[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_MODIFIED_STAGE_FS] =
      state->sq_samplers_ever_written[TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_FS];

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
   BITSET_FOREACH_SET (state_index, state->state_modified, TERAKAN_HW_STATE_DRAW_INDEX_COUNT) {
      terakan_hw_state_draw_emit_functions[state_index](command_writer);
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

   /* Make sure emission callbacks with additional modification tracking emit everything needed. */
   state->viewport_counts.scale_offset_z_min_max_emitted = 0;
   state->viewport_counts.scissor_emitted = 0;
   state->cb_blend_control.modified = state->cb_blend_control.ever_written;
   state->cb_color.modified = state->cb_color.ever_written;

   unsigned state_index;
   BITSET_FOREACH_SET (state_index, state->state_ever_written, TERAKAN_HW_STATE_DRAW_INDEX_COUNT) {
      terakan_hw_state_draw_emit_functions[state_index](command_writer);
   }
}

void
terakan_hw_state_draw_reset(struct terakan_hw_state_draw * const state)
{
   BITSET_ZERO(state->state_ever_written);
   BITSET_ZERO(state->state_modified);

   memset(&state->viewport_counts, 0, sizeof(state->viewport_counts));
   /* Viewport state fields may be modified at different frequencies, initialize all to safe values.
    */
   memset(&state->viewports, 0, sizeof(state->viewports));
   /* For simplicity, consider the viewport state always valid (starting from 0 viewports needed).
    */
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_INDEX_VIEWPORT);

   state->cb_blend_control.ever_written = 0;
   state->cb_blend_control.modified = 0;
   /* For simplicity, consider the blend control state always valid (starting from no color targets
    * ever having configured blending).
    */
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_INDEX_CB_BLEND_CONTROL);

   state->cb_color.ever_written = 0;
   state->cb_color.modified = 0;
   /* For simplicity, consider the color targets always valid (starting from no color targets ever
    * bound).
    */
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_INDEX_CB_COLOR);

   memset(&state->sq_constants_needed, 0, sizeof(state->sq_constants_needed));

   /* For simplicity, don't track whether constants have actually ever been set, reset them to null.
    */
   memset(&state->sq_kcache_buffers, 0, sizeof(state->sq_kcache_buffers));
   memset(&state->sq_resources_not_null, 0, sizeof(state->sq_resources_not_null));

   memset(&state->sq_samplers_ever_written, 0, sizeof(state->sq_samplers_ever_written));
   memset(&state->sq_sampler_border_colors_ever_written, 0,
          sizeof(state->sq_sampler_border_colors_ever_written));
   memset(&state->sq_samplers_with_border_color, 0, sizeof(state->sq_samplers_with_border_color));

   /* state_modified is set specially for sequencer constants, and state_ever_written is not
    * updated for them, but make sure they are emitted next full state emission - they've also just
    * been initialized to a valid state (to all being null).
    */
   for (unsigned stage = 0; stage < TERAKAN_HW_STATE_DRAW_SQ_CONSTANTS_NEEDED_STAGE_COUNT;
        ++stage) {
      BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_INDEX_SQ_KCACHE_VS + stage);
      BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VS + stage);
      BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLERS_VS + stage);
      BITSET_SET(state->state_ever_written,
                 TERAKAN_HW_STATE_DRAW_INDEX_SQ_SAMPLER_BORDER_COLORS_VS + stage);
   }
   BITSET_SET(state->state_ever_written, TERAKAN_HW_STATE_DRAW_INDEX_SQ_RESOURCES_VI);
}
