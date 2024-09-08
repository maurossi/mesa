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

#include "terakan_meta.h"

#include "terakan_bo.h"
#include "terakan_buffer.h"
#include "terakan_descriptor.h"
#include "terakan_entrypoints.h"
#include "terakan_format.h"
#include "terakan_image.h"

#include "gallium/drivers/r600/eg_sq.h"
#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600_opcodes.h"
#include "util/macros.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* Buffer to image copying by drawing to a color attachment.
 * Not using an image RAT because "Tex2D UAV on cypress will fail/hang if tile mode is linear"
 * according to the R800 AddrLib.
 */

struct terakan_meta_copy_buffer_to_image_push_constants {
   /* All values are in blocks. */
   int32_t image_offset_x;
   int32_t image_offset_y;
   uint32_t buffer_y_pitch;
   uint32_t buffer_z_pitch;
};

static uint32_t const terakan_meta_copy_buffer_to_image_ps_r8xx[] = {
   /* Control flow. */

   /* 0: Address calculation. */

   S_SQ_CF_WORD0_ADDR(3) | S_SQ_CF_ALU_WORD0_KCACHE_BANK0(0) |
      S_SQ_CF_ALU_WORD0_KCACHE_MODE0(V_SQ_CF_KCACHE_LOCK_1),
   S_SQ_CF_ALU_WORD1_KCACHE_ADDR0(0) | S_SQ_CF_ALU_WORD1_COUNT(8 - 3) |
      S_SQ_CF_ALU_WORD1_BARRIER(1) | EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 1: Fetch from the source buffer. */

   S_SQ_CF_WORD0_ADDR(10),
   S_SQ_CF_WORD1_COUNT(0) | S_SQ_CF_WORD1_BARRIER(1) | EG_V_SQ_CF_WORD1_SQ_CF_INST_TEX,

   /* 2: Export the color and end the program. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(V_03000C_SQ_SEL_X) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(V_03000C_SQ_SEL_Y) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(V_03000C_SQ_SEL_Z) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(V_03000C_SQ_SEL_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) | S_SQ_CF_ALLOC_EXPORT_WORD1_END_OF_PROGRAM(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* ALU clause. */

   /* Convert from image XY to buffer XY, and apply the layer pitch to the layer index.
    *
    * 3:     PV.X = SUB_INT R0.X, CB[0][0].X
    * 4:     PV.Y = SUB_INT R0.Y, CB[0][0].Y
    * 5: (T) PS (Z) = MULLO_UINT CB[0][0].W, R0.Z
    * Cycle 0 (GPR only for XYZW): GPR.X = R0, GPR.Y = R0, 1 constant
    * Cycle 1: GPR.Z = R0, 2 constants
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) | S_SQ_ALU_WORD0_SRC1_SEL(0x80) |
      S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_SUB_INT),

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) | S_SQ_ALU_WORD0_SRC1_SEL(0x80) |
      S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_SUB_INT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0x80) | S_SQ_ALU_WORD0_SRC0_CHAN(3) |
      S_SQ_ALU_WORD0_SRC1_SEL(0) | S_SQ_ALU_WORD0_SRC1_CHAN(2),
   S_SQ_ALU_WORD1_DST_CHAN(2) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   /* Apply the layer offset to the address, and apply the row pitch to the row index.
    *
    * 6:     PV.X = ADD_INT PV.X, PS
    * 7: (T) PS (Y) = MULLO_UINT CB[0][0].Z, PV.Y
    * Cycle 0 (GPR only for XYZW): 1 constant
    */

   S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PS),
   S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0x80) | S_SQ_ALU_WORD0_SRC0_CHAN(2) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   /* Apply the row offset to the address written to R0.X.
    *
    * 8: (v) R0.X = ADD_INT PV.X, PS
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PS),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT),

   /* 9 (alignment padding), 10, 11: Vertex-fetch from the source buffer to R0. */

   0,
   0,

   S_SQ_VTX_WORD0_VTX_INST(0) | S_SQ_VTX_WORD0_FETCH_TYPE(SQ_VTX_FETCH_NO_INDEX_OFFSET) |
      S_SQ_VTX_WORD0_BUFFER_ID(TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META) |
      S_SQ_VTX_WORD0_SRC_GPR(0) | S_SQ_VTX_WORD0_SRC_SEL_X(V_03000C_SQ_SEL_X) |
      S_SQ_VTX_WORD0_MEGA_FETCH_COUNT(16),
   S_SQ_VTX_WORD1_GPR_DST_GPR(0) | S_SQ_VTX_WORD1_DST_SEL_X(V_03000C_SQ_SEL_X) |
      S_SQ_VTX_WORD1_DST_SEL_Y(V_03000C_SQ_SEL_Y) | S_SQ_VTX_WORD1_DST_SEL_Z(V_03000C_SQ_SEL_Z) |
      S_SQ_VTX_WORD1_DST_SEL_W(V_03000C_SQ_SEL_W) | S_SQ_VTX_WORD1_USE_CONST_FIELDS(1),
   S_SQ_VTX_WORD2_MEGA_FETCH(1),
   0,
};

