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

#include "terakan_state_color.h"

#include "terakan_command_buffer.h"
#include "terakan_entrypoints.h"

#include "gallium/drivers/r600/evergreend.h"
#include "util/macros.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetLogicOpEnableEXT(VkCommandBuffer const commandBuffer, VkBool32 const logicOpEnable)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   bool const enable = logicOpEnable != VK_FALSE;
   if (state->logic_op.enable != enable) {
      state->logic_op.enable = enable;
      terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_LOGIC_OP);
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetLogicOpEXT(VkCommandBuffer const commandBuffer, VkLogicOp const logicOp)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   uint8_t const rop3 = terakan_state_draw_logic_op_rop3(logicOp);
   if (state->logic_op.enable && state->logic_op.rop3 != rop3) {
      terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_LOGIC_OP);
   }
   state->logic_op.rop3 = rop3;
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetColorBlendEnableEXT(VkCommandBuffer const commandBuffer,
                                  uint32_t const firstAttachment, uint32_t const attachmentCount,
                                  VkBool32 const * const pColorBlendEnables)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   for (uint32_t attachment_relative_index = 0; attachment_relative_index < attachmentCount;
        ++attachment_relative_index) {
      uint32_t const attachment_index = firstAttachment + attachment_relative_index;
      assert(attachment_index < ARRAY_SIZE(state->cb_blend_control.attachments));
      uint32_t * const cb_blend_control_ptr =
         &state->cb_blend_control.attachments[attachment_index];
      bool const attachment_enable = pColorBlendEnables[attachment_relative_index] != VK_FALSE;
      if (G_028780_BLEND_CONTROL_ENABLE(*cb_blend_control_ptr) != attachment_enable) {
         *cb_blend_control_ptr = (*cb_blend_control_ptr & C_028780_BLEND_CONTROL_ENABLE) |
                                 S_028780_BLEND_CONTROL_ENABLE(attachment_enable);
         terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_CB_BLEND_CONTROL);
      }
   }
}

