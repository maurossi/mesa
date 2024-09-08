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

#ifndef TERAKAN_COMMAND_BUFFER_H
#define TERAKAN_COMMAND_BUFFER_H

#include "winsys/terakan_winsys.h"
#include "terakan_hw_state.h"
#include "terakan_state.h"

#include "gallium/drivers/r600/evergreend.h"
#include "util/bitset.h"
#include "util/list.h"
#include "vk_command_buffer.h"
#include "vk_command_pool.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define TERAKAN_CONFIG_REG_OFFSET(address)  (((address)-EVERGREEN_CONFIG_REG_OFFSET) >> 2)
#define TERAKAN_CONTEXT_REG_OFFSET(address) (((address)-EVERGREEN_CONTEXT_REG_OFFSET) >> 2)
#define TERAKAN_CTL_CONST_OFFSET(address)   (((address)-EVERGREEN_CTL_CONST_OFFSET) >> 2)

/* Given that Terakan exposes more sampled image bindings than the Gallium R600 driver due to
 * separate images and samplers, the number of bindings may be much bigger, and thus, also taking
 * into account that each binding is set one by one, the sizes are larger than in the Gallium R600
 * driver.
 */

/* Must be large enough to hold all the necessary setup, including up to 1024 resources (up to 14
 * dwords per resource - 2 for the SET_RESOURCE header, 8 for the constant, and 4 dwords for 2
 * relocations for textures), for at least one draw / dispatch command.
 * Command buffers using virtual memory on Linux must not be larger than RADEON_INFO_IB_VM_MAX_SIZE
 * dwords reported by the kernel driver, however.
 * Twice the size in the Gallium R600 driver as of May 2023.
 */
#define TERAKAN_MAX_INDIRECT_BUFFER_SIZE_DWORDS ((uint32_t)1 << 15)

/* Must be large enough to hold all bindings for a single command even if they all point to
 * different BOs.
 * Assuming that new references may be needed every 8 dwords on average (resource constants are
 * 2 SET_RESOURCE dwords plus 10-12 dwords, DRAW_INDEX_2 is 6 dwords plus 2 dwords for the
 * relocation).
 */
#define TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT_LOG2 12
#define TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT                                                \
   ((uint32_t)1 << TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT_LOG2)

/* Double as large as the reference count to reduce the likelihood of hash collisions, and also to
 * provide one additional entry per hash value for quick collision resolution.
 * Twice the size of the Gallium R600 driver relocation hash table as of May 2023.
 */
#define TERAKAN_BO_REFERENCE_HASH_BITS (TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT_LOG2 + 1)
#define TERAKAN_BO_REFERENCE_HASH_MASK (((uint32_t)1 << TERAKAN_BO_REFERENCE_HASH_BITS) - 1)
static_assert(
   TERAKAN_BO_REFERENCE_HASH_MASK + 1 >= TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT,
   "There need to be enough BO reference hash map entries for each BO reference with the default "
   "BO reference count, so it can be assumed externally that allocation can fail only due to an "
   "overflow of the total BO reference count and not because of the hash map.");

struct terakan_bo_reference_writer {
   struct terakan_winsys const * winsys;
   void * references;

   uint32_t reference_count;

   struct terakan_winsys_bo const * reference_bos[TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT];

   /* Which elements of the map are used, faster to clear than the array itself. */
   BITSET_DECLARE(map_entries_used, TERAKAN_BO_REFERENCE_HASH_MASK + 1);
   uint32_t map[TERAKAN_BO_REFERENCE_HASH_MASK + 1];
};

/* bo_references must point to `terakan_gpu_info::cs_bo_reference_size *
 * TERAKAN_BO_REFERENCE_WRITER_REFERENCE_COUNT` references that will be passed to the winsys during
 * submission.
 */
void terakan_bo_reference_writer_reset(struct terakan_bo_reference_writer * writer,
                                       void * bo_references);

/* Returns the reference offset in dwords to use in the relocation, or UINT32_MAX if too many
 * references.
 */
