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

#ifndef TERAKAN_QUEUE_H
#define TERAKAN_QUEUE_H

#include "terakan_bo.h"
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

/* CP fetch requires 8 dword alignment according to the Gallium Radeon winsys, at least for DRM
 * Radeon indirect buffers need to be padded to this with type-2 NOP packets. AMD's WDDM driver
 * doesn't seem to require this though according to the command buffer sizes in Direct3D 11
 * submissions.
 */
#define TERAKAN_QUEUE_INDIRECT_BUFFER_SIZE_ALIGNMENT_DWORDS_GFX 8

/* Matches the D3DDDI_PATCHLOCATIONLIST structure. */
struct terakan_queue_relocation_wddm_patch {
   uint32_t allocation_index;
   /* 24-bit, upper 8 bits are reserved and must be 0. */
   uint32_t slot_id;
   /* In AMD's driver, generally:
    * - 0x0 for 256-aligned addresses and the upper 8 bits.
    * - 0x800 for the lower 32 bits of a 40-bit address.
    * Some (slot ID `0x15 << 10`, or 0x5400) are seen with the driver ID 0x300, pointing to some
    * type-2 NOP packet followed by some type-3 command packets enclosed in the body of a type-3
    * NOP, with the allocation offset being 0.
    */
   uint32_t driver_id;
   /* In AMD's driver, generally the entire 32 bits patch_offset points to. */
   uint32_t allocation_offset;
   /* In bytes. */
   uint32_t patch_offset;
   /* 0 in AMD's driver. */
   uint32_t split_offset;
};

enum terakan_queue_relocation_type {
   TERAKAN_QUEUE_RELOCATION_TYPE_NONE,
   /* NOP packets containing the BO reference offset in dwords after the packets containing the
    * relative address.
    */
   TERAKAN_QUEUE_RELOCATION_TYPE_DRM_NOP,
   /* Array of `terakan_queue_relocation_wddm_patch`. */
   TERAKAN_QUEUE_RELOCATION_TYPE_WDDM_PATCH,
};

struct terakan_queue_submission_size {
   uint32_t bo_references;
   uint32_t indirect_buffer_dwords;
   uint32_t relocations;
};

static inline struct terakan_queue_submission_size
terakan_queue_submission_size_add(struct terakan_queue_submission_size const a,
                                  struct terakan_queue_submission_size const b)
{
   struct terakan_queue_submission_size submission_size;
   submission_size.bo_references = a.bo_references + b.bo_references;
   submission_size.indirect_buffer_dwords = a.indirect_buffer_dwords + b.indirect_buffer_dwords;
   submission_size.relocations = a.relocations + b.relocations;
   return submission_size;
}

/* Primarily for reserving space for winsys-specific data.
 * The subtraction is saturating primarily to handle the case when virtual memory is used and thus
 * relocations aren't needed for the commands themselves, thus the optimal relocation count would be
 * 0, but the winsys still passes some data via relocations internally.
 */
static inline struct terakan_queue_submission_size
terakan_queue_submission_size_subtract_saturating(struct terakan_queue_submission_size const a,
                                                  struct terakan_queue_submission_size const b)
{
   struct terakan_queue_submission_size submission_size;
   submission_size.bo_references = MAX2(a.bo_references, b.bo_references) - b.bo_references;
   submission_size.indirect_buffer_dwords =
      MAX2(a.indirect_buffer_dwords, b.indirect_buffer_dwords) - b.indirect_buffer_dwords;
   submission_size.relocations = MAX2(a.relocations, b.relocations) - b.relocations;
   return submission_size;
}

static inline struct terakan_queue_submission_size
terakan_queue_submission_size_min(struct terakan_queue_submission_size const a,
                                  struct terakan_queue_submission_size const b)
{
   struct terakan_queue_submission_size submission_size;
   submission_size.bo_references = MIN2(a.bo_references, b.bo_references);
   submission_size.indirect_buffer_dwords =
      MIN2(a.indirect_buffer_dwords, b.indirect_buffer_dwords);
   submission_size.relocations = MIN2(a.relocations, b.relocations);
   return submission_size;
}

