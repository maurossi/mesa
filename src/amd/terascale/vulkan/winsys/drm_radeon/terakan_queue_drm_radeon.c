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

#include "terakan_bo_drm_radeon.h"
#include "terakan_device_drm_radeon.h"
#include "terakan_queue.h"

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

static void
terakan_queue_drm_radeon_create_bo_reference(void * const bo_reference_ptr,
                                             struct terakan_bo const * const bo_base,
                                             bool const is_reading, bool const is_writing,
                                             enum terakan_bo_priority const priority)
{
   struct drm_radeon_cs_reloc * const bo_reference = (struct drm_radeon_cs_reloc *)bo_reference_ptr;

   struct terakan_bo_drm_radeon const * const bo =
      container_of(bo_base, struct terakan_bo_drm_radeon const, base);

   bo_reference->handle = bo->handle;

   bo_reference->read_domains = is_reading ? bo->domains : 0;
   bo_reference->write_domain = is_writing ? bo->domains : 0;

   assert(((__u32)priority & ~(__u32)RADEON_RELOC_PRIO_MASK) == 0);
   bo_reference->flags = (__u32)priority;
}

static void
terakan_queue_drm_radeon_update_bo_reference(void * const bo_reference_ptr,
                                             struct terakan_bo const * const bo_base,
                                             bool const is_reading, bool const is_writing,
                                             enum terakan_bo_priority const priority)
{
   struct drm_radeon_cs_reloc * const bo_reference = (struct drm_radeon_cs_reloc *)bo_reference_ptr;

   struct terakan_bo_drm_radeon const * const bo =
      container_of(bo_base, struct terakan_bo_drm_radeon const, base);

   assert(bo_reference->handle == bo->handle);

   if (is_reading) {
      bo_reference->read_domains |= bo->domains;
   }
   if (is_writing) {
      bo_reference->write_domain |= bo->domains;
   }

   assert(((__u32)priority & ~(__u32)RADEON_RELOC_PRIO_MASK) == 0);
   /* The flags only contain the priority. */
   bo_reference->flags = MAX2((__u32)priority, bo_reference->flags);
}

static void
terakan_queue_drm_radeon_release_submission_context(
   UNUSED struct terakan_queue_submission_context * const submission_context_base)
{
   /* Contexts are singletons within a device, no need to do anything. */
}

static VkResult
terakan_queue_drm_radeon_acquire_submission_context(
   struct terakan_device * const device_base, enum amd_ip_type const ip_type,
   UNUSED struct terakan_queue_submission_size const desired_submission_size,
   struct terakan_queue_submission_context ** const submission_context_out)
{
   struct terakan_device_drm_radeon * const device =
      container_of(device_base, struct terakan_device_drm_radeon, base);

   /* DRM Radeon receives submissions directly through the render device file descriptor without a
    * context, using per-device singletons.
    */

   switch (ip_type) {
   case AMD_IP_GFX:
      *submission_context_out = &device->gfx_submission_context.base;
      return VK_SUCCESS;
   default:
      assert(!"Unsupported queue type");
      return VK_ERROR_INITIALIZATION_FAILED;
   }
}

