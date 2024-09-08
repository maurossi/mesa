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

#include "terakan_queue.h"

#include "winsys/terakan_winsys.h"
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
   struct terakan_winsys_bo_fn const * const bo_fn =
      container_of(device->vk.physical, struct terakan_physical_device const, vk)->winsys->bo_fn;

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
      bool const awaited =
         bo_fn->wait_idle(submission->bo) &&
         *(uint64_t const volatile *)submission->bo->mapping == submission->expected_bo_data;
      mtx_lock(&device->completion_mutex);
      if (unlikely(!awaited)) {
         vk_device_set_lost(
            &device->vk,
            "Failed to wait for the synchronization buffer object to become idle and to contain "
            "the expected value");
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

static VkResult
terakan_queue_submit(struct vk_queue * const queue_base, struct vk_queue_submit * const submit)
{
   struct terakan_queue * const queue = container_of(queue_base, struct terakan_queue, vk);

   struct terakan_device * const device =
      container_of(queue->vk.base.device, struct terakan_device, vk);
   struct terakan_winsys * const winsys =
      container_of(device->vk.physical, struct terakan_physical_device const, vk)->winsys;

   /* Submit the command buffers. */

   /* TODO(Triang3l): Implement inheritance in secondary command buffers.
    *
    * Instead of writing dwords with the real register values and relocations to the secondary
    * indirect buffers for inherited bindings, write dword-sized substitution tokens in place of the
    * real indirect buffer dwords that form a linked list inside the indirect buffer submission.
    *
    * A substitution token should contain:
    * - The value of which field (including relocations) of which view the dword should be replaced
    *   with.
    * - The attachment number within the subpass the value needs to be taken from (for color
    *   targets, it may be different from the CB_COLOR# index due to MRT and RAT indices being
    *   compacted skipping those not used in the fragment shader).
    * - Offset to the next substitution token. It can be stored in 16 bits since 2^16 dwords is
    *   essentially the maximum indirect buffer size on Linux Radeon 2.50.0 (when using virtual
    *   memory), and in this case to make sure the dword 0xFFFF can be addressed too, an offset
    *   relative to the current dword can be stored, so that 0 will be the terminator.
    *
    * During queue submission, the secondary indirect buffer should be copied to a temporary
    * indirect buffer in the queue (take threaded submission in Mesa into account though), and
    * references to inherited BOs also need to be added to the submission.
    *
    * It must be guaranteed that during submission, there will be free space for all the attachment
    * references in the BO list - either more space needs to be allocated in the temporary BO list,
    * or some needs to be reserved in the secondary indirect buffer submission (however, BO
    * references from the secondary command buffer still must be copied into the temporary buffer,
    * because reading / writing flags and priorities of _existing_ BO references in the secondary
    * command buffer may be touched too by the execution depending on whether the secondary indirect
    * buffer already references something that's inherited but possibly in a different way, and that
    * depends on the location where it's executed, and in general a secondary command buffer can be
    * executed in any queue and thus without external synchronization, it just happens that at the
    * moment this comment is written Terakan has only one queue per type).
    *
    * Because some BOs that may be inherited at the execution location may also happen to be
    * referenced by the secondary indirect buffer itself directly and possibly with different
    * reading / writing flags and priority, the BO reference hash map also needs to be preserved or
    * reconstructed for secondary indirect buffers referencing any inherited BOs.
    *
    * Note that if virtual memory is used, there won't be relocations, so BO references need to be
    * created not only by substitution tokens for relocations, but also by tokens that will be
    * replaced with virtual addresses.
    */

   for (uint32_t command_buffer_index = 0; command_buffer_index < submit->command_buffer_count;
        ++command_buffer_index) {
      struct terakan_command_buffer const * const command_buffer = container_of(
         submit->command_buffers[command_buffer_index], struct terakan_command_buffer const, vk);
      list_for_each_entry(struct terakan_command_buffer_submission, command_buffer_submission_base,
                          &command_buffer->submissions, command_buffer_submission_link)
      {
         struct terakan_command_buffer_submission_indirect_buffer const *
            command_buffer_indirect_buffer;
         if (command_buffer_submission_base->is_secondary_execution) {
            struct terakan_command_buffer_submission_secondary_execution const *
               command_buffer_submission = container_of(
                  command_buffer_submission_base,
                  struct terakan_command_buffer_submission_secondary_execution const, base);
            command_buffer_indirect_buffer = command_buffer_submission->indirect_buffer;
         } else {
            command_buffer_indirect_buffer =
               container_of(command_buffer_submission_base,
                            struct terakan_command_buffer_submission_indirect_buffer const, base);
         }
         /* TODO(Triang3l): End of frame flag. */
         VkResult const command_buffer_submit_result = winsys->cs_fn->submit(
            winsys, queue->ip_type, command_buffer_indirect_buffer->bo_reference_count,
            command_buffer_indirect_buffer->bo_references,
            command_buffer_indirect_buffer->indirect_buffer_size_dwords,
            command_buffer_indirect_buffer->indirect_buffer, false);
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

   /* Construct the list of the timeline semaphores that need to be signaled. */
   struct list_head completion_signals;
   list_inithead(&completion_signals);
   for (uint32_t submit_signal_index = 0; submit_signal_index < submit->signal_count;
        ++submit_signal_index) {
      struct vk_sync_signal const * const submit_signal = &submit->signals[submit_signal_index];
      if (submit_signal->sync->type == &vk_sync_dummy_type) {
         continue;
      }
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

   /* Set up the fence BO. */
   struct terakan_queue_completion_submission * completion_submission;
   mtx_lock(&device->completion_mutex);
   if (!list_is_empty(&queue->completion_submissions_free)) {
      completion_submission = list_first_entry(&queue->completion_submissions_free,
                                               struct terakan_queue_completion_submission, link);
      list_del(&completion_submission->link);
      mtx_unlock(&device->completion_mutex);
   } else {
      mtx_unlock(&device->completion_mutex);
      completion_submission = vk_alloc(
         &device->vk.alloc, sizeof(struct terakan_queue_completion_submission),
         alignof(struct terakan_queue_completion_submission), VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
      if (completion_submission == NULL) {
         /* Lose the device as the submission has been done partially already, don't leave it in an
          * indeterminate state.
          */
         vk_device_set_lost(&device->vk,
                            "Failed to allocate memory for a submission completion submission");
         mtx_lock(&device->completion_mutex);
         list_splice(&completion_signals, &queue->completion_signals_free);
         device->completion_lost = true;
         mtx_unlock(&device->completion_mutex);
         cnd_broadcast(&device->completion_condition);
         return VK_ERROR_DEVICE_LOST;
      }
      completion_submission->bo = winsys->bo_fn->allocate_device_memory(
         winsys, sizeof(uint64_t), sizeof(uint64_t),
         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 0);
      if (completion_submission->bo == NULL) {
         vk_device_set_lost(&device->vk,
                            "Failed to create the submission completion fence buffer object");
         vk_free(&device->vk.alloc, completion_submission);
         mtx_lock(&device->completion_mutex);
         list_splice(&completion_signals, &queue->completion_signals_free);
         device->completion_lost = true;
         mtx_unlock(&device->completion_mutex);
         cnd_broadcast(&device->completion_condition);
         return VK_ERROR_DEVICE_LOST;
      }
      if (terakan_winsys_bo_map(completion_submission->bo) == NULL) {
         vk_device_set_lost(&device->vk,
                            "Failed to map the submission completion fence buffer object");
         terakan_winsys_bo_free(completion_submission->bo);
         vk_free(&device->vk.alloc, completion_submission);
         mtx_lock(&device->completion_mutex);
         list_splice(&completion_signals, &queue->completion_signals_free);
         device->completion_lost = true;
         mtx_unlock(&device->completion_mutex);
         cnd_broadcast(&device->completion_condition);
         return VK_ERROR_DEVICE_LOST;
      }
      *(uint64_t volatile *)completion_submission->bo->mapping = queue->next_completion_bo_data - 1;
   }
   completion_submission->expected_bo_data = queue->next_completion_bo_data;
   list_replace(&completion_signals, &completion_submission->signals);

   /* Submit a signal to the fence BO, making that BO not idle until the GPU has completed the
    * submission.
    */
   void * const signal_bo_reference = alloca(winsys->gpu_info.cs_bo_reference_size);
   winsys->cs_fn->create_bo_reference(signal_bo_reference, completion_submission->bo, false, true,
                                      TERAKAN_WINSYS_CS_BO_PRIORITY_FENCE_TRACE);
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
   memcpy(&signal_indirect_buffer[4], &queue->next_completion_bo_data,
          sizeof(queue->next_completion_bo_data));
   ++queue->next_completion_bo_data;
   VkResult const signal_submit_result =
      winsys->cs_fn->submit(winsys, queue->ip_type, 1, signal_bo_reference,
                            ARRAY_SIZE(signal_indirect_buffer), signal_indirect_buffer, false);
   if (signal_submit_result != VK_SUCCESS) {
      /* Lose the device regardless of the actual result for this specific command buffer because a
       * part of the queue submission might have already been done, don't leave the device in an
       * indeterminate state.
       */
      vk_device_set_lost(&device->vk,
                         "Synchronization signal command buffer submission failed with result %s",
                         vk_Result_to_str(signal_submit_result));
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
      terakan_winsys_bo_free(completion_submission->bo);
      vk_free(&device->vk.alloc, completion_submission);
   }
   LIST_FOR_EACH_ENTRY_SAFE (completion_submission, next_submission,
                             &queue->completion_submissions_free, link) {
      terakan_winsys_bo_free(completion_submission->bo);
      vk_free(&device->vk.alloc, completion_submission);
   }
   LIST_FOR_EACH_ENTRY_SAFE (completion_signal, next_signal, &queue->completion_signals_free,
                             link) {
      vk_free(&device->vk.alloc, completion_signal);
   }

   vk_queue_finish(&queue->vk);

   vk_free(&device->vk.alloc, queue);
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

   queue->ip_type = AMD_IP_GFX;

   list_inithead(&queue->completion_signals_free);
   list_inithead(&queue->completion_submissions_free);

   list_inithead(&queue->completion_submissions_pending);

   queue->shutdown_competion_thread = false;

   if (thrd_create(&queue->completion_thread, terakan_queue_completion_thread_func, queue) !=
       thrd_success) {
      result = vk_errorf(device, VK_ERROR_OUT_OF_HOST_MEMORY,
                         "Failed to create the submission completion thread");
      goto fail_queue;
   }

   /* Start from 1 to distinguish from the zero that might have potentially been used to initialize
    * the contents of a new BO.
    */
   queue->next_completion_bo_data = 1;

   queue->vk.driver_submit = terakan_queue_submit;

   *queue_out = queue;

   return VK_SUCCESS;

fail_queue:
   vk_queue_finish(&queue->vk);
fail_alloc:
   vk_free(&device->vk.alloc, queue);
   return result;
}