/* Partially implemented by the winsys. */
struct terakan_queue_submission_context {
   struct terakan_queue_submission_size max_submission_size;
};

static inline void
terakan_queue_submission_context_init(
   struct terakan_queue_submission_context * const submission_context,
   struct terakan_queue_submission_size const max_submission_size)
{
   submission_context->max_submission_size = max_submission_size;
}

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

   struct list_head signals;
};

struct terakan_queue {
   struct vk_queue vk;

   struct terakan_queue_submission_context * submission_context;

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
};

VK_DEFINE_HANDLE_CASTS(terakan_queue, vk.base, VkQueue, VK_OBJECT_TYPE_QUEUE)

void terakan_queue_destroy(struct terakan_queue * queue);

struct terakan_device;

VkResult terakan_queue_create(struct terakan_device * device,
                              VkDeviceQueueCreateInfo const * create_info, uint32_t index_in_family,
                              struct terakan_queue ** queue_out);

struct terakan_queue_winsys_fn {
   /* BO references in command submissions are winsys-specific objects whose size and alignment are
    * terakan_device::bo_reference_size/alignment.
    */
   void (*create_bo_reference)(void * bo_reference, struct terakan_bo const * bo, bool is_reading,
                               bool is_writing, enum terakan_bo_priority priority);
   void (*update_bo_reference)(void * bo_reference, struct terakan_bo const * bo, bool is_reading,
                               bool is_writing, enum terakan_bo_priority priority);

   void (*release_submission_context)(struct terakan_queue_submission_context * submission_context);
   /* Depending on the winsys implementation, the returned pointer may be not unique, hence
    * acquire / release rather than create / destroy.
    *
    * The actual maximum submission size for the created context can end up being arbitrarily large,
    * including smaller than the desired size, or values like UINT32_MAX (the implementation though
    * should internally request a size that would be sufficient not only for command buffer
    * submissions, but also for completion submission signals).
    *
    * In general a submission context is not free-threaded (shares the external synchronization
    * requirement with the queue), so if the winsys shares a submission context between queues,
    * thread safety must be ensured inside the implementation.
    *
    * Not exposing queue priorities as the DRM Radeon 2.50.0 kernel driver only provides a
    * high-priority DMA ring on R9xx.
    *
    * On failure, returns one of the vkCreateDevice error codes.
    */
   VkResult (*acquire_submission_context)(
      struct terakan_device * device, enum amd_ip_type ip_type,
      struct terakan_queue_submission_size desired_submission_size,
      struct terakan_queue_submission_context ** submission_context_out);

   /* The indirect buffer must not be empty.
    * On failure, returns VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY or
    * VK_ERROR_UNKNOWN.
    */
   VkResult (*submit)(struct terakan_queue_submission_context * submission_context,
                      uint32_t bo_reference_count, void const * bo_references,
                      uint32_t indirect_buffer_size_dwords, uint32_t const * indirect_buffer,
                      uint32_t relocation_count, void const * relocations);

   /* Submits the sync indirect buffer and enqueues the completion signal.
    * On failure, returns VK_ERROR_OUT_OF_HOST_MEMORY, VK_ERROR_OUT_OF_DEVICE_MEMORY or
    * VK_ERROR_UNKNOWN.
    */
   VkResult (*completion_submission_submit)(struct terakan_queue_completion_submission * submission,
                                            uint32_t signal_indirect_buffer_size_dwords,
                                            uint32_t const * signal_indirect_buffer);
   /* Returns whether the wait was successful. In case of a GPU hang, must return in finite time. */
   bool (*completion_submission_await)(struct terakan_queue_completion_submission * submission);
   void (*completion_submission_finish_winsys_and_free)(
      struct terakan_queue_completion_submission * submission);
   /* On failure, returns VK_ERROR_OUT_OF_HOST_MEMORY or VK_ERROR_OUT_OF_DEVICE_MEMORY. */
   VkResult (*completion_submission_alloc_and_init_winsys)(
      struct terakan_queue * queue, struct terakan_queue_completion_submission ** submission_out);
};

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_QUEUE_H */
