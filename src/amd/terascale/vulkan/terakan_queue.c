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

#include "terakan_queue.h"

#include "terakan_command_buffer.h"
#include "terakan_device.h"
#include "terakan_physical_device.h"
#include "terakan_sync_completion.h"

#include "c11/threads.h"
#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600d_common.h"
#include "util/macros.h"
#include "c99_alloca.h"
#include "vk_alloc.h"
#include "vk_enum_to_str.h"
#include "vk_log.h"
#include "vk_sync.h"
#include "vk_sync_dummy.h"
#include "vk_synchronization.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static int
terakan_queue_completion_thread_func(void * queue_ptr)
{
   struct terakan_queue * const queue = queue_ptr;

   struct terakan_device * const device =
      container_of(queue->vk.base.device, struct terakan_device, vk);

   mtx_lock(&device->completion_mutex);
   while (true) {
      if (device->completion_lost) {
         mtx_unlock(&device->completion_mutex);
         return 1;
      }

      if (queue->shutdown_competion_thread) {
         /* Graceful device shutdown. */
         break;
      }

      if (list_is_empty(&queue->completion_submissions_pending)) {
         if (cnd_wait(&device->completion_condition, &device->completion_mutex) != thrd_success) {
            vk_device_set_lost(&device->vk, "Failed to await the submission condition variable");
            device->completion_lost = true;
            mtx_unlock(&device->completion_mutex);
            cnd_broadcast(&device->completion_condition);
            return 1;
         }
         continue;
      }

      /* Await the BO not to be potentially accessed by the GPU anymore without holding the
       * mutex.
       */
      struct terakan_queue_completion_submission * submission = list_first_entry(
         &queue->completion_submissions_pending, struct terakan_queue_completion_submission, link);
      list_del(&submission->link);
      mtx_unlock(&device->completion_mutex);
      bool const awaited = device->winsys_fn->queue->completion_submission_await(submission);
      mtx_lock(&device->completion_mutex);
      if (unlikely(!awaited)) {
         vk_device_set_lost(&device->vk, "Failed to await for the submission completion fence");
         device->completion_lost = true;
      } else {
         struct terakan_queue_completion_signal * signal;
         LIST_FOR_EACH_ENTRY (signal, &submission->signals, link) {
            assert(signal->value <= signal->sync->pending_value);
            assert(signal->value > signal->sync->current_value);
            signal->sync->current_value = signal->value;
         }
      }

      /* Recycle the submission. */
      list_splice(&submission->signals, &queue->completion_signals_free);
      list_inithead(&submission->signals);
      list_add(&submission->link, &queue->completion_submissions_free);

      /* Notify signal waits of new semaphore values or the failure. */
      cnd_broadcast(&device->completion_condition);
   }
   mtx_unlock(&device->completion_mutex);

   return 0;
}

#define TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS ((uint32_t)1 << 5)
static_assert(
   TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS >=
      TERAKAN_QUEUE_INDIRECT_BUFFER_SIZE_ALIGNMENT_DWORDS_GFX,
   "Signal indirect buffer size upper bound must be high enough to fit all GFX indirect buffer "
   "size alignment padding.");

