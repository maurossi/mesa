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

#ifndef TERAKAN_QUEUE_H
#define TERAKAN_QUEUE_H

#include "terakan_command_buffer.h"
#include "terakan_sync_completion.h"

#include "c11/threads.h"
#include "util/list.h"
#include "amd_family.h"
#include "vk_queue.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_queue_completion_signal {
   struct list_head link;

   struct terakan_sync_completion * sync;
   uint64_t value;
};

struct terakan_queue;

/* Partially implemented by the winsys. */
struct terakan_queue_completion_submission {
   struct terakan_queue * queue;

   struct list_head link;

   uint64_t expected_payload;

   struct list_head signals;
};

struct terakan_queue {
   struct vk_queue vk;

   enum amd_ip_type ip_type;

   /* Completion free lists protected by completion_mutex of the device. */
   struct list_head completion_signals_free;
   struct list_head completion_submissions_free;

   /* Protected by completion_mutex of the device, broadcast completion_condition after adding. */
   struct list_head completion_submissions_pending;

   /* Protected by completion_mutex of the device, broadcast completion_condition after setting. */
   bool shutdown_competion_thread;

   /* Thread awaiting completion of submissions that perform any signals, to signal timeline
    * semaphores, and let dependent submissions and CPU threads continue.
    * Note that GPU waits are implemented by deferring the dependent submissions on the CPU.
    * WAIT_REG_MEM doing wait-before-signal must never be submitted, because it can hang the GPU if,
    * for instance, the process that is supposed to do the signal is killed.
    */
   thrd_t completion_thread;

   /* Accessed by vkQueueSubmit, the next value to write to a winsys-specific submission completion
    * fence from the GPU, used for verifying that the submission has been executed successfully.
    */
   uint64_t next_completion_payload;
};

VK_DEFINE_HANDLE_CASTS(terakan_queue, vk.base, VkQueue, VK_OBJECT_TYPE_QUEUE)

void terakan_queue_destroy(struct terakan_queue * queue);

struct terakan_device;

VkResult terakan_queue_create(struct terakan_device * device,
                              VkDeviceQueueCreateInfo const * create_info, uint32_t index_in_family,
                              struct terakan_queue ** queue_out);

struct terakan_queue_winsys_fn {
   /* Not exposing queue priorities as the Linux Radeon 2.50.0 kernel driver only provides a
    * high-priority DMA ring on R9xx.
    * On failure, returns VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY or
    * VK_ERROR_UNKNOWN.
    */
   VkResult (*submit)(struct terakan_device * device, enum amd_ip_type ip_type,
                      uint32_t bo_reference_count, void const * bo_references,
                      uint32_t indirect_buffer_size_dwords, uint32_t const * indirect_buffer);

   /* On failure, returns VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY or
    * VK_ERROR_UNKNOWN.
    */
   VkResult (*completion_submission_submit)(struct terakan_queue_completion_submission * submission);
   /* Returns whether the wait was successful. In case of a GPU hang, must return in finite time. */
   bool (*completion_submission_await)(struct terakan_queue_completion_submission * submission);
   void (*completion_submission_finish_winsys_and_free)(
      struct terakan_queue_completion_submission * submission);
   /* On failure, returns VK_ERROR_OUT_OF_HOST_MEMORY or VK_ERROR_OUT_OF_DEVICE_MEMORY. */
   VkResult (*completion_submission_alloc_and_init_winsys)(
      struct terakan_queue * queue, uint64_t initial_payload,
      struct terakan_queue_completion_submission ** submission_out);
};

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_QUEUE_H */
