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

#include "terakan_bo_wddm.h"

#include "terakan_device_wddm.h"
#include "terakan_wddm_d3dkmthk.h"

#include "amd/terascale/common/terascale_wddm.h"
#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

static bool
terakan_bo_wddm_set_tiling(UNUSED struct terakan_bo * const bo_base,
                           UNUSED struct terakan_bo_tiling const * const tiling)
{
   /* TODO(Triang3l): Pass dedicated allocation info to allocation creation since tiling is likely
    * specified in private data at allocation creation time. For now this empty implementation is
    * there just not to crash because set_tiling is done for all dedicated allocations.
    */
   return true;
}

static void *
terakan_bo_wddm_map_impl(struct terakan_bo * const bo_base)
{
   struct terakan_bo_wddm const * const bo =
      container_of(bo_base, struct terakan_bo_wddm const, base);
   struct terakan_device_wddm const * const device =
      container_of(bo->base.device, struct terakan_device_wddm const, base);

   /* Arguments matching what is sent in the IgnoreSync case during Direct3D 11 execution. */
   D3DKMT_LOCK lock_arguments = {
      .hDevice = device->d3dkmt_device,
      .hAllocation = bo->allocation,
      .Flags =
         {
            .DonotWait = 1,
            .IgnoreSync = 1,
         },
   };

   NTSTATUS const lock_status = D3DKMTLock(&lock_arguments);
   if (!NT_SUCCESS(lock_status)) {
      vk_loge(VK_LOG_OBJS(terakan_device_log_obj(&device->base)),
              "Failed to lock a D3DKMT memory allocation");
      return NULL;
   }

   return lock_arguments.pData;
}

static void
terakan_bo_wddm_unmap_impl(struct terakan_bo * const bo_base)
{
   struct terakan_bo_wddm const * const bo =
      container_of(bo_base, struct terakan_bo_wddm const, base);
   struct terakan_device_wddm const * const device =
      container_of(bo->base.device, struct terakan_device_wddm const, base);

   D3DKMT_UNLOCK const unlock_arguments = {
      .hDevice = device->d3dkmt_device,
      .NumAllocations = 1,
      .phAllocations = &bo->allocation,
   };
   D3DKMTUnlock(&unlock_arguments);
}

static void
terakan_bo_wddm_free_impl(struct terakan_bo * const bo_base,
                          VkAllocationCallbacks const * const allocator)
{
   struct terakan_bo_wddm * const bo = container_of(bo_base, struct terakan_bo_wddm, base);
   struct terakan_device_wddm const * const device =
      container_of(bo->base.device, struct terakan_device_wddm const, base);

   D3DKMT_DESTROYALLOCATION const destroy_allocation_arguments = {
      .hDevice = device->d3dkmt_device,
      .phAllocationList = &bo->allocation,
      .AllocationCount = 1,
   };
   D3DKMTDestroyAllocation(&destroy_allocation_arguments);

   vk_free2(&device->base.vk.alloc, allocator, bo);
}

static VkResult
terakan_bo_wddm_allocate_device_memory(
   struct terakan_device * const device_base, VkDeviceSize const size, VkDeviceSize const alignment,
   VkMemoryPropertyFlags const flags,
   UNUSED VkExternalMemoryHandleTypeFlags const export_handle_types,
   VkAllocationCallbacks const * const allocator, VkSystemAllocationScope const scope,
   struct terakan_bo ** const bo_out)
{
   struct terakan_device_wddm * const device =
      container_of(device_base, struct terakan_device_wddm, base);