static uint32_t
terakan_queue_get_graphics_signal_indirect_buffer(
   struct terakan_device const * const device, VkPipelineStageFlags2 const expanded_signal_stages,
   uint32_t indirect_buffer[TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS])
{
   uint32_t indirect_buffer_size_dwords = 0;

   /* Disable register shadowing before executing any packets that may set registers (not clear if
    * CP_COHER_CNTL setting in SURFACE_SYNC interacts with it, but for safety it's preferable to do
    * this for all submissions).
    */
   assert(TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS - indirect_buffer_size_dwords >= 3);
   indirect_buffer[indirect_buffer_size_dwords++] = PKT3(PKT3_CONTEXT_CONTROL, 1, 0);
   /* CC0_UPDATE_LOAD_ENABLES(1) */
   indirect_buffer[indirect_buffer_size_dwords++] = (uint32_t)1 << 31;
   /* CC1_UPDATE_SHADOW_ENABLES(1) */
   indirect_buffer[indirect_buffer_size_dwords++] = (uint32_t)1 << 31;

   uint32_t cp_coher_cntl_cb_db_dest_base_ena = 0;
   uint32_t cp_coher_cntl = 0;

   if (expanded_signal_stages & (VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                                 VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT)) {
      cp_coher_cntl_cb_db_dest_base_ena |= S_0085F0_DB_DEST_BASE_ENA(1);
      cp_coher_cntl |= S_0085F0_DB_ACTION_ENA(1);
   }
   if (expanded_signal_stages & ((device->vk.enabled_features.fragmentStoresAndAtomics
                                     ? VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT
                                     : 0) |
                                 VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT)) {
      /* Perform a full destination cache flush if UAVs need to be flushed because
       * FLUSH_AND_INV_CB_DATA_TS writes a timestamp and thus needs a BO.
       * Fence signals result in a full flush anyway, more granularity may only be useful for
       * semaphores.
       */
      cp_coher_cntl_cb_db_dest_base_ena |=
         S_0085F0_CB0_DEST_BASE_ENA(1) | S_0085F0_CB1_DEST_BASE_ENA(1) |
         S_0085F0_CB2_DEST_BASE_ENA(1) | S_0085F0_CB3_DEST_BASE_ENA(1) |
         S_0085F0_CB4_DEST_BASE_ENA(1) | S_0085F0_CB5_DEST_BASE_ENA(1) |
         S_0085F0_CB6_DEST_BASE_ENA(1) | S_0085F0_CB7_DEST_BASE_ENA(1) |
         S_0085F0_CB8_DEST_BASE_ENA(1) | S_0085F0_CB9_DEST_BASE_ENA(1) |
         S_0085F0_CB10_DEST_BASE_ENA(1) | S_0085F0_CB11_DEST_BASE_ENA(1);
      cp_coher_cntl |= S_0085F0_CB_ACTION_ENA(1) | S_0085F0_SMX_ACTION_ENA(1);
      assert(TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS - indirect_buffer_size_dwords >= 2);
      indirect_buffer[indirect_buffer_size_dwords++] = PKT3(PKT3_EVENT_WRITE, 1 - 1, 0);
      indirect_buffer[indirect_buffer_size_dwords++] =
         EVENT_TYPE(EVENT_TYPE_CACHE_FLUSH_AND_INV_EVENT) | EVENT_INDEX(0);
   } else {
      if (expanded_signal_stages &
          (VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_BLIT_BIT)) {
         cp_coher_cntl_cb_db_dest_base_ena |=
            S_0085F0_CB0_DEST_BASE_ENA(1) | S_0085F0_CB1_DEST_BASE_ENA(1) |
            S_0085F0_CB2_DEST_BASE_ENA(1) | S_0085F0_CB3_DEST_BASE_ENA(1) |
            S_0085F0_CB4_DEST_BASE_ENA(1) | S_0085F0_CB5_DEST_BASE_ENA(1) |
            S_0085F0_CB6_DEST_BASE_ENA(1) | S_0085F0_CB7_DEST_BASE_ENA(1);
         cp_coher_cntl |= S_0085F0_CB_ACTION_ENA(1);
         assert(TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS - indirect_buffer_size_dwords >=
                2 * 2);
         indirect_buffer[indirect_buffer_size_dwords++] = PKT3(PKT3_EVENT_WRITE, 1 - 1, 0);
         indirect_buffer[indirect_buffer_size_dwords++] =
            EVENT_TYPE(EVENT_TYPE_FLUSH_AND_INV_CB_PIXEL_DATA) | EVENT_INDEX(0);
         indirect_buffer[indirect_buffer_size_dwords++] = PKT3(PKT3_EVENT_WRITE, 1 - 1, 0);
         indirect_buffer[indirect_buffer_size_dwords++] =
            EVENT_TYPE(EVENT_TYPE_FLUSH_AND_INV_CB_META) | EVENT_INDEX(0);
      }
      if (expanded_signal_stages & (VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                                    VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT)) {
         /* CP_COHER_CNTL bits have already been set. */
         assert(TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS - indirect_buffer_size_dwords >= 2);
         indirect_buffer[indirect_buffer_size_dwords++] = PKT3(PKT3_EVENT_WRITE, 1 - 1, 0);
         indirect_buffer[indirect_buffer_size_dwords++] =
            EVENT_TYPE(EVENT_TYPE_DB_CACHE_FLUSH_AND_INV) | EVENT_INDEX(0);
      }
   }

   /* SURFACE_SYNC with any CB/DB_DEST_BASE_ENA implies PS_PARTIAL_FLUSH. */
   if (!cp_coher_cntl_cb_db_dest_base_ena) {
      if (expanded_signal_stages &
          (VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_BLIT_BIT)) {
         assert(TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS - indirect_buffer_size_dwords >= 2);
         indirect_buffer[indirect_buffer_size_dwords++] = PKT3(PKT3_EVENT_WRITE, 1 - 1, 0);
         indirect_buffer[indirect_buffer_size_dwords++] =
            EVENT_TYPE(EVENT_TYPE_PS_PARTIAL_FLUSH) | EVENT_INDEX(4);
      } else if (expanded_signal_stages &
                 (VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT | VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT |
                  VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT |
                  VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
                  VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT |
                  VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT |
                  VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT)) {
         assert(TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS - indirect_buffer_size_dwords >= 2);
         indirect_buffer[indirect_buffer_size_dwords++] = PKT3(PKT3_EVENT_WRITE, 1 - 1, 0);
         indirect_buffer[indirect_buffer_size_dwords++] =
            EVENT_TYPE(EVENT_TYPE_VS_PARTIAL_FLUSH) | EVENT_INDEX(4);
      }
   }

   if (expanded_signal_stages & VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT) {
      assert(TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS - indirect_buffer_size_dwords >= 2);
      indirect_buffer[indirect_buffer_size_dwords++] = PKT3(PKT3_EVENT_WRITE, 1 - 1, 0);
      indirect_buffer[indirect_buffer_size_dwords++] =
         EVENT_TYPE(EVENT_TYPE_CS_PARTIAL_FLUSH) | EVENT_INDEX(4);
   }

   /* VK_PIPELINE_STAGE_2_COPY_BIT is flushed in command buffer ending. */

   /* TODO(Triang3l): VK_PIPELINE_STAGE_2_CLEAR_BIT. */

   cp_coher_cntl |= cp_coher_cntl_cb_db_dest_base_ena;
   if (cp_coher_cntl) {
      assert(TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS - indirect_buffer_size_dwords >= 5);
      indirect_buffer[indirect_buffer_size_dwords++] = PKT3(PKT3_SURFACE_SYNC, 4 - 1, 0);
      /* In ME. */
      indirect_buffer[indirect_buffer_size_dwords++] = cp_coher_cntl | ((uint32_t)1 << 31);
      indirect_buffer[indirect_buffer_size_dwords++] = UINT32_MAX; /* CP_COHER_SIZE */
      indirect_buffer[indirect_buffer_size_dwords++] = 0;          /* CP_COHER_BASE */
      indirect_buffer[indirect_buffer_size_dwords++] = 10;         /* POLL_INTERVAL */
   }

   /* Pad the GFX ring indirect buffer to the size alignment requirement with NOPs, and also prevent
    * the submission from being empty as it's still needed for the completion signal, but an empty
    * one may be rejected by the winsys.
    */
   while ((indirect_buffer_size_dwords &
           (TERAKAN_QUEUE_INDIRECT_BUFFER_SIZE_ALIGNMENT_DWORDS_GFX - 1)) ||
          indirect_buffer_size_dwords == 0) {
      assert(TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS - indirect_buffer_size_dwords >= 1);
      indirect_buffer[indirect_buffer_size_dwords++] = PKT_TYPE_S(2);
   }

   return indirect_buffer_size_dwords;
}

