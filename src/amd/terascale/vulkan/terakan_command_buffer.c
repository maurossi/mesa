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

#include "terakan_cp_dma.h"
#include "terakan_descriptor.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_image.h"
#include "terakan_limits.h"
#include "terakan_physical_device.h"
#include "terakan_queue.h"
#include "terakan_shader.h"
#include "terakan_vertex_input.h"

#include "amd/terascale/common/terascale_wddm.h"
#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600d_common.h"
#include "util/bitscan.h"
#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void
terakan_bo_reference_writer_reset(struct terakan_bo_reference_writer * const writer,
                                  void * const bo_references, uint32_t const max_bo_reference_count)
{
   writer->references = bo_references;
   assert(max_bo_reference_count <= TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT);
   writer->max_reference_count = max_bo_reference_count;

   writer->reference_count = 0;

   BITSET_ZERO(writer->map_entries_used);
}

uint32_t
terakan_bo_reference_writer_add_reference(struct terakan_bo_reference_writer * const writer,
                                          struct terakan_bo const * const bo, bool const is_reading,
                                          bool const is_writing,
                                          enum terakan_bo_priority const priority)
{
   /* Provide two slots per hash value for quick handling of collisions by effectively doing
    * separate chaining if there are only 2 BOs per hash in this open addressing scheme (though this
    * separate-chaining-like behavior is not guaranteed if BOs with another hash value have stomped
    * on the two entries for this hash value if there were many collisions for other hash values).
    */
   uint32_t const hash = (bo->creation_number << 1) & TERAKAN_BO_REFERENCE_HASH_MASK;

   /* Search for the hash map entry.
    * Expect that more recently created BOs are more likely to be used than older ones (in a
    * scenario of an application preferring dedicated allocations - in applications with
    * suballocation from large allocations, collisions are less likely overall as the total number
    * of allocations ever created is likely to be smaller). Thus, because the hash is the lower bits
    * of the BO creation number, by going forward from the most recently created BO, collision
    * resolution will immediately end up at a hash value for a very old BO.
    */
   uint32_t reference_index = UINT32_MAX;
   uint32_t collisions;
   for (collisions = 0; collisions <= TERAKAN_BO_REFERENCE_HASH_MASK; ++collisions) {
      uint32_t const check_hash = (hash + collisions) & TERAKAN_BO_REFERENCE_HASH_MASK;
      if (!BITSET_TEST(writer->map_entries_used, check_hash)) {
         /* Didn't find an existing BO. */
         break;
      }
      uint32_t const check_reference_index = writer->map[check_hash];
      if (writer->reference_bos[check_reference_index] == bo) {
         /* Found an existing BO. */
         reference_index = check_reference_index;
         break;
      }
   }
   if (unlikely(collisions > TERAKAN_BO_REFERENCE_HASH_MASK)) {
      /* No free space in the hash map.
       * External code may assume that this will never happen if there's space for BO references
       * themselves, so normally this should happen only if the capacity of the hash map matches the
       * total maximum number of BO references, and there's already no free space in the BO
       * reference array.
       * A hash map smaller than the maximum BO reference count is pointless anyway because it would
       * effectively clamp the maximum BO reference count.
       */
      assert(writer->reference_count >= writer->max_reference_count);
      return UINT32_MAX;
   }

   /* Create the new or update the existing reference. */
   size_t const reference_size = bo->device->bo_reference_size;
   if (reference_index != UINT32_MAX) {
      bo->device->winsys_fn->queue->update_bo_reference(
         (char *)writer->references + reference_size * reference_index, bo, is_reading, is_writing,
         priority);
   } else {
      if (unlikely(writer->reference_count >= writer->max_reference_count)) {
         return UINT32_MAX;
      }
      reference_index = writer->reference_count++;
      writer->reference_bos[reference_index] = bo;
      bo->device->winsys_fn->queue->create_bo_reference(
         (char *)writer->references + reference_size * reference_index, bo, is_reading, is_writing,
         priority);
   }

   /* Insert the BO reference in the beginning of the collision chain for this hash, moving the
    * collisions forward by one (so they're ordered from the most recently used to the least
    * recently used), so when this BO is referenced again in the near future, it will be found
    * quickly.
    */
   for (uint32_t collision_index = 0; collision_index < collisions; ++collision_index) {
      writer->map[(hash + (collisions - collision_index)) & TERAKAN_BO_REFERENCE_HASH_MASK] =
         writer->map[(hash + (collisions - collision_index - 1)) & TERAKAN_BO_REFERENCE_HASH_MASK];
   }
   BITSET_SET(writer->map_entries_used, (hash + collisions) & TERAKAN_BO_REFERENCE_HASH_MASK);
   writer->map[hash] = reference_index;

   return reference_index;
}

struct terakan_queue_submission_size
terakan_command_buffer_optimal_submission_size_gfx(
   struct terakan_physical_device_submission_info const * const submission_info_gfx)
{
   /* Subtract the reserved amount from the optimal size of the application's indirect buffers
    * because it's preferable to keep the amount round, as the winsys may internally allocate its
    * submission memory with a large alignment (for instance, according to the addresses, it seems
    * that in the WDDM Radeon Software driver, memory for submissions is allocated with VirtualAlloc
    * granularity), and adding a few BO references or relocations beyond some power-of-two amount
    * may result in a lot of padding.
    */
   return terakan_queue_submission_size_subtract_saturating(
      (struct terakan_queue_submission_size){
         .bo_references = TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT,
         .indirect_buffer_dwords = TERAKAN_GFX_OPTIMAL_INDIRECT_BUFFER_SIZE_DWORDS,
         .relocations =
            submission_info_gfx->relocation_type == TERAKAN_QUEUE_RELOCATION_TYPE_WDDM_PATCH
               ? TERAKAN_GFX_OPTIMAL_WDDM_RELOCATION_COUNT
               : 0,
      },
      submission_info_gfx->submission_outer_reserved_amount);
}

#define TERAKAN_PUSH_BUFFER_SIZE_BYTES                                                             \
   ALIGN_POT(MAX2((uint32_t)1 << 16, sizeof(uint32_t) * 2 * TERAKAN_VERTEX_INPUT_FS_MAX_QWORDS),   \
             TERAKAN_KCACHE_HW_LINE_BYTES)
#define TERAKAN_PUSH_BUFFER_ALIGNMENT ((uint32_t)1 << 8)
static_assert(TERAKAN_PUSH_BUFFER_SIZE_BYTES >= TERAKAN_KCACHE_HW_MAX_BUFFER_SIZE_BYTES &&
                 TERAKAN_PUSH_BUFFER_ALIGNMENT >= TERAKAN_KCACHE_HW_LINE_BYTES,
              "Push buffers must be usable for kcache buffers.");
static_assert(TERAKAN_PUSH_BUFFER_SIZE_BYTES >= ((uint32_t)1 << 16) &&
                 TERAKAN_PUSH_BUFFER_ALIGNMENT >= TERAKAN_CP_DMA_COPY_OPTIMAL_ALIGNMENT,
              "Push buffers must be usable for vkCmdUpdateBuffer done via CP DMA.");
static_assert(TERAKAN_PUSH_BUFFER_SIZE_BYTES >=
                    sizeof(uint32_t) * 2 * TERAKAN_VERTEX_INPUT_FS_MAX_QWORDS &&
                 TERAKAN_PUSH_BUFFER_ALIGNMENT >= TERAKAN_SHADER_PROGRAM_ALIGNMENT,
              "Push buffers must be usable for dynamic vertex fetch shaders.");
