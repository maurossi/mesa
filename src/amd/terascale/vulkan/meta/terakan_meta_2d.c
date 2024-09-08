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

#include "terakan_meta.h"

#include "terakan_command_buffer.h"
#include "terakan_device.h"
#include "terakan_draw.h"

#include "amd/terascale/common/terascale_wddm.h"
#include "gallium/drivers/r600/eg_sq.h"
#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600_opcodes.h"
#include "gallium/drivers/r600/r600d_common.h"
#include "util/macros.h"
#include "util/u_math.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static uint32_t const terakan_meta_position_from_index_vs_r8xx[] = {
   /* Control flow. */

   /* 0: Export the instance ID as the first parameter. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PARAM) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_W) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* 1: Vertex position calculation. */

   S_SQ_CF_WORD0_ADDR(3),
   S_SQ_CF_ALU_WORD1_COUNT(8 - 3) | S_SQ_CF_ALU_WORD1_BARRIER(1) |
      EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 2: Export the position in R0.XY01 and end the program. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_POS) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(60) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_X) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_0) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_1) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) | S_SQ_CF_ALLOC_EXPORT_WORD1_END_OF_PROGRAM(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* ALU clause. */

   /* Extract bits 15:0 of the vertex index (to be used as position) to PV.X and bits 31:16 to PV.Y.
    *
    * 3:     PV.X = BFE_INT R0.X, 0, 16
    * 4: (V) PV.Y = BFE_INT R0.X, 16, 16
    * 5: Literal X = 16, Y = unused
    * Cycle 0: X = R0
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_OP3_SRC2_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD1_OP3_SRC2_CHAN(0) |
      S_SQ_ALU_WORD1_DST_GPR(0x7F - 4) | S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_BFE_INT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_OP3_SRC2_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD1_OP3_SRC2_CHAN(0) |
      S_SQ_ALU_WORD1_DST_GPR(0x7F - 4) | S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_BFE_INT),

   16,
   0,

   /* Convert the vertex position X from int to float.
    *
    * 6:     PV.Y = PV.Y, unused PV.Y
    * 7: (T) R0.X = INT_TO_FLT PV.X, unused PV.X
    */

   S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_CHAN(1) | S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_INT_TO_FLT),

   /* Convert the vertex position Y from int to float.
    *
    * 8: (T) R0.Y = INT_TO_FLT PV.Y, unused PV.Y
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(1) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_INT_TO_FLT),
};

static uint32_t const terakan_meta_position_and_layer_from_index_vs_r8xx[] = {
   /* Control flow. */

   /* 0: Export the instance ID as the first parameter. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PARAM) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_W) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* 1: Export the instance ID as the render target array layer index. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_POS) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(61) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_MASK) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT,

   /* 2: Vertex position calculation. */

   S_SQ_CF_WORD0_ADDR(4),
   S_SQ_CF_ALU_WORD1_COUNT(9 - 4) | S_SQ_CF_ALU_WORD1_BARRIER(1) |
      EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 3: Export the position in R0.XY01 and end the program. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_POS) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(60) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_X) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_0) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_1) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) | S_SQ_CF_ALLOC_EXPORT_WORD1_END_OF_PROGRAM(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* ALU clause. */

   /* Extract bits 15:0 of the vertex index (to be used as position) to PV.X and bits 31:16 to PV.Y.
    *
    * 4:     PV.X = BFE_INT R0.X, 0, 16
    * 5: (V) PV.Y = BFE_INT R0.X, 16, 16
    * 6: Literal X = 16, Y = unused
    * Cycle 0: X = R0
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_OP3_SRC2_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD1_OP3_SRC2_CHAN(0) |
      S_SQ_ALU_WORD1_DST_GPR(0x7F - 4) | S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_BFE_INT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_OP3_SRC2_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD1_OP3_SRC2_CHAN(0) |
      S_SQ_ALU_WORD1_DST_GPR(0x7F - 4) | S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_BFE_INT),

   16,
   0,

   /* Convert the vertex position X from int to float.
    *
    * 7:     PV.Y = PV.Y, unused PV.Y
    * 8: (T) R0.X = INT_TO_FLT PV.X, unused PV.X
    */

   S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_CHAN(1) | S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_INT_TO_FLT),

   /* Convert the vertex position Y from int to float.
    *
    * 9: (T) R0.Y = INT_TO_FLT PV.Y, unused PV.Y
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(1) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_INT_TO_FLT),
};

static uint32_t const terakan_meta_position_from_index_vs_r9xx[] = {
   /* Control flow. */

   /* 0: Export the instance ID as the first parameter. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PARAM) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_W) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* 1: Vertex position calculation. */

   S_SQ_CF_WORD0_ADDR(4),
   S_SQ_CF_ALU_WORD1_COUNT(8 - 4) | S_SQ_CF_ALU_WORD1_BARRIER(1) |
      EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 2: Export the position in R0.XY01. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_POS) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(60) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_X) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_0) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_1) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) | EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* 3: End the program. */

   0,
   S_SQ_CF_WORD1_BARRIER(1) | CM_V_SQ_CF_WORD1_SQ_CF_INST_END,

   /* ALU clause. */

   /* Extract bits 15:0 of the vertex index (to be used as position) to PV.X and bits 31:16 to PV.Y.
    *
    * 4: PV.X = BFE_INT R0.X, 0, 16
    * 5: PV.Y = BFE_INT R0.X, 16, 16
    * 6: Literal X = 16, Y = unused
    * Cycle 0: X = R0
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_OP3_SRC2_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD1_OP3_SRC2_CHAN(0) |
      S_SQ_ALU_WORD1_DST_GPR(0x7F - 4) | S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_BFE_INT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_OP3_SRC2_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD1_OP3_SRC2_CHAN(0) |
      S_SQ_ALU_WORD1_DST_GPR(0x7F - 4) | S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_BFE_INT),

   16,
   0,

   /* Convert the vertex position from int to float.
    *
    * 7: R0.X = INT_TO_FLT PV.X, unused PV.X
    * 8: R0.Y = INT_TO_FLT PV.Y, unused PV.Y
    */

   S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_INT_TO_FLT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(1) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_INT_TO_FLT),
};