static VkResult
terakan_queue_submit(struct vk_queue * const queue_base, struct vk_queue_submit * const submit)
{
   struct terakan_queue * const queue = container_of(queue_base, struct terakan_queue, vk);

   struct terakan_device * const device =
      container_of(queue->vk.base.device, struct terakan_device, vk);

   /* Submit the command buffers. */

   for (uint32_t command_buffer_index = 0; command_buffer_index < submit->command_buffer_count;
        ++command_buffer_index) {
      struct terakan_command_buffer const * const command_buffer = container_of(
         submit->command_buffers[command_buffer_index], struct terakan_command_buffer const, vk);
      list_for_each_entry (struct terakan_command_buffer_indirect_buffer,
                           command_buffer_indirect_buffer, &command_buffer->indirect_buffers,
                           link) {
         /* The winsys may not support empty indirect buffers. */
         assert(command_buffer_indirect_buffer->indirect_buffer_size_dwords != 0);
         VkResult const command_buffer_submit_result = device->winsys_fn->queue->submit(
            queue->submission_context, command_buffer_indirect_buffer->bo_reference_count,
            command_buffer_indirect_buffer->bo_references,
            command_buffer_indirect_buffer->indirect_buffer_size_dwords,
            command_buffer_indirect_buffer->indirect_buffer,
            command_buffer_indirect_buffer->relocation_count,
            command_buffer_indirect_buffer->relocations);
         if (command_buffer_submit_result != VK_SUCCESS) {
            /* Lose the device as the submission might have been done partially already, don't leave
             * it in an indeterminate state.
             */
            vk_device_set_lost(&device->vk, "Command buffer submission failed with result %s",
                               vk_Result_to_str(command_buffer_submit_result));
            mtx_lock(&device->completion_mutex);
            device->completion_lost = true;
            mtx_unlock(&device->completion_mutex);
            cnd_broadcast(&device->completion_condition);
            return VK_ERROR_DEVICE_LOST;
         }
      }
   }

   /* If there are semaphores to signal from this submission, signal the fence BO to await the
    * completion from the completion thread. */

   /* Construct the list of the timeline semaphores that need to be signaled, and gather the stages
    * for the dependency.
    */
   VkPipelineStageFlags2 signal_stages = 0;
   struct list_head completion_signals;
   list_inithead(&completion_signals);
   for (uint32_t submit_signal_index = 0; submit_signal_index < submit->signal_count;
        ++submit_signal_index) {
      struct vk_sync_signal const * const submit_signal = &submit->signals[submit_signal_index];
      if (submit_signal->sync->type == &vk_sync_dummy_type) {
         continue;
      }
      signal_stages |= submit_signal->stage_mask;
      assert(submit_signal->sync->type == &terakan_sync_completion_type);
      struct terakan_queue_completion_signal * completion_signal;
      mtx_lock(&device->completion_mutex);
      if (!list_is_empty(&queue->completion_signals_free)) {
         completion_signal = list_first_entry(&queue->completion_signals_free,
                                              struct terakan_queue_completion_signal, link);
         list_del(&completion_signal->link);
         mtx_unlock(&device->completion_mutex);
      } else {
         mtx_unlock(&device->completion_mutex);
         completion_signal = vk_alloc(
            &device->vk.alloc, sizeof(struct terakan_queue_completion_signal),
            alignof(struct terakan_queue_completion_signal), VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
         if (completion_signal == NULL) {
            /* Lose the device as the submission has been done partially already, don't leave it in
             * an indeterminate state.
             */
            vk_device_set_lost(&device->vk,
                               "Failed to allocate memory for a submission completion signal");
            mtx_lock(&device->completion_mutex);
            list_splice(&completion_signals, &queue->completion_signals_free);
            device->completion_lost = true;
            mtx_unlock(&device->completion_mutex);
            cnd_broadcast(&device->completion_condition);
            return VK_ERROR_DEVICE_LOST;
         }
      }
      completion_signal->sync =
         container_of(submit_signal->sync, struct terakan_sync_completion, vk);
      completion_signal->value = submit_signal->signal_value;
      list_add(&completion_signal->link, &completion_signals);
   }
   if (list_is_empty(&completion_signals)) {
      /* Nothing to signal. */
      return VK_SUCCESS;
   }

   /* Set up the completion fence. */
   struct terakan_queue_completion_submission * completion_submission;
   mtx_lock(&device->completion_mutex);
   if (!list_is_empty(&queue->completion_submissions_free)) {
      completion_submission = list_first_entry(&queue->completion_submissions_free,
                                               struct terakan_queue_completion_submission, link);
      list_del(&completion_submission->link);
      mtx_unlock(&device->completion_mutex);
   } else {
      mtx_unlock(&device->completion_mutex);
      VkResult const completion_submission_create_result =
         device->winsys_fn->queue->completion_submission_alloc_and_init_winsys(
            queue, &completion_submission);
      if (completion_submission_create_result != VK_SUCCESS) {
         /* Lose the device as the submission has been done partially already, don't leave it in an
          * indeterminate state.
          */
         vk_device_set_lost(&device->vk,
                            "Submission completion fence creation failed with result %s",
                            vk_Result_to_str(completion_submission_create_result));
         mtx_lock(&device->completion_mutex);
         list_splice(&completion_signals, &queue->completion_signals_free);
         device->completion_lost = true;
         mtx_unlock(&device->completion_mutex);
         cnd_broadcast(&device->completion_condition);
         return VK_ERROR_DEVICE_LOST;
      }
      completion_submission->queue = queue;
   }
   list_replace(&completion_signals, &completion_submission->signals);

   /* Section 7.4.1. "Semaphore Signaling" of the Vulkan 1.3.277 specification says:
    *
    *     "When a batch is submitted to a queue via a queue submission, and it includes semaphores
    *     to be signaled, it defines a memory dependency on the batch, and defines semaphore signal
    *     operations which set the semaphores to the signaled state."
    *
    *     "The first synchronization scope includes every command submitted in the same batch. In
    *     the case of vkQueueSubmit2, the first synchronization scope is limited to the pipeline
    *     stage specified by VkSemaphoreSubmitInfo::stageMask. Semaphore signal operations that are
    *     defined by vkQueueSubmit or vkQueueSubmit2 additionally include all commands that occur
    *     earlier in submission order."
    *
    *     "The first access scope includes all memory access performed by the device."
    *
    * Make sure all writes and reads in the first synchronization scope are complete to prevent all
    * types of data hazards, and flush write caches to make written memory available.
    */
   signal_stages = vk_expand_src_stage_flags2(signal_stages);
   uint32_t signal_indirect_buffer[TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS];
   uint32_t const signal_indirect_buffer_size_dwords =
      terakan_queue_get_graphics_signal_indirect_buffer(device, signal_stages,
                                                        signal_indirect_buffer);
   assert(signal_indirect_buffer_size_dwords != 0);

   /* Submit the memory dependency packets and a signal of the completion fence. */
   VkResult const completion_submission_submit_result =
      device->winsys_fn->queue->completion_submission_submit(
         completion_submission, signal_indirect_buffer_size_dwords, signal_indirect_buffer);
   if (completion_submission_submit_result != VK_SUCCESS) {
      /* Lose the device regardless of the actual result for this specific command buffer because a
       * part of the queue submission might have already been done, don't leave the device in an
       * indeterminate state.
       */
      vk_device_set_lost(&device->vk,
                         "Submission completion fence signal submission failed with result %s",
                         vk_Result_to_str(completion_submission_submit_result));
      mtx_lock(&device->completion_mutex);
      list_splice(&completion_submission->signals, &queue->completion_signals_free);
      list_inithead(&completion_submission->signals);
      list_add(&completion_submission->link, &queue->completion_submissions_free);
      device->completion_lost = true;
      mtx_unlock(&device->completion_mutex);
      cnd_broadcast(&device->completion_condition);
      return VK_ERROR_DEVICE_LOST;
   }

   /* Update the pending values of the signaled semaphores once the kernel has received the
    * submission, make the submission completion awaiting thread await the completion of the new
    * submission, and wake pending signal waiting threads and the submission completion awaiting
    * thread.
    */
   mtx_lock(&device->completion_mutex);
   for (uint32_t submit_signal_index = 0; submit_signal_index < submit->signal_count;
        ++submit_signal_index) {
      struct vk_sync_signal const * const submit_signal = &submit->signals[submit_signal_index];
      if (submit_signal->sync->type == &vk_sync_dummy_type) {
         continue;
      }
      assert(submit_signal->sync->type == &terakan_sync_completion_type);
      struct terakan_sync_completion * const submit_signal_sync =
         container_of(submit_signal->sync, struct terakan_sync_completion, vk);
      assert(submit_signal_sync->pending_value < submit_signal->signal_value);
      submit_signal_sync->pending_value = submit_signal->signal_value;
   }
   list_addtail(&completion_submission->link, &queue->completion_submissions_pending);
   mtx_unlock(&device->completion_mutex);
   cnd_broadcast(&device->completion_condition);

   return VK_SUCCESS;
}

void
terakan_queue_destroy(struct terakan_queue * const queue)
{
   struct terakan_device * const device =
      container_of(queue->vk.base.device, struct terakan_device, vk);

   mtx_lock(&device->completion_mutex);
   queue->shutdown_competion_thread = true;
   mtx_unlock(&device->completion_mutex);
   cnd_broadcast(&device->completion_condition);

   thrd_join(queue->completion_thread, NULL);

   struct terakan_queue_completion_submission *completion_submission, *next_submission;
   struct terakan_queue_completion_signal *completion_signal, *next_signal;
   LIST_FOR_EACH_ENTRY_SAFE (completion_submission, next_submission,
                             &queue->completion_submissions_pending, link) {
      LIST_FOR_EACH_ENTRY_SAFE (completion_signal, next_signal, &completion_submission->signals,
                                link) {
         vk_free(&device->vk.alloc, completion_signal);
      }
      device->winsys_fn->queue->completion_submission_finish_winsys_and_free(completion_submission);
   }
   LIST_FOR_EACH_ENTRY_SAFE (completion_submission, next_submission,
                             &queue->completion_submissions_free, link) {
      device->winsys_fn->queue->completion_submission_finish_winsys_and_free(completion_submission);
   }
   LIST_FOR_EACH_ENTRY_SAFE (completion_signal, next_signal, &queue->completion_signals_free,
                             link) {
      vk_free(&device->vk.alloc, completion_signal);
   }

   device->winsys_fn->queue->release_submission_context(queue->submission_context);

   vk_queue_finish(&queue->vk);

   vk_free(&queue->vk.base.device->alloc, queue);
}

VkResult
terakan_queue_create(struct terakan_device * const device,
                     VkDeviceQueueCreateInfo const * const create_info,
                     uint32_t const index_in_family, struct terakan_queue ** const queue_out)
{
   VkResult result;

   struct terakan_queue * const queue =
      vk_alloc(&device->vk.alloc, sizeof(struct terakan_queue), alignof(struct terakan_queue),
               VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (queue == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   result = vk_queue_init(&queue->vk, &device->vk, create_info, index_in_family);
   if (result != VK_SUCCESS) {
      goto fail_alloc;
   }

   struct terakan_physical_device const * const physical_device =
      terakan_device_physical_device(device);

   struct terakan_queue_submission_size desired_submission_size =
      terakan_command_buffer_optimal_submission_size_gfx(
         &physical_device->submission_info_gfx.base);
   desired_submission_size.indirect_buffer_dwords =
      MAX2(desired_submission_size.indirect_buffer_dwords,
           TERAKAN_QUEUE_SIGNAL_INDIRECT_BUFFER_MAX_DWORDS);

   result = device->winsys_fn->queue->acquire_submission_context(
      device, AMD_IP_GFX, desired_submission_size, &queue->submission_context);
   if (result != VK_SUCCESS) {
      result = vk_error(device, result);
      goto fail_queue;
   }

   list_inithead(&queue->completion_signals_free);
   list_inithead(&queue->completion_submissions_free);

   list_inithead(&queue->completion_submissions_pending);

   queue->shutdown_competion_thread = false;

   if (thrd_create(&queue->completion_thread, terakan_queue_completion_thread_func, queue) !=
       thrd_success) {
      result = vk_errorf(device, VK_ERROR_OUT_OF_HOST_MEMORY,
                         "Failed to create the submission completion thread");
      goto fail_submission_context;
   }

   queue->vk.driver_submit = terakan_queue_submit;

   *queue_out = queue;
   return VK_SUCCESS;

fail_submission_context:
   device->winsys_fn->queue->release_submission_context(queue->submission_context);
fail_queue:
   vk_queue_finish(&queue->vk);
fail_alloc:
   vk_free(&device->vk.alloc, queue);
   return result;
}