static uint32_t const terakan_meta_copy_buffer_to_image_ps_r9xx[] = {
   /* Control flow. */

   /* 0: Address calculation. */

   S_SQ_CF_WORD0_ADDR(4) | S_SQ_CF_ALU_WORD0_KCACHE_BANK0(0) |
      S_SQ_CF_ALU_WORD0_KCACHE_MODE0(V_SQ_CF_KCACHE_LOCK_1),
   S_SQ_CF_ALU_WORD1_KCACHE_ADDR0(0) | S_SQ_CF_ALU_WORD1_COUNT(15 - 4) |
      S_SQ_CF_ALU_WORD1_BARRIER(1) | EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 1: Fetch from the source buffer. */

   S_SQ_CF_WORD0_ADDR(16),
   S_SQ_CF_WORD1_COUNT(0) | S_SQ_CF_WORD1_BARRIER(1) | EG_V_SQ_CF_WORD1_SQ_CF_INST_TEX,

   /* 2: Export the color and end the program. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(V_03000C_SQ_SEL_X) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(V_03000C_SQ_SEL_Y) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(V_03000C_SQ_SEL_Z) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(V_03000C_SQ_SEL_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) | EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* 3: End the program. */

   0,
   CM_V_SQ_CF_WORD1_SQ_CF_INST_END,

   /* ALU clause. */

   /* Convert from image XY to buffer XY, and apply the layer pitch to the layer index.
    *
    * 4: R0.X = SUB_INT R0.X, CB[0][0].X
    * 5: PV.Y = SUB_INT R0.Y, CB[0][0].Y
    * Cycle 0: GPR.X = R0, GPR.Y = R0
    * Cycle 1: 2 constants
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) | S_SQ_ALU_WORD0_SRC1_SEL(0x80) |
      S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_SUB_INT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      S_SQ_ALU_WORD0_SRC1_SEL(0x80) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_SUB_INT),

   /* Apply the row pitch to the row index.
    *
    * MULLO_UINT uses 4 slots.
    * 6: PV.X = MULLO_UINT CB[0][0].Z, PV.Y
    * 7: R0.Y = MULLO_UINT CB[0][0].Z, PV.Y
    * 8: PV.Z = MULLO_UINT CB[0][0].Z, PV.Y
    * 9: PV.W = MULLO_UINT CB[0][0].Z, PV.Y
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0x80) | S_SQ_ALU_WORD0_SRC0_CHAN(2) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_SRC0_SEL(0x80) | S_SQ_ALU_WORD0_SRC0_CHAN(2) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(1) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_SRC0_SEL(0x80) | S_SQ_ALU_WORD0_SRC0_CHAN(2) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_CHAN(2) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0x80) | S_SQ_ALU_WORD0_SRC0_CHAN(2) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_CHAN(3) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   /* Apply the layer pitch to the layer index.
    *
    * MULLO_UINT uses 4 slots.
    * 10: PV.X = MULLO_UINT CB[0][0].W, R0.Z
    * 11: PV.Y = MULLO_UINT CB[0][0].W, R0.Z
    * 12: PV.Z = MULLO_UINT CB[0][0].W, R0.Z
    * 13: PV.W = MULLO_UINT CB[0][0].W, R0.Z
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0x80) | S_SQ_ALU_WORD0_SRC0_CHAN(3) | S_SQ_ALU_WORD0_SRC1_SEL(0) |
      S_SQ_ALU_WORD0_SRC1_CHAN(2),
   S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_SRC0_SEL(0x80) | S_SQ_ALU_WORD0_SRC0_CHAN(3) | S_SQ_ALU_WORD0_SRC1_SEL(0) |
      S_SQ_ALU_WORD0_SRC1_CHAN(2),
   S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_SRC0_SEL(0x80) | S_SQ_ALU_WORD0_SRC0_CHAN(3) | S_SQ_ALU_WORD0_SRC1_SEL(0) |
      S_SQ_ALU_WORD0_SRC1_CHAN(2),
   S_SQ_ALU_WORD1_DST_CHAN(2) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0x80) | S_SQ_ALU_WORD0_SRC0_CHAN(3) |
      S_SQ_ALU_WORD0_SRC1_SEL(0) | S_SQ_ALU_WORD0_SRC1_CHAN(2),
   S_SQ_ALU_WORD1_DST_CHAN(3) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   /* Apply the layer offset to the address.
    *
    * 14: PV.X = ADD_INT R0.X, PV.Z
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(2),
   S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT),

   /* Apply the row offset to the address written to R0.X.
    *
    * 15: R0.X = ADD_INT PV.X, R0.Y
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(0) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT),

   /* 16, 17: Vertex-fetch from the source buffer to R0. */

   S_SQ_VTX_WORD0_VTX_INST(0) | S_SQ_VTX_WORD0_FETCH_TYPE(SQ_VTX_FETCH_NO_INDEX_OFFSET) |
      S_SQ_VTX_WORD0_BUFFER_ID(TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META) |
      S_SQ_VTX_WORD0_SRC_GPR(0) | S_SQ_VTX_WORD0_SRC_SEL_X(V_03000C_SQ_SEL_X),
   S_SQ_VTX_WORD1_GPR_DST_GPR(0) | S_SQ_VTX_WORD1_DST_SEL_X(V_03000C_SQ_SEL_X) |
      S_SQ_VTX_WORD1_DST_SEL_Y(V_03000C_SQ_SEL_Y) | S_SQ_VTX_WORD1_DST_SEL_Z(V_03000C_SQ_SEL_Z) |
      S_SQ_VTX_WORD1_DST_SEL_W(V_03000C_SQ_SEL_W) | S_SQ_VTX_WORD1_USE_CONST_FIELDS(1),
   0,
   0,
};

