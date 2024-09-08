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

#include "terakan_barrier.h"
#include "terakan_command_buffer.h"
#include "terakan_cp_dma.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_physical_device.h"

#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600d_common.h"
#include "util/macros.h"
#include "vk_synchronization.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Apply aliases and filter out access not performed by the involved stages. */
static void
terakan_barrier_filter_access(VkAccessFlags2 * const access, VkPipelineStageFlags2 const stages)
{
   VkPipelineStageFlags2 const all_read_access = vk_read_access2_for_pipeline_stage_flags2(stages);
   VkPipelineStageFlags2 const all_write_access =
      vk_write_access2_for_pipeline_stage_flags2(stages);
   if (*access & VK_ACCESS_2_MEMORY_READ_BIT) {
      *access |= all_read_access;
   }
   if (*access & VK_ACCESS_2_MEMORY_WRITE_BIT) {
      *access |= all_write_access;
   }

   if (*access & VK_ACCESS_2_SHADER_READ_BIT) {
      *access |= VK_ACCESS_2_SHADER_SAMPLED_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
   }
   if (*access & VK_ACCESS_2_SHADER_WRITE_BIT) {
      *access |= VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
   }

   *access &= all_read_access | all_write_access;
}

/* Note that barriers must be transitive, and optimizations breaking that must not be performed.
 * For instance, in a chain like:
 * SHADER_STORAGE_WRITE > SHADER_SAMPLED_READ > VERTEX_ATTRIBUTE_READ
 * the second barrier must not be ignored even though it's read > read, because otherwise the writes
 * made available by CB would've only been made visible to TC, but not to VC.
 */

