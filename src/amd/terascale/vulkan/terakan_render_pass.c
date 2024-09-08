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
#include "terakan_descriptor.h"
#include "terakan_entrypoints.h"
#include "terakan_image.h"
#include "terakan_state.h"

#include "amd/terascale/common/terascale_format.h"
#include "util/macros.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

VKAPI_ATTR void VKAPI_CALL
terakan_CmdBeginRendering(VkCommandBuffer const commandBuffer,
                          VkRenderingInfo const * const pRenderingInfo)
{
   struct terakan_state_draw * const state =
      &terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx->state_draw;

   uint32_t clear_attachment_count = 0;
   VkClearAttachment clear_attachments[TERAKAN_COLOR_HW_RTV_COUNT + 1];

   terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER);
   state->db_depth_stencil_buffer.bo = NULL;
   memset(&state->db_depth_stencil_buffer.descriptor, 0,
          sizeof(state->db_depth_stencil_buffer.descriptor));
   VkClearDepthStencilValue depth_stencil_clear_value = {};
   VkImageAspectFlags depth_stencil_clear_aspects = 0;
   if (pRenderingInfo->pDepthAttachment != NULL) {
      struct terakan_image_view const * const depth_view =
         terakan_image_view_from_handle(pRenderingInfo->pDepthAttachment->imageView);
      if (depth_view != NULL &&
          G_028040_FORMAT(depth_view->depth_stencil.z_info) != V_028040_Z_INVALID) {
         state->db_depth_stencil_buffer.bo = depth_view->bo;
         state->db_depth_stencil_buffer.descriptor.view = depth_view->depth_stencil.view;
         state->db_depth_stencil_buffer.descriptor.z_info = depth_view->depth_stencil.z_info;
         state->db_depth_stencil_buffer.descriptor.z_base = depth_view->depth_stencil.z_base;
         state->db_depth_stencil_buffer.descriptor.size = depth_view->depth_stencil.size;
         state->db_depth_stencil_buffer.descriptor.slice = depth_view->depth_stencil.slice;
         if (pRenderingInfo->pDepthAttachment->loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) {
            depth_stencil_clear_value.depth =
               pRenderingInfo->pDepthAttachment->clearValue.depthStencil.depth;
            depth_stencil_clear_aspects |= VK_IMAGE_ASPECT_DEPTH_BIT;
         }
      }
   }
   if (pRenderingInfo->pStencilAttachment != NULL) {
      struct terakan_image_view const * const stencil_view =
         terakan_image_view_from_handle(pRenderingInfo->pStencilAttachment->imageView);
      if (stencil_view != NULL &&
          G_028044_FORMAT(stencil_view->depth_stencil.stencil_info) != V_028044_STENCIL_INVALID) {
         if (state->db_depth_stencil_buffer.bo == NULL) {
            state->db_depth_stencil_buffer.bo = stencil_view->bo;
            state->db_depth_stencil_buffer.descriptor.view = stencil_view->depth_stencil.view;
            state->db_depth_stencil_buffer.descriptor.z_info =
               stencil_view->depth_stencil.z_info &
               (C_028040_FORMAT & C_028040_ZRANGE_PRECISION & C_028040_TILE_SPLIT);
            state->db_depth_stencil_buffer.descriptor.size = stencil_view->depth_stencil.size;
            state->db_depth_stencil_buffer.descriptor.slice = stencil_view->depth_stencil.slice;
         }
         state->db_depth_stencil_buffer.descriptor.stencil_info =
            stencil_view->depth_stencil.stencil_info;
         state->db_depth_stencil_buffer.descriptor.stencil_base =
            stencil_view->depth_stencil.stencil_base;
         if (pRenderingInfo->pStencilAttachment->loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) {
            depth_stencil_clear_value.stencil =
               pRenderingInfo->pStencilAttachment->clearValue.depthStencil.stencil;
            depth_stencil_clear_aspects |= VK_IMAGE_ASPECT_STENCIL_BIT;
         }
      }
   }
   if (depth_stencil_clear_aspects) {
      VkClearAttachment * const clear_depth_stencil_attachment =
         &clear_attachments[clear_attachment_count++];
      clear_depth_stencil_attachment->aspectMask = depth_stencil_clear_aspects;
      clear_depth_stencil_attachment->clearValue.depthStencil = depth_stencil_clear_value;
   }

   terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_CB_COLOR_RTV);
   memset(state->cb_color_rtv.attachments, 0, sizeof(state->cb_color_rtv.attachments));
   for (uint32_t color_attachment_index = 0;
        color_attachment_index < pRenderingInfo->colorAttachmentCount; ++color_attachment_index) {
      VkRenderingAttachmentInfo const * const color_attachment =
         &pRenderingInfo->pColorAttachments[color_attachment_index];

      struct terakan_image_view const * const color_view =
         terakan_image_view_from_handle(color_attachment->imageView);
      if (color_view == NULL ||
          G_028C70_FORMAT(color_view->color.info) == TERASCALE_FORMAT_INDEX_INVALID) {
         continue;
      }

      struct terakan_state_draw_cb_color * const cb_color =
         &state->cb_color_rtv.attachments[color_attachment_index];
      cb_color->bo = color_view->bo;
      memcpy(&cb_color->color, &color_view->color, sizeof(struct terakan_color_descriptor));
      terakan_color_descriptor_image_view_to_color_attachment(&cb_color->color);
      memcpy(&cb_color->meta, &color_view->color_meta,
             sizeof(struct terakan_color_meta_descriptor));

      if (color_attachment->loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) {
         VkClearAttachment * const clear_color_attachment =
            &clear_attachments[clear_attachment_count++];
         clear_color_attachment->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
         clear_color_attachment->colorAttachment = color_attachment_index;
         clear_color_attachment->clearValue = color_attachment->clearValue;
      }
   }

   if (!(pRenderingInfo->flags & VK_RENDERING_RESUMING_BIT) && clear_attachment_count != 0) {
      VkClearRect const clear_rect = {
         .rect = pRenderingInfo->renderArea,
         .baseArrayLayer = 0,
         .layerCount = pRenderingInfo->layerCount,
      };
      terakan_CmdClearAttachments(commandBuffer, clear_attachment_count, clear_attachments, 1,
                                  &clear_rect);
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdEndRendering(UNUSED VkCommandBuffer const commandBuffer)
{
   /* TODO(Triang3l): Resolve the attachments. */
}