   struct terakan_bo_wddm * const bo =
      vk_alloc2(&device->base.vk.alloc, allocator, sizeof(struct terakan_bo_wddm),
                alignof(struct terakan_bo_wddm), scope);
   if (bo == NULL) {
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   alignas(uint32_t) char
      allocation_private_data[sizeof(struct terascale_wddm_allocation_private_data_header) +
                              sizeof(struct terascale_wddm_allocation_private_data_unknown_struct)];
   uint32_t allocation_private_data_size_bytes = 0;

   assert(sizeof(allocation_private_data) - allocation_private_data_size_bytes >=
          sizeof(struct terascale_wddm_allocation_private_data_header));
   struct terascale_wddm_allocation_private_data_header * const allocation_private_data_header =
      (struct terascale_wddm_allocation_private_data_header *)(allocation_private_data +
                                                               allocation_private_data_size_bytes);
   allocation_private_data_size_bytes +=
      sizeof(struct terascale_wddm_allocation_private_data_header);

   terascale_wddm_allocation_private_data_header_clear(allocation_private_data_header);
   allocation_private_data_header->alignment_bytes = (uint32_t)alignment;
   allocation_private_data_header->size_bytes = (uint32_t)size;

   uint8_t memory_type_priority[4] = {};
   bool const host_visible = (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
   if (host_visible) {
      if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
         /* Matching a D3D11_USAGE_STAGING allocation. */
         memory_type_priority[0] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_HOST_CACHED;
      } else {
         if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            /* Matching a small D3D11_USAGE_DYNAMIC allocation. */
            memory_type_priority[0] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_DEVICE_HOST_VISIBLE;
            memory_type_priority[1] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_DEVICE_HOST_VISIBLE;
            memory_type_priority[2] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_HOST_WRITE_COMBINED;
            memory_type_priority[3] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_HOST_CACHED;
         } else {
            memory_type_priority[0] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_HOST_WRITE_COMBINED;
            memory_type_priority[1] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_HOST_CACHED;
         }
      }
   } else {
      /* Matching a D3D11_USAGE_DEFAULT allocation. */
      memory_type_priority[0] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_DEVICE_NON_HOST_VISIBLE;
      memory_type_priority[1] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_DEVICE_HOST_VISIBLE;
      memory_type_priority[2] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_HOST_WRITE_COMBINED;
      memory_type_priority[3] = TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_HOST_CACHED;
   }
   terascale_wddm_allocation_private_data_header_set_memory_types(
      allocation_private_data_header, memory_type_priority, host_visible);

   /* TODO(Triang3l): Fill subresource data for a dedicated allocation. */

   allocation_private_data_header->unknown_struct_offset_bytes = allocation_private_data_size_bytes;
   assert(sizeof(allocation_private_data) - allocation_private_data_size_bytes >=
          sizeof(struct terascale_wddm_allocation_private_data_unknown_struct));
   struct terascale_wddm_allocation_private_data_unknown_struct * const
      allocation_private_data_unknown_struct =
         (struct terascale_wddm_allocation_private_data_unknown_struct *)(allocation_private_data +
                                                                          allocation_private_data_size_bytes);
   allocation_private_data_size_bytes +=
      sizeof(struct terascale_wddm_allocation_private_data_unknown_struct);

   terascale_wddm_allocation_private_data_unknown_struct_clear(
      allocation_private_data_unknown_struct);

   allocation_private_data_header->private_data_size_bytes = allocation_private_data_size_bytes;

   D3DDDI_ALLOCATIONINFO allocation_info = {
      .pPrivateDriverData = allocation_private_data,
      .PrivateDriverDataSize = allocation_private_data_size_bytes,
   };

   struct terascale_wddm_create_allocation_private_data create_allocation_private_driver_data;
   terascale_wddm_create_allocation_private_data_clear(&create_allocation_private_driver_data);
   D3DKMT_CREATEALLOCATION create_allocation_arguments = {
      .hDevice = device->d3dkmt_device,
      .pPrivateDriverData = &create_allocation_private_driver_data,
      .PrivateDriverDataSize = sizeof(create_allocation_private_driver_data),
      .NumAllocations = 1,
      .pAllocationInfo = &allocation_info,
   };
   NTSTATUS const create_allocation_status = D3DKMTCreateAllocation(&create_allocation_arguments);
   if (!NT_SUCCESS(create_allocation_status)) {
      vk_free2(&device->base.vk.alloc, allocator, bo);
      vk_loge(VK_LOG_OBJS(terakan_device_log_obj(&device->base)),
              "Failed to create a D3DKMT memory allocation, size: %" PRIu64 " bytes, "
              "alignment: %" PRIu64 " bytes, memory property flags: 0x%" PRIX32 ", status 0x%08lX",
              size, alignment, flags, create_allocation_status);
      return VK_ERROR_OUT_OF_DEVICE_MEMORY;
   }

   terakan_bo_init(&bo->base, &device->base);

   bo->allocation = allocation_info.hAllocation;

   *bo_out = &bo->base;
   return VK_SUCCESS;
}

struct terakan_bo_winsys_fn const terakan_bo_wddm_fn = {
   .set_tiling = terakan_bo_wddm_set_tiling,
   .map_impl = terakan_bo_wddm_map_impl,
   .unmap_impl = terakan_bo_wddm_unmap_impl,
   .free_impl = terakan_bo_wddm_free_impl,
   .allocate_device_memory = terakan_bo_wddm_allocate_device_memory,
};
