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

#include "terakan_bo_drm_radeon.h"
#include "terakan_device_drm_radeon.h"
#include "terakan_queue.h"

#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600d_common.h"
#include "util/macros.h"
#include "util/u_debug.h"
#include "c99_alloca.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <xf86drm.h>
#include <radeon_drm.h>

static VkResult
terakan_queue_drm_radeon_submit(struct terakan_device * const device_base,
                                enum amd_ip_type const ip_type, uint32_t const bo_reference_count,
                                void const * const bo_references,
                                uint32_t const indirect_buffer_size_dwords,
                                uint32_t const * const indirect_buffer)
{
   if (indirect_buffer_size_dwords == 0) {
      /* The kernel driver returns -EINVAL for zero-length indirect buffers. */
      return VK_SUCCESS;
   }

   /* Flags, ring, priority. */
   __u32 flags[3] = {};

   switch (ip_type) {
   case AMD_IP_GFX:
      /* CP fetch requires 8 dword alignment, the provided indirect buffer must be padded with NOPs
       * externally if needed.
       */
      assert((indirect_buffer_size_dwords & 7) == 0);
      flags[0] = RADEON_CS_KEEP_TILING_FLAGS;
      flags[1] = RADEON_CS_RING_GFX;
      break;

   default:
      assert(!"Unsupported queue type");
      return VK_ERROR_UNKNOWN;
   }

   struct drm_radeon_cs_chunk relocations_chunk = {
      .chunk_id = RADEON_CHUNK_ID_RELOCS,
      .length_dw =
         (__u32)((sizeof(struct drm_radeon_cs_reloc) / sizeof(__u32)) * bo_reference_count),
      .chunk_data = (__u64)bo_references,
   };

   struct drm_radeon_cs_chunk indirect_buffer_chunk = {
      .chunk_id = RADEON_CHUNK_ID_IB,
      .length_dw = indirect_buffer_size_dwords,
      .chunk_data = (__u64)(void const *)indirect_buffer,
   };

   struct drm_radeon_cs_chunk flags_chunk = {
      .chunk_id = RADEON_CHUNK_ID_FLAGS,
      .length_dw = ARRAY_SIZE(flags),
      .chunk_data = (__u64)(void const *)flags,
   };

   __u64 chunks[] = {
      (__u64)(void *)&relocations_chunk,
      (__u64)(void *)&indirect_buffer_chunk,
      (__u64)(void *)&flags_chunk,
   };

   struct drm_radeon_cs cs_arguments = {
      .num_chunks = ARRAY_SIZE(chunks),
      .chunks = (__u64)(void const *)chunks,
   };

   struct terakan_device_drm_radeon const * const device =
      container_of(device_base, struct terakan_device_drm_radeon const, base);

   int const cs_result = drmCommandWriteRead(device->render_node_fd, DRM_RADEON_CS, &cs_arguments,
                                             sizeof(cs_arguments));

   if (cs_result != 0) {
      if (cs_result == -ENOMEM) {
         vk_loge(VK_LOG_OBJS(device), "Not enough memory for command submission");
         return VK_ERROR_OUT_OF_HOST_MEMORY;
      }

      vk_loge(VK_LOG_OBJS(device),
              "The kernel has rejected the command submission with error number %d, see dmesg for "
              "more information",
              cs_result);
      if (debug_get_bool_option("TERAKAN_DUMP_CS", false)) {
         fputs("terakan/drm_radeon: Dumping the rejected command buffer...\n", stderr);
         for (uint32_t indirect_buffer_dword_index = 0;
              indirect_buffer_dword_index < indirect_buffer_size_dwords;
              ++indirect_buffer_dword_index) {
            fprintf(stderr, "0x%08" PRIX32 "\n", indirect_buffer[indirect_buffer_dword_index]);
         }
      }
      return VK_ERROR_UNKNOWN;
   }

   return VK_SUCCESS;
}

struct terakan_queue_completion_submission_drm_radeon {
   struct terakan_queue_completion_submission base;

   struct terakan_bo_drm_radeon * bo;
};

static VkResult
terakan_queue_completion_submission_drm_radeon_submit(
   struct terakan_queue_completion_submission * const submission_base)
{
   struct terakan_queue_completion_submission_drm_radeon const * const submission = container_of(
      submission_base, struct terakan_queue_completion_submission_drm_radeon const, base);
   struct terakan_device * const device =
      container_of(submission->base.queue->vk.base.device, struct terakan_device, vk);