static uint32_t const terakan_meta_position_and_layer_from_index_vs_r9xx[] = {
   /* Control flow. */

   /* 0: Export the instance ID as the first parameter. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PARAM) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_W) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* 1: Export the instance ID as the render target array layer index. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_POS) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(61) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_MASK) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT,

   /* 2: Vertex position calculation. */

   S_SQ_CF_WORD0_ADDR(5),
   S_SQ_CF_ALU_WORD1_COUNT(9 - 5) | S_SQ_CF_ALU_WORD1_BARRIER(1) |
      EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 3: Export the position in R0.XY01. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_POS) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(60) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_X) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_0) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_1) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) | EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* 4: End the program. */

   0,
   S_SQ_CF_WORD1_BARRIER(1) | CM_V_SQ_CF_WORD1_SQ_CF_INST_END,

   /* ALU clause. */

   /* Extract bits 15:0 of the vertex index (to be used as position) to PV.X and bits 31:16 to PV.Y.
    *
    * 5: PV.X = BFE_INT R0.X, 0, 16
    * 6: PV.Y = BFE_INT R0.X, 16, 16
    * 7: Literal X = 16, Y = unused
    * Cycle 0: X = R0
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_OP3_SRC2_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD1_OP3_SRC2_CHAN(0) |
      S_SQ_ALU_WORD1_DST_GPR(0x7F - 4) | S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_BFE_INT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_OP3_SRC2_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD1_OP3_SRC2_CHAN(0) |
      S_SQ_ALU_WORD1_DST_GPR(0x7F - 4) | S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_BFE_INT),

   16,
   0,

   /* Convert the vertex position from int to float.
    *
    * 8: R0.X = INT_TO_FLT PV.X, unused PV.X
    * 9: R0.Y = INT_TO_FLT PV.Y, unused PV.Y
    */

   S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_INT_TO_FLT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(1),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(1) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_INT_TO_FLT),
};

struct terakan_meta_shader const terakan_meta_position_from_index_vs = {
   .r8xx =
      {
         .program = terakan_meta_position_from_index_vs_r8xx,
         .program_size_bytes = sizeof(terakan_meta_position_from_index_vs_r8xx),
         .static_registers =
            {
               .sq_pgm_resources =
                  {
                     S_028860_NUM_GPRS(1) | TERAKAN_META_SQ_PGM_RESOURCES_COMMON,
                     TERAKAN_META_SQ_PGM_RESOURCES_2_COMMON,
                  },
            },
      },
   .r9xx =
      {
         .program = terakan_meta_position_from_index_vs_r9xx,
         .program_size_bytes = sizeof(terakan_meta_position_from_index_vs_r9xx),
         .static_registers =
            {
               .sq_pgm_resources =
                  {
                     S_028860_NUM_GPRS(1) | TERAKAN_META_SQ_PGM_RESOURCES_COMMON,
                     TERAKAN_META_SQ_PGM_RESOURCES_2_COMMON,
                  },
            },
      },
};

