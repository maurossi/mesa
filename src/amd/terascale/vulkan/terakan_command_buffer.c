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

#include "terakan_descriptor.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_image.h"
#include "terakan_limits.h"
#include "terakan_physical_device.h"

#include "gallium/drivers/r600/r600d_common.h"
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
                                  void * const bo_references)
{
   writer->references = bo_references;

   writer->reference_count = 0;

   BITSET_ZERO(writer->map_entries_used);
}

uint32_t
terakan_bo_reference_writer_add_reference(struct terakan_bo_reference_writer * const writer,
                                          struct terakan_winsys_bo const * const bo,
                                          bool const is_reading, bool const is_writing,
                                          enum terakan_winsys_cs_bo_priority const priority)
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
      assert(writer->reference_count >= TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT);
      return UINT32_MAX;
   }

   /* Create the new or update the existing reference. */
   size_t const reference_size = bo->winsys->gpu_info.cs_bo_reference_size;
   if (reference_index != UINT32_MAX) {
      bo->winsys->cs_fn->update_bo_reference(
         (char *)writer->references + reference_size * reference_index, bo, is_reading, is_writing,
         priority);
   } else {
      if (unlikely(writer->reference_count >= TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT)) {
         return UINT32_MAX;
      }
      reference_index = writer->reference_count++;
      writer->reference_bos[reference_index] = bo;
      bo->winsys->cs_fn->create_bo_reference(
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

   assert(reference_size % sizeof(uint32_t) == 0);
   return (uint32_t)(reference_size / sizeof(uint32_t)) * reference_index;
}

void *
terakan_command_buffer_allocate_push_constants(struct terakan_command_buffer * const command_buffer,
                                               uint32_t const size_bytes,
                                               struct terakan_winsys_bo const ** const bo_out,
                                               uint32_t * base_cache_lines_out)
{
   assert(size_bytes <= TERAKAN_CONSTANT_CACHE_MAX_BUFFER_SIZE_BYTES);

   uint32_t const size_cache_lines =
      (size_bytes + (TERAKAN_CONSTANT_CACHE_LINE_BYTES - 1)) / TERAKAN_CONSTANT_CACHE_LINE_BYTES;

   /* One constant buffer of the maximum possible size.
    * Because allocation is linear, there may be a lot of fragmentation if requested push constant
    * amounts are large.
    * However, it's expected that very large push constants are rare, so it's more preferable not to
    * allocate too much memory for push constants in small command buffers instead of creating
    * larger BOs.
    * Fragmentation can also be filled with 1-cache-line push constants.
    */
   uint32_t const buffer_size_cache_lines = TERAKAN_CONSTANT_CACHE_MAX_LINES_IN_BUFFER;

   if (!list_is_empty(&command_buffer->push_constant_buffers_with_free_space) &&
       list_first_entry(&command_buffer->push_constant_buffers_with_free_space,
                        struct terakan_push_constant_buffer, link)
             ->cache_lines_free >= size_cache_lines) {
      /* Small buffers (not larger than 1 constant cache line) are expected to be the most common,
       * and they are allocated from the tail rather than from the head to fill the fragmentation
       * padding.
       */
      struct terakan_push_constant_buffer * const existing_buffer =
         size_cache_lines <= 1
            ? list_last_entry(&command_buffer->push_constant_buffers_with_free_space,
                              struct terakan_push_constant_buffer, link)
            : list_first_entry(&command_buffer->push_constant_buffers_with_free_space,
                               struct terakan_push_constant_buffer, link);
      uint32_t const existing_buffer_offset_cache_lines =
         buffer_size_cache_lines - existing_buffer->cache_lines_free;

      existing_buffer->cache_lines_free -= size_cache_lines;
      if (existing_buffer->cache_lines_free == 0) {
         list_move_to(&existing_buffer->link, &command_buffer->push_constant_buffers_full);
      }

      *bo_out = existing_buffer->bo;
      *base_cache_lines_out = existing_buffer_offset_cache_lines;
      return (char *)existing_buffer->bo->mapping +
             TERAKAN_CONSTANT_CACHE_LINE_BYTES * existing_buffer_offset_cache_lines;
   }

   struct terakan_push_constant_buffer * new_buffer;

   struct terakan_command_pool * const command_pool =
      container_of(command_buffer->vk.pool, struct terakan_command_pool, vk);
   if (!list_is_empty(&command_pool->push_constant_buffers_free)) {
      new_buffer = list_first_entry(&command_pool->push_constant_buffers_free,
                                    struct terakan_push_constant_buffer, link);
      list_del(&new_buffer->link);
   } else {
      new_buffer =
         vk_alloc(&command_pool->vk.alloc, sizeof(struct terakan_push_constant_buffer),
                  alignof(struct terakan_push_constant_buffer), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
      if (new_buffer == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
         return NULL;
      }

      struct terakan_winsys * const winsys =
         container_of(command_buffer->vk.pool->base.device->physical,
                      struct terakan_physical_device const, vk)
            ->winsys;

      new_buffer->bo = winsys->bo_fn->allocate_device_memory(
         winsys, TERAKAN_CONSTANT_CACHE_LINE_BYTES * buffer_size_cache_lines,
         TERAKAN_CONSTANT_CACHE_LINE_BYTES,
         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
         0);
      if (new_buffer->bo == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_DEVICE_MEMORY);
         vk_free(&command_pool->vk.alloc, new_buffer);
         return NULL;
      }

      if (terakan_winsys_bo_map(new_buffer->bo) == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
         terakan_winsys_bo_free(new_buffer->bo);
         vk_free(&command_pool->vk.alloc, new_buffer);
         return NULL;
      }
   }

   if (size_cache_lines < buffer_size_cache_lines) {
      new_buffer->cache_lines_free = buffer_size_cache_lines - size_cache_lines;
      list_add(&new_buffer->link, &command_buffer->push_constant_buffers_with_free_space);
   } else {
      list_add(&new_buffer->link, &command_buffer->push_constant_buffers_full);
   }

   *bo_out = new_buffer->bo;
   *base_cache_lines_out = 0;
   return new_buffer->bo->mapping;
}

static struct terakan_command_buffer_submission_indirect_buffer *
terakan_command_buffer_new_indirect_buffer(struct terakan_command_buffer * const command_buffer)
{
   if (vk_command_buffer_has_error(&command_buffer->vk)) {
      return NULL;
   }

   struct terakan_command_pool * const command_pool =
      container_of(command_buffer->vk.pool, struct terakan_command_pool, vk);

   struct terakan_command_buffer_submission_indirect_buffer * indirect_buffer;
   if (!list_is_empty(&command_pool->indirect_buffers_free)) {
      indirect_buffer =
         list_first_entry(&command_pool->indirect_buffers_free,
                          struct terakan_command_buffer_submission_indirect_buffer, free_link);
      list_del(&indirect_buffer->free_link);
   } else {
      indirect_buffer = vk_alloc(&command_pool->vk.alloc,
                                 sizeof(struct terakan_command_buffer_submission_indirect_buffer),
                                 alignof(struct terakan_command_buffer_submission_indirect_buffer),
                                 VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
      if (indirect_buffer == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
         return NULL;
      }

      indirect_buffer->base.is_secondary_execution = false;

      struct terakan_gpu_info const * const gpu_info =
         &container_of(command_buffer->vk.pool->base.device->physical,
                       struct terakan_physical_device const, vk)
             ->winsys->gpu_info;

      indirect_buffer->bo_references =
         vk_alloc(&command_pool->vk.alloc,
                  gpu_info->cs_bo_reference_size * TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT,
                  gpu_info->cs_bo_reference_alignment, VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
      if (indirect_buffer->bo_references == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
         vk_free(&command_pool->vk.alloc, indirect_buffer);
         return NULL;
      }

      indirect_buffer->indirect_buffer = vk_alloc(
         &command_pool->vk.alloc, sizeof(uint32_t) * TERAKAN_MAX_INDIRECT_BUFFER_SIZE_DWORDS,
         alignof(uint32_t), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
      if (indirect_buffer->indirect_buffer == NULL) {
         vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
         vk_free(&command_pool->vk.alloc, indirect_buffer->bo_references);
         vk_free(&command_pool->vk.alloc, indirect_buffer);
         return NULL;
      }
   }

   indirect_buffer->bo_reference_count = 0;
   indirect_buffer->indirect_buffer_size_dwords = 0;

   list_addtail(&indirect_buffer->base.command_buffer_submission_link,
                &command_buffer->submissions);

   return indirect_buffer;
}

static void
terakan_command_buffer_release_resources(struct terakan_command_buffer * const command_buffer)
{
   struct terakan_command_pool * const command_pool =
      container_of(command_buffer->vk.pool, struct terakan_command_pool, vk);

   if (command_buffer->command_writer.gfx != NULL) {
      struct terakan_command_pool * const command_pool =
         container_of(command_buffer->vk.pool, struct terakan_command_pool, vk);
      list_add(&command_buffer->command_writer.gfx->base.free_link,
               &command_pool->command_writers_free);
      command_buffer->command_writer.gfx = NULL;
   }

   list_for_each_entry_safe(struct terakan_command_buffer_submission, submission_base,
                            &command_buffer->submissions, command_buffer_submission_link)
   {
      list_del(&submission_base->command_buffer_submission_link);
      if (submission_base->is_secondary_execution) {
         struct terakan_command_buffer_submission_secondary_execution * const submission =
            container_of(submission_base,
                         struct terakan_command_buffer_submission_secondary_execution, base);
         list_add(&submission->free_link, &command_pool->secondary_executions_free);
      } else {
         struct terakan_command_buffer_submission_indirect_buffer * const submission = container_of(
            submission_base, struct terakan_command_buffer_submission_indirect_buffer, base);
         list_add(&submission->free_link, &command_pool->indirect_buffers_free);
      }
   }

   list_splice(&command_buffer->push_constant_buffers_full,
               &command_pool->push_constant_buffers_free);
   list_inithead(&command_buffer->push_constant_buffers_full);
   list_splice(&command_buffer->push_constant_buffers_with_free_space,
               &command_pool->push_constant_buffers_free);
   list_inithead(&command_buffer->push_constant_buffers_with_free_space);
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

   list_inithead(&command_buffer->push_constant_buffers_with_free_space);
   list_inithead(&command_buffer->push_constant_buffers_full);

   list_inithead(&command_buffer->submissions);

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
   if (command_writer->indirect_buffer == NULL) {
      return;
   }

   command_writer->indirect_buffer->bo_reference_count =
      command_writer->base.bo_reference_writer.reference_count;

   /* Pad the GFX ring indirect buffer to a multiple of 8 dwords with NOPs. */
   while ((command_writer->indirect_buffer->indirect_buffer_size_dwords & 7) != 0) {
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
    * - Linux kernel Radeon driver
    * - fglrx indirect buffers
    */

   struct terakan_gpu_info const * const gpu_info =
      &container_of(command_writer->base.command_buffer->vk.pool->base.device->physical,
                    struct terakan_physical_device const, vk)
          ->winsys->gpu_info;

   uint32_t * packet;

   /* Disable register shadowing before setting any registers. */
   packet = terakan_gfx_command_writer_emit(command_writer, 3, 0, 0, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_CONTEXT_CONTROL, 1, 0);
   *packet++ = (uint32_t)1 << 31; /* CC0_UPDATE_LOAD_ENABLES(1) */
   *packet++ = (uint32_t)1 << 31; /* CC1_UPDATE_SHADOW_ENABLES(1) */

   /*
    * Setup graphics context registers outside terakan_hw_state_draw.
    */

   if (gpu_info->gfx_level <= EVERGREEN) {
      /* Workaround for hardware issues with dynamic GPRs - must set all limits to 240 (in units of
       * 8 registers) instead of 0. */
      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(R_028838_SQ_DYN_GPR_RESOURCE_LIMIT_1);
      /* Workaround for hardware issues with dynamic GPRs - must set all limits to 240 (in units of
       * 8 registers) instead of 0. */
      *packet++ = S_028838_PS_GPRS(0x1E) | S_028838_VS_GPRS(0x1E) | S_028838_GS_GPRS(0x1E) |
                  S_028838_ES_GPRS(0x1E) | S_028838_HS_GPRS(0x1E) | S_028838_LS_GPRS(0x1E);
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
      /* TODO(Triang3l): Gallium R600 has 0 for SPI_THREAD_GROUPING, but Linux Radeon 2.50.0 has 1
       * in cleanstate_evergreen/cayman.h. Research which is more correct.
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
      S_028034_BR_X(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES) |
         S_028034_BR_Y(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES),

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 2, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028204_PA_SC_WINDOW_SCISSOR_TL),
      /* R_028204_PA_SC_WINDOW_SCISSOR_TL */
      0,
      /* R_028208_PA_SC_WINDOW_SCISSOR_BR */
      S_028208_BR_X(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES) |
         S_028208_BR_Y(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES),

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
      S_028244_BR_X(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES) |
         S_028244_BR_Y(TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES),

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
      PKT3(PKT3_SET_CONTEXT_REG, 2, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028A48_PA_SC_MODE_CNTL_0),
      /* R_028A48_PA_SC_MODE_CNTL_0 */
      0,
      /* R_028A4C_PA_SC_MODE_CNTL_1 */
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
      PKT3(PKT3_SET_CONTEXT_REG, 2, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_02800C_DB_RENDER_OVERRIDE),
      /* R_02800C_DB_RENDER_OVERRIDE */
      0,
      /* R_028010_DB_RENDER_OVERRIDE2 */
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028800_DB_DEPTH_CONTROL),
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_02880C_DB_SHADER_CONTROL),
      S_02880C_Z_ORDER(V_02880C_EARLY_Z_THEN_LATE_Z) | S_02880C_DUAL_EXPORT_ENABLE(1) |
         S_02880C_DB_SOURCE_FORMAT(V_02880C_EXPORT_DB_TWO),

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028B70_DB_ALPHA_TO_MASK),
      0,

      /*
       * Color buffer.
       */

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 8, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028780_CB_BLEND0_CONTROL),
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,

      /* TODO(Triang3l): Move to hw_state_draw. */
      PKT3(PKT3_SET_CONTEXT_REG, 1, 0),
      TERAKAN_CONTEXT_REG_OFFSET(R_028808_CB_COLOR_CONTROL),
      S_028808_MODE(V_028808_CB_NORMAL) | S_028808_ROP3(0xCC),
   };

   packet =
      terakan_gfx_command_writer_emit(command_writer, ARRAY_SIZE(draw_context_regs), 0, 0, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   memcpy(packet, draw_context_regs, sizeof(draw_context_regs));

   if (gpu_info->gfx_level >= CAYMAN) {
      /* TODO(Triang3l): Move to hw_state_draw. */
      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_028AA8_IA_MULTI_VGT_PARAM);
      *packet++ = S_028AA8_PRIMGROUP_SIZE(128 - 1);

      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_0286FC_SPI_LDS_MGMT);
      *packet++ = 0;

      /* TODO(Triang3l): Move to hw_state_draw. */
      packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
      *packet++ = TERAKAN_CONTEXT_REG_OFFSET(CM_R_028804_DB_EQAA);
      *packet++ = S_028804_HIGH_QUALITY_INTERSECTIONS(1) | S_028804_INCOHERENT_EQAA_READS(1) |
                  S_028804_STATIC_ANCHOR_ASSOCIATIONS(1);
   }

   packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONTEXT_REG, 1, 0);
   *packet++ = TERAKAN_CONTEXT_REG_OFFSET(gpu_info->gfx_level >= CAYMAN ? CM_R_028BE4_PA_SU_VTX_CNTL
                                                                        : R_028C08_PA_SU_VTX_CNTL);
   *packet++ = S_028C08_PIX_CENTER_HALF(1) | S_028C08_ROUND_MODE(V_028C08_X_ROUND_TO_EVEN) |
               S_028C08_QUANT_MODE(V_028C08_X_1_256TH);

   /*
    * Setup configuration registers common between graphics and compute.
    */

   uint32_t sq_config = S_008C00_VC_ENABLE(gpu_info->has_vertex_cache) | S_008C00_EXPORT_SRC_C(1);
   /* Not raising CS2 priority in SQ_CONFIG on R9xx unlike in Linux Radeon 2.50.0 because it doesn't
    * expose the compute rings at all.
    */
   if (gpu_info->gfx_level <= EVERGREEN) {
      sq_config |= S_008C00_LS_PRIO(3) | S_008C00_HS_PRIO(3) | S_008C00_ES_PRIO(3) |
                   S_008C00_GS_PRIO(2) | S_008C00_VS_PRIO(1) | S_008C00_PS_PRIO(0) |
                   S_008C00_CS_PRIO(0);
   }
   packet = terakan_gfx_command_writer_emit(command_writer, 2 + 2 + 2 + 2, 0, 0, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_SET_CONFIG_REG, gpu_info->gfx_level >= CAYMAN ? 2 : 6, 0);
   *packet++ = TERAKAN_CONFIG_REG_OFFSET(R_008C00_SQ_CONFIG);
   /* R_008C00_SQ_CONFIG */
   *packet++ = sq_config;
   /* R_008C04_SQ_GPR_RESOURCE_MGMT_1 */
   *packet++ = S_008C04_NUM_CLAUSE_TEMP_GPRS(4);
   if (gpu_info->gfx_level >= CAYMAN) {
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

   /* TODO(Triang3l): Dynamic GPR usage on R8xx - see evergreen_emit_config_state, and also disable
    * them for tessellation, see evergreen_adjust_gprs. Keep them always enabled for R9xx though.
    */
   packet = terakan_gfx_command_writer_emit(command_writer, 2 + 2 + 1, 0, 0, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   *packet++ = PKT3(PKT3_EVENT_WRITE, 0, 0);
   *packet++ = EVENT_TYPE(EVENT_TYPE_PS_PARTIAL_FLUSH) | EVENT_INDEX(4);
   *packet++ = PKT3(PKT3_SET_CONFIG_REG, 1, 0);
   *packet++ = TERAKAN_CONFIG_REG_OFFSET(R_008D8C_SQ_DYN_GPR_CNTL_PS_FLUSH_REQ);
   *packet++ = S_008D8C_DYN_GPR_ENABLE(1);

   uint32_t const config_regs[] = {
      /* Remove LS and HS from one SIMD for a hardware bug workaround according to the Gallium R600
       * driver.
       */
      PKT3(PKT3_SET_CONFIG_REG, 3, 0),
      TERAKAN_CONFIG_REG_OFFSET(R_008E20_SQ_STATIC_THREAD_MGMT1),
      /* R_008E20_SQ_STATIC_THREAD_MGMT1 */
      ~(uint32_t)0,
      /* R_008E20_SQ_STATIC_THREAD_MGMT2 */
      ~(uint32_t)0,
      /* R_008E20_SQ_STATIC_THREAD_MGMT3 */
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

   packet = terakan_gfx_command_writer_emit(command_writer, ARRAY_SIZE(config_regs), 0, 0, false);
   if (unlikely(packet == NULL)) {
      return;
   }
   memcpy(packet, config_regs, sizeof(config_regs));

   /*
    * Setup configuration registers for graphics.
    */

   /* TODO(Triang3l): Emit the values for graphics or compute when switching between the two. */

   if (gpu_info->gfx_level <= EVERGREEN) {
      /* The thread counts should be a multiple of 8 as space is allocated in blocks of 8 according
       * to the register reference.
       * Linux Radeon 2.50.0 spreads the non-pixel-shader threads evenly between 6 stages, but aside
       * from the pixel shader there are 5 stages - allocate more.
       */
      uint32_t const sq_vertex_threads =
         (gpu_info->sq_max_threads - gpu_info->sq_ps_threads_r8xx) / 5 / 8 * 8;

      uint32_t const sq_stage_stack_entries = gpu_info->sq_max_stack_entries / 6;

      uint32_t const sq_thread_stack_register_count =
         (R_008C28_SQ_STACK_RESOURCE_MGMT_3 - R_008C18_SQ_THREAD_RESOURCE_MGMT_1) /
            sizeof(uint32_t) +
         1;
      packet = terakan_gfx_command_writer_emit(
         command_writer, 2 + sq_thread_stack_register_count + 2 + 1, 0, 0, false);
      if (unlikely(packet == NULL)) {
         return;
      }
      *packet++ = PKT3(PKT3_SET_CONFIG_REG, sq_thread_stack_register_count, 0);
      *packet++ = TERAKAN_CONFIG_REG_OFFSET(R_008C18_SQ_THREAD_RESOURCE_MGMT_1);
      /* R_008C18_SQ_THREAD_RESOURCE_MGMT_1 */
      *packet++ = S_008C18_NUM_PS_THREADS(gpu_info->sq_ps_threads_r8xx) |
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
                                     command_writer->indirect_buffer->bo_references);

   command_writer->is_beginning_indirect_buffer = true;

   terakan_gfx_command_writer_emit_preamble(command_writer);

   /* Clear all resources in the hardware. */
   uint32_t * packet = terakan_gfx_command_writer_emit(command_writer, 2 + 1, 0, 0, false);
   if (unlikely(packet == NULL)) {
      return false;
   }
   *packet++ = PKT3(PKT3_SET_CTL_CONST, 1, 0);
   *packet++ = TERAKAN_CTL_CONST_OFFSET(R_03CFF4_SQ_VTX_START_INST_LOC);
   *packet++ = UINT32_MAX;

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
terakan_gfx_command_writer_emit(struct terakan_gfx_command_writer * const command_writer,
                                uint32_t const packet_dwords, uint32_t const bo_count,
                                uint32_t const relocation_packet_dwords,
                                bool abort_if_all_state_emitted)
{
   if (unlikely(vk_command_buffer_has_error(&command_writer->base.command_buffer->vk))) {
      return NULL;
   }

   uint32_t const total_packet_dwords = packet_dwords + relocation_packet_dwords;

   uint32_t const indirect_buffer_max_dwords =
      TERAKAN_MAX_INDIRECT_BUFFER_SIZE_DWORDS & ~((uint32_t)7);

   if (command_writer->indirect_buffer == NULL ||
       (indirect_buffer_max_dwords - command_writer->indirect_buffer->indirect_buffer_size_dwords) <
          total_packet_dwords ||
       (TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT -
        command_writer->base.bo_reference_writer.reference_count) < bo_count) {
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

   if (unlikely((indirect_buffer_max_dwords -
                 command_writer->indirect_buffer->indirect_buffer_size_dwords) <
                   total_packet_dwords ||
                (TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT -
                 command_writer->base.bo_reference_writer.reference_count) < bo_count)) {
      assert(
         !"A single command emission is too large, no space even after moving to the new indirect "
          "buffer");
      vk_command_buffer_set_error(&command_writer->base.command_buffer->vk,
                                  VK_ERROR_OUT_OF_HOST_MEMORY);
      return NULL;
   }

   uint32_t * const indirect_buffer_allocation =
      command_writer->indirect_buffer->indirect_buffer +
      command_writer->indirect_buffer->indirect_buffer_size_dwords;
   command_writer->indirect_buffer->indirect_buffer_size_dwords += total_packet_dwords;
   return indirect_buffer_allocation;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_EndCommandBuffer(VkCommandBuffer const commandBuffer)
{
   struct terakan_command_buffer * const command_buffer =
      terakan_command_buffer_from_handle(commandBuffer);

   terakan_gfx_command_writer_end_indirect_buffer(command_buffer->command_writer.gfx);

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
   if (!list_is_empty(&command_pool->command_writers_free)) {
      command_buffer->command_writer.gfx = list_first_entry(
         &command_pool->command_writers_free, struct terakan_gfx_command_writer, base.free_link);
      list_del(&command_buffer->command_writer.gfx->base.free_link);
   } else {
      command_buffer->command_writer.gfx =
         vk_alloc(&command_pool->vk.alloc, sizeof(struct terakan_gfx_command_writer),
                  alignof(struct terakan_gfx_command_writer), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
      if (command_buffer->command_writer.gfx == NULL) {
         return vk_command_buffer_set_error(&command_buffer->vk, VK_ERROR_OUT_OF_HOST_MEMORY);
      }
   }

   command_buffer->command_writer.gfx->base.command_buffer = command_buffer;

   command_buffer->command_writer.gfx->indirect_buffer_ever_begun = false;

   /* The first emission will request the first indirect buffer. */
   command_buffer->command_writer.gfx->is_beginning_indirect_buffer = false;

   terakan_hw_state_draw_reset(&command_buffer->command_writer.gfx->hw_state_draw);

   terakan_state_draw_reset(&command_buffer->command_writer.gfx->state_draw);

   return vk_command_buffer_get_record_result(&command_buffer->vk);
}

static void
terakan_command_pool_trim_resources(struct terakan_command_pool * const command_pool)
{
   list_for_each_entry_safe(struct terakan_gfx_command_writer, command_writer,
                            &command_pool->command_writers_free, base.free_link)
   {
      vk_free(&command_pool->vk.alloc, command_writer);
   }
   list_inithead(&command_pool->command_writers_free);

   list_for_each_entry_safe(struct terakan_command_buffer_submission_secondary_execution,
                            submission, &command_pool->secondary_executions_free, free_link)
   {
      vk_free(&command_pool->vk.alloc, submission);
   }
   list_inithead(&command_pool->secondary_executions_free);

   list_for_each_entry_safe(struct terakan_command_buffer_submission_indirect_buffer, submission,
                            &command_pool->indirect_buffers_free, free_link)
   {
      vk_free(&command_pool->vk.alloc, submission->indirect_buffer);
      vk_free(&command_pool->vk.alloc, submission->bo_references);
      vk_free(&command_pool->vk.alloc, submission);
   }
   list_inithead(&command_pool->indirect_buffers_free);

   list_for_each_entry_safe (struct terakan_push_constant_buffer, push_constant_buffer,
                             &command_pool->push_constant_buffers_free, link) {
      terakan_winsys_bo_free(push_constant_buffer->bo);
      vk_free(&command_pool->vk.alloc, push_constant_buffer);
   }
   list_inithead(&command_pool->push_constant_buffers_free);
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

   list_inithead(&command_pool->push_constant_buffers_free);

   list_inithead(&command_pool->indirect_buffers_free);

   list_inithead(&command_pool->secondary_executions_free);

   list_inithead(&command_pool->command_writers_free);

   result = vk_command_pool_init(&device->vk, &command_pool->vk, pCreateInfo, pAllocator);
   if (result != VK_SUCCESS) {
      vk_free2(&device->vk.alloc, pAllocator, command_pool);
      return result;
   }

   *pCommandPool = terakan_command_pool_to_handle(command_pool);

   return VK_SUCCESS;
}