static_assert(TERAKAN_PUSH_BUFFER_SIZE_BYTES >= sizeof(uint32_t) * 3 &&
                 TERAKAN_PUSH_BUFFER_ALIGNMENT >= sizeof(uint32_t),
              "Push buffers must be usable for direct draw and dispatch parameters.");
static_assert(
   TERAKAN_PUSH_BUFFER_SIZE_BYTES >= sizeof(uint32_t) * 4 &&
      TERAKAN_PUSH_BUFFER_ALIGNMENT >= sizeof(uint32_t),
   "Push buffers must be usable for at least one rectangle with 16.16 coordinates packed in vertex "
   "indices.");

void *
terakan_push_buffer_allocate(struct terakan_command_buffer * const command_buffer,
                             uint32_t const size_bytes, uint32_t const alignment_bytes,
                             struct terakan_bo const ** const bo_out, uint64_t * const va_out)
{
   assert(util_is_power_of_two_nonzero(alignment_bytes));
   assert(alignment_bytes <= TERAKAN_PUSH_BUFFER_ALIGNMENT);

   assert(size_bytes <= TERAKAN_PUSH_BUFFER_SIZE_BYTES);
   if (unlikely(size_bytes > TERAKAN_PUSH_BUFFER_SIZE_BYTES)) {
      vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_UNKNOWN);
      return NULL;
   }

   if (!list_is_empty(&command_buffer->push_buffers)) {
      uint32_t const existing_buffer_offset_bytes =
         ALIGN_POT(command_buffer->current_push_buffer_used_bytes, alignment_bytes);
      uint32_t const existing_buffer_new_used_bytes = existing_buffer_offset_bytes + size_bytes;
      if (existing_buffer_new_used_bytes <= TERAKAN_PUSH_BUFFER_SIZE_BYTES) {
         command_buffer->current_push_buffer_used_bytes = existing_buffer_new_used_bytes;
         struct terakan_push_buffer * const existing_buffer =
            list_first_entry(&command_buffer->push_buffers, struct terakan_push_buffer, link);
         *bo_out = existing_buffer->bo;
         *va_out = existing_buffer->bo->va + existing_buffer_offset_bytes;
         return (char *)existing_buffer->bo->mapping + existing_buffer_offset_bytes;
      }
   }

   struct terakan_push_buffer * new_buffer;

   struct terakan_command_pool * const command_pool =
      container_of(command_buffer->vk.pool, struct terakan_command_pool, vk);
   if (!list_is_empty(&command_pool->push_buffers_free)) {
      new_buffer =
         list_first_entry(&command_pool->push_buffers_free, struct terakan_push_buffer, link);
      list_del(&new_buffer->link);
   } else {
      new_buffer = vk_alloc(&command_pool->vk.alloc, sizeof(struct terakan_push_buffer),
                            alignof(struct terakan_push_buffer), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
      if (new_buffer == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
         return NULL;
      }

      struct terakan_device * const device = terakan_command_buffer_device(command_buffer);

      VkResult const bo_allocate_result = device->winsys_fn->bo->allocate_device_memory(
         device, TERAKAN_PUSH_BUFFER_SIZE_BYTES, TERAKAN_PUSH_BUFFER_ALIGNMENT,
         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
         0, &command_pool->vk.alloc, VK_SYSTEM_ALLOCATION_SCOPE_OBJECT, &new_buffer->bo);
      if (bo_allocate_result != VK_SUCCESS) {
         vk_command_buffer_set_error(&command_buffer->vk, bo_allocate_result);
         vk_free(&command_pool->vk.alloc, new_buffer);
         return NULL;
      }

      if (terakan_bo_map(new_buffer->bo) == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
         terakan_bo_free(new_buffer->bo, &command_pool->vk.alloc);
         vk_free(&command_pool->vk.alloc, new_buffer);
         return NULL;
      }
   }

   list_add(&new_buffer->link, &command_buffer->push_buffers);
   command_buffer->current_push_buffer_used_bytes = size_bytes;
   *bo_out = new_buffer->bo;
   *va_out = new_buffer->bo->va;
   return new_buffer->bo->mapping;
}