struct terakan_meta_shader const terakan_meta_copy_buffer_to_image_ps =
   {
      .r8xx =
         {
            .program = terakan_meta_copy_buffer_to_image_ps_r8xx,
            .program_size_bytes = sizeof(terakan_meta_copy_buffer_to_image_ps_r8xx),
            .static_registers =
               {
                  .sq_pgm_resources =
                     {
                        S_028844_NUM_GPRS(1) | TERAKAN_META_SQ_PGM_RESOURCES_COMMON,
                        TERAKAN_META_SQ_PGM_RESOURCES_2_COMMON,
                     },
                  .stage =
                     {
                        .ps =
                           {
                              .sq_pgm_exports_ps = S_02884C_EXPORT_COLORS(1),
                              .spi_ps_in_control =
                                 {
                                    S_0286CC_NUM_INTERP(1) | S_0286CC_LINEAR_GRADIENT_ENA(1),
                                    S_0286D0_FIXED_PT_POSITION_ENA(1) |
                                       S_0286D0_FIXED_PT_POSITION_ADDR(0),
                                 },
                              .spi_baryc_cntl = S_0286E0_LINEAR_CENTER_ENA(1),
                              .cb_shader_mask = 0xF,
                           },
                     },
               },
         },
      .r9xx =
         {
            .program = terakan_meta_copy_buffer_to_image_ps_r9xx,
            .program_size_bytes = sizeof(terakan_meta_copy_buffer_to_image_ps_r9xx),
            .static_registers =
               {
                  .sq_pgm_resources =
                     {
                        S_028844_NUM_GPRS(1) | TERAKAN_META_SQ_PGM_RESOURCES_COMMON,
                        TERAKAN_META_SQ_PGM_RESOURCES_2_COMMON,
                     },
                  .stage =
                     {
                        .ps =
                           {
                              .sq_pgm_exports_ps = S_02884C_EXPORT_COLORS(1),
                              .spi_ps_in_control =
                                 {
                                    S_0286CC_NUM_INTERP(1) | S_0286CC_LINEAR_GRADIENT_ENA(1),
                                    S_0286D0_FIXED_PT_POSITION_ENA(1) |
                                       S_0286D0_FIXED_PT_POSITION_ADDR(0),
                                 },
                              .spi_baryc_cntl = S_0286E0_LINEAR_CENTER_ENA(1),
                              .cb_shader_mask = 0xF,
                           },
                     },
               },
         },
      .constant_cache_needed = 0b1,
      .resources_needed =
         {
            [BITSET_BITWORD(TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META)] =
               BITSET_BIT(TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META),
         },
};

