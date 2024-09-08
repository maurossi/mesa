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

#include "terakan_draw.h"

#include "terakan_buffer.h"
#include "terakan_command_buffer.h"
#include "terakan_entrypoints.h"
#include "terakan_hw_state.h"
#include "terakan_state.h"

#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600d_common.h"
#include "util/macros.h"
#include "util/u_endian.h"
#include "util/u_math.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

VKAPI_ATTR void VKAPI_CALL
terakan_CmdBindIndexBuffer(VkCommandBuffer const commandBuffer, VkBuffer const bufferHandle,
                           VkDeviceSize const offset, VkIndexType const indexType)
{
   struct terakan_gfx_command_writer * const command_writer =
      terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx;

   struct terakan_buffer const * const buffer = terakan_buffer_from_handle(bufferHandle);

   uint32_t vgt_index_type;
   VkDeviceSize vgt_index_buffer_size = buffer->vk.size - offset;
   if (indexType == VK_INDEX_TYPE_UINT32) {
#if UTIL_ARCH_BIG_ENDIAN
      vgt_index_type = VGT_INDEX_32 | VGT_DMA_SWAP_32_BIT;
#else
      vgt_index_type = VGT_INDEX_32;
#endif
      vgt_index_buffer_size /= sizeof(uint32_t);
   } else {
      assert(indexType == VK_INDEX_TYPE_UINT16);
#if UTIL_ARCH_BIG_ENDIAN
      vgt_index_type = VGT_INDEX_16 | VGT_DMA_SWAP_16_BIT;
#else
      vgt_index_type = VGT_INDEX_16;
#endif
      vgt_index_buffer_size /= sizeof(uint16_t);
   }
   vgt_index_buffer_size = MIN2(vgt_index_buffer_size, UINT32_MAX);

   command_writer->state_draw.vgt_index_type = vgt_index_type;
   terakan_state_draw_set_pending(&command_writer->state_draw, TERAKAN_STATE_DRAW_VGT_INDEX_TYPE);

   /* The index buffer is not needed by internal draws, modify hw_state_draw directly. */
   uint64_t const vgt_index_buffer_base = (buffer->bo_offset + offset) / sizeof(uint16_t);
   bool const vgt_index_buffer_modified =
      command_writer->hw_state_draw.vgt_index_buffer.bo != buffer->bo ||
      command_writer->hw_state_draw.vgt_index_buffer.base != vgt_index_buffer_base ||
      command_writer->hw_state_draw.vgt_index_buffer.size != vgt_index_buffer_size;
   command_writer->hw_state_draw.vgt_index_buffer.bo = buffer->bo;
   command_writer->hw_state_draw.vgt_index_buffer.base = vgt_index_buffer_base;
   command_writer->hw_state_draw.vgt_index_buffer.size = vgt_index_buffer_size;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_VGT_INDEX_TYPE, vgt_index_buffer_modified);
}

static void
terakan_set_vertex_instance_offsets(struct terakan_gfx_command_writer * const command_writer,
                                    uint32_t const vertex_offset, uint32_t const instance_offset)
{
   command_writer->state_draw.vgt_index_offset = vertex_offset;
   terakan_state_draw_set_pending(&command_writer->state_draw, TERAKAN_STATE_DRAW_VGT_INDEX_OFFSET);

   /* The instance offset is not needed by internal draws, modify hw_state_draw directly. */
   bool const sq_vtx_start_inst_loc_modified =
      command_writer->hw_state_draw.sq_vtx_start_inst_loc != instance_offset;
   command_writer->hw_state_draw.sq_vtx_start_inst_loc = instance_offset;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_SQ_VTX_START_INST_LOC,
                                 sq_vtx_start_inst_loc_modified);
}

void
terakan_before_hw_draw(struct terakan_gfx_command_writer * const command_writer)
{
   terakan_hw_state_draw_emit_modified(command_writer);
}

static void
terakan_before_draw(struct terakan_gfx_command_writer * const command_writer)
{
   terakan_state_draw_apply_pending(command_writer);

   terakan_before_hw_draw(command_writer);
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdDraw(VkCommandBuffer const commandBuffer, uint32_t const vertexCount,
                uint32_t const instanceCount, uint32_t const firstVertex,
                uint32_t const firstInstance)
{
   if (unlikely(instanceCount == 0)) {
      /* VGT_NUM_INSTANCES 0 is interpreted as 1. */
      return;
   }

   struct terakan_gfx_command_writer * const command_writer =
      terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx;

   terakan_set_vertex_instance_offsets(command_writer, firstVertex, firstInstance);

   terakan_before_draw(command_writer);

   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 3, 0, 0, false);
   if (unlikely(packet == NULL)) {
      return;
   }

   /* NUM_INSTANCES in the same indirect buffer as the draw. */
   *packet++ = PKT3(PKT3_NUM_INSTANCES, 0, 0);
   *packet++ = instanceCount;

   *packet++ = PKT3(PKT3_DRAW_INDEX_AUTO, 3 - 2, 0);
   *packet++ = vertexCount;
   *packet++ = S_0287F0_SOURCE_SELECT(V_0287F0_DI_SRC_SEL_AUTO_INDEX);
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdDrawIndexed(VkCommandBuffer const commandBuffer, uint32_t const indexCount,
                       uint32_t const instanceCount, uint32_t const firstIndex,
                       int32_t const vertexOffset, uint32_t const firstInstance)
{
   if (unlikely(instanceCount == 0)) {
      /* VGT_NUM_INSTANCES 0 is interpreted as 1. */
      return;
   }

   struct terakan_gfx_command_writer * const command_writer =
      terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx;

   terakan_set_vertex_instance_offsets(command_writer, (uint32_t)vertexOffset, firstInstance);

   terakan_before_draw(command_writer);

   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 4, 0, 0, false);
   if (unlikely(packet == NULL)) {
      return;
   }

   /* NUM_INSTANCES in the same indirect buffer as the draw. */
   *packet++ = PKT3(PKT3_NUM_INSTANCES, 0, 0);
   *packet++ = instanceCount;

   *packet++ = PKT3(EG_PKT3_DRAW_INDEX_OFFSET, 4 - 2, 0);
   *packet++ = firstIndex;
   *packet++ = indexCount;
   *packet++ = S_0287F0_SOURCE_SELECT(V_0287F0_DI_SRC_SEL_DMA);
}
