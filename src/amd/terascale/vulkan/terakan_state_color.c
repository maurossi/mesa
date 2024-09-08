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

#include "terakan_command_buffer.h"
#include "terakan_entrypoints.h"
#include "terakan_hw_state.h"

#include "util/macros.h"

#include <stdbool.h>
#include <string.h>

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetBlendConstants(VkCommandBuffer const commandBuffer, float const blendConstants[4])
{
   struct terakan_hw_state_draw * const hw_state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->hw_state_draw;
   /* The blend constant is not needed by internal draws, modify hw_state_draw directly. */
   bool const modified =
      memcmp(hw_state_draw->cb_blend_rgba, blendConstants, sizeof(float) * 4) != 0;
   memcpy(hw_state_draw->cb_blend_rgba, blendConstants, sizeof(float) * 4);
   terakan_hw_state_draw_written(hw_state_draw, TERAKAN_HW_STATE_DRAW_CB_BLEND_RGBA, modified);
}