static struct terakan_command_buffer_indirect_buffer *
terakan_command_buffer_new_indirect_buffer(struct terakan_command_buffer * const command_buffer)
{
   if (vk_command_buffer_has_error(&command_buffer->vk)) {
      return NULL;
   }

   struct terakan_command_pool * const command_pool =
      container_of(command_buffer->vk.pool, struct terakan_command_pool, vk);

   struct terakan_command_buffer_indirect_buffer * indirect_buffer;
   if (!list_is_empty(&command_pool->indirect_buffers_free)) {
      indirect_buffer = list_first_entry(&command_pool->indirect_buffers_free,
                                         struct terakan_command_buffer_indirect_buffer, link);
      list_del(&indirect_buffer->link);
   } else {
      indirect_buffer = vk_alloc(
         &command_pool->vk.alloc, sizeof(struct terakan_command_buffer_indirect_buffer),
         alignof(struct terakan_command_buffer_indirect_buffer), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
      if (indirect_buffer == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
         return NULL;
      }

      struct terakan_device const * const device = terakan_command_buffer_device(command_buffer);

      indirect_buffer->bo_references = vk_alloc(
         &command_pool->vk.alloc,
         device->bo_reference_size * device->command_buffer_submission_size_gfx.bo_references,
         device->bo_reference_alignment, VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
      if (indirect_buffer->bo_references == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
         vk_free(&command_pool->vk.alloc, indirect_buffer);
         return NULL;
      }

      indirect_buffer->indirect_buffer = vk_alloc(
         &command_pool->vk.alloc,
         sizeof(uint32_t) * device->command_buffer_submission_size_gfx.indirect_buffer_dwords,
         alignof(uint32_t), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
      if (indirect_buffer->indirect_buffer == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
         vk_free(&command_pool->vk.alloc, indirect_buffer->bo_references);
         vk_free(&command_pool->vk.alloc, indirect_buffer);
         return NULL;
      }

      indirect_buffer->relocations = NULL;
      if (device->command_buffer_submission_size_gfx.relocations != 0) {
         indirect_buffer->relocations = vk_alloc(
            &command_pool->vk.alloc,
            sizeof(struct terakan_queue_relocation_wddm_patch) *
               device->command_buffer_submission_size_gfx.relocations,
            alignof(struct terakan_queue_relocation_wddm_patch), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
         if (indirect_buffer->relocations == NULL) {
            vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
            vk_free(&command_pool->vk.alloc, indirect_buffer->indirect_buffer);
            vk_free(&command_pool->vk.alloc, indirect_buffer->bo_references);
            vk_free(&command_pool->vk.alloc, indirect_buffer);
            return NULL;
         }
      }
   }

   indirect_buffer->bo_reference_count = 0;
   indirect_buffer->indirect_buffer_size_dwords = 0;
   indirect_buffer->relocation_count = 0;

   list_addtail(&indirect_buffer->link, &command_buffer->indirect_buffers);

   return indirect_buffer;
}

static void
terakan_command_buffer_release_resources(struct terakan_command_buffer * const command_buffer)
{
   struct terakan_command_pool * const command_pool =
      container_of(command_buffer->vk.pool, struct terakan_command_pool, vk);

   if (command_buffer->command_writer.gfx != NULL) {
      list_add(&command_buffer->command_writer.gfx->base.free_link,
               &command_pool->command_writers_free);
      command_buffer->command_writer.gfx = NULL;
   }

   list_for_each_entry_safe (struct terakan_command_buffer_indirect_buffer, indirect_buffer,
                             &command_buffer->indirect_buffers, link) {
      list_move_to(&indirect_buffer->link, &command_pool->indirect_buffers_free);
   }

   list_splice(&command_buffer->push_buffers, &command_pool->push_buffers_free);
   list_inithead(&command_buffer->push_buffers);
}

static void
terakan_command_buffer_reset(struct vk_command_buffer * const command_buffer_base,
                             UNUSED VkCommandBufferResetFlags const flags)
{
   struct terakan_command_buffer * command_buffer =
      container_of(command_buffer_base, struct terakan_command_buffer, vk);

   vk_command_buffer_reset(&command_buffer->vk);

   terakan_command_buffer_release_resources(command_buffer);
}

static void
terakan_command_buffer_destroy(struct vk_command_buffer * const command_buffer_base)
{
   struct terakan_command_buffer * command_buffer =
      container_of(command_buffer_base, struct terakan_command_buffer, vk);

   terakan_command_buffer_release_resources(command_buffer);

   vk_command_buffer_finish(&command_buffer->vk);

   vk_free(&command_buffer->vk.pool->alloc, command_buffer);
}

static VkResult
terakan_command_buffer_create(struct vk_command_pool * const command_pool,
                              UNUSED VkCommandBufferLevel level,
                              struct vk_command_buffer ** const command_buffer_out)
{
   VkResult result;

   struct terakan_command_buffer * command_buffer =
      vk_alloc(&command_pool->alloc, sizeof(struct terakan_command_buffer),
               alignof(struct terakan_command_buffer), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (command_buffer == NULL) {
      return vk_error(command_pool->base.device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   result =
      vk_command_buffer_init(command_pool, &command_buffer->vk, &terakan_command_buffer_ops, 0);
   if (result != VK_SUCCESS) {
      vk_free(&command_pool->alloc, command_buffer);
      return result;
   }

   list_inithead(&command_buffer->push_buffers);
   command_buffer->current_push_buffer_used_bytes = TERAKAN_PUSH_BUFFER_SIZE_BYTES;

   list_inithead(&command_buffer->indirect_buffers);

   command_buffer->command_writer.gfx = NULL;

   *command_buffer_out = &command_buffer->vk;
   return VK_SUCCESS;
}

struct vk_command_buffer_ops const terakan_command_buffer_ops = {
   .create = terakan_command_buffer_create,
   .reset = terakan_command_buffer_reset,
   .destroy = terakan_command_buffer_destroy,
};

static void
terakan_gfx_command_writer_end_indirect_buffer(
   struct terakan_gfx_command_writer * const command_writer)
{
#ifndef NDEBUG
   assert(!command_writer->is_emitting &&
          "terakan_gfx_command_writer_emit_done must be called with the final append pointer after "
          "every command emission");
#endif

   if (command_writer->indirect_buffer == NULL) {
      return;
   }

   command_writer->indirect_buffer->bo_reference_count =
      command_writer->base.bo_reference_writer.reference_count;

   while (command_writer->indirect_buffer->indirect_buffer_size_dwords &
          (TERAKAN_QUEUE_INDIRECT_BUFFER_SIZE_ALIGNMENT_DWORDS_GFX - 1)) {
      command_writer->indirect_buffer
         ->indirect_buffer[command_writer->indirect_buffer->indirect_buffer_size_dwords++] =
         PKT_TYPE_S(2);
   }

   command_writer->indirect_buffer = NULL;
}

static void
terakan_gfx_command_writer_emit_preamble(struct terakan_gfx_command_writer * const command_writer)
{
   /* According the Gallium R600 driver, the order of register setting matters, and sometimes the
    * wrong order may cause incorrect behavior or GPU hangs.
    *
    * Useful references:
    * - Gallium R600 driver
    * - xf86-video-ati
    * - Radeon DRM kernel driver
    * - fglrx indirect buffers
    */

   struct terakan_physical_device const * const physical_device =
      terakan_gfx_command_writer_physical_device(command_writer);
   struct terakan_physical_device_chip_family_info const * const chip_family_info =
      &physical_device->chip_family_info;
   bool const is_r9xx = chip_family_info->is_r9xx;
   struct terakan_physical_device_submission_info_gfx const * const submission_info_gfx =
      &physical_device->submission_info_gfx;

   uint32_t * packet;

   /* Disable register shadowing before setting any registers. */
   packet = terakan_gfx_command_writer_emit(command_writer, 3, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_CONTEXT_CONTROL, 1, 0);
   *packet++ = (uint32_t)1 << 31; /* CC0_UPDATE_LOAD_ENABLES(1) */
   *packet++ = (uint32_t)1 << 31; /* CC1_UPDATE_SHADOW_ENABLES(1) */
   terakan_gfx_command_writer_emit_done(command_writer, packet);

   if (submission_info_gfx->need_sq_alu_const_mode_control) {
      /* Switch to the Direct3D 10 mode for ALU constants (provided via kcache buffers).
       * DRM Radeon 2.50.0 does this MODE_CONTROL before every indirect buffer execution and doesn't
       * allow it in submissions without virtual memory, but WDDM Radeon Software as of
       * 15.301.1901 does it in submissions after CONTEXT_CONTROL.
       */
      packet = terakan_gfx_command_writer_emit(command_writer, 2, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_MODE_CONTROL, 0, 0);
      *packet++ = 1;
      terakan_gfx_command_writer_emit_done(command_writer, packet);
   }

   /*
    * Setup graphics context registers outside terakan_hw_state_draw.
    */

   if (!is_r9xx) {
      /* Workaround for hardware issues with dynamic GPRs - must set all limits to 240 (in units of
       * 8 registers) instead of 0. */
      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028838_SQ_DYN_GPR_RESOURCE_LIMIT_1);
      /* Workaround for hardware issues with dynamic GPRs - must set all limits to 240 (in units of
       * 8 registers) instead of 0.
       */
      *packet++ = S_028838_PS_GPRS(0x1E) | S_028838_VS_GPRS(0x1E) | S_028838_GS_GPRS(0x1E) |
                  S_028838_ES_GPRS(0x1E) | S_028838_HS_GPRS(0x1E) | S_028838_LS_GPRS(0x1E);
      terakan_gfx_command_writer_emit_done(command_writer, packet);
   }

   uint32_t const draw_context_regs[] = {
      /*
       * Vertex grouper and tessellator.
       */

      PKT3(PKT3_SET_CONTEXT_REG,
           (R_028404_VGT_MIN_VTX_INDX - R_028400_VGT_MAX_VTX_INDX) / sizeof(uint32_t) + 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028400_VGT_MAX_VTX_INDX),
      /* R_028400_VGT_MAX_VTX_INDX */
      UINT32_MAX,
      /* R_028404_VGT_MIN_VTX_INDX */
      0,

      PKT3(PKT3_SET_CONTEXT_REG,
           (R_028A3C_VGT_GROUP_VECT_1_FMT_CNTL - R_028A10_VGT_OUTPUT_PATH_CNTL) / sizeof(uint32_t) +
              1,
           0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028A10_VGT_OUTPUT_PATH_CNTL),
      /* R_028A10_VGT_OUTPUT_PATH_CNTL */
      0,
      /* R_028A14_VGT_HOS_CNTL */
      0,
      /* R_028A18_VGT_HOS_MAX_TESS_LEVEL */
      (uint32_t)(6 + 127) << 23, /* 64.0f */
      /* R_028A1C_VGT_HOS_MIN_TESS_LEVEL */
      0,
      /* R_028A20_VGT_HOS_REUSE_DEPTH */
      16,
      /* R_028A24_VGT_GROUP_PRIM_TYPE */
      0,
      /* R_028A28_VGT_GROUP_FIRST_DECR */
      0,
      /* R_028A2C_VGT_GROUP_DECR */
      0,
      /* R_028A30_VGT_GROUP_VECT_0_CNTL */
      0,
      /* R_028A34_VGT_GROUP_VECT_1_CNTL */
      0,
      /* R_028A38_VGT_GROUP_VECT_0_FMT_CNTL */
      0,
      /* R_028A3C_VGT_GROUP_VECT_1_FMT_CNTL */
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028A40_VGT_GS_MODE),
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028A84_VGT_PRIMITIVEID_EN),
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028A94_VGT_MULTI_PRIM_IB_RESET_EN),
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028AB4_VGT_REUSE_OFF),
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028AB8_VGT_VTX_CNT_EN),
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028B54_VGT_SHADER_STAGES_EN),
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(
         PKT3_SET_CONTEXT_REG,
         (R_028B98_VGT_STRMOUT_BUFFER_CONFIG - R_028B94_VGT_STRMOUT_CONFIG) / sizeof(uint32_t) + 1,
         0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028B94_VGT_STRMOUT_CONFIG),
      /* R_028B94_VGT_STRMOUT_CONFIG */
      0,
      /* R_028B98_VGT_STRMOUT_BUFFER_CONFIG */
      0,

      /*
       * Sequencer.
       */

      PKT3(PKT3_SET_CONTEXT_REG,
           (R_0288EC_SQ_LDS_ALLOC_PS - R_0288E8_SQ_LDS_ALLOC) / sizeof(uint32_t) + 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_0288E8_SQ_LDS_ALLOC),
      /* R_0288E8_SQ_LDS_ALLOC */
      0,
      /* R_0288EC_SQ_LDS_ALLOC_PS */
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_0288F0_SQ_VTX_SEMANTIC_CLEAR),
      UINT32_MAX,

      /*
       * Shader export.
       */

      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028350_SX_MISC),
      0,

      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028410_SX_ALPHA_TEST_CONTROL),
      0,

      /*
       * Shader interpolator.
       */

      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_0286C8_SPI_THREAD_GROUPING),
      /* TODO(Triang3l): Gallium R600 has 0 for SPI_THREAD_GROUPING, but DRM Radeon 2.50.0 has 1 in
       * cleanstate_evergreen/cayman.h. Research which is more correct.
       */
      0,

      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_0286D4_SPI_INTERP_CONTROL_0),
      S_0286D4_FLAT_SHADE_ENA(1) | S_0286D4_PNT_SPRITE_ENA(1) |
         S_0286D4_PNT_SPRITE_OVRD_X(V_0286D4_SPI_PNT_SPRITE_SEL_S) |
         S_0286D4_PNT_SPRITE_OVRD_Y(V_0286D4_SPI_PNT_SPRITE_SEL_T) |
         S_0286D4_PNT_SPRITE_OVRD_Z(V_0286D4_SPI_PNT_SPRITE_SEL_0) |
         S_0286D4_PNT_SPRITE_OVRD_W(V_0286D4_SPI_PNT_SPRITE_SEL_1),

      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_0286DC_SPI_FOG_CNTL),
      0,

      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_0286E4_SPI_PS_IN_CONTROL_2),
      0,

      /*
       * Primitive assembly.
       */

      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028A00_PA_SU_POINT_SIZE),
      S_028A00_HEIGHT((uint32_t)1 << 3) | S_028A00_WIDTH((uint32_t)1 << 3),

      PKT3(PKT3_SET_CONTEXT_REG, 2, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028030_PA_SC_SCREEN_SCISSOR_TL),
      /* R_028030_PA_SC_SCREEN_SCISSOR_TL */
      0,
      /* R_028034_PA_SC_SCREEN_SCISSOR_BR */
      S_028034_BR_X(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT) | S_028034_BR_Y(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT),

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 2, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028204_PA_SC_WINDOW_SCISSOR_TL),
      /* R_028204_PA_SC_WINDOW_SCISSOR_TL */
      0,
      /* R_028208_PA_SC_WINDOW_SCISSOR_BR */
      S_028208_BR_X(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT) | S_028208_BR_Y(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT),

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_02820C_PA_SC_CLIPRECT_RULE),
      0xFFFF,

      PKT3(PKT3_SET_CONTEXT_REG,
           (R_028234_PA_SU_HARDWARE_SCREEN_OFFSET - R_028230_PA_SC_EDGERULE) / sizeof(uint32_t) + 1,
           0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028230_PA_SC_EDGERULE),
      /* R_028230_PA_SC_EDGERULE
       * Direct3D top-left rule, also compatible with Direct3D line rasterization diamond test.
       */
      S_028230_ER_TRI(0b1010) | S_028230_ER_POINT(0b1010) | S_028230_ER_RECT(0b1010) |
         S_028230_ER_LINE_LR(0b011010) | S_028230_ER_LINE_RL(0b100110) |
         S_028230_ER_LINE_TB(0b1010) | S_028230_ER_LINE_BT(0b1010),
      /* R_028234_PA_SU_HARDWARE_SCREEN_OFFSET */
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 2, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028240_PA_SC_GENERIC_SCISSOR_TL),
      /* R_028240_PA_SC_GENERIC_SCISSOR_TL */
      0,
      /* R_028244_PA_SC_GENERIC_SCISSOR_BR */
      S_028244_BR_X(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT) | S_028244_BR_Y(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT),

      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028820_PA_CL_NANINF_CNTL),
      0,

      PKT3(PKT3_SET_CONTEXT_REG,
           (R_028A08_PA_SU_LINE_CNTL - R_028A04_PA_SU_POINT_MINMAX) / sizeof(uint32_t) + 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028A04_PA_SU_POINT_MINMAX),
      /* R_028A04_PA_SU_POINT_MINMAX */
      S_028A04_MAX_SIZE(UINT16_MAX),
      /* R_028A08_PA_SU_LINE_CNTL */
      S_028A08_WIDTH((uint32_t)1 << 3),

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028A4C_PA_SC_MODE_CNTL_1),
      EG_S_028A4C_FORCE_EOV_CNTDWN_ENABLE(1) | EG_S_028A4C_FORCE_EOV_REZ_ENABLE(1),

      /*
       * Depth buffer.
       */

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028000_DB_RENDER_CONTROL),
      0,

      /* TODO(Triang3l): Move to hw_state_draw.
       * DECOMPRESS_Z_ON_FLUSH on R9xx must be enabled for 4x+ AA.
       */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028010_DB_RENDER_OVERRIDE2),
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028B70_DB_ALPHA_TO_MASK),
      0,
   };

   packet = terakan_gfx_command_writer_emit(command_writer, ARRAY_SIZE(draw_context_regs), false);
   if (unlikely(packet == NULL)) {
      return;
   }
   memcpy(packet, draw_context_regs, sizeof(draw_context_regs));
   packet += ARRAY_SIZE(draw_context_regs);
   terakan_gfx_command_writer_emit_done(command_writer, packet);

   if (is_r9xx) {
      /* TODO(Triang3l): Move to hw_state_draw. */
      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_028AA8_IA_MULTI_VGT_PARAM);
      *packet++ = S_028AA8_PRIMGROUP_SIZE(128 - 1);
      terakan_gfx_command_writer_emit_done(command_writer, packet);

      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_0286FC_SPI_LDS_MGMT);
      *packet++ = 0;
      terakan_gfx_command_writer_emit_done(command_writer, packet);

      /* TODO(Triang3l): Move to hw_state_draw. */
      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_028804_DB_EQAA);
      *packet++ = S_028804_HIGH_QUALITY_INTERSECTIONS(1) | S_028804_INCOHERENT_EQAA_READS(1) |
                  S_028804_STATIC_ANCHOR_ASSOCIATIONS(1);
      terakan_gfx_command_writer_emit_done(command_writer, packet);
   }

   packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ =
      TERAKAN_CONTEXT_REG_OFFSET(is_r9xx ? CM_R_028BE4_PA_SU_VTX_CNTL : R_028C08_PA_SU_VTX_CNTL);
   *packet++ = S_028C08_PIX_CENTER_HALF(1) | S_028C08_ROUND_MODE(V_028C08_X_ROUND_TO_EVEN) |
               S_028C08_QUANT_MODE(V_028C08_X_1_256TH);
   terakan_gfx_command_writer_emit_done(command_writer, packet);

   /*
    * Setup configuration registers common between graphics and compute.
    */

   uint32_t sq_config =
      S_008C00_VC_ENABLE(chip_family_info->has_vertex_cache) | S_008C00_EXPORT_SRC_C(1);
   /* Not raising CS2 priority in SQ_CONFIG on R9xx unlike in DRM Radeon 2.50.0 because it doesn't
    * expose the compute rings at all.
    */
   if (!is_r9xx) {
      sq_config |= S_008C00_LS_PRIO(3) | S_008C00_HS_PRIO(3) | S_008C00_ES_PRIO(3) |
                   S_008C00_GS_PRIO(2) | S_008C00_VS_PRIO(1) | S_008C00_PS_PRIO(0) |
                   S_008C00_CS_PRIO(0);
   }
   packet = terakan_gfx_command_writer_emit(command_writer, 2 + 2 + 2 + 2 + 2, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   /* PS_PARTIAL_FLUSH is necessary before setting SQ_CONFIG (as well as
    * SQ_DYN_GPR_CNTL_PS_FLUSH_REQ later), otherwise on WDDM Radeon Software (tested on Barts on
    * 15.301.1901), the GPU hangs randomly likely because other work is still being done by the SQ.
    * In Direct3D 11 submissions on R8xx, WAIT_UNTIL for WAIT_3D_IDLE is done instead even, but
    * PS_PARTIAL_FLUSH seems sufficient.
    */
   /* TODO(Triang3l): Is CS_PARTIAL_FLUSH needed too? */
   *packet++ = PKT3(PKT3_EVENT_WRITE, 0, 0);
   *packet++ = EVENT_TYPE(EVENT_TYPE_PS_PARTIAL_FLUSH) | EVENT_INDEX(4);
   *packet++ = PKT3(PKT3_SET_CONFIG_REG, is_r9xx ? 2 : 6, 0);
   *packet++ = TERAKAN_CONFIG_REG_OFFSET(R_008C00_SQ_CONFIG);
   /* R_008C00_SQ_CONFIG */
   *packet++ = sq_config;
   /* R_008C04_SQ_GPR_RESOURCE_MGMT_1 */
   *packet++ = S_008C04_NUM_CLAUSE_TEMP_GPRS(4);
   if (is_r9xx) {
      *packet++ = PKT3(PKT3_SET_CONFIG_REG, 2, 0);
      *packet++ = TERAKAN_CONFIG_REG_OFFSET(R_008C10_SQ_GLOBAL_GPR_RESOURCE_MGMT_1);
   } else {
      /* R_008C08_SQ_GPR_RESOURCE_MGMT_2 */
      *packet++ = 0;
      /* R_008C0C_SQ_GPR_RESOURCE_MGMT_3 */
      *packet++ = 0;
   }
   /* R_008C10_SQ_GLOBAL_GPR_RESOURCE_MGMT_1 */
   *packet++ = 0;
   /* R_008C14_SQ_GLOBAL_GPR_RESOURCE_MGMT_2 */
   *packet++ = 0;
   terakan_gfx_command_writer_emit_done(command_writer, packet);

   /* TODO(Triang3l): Dynamic GPR usage on R8xx - see evergreen_emit_config_state, and also disable
    * them for tessellation, see evergreen_adjust_gprs. Keep them always enabled for R9xx though.
    */
   packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONFIG_REG, 1, 0);
   /* PS_PARTIAL_FLUSH required for this was done earlier. */
   *packet++ = TERAKAN_CONFIG_REG_OFFSET(R_008D8C_SQ_DYN_GPR_CNTL_PS_FLUSH_REQ);
   *packet++ = S_008D8C_DYN_GPR_ENABLE(1);
   terakan_gfx_command_writer_emit_done(command_writer, packet);

   uint32_t const config_regs[] = {
      /* Remove LS and HS from one SIMD for a hardware bug workaround according to the Gallium R600
       * driver.
       */
      PKT3(PKT3_SET_CONFIG_REG, 3, 0),
      TERAKAN_CONFIG_REG_OFFSET(R_008E20_SQ_STATIC_THREAD_MGMT1),
      /* R_008E20_SQ_STATIC_THREAD_MGMT1 */
      ~(uint32_t)0,
      /* R_008E24_SQ_STATIC_THREAD_MGMT2 */
      ~(uint32_t)0,
      /* R_008E28_SQ_STATIC_THREAD_MGMT3 */
      ~(uint32_t)1,

      PKT3(PKT3_SET_CONFIG_REG, 1, 0),
      TERAKAN_CONFIG_REG_OFFSET(R_009100_SPI_CONFIG_CNTL),
      0,

      PKT3(PKT3_SET_CONFIG_REG, 1, 0),
      TERAKAN_CONFIG_REG_OFFSET(R_00913C_SPI_CONFIG_CNTL_1),
      S_00913C_VTX_DONE_DELAY(1),

      PKT3(PKT3_SET_CONFIG_REG, 1, 0),
      TERAKAN_CONFIG_REG_OFFSET(R_008A14_PA_CL_ENHANCE),
      S_008A14_CLIP_VTX_REORDER_ENA(1) | S_008A14_NUM_CLIP_SEQ(3),
   };

   packet = terakan_gfx_command_writer_emit(command_writer, ARRAY_SIZE(config_regs), false);
   if (unlikely(packet == NULL)) {
      return;
   }
   memcpy(packet, config_regs, sizeof(config_regs));
   packet += ARRAY_SIZE(config_regs);
   terakan_gfx_command_writer_emit_done(command_writer, packet);

   /*
    * Setup configuration registers for graphics.
    */

   /* TODO(Triang3l): Emit the values for graphics or compute when switching between the two. */

   if (!is_r9xx) {
      /* The thread counts should be a multiple of 8 as space is allocated in blocks of 8 according
       * to the register reference.
       * DRM Radeon 2.50.0 spreads the non-pixel-shader threads evenly between 6 stages, but aside
       * from the pixel shader there are 5 stages - allocate more.
       */
      uint32_t const sq_vertex_threads =
         (chip_family_info->sq_max_threads - chip_family_info->sq_ps_threads_r8xx) / 5 / 8 * 8;

      uint32_t const sq_stage_stack_entries = chip_family_info->sq_max_stack_entries / 6;

      uint32_t const sq_thread_stack_register_count =
         (R_008C28_SQ_STACK_RESOURCE_MGMT_3 - R_008C18_SQ_THREAD_RESOURCE_MGMT_1) /
            sizeof(uint32_t) +
         1;
      packet = terakan_gfx_command_writer_emit(command_writer,
                                               2 + sq_thread_stack_register_count + 2 + 1, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONFIG_REG, sq_thread_stack_register_count, 0);
      *packet++ = TERAKAN_CONFIG_REG_OFFSET(R_008C18_SQ_THREAD_RESOURCE_MGMT_1);
      /* R_008C18_SQ_THREAD_RESOURCE_MGMT_1 */
      *packet++ = S_008C18_NUM_PS_THREADS(chip_family_info->sq_ps_threads_r8xx) |
                  S_008C18_NUM_VS_THREADS(sq_vertex_threads) |
                  S_008C18_NUM_GS_THREADS(sq_vertex_threads) |
                  S_008C18_NUM_ES_THREADS(sq_vertex_threads);
      /* R_008C1C_SQ_THREAD_RESOURCE_MGMT_2 */
      *packet++ =
         S_008C1C_NUM_HS_THREADS(sq_vertex_threads) | S_008C1C_NUM_LS_THREADS(sq_vertex_threads);
      /* R_008C20_SQ_STACK_RESOURCE_MGMT_1 */
      *packet++ = S_008C20_NUM_PS_STACK_ENTRIES(sq_stage_stack_entries) |
                  S_008C20_NUM_VS_STACK_ENTRIES(sq_stage_stack_entries);
      /* R_008C24_SQ_STACK_RESOURCE_MGMT_2 */
      *packet++ = S_008C24_NUM_GS_STACK_ENTRIES(sq_stage_stack_entries) |
                  S_008C24_NUM_ES_STACK_ENTRIES(sq_stage_stack_entries);
      /* R_008C28_SQ_STACK_RESOURCE_MGMT_3 */
      *packet++ = S_008C28_NUM_HS_STACK_ENTRIES(sq_stage_stack_entries) |
                  S_008C28_NUM_LS_STACK_ENTRIES(sq_stage_stack_entries);
      *packet++ = PKT3(PKT3_SET_CONFIG_REG, 1, 0);
      *packet++ = TERAKAN_CONFIG_REG_OFFSET(R_008E2C_SQ_LDS_RESOURCE_MGMT);
      *packet++ = S_008E2C_NUM_PS_LDS(TERAKAN_LIMITS_HW_LDS_SIMD_DWORD_COUNT / 2) |
                  S_008E2C_NUM_LS_LDS(TERAKAN_LIMITS_HW_LDS_SIMD_DWORD_COUNT / 2);
      terakan_gfx_command_writer_emit_done(command_writer, packet);
   }
}

