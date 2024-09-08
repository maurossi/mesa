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

#include "terakan_command_buffer.h"
#include "terakan_entrypoints.h"

#include "gallium/drivers/r600/evergreend.h"
#include "util/macros.h"

#include <stdint.h>

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetDepthTestEnable(VkCommandBuffer const commandBuffer, VkBool32 const depthTestEnable)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(state, TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_CONTROL,
                                     &state->db_depth_control, C_028800_Z_ENABLE,
                                     S_028800_Z_ENABLE(depthTestEnable != VK_FALSE));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetDepthWriteEnable(VkCommandBuffer const commandBuffer, VkBool32 const depthWriteEnable)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(state, TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_CONTROL,
                                     &state->db_depth_control, C_028800_Z_WRITE_ENABLE,
                                     S_028800_Z_WRITE_ENABLE(depthWriteEnable != VK_FALSE));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetDepthCompareOp(VkCommandBuffer const commandBuffer, VkCompareOp const depthCompareOp)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(state, TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_CONTROL,
                                     &state->db_depth_control, C_028800_ZFUNC,
                                     S_028800_ZFUNC((uint32_t)depthCompareOp));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetStencilTestEnable(VkCommandBuffer const commandBuffer,
                                VkBool32 const stencilTestEnable)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   terakan_state_draw_replace_fields(state, TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_CONTROL,
                                     &state->db_depth_control, C_028800_STENCIL_ENABLE,
                                     S_028800_STENCIL_ENABLE(stencilTestEnable != VK_FALSE));
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetStencilOp(VkCommandBuffer const commandBuffer, VkStencilFaceFlags const faceMask,
                        VkStencilOp const failOp, VkStencilOp const passOp,
                        VkStencilOp const depthFailOp, VkCompareOp const compareOp)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;
   uint32_t db_depth_control_keep = UINT32_MAX, db_depth_control_set = 0;
   if (faceMask & VK_STENCIL_FACE_FRONT_BIT) {
      db_depth_control_keep &= C_028800_STENCILFAIL & C_028800_STENCILZPASS &
                               C_028800_STENCILZFAIL & C_028800_STENCILFUNC;
      db_depth_control_set |=
         S_028800_STENCILFAIL((uint32_t)failOp) | S_028800_STENCILZPASS((uint32_t)passOp) |
         S_028800_STENCILZFAIL((uint32_t)depthFailOp) | S_028800_STENCILFUNC((uint32_t)compareOp);
   }
   if (faceMask & VK_STENCIL_FACE_BACK_BIT) {
      db_depth_control_keep &= C_028800_STENCILFAIL_BF & C_028800_STENCILZPASS_BF &
                               C_028800_STENCILZFAIL_BF & C_028800_STENCILFUNC_BF;
      db_depth_control_set |= S_028800_STENCILFAIL_BF((uint32_t)failOp) |
                              S_028800_STENCILZPASS_BF((uint32_t)passOp) |
                              S_028800_STENCILZFAIL_BF((uint32_t)depthFailOp) |
                              S_028800_STENCILFUNC_BF((uint32_t)compareOp);
   }
   terakan_state_draw_replace_fields(state, TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_CONTROL,
                                     &state->db_depth_control, db_depth_control_keep,
                                     db_depth_control_set);
}

static void
terakan_state_draw_set_stencil_ref_mask(VkCommandBuffer const command_buffer_handle,
                                        VkStencilFaceFlags const face_mask, uint32_t const value,
                                        unsigned const shift)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(command_buffer_handle)->command_writer.gfx->state_draw;
   uint32_t const db_stencilrefmask_keep = ~((uint32_t)UINT8_MAX << shift);
   uint32_t const db_stencilrefmask_set = (value & UINT8_MAX) << shift;
   for (unsigned face_index = 0; face_index < 2; ++face_index) {
      /* VK_STENCIL_FACE_FRONT_BIT and VK_STENCIL_FACE_BACK_BIT are 1 << 0 and 1 << 1 respectively.
       */
      if (!(face_mask & (VkStencilFaceFlagBits)BITFIELD_BIT(face_index))) {
         continue;
      }
      terakan_state_draw_replace_fields(state, TERAKAN_STATE_DRAW_INDEX_DB_STENCILREFMASK,
                                        &state->db_stencilrefmask_front_back[face_index],
                                        db_stencilrefmask_keep, db_stencilrefmask_set);
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetStencilCompareMask(VkCommandBuffer const commandBuffer,
                                 VkStencilFaceFlags const faceMask, uint32_t const compareMask)
{
   terakan_state_draw_set_stencil_ref_mask(commandBuffer, faceMask, compareMask, 8);
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetStencilWriteMask(VkCommandBuffer const commandBuffer,
                               VkStencilFaceFlags const faceMask, uint32_t const writeMask)
{
   terakan_state_draw_set_stencil_ref_mask(commandBuffer, faceMask, writeMask, 16);
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetStencilReference(VkCommandBuffer const commandBuffer,
                               VkStencilFaceFlags const faceMask, uint32_t const reference)
{
   terakan_state_draw_set_stencil_ref_mask(commandBuffer, faceMask, reference, 0);
}