struct terakan_meta_shader const terakan_meta_position_and_layer_from_index_vs = {
   .r8xx =
      {
         .program = terakan_meta_position_and_layer_from_index_vs_r8xx,
         .program_size_bytes = sizeof(terakan_meta_position_and_layer_from_index_vs_r8xx),
         .static_registers =
            {
               .sq_pgm_resources =
                  {
                     S_028860_NUM_GPRS(1) | TERAKAN_META_SQ_PGM_RESOURCES_COMMON,
                     TERAKAN_META_SQ_PGM_RESOURCES_2_COMMON,
                  },
               .stage =
                  {
                     .vs =
                        {
                           .pa_cl_vs_out_cntl = S_02881C_USE_VTX_RENDER_TARGET_INDX(1) |
                                                S_02881C_VS_OUT_MISC_VEC_ENA(1),
                        },
                  },
            },
      },
   .r9xx =
      {
         .program = terakan_meta_position_and_layer_from_index_vs_r9xx,
         .program_size_bytes = sizeof(terakan_meta_position_and_layer_from_index_vs_r9xx),
         .static_registers =
            {
               .sq_pgm_resources =
                  {
                     S_028860_NUM_GPRS(1) | TERAKAN_META_SQ_PGM_RESOURCES_COMMON,
                     TERAKAN_META_SQ_PGM_RESOURCES_2_COMMON,
                  },
               .stage =
                  {
                     .vs =
                        {
                           .pa_cl_vs_out_cntl = S_02881C_USE_VTX_RENDER_TARGET_INDX(1) |
                                                S_02881C_VS_OUT_MISC_VEC_ENA(1),
                        },
                  },
            },
      },
};

void
terakan_meta_begin_2d(struct terakan_gfx_command_writer * const command_writer,
                      uint32_t const pa_cl_vte_cntl)
{
   terakan_meta_modify_state_draw_dword(command_writer, TERAKAN_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL,
                                        TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL,
                                        &command_writer->hw_state_draw.pa_cl_clip_cntl,
                                        S_028810_CLIP_DISABLE(1));

   terakan_meta_modify_state_draw_dword(command_writer, TERAKAN_STATE_DRAW_INDEX_PA_SU_SC_MODE_CNTL,
                                        TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_SC_MODE_CNTL,
                                        &command_writer->hw_state_draw.pa_su_sc_mode_cntl, 0);

   terakan_meta_set_pa_cl_vte_cntl(command_writer, pa_cl_vte_cntl);

   uint32_t const num_samples_log2 = 0;
   /* TODO(Triang3l): Make what depends on the sample count pending in state_draw. */

   terakan_meta_modify_state_draw_dword(command_writer, TERAKAN_STATE_DRAW_INDEX_PA_SC_MODE_CNTL_0,
                                        TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_MODE_CNTL_0,
                                        &command_writer->hw_state_draw.pa_sc_mode_cntl_0,
                                        S_028A48_MSAA_ENABLE(num_samples_log2 > 0));

   bool const pa_sc_aa_samples_modified =
      command_writer->hw_state_draw.pa_sc_aa_samples.num_samples_log2 != num_samples_log2;
   command_writer->hw_state_draw.pa_sc_aa_samples.num_samples_log2 = num_samples_log2;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_AA_SAMPLES,
                                 pa_sc_aa_samples_modified);

   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_INDEX_PA_SC_AA_MASK);
   uint16_t const pa_sc_aa_mask = ~(uint16_t)0;
   bool const pa_sc_aa_mask_modified = command_writer->hw_state_draw.pa_sc_aa_mask != pa_sc_aa_mask;
   command_writer->hw_state_draw.pa_sc_aa_mask = pa_sc_aa_mask;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_AA_MASK, pa_sc_aa_mask_modified);
}

void
terakan_meta_begin_rects(struct terakan_gfx_command_writer * const command_writer)
{
   terakan_meta_modify_state_draw_dword(command_writer, TERAKAN_STATE_DRAW_INDEX_VGT_PRIMITIVE_TYPE,
                                        TERAKAN_HW_STATE_DRAW_INDEX_VGT_PRIMITIVE_TYPE,
                                        &command_writer->hw_state_draw.vgt_primitive_type,
                                        S_008958_PRIM_TYPE(V_008958_DI_PT_RECTLIST));
}