static bool
terakan_gfx_command_writer_new_indirect_buffer(
   struct terakan_gfx_command_writer * const command_writer, bool * const all_state_emitted_out)
{
   terakan_gfx_command_writer_end_indirect_buffer(command_writer);

   command_writer->indirect_buffer =
      terakan_command_buffer_new_indirect_buffer(command_writer->base.command_buffer);
   if (command_writer->indirect_buffer == NULL) {
      return false;
   }

   terakan_bo_reference_writer_reset(&command_writer->base.bo_reference_writer,
                                     command_writer->indirect_buffer->bo_references,
                                     terakan_gfx_command_writer_device(command_writer)
                                        ->command_buffer_submission_size_gfx.bo_references);

   command_writer->is_beginning_indirect_buffer = true;

   terakan_gfx_command_writer_emit_preamble(command_writer);

   /* Clear all resources in the hardware. */
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, false);
   if (unlikely(packet == NULL)) {
      return false;
   }
   *packet++ = PKT3(PKT3_SET_CTL_CONST, 1, 0);
   *packet++ = TERAKAN_CTL_CONST_OFFSET(R_03CFF4_SQ_VTX_START_INST_LOC);
   *packet++ = UINT32_MAX;
   terakan_gfx_command_writer_emit_done(command_writer, packet);

   terakan_hw_state_draw_indirect_buffer_begun_and_sq_resources_cleared(
      &command_writer->hw_state_draw);

   bool const emit_all_state = command_writer->indirect_buffer_ever_begun;
   if (emit_all_state) {
      /* Re-emit the state from the previous indirect buffer. */
      terakan_hw_state_draw_emit_all(command_writer);
   }
   command_writer->indirect_buffer_ever_begun = true;

   command_writer->is_beginning_indirect_buffer = false;

   if (vk_command_buffer_has_error(&command_writer->base.command_buffer->vk)) {
      return false;
   }

   *all_state_emitted_out = emit_all_state;

   return true;
}

