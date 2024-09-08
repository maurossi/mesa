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

#include "terakan_buffer.h"
#include "terakan_command_buffer.h"
#include "terakan_descriptor.h"
#include "terakan_image.h"
#include "terakan_physical_device.h"

#include "gallium/drivers/r600/eg_sq.h"
#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600_opcodes.h"
#include "util/macros.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/* One layer per draw for simplicity (eliminates lots of per-pixel instructions). */

enum {
   /* All values are in surface elements (components) and unsigned. */
   TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH,
   TERAKAN_META_COPY_EXPAND_3X_CONST_DST_PITCH,
   /* Sub-pipe-interleave offset plus an arbitrary offset of the top-left screen pixel in the
    * destination UAV.
    */
   TERAKAN_META_COPY_EXPAND_3X_CONST_DST_OFFSET,

   TERAKAN_META_COPY_EXPAND_3X_CONSTS_COUNT,
};

/* UAV instructions don't support swizzling, so the destination address and the surfel must be in X.
 */

static uint32_t const terakan_meta_copy_expand_3x_ps_r8xx[] = {
   /* Control flow. */

   /* 0: Address calculation. */

   S_SQ_CF_WORD0_ADDR(7) | S_SQ_CF_ALU_WORD0_KCACHE_BANK0(TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS) |
      S_SQ_CF_ALU_WORD0_KCACHE_MODE0(V_SQ_CF_KCACHE_LOCK_1),
   S_SQ_CF_ALU_WORD1_KCACHE_ADDR0(
      TERAKAN_KCACHE_DWORD_LINE(TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH)) |
      S_SQ_CF_ALU_WORD1_COUNT(15 - 7) | EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 1: Fetch from the source. */

   S_SQ_CF_WORD0_ADDR(16),
   S_SQ_CF_WORD1_COUNT(0) | S_SQ_CF_WORD1_BARRIER(1) | EG_V_SQ_CF_WORD1_SQ_CF_INST_TEX,

   /* 2: Store the R component. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_ID(0) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_INST(V_RAT_INST_STORE_TYPED) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_WRITE_IND) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_INDEX_GPR(1),
   S_SQ_CF_ALLOC_EXPORT_WORD1_BUF_COMP_MASK(0b1111) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_VALID_PIXEL_MODE(1) | S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_MEM_RAT,

   /* 3: Moving the G and B components to GPR.X. */

   S_SQ_CF_WORD0_ADDR(18) | S_SQ_CF_ALU_WORD0_KCACHE_BANK0(TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS) |
      S_SQ_CF_ALU_WORD0_KCACHE_MODE0(V_SQ_CF_KCACHE_LOCK_1),
   S_SQ_CF_ALU_WORD1_COUNT(19 - 18) | S_SQ_CF_ALU_WORD1_BARRIER(1) |
      EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 4: Store the G component. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_ID(0) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_INST(V_RAT_INST_STORE_TYPED) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_WRITE_IND) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_INDEX_GPR(2),
   S_SQ_CF_ALLOC_EXPORT_WORD1_BUF_COMP_MASK(0b1111) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_VALID_PIXEL_MODE(1) | S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_MEM_RAT,

   /* 5: Store the B component. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_ID(0) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_INST(V_RAT_INST_STORE_TYPED) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_WRITE_IND) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(1) | S_SQ_CF_ALLOC_EXPORT_WORD0_INDEX_GPR(3),
   S_SQ_CF_ALLOC_EXPORT_WORD1_BUF_COMP_MASK(0b1111) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_VALID_PIXEL_MODE(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_MEM_RAT,

   /* 6: Perform a dummy export and end the program. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) | S_SQ_CF_ALLOC_EXPORT_WORD1_END_OF_PROGRAM(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* ALU clause before the fetch and R storing. */

   /* X in surfels plus the destination offset to PV.X.
    * Destination row address to PS.
    *
    * 7:    PV.X = MULADD_UINT24 R0.X, 3, CB[push].dst_offset
    * 8: (T) PS (Y) = MULLO_UINT R0.Y, CB[push].dst_pitch
    * 9: Literal X = 3, Y = unused
    * Cycle 0: X = R0, T constant
    * Cycle 2: Y = R0
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   TERAKAN_KCACHE_DWORD_WORD1_SRC2(TERAKAN_META_COPY_EXPAND_3X_CONST_DST_OFFSET) |
      S_SQ_ALU_WORD1_DST_GPR(0x7F - 4) | S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_MULADD_UINT24),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC1(TERAKAN_META_COPY_EXPAND_3X_CONST_DST_PITCH),
   S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   3,
   0,

   /* Destination R component address to R1.X.
    * Source row address to PS.
    *
    * 10:     R1.X = ADD_INT PS, PV.X
    * 11: (T) PS (Y) = MULLO_UINT R0.Y, CB[push].src_pitch
    * Cycle 0: T constant
    * Cycle 2: Y = R0
    */

   S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PS) | S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) |
      S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_DST_GPR(1) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC1(TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH),
   S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   /* Destination G component address to R2.X.
    * Source address to R0.W.
    *
    * 12:     R2.X = ADD_INT PV.X, 1
    * 13: (V) R0.W = MULADD_UINT24 R0.X, 3, PS
    * 14: Literal X = 3, Y = unused
    * Cycle 0: X = R0
    */

   S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_1_INT),
   S_SQ_ALU_WORD1_DST_GPR(2) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_OP3_SRC2_SEL(V_SQ_ALU_SRC_PS) | S_SQ_ALU_WORD1_DST_GPR(0) |
      S_SQ_ALU_WORD1_DST_CHAN(3) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_MULADD_UINT24),

   3,
   0,

   /* Destination B component address to R3.X.
    *
    * 15: (v) R3.X = ADD_INT PV.X, 1
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_1_INT),
   S_SQ_ALU_WORD1_DST_GPR(3) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT),

   /* 16, 17: Vertex-fetch from the source to R0.XYZ. */

   S_SQ_VTX_WORD0_VTX_INST(0) | S_SQ_VTX_WORD0_FETCH_TYPE(SQ_VTX_FETCH_NO_INDEX_OFFSET) |
      S_SQ_VTX_WORD0_BUFFER_ID(TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META) |
      S_SQ_VTX_WORD0_SRC_GPR(0) | S_SQ_VTX_WORD0_SRC_SEL_X(TERASCALE_SWIZZLE_W) |
      S_SQ_VTX_WORD0_MEGA_FETCH_COUNT(sizeof(uint32_t) * 3 - 1),
   S_SQ_VTX_WORD1_GPR_DST_GPR(0) | S_SQ_VTX_WORD1_DST_SEL_X(TERASCALE_SWIZZLE_X) |
      S_SQ_VTX_WORD1_DST_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_SQ_VTX_WORD1_DST_SEL_Z(TERASCALE_SWIZZLE_Z) |
      S_SQ_VTX_WORD1_DST_SEL_W(TERASCALE_SWIZZLE_MASK) | S_SQ_VTX_WORD1_USE_CONST_FIELDS(1),
   S_SQ_VTX_WORD2_MEGA_FETCH(1),
   0,

   /* ALU clause after storing R and before storing G and B. */

   /* Move the G component to R0.X.
    *
    * 18: (v) R0.X = MOV R0.Y, unused 0
    * Cycle 0: Y = R0
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),

   /* Move the B component to R1.X.
    *
    * 19: (v) R1.X = MOV R0.Z, unused 0
    * Cycle 0: Z = R0
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(2) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(1) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),
};

static uint32_t const terakan_meta_copy_expand_3x_ps_r9xx[] = {
   /* Control flow. */

   /* 0: Address calculation. */

   S_SQ_CF_WORD0_ADDR(8) | S_SQ_CF_ALU_WORD0_KCACHE_BANK0(TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS) |
      S_SQ_CF_ALU_WORD0_KCACHE_MODE0(V_SQ_CF_KCACHE_LOCK_1),
   S_SQ_CF_ALU_WORD1_KCACHE_ADDR0(
      TERAKAN_KCACHE_DWORD_LINE(TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH)) |
      S_SQ_CF_ALU_WORD1_COUNT(21 - 8) | EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 1: Fetch from the source. */

   S_SQ_CF_WORD0_ADDR(22),
   S_SQ_CF_WORD1_COUNT(0) | S_SQ_CF_WORD1_BARRIER(1) | EG_V_SQ_CF_WORD1_SQ_CF_INST_TEX,

   /* 2: Store the R component. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_ID(0) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_INST(V_RAT_INST_STORE_TYPED) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_WRITE_IND) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_INDEX_GPR(1),
   S_SQ_CF_ALLOC_EXPORT_WORD1_BUF_COMP_MASK(0b1111) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_VALID_PIXEL_MODE(1) | S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_MEM_RAT,

   /* 3: Moving the G and B components to GPR.X. */

   S_SQ_CF_WORD0_ADDR(24) | S_SQ_CF_ALU_WORD0_KCACHE_BANK0(TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS) |
      S_SQ_CF_ALU_WORD0_KCACHE_MODE0(V_SQ_CF_KCACHE_LOCK_1),
   S_SQ_CF_ALU_WORD1_COUNT(25 - 24) | S_SQ_CF_ALU_WORD1_BARRIER(1) |
      EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 4: Store the G component. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_ID(0) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_INST(V_RAT_INST_STORE_TYPED) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_WRITE_IND) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_INDEX_GPR(2),
   S_SQ_CF_ALLOC_EXPORT_WORD1_BUF_COMP_MASK(0b1111) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_VALID_PIXEL_MODE(1) | S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_MEM_RAT,

   /* 5: Store the B component. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_ID(0) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RAT_RAT_INST(V_RAT_INST_STORE_TYPED) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_WRITE_IND) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(1) | S_SQ_CF_ALLOC_EXPORT_WORD0_INDEX_GPR(3),
   S_SQ_CF_ALLOC_EXPORT_WORD1_BUF_COMP_MASK(0b1111) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_VALID_PIXEL_MODE(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_MEM_RAT,

   /* 6: Perform a dummy export. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_MASK) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_MASK) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* 7: End the program. */

   0,
   S_SQ_CF_WORD1_BARRIER(1) | CM_V_SQ_CF_WORD1_SQ_CF_INST_END,

   /* ALU clause before the fetch and R storing. */

   /* Destination row address to R1.X.
    * Done early due to the GPR dependency latency.
    *
    * MULLO_UINT uses 4 slots.
    *  8: R1.X = MULLO_UINT R0.Y, CB[push].dst_pitch
    *  9: PV.Y = MULLO_UINT R0.Y, CB[push].dst_pitch
    * 10: PV.Z = MULLO_UINT R0.Y, CB[push].dst_pitch
    * 11: PV.W = MULLO_UINT R0.Y, CB[push].dst_pitch
    * Cycle 0: Y = R0
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC1(TERAKAN_META_COPY_EXPAND_3X_CONST_DST_PITCH),
   S_SQ_ALU_WORD1_DST_GPR(1) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC1(TERAKAN_META_COPY_EXPAND_3X_CONST_DST_PITCH),
   S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC1(TERAKAN_META_COPY_EXPAND_3X_CONST_DST_PITCH),
   S_SQ_ALU_WORD1_DST_CHAN(2) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC1(TERAKAN_META_COPY_EXPAND_3X_CONST_DST_PITCH),
   S_SQ_ALU_WORD1_DST_CHAN(3) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   /* Source row address to PV.
    *
    * 12: PV.X = MULLO_UINT R0.Y, CB[push].src_pitch
    * 13: PV.Y = MULLO_UINT R0.Y, CB[push].src_pitch
    * 14: PV.Z = MULLO_UINT R0.Y, CB[push].src_pitch
    * 15: PV.W = MULLO_UINT R0.Y, CB[push].src_pitch
    * Cycle 0: Y = R0
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC1(TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH),
   S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC1(TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH),
   S_SQ_ALU_WORD1_DST_CHAN(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC1(TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH),
   S_SQ_ALU_WORD1_DST_CHAN(2) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC1(TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH),
   S_SQ_ALU_WORD1_DST_CHAN(3) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULLO_UINT),

   /* X in surfels plus the destination offset to PV.X.
    * Source address to R0.W.
    *
    * 16: PV.X = MULADD_UINT24 R0.X, 3, CB[push].dst_offset
    * 17: R0.W = MULADD_UINT24 R0.X, 3, PV
    * 18: Literal X = 3, Y = unused
    * Cycle 0: X = R0
    */

   S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   TERAKAN_KCACHE_DWORD_WORD1_SRC2(TERAKAN_META_COPY_EXPAND_3X_CONST_DST_OFFSET) |
      S_SQ_ALU_WORD1_DST_GPR(0x7F - 4) | S_SQ_ALU_WORD1_DST_CHAN(0) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_MULADD_UINT24),

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_LITERAL) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_OP3_SRC2_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD1_DST_GPR(0) |
      S_SQ_ALU_WORD1_DST_CHAN(3) |
      S_SQ_ALU_WORD1_OP3_ALU_INST(EG_V_SQ_ALU_WORD1_OP3_SQ_OP3_INST_MULADD_UINT24),

   3,
   0,

   /* Destination R component address to R1.X.
    *
    * 19: R1.X = ADD_INT R1.X, PV.X
    * Cycle 0: X = R1
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(1) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
   S_SQ_ALU_WORD1_DST_GPR(1) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT),

   /* Destination G component address to R2.X.
    *
    * 20: R2.X = ADD_INT PV.X, 1
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_1_INT),
   S_SQ_ALU_WORD1_DST_GPR(2) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT),

   /* Destination B component address to R3.X.
    *
    * 21: R3.X = ADD_INT PV.X, 1
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_PV) | S_SQ_ALU_WORD0_SRC0_CHAN(0) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_1_INT),
   S_SQ_ALU_WORD1_DST_GPR(3) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT),

   /* 22, 23: Vertex-fetch from the source to R0.XYZ. */

   S_SQ_VTX_WORD0_VTX_INST(0) | S_SQ_VTX_WORD0_FETCH_TYPE(SQ_VTX_FETCH_NO_INDEX_OFFSET) |
      S_SQ_VTX_WORD0_BUFFER_ID(TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META) |
      S_SQ_VTX_WORD0_SRC_GPR(0) | S_SQ_VTX_WORD0_SRC_SEL_X(TERASCALE_SWIZZLE_W),
   S_SQ_VTX_WORD1_GPR_DST_GPR(0) | S_SQ_VTX_WORD1_DST_SEL_X(TERASCALE_SWIZZLE_X) |
      S_SQ_VTX_WORD1_DST_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_SQ_VTX_WORD1_DST_SEL_Z(TERASCALE_SWIZZLE_Z) |
      S_SQ_VTX_WORD1_DST_SEL_W(TERASCALE_SWIZZLE_MASK) | S_SQ_VTX_WORD1_USE_CONST_FIELDS(1),
   0,
   0,

   /* ALU clause after storing R and before storing G and B. */

   /* Move the G component to R0.X.
    *
    * 24: R0.X = MOV R0.Y, unused 0
    * Cycle 0: Y = R0
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(1) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),

   /* Move the B component to R1.X.
    *
    * 25: R1.X = MOV R0.Z, unused 0
    * Cycle 0: Z = R0
    */

   S_SQ_ALU_WORD0_LAST(1) | S_SQ_ALU_WORD0_SRC0_SEL(0) | S_SQ_ALU_WORD0_SRC0_CHAN(2) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(1) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),
};