VKAPI_ATTR void VKAPI_CALL
terakan_CmdCopyBufferToImage2(VkCommandBuffer const commandBuffer,
                              VkCopyBufferToImageInfo2 const * const pCopyBufferToImageInfo)
{
   struct terakan_image const * const image =
      terakan_image_from_handle(pCopyBufferToImageInfo->dstImage);

   /* TODO(Triang3l): Path for linear 8_8_8, 16_16_16 and 32_32_32 as buffer to buffer. */

   VkFormat const transfer_format = terakan_meta_transfer_image_block_format(image->surface.bpe);

   VkImageViewCreateInfo image_view_create_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = pCopyBufferToImageInfo->dstImage,
      .viewType = image->vk.image_type == VK_IMAGE_TYPE_1D ? VK_IMAGE_VIEW_TYPE_1D_ARRAY
                                                           : VK_IMAGE_VIEW_TYPE_2D_ARRAY,
      .subresourceRange =
         {
            .levelCount = 1,
         },
   };

   struct terakan_buffer const * const buffer =
      terakan_buffer_from_handle(pCopyBufferToImageInfo->srcBuffer);

   uint32_t buffer_resource[8] = {
      [3] = S_03000C_DST_SEL_X(V_03000C_SQ_SEL_X) | S_03000C_DST_SEL_Y(V_03000C_SQ_SEL_Y) |
            S_03000C_DST_SEL_Z(V_03000C_SQ_SEL_Z) | S_03000C_DST_SEL_W(V_03000C_SQ_SEL_W),
      [7] = S_03001C_TYPE(V_03001C_SQ_TEX_VTX_VALID_BUFFER),
      [TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD] = TERAKAN_BO_PRIORITY_SHADER_READ_BUFFER,
   };

   struct terakan_gfx_command_writer * const command_writer =
      terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx;

   terakan_meta_begin_2d_immediate_rectangles(command_writer,
                                              TERAKAN_META_DB_RENDER_OVERRIDE_DEFAULT);

   terakan_meta_set_vs(command_writer, TERAKAN_META_SHADER_POSITION_FROM_INDEX_VS);
   terakan_meta_set_ps(command_writer, TERAKAN_META_SHADER_COPY_BUFFER_TO_IMAGE_PS);

   terakan_state_draw_set_pending(&command_writer->state_draw, TERAKAN_STATE_DRAW_CB_COLOR_FIRST);

   struct terakan_meta_copy_buffer_to_image_push_constants push_constants = {};
   struct terakan_bo const * push_constants_bo = NULL;
   uint32_t push_constants_base;

   for (uint32_t region_index = 0; region_index < pCopyBufferToImageInfo->regionCount;
        ++region_index) {
      VkBufferImageCopy2 const * const region = &pCopyBufferToImageInfo->pRegions[region_index];

      VkRect2D rectangle;
      /* The offset must be block-aligned, but offset + extent is limited to the extent of the
       * subresource, which is not block-aligned.
       */
      rectangle.offset.x = region->imageOffset.x / image->surface.blk_w;
      rectangle.offset.y = region->imageOffset.y / image->surface.blk_h;
      rectangle.extent.width =
         region->imageExtent.width / image->surface.blk_w +
         (uint32_t)((region->imageExtent.width & (image->surface.blk_w - 1)) != 0);
      rectangle.extent.height =
         region->imageExtent.height / image->surface.blk_h +
         (uint32_t)((region->imageExtent.height & (image->surface.blk_h - 1)) != 0);

      /* Buffer row length and image height are block-aligned. */
      uint32_t const buffer_y_pitch = region->bufferRowLength / image->surface.blk_w;
      uint32_t const buffer_z_pitch =
         buffer_y_pitch * (region->bufferImageHeight / image->surface.blk_h);
      if (push_constants.image_offset_x != rectangle.offset.x ||
          push_constants.image_offset_y != rectangle.offset.y ||
          push_constants.buffer_y_pitch != buffer_y_pitch ||
          push_constants.buffer_z_pitch != buffer_z_pitch) {
         push_constants.image_offset_x = rectangle.offset.x;
         push_constants.image_offset_y = rectangle.offset.y;
         push_constants.buffer_y_pitch = buffer_y_pitch;
         push_constants.buffer_z_pitch = buffer_z_pitch;
         push_constants_bo = NULL;
      }
      if (push_constants_bo == NULL) {
         void * const push_constants_mapping = terakan_command_buffer_allocate_push_constants(
            command_writer->base.command_buffer, sizeof(push_constants), &push_constants_bo,
            &push_constants_base);
         if (unlikely(push_constants_mapping == NULL)) {
            return;
         }
         memcpy(push_constants_mapping, &push_constants, sizeof(push_constants));
         terakan_hw_state_draw_set_sq_constant_cache_fs(
            &command_writer->hw_state_draw, 0,
            (sizeof(push_constants) + (TERAKAN_CONSTANT_CACHE_LINE_BYTES - 1)) /
               TERAKAN_CONSTANT_CACHE_LINE_BYTES,
            push_constants_bo, push_constants_base);
      }

      bool const is_stencil = region->imageSubresource.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT;

      VkFormat const region_transfer_format = is_stencil ? VK_FORMAT_R8_UINT : transfer_format;
      image_view_create_info.format = region_transfer_format;

      image_view_create_info.subresourceRange.aspectMask = region->imageSubresource.aspectMask;

      if (image->vk.image_type == VK_IMAGE_TYPE_3D) {
         image_view_create_info.subresourceRange.baseArrayLayer = (uint32_t)region->imageOffset.z;
         image_view_create_info.subresourceRange.layerCount = region->imageExtent.depth;
      } else {
         image_view_create_info.subresourceRange.baseArrayLayer =
            region->imageSubresource.baseArrayLayer;
         image_view_create_info.subresourceRange.layerCount =
            region->imageSubresource.layerCount == VK_REMAINING_ARRAY_LAYERS
               ? image->vk.array_layers - region->imageSubresource.baseArrayLayer
               : region->imageSubresource.layerCount;
      }

      VkDeviceSize buffer_offset = buffer->bo_offset + region->bufferOffset;

      uint32_t const bpe = is_stencil ? 1 : image->surface.bpe;

      while (image_view_create_info.subresourceRange.layerCount > 0) {
         struct terakan_color_descriptor color_descriptor;
         struct terakan_color_meta_descriptor color_meta_descriptor;
         uint32_t const color_descriptor_layer_count = terakan_image_create_color_descriptor(
            &image_view_create_info, &color_descriptor, &color_meta_descriptor);
         if (unlikely(color_descriptor_layer_count == 0)) {
            assert(!"Invalid image view create info");
            return;
         }
         terakan_color_descriptor_image_view_to_color_attachment(&color_descriptor);

         bool const cb_color_modified =
            command_writer->hw_state_draw.cb_color_bo[0] != image->bo ||
            memcmp(&command_writer->hw_state_draw.cb_color[0], &color_descriptor,
                   sizeof(color_descriptor)) != 0 ||
            memcmp(&command_writer->hw_state_draw.cb_color_meta[0], &color_meta_descriptor,
                   sizeof(color_meta_descriptor)) != 0;
         if (cb_color_modified) {
            command_writer->hw_state_draw.cb_color_bo[0] = image->bo;
            memcpy(&command_writer->hw_state_draw.cb_color[0], &color_descriptor,
                   sizeof(color_descriptor));
            memcpy(&command_writer->hw_state_draw.cb_color_meta[0], &color_meta_descriptor,
                   sizeof(color_meta_descriptor));
         }
         terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                       TERAKAN_HW_STATE_DRAW_CB_COLOR_FIRST, cb_color_modified);

         VkDeviceSize const buffer_size_elements =
            (image_view_create_info.subresourceRange.layerCount - 1) * buffer_z_pitch +
            (rectangle.extent.height - 1) * buffer_y_pitch + rectangle.extent.width;
         buffer_resource[0] = (uint32_t)buffer_offset;
         buffer_resource[1] = (uint32_t)(bpe * buffer_size_elements - 1);
         buffer_resource[2] =
            S_030008_BASE_ADDRESS_HI(buffer_offset >> 32) | S_030008_STRIDE(bpe) |
            S_030008_DATA_FORMAT(terakan_format_vertex_get_format(region_transfer_format)) |
            S_030008_NUM_FORMAT_ALL(terakan_format_data_get_number_format(region_transfer_format));
         buffer_resource[4] = (uint32_t)buffer_size_elements;
         terakan_hw_state_draw_set_sq_resource_fs(
            &command_writer->hw_state_draw, TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META,
            buffer->bo, buffer_resource);

         terakan_meta_emit_rectangle_3_vertices_draw(
            command_writer, &rectangle, image_view_create_info.subresourceRange.layerCount);

         image_view_create_info.subresourceRange.baseArrayLayer += color_descriptor_layer_count;
         image_view_create_info.subresourceRange.layerCount -= color_descriptor_layer_count;
         buffer_offset += bpe * (VkDeviceSize)buffer_z_pitch * color_descriptor_layer_count;
      }
   }
}
