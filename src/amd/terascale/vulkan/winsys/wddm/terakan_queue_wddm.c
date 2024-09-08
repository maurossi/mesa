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

#include "terakan_queue_wddm.h"

#include "terakan_bo_wddm.h"
#include "terakan_device_wddm.h"
#include "terakan_queue.h"
#include "terakan_wddm_d3dkmthk.h"

#include "amd/terascale/common/terascale_wddm.h"
#include "util/macros.h"
#include "c99_alloca.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static void
terakan_queue_wddm_create_bo_reference(void * const bo_reference_ptr,
                                       struct terakan_bo const * const bo_base,
                                       UNUSED bool const is_reading, bool const is_writing,
                                       UNUSED enum terakan_bo_priority const priority)
{
   struct terakan_bo_wddm const * const bo =
      container_of(bo_base, struct terakan_bo_wddm const, base);

   *(D3DDDI_ALLOCATIONLIST *)bo_reference_ptr = (D3DDDI_ALLOCATIONLIST){
      .hAllocation = bo->allocation,
      .WriteOperation = (UINT)is_writing,
   };
}

static void
terakan_queue_wddm_update_bo_reference(void * const bo_reference_ptr,
                                       struct terakan_bo const * const bo_base,
                                       UNUSED bool const is_reading, bool const is_writing,
                                       UNUSED enum terakan_bo_priority const priority)
{
   D3DDDI_ALLOCATIONLIST * const bo_reference = (D3DDDI_ALLOCATIONLIST *)bo_reference_ptr;

   struct terakan_bo_wddm const * const bo =
      container_of(bo_base, struct terakan_bo_wddm const, base);

   assert(bo_reference->hAllocation == bo->allocation);

   if (is_writing) {
      bo_reference->WriteOperation = 1;
   }
}

static void
terakan_queue_wddm_release_submission_context(
   struct terakan_queue_submission_context * const submission_context_base)
{
   struct terakan_queue_submission_context_wddm * const submission_context =
      container_of(submission_context_base, struct terakan_queue_submission_context_wddm, base);

   D3DKMT_DESTROYCONTEXT const destroy_context_arguments = {
      .hContext = submission_context->context,
   };
   D3DKMTDestroyContext(&destroy_context_arguments);

   vk_free(&submission_context->device->base.vk.alloc, submission_context);
}

enum terakan_queue_submission_context_wddm_node {
   TERAKAN_QUEUE_SUBMISSION_CONTEXT_NODE_GFX = 0,
};

struct terakan_queue_submission_context_wddm_private_data {
   uint32_t private_data_size_bytes;
   uint32_t unknown_0x4_0x2002;
   uint32_t command_buffer_size_bytes;
   uint32_t allocation_list_size_elements;
   uint32_t patch_location_list_size_elements;
   uint32_t unknown_0x14_0[0xB];
};

static_assert(
   sizeof(struct terakan_queue_submission_context_wddm_private_data) == 0x40,
   "The sizes of private data structures must match those expected by the kernel-mode driver.");

static VkResult
terakan_queue_wddm_acquire_submission_context(
   struct terakan_device * const device_base, enum amd_ip_type const ip_type,
   struct terakan_queue_submission_size const desired_submission_size,
   struct terakan_queue_submission_context ** const submission_context_out)
{
   VkResult result;

   struct terakan_device_wddm const * const device =
      container_of(device_base, struct terakan_device_wddm const, base);