uint32_t *
terakan_gfx_command_writer_emit_with_bo(struct terakan_gfx_command_writer * const command_writer,
                                        uint32_t const packet_dwords,
                                        bool const abort_if_all_state_emitted,
                                        uint32_t const bo_count,
                                        uint32_t const relocation_for_32_bits_count,
                                        uint32_t const relocation_for_40_bits_count)
{
#ifndef NDEBUG
   assert(!command_writer->is_emitting &&
          "terakan_gfx_command_writer_emit_done must be called with the final append pointer after "
          "every command emission");
#endif

   /* Empty indirect buffer submissions may not be supported by the queue, make sure indirect
    * buffers can't be allocated only to end up being empty.
    */
   assert(packet_dwords != 0);

   if (unlikely(vk_command_buffer_has_error(&command_writer->base.command_buffer->vk))) {
      return NULL;
   }

   struct terakan_device const * const device = terakan_gfx_command_writer_device(command_writer);

   enum terakan_queue_relocation_type const relocation_type =
      terakan_device_physical_device(device)->submission_info_gfx.base.relocation_type;
   uint32_t relocation_count;
   bool relocation_array_used = false;
   uint32_t total_packet_dwords = packet_dwords;
   switch (relocation_type) {
   case TERAKAN_QUEUE_RELOCATION_TYPE_DRM_NOP:
      /* One relocation for the whole address. */
      relocation_count = relocation_for_32_bits_count + relocation_for_40_bits_count;
      total_packet_dwords += 2 * relocation_count;
      break;
   case TERAKAN_QUEUE_RELOCATION_TYPE_WDDM_PATCH:
      /* One relocation per address dword (two for 40-bit addresses). */
      relocation_count = relocation_for_32_bits_count + 2 * relocation_for_40_bits_count;
      relocation_array_used = true;
      break;
   default:
      assert(relocation_type == TERAKAN_QUEUE_RELOCATION_TYPE_NONE);
      relocation_count = 0;
   }

   if (command_writer->indirect_buffer == NULL ||
       (device->command_buffer_submission_size_gfx.indirect_buffer_dwords -
        command_writer->indirect_buffer->indirect_buffer_size_dwords) < total_packet_dwords ||
       (device->command_buffer_submission_size_gfx.bo_references -
        command_writer->base.bo_reference_writer.reference_count) < bo_count ||
       (relocation_array_used &&
        (device->command_buffer_submission_size_gfx.relocations -
         command_writer->indirect_buffer->relocation_count) < relocation_count)) {
      assert(!command_writer->is_beginning_indirect_buffer);
      if (unlikely(command_writer->is_beginning_indirect_buffer)) {
         /* Possibly a recursive overflow while moving to the new indirect buffer, if this happens,
          * it's a Terakan bug.
          */
         vk_command_buffer_set_error(&command_writer->base.command_buffer->vk,
                                     VK_ERROR_OUT_OF_HOST_MEMORY);
         return NULL;
      }
      bool all_state_emitted;
      if (!terakan_gfx_command_writer_new_indirect_buffer(command_writer, &all_state_emitted)) {
         return NULL;
      }
      if (abort_if_all_state_emitted && all_state_emitted) {
         return NULL;
      }
   }

   if (unlikely((device->command_buffer_submission_size_gfx.indirect_buffer_dwords -
                 command_writer->indirect_buffer->indirect_buffer_size_dwords) <
                   total_packet_dwords ||
                (device->command_buffer_submission_size_gfx.bo_references -
                 command_writer->base.bo_reference_writer.reference_count) < bo_count) ||
       (relocation_array_used &&
        (device->command_buffer_submission_size_gfx.relocations -
         command_writer->indirect_buffer->relocation_count) < relocation_count)) {
      assert(
         !"A single command emission is too large, no space even after moving to the new indirect "
          "buffer");
      vk_command_buffer_set_error(&command_writer->base.command_buffer->vk,
                                  VK_ERROR_OUT_OF_HOST_MEMORY);
      return NULL;
   }

#ifndef NDEBUG
   command_writer->is_emitting = true;
   command_writer->current_emission_relocations_remaining = relocation_count;
#endif

   uint32_t * const indirect_buffer_allocation =
      command_writer->indirect_buffer->indirect_buffer +
      command_writer->indirect_buffer->indirect_buffer_size_dwords;
   command_writer->indirect_buffer->indirect_buffer_size_dwords += total_packet_dwords;
   return indirect_buffer_allocation;
}