void
terakan_meta_begin_index_immediate_32(struct terakan_gfx_command_writer * const command_writer)
{
   terakan_meta_modify_state_draw_dword(command_writer, TERAKAN_STATE_DRAW_INDEX_VGT_INDEX_TYPE,
                                        TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_TYPE,
                                        &command_writer->hw_state_draw.vgt_index_type,
                                        VGT_INDEX_32);

   terakan_meta_modify_state_draw_dword(command_writer, TERAKAN_STATE_DRAW_INDEX_VGT_INDEX_OFFSET,
                                        TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_OFFSET,
                                        &command_writer->hw_state_draw.vgt_index_offset, 0);
}

void
terakan_meta_emit_rect_3_vertices_draw(struct terakan_gfx_command_writer * const command_writer,
                                       VkRect2D const * const rect, uint32_t const instance_count)
{
   terakan_before_hw_draw(command_writer);

   uint32_t * packet;

   uint32_t const vertices[] = {
      (uint16_t)rect->offset.x | ((uint32_t)rect->offset.y << 16),
      (uint16_t)rect->offset.x | ((uint32_t)(rect->offset.y + rect->extent.height) << 16),
      (uint16_t)(rect->offset.x + rect->extent.width) | ((uint32_t)rect->offset.y << 16)};

   if (instance_count > 1) {
      /* PKT3_DRAW_INDEX_IMMD with multiple instances causes a hang in this usage scenario (tested
       * on Barts with the firmware used by DRM Radeon 2.50.0).
       */
      struct terakan_bo const * index_buffer_bo;
      uint64_t index_buffer_va;
      uint32_t * const index_buffer_mapping =
         terakan_push_buffer_allocate(command_writer->base.command_buffer, sizeof(vertices),
                                      sizeof(uint32_t), &index_buffer_bo, &index_buffer_va);
      if (unlikely(index_buffer_mapping == NULL)) {
         return;
      }
      /* terakan_meta_begin_index_immediate_32 sets VGT_INDEX_TYPE to non-endian-swapped. */
      util_memcpy_cpu_to_le32(index_buffer_mapping, vertices, sizeof(vertices));

      packet = terakan_gfx_command_writer_emit_with_bo(command_writer, 2 + 5, false, 1, 0, 1);
      if (unlikely(packet == NULL)) {
         return;
      }

      /* NUM_INSTANCES in the same indirect buffer as the draw. */
      *packet++ = PKT3(PKT3_NUM_INSTANCES, 0, 0);
      *packet++ = instance_count;

      *packet++ = PKT3(PKT3_DRAW_INDEX, 5 - 2, 0);
      uint32_t const * const packet_index_base = packet;
      *packet++ = (uint32_t)index_buffer_va;
      *packet++ = (index_buffer_va >> 32) & 0xFF;
      *packet++ = ARRAY_SIZE(vertices);
      *packet++ = S_0287F0_SOURCE_SELECT(V_0287F0_DI_SRC_SEL_DMA);
      /* The exact WDDM patch location for PKT3_DRAW_INDEX isn't known, so reusing the one for
       * PKT3_INDEX_BASE.
       */
      terakan_gfx_command_writer_add_relocation_for_40_bits(
         command_writer, &packet, packet_index_base, packet_index_base + 1,
         TERASCALE_WDDM_PATCH_IDS_INDEX_BASE_LO, TERASCALE_WDDM_PATCH_IDS_INDEX_BASE_HI,
         terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer,
                                                   index_buffer_bo, true, false,
                                                   TERAKAN_BO_PRIORITY_INDEX_BUFFER));

      terakan_gfx_command_writer_emit_done(command_writer, packet);

      return;
   }

   packet = terakan_gfx_command_writer_emit(command_writer, 2 + 3 + ARRAY_SIZE(vertices), false);
   if (unlikely(packet == NULL)) {
      return;
   }

   /* NUM_INSTANCES in the same indirect buffer as the draw. */
   *packet++ = PKT3(PKT3_NUM_INSTANCES, 0, 0);
   *packet++ = instance_count;

   *packet++ = PKT3(PKT3_DRAW_INDEX_IMMD, 1 + ARRAY_SIZE(vertices), 0);
   *packet++ = ARRAY_SIZE(vertices);
   *packet++ = S_0287F0_SOURCE_SELECT(V_0287F0_DI_SRC_SEL_IMMEDIATE);
   memcpy(packet, vertices, sizeof(vertices));
   packet += ARRAY_SIZE(vertices);

   terakan_gfx_command_writer_emit_done(command_writer, packet);
}