   struct terakan_queue_submission_context_wddm * const submission_context = vk_alloc(
      &device->base.vk.alloc, sizeof(struct terakan_queue_submission_context_wddm),
      alignof(struct terakan_queue_submission_context_wddm), VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (submission_context == NULL) {
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   struct terascale_wddm_create_context_private_data private_data;
   terascale_wddm_create_context_private_data_clear(&private_data);
   private_data.command_buffer_size_bytes =
      sizeof(uint32_t) * desired_submission_size.indirect_buffer_dwords;
   private_data.allocation_list_size_elements =
      desired_submission_size.bo_references + TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_BO_REFERENCES;
   private_data.patch_location_list_size_elements =
      desired_submission_size.relocations + TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_RELOCATIONS;

   D3DKMT_CREATECONTEXT create_context_arguments = {
      .hDevice = device->d3dkmt_device,
      .EngineAffinity = 0b1,
      .pPrivateDriverData = &private_data,
      .PrivateDriverDataSize = sizeof(private_data),
      /* D3DKMT_CLIENTHINT_VULKAN is meaningless to the target kernel driver and was added long
       * after the last driver package for TeraScale was released, but UNKNOWN is sufficient, it's
       * better to avoid any potential expectations in the kernel-mode driver about the user-mode
       * driver implementing any API-specific functionality.
       */
      .ClientHint = D3DKMT_CLIENTHINT_UNKNOWN,
   };

   switch (ip_type) {
   case AMD_IP_GFX:
      create_context_arguments.NodeOrdinal = TERAKAN_QUEUE_SUBMISSION_CONTEXT_NODE_GFX;
      break;
   default:
      assert(!"Unsupported queue type");
      vk_free(&device->base.vk.alloc, submission_context);
      return VK_ERROR_INITIALIZATION_FAILED;
   }

   NTSTATUS const create_context_status = D3DKMTCreateContext(&create_context_arguments);
   if (!NT_SUCCESS(create_context_status)) {
      vk_loge(VK_LOG_OBJS(terakan_device_log_obj(&device->base)),
              "Failed to create the D3DKMT context for node %" PRIu32 " with %" PRIu32 " command "
              "buffer dwords, %" PRIu32 " allocations, %" PRIu32 " patch locations per submission, "
              "status 0x%08lX",
              create_context_arguments.NodeOrdinal, desired_submission_size.indirect_buffer_dwords,
              desired_submission_size.bo_references, desired_submission_size.relocations,
              create_context_status);
      vk_free(&device->base.vk.alloc, submission_context);
      return VK_ERROR_INITIALIZATION_FAILED;
   }

   assert(create_context_arguments.AllocationListSize >=
          TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_BO_REFERENCES);
   assert(create_context_arguments.PatchLocationListSize >=
          TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_RELOCATIONS);
   terakan_queue_submission_context_init(
      &submission_context->base,
      (struct terakan_queue_submission_size){
         .indirect_buffer_dwords = create_context_arguments.CommandBufferSize / sizeof(uint32_t),
         .bo_references = create_context_arguments.AllocationListSize -
                          TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_BO_REFERENCES,
         .relocations = create_context_arguments.PatchLocationListSize -
                        TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_RELOCATIONS,
      });

   submission_context->device = device;

   submission_context->context = create_context_arguments.hContext;

   submission_context->command_buffer = create_context_arguments.pCommandBuffer;
   submission_context->allocation_list = create_context_arguments.pAllocationList;
   submission_context->patch_location_list = create_context_arguments.pPatchLocationList;

   *submission_context_out = &submission_context->base;
   return VK_SUCCESS;
}

static VkResult
terakan_queue_wddm_submit(struct terakan_queue_submission_context * const submission_context_base,
                          uint32_t const bo_reference_count, void const * const bo_references,
                          uint32_t const indirect_buffer_size_dwords,
                          uint32_t const * const indirect_buffer, uint32_t const relocation_count,
                          void const * const relocations)
{
   struct terakan_queue_submission_context_wddm * const submission_context =
      container_of(submission_context_base, struct terakan_queue_submission_context_wddm, base);

   uint32_t indirect_buffer_size_bytes = (uint32_t)(sizeof(uint32_t) * indirect_buffer_size_dwords);
   memcpy(submission_context->command_buffer, indirect_buffer, indirect_buffer_size_bytes);

   /* Allocation list setup. */

   uint32_t internal_allocation_count = 0;

   /* Dummy allocation for the mandatory patch location header (STATUS_GRAPHICS_DRIVER_MISMATCH is
    * returned without it) and the patch location footer that accepts some Windows handle.
    */
   assert(internal_allocation_count < TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_BO_REFERENCES);
   uint32_t const header_allocation_index = internal_allocation_count;
   submission_context->allocation_list[internal_allocation_count++] = (D3DDDI_ALLOCATIONLIST){};

   /* Explicitly checking not to memcpy from a null pointer if BO references are not needed. */
   if (bo_reference_count != 0) {
      memcpy(submission_context->allocation_list + internal_allocation_count, bo_references,
             sizeof(D3DDDI_ALLOCATIONLIST) * bo_reference_count);
   }

   /* Patch location list setup. */

   uint32_t internal_patch_location_count = 0;

   assert(internal_patch_location_count < TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_RELOCATIONS);
   submission_context->patch_location_list[internal_patch_location_count++] =
      (D3DDDI_PATCHLOCATIONLIST){
         .AllocationIndex = header_allocation_index,
         .SlotId = ((uint32_t)0x1D << 10) | 3,
         .AllocationOffset = (uint32_t)1 << 25,
      };
   assert(internal_patch_location_count < TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_RELOCATIONS);
   submission_context->patch_location_list[internal_patch_location_count++] =
      (D3DDDI_PATCHLOCATIONLIST){
         .AllocationIndex = header_allocation_index,
         .SlotId = ((uint32_t)0x8F << 10) | 1,
      };

   static_assert(
      sizeof(struct terakan_queue_relocation_wddm_patch) == sizeof(D3DDDI_PATCHLOCATIONLIST),
      "The structure representing D3DDDI_PATCHLOCATIONLIST within the driver must match the actual "
      "D3DDDI_PATCHLOCATIONLIST.");
   /* Explicitly checking not to memcpy from a null pointer if relocations are not needed. */
   if (relocation_count != 0) {
      memcpy(submission_context->patch_location_list + internal_patch_location_count, relocations,
             sizeof(D3DDDI_PATCHLOCATIONLIST) * relocation_count);
   }

   /* TODO(Triang3l): Research how the final (possibly signal) patch locations may be used (see
    * terascale_wddm.h): what their purpose is, what happens with the first pair at over 4096
    * submissions and maybe at over UINT32_MAX submissions (possibly by manually submitting with the
    * second patch location with a nonzero AllocationOffset), contents and usage of that mapped
    * memory, what handle is accepted by the second pair (and see if it's a pair on 32-bit systems)
    * and how it's interacted with.
    */

   /* Submission. */

   D3DKMT_RENDER render_arguments = {
      .hContext = submission_context->context,
      .CommandLength = indirect_buffer_size_bytes,
      .AllocationCount = bo_reference_count + internal_allocation_count,
      .PatchLocationCount = relocation_count + internal_patch_location_count,
   };

   NTSTATUS const render_status = D3DKMTRender(&render_arguments);

   /* The MSDN article for D3DKMT_RENDER says:
    *
    *     "The driver must always update its pointer to the [command buffer / allocation list /
    *     patch-location list] after any call to D3DKMTRender regardless of whether the call is
    *     successful."
    */
   submission_context->command_buffer = render_arguments.pNewCommandBuffer;
   submission_context->allocation_list = render_arguments.pNewAllocationList;
   submission_context->patch_location_list = render_arguments.pNewPatchLocationList;

   if (!NT_SUCCESS(render_status)) {
      vk_loge(VK_LOG_OBJS(terakan_device_log_obj(&submission_context->device->base)),
              "Failed to submit commands to the kernel driver, status 0x%08lX", render_status);
      return VK_ERROR_UNKNOWN;
   }

   /* The MSDN article for D3DKMT_RENDER says:
    *
    *    "Depending on current memory conditions the output size might not match the input size."
    *
    * As command buffers are written independently of submissions, it's not possible to change the
    * maximum size based on the result of a submission, so assume that the kernel driver is nearly
    * out of memory.
    */
   assert(render_arguments.NewCommandBufferSize / sizeof(uint32_t) >=
          submission_context->base.max_submission_size.indirect_buffer_dwords);
   assert(render_arguments.NewAllocationListSize >=
          submission_context->base.max_submission_size.bo_references +
             TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_BO_REFERENCES);
   assert(render_arguments.NewPatchLocationListSize >=
          submission_context->base.max_submission_size.relocations +
             TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_RELOCATIONS);
   if (render_arguments.NewCommandBufferSize / sizeof(uint32_t) <
          submission_context->base.max_submission_size.indirect_buffer_dwords ||
       render_arguments.NewAllocationListSize <
          submission_context->base.max_submission_size.bo_references +
             TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_BO_REFERENCES ||
       render_arguments.NewPatchLocationListSize <
          submission_context->base.max_submission_size.relocations +
             TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_RELOCATIONS) {
      vk_loge(
         VK_LOG_OBJS(terakan_device_log_obj(&submission_context->device->base)),
         "Command submission returned new D3DKMT command buffer, allocation list or patch location "
         "list memory that's smaller than the limit used for recording Vulkan command buffers, not "
         "possible to submit recorded command buffers anymore, so assuming that the device is out "
         "of host memory",
         render_status);
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   return VK_SUCCESS;
}

static VkResult
terakan_queue_completion_submission_wddm_submit(
   struct terakan_queue_completion_submission * const submission_base,
   uint32_t const signal_indirect_buffer_size_dwords, uint32_t const * const signal_indirect_buffer)
{
   struct terakan_queue_completion_submission_wddm_1_0 const * const submission = container_of(
      submission_base, struct terakan_queue_completion_submission_wddm_1_0 const, base);
   struct terakan_device * const device =
      container_of(submission->base.queue->vk.base.device, struct terakan_device, vk);

   /* Make the BO in use until the GPU has completed the submission. */
   void * const signal_bo_reference = alloca(device->bo_reference_size);
   device->winsys_fn->queue->create_bo_reference(signal_bo_reference, &submission->bo->base, false,
                                                 true, TERAKAN_BO_PRIORITY_SYNC);
   return device->winsys_fn->queue->submit(submission->base.queue->submission_context, 1,
                                           signal_bo_reference, signal_indirect_buffer_size_dwords,
                                           signal_indirect_buffer, 0, NULL);
}

static bool
terakan_queue_completion_submission_wddm_await(
   struct terakan_queue_completion_submission * const submission_base)
{
   struct terakan_queue_completion_submission_wddm_1_0 const * const submission = container_of(
      submission_base, struct terakan_queue_completion_submission_wddm_1_0 const, base);
   struct terakan_device_wddm const * const device = container_of(
      submission->base.queue->vk.base.device, struct terakan_device_wddm const, base.vk);

   D3DKMT_LOCK lock_arguments = {
      .hDevice = device->d3dkmt_device,
      .hAllocation = submission->bo->allocation,
      .Flags =
         {
            .ReadOnly = 1,
         },
   };
   if (!NT_SUCCESS(D3DKMTLock(&lock_arguments))) {
      return false;
   }

   D3DKMT_UNLOCK const unlock_arguments = {
      .hDevice = device->d3dkmt_device,
      .NumAllocations = 1,
      .phAllocations = &submission->bo->allocation,
   };
   D3DKMTUnlock(&unlock_arguments);

   return true;
}

static void
terakan_queue_completion_submission_wddm_finish_winsys_and_free(
   struct terakan_queue_completion_submission * const submission_base)
{
   struct terakan_queue_completion_submission_wddm_1_0 * const submission =
      container_of(submission_base, struct terakan_queue_completion_submission_wddm_1_0, base);

   terakan_bo_free(&submission->bo->base, NULL);

   vk_free(&submission->base.queue->vk.base.device->alloc, submission);
}

static VkResult
terakan_queue_completion_submission_wddm_alloc_and_init_winsys(
   struct terakan_queue * const queue,
   struct terakan_queue_completion_submission ** const submission_out)
{
   VkResult result;

   struct terakan_device * const device =
      container_of(queue->vk.base.device, struct terakan_device, vk);

   struct terakan_queue_completion_submission_wddm_1_0 * const submission =
      vk_alloc(&device->vk.alloc, sizeof(struct terakan_queue_completion_submission_wddm_1_0),
               alignof(struct terakan_queue_completion_submission_wddm_1_0),
               VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (submission == NULL) {
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   struct terakan_bo * bo_base;
   result = device->winsys_fn->bo->allocate_device_memory(
      device, 1, 1, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 0, NULL, VK_SYSTEM_ALLOCATION_SCOPE_DEVICE,
      &bo_base);
   if (result != VK_SUCCESS) {
      vk_free(&device->vk.alloc, submission);
      return result;
   }
   submission->bo = container_of(bo_base, struct terakan_bo_wddm, base);
   /* Will be mapping the BO to wait for it to become not in use, so creation must not map it by
    * itself.
    */
   assert(submission->bo->base.mapping == NULL);

   *submission_out = &submission->base;
   return VK_SUCCESS;
}

struct terakan_queue_winsys_fn const terakan_queue_wddm_fn = {
   .create_bo_reference = terakan_queue_wddm_create_bo_reference,
   .update_bo_reference = terakan_queue_wddm_update_bo_reference,
   .release_submission_context = terakan_queue_wddm_release_submission_context,
   .acquire_submission_context = terakan_queue_wddm_acquire_submission_context,
   .submit = terakan_queue_wddm_submit,
   .completion_submission_submit = terakan_queue_completion_submission_wddm_submit,
   .completion_submission_await = terakan_queue_completion_submission_wddm_await,
   .completion_submission_finish_winsys_and_free =
      terakan_queue_completion_submission_wddm_finish_winsys_and_free,
   .completion_submission_alloc_and_init_winsys =
      terakan_queue_completion_submission_wddm_alloc_and_init_winsys,
};