static VkResult
terakan_queue_drm_radeon_submit(
   struct terakan_queue_submission_context * const submission_context_base,
   uint32_t const bo_reference_count, void const * const bo_references,
   uint32_t const indirect_buffer_size_dwords, uint32_t const * const indirect_buffer,
   UNUSED uint32_t const relocation_count, UNUSED void const * const relocations)
{
   /* The kernel driver returns -EINVAL for zero-length indirect buffers, so even if a submission is
    * needed only for BO fence purposes, it still must not be empty.
    */
   assert(indirect_buffer_size_dwords != 0);

   struct terakan_queue_submission_context_drm_radeon const * const submission_context =
      container_of(submission_context_base,
                   struct terakan_queue_submission_context_drm_radeon const, base);
   struct terakan_device_drm_radeon const * const device = submission_context->device;

   /* Flags, ring, priority. */
   __u32 flags[3] = {
      [1] = submission_context->ring,
   };

   if (submission_context->ring == RADEON_CS_RING_GFX) {
      /* The size alignment requirement is not handled by the kernel driver. */
      assert(!(indirect_buffer_size_dwords &
               (TERAKAN_QUEUE_INDIRECT_BUFFER_SIZE_ALIGNMENT_DWORDS_GFX - 1)));
      flags[0] |= RADEON_CS_KEEP_TILING_FLAGS;
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

   int const cs_result = drmCommandWriteRead(device->render_node_fd, DRM_RADEON_CS, &cs_arguments,
                                             sizeof(cs_arguments));

   if (cs_result != 0) {
      if (cs_result == -ENOMEM) {
         vk_loge(VK_LOG_OBJS(terakan_device_log_obj(&device->base)),
                 "Not enough memory for command submission");
         return VK_ERROR_OUT_OF_HOST_MEMORY;
      }

      vk_loge(VK_LOG_OBJS(terakan_device_log_obj(&device->base)),
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

static VkResult
terakan_queue_completion_submission_drm_radeon_submit(
   struct terakan_queue_completion_submission * const submission_base,
   uint32_t const signal_indirect_buffer_size_dwords, uint32_t const * const signal_indirect_buffer)
{
   struct terakan_queue_completion_submission_drm_radeon const * const submission = container_of(
      submission_base, struct terakan_queue_completion_submission_drm_radeon const, base);
   struct terakan_device * const device =
      container_of(submission->base.queue->vk.base.device, struct terakan_device, vk);

   /* Make the BO not idle until the GPU has completed the submission. */
   void * const signal_bo_reference = alloca(device->bo_reference_size);
   device->winsys_fn->queue->create_bo_reference(signal_bo_reference, &submission->bo->base, false,
                                                 true, TERAKAN_BO_PRIORITY_SYNC);
   return device->winsys_fn->queue->submit(submission->base.queue->submission_context, 1,
                                           signal_bo_reference, signal_indirect_buffer_size_dwords,
                                           signal_indirect_buffer, 0, NULL);
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
   /* Returns -EBUSY in finite time in case of a hang (30-second timeout in DRM Radeon 2.50.0). */
   return drmCommandWrite(device->render_node_fd, DRM_RADEON_GEM_WAIT_IDLE,
                          &gem_wait_idle_arguments, sizeof(gem_wait_idle_arguments)) == 0;
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
   struct terakan_queue * const queue,
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
      device, 1, 1, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0, NULL, VK_SYSTEM_ALLOCATION_SCOPE_DEVICE,
      &bo_base);
   if (result != VK_SUCCESS) {
      vk_free(&device->vk.alloc, submission);
      return result;
   }
   submission->bo = container_of(bo_base, struct terakan_bo_drm_radeon, base);

   *submission_out = &submission->base;
   return VK_SUCCESS;
}

struct terakan_queue_winsys_fn const terakan_queue_drm_radeon_fn = {
   .create_bo_reference = terakan_queue_drm_radeon_create_bo_reference,
   .update_bo_reference = terakan_queue_drm_radeon_update_bo_reference,
   .release_submission_context = terakan_queue_drm_radeon_release_submission_context,
   .acquire_submission_context = terakan_queue_drm_radeon_acquire_submission_context,
   .submit = terakan_queue_drm_radeon_submit,
   .completion_submission_submit = terakan_queue_completion_submission_drm_radeon_submit,
   .completion_submission_await = terakan_queue_completion_submission_drm_radeon_await,
   .completion_submission_finish_winsys_and_free =
      terakan_queue_completion_submission_drm_radeon_finish_winsys_and_free,
   .completion_submission_alloc_and_init_winsys =
      terakan_queue_completion_submission_drm_radeon_alloc_and_init_winsys,
};