static enum terakan_barrier_action_flags
terakan_barrier_get_src_actions(struct terakan_gfx_command_writer const * const command_writer,
                                VkPipelineStageFlags2 src_stages, VkAccessFlags2 src_access,
                                bool const for_buffer, VkImageAspectFlags const image_aspects)
{
   enum terakan_barrier_action_flags actions = 0;

   src_stages = vk_expand_src_stage_flags2(src_stages);
   terakan_barrier_filter_access(&src_access, src_stages);

   if (src_access & (VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_2_INDEX_READ_BIT |
                     VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT)) {
      actions |= TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_VS;
   }

   if (src_access & (VK_ACCESS_2_UNIFORM_READ_BIT | VK_ACCESS_2_SHADER_SAMPLED_READ_BIT |
                     VK_ACCESS_2_SHADER_STORAGE_READ_BIT)) {
      if (src_stages & (VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
                        VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT |
                        VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT |
                        VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT)) {
         actions |= TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_VS;
      }
      if (src_stages & VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT) {
         actions |= TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_PS;
      }
      if (src_stages & VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT) {
         actions |= TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CS;
      }
   }

   if (src_access & VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT) {
      actions |= TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_PS;
   }

   if (src_access &
       (VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT)) {
      actions |= TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_DATA |
                 TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_META;
   }

   if (src_access & (VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                     VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) {
      actions |=
         TERAKAN_BARRIER_ACTION_FLUSH_INV_DB_DATA | TERAKAN_BARRIER_ACTION_FLUSH_INV_DB_META;
   }

   if (src_access & VK_ACCESS_2_TRANSFER_READ_BIT) {
      actions |= TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_PS;
      if (src_stages & VK_PIPELINE_STAGE_2_COPY_BIT) {
         actions |= TERAKAN_BARRIER_ACTION_SYNC_ME_TO_CP_DMA;
      }
   }

   if (src_access & VK_ACCESS_2_TRANSFER_WRITE_BIT) {
      bool const for_color_image =
         (image_aspects & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_PLANE_0_BIT |
                           VK_IMAGE_ASPECT_PLANE_1_BIT | VK_IMAGE_ASPECT_PLANE_2_BIT)) != 0;
      bool const for_depth_stencil_image =
         (image_aspects & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) != 0;
      if (src_stages & VK_PIPELINE_STAGE_2_COPY_BIT) {
         if (for_buffer) {
            actions |= command_writer->post_buffer_copy_write_barrier_actions;
         }
         if (for_color_image) {
            actions |= command_writer->post_color_image_copy_write_barrier_actions;
         }
         if (for_depth_stencil_image) {
            actions |= command_writer->post_depth_stencil_image_copy_write_barrier_actions;
         }
      }
      /* TODO(Triang3l): Resolve actions. */
      if (src_stages & VK_PIPELINE_STAGE_2_BLIT_BIT) {
         if (for_color_image) {
            actions |= TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_DATA |
                       TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_META;
         }
         /* TODO(Triang3l): Depth/stencil blit actions. */
      }
      /* TODO(Triang3l): Actions for clears. */
   }

   if (src_access & (VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT)) {
      struct terakan_device const * const device =
         terakan_gfx_command_writer_device(command_writer);
      VkPipelineStageFlags2 const src_uav_stages =
         src_stages & ((device->vk.enabled_features.fragmentStoresAndAtomics
                           ? VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT
                           : 0) |
                       VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
      if (src_uav_stages) {
         actions |= TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_UAV |
                    (src_uav_stages & VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT
                        ? TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_PS
                        : 0) |
                    (src_uav_stages & VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT
                        ? TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CS
                        : 0);
      }
   }

   return actions;
}

static enum terakan_barrier_action_flags
terakan_barrier_get_dst_actions(struct terakan_gfx_command_writer const * const command_writer,
                                VkPipelineStageFlags2 dst_stages, VkAccessFlags2 dst_access,
                                bool const for_buffer, VkImageAspectFlags const image_aspects)
{
   struct terakan_device const * const device = terakan_gfx_command_writer_device(command_writer);

   enum terakan_barrier_action_flags actions = 0;

   dst_stages = vk_expand_dst_stage_flags2(dst_stages);
   terakan_barrier_filter_access(&dst_access, dst_stages);

   if (dst_access & (VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_2_INDEX_READ_BIT)) {
      actions |= TERAKAN_BARRIER_ACTION_SYNC_PFP_TO_ME;
   }

   /* Invalidate the texture cache.
    * INDIRECT_COMMAND_READ: Draw parameters and NumWorkgroups.
    * UNIFORM_READ: Texture cache for cases not supported by the ALU like dynamic data addressing.
    */
   if (dst_access & (VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_2_UNIFORM_READ_BIT |
                     VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_2_TRANSFER_READ_BIT |
                     VK_ACCESS_2_SHADER_SAMPLED_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT)) {
      actions |= TERAKAN_BARRIER_ACTION_INV_TC;
   }

   if (dst_access & VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT) {
      actions |= terakan_device_physical_device(device)->chip_family_info.has_vertex_cache
                    ? TERAKAN_BARRIER_ACTION_INV_VC
                    : TERAKAN_BARRIER_ACTION_INV_TC;
   }

   if (dst_access & VK_ACCESS_2_UNIFORM_READ_BIT) {
      actions |= TERAKAN_BARRIER_ACTION_INV_SH;
   }

   if (dst_access &
       (VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT)) {
      actions |= TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_DATA |
                 TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_META;
   }

   if (dst_access & (VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                     VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) {
      actions |=
         TERAKAN_BARRIER_ACTION_FLUSH_INV_DB_DATA | TERAKAN_BARRIER_ACTION_FLUSH_INV_DB_META;
   }

   if (dst_access & VK_ACCESS_2_TRANSFER_WRITE_BIT) {
      bool const for_color_image =
         (image_aspects & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_PLANE_0_BIT |
                           VK_IMAGE_ASPECT_PLANE_1_BIT | VK_IMAGE_ASPECT_PLANE_2_BIT)) != 0;
      if (for_buffer) {
         actions |= TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_UAV;
      }
      if (for_color_image) {
         actions |= TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_DATA |
                    TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_META;
         if (dst_stages & (VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_CLEAR_BIT)) {
            /* Formats with non-power-of-two bytes per block don't support RTV usage and may use a
             * buffer UAV instead.
             */
            actions |= TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_UAV;
         }
      }
      /* TODO(Triang3l): Depth/stencil actions. */
   }

   if ((dst_access &
        (VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT)) &&
       (dst_stages & ((device->vk.enabled_features.fragmentStoresAndAtomics
                          ? VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT
                          : 0) |
                      VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT))) {
      actions |= TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_UAV;
   }

   return actions;
}

static void
terakan_barrier_emit_event_write(struct terakan_gfx_command_writer * const command_writer,
                                 uint32_t event)
{
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_EVENT_WRITE, 0, 0);
   *packet++ = event;
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

void
terakan_barrier_emit_pending_actions(struct terakan_gfx_command_writer * const command_writer)
{
   enum terakan_barrier_action_flags actions = command_writer->pending_barrier_actions;
   /* Nonzero mostly outside render passes, skip lots of checks if there's nothing to do. */
   if (actions == 0) {
      return;
   }
   if (actions & TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_PS) {
      /* Clear VS_PARTIAL_FLUSH in post-transfer barrier actions too as it's included in
       * PS_PARTIAL_FLUSH.
       */
      actions |= TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_VS;
   }
   command_writer->pending_barrier_actions = 0;
   command_writer->post_buffer_copy_write_barrier_actions &= ~actions;
   command_writer->post_color_image_copy_write_barrier_actions &= ~actions;
   command_writer->post_depth_stencil_image_copy_write_barrier_actions &= ~actions;

   uint32_t cp_coher_cntl_cb_db_dest_base_ena = 0;
   uint32_t cp_coher_cntl = 0;

   /* Flushes are performed in bottom-to-top-of-pipe order, so preceding flushes are likely to be
    * able to make subsequent ones not have to wait.
    */

   /* Wait for graphics writes to be flushed to prevent read/write-after-write hazards. */

   if (actions &
       (TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_UAV | TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_DATA |
        TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_META)) {
      cp_coher_cntl_cb_db_dest_base_ena |=
         S_0085F0_CB0_DEST_BASE_ENA(1) | S_0085F0_CB1_DEST_BASE_ENA(1) |
         S_0085F0_CB2_DEST_BASE_ENA(1) | S_0085F0_CB3_DEST_BASE_ENA(1) |
         S_0085F0_CB4_DEST_BASE_ENA(1) | S_0085F0_CB5_DEST_BASE_ENA(1) |
         S_0085F0_CB6_DEST_BASE_ENA(1) | S_0085F0_CB7_DEST_BASE_ENA(1);
      cp_coher_cntl |= S_0085F0_CB_ACTION_ENA(1);
      if (actions & TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_UAV) {
         cp_coher_cntl_cb_db_dest_base_ena |=
            S_0085F0_CB8_DEST_BASE_ENA(1) | S_0085F0_CB9_DEST_BASE_ENA(1) |
            S_0085F0_CB10_DEST_BASE_ENA(1) | S_0085F0_CB11_DEST_BASE_ENA(1);
         cp_coher_cntl |= S_0085F0_SMX_ACTION_ENA(1);
         /* Flushes and invalidates RTV and UAV data, but not meta. */
         terakan_gfx_command_writer_emit_event_write_eop_discarding_data(
            command_writer, EVENT_TYPE(EVENT_TYPE_FLUSH_AND_INV_CB_DATA_TS) | EVENT_INDEX(5));
      } else if (actions & TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_DATA) {
         /* Flushes and invalidates RTV data, but not meta or UAV data. */
         terakan_barrier_emit_event_write(
            command_writer, EVENT_TYPE(EVENT_TYPE_FLUSH_AND_INV_CB_PIXEL_DATA) | EVENT_INDEX(0));
      }
      if (actions & TERAKAN_BARRIER_ACTION_FLUSH_INV_CB_RTV_META) {
         terakan_barrier_emit_event_write(
            command_writer, EVENT_TYPE(EVENT_TYPE_FLUSH_AND_INV_CB_META) | EVENT_INDEX(0));
      }
   }

   if (actions &
       (TERAKAN_BARRIER_ACTION_FLUSH_INV_DB_DATA | TERAKAN_BARRIER_ACTION_FLUSH_INV_DB_META)) {
      cp_coher_cntl_cb_db_dest_base_ena |= S_0085F0_DB_DEST_BASE_ENA(1);
      cp_coher_cntl |= S_0085F0_DB_ACTION_ENA(1);
      if (!(~actions & (TERAKAN_BARRIER_ACTION_FLUSH_INV_DB_DATA |
                        TERAKAN_BARRIER_ACTION_FLUSH_INV_DB_META))) {
         terakan_barrier_emit_event_write(
            command_writer, EVENT_TYPE(EVENT_TYPE_DB_CACHE_FLUSH_AND_INV) | EVENT_INDEX(0));
      } else {
         if (actions & TERAKAN_BARRIER_ACTION_FLUSH_INV_DB_DATA) {
            terakan_gfx_command_writer_emit_event_write_eop_discarding_data(
               command_writer, EVENT_TYPE(EVENT_TYPE_FLUSH_AND_INV_DB_DATA_TS) | EVENT_INDEX(5));
         }
         if (actions & TERAKAN_BARRIER_ACTION_FLUSH_INV_DB_META) {
            terakan_barrier_emit_event_write(
               command_writer, EVENT_TYPE(EVENT_TYPE_FLUSH_AND_INV_DB_META) | EVENT_INDEX(0));
         }
      }
   }

   /* Wait for graphics stages that perform reads to become idle before invalidating their read
    * caches via SURFACE_SYNC and launching new work to prevent write-after-read hazards.
    *
    * SURFACE_SYNC with any CB/DB_DEST_BASE_ENA implies PS_PARTIAL_FLUSH.
    */

   if (!cp_coher_cntl_cb_db_dest_base_ena) {
      if (actions & TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_PS) {
         terakan_barrier_emit_event_write(command_writer,
                                          EVENT_TYPE(EVENT_TYPE_PS_PARTIAL_FLUSH) | EVENT_INDEX(4));
      } else if (actions & TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CP_THROUGH_VS) {
         terakan_barrier_emit_event_write(command_writer,
                                          EVENT_TYPE(EVENT_TYPE_VS_PARTIAL_FLUSH) | EVENT_INDEX(4));
      }
   }

   if (actions & TERAKAN_BARRIER_ACTION_PARTIAL_FLUSH_CS) {
      terakan_barrier_emit_event_write(command_writer,
                                       EVENT_TYPE(EVENT_TYPE_CS_PARTIAL_FLUSH) | EVENT_INDEX(4));
   }

   /* Wait for CP DMA writes and reads to complete in ME. */

   if (actions & TERAKAN_BARRIER_ACTION_SYNC_ME_TO_CP_DMA) {
      terakan_cp_dma_sync_cp_me(command_writer);
   }

   /* Specify which read caches to invalidate. */

   if (actions & TERAKAN_BARRIER_ACTION_INV_TC) {
      cp_coher_cntl |= S_0085F0_TC_ACTION_ENA(1);
   }

   if (actions & TERAKAN_BARRIER_ACTION_INV_VC) {
      cp_coher_cntl |= S_0085F0_VC_ACTION_ENA(1);
   }

   if (actions & TERAKAN_BARRIER_ACTION_INV_SH) {
      cp_coher_cntl |= S_0085F0_SH_ACTION_ENA(1);
   }

   /* Perform implicit stage waits and cache flushes and invalidations in ME. */

   cp_coher_cntl |= cp_coher_cntl_cb_db_dest_base_ena;
   if (cp_coher_cntl) {
      uint32_t * surface_sync_packet = terakan_gfx_command_writer_emit(command_writer, 5, false);
      if (unlikely(surface_sync_packet == NULL)) {
         return;
      }
      *surface_sync_packet++ = PKT3(PKT3_SURFACE_SYNC, 4 - 1, 0);
      *surface_sync_packet++ = cp_coher_cntl | ((uint32_t)1 << 31);
      *surface_sync_packet++ = UINT32_MAX; /* CP_COHER_SIZE */
      *surface_sync_packet++ = 0;          /* CP_COHER_BASE */
      *surface_sync_packet++ = 10;         /* POLL_INTERVAL */
      terakan_gfx_command_writer_emit_done(command_writer, surface_sync_packet);
   }

   /* Wait until all synchronization in ME is done before starting new PFP reads if PFP is in the
    * second synchronization scope.
    */

   if (actions & TERAKAN_BARRIER_ACTION_SYNC_PFP_TO_ME) {
      uint32_t * pfp_sync_me_packet = terakan_gfx_command_writer_emit(command_writer, 2, false);
      if (unlikely(pfp_sync_me_packet == NULL)) {
         return;
      }
      *pfp_sync_me_packet++ = PKT3(PKT3_PFP_SYNC_ME, 0, 0);
      *pfp_sync_me_packet++ = 0;
      terakan_gfx_command_writer_emit_done(command_writer, pfp_sync_me_packet);
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdPipelineBarrier2(VkCommandBuffer const commandBuffer,
                            VkDependencyInfo const * const pDependencyInfo)
{
   struct terakan_gfx_command_writer * const command_writer =
      terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx;

   VkImageAspectFlags const global_barrier_image_aspects =
      VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT |
      VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT | VK_IMAGE_ASPECT_PLANE_2_BIT;
   for (uint32_t barrier_index = 0; barrier_index < pDependencyInfo->memoryBarrierCount;
        ++barrier_index) {
      VkMemoryBarrier2 const * const barrier = &pDependencyInfo->pMemoryBarriers[barrier_index];
      command_writer->pending_barrier_actions |=
         terakan_barrier_get_src_actions(command_writer, barrier->srcStageMask,
                                         barrier->srcAccessMask, true,
                                         global_barrier_image_aspects) |
         terakan_barrier_get_dst_actions(command_writer, barrier->dstStageMask,
                                         barrier->dstAccessMask, true,
                                         global_barrier_image_aspects);
   }

   for (uint32_t barrier_index = 0; barrier_index < pDependencyInfo->bufferMemoryBarrierCount;
        ++barrier_index) {
      VkBufferMemoryBarrier2 const * const barrier =
         &pDependencyInfo->pBufferMemoryBarriers[barrier_index];
      command_writer->pending_barrier_actions |=
         terakan_barrier_get_src_actions(command_writer, barrier->srcStageMask,
                                         barrier->srcAccessMask, true, VK_IMAGE_ASPECT_NONE) |
         terakan_barrier_get_dst_actions(command_writer, barrier->dstStageMask,
                                         barrier->dstAccessMask, true, VK_IMAGE_ASPECT_NONE);
   }

   for (uint32_t barrier_index = 0; barrier_index < pDependencyInfo->imageMemoryBarrierCount;
        ++barrier_index) {
      VkImageMemoryBarrier2 const * const barrier =
         &pDependencyInfo->pImageMemoryBarriers[barrier_index];
      command_writer->pending_barrier_actions |=
         terakan_barrier_get_src_actions(command_writer, barrier->srcStageMask,
                                         barrier->srcAccessMask, false,
                                         barrier->subresourceRange.aspectMask) |
         terakan_barrier_get_dst_actions(command_writer, barrier->dstStageMask,
                                         barrier->dstAccessMask, false,
                                         barrier->subresourceRange.aspectMask);
   }
}
