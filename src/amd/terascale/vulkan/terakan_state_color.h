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

#ifndef TERAKAN_STATE_COLOR_H
#define TERAKAN_STATE_COLOR_H

#include "gallium/drivers/r600/evergreend.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint8_t
terakan_state_draw_logic_op_rop3(VkLogicOp const logic_op)
{
   switch (logic_op) {
   case VK_LOGIC_OP_CLEAR:
      return 0x00;
   case VK_LOGIC_OP_AND:
      return 0x88;
   case VK_LOGIC_OP_AND_REVERSE:
      return 0x44;
   case VK_LOGIC_OP_COPY:
      return 0xCC;
   case VK_LOGIC_OP_AND_INVERTED:
      return 0x22;
   case VK_LOGIC_OP_NO_OP:
      return 0xAA;
   case VK_LOGIC_OP_XOR:
      return 0x66;
   case VK_LOGIC_OP_OR:
      return 0xEE;
   case VK_LOGIC_OP_NOR:
      return 0x11;
   case VK_LOGIC_OP_EQUIVALENT:
      return 0x99;
   case VK_LOGIC_OP_INVERT:
      return 0x55;
   case VK_LOGIC_OP_OR_REVERSE:
      return 0xDD;
   case VK_LOGIC_OP_COPY_INVERTED:
      return 0x33;
   case VK_LOGIC_OP_OR_INVERTED:
      return 0xBB;
   case VK_LOGIC_OP_NAND:
      return 0x77;
   case VK_LOGIC_OP_SET:
      return 0xFF;
   default:
      assert(!"Unsupported logical operation");
      return 0xCC;
   }
}

uint32_t terakan_state_draw_blend_factor_translate(VkBlendFactor blend_factor,
                                                   bool allow_dual_source);

static inline uint32_t
terakan_state_draw_blend_op_translate(VkBlendOp const blend_op)
{
   switch (blend_op) {
   case VK_BLEND_OP_ADD:
      return V_028780_COMB_DST_PLUS_SRC;
   case VK_BLEND_OP_SUBTRACT:
      return V_028780_COMB_SRC_MINUS_DST;
   case VK_BLEND_OP_REVERSE_SUBTRACT:
      return V_028780_COMB_DST_MINUS_SRC;
   case VK_BLEND_OP_MIN:
      return V_028780_COMB_MIN_DST_SRC;
   case VK_BLEND_OP_MAX:
      return V_028780_COMB_MAX_DST_SRC;
   default:
      assert(!"Unsupported blend operation");
      return V_028780_COMB_DST_PLUS_SRC;
   }
}

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_STATE_COLOR_H */