void
terakan_gfx_command_writer_add_relocation(struct terakan_gfx_command_writer * const command_writer,
                                          uint32_t ** const indirect_buffer_append_ptr,
                                          uint32_t const * const address_in_packet,
                                          uint32_t const wddm_allocation_offset,
                                          uint64_t const wddm_patch_ids,
                                          uint32_t const bo_reference)
{
   enum terakan_queue_relocation_type const relocation_type =
      terakan_gfx_command_writer_physical_device(command_writer)
         ->submission_info_gfx.base.relocation_type;

   if (relocation_type == TERAKAN_QUEUE_RELOCATION_TYPE_NONE) {
      return;
   }

#ifndef NDEBUG
   assert(command_writer->is_emitting);
   assert(command_writer->current_emission_relocations_remaining != 0);
   --command_writer->current_emission_relocations_remaining;
#endif

   switch (relocation_type) {
   case TERAKAN_QUEUE_RELOCATION_TYPE_DRM_NOP:
      *((*indirect_buffer_append_ptr)++) = PKT3(PKT3_NOP, 0, 0);
      /* DRM Radeon accepts BO reference offsets in dwords, so multiplying by
       * `sizeof(struct drm_radeon_cs_reloc) / sizeof(__u32)`.
       */
      *((*indirect_buffer_append_ptr)++) = 4 * bo_reference;
      break;

   case TERAKAN_QUEUE_RELOCATION_TYPE_WDDM_PATCH: {
      struct terakan_queue_relocation_wddm_patch * const patch =
         &((struct terakan_queue_relocation_wddm_patch *)command_writer->indirect_buffer
              ->relocations)[command_writer->indirect_buffer->relocation_count++];
      /* + 1 because a hAllocation = 0 allocation list entry is prepended when submitting. */
      patch->allocation_index = 1 + bo_reference;
      patch->slot_id = (uint32_t)wddm_patch_ids;
      patch->driver_id = (uint32_t)(wddm_patch_ids >> 32);
      patch->allocation_offset = wddm_allocation_offset;
      patch->patch_offset =
         sizeof(uint32_t) *
         (uint32_t)(address_in_packet - command_writer->indirect_buffer->indirect_buffer);
      patch->split_offset = 0;
   } break;

   default:
      assert(!"Unsupported relocation type");
   }
}