struct terakan_meta_shader const
   terakan_meta_copy_expand_3x_ps =
      {
         .r8xx =
            {
               .program = terakan_meta_copy_expand_3x_ps_r8xx,
               .program_size_bytes = sizeof(terakan_meta_copy_expand_3x_ps_r8xx),
               .static_registers =
                  {
                     .sq_pgm_resources =
                        {
                           S_028844_NUM_GPRS(4) | TERAKAN_META_SQ_PGM_RESOURCES_COMMON,
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
               .program = terakan_meta_copy_expand_3x_ps_r9xx,
               .program_size_bytes = sizeof(terakan_meta_copy_expand_3x_ps_r9xx),
               .static_registers =
                  {
                     .sq_pgm_resources =
                        {
                           S_028844_NUM_GPRS(4) | TERAKAN_META_SQ_PGM_RESOURCES_COMMON,
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
         .kcache_needed = (uint16_t)1 << TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS,
         .resources_needed =
            {
               [BITSET_BITWORD(TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META)] =
                  BITSET_BIT(TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META),
            },
         .stage =
            {
               .ps =
                  {
                     .db_shader_control = TERAKAN_META_DB_SHADER_CONTROL_PS_MEMORY_EXPORT,
                  },
            },
};

static void
terakan_meta_copy_expand_3x_begin(struct terakan_gfx_command_writer * const command_writer)
{
   command_writer->post_color_image_copy_write_barrier_actions |=
      TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_UAV | TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_PS;

   terakan_meta_begin_2d_immediate_rects(command_writer, TERAKAN_META_PA_CL_VTE_CNTL_2D,
                                         TERAKAN_META_DB_RENDER_OVERRIDE_DEFAULT, true);

   terakan_meta_set_vs(command_writer, TERAKAN_META_SHADER_POSITION_FROM_INDEX_VS);
   terakan_meta_set_ps(command_writer, TERAKAN_META_SHADER_COPY_EXPAND_3X_PS, true);

   terakan_meta_begin_cb(command_writer, V_028808_CB_NORMAL, 0xF, 0b0);

   command_writer->push_constants_state.up_to_date_push_constants_bound_to_stages &=
      ~VK_SHADER_STAGE_FRAGMENT_BIT;
}

void
terakan_meta_copy_expand_3x_buffer_to_image(
   struct terakan_gfx_command_writer * const command_writer,
   VkCopyBufferToImageInfo2 const * const copy_buffer_to_image_info)
{
   terakan_meta_copy_expand_3x_begin(command_writer);

   struct terakan_buffer const * const src_buffer =
      terakan_buffer_from_handle(copy_buffer_to_image_info->srcBuffer);
   struct terakan_image const * const dst_image =
      terakan_image_from_handle(copy_buffer_to_image_info->dstImage);

   unsigned const bytes_per_surfel = dst_image->surface.aspects[0].bytes_per_block / 3;

   uint32_t src_resource[8];
   terakan_meta_transfer_expand_3x_resource(bytes_per_surfel, src_resource);

   struct terakan_color_descriptor dst_uav = terakan_meta_transfer_expand_3x_uav(
      bytes_per_surfel, terakan_gfx_command_writer_physical_device(command_writer)
                           ->tiling_info.pipe_interleave_bytes_log2);
   /* For simplicity and not to explicitly handle UAV alignment as image slices are always aligned,
    * adjusting only the destination offset in the push constants. Assuming that images are never
    * 2^32 surfels or larger.
    */
   VkDeviceSize const dst_aspect_size_surfels_minus_one =
      ((VkDeviceSize)dst_image->surface.aspects[0].size_bytes_shr8 << 8) / bytes_per_surfel - 1;
   assert(dst_aspect_size_surfels_minus_one <= UINT32_MAX);
   dst_uav.base =
      (uint32_t)(dst_image->va >> 8) + dst_image->surface.aspects[0].offset_in_memory_bytes_shr8;
   dst_uav.dim = (uint32_t)dst_aspect_size_surfels_minus_one;
   terakan_hw_state_draw_set_cb_color(&command_writer->hw_state_draw, 0, dst_image->bo, &dst_uav,
                                      NULL, true);

   for (uint32_t region_index = 0; region_index < copy_buffer_to_image_info->regionCount;
        ++region_index) {
      VkBufferImageCopy2 const * const region = &copy_buffer_to_image_info->pRegions[region_index];

      uint32_t base_layer, layer_count;
      if (dst_image->vk.image_type == VK_IMAGE_TYPE_3D) {
         base_layer = (uint32_t)region->imageOffset.z;
         layer_count = region->imageExtent.depth;
      } else {
         base_layer = region->imageSubresource.baseArrayLayer;
         layer_count = vk_image_subresource_layer_count(&dst_image->vk, &region->imageSubresource);
      }

      uint32_t const src_y_pitch_surfels =
         3 * (region->bufferRowLength != 0 ? region->bufferRowLength : region->imageExtent.width);
      VkDeviceSize const src_z_pitch_bytes =
         bytes_per_surfel * (VkDeviceSize)src_y_pitch_surfels *
         (region->bufferImageHeight != 0 ? region->bufferImageHeight : region->imageExtent.height);
      VkDeviceSize const src_rect_extent_surfels =
         (VkDeviceSize)src_y_pitch_surfels * (region->imageExtent.height - 1) +
         3 * region->imageExtent.width;
      src_resource[1] = (uint32_t)(bytes_per_surfel * src_rect_extent_surfels - 1);
      src_resource[4] = (uint32_t)src_rect_extent_surfels;
      uint64_t src_va = src_buffer->va + region->bufferOffset;

      struct terakan_image_surface_level const * const dst_surface_level =
         &dst_image->surface.aspects[0].levels[region->imageSubresource.mipLevel];
      uint32_t const dst_z_pitch_surfels =
         ((VkDeviceSize)dst_surface_level->slice_size_bytes_shr8 << 8) / bytes_per_surfel;
      uint32_t dst_offset_surfels =
         (uint32_t)(((VkDeviceSize)(dst_surface_level->offset_in_memory_bytes_shr8 -
                                    dst_image->surface.aspects[0].offset_in_memory_bytes_shr8)
                     << 8) /
                    bytes_per_surfel) +
         dst_z_pitch_surfels * base_layer +
         (dst_surface_level->aligned_extent_surfels[0] * (uint32_t)region->imageOffset.y +
          3 * (uint32_t)region->imageOffset.x);

      for (uint32_t layer_index = 0; layer_index < layer_count; ++layer_index) {
         /* Constants are always changed because the destination offset is. */
         struct terakan_bo const * constants_bo;
         uint32_t constants_va_lines;
         uint32_t * const constants = terakan_push_buffer_allocate_kcache(
            command_writer->base.command_buffer,
            sizeof(uint32_t) * TERAKAN_META_COPY_EXPAND_3X_CONSTS_COUNT, &constants_bo,
            &constants_va_lines);
         if (unlikely(constants == NULL)) {
            return;
         }
         constants[TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH] = src_y_pitch_surfels;
         constants[TERAKAN_META_COPY_EXPAND_3X_CONST_DST_PITCH] =
            dst_surface_level->aligned_extent_surfels[0];
         constants[TERAKAN_META_COPY_EXPAND_3X_CONST_DST_OFFSET] = dst_offset_surfels;
         terakan_hw_state_draw_set_sq_kcache_fs(
            &command_writer->hw_state_draw, TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS,
            DIV_ROUND_UP(sizeof(uint32_t) * TERAKAN_META_COPY_EXPAND_3X_CONSTS_COUNT,
                         TERAKAN_KCACHE_HW_LINE_BYTES),
            constants_bo, constants_va_lines);

         src_resource[0] = (uint32_t)src_va;
         src_resource[2] =
            (src_resource[2] & C_030008_BASE_ADDRESS_HI) | S_030008_BASE_ADDRESS_HI(src_va >> 32);
         terakan_hw_state_draw_set_sq_resource_fs(
            &command_writer->hw_state_draw, TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META,
            src_buffer->bo, src_resource);

         VkRect2D const rect = {
            .extent =
               {
                  .width = region->imageExtent.width,
                  .height = region->imageExtent.height,
               },
         };
         terakan_meta_emit_rect_3_vertices_draw(command_writer, &rect, 1);

         src_va += src_z_pitch_bytes;
         dst_offset_surfels += dst_z_pitch_surfels;
      }
   }
}

void
terakan_meta_copy_expand_3x_image_to_buffer(
   struct terakan_gfx_command_writer * const command_writer,
   VkCopyImageToBufferInfo2 const * const copy_image_to_buffer_info)
{
   terakan_meta_copy_expand_3x_begin(command_writer);

   struct terakan_image const * const src_image =
      terakan_image_from_handle(copy_image_to_buffer_info->srcImage);
   struct terakan_buffer const * const dst_buffer =
      terakan_buffer_from_handle(copy_image_to_buffer_info->dstBuffer);

   unsigned const bytes_per_surfel = src_image->surface.aspects[0].bytes_per_block / 3;

   uint32_t src_resource[8];
   terakan_meta_transfer_expand_3x_resource(bytes_per_surfel, src_resource);

   unsigned const tile_pipe_interleave_bytes_log2 =
      terakan_gfx_command_writer_physical_device(command_writer)
         ->tiling_info.pipe_interleave_bytes_log2;

   struct terakan_color_descriptor dst_uav =
      terakan_meta_transfer_expand_3x_uav(bytes_per_surfel, tile_pipe_interleave_bytes_log2);

   uint32_t constants[TERAKAN_META_COPY_EXPAND_3X_CONSTS_COUNT] = {};
   struct terakan_bo const * constants_bo = NULL;
   uint32_t constants_va_lines;

   for (uint32_t region_index = 0; region_index < copy_image_to_buffer_info->regionCount;
        ++region_index) {
      VkBufferImageCopy2 const * const region = &copy_image_to_buffer_info->pRegions[region_index];

      uint32_t base_layer, layer_count;
      if (src_image->vk.image_type == VK_IMAGE_TYPE_3D) {
         base_layer = (uint32_t)region->imageOffset.z;
         layer_count = region->imageExtent.depth;
      } else {
         base_layer = region->imageSubresource.baseArrayLayer;
         layer_count = vk_image_subresource_layer_count(&src_image->vk, &region->imageSubresource);
      }

      struct terakan_image_surface_level const * const src_surface_level =
         &src_image->surface.aspects[0].levels[region->imageSubresource.mipLevel];
      VkDeviceSize const src_z_pitch_bytes = (VkDeviceSize)src_surface_level->slice_size_bytes_shr8
                                             << 8;
      uint32_t const src_rect_extent_surfels =
         src_surface_level->aligned_extent_surfels[0] * (region->imageExtent.height - 1) +
         3 * region->imageExtent.width;
      src_resource[1] = bytes_per_surfel * src_rect_extent_surfels - 1;
      src_resource[4] = src_rect_extent_surfels;
      uint64_t src_va =
         src_image->va + ((VkDeviceSize)src_surface_level->offset_in_memory_bytes_shr8 << 8) +
         src_z_pitch_bytes * base_layer +
         bytes_per_surfel * (VkDeviceSize)(src_surface_level->aligned_extent_surfels[0] *
                                              (uint32_t)region->imageOffset.y +
                                           3 * (uint32_t)region->imageOffset.x);

      uint32_t const dst_y_pitch_surfels =
         3 * (region->bufferRowLength != 0 ? region->bufferRowLength : region->imageExtent.width);
      VkDeviceSize const dst_z_pitch_bytes =
         bytes_per_surfel * (VkDeviceSize)dst_y_pitch_surfels *
         (region->bufferImageHeight != 0 ? region->bufferImageHeight : region->imageExtent.height);
      VkDeviceSize const dst_rect_extent_surfels =
         (VkDeviceSize)dst_y_pitch_surfels * (region->imageExtent.height - 1) +
         3 * region->imageExtent.width;
      uint64_t dst_va = dst_buffer->va + region->bufferOffset;

      if (constants[TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH] !=
             src_surface_level->aligned_extent_surfels[0] ||
          constants[TERAKAN_META_COPY_EXPAND_3X_CONST_DST_PITCH] != dst_y_pitch_surfels) {
         constants_bo = NULL;
         constants[TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH] =
            src_surface_level->aligned_extent_surfels[0];
         constants[TERAKAN_META_COPY_EXPAND_3X_CONST_DST_PITCH] = dst_y_pitch_surfels;
      }

      for (uint32_t layer_index = 0; layer_index < layer_count; ++layer_index) {
         uint64_t const dst_va_aligned = dst_va >> tile_pipe_interleave_bytes_log2
                                                      << tile_pipe_interleave_bytes_log2;
         uint32_t const dst_offset_surfels =
            (uint32_t)((dst_va - dst_va_aligned) / bytes_per_surfel);

         if (constants[TERAKAN_META_COPY_EXPAND_3X_CONST_DST_OFFSET] != dst_offset_surfels) {
            constants_bo = NULL;
            constants[TERAKAN_META_COPY_EXPAND_3X_CONST_DST_OFFSET] = dst_offset_surfels;
         }

         if (constants_bo == NULL) {
            void * const constants_mapping = terakan_push_buffer_allocate_kcache(
               command_writer->base.command_buffer, sizeof(constants), &constants_bo,
               &constants_va_lines);
            if (unlikely(constants_mapping == NULL)) {
               return;
            }
            memcpy(constants_mapping, constants, sizeof(constants));
            terakan_hw_state_draw_set_sq_kcache_fs(
               &command_writer->hw_state_draw, TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS,
               DIV_ROUND_UP(sizeof(constants), TERAKAN_KCACHE_HW_LINE_BYTES), constants_bo,
               constants_va_lines);
         }

         src_resource[0] = (uint32_t)src_va;
         src_resource[2] =
            (src_resource[2] & C_030008_BASE_ADDRESS_HI) | S_030008_BASE_ADDRESS_HI(src_va >> 32);
         terakan_hw_state_draw_set_sq_resource_fs(
            &command_writer->hw_state_draw, TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META,
            src_image->bo, src_resource);

         dst_uav.base = (uint32_t)(dst_va_aligned >> 8);
         dst_uav.dim = (uint32_t)(dst_offset_surfels + dst_rect_extent_surfels - 1);
         terakan_hw_state_draw_set_cb_color(&command_writer->hw_state_draw, 0, dst_buffer->bo,
                                            &dst_uav, NULL, true);

         VkRect2D const rect = {
            .extent =
               {
                  .width = region->imageExtent.width,
                  .height = region->imageExtent.height,
               },
         };
         terakan_meta_emit_rect_3_vertices_draw(command_writer, &rect, 1);

         src_va += src_z_pitch_bytes;
         dst_va += dst_z_pitch_bytes;
      }
   }
}

void
terakan_meta_copy_expand_3x_image(struct terakan_gfx_command_writer * const command_writer,
                                  VkCopyImageInfo2 const * const copy_image_info)
{
   terakan_meta_copy_expand_3x_begin(command_writer);

   struct terakan_image const * const src_image =
      terakan_image_from_handle(copy_image_info->srcImage);
   struct terakan_image const * const dst_image =
      terakan_image_from_handle(copy_image_info->dstImage);

   unsigned const bytes_per_surfel = dst_image->surface.aspects[0].bytes_per_block / 3;

   uint32_t src_resource[8];
   terakan_meta_transfer_expand_3x_resource(bytes_per_surfel, src_resource);

   struct terakan_color_descriptor dst_uav = terakan_meta_transfer_expand_3x_uav(
      bytes_per_surfel, terakan_gfx_command_writer_physical_device(command_writer)
                           ->tiling_info.pipe_interleave_bytes_log2);
   /* For simplicity and not to explicitly handle UAV alignment as image slices are always aligned,
    * adjusting only the destination offset in the push constants. Assuming that images are never
    * 2^32 surfels or larger.
    */
   VkDeviceSize const dst_aspect_size_surfels_minus_one =
      ((VkDeviceSize)dst_image->surface.aspects[0].size_bytes_shr8 << 8) / bytes_per_surfel - 1;
   assert(dst_aspect_size_surfels_minus_one <= UINT32_MAX);
   dst_uav.base =
      (uint32_t)(dst_image->va >> 8) + dst_image->surface.aspects[0].offset_in_memory_bytes_shr8;
   dst_uav.dim = (uint32_t)dst_aspect_size_surfels_minus_one;
   terakan_hw_state_draw_set_cb_color(&command_writer->hw_state_draw, 0, dst_image->bo, &dst_uav,
                                      NULL, true);

   for (uint32_t region_index = 0; region_index < copy_image_info->regionCount; ++region_index) {
      VkImageCopy2 const * const region = &copy_image_info->pRegions[region_index];

      uint32_t src_base_layer, layer_count;
      if (src_image->vk.image_type == VK_IMAGE_TYPE_3D) {
         src_base_layer = (uint32_t)region->srcOffset.z;
         layer_count = region->extent.depth;
      } else {
         src_base_layer = region->srcSubresource.baseArrayLayer;
         layer_count = vk_image_subresource_layer_count(&src_image->vk, &region->srcSubresource);
      }
      uint32_t const dst_base_layer = dst_image->vk.image_type == VK_IMAGE_TYPE_3D
                                         ? (uint32_t)region->dstOffset.z
                                         : region->dstSubresource.baseArrayLayer;

      struct terakan_image_surface_level const * const src_surface_level =
         &src_image->surface.aspects[0].levels[region->srcSubresource.mipLevel];
      uint32_t const src_rect_extent_surfels =
         src_surface_level->aligned_extent_surfels[0] * (region->extent.height - 1) +
         3 * region->extent.width;
      src_resource[1] = bytes_per_surfel * src_rect_extent_surfels - 1;
      src_resource[4] = src_rect_extent_surfels;
      VkDeviceSize const src_z_pitch_bytes = (VkDeviceSize)src_surface_level->slice_size_bytes_shr8
                                             << 8;
      uint64_t src_va =
         src_image->va + ((VkDeviceSize)src_surface_level->offset_in_memory_bytes_shr8 << 8) +
         src_z_pitch_bytes * src_base_layer +
         bytes_per_surfel * (VkDeviceSize)(src_surface_level->aligned_extent_surfels[0] *
                                              (uint32_t)region->srcOffset.y +
                                           3 * (uint32_t)region->srcOffset.x);

      struct terakan_image_surface_level const * const dst_surface_level =
         &dst_image->surface.aspects[0].levels[region->dstSubresource.mipLevel];
      uint32_t const dst_z_pitch_surfels =
         ((VkDeviceSize)dst_surface_level->slice_size_bytes_shr8 << 8) / bytes_per_surfel;
      uint32_t dst_offset_surfels =
         (uint32_t)(((VkDeviceSize)(dst_surface_level->offset_in_memory_bytes_shr8 -
                                    dst_image->surface.aspects[0].offset_in_memory_bytes_shr8)
                     << 8) /
                    bytes_per_surfel) +
         dst_z_pitch_surfels * dst_base_layer +
         (dst_surface_level->aligned_extent_surfels[0] * (uint32_t)region->dstOffset.y +
          3 * (uint32_t)region->dstOffset.x);

      for (uint32_t layer_index = 0; layer_index < layer_count; ++layer_index) {
         /* Constants are always changed because the destination offset is. */
         struct terakan_bo const * constants_bo;
         uint32_t constants_va_lines;
         uint32_t * const constants = terakan_push_buffer_allocate_kcache(
            command_writer->base.command_buffer,
            sizeof(uint32_t) * TERAKAN_META_COPY_EXPAND_3X_CONSTS_COUNT, &constants_bo,
            &constants_va_lines);
         if (unlikely(constants == NULL)) {
            return;
         }
         constants[TERAKAN_META_COPY_EXPAND_3X_CONST_SRC_PITCH] =
            src_surface_level->aligned_extent_surfels[0];
         constants[TERAKAN_META_COPY_EXPAND_3X_CONST_DST_PITCH] =
            dst_surface_level->aligned_extent_surfels[0];
         constants[TERAKAN_META_COPY_EXPAND_3X_CONST_DST_OFFSET] = dst_offset_surfels;
         terakan_hw_state_draw_set_sq_kcache_fs(
            &command_writer->hw_state_draw, TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS,
            DIV_ROUND_UP(sizeof(uint32_t) * TERAKAN_META_COPY_EXPAND_3X_CONSTS_COUNT,
                         TERAKAN_KCACHE_HW_LINE_BYTES),
            constants_bo, constants_va_lines);

         src_resource[0] = (uint32_t)src_va;
         src_resource[2] =
            (src_resource[2] & C_030008_BASE_ADDRESS_HI) | S_030008_BASE_ADDRESS_HI(src_va >> 32);
         terakan_hw_state_draw_set_sq_resource_fs(
            &command_writer->hw_state_draw, TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META,
            src_image->bo, src_resource);

         VkRect2D const rect = {
            .extent =
               {
                  .width = region->extent.width,
                  .height = region->extent.height,
               },
         };
         terakan_meta_emit_rect_3_vertices_draw(command_writer, &rect, 1);

         src_va += src_z_pitch_bytes;
         dst_offset_surfels += dst_z_pitch_surfels;
      }
   }
}
