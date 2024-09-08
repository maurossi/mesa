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
#include "terakan_image.h"
#include "terakan_state.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

VKAPI_ATTR void VKAPI_CALL
terakan_CmdBeginRendering(VkCommandBuffer const commandBuffer,
                          VkRenderingInfo const * const pRenderingInfo)
{
   struct terakan_state_draw * const state_draw =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;

   /* TODO(Triang3l): Don't set CB_COLOR# directly, they need to be remapped according to the pixel
    * shader usage. Also, secondary command buffers need inheritance.
    */

   for (uint32_t color_attachment_index = 0;
        color_attachment_index < pRenderingInfo->colorAttachmentCount; ++color_attachment_index) {
      struct terakan_image_view const * const color_view = terakan_image_view_from_handle(
         pRenderingInfo->pColorAttachments[color_attachment_index].imageView);
      if (color_view != NULL) {
         state_draw->cb_color_bo[color_attachment_index] = color_view->bo;
         memcpy(&state_draw->cb_color[color_attachment_index], &color_view->color,
                sizeof(struct terakan_color_descriptor));
         terakan_color_descriptor_image_view_to_color_attachment(
            &state_draw->cb_color[color_attachment_index]);
         memcpy(&state_draw->cb_color_meta[color_attachment_index], &color_view->color_meta,
                sizeof(struct terakan_color_meta_descriptor));
      } else {
         state_draw->cb_color_bo[color_attachment_index] = NULL;
      }
      terakan_state_draw_written(
         state_draw, (enum terakan_state_draw_index)((uint32_t)TERAKAN_STATE_DRAW_CB_COLOR_FIRST +
                                                     color_attachment_index));
   }
}