void
terakan_gfx_command_writer_emit_event_write_eop_discarding_data(
   struct terakan_gfx_command_writer * const command_writer, uint32_t const event)
{
   uint32_t * packet = terakan_gfx_command_writer_emit_with_bo(command_writer, 6, false, 1, 0, 1);
   if (unlikely(packet == NULL)) {
      return;
   }
   struct terakan_bo const * const gfx_discard_bo =
      terakan_gfx_command_writer_device(command_writer)->gfx_discard_bo;
   *packet++ = PKT3(PKT3_EVENT_WRITE_EOP, 5 - 1, 0);
   *packet++ = event;
   uint32_t const * const packet_address = packet;
   *packet++ = (uint32_t)gfx_discard_bo->va;      /* ADDRESS_LO */
   *packet++ = (gfx_discard_bo->va >> 32) & 0xFF; /* ADDRESS_HI, INT_SEL, DATA_SEL */
   *packet++ = 0;                                 /* DATA_LO */
   *packet++ = 0;                                 /* DATA_HI */
   terakan_gfx_command_writer_add_relocation_for_40_bits(
      command_writer, &packet, packet_address, packet_address + 1,
      TERASCALE_WDDM_PATCH_IDS_EVENT_WRITE_EOP_LO, TERASCALE_WDDM_PATCH_IDS_EVENT_WRITE_EOP_HI,
      terakan_bo_reference_writer_add_reference(&command_writer->base.bo_reference_writer,
                                                gfx_discard_bo, false, true,
                                                TERAKAN_BO_PRIORITY_SYNC));
   terakan_gfx_command_writer_emit_done(command_writer, packet);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_EndCommandBuffer(VkCommandBuffer const commandBuffer)
{
   struct terakan_command_buffer * const command_buffer =
      terakan_command_buffer_from_handle(commandBuffer);

   struct terakan_gfx_command_writer * const gfx_command_writer =
      command_buffer->command_writer.gfx;

   /* Insert a barrier for outstanding transfer writes because command buffers track the needed
    * barriers for this purpose locally, and subsequently submitted command buffers won't be aware
    * of how transfers were actually done in the current command buffer.
    */
   gfx_command_writer->pending_barrier_actions |=
      gfx_command_writer->post_buffer_copy_write_barrier_actions |
      gfx_command_writer->post_color_image_copy_write_barrier_actions |
      gfx_command_writer->post_depth_stencil_image_copy_write_barrier_actions;

   /* As barriers are deferred rather than emitted immediately in vkCmdPipelineBarrier, flush them.
    */
   terakan_barrier_emit_pending_actions(gfx_command_writer);

   terakan_gfx_command_writer_end_indirect_buffer(gfx_command_writer);

   return vk_command_buffer_end(&command_buffer->vk);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_BeginCommandBuffer(VkCommandBuffer const commandBuffer,
                           VkCommandBufferBeginInfo const * const pBeginInfo)
{
   struct terakan_command_buffer * const command_buffer =
      terakan_command_buffer_from_handle(commandBuffer);

   vk_command_buffer_begin(&command_buffer->vk, pBeginInfo);

   struct terakan_command_pool * const command_pool =
      container_of(command_buffer->vk.pool, struct terakan_command_pool, vk);

   assert(command_buffer->command_writer.gfx == NULL);
   struct terakan_gfx_command_writer * gfx_command_writer;
   if (!list_is_empty(&command_pool->command_writers_free)) {
      gfx_command_writer = list_first_entry(&command_pool->command_writers_free,
                                            struct terakan_gfx_command_writer, base.free_link);
      list_del(&gfx_command_writer->base.free_link);
   } else {
      gfx_command_writer =
         vk_alloc(&command_pool->vk.alloc, sizeof(struct terakan_gfx_command_writer),
                  alignof(struct terakan_gfx_command_writer), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
      if (gfx_command_writer == NULL) {
         return vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
      }
   }
   command_buffer->command_writer.gfx = gfx_command_writer;

   gfx_command_writer->base.command_buffer = command_buffer;

   /* The first emission will request the first indirect buffer. */
   gfx_command_writer->indirect_buffer = NULL;

   gfx_command_writer->indirect_buffer_ever_begun = false;

   gfx_command_writer->is_beginning_indirect_buffer = false;

#ifndef NDEBUG
   gfx_command_writer->is_emitting = false;
   gfx_command_writer->current_emission_relocations_remaining = 0;
#endif

   gfx_command_writer->pending_barrier_actions = 0;

   gfx_command_writer->post_buffer_copy_write_barrier_actions = 0;
   gfx_command_writer->post_color_image_copy_write_barrier_actions = 0;
   gfx_command_writer->post_depth_stencil_image_copy_write_barrier_actions = 0;

   terakan_hw_state_draw_reset(&gfx_command_writer->hw_state_draw);

   terakan_push_constants_state_reset(&gfx_command_writer->push_constants_state);

   struct terakan_device const * const device = terakan_command_buffer_device(command_buffer);

   terakan_state_draw_reset(&gfx_command_writer->state_draw, device);

   /* Section Appendix B: Memory Model "Availability, Visibility, and Domain Operations" of the
    * Vulkan 1.3.277 specification says:
    *
    *     "vkQueueSubmit performs a memory domain operation from host to device, and a visibility
    *     operation with source scope of the device domain and destination scope of all agents and
    *     references on the device."
    *
    * Make device memory visible to all agents on the device by invalidating all caches.
    * That's done via the command writer's emission logic, after setting up the initial state
    * registers, so the setup is not blocked by the waits involved.
    * This is only necessary for the first command buffer in a submission, but doing that here for
    * simplicity of submitting.
    */
   uint32_t const invalidate_caches_packets[] = {
      PKT3(PKT3_EVENT_WRITE, 1 - 1, 0),
      EVENT_TYPE(EVENT_TYPE_CACHE_FLUSH_AND_INV_EVENT) | EVENT_INDEX(0),

      PKT3(PKT3_SURFACE_SYNC, 4 - 1, 0),
      /* CP_COHER_CNTL and engine (ME). */
      S_0085F0_CB0_DEST_BASE_ENA(1) | S_0085F0_CB1_DEST_BASE_ENA(1) |
         S_0085F0_CB2_DEST_BASE_ENA(1) | S_0085F0_CB3_DEST_BASE_ENA(1) |
         S_0085F0_CB4_DEST_BASE_ENA(1) | S_0085F0_CB5_DEST_BASE_ENA(1) |
         S_0085F0_CB6_DEST_BASE_ENA(1) | S_0085F0_CB7_DEST_BASE_ENA(1) |
         S_0085F0_CB8_DEST_BASE_ENA(1) | S_0085F0_CB9_DEST_BASE_ENA(1) |
         S_0085F0_CB10_DEST_BASE_ENA(1) | S_0085F0_CB11_DEST_BASE_ENA(1) |
         S_0085F0_DB_DEST_BASE_ENA(1) | S_0085F0_TC_ACTION_ENA(1) |
         S_0085F0_VC_ACTION_ENA(
            terakan_device_physical_device(device)->chip_family_info.has_vertex_cache) |
         S_0085F0_CB_ACTION_ENA(1) | S_0085F0_DB_ACTION_ENA(1) | S_0085F0_SH_ACTION_ENA(1) |
         S_0085F0_SMX_ACTION_ENA(1) | ((uint32_t)1 << 31),
      /* CP_COHER_SIZE */
      UINT32_MAX,
      /* CP_COHER_BASE */
      0,
      /* POLL_INTERVAL */
      10,

      /* Make all prior writes made available by various packets in ME visible to PFP (indirect
       * arguments, index buffers).
       */
      PKT3(PKT3_PFP_SYNC_ME, 0, 0),
      0,
   };
   {
      uint32_t * invalidate_cache_packets_ptr = terakan_gfx_command_writer_emit(
         gfx_command_writer, ARRAY_SIZE(invalidate_caches_packets), false);
      if (likely(invalidate_cache_packets_ptr != NULL)) {
         memcpy(invalidate_cache_packets_ptr, invalidate_caches_packets,
                sizeof(invalidate_caches_packets));
         invalidate_cache_packets_ptr += ARRAY_SIZE(invalidate_caches_packets);
         terakan_gfx_command_writer_emit_done(gfx_command_writer, invalidate_cache_packets_ptr);
      }
   }

   return vk_command_buffer_get_record_result(&command_buffer->vk);
}

static void
terakan_command_pool_trim_resources(struct terakan_command_pool * const command_pool)
{
   list_for_each_entry_safe (struct terakan_gfx_command_writer, command_writer,
                             &command_pool->command_writers_free, base.free_link) {
      vk_free(&command_pool->vk.alloc, command_writer);
   }
   list_inithead(&command_pool->command_writers_free);

   list_for_each_entry_safe (struct terakan_command_buffer_indirect_buffer, indirect_buffer,
                             &command_pool->indirect_buffers_free, link) {
      vk_free(&command_pool->vk.alloc, indirect_buffer->relocations);
      vk_free(&command_pool->vk.alloc, indirect_buffer->indirect_buffer);
      vk_free(&command_pool->vk.alloc, indirect_buffer->bo_references);
      vk_free(&command_pool->vk.alloc, indirect_buffer);
   }
   list_inithead(&command_pool->indirect_buffers_free);

   list_for_each_entry_safe (struct terakan_push_buffer, push_buffer,
                             &command_pool->push_buffers_free, link) {
      terakan_bo_free(push_buffer->bo, &command_pool->vk.alloc);
      vk_free(&command_pool->vk.alloc, push_buffer);
   }
   list_inithead(&command_pool->push_buffers_free);
}

VKAPI_ATTR void VKAPI_CALL
terakan_TrimCommandPool(VkDevice const deviceHandle, VkCommandPool const commandPool,
                        UNUSED VkCommandPoolTrimFlags const flags)
{
   struct terakan_command_pool * const command_pool = terakan_command_pool_from_handle(commandPool);

   vk_command_pool_trim(&command_pool->vk, flags);

   terakan_command_pool_trim_resources(command_pool);
}

VKAPI_ATTR void VKAPI_CALL
terakan_DestroyCommandPool(VkDevice const deviceHandle, VkCommandPool const commandPool,
                           VkAllocationCallbacks const * const pAllocator)
{
   struct terakan_command_pool * const command_pool = terakan_command_pool_from_handle(commandPool);

   if (command_pool == NULL) {
      return;
   }

   /* Finish the command pool base before destroying their dependencies, as finishing the base
    * destroys all allocated command buffers, and resetting command buffers sends their dependencies
    * back to the free lists.
    */
   vk_command_pool_finish(&command_pool->vk);

   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);

   terakan_command_pool_trim_resources(command_pool);

   vk_free2(&device->vk.alloc, pAllocator, command_pool);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreateCommandPool(VkDevice const deviceHandle,
                          VkCommandPoolCreateInfo const * const pCreateInfo,
                          VkAllocationCallbacks const * const pAllocator,
                          VkCommandPool * const pCommandPool)
{
   VkResult result;

   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   struct terakan_command_pool * const command_pool =
      vk_alloc2(&device->vk.alloc, pAllocator, sizeof(struct terakan_command_pool),
                alignof(struct terakan_command_pool), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (command_pool == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   list_inithead(&command_pool->push_buffers_free);

   list_inithead(&command_pool->indirect_buffers_free);

   list_inithead(&command_pool->command_writers_free);

   result = vk_command_pool_init(&device->vk, &command_pool->vk, pCreateInfo, pAllocator);
   if (result != VK_SUCCESS) {
      vk_free2(&device->vk.alloc, pAllocator, command_pool);
      return result;
   }

   *pCommandPool = terakan_command_pool_to_handle(command_pool);
   return VK_SUCCESS;
}