   /* Make the BO not idle until the GPU has completed the submission. */
   void * const signal_bo_reference = alloca(device->bo_reference_size);
   device->winsys_fn->bo->create_reference(signal_bo_reference, &submission->bo->base, false, true,
                                           TERAKAN_BO_PRIORITY_FENCE_TRACE);
   uint32_t signal_indirect_buffer[8] = {
      [0] = PKT3(PKT3_EVENT_WRITE_EOP, 5 - 1, 0),
      /* TODO(Triang3l): Correct event type. */
      [1] = EVENT_INDEX(5) | EVENT_TYPE(EVENT_TYPE_CACHE_FLUSH_AND_INV_TS_EVENT),
      /* Lower address bits. */
      [2] = 0,
      /* Data selection, interrupt selection, and higher address bits. */
      [3] = EOP_DATA_SEL(EOP_DATA_SEL_VALUE_64BIT),
      /* [4], [5] - data (will be written with memcpy for correct endianness, the GPU write is
       * little-endian).
       */
      /* Relocation. */
      [6] = PKT3(PKT3_NOP, 1 - 1, 0),
      [7] = 0,
      /* GFX command buffers must be padded to a multiple of 8 dwords with NOPs, but this indirect
       * buffer is exactly 8 dwords long.
       */
   };
   memcpy(&signal_indirect_buffer[4], &submission->base.expected_payload, sizeof(uint64_t));
   return device->winsys_fn->queue->submit(device, submission->base.queue->ip_type, 1,
                                           signal_bo_reference, ARRAY_SIZE(signal_indirect_buffer),
                                           signal_indirect_buffer);
}

static bool
terakan_queue_completion_submission_drm_radeon_await(
   struct terakan_queue_completion_submission * const submission_base)
{
   struct terakan_queue_completion_submission_drm_radeon const * const submission = container_of(
      submission_base, struct terakan_queue_completion_submission_drm_radeon const, base);
   struct terakan_device_drm_radeon const * const device = container_of(
      submission->base.queue->vk.base.device, struct terakan_device_drm_radeon const, base.vk);
   struct terakan_bo_drm_radeon const * const bo = submission->bo;

   struct drm_radeon_gem_wait_idle gem_wait_idle_arguments = {
      .handle = bo->handle,
   };
   /* Returns -EBUSY in finite time in case of a hang (30-second timeout in Linux Radeon 2.50.0). */
   if (drmCommandWrite(device->render_node_fd, DRM_RADEON_GEM_WAIT_IDLE, &gem_wait_idle_arguments,
                       sizeof(gem_wait_idle_arguments)) != 0) {
      return false;
   }

   return *(uint64_t const volatile *)bo->base.mapping == submission->base.expected_payload;
}

static void
terakan_queue_completion_submission_drm_radeon_finish_winsys_and_free(
   struct terakan_queue_completion_submission * const submission_base)
{
   struct terakan_queue_completion_submission_drm_radeon * const submission =
      container_of(submission_base, struct terakan_queue_completion_submission_drm_radeon, base);

   terakan_bo_free(&submission->bo->base, NULL);

   vk_free(&submission->base.queue->vk.base.device->alloc, submission);
}

static VkResult
terakan_queue_completion_submission_drm_radeon_alloc_and_init_winsys(
   struct terakan_queue * const queue, uint64_t const initial_payload,
   struct terakan_queue_completion_submission ** const submission_out)
{
   VkResult result;

   struct terakan_device * const device =
      container_of(queue->vk.base.device, struct terakan_device, vk);

   struct terakan_queue_completion_submission_drm_radeon * const submission =
      vk_alloc(&device->vk.alloc, sizeof(struct terakan_queue_completion_submission_drm_radeon),
               alignof(struct terakan_queue_completion_submission_drm_radeon),
               VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (submission == NULL) {
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   struct terakan_bo * bo_base;
   result = device->winsys_fn->bo->allocate_device_memory(
      device, sizeof(uint64_t), alignof(uint64_t),
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 0, NULL,
      VK_SYSTEM_ALLOCATION_SCOPE_DEVICE, &bo_base);
   if (result != VK_SUCCESS) {
      vk_free(&device->vk.alloc, submission);
      return result;
   }
   submission->bo = container_of(bo_base, struct terakan_bo_drm_radeon, base);

   if (terakan_bo_map(bo_base) == NULL) {
      terakan_bo_free(bo_base, NULL);
      vk_free(&device->vk.alloc, submission);
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   *(uint64_t volatile *)bo_base->mapping = initial_payload;

   *submission_out = &submission->base;
   return VK_SUCCESS;
}

struct terakan_queue_winsys_fn const terakan_queue_drm_radeon_fn = {
   .submit = terakan_queue_drm_radeon_submit,
   .completion_submission_submit = terakan_queue_completion_submission_drm_radeon_submit,
   .completion_submission_await = terakan_queue_completion_submission_drm_radeon_await,
   .completion_submission_finish_winsys_and_free =
      terakan_queue_completion_submission_drm_radeon_finish_winsys_and_free,
   .completion_submission_alloc_and_init_winsys =
      terakan_queue_completion_submission_drm_radeon_alloc_and_init_winsys,
};