uint32_t terakan_bo_reference_writer_add_reference(struct terakan_bo_reference_writer * writer,
                                                   struct terakan_winsys_bo const * bo,
                                                   bool is_reading, bool is_writing,
                                                   enum terakan_winsys_cs_bo_priority priority);

struct terakan_push_constant_buffer {
   struct terakan_winsys_bo * bo;

   uint32_t cache_lines_free;

   struct list_head link;
};

struct terakan_command_buffer_submission {
   bool is_secondary_execution;

   /* Within terakan_command_buffer::submissions. */
   struct list_head command_buffer_submission_link;
};

struct terakan_command_buffer_submission_indirect_buffer {
   struct terakan_command_buffer_submission base;

   /* Within terakan_command_pool::indirect_buffers_free. */
   struct list_head free_link;

   uint32_t bo_reference_count;
   void * bo_references;

   uint32_t indirect_buffer_size_dwords;
   uint32_t * indirect_buffer;
};

struct terakan_command_buffer_submission_secondary_execution {
   struct terakan_command_buffer_submission base;

   /* Within terakan_command_pool::secondary_executions_free. */
   struct list_head free_link;

   struct terakan_command_buffer_submission_indirect_buffer const * indirect_buffer;
};

struct terakan_gfx_command_writer;

struct terakan_command_buffer {
   struct vk_command_buffer vk;

   struct list_head push_constant_buffers_with_free_space;
   struct list_head push_constant_buffers_full;

   struct list_head submissions;

   union {
      struct terakan_gfx_command_writer * gfx;
   } command_writer;
};

VK_DEFINE_HANDLE_CASTS(terakan_command_buffer, vk.base, VkCommandBuffer,
                       VK_OBJECT_TYPE_COMMAND_BUFFER)

/* Returns the mapping, or NULL if failed. */
void * terakan_command_buffer_allocate_push_constants(
   struct terakan_command_buffer * command_buffer, uint32_t size_bytes,
   struct terakan_winsys_bo const ** bo_out, uint32_t * base_cache_lines_out);

extern struct vk_command_buffer_ops const terakan_command_buffer_ops;

struct terakan_command_writer {
   /* Within terakan_command_pool::command_writers_free. */
   struct list_head free_link;

   struct terakan_command_buffer * command_buffer;

   struct terakan_bo_reference_writer bo_reference_writer;
};

struct terakan_gfx_command_writer {
   struct terakan_command_writer base;

   struct terakan_command_buffer_submission_indirect_buffer * indirect_buffer;

   bool indirect_buffer_ever_begun;

   bool is_beginning_indirect_buffer;

   struct terakan_hw_state_draw hw_state_draw;

   struct terakan_state_draw state_draw;
};

/* Entry point for emitting packets.
 * Allocates space for `packet_dwords`, and if relocations are needed, `relocation_packet_dwords`,
 * and assumes that the application will write them all.
 * Also ensures that `bo_count` calls to `terakan_bo_reference_writer_add_reference` for
 * `terakan_gfx_command_writer::bo_reference_writer` will succeed (regardless of which BOs are
 * specified).
 * Switches to the next indirect buffer and reapplies the state if needed.
 * Returns a pointer to the packet dwords, or NULL if failed to allocate (the result must be
 * checked).
 * The returned BO reference allocation is valid within the current command buffer recording until
 * the next `terakan_gfx_command_writer_emit` call for it.
 */
uint32_t * terakan_gfx_command_writer_emit(struct terakan_gfx_command_writer * command_writer,
                                           uint32_t packet_dwords, uint32_t bo_count,
                                           uint32_t relocation_packet_dwords,
                                           bool abort_if_all_state_emitted);

struct terakan_command_pool {
   struct vk_command_pool vk;

   struct list_head push_constant_buffers_free;

   struct list_head indirect_buffers_free;

   struct list_head secondary_executions_free;

   struct list_head command_writers_free;
};

VK_DEFINE_HANDLE_CASTS(terakan_command_pool, vk.base, VkCommandPool, VK_OBJECT_TYPE_COMMAND_POOL)

#endif /* TERAKAN_COMMAND_BUFFER_H */