uint32_t
terakan_state_draw_blend_factor_translate(VkBlendFactor const blend_factor,
                                          bool const allow_dual_source)
{
   switch (blend_factor) {
   case VK_BLEND_FACTOR_ZERO:
      return V_028780_BLEND_ZERO;
   case VK_BLEND_FACTOR_ONE:
      return V_028780_BLEND_ONE;
   case VK_BLEND_FACTOR_SRC_COLOR:
      return V_028780_BLEND_SRC_COLOR;
   case VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
      return V_028780_BLEND_ONE_MINUS_SRC_COLOR;
   case VK_BLEND_FACTOR_DST_COLOR:
      return V_028780_BLEND_DST_COLOR;
   case VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR:
      return V_028780_BLEND_ONE_MINUS_DST_COLOR;
   case VK_BLEND_FACTOR_SRC_ALPHA:
      return V_028780_BLEND_SRC_ALPHA;
   case VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
      return V_028780_BLEND_ONE_MINUS_SRC_ALPHA;
   case VK_BLEND_FACTOR_DST_ALPHA:
      return V_028780_BLEND_DST_ALPHA;
   case VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
      return V_028780_BLEND_ONE_MINUS_DST_ALPHA;
   case VK_BLEND_FACTOR_CONSTANT_COLOR:
      return V_028780_BLEND_CONST_COLOR;
   case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
      return V_028780_BLEND_ONE_MINUS_CONST_COLOR;
   case VK_BLEND_FACTOR_CONSTANT_ALPHA:
      return V_028780_BLEND_CONST_ALPHA;
   case VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
      return V_028780_BLEND_ONE_MINUS_CONST_ALPHA;
   case VK_BLEND_FACTOR_SRC_ALPHA_SATURATE:
      return V_028780_BLEND_SRC_ALPHA_SATURATE;
   case VK_BLEND_FACTOR_SRC1_COLOR:
      return allow_dual_source ? V_028780_BLEND_SRC1_COLOR : V_028780_BLEND_ZERO;
   case VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR:
      return allow_dual_source ? V_028780_BLEND_INV_SRC1_COLOR : V_028780_BLEND_ZERO;
   case VK_BLEND_FACTOR_SRC1_ALPHA:
      return allow_dual_source ? V_028780_BLEND_SRC1_ALPHA : V_028780_BLEND_ZERO;
   case VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA:
      return allow_dual_source ? V_028780_BLEND_INV_SRC1_ALPHA : V_028780_BLEND_ZERO;
   default:
      assert(!"Unsupported blend factor");
      return V_028780_BLEND_ZERO;
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetColorBlendEquationEXT(VkCommandBuffer const commandBuffer,
                                    uint32_t const firstAttachment, uint32_t const attachmentCount,
                                    VkColorBlendEquationEXT const * const pColorBlendEquations)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   for (uint32_t attachment_relative_index = 0; attachment_relative_index < attachmentCount;
        ++attachment_relative_index) {
      uint32_t const attachment_index = firstAttachment + attachment_relative_index;
      assert(attachment_index < ARRAY_SIZE(state->cb_blend_control.attachments));
      uint32_t * const cb_blend_control_ptr =
         &state->cb_blend_control.attachments[attachment_index];
      bool const attachment_enable = G_028780_BLEND_CONTROL_ENABLE(*cb_blend_control_ptr);
      VkColorBlendEquationEXT const * const equation =
         &pColorBlendEquations[attachment_relative_index];
      bool const allow_dual_source = attachment_index == 0;
      uint32_t cb_blend_control =
         S_028780_BLEND_CONTROL_ENABLE(attachment_enable) |
         S_028780_COLOR_SRCBLEND(terakan_state_draw_blend_factor_translate(
            equation->srcColorBlendFactor, allow_dual_source)) |
         S_028780_COLOR_DESTBLEND(terakan_state_draw_blend_factor_translate(
            equation->dstColorBlendFactor, allow_dual_source)) |
         S_028780_COLOR_COMB_FCN(terakan_state_draw_blend_op_translate(equation->colorBlendOp));
      if (equation->srcAlphaBlendFactor != equation->srcColorBlendFactor ||
          equation->dstAlphaBlendFactor != equation->dstColorBlendFactor ||
          equation->alphaBlendOp != equation->colorBlendOp) {
         cb_blend_control |=
            S_028780_SEPARATE_ALPHA_BLEND(1) |
            S_028780_ALPHA_SRCBLEND(terakan_state_draw_blend_factor_translate(
               equation->srcAlphaBlendFactor, allow_dual_source)) |
            S_028780_ALPHA_DESTBLEND(terakan_state_draw_blend_factor_translate(
               equation->dstAlphaBlendFactor, allow_dual_source)) |
            S_028780_ALPHA_COMB_FCN(terakan_state_draw_blend_op_translate(equation->alphaBlendOp));
      }
      if (*cb_blend_control_ptr != cb_blend_control) {
         *cb_blend_control_ptr = cb_blend_control;
         if (attachment_enable) {
            terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_CB_BLEND_CONTROL);
         }
      }
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetColorWriteMaskEXT(VkCommandBuffer const commandBuffer, uint32_t const firstAttachment,
                                uint32_t const attachmentCount,
                                VkColorComponentFlags const * const pColorWriteMasks)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   for (uint32_t attachment_relative_index = 0; attachment_relative_index < attachmentCount;
        ++attachment_relative_index) {
      uint32_t const attachment_index = firstAttachment + attachment_relative_index;
      assert(attachment_index < ARRAY_SIZE(state->cb_blend_control.attachments));
      uint8_t const attachment_write_mask =
         (uint8_t)pColorWriteMasks[attachment_relative_index] & 0b1111;
      if (state->cb_target_mask.attachment_write_masks[attachment_index] != attachment_write_mask) {
         state->cb_target_mask.attachment_write_masks[attachment_index] = attachment_write_mask;
         terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_CB_TARGET_MASK);
      }
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetColorWriteEnableEXT(VkCommandBuffer const commandBuffer,
                                  uint32_t const attachmentCount,
                                  VkBool32 const * const pColorWriteEnables)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   assert(attachmentCount <= TERAKAN_COLOR_HW_RTV_COUNT);
   uint8_t write_enable =
      state->cb_target_mask.attachment_write_enable & ~(uint8_t)BITFIELD_MASK(attachmentCount);
   for (uint32_t attachment_index = 0; attachment_index < attachmentCount; ++attachment_index) {
      if (pColorWriteEnables[attachment_index]) {
         write_enable |= (uint8_t)1 << attachment_index;
      }
   }
   if (state->cb_target_mask.attachment_write_enable != write_enable) {
      state->cb_target_mask.attachment_write_enable = write_enable;
      terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_CB_TARGET_MASK);
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetBlendConstants(VkCommandBuffer const commandBuffer, float const blendConstants[4])
{
   struct terakan_hw_state_draw * const hw_state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->hw_state_draw;
   /* The blend constant is not needed by internal draws, modify hw_state_draw directly. */
   bool const modified =
      memcmp(hw_state_draw->cb_blend_rgba, blendConstants, sizeof(float) * 4) != 0;
   memcpy(hw_state_draw->cb_blend_rgba, blendConstants, sizeof(float) * 4);
   terakan_hw_state_draw_written(hw_state_draw, TERAKAN_HW_STATE_DRAW_INDEX_CB_BLEND_RGBA,
                                 modified);
}
