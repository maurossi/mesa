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

#include "terakan_device_memory.h"

#include "terakan_descriptor.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_image.h"
#include "terakan_physical_device.h"

#include "util/bitscan.h"
#include "util/macros.h"
#include "util/u_math.h"
#include "vk_alloc.h"
#include "vk_log.h"
#include "vk_util.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

VKAPI_ATTR VkResult VKAPI_CALL
terakan_GetMemoryFdPropertiesKHR(VkDevice const deviceHandle,
                                 VkExternalMemoryHandleTypeFlagBits const handleType, int const fd,
                                 VkMemoryFdPropertiesKHR * const pMemoryFdProperties)
{
   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);
   struct terakan_physical_device const * const physical_device =
      container_of(device->vk.physical, struct terakan_physical_device const, vk);

   if (!(terakan_physical_device_supported_external_memory_types(physical_device) & handleType)) {
      return vk_error(device, VK_ERROR_INVALID_EXTERNAL_HANDLE);
   }

   if (handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT) {
      return vk_error(device, VK_ERROR_INVALID_EXTERNAL_HANDLE);
   }

   bool vram_preferred;
   if (!device->winsys_fn->bo->get_fd_vram_preference(device, fd, &vram_preferred)) {
      return vk_error(device, VK_ERROR_INVALID_EXTERNAL_HANDLE);
   }

   pMemoryFdProperties->memoryTypeBits =
      ((uint32_t)1 << physical_device->memory_properties.memoryTypeCount) - 1;
   /* The domain can be changed from the initial one in command submissions, so technically any
    * memory type should be fine for importing, but prefer not to move BOs between VRAM and GTT.
    */
   if (physical_device->chip_family_info.has_dedicated_vram) {
      VkMemoryPropertyFlags const device_local_flag_preferred =
         vram_preferred ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : 0;
      for (uint32_t memory_type_index = 0;
           memory_type_index < physical_device->memory_properties.memoryTypeCount;
           ++memory_type_index) {
         if ((physical_device->memory_properties.memoryTypes[memory_type_index].propertyFlags &
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != device_local_flag_preferred) {
            pMemoryFdProperties->memoryTypeBits &= ~((uint32_t)1 << memory_type_index);
         }
      }
   }

   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_GetMemoryFdKHR(VkDevice const deviceHandle, VkMemoryGetFdInfoKHR const * const pGetFdInfo,
                       int * const pFd)
{
   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);

   struct terakan_bo * const bo = terakan_device_memory_from_handle(pGetFdInfo->memory)->bo;

   int const fd = device->winsys_fn->bo->export_fd(
      bo, pGetFdInfo->handleType == VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT);
   if (fd < 0) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   *pFd = fd;
   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_FlushMappedMemoryRanges(UNUSED VkDevice const device,
                                UNUSED uint32_t const memoryRangeCount,
                                UNUSED VkMappedMemoryRange const * const pMemoryRanges)
{
   /* All host-visible memory types are host-coherent currently. */
   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_InvalidateMappedMemoryRanges(UNUSED VkDevice const device,
                                     UNUSED uint32_t const memoryRangeCount,
                                     UNUSED VkMappedMemoryRange const * const pMemoryRanges)
{
   /* All host-visible memory types are host-coherent currently. */
   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_UnmapMemory2KHR(UNUSED VkDevice const device,
                        VkMemoryUnmapInfoKHR const * const pMemoryUnmapInfo)
{
   struct terakan_device_memory * const device_memory =
      terakan_device_memory_from_handle(pMemoryUnmapInfo->memory);

   terakan_bo_unmap(device_memory->bo);

   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_MapMemory2KHR(VkDevice const deviceHandle, VkMemoryMapInfoKHR const * const pMemoryMapInfo,
                      void ** const ppData)
{
   struct terakan_device_memory * const device_memory =
      terakan_device_memory_from_handle(pMemoryMapInfo->memory);

   void * const mapping = terakan_bo_map(device_memory->bo);
   if (mapping == NULL) {
      return vk_error(terakan_device_from_handle(deviceHandle), VK_ERROR_MEMORY_MAP_FAILED);
   }

   *ppData = mapping;
   return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
terakan_FreeMemory(VkDevice const deviceHandle, VkDeviceMemory const deviceMemory,
                   VkAllocationCallbacks const * const pAllocator)
{
   struct terakan_device_memory * const device_memory =
      terakan_device_memory_from_handle(deviceMemory);

   if (device_memory == NULL) {
      return;
   }

   terakan_bo_free(device_memory->bo, pAllocator);

   vk_object_free(&terakan_device_from_handle(deviceHandle)->vk, pAllocator, device_memory);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_AllocateMemory(VkDevice const deviceHandle,
                       VkMemoryAllocateInfo const * const pAllocateInfo,
                       VkAllocationCallbacks const * pAllocator, VkDeviceMemory * const pMemory)
{
   VkResult result;

   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   struct terakan_device_memory * const device_memory = vk_device_memory_create(
      &device->vk, pAllocateInfo, pAllocator, sizeof(struct terakan_device_memory));
   if (device_memory == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   struct terakan_physical_device const * const physical_device =
      container_of(device->vk.physical, struct terakan_physical_device const, vk);

   /* Storage and uniform buffers in Vulkan require only the offset to be aligned, not the range,
    * but the entire range must be visible to the shader anyway. For the purpose of bounds checking,
    * the ranges are rounded up to their respective access size alignments in
    * VkPhysicalDeviceRobustness2PropertiesEXT, so make sure the BO is never smaller than the size
    * rounded up, and the validation in the kernel driver doesn't consider the binding out of
    * bounds.
    * Linux Radeon 2.50.0 also validates the size of buffer RATs as LINEAR_ALIGNED image size, but
    * with the smallest SLICE_TILE_MAX it considers them zero-size, so the RAT pitch alignment is
    * not important here.
    */
   VkDeviceSize const bo_size = ALIGN_POT(device_memory->vk.size, TERAKAN_KCACHE_HW_LINE_BYTES);

   VkMemoryPropertyFlags const memory_property_flags =
      physical_device->memory_properties.memoryTypes[device_memory->vk.memory_type_index]
         .propertyFlags;

   /* Implementations must validate external handle usage requirements. */
   VkExternalMemoryHandleTypeFlags const import_handle_type = device_memory->vk.import_handle_type;
   if (import_handle_type) {
      if (!(terakan_physical_device_supported_external_memory_types(physical_device) &
            import_handle_type)) {
         result = vk_error(device, VK_ERROR_INVALID_EXTERNAL_HANDLE);
         goto fail_device_memory;
      }

      switch (device_memory->vk.import_handle_type) {
      case VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT:
      case VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT: {
         VkImportMemoryFdInfoKHR const * const import_fd_info =
            vk_find_struct_const(pAllocateInfo->pNext, IMPORT_MEMORY_FD_INFO_KHR);
         if (import_fd_info == NULL) {
            result = vk_error(device, VK_ERROR_INVALID_EXTERNAL_HANDLE);
            goto fail_device_memory;
         }
         result = device->winsys_fn->bo->import_fd(
            device, import_fd_info->fd, bo_size,
            (memory_property_flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0, pAllocator,
            VK_SYSTEM_ALLOCATION_SCOPE_OBJECT, &device_memory->bo);
         if (result != VK_SUCCESS) {
            result = vk_error(device, result);
            goto fail_device_memory;
         }
      } break;

      default:
         assert(!"Unsupported handle type exposed by "
                 "terakan_physical_device_supported_external_memory_types");
         result = vk_error(device, VK_ERROR_INVALID_EXTERNAL_HANDLE);
         goto fail_device_memory;
      }
   } else {
      result = device->winsys_fn->bo->allocate_device_memory(
         device, bo_size, physical_device->buffer_image_bo_alignment, memory_property_flags,
         device_memory->vk.export_handle_types, pAllocator, VK_SYSTEM_ALLOCATION_SCOPE_OBJECT,
         &device_memory->bo);
      if (result != VK_SUCCESS) {
         result = vk_error(device, result);
         goto fail_device_memory;
      }

      VkMemoryDedicatedAllocateInfo const * const dedicated_info =
         vk_find_struct_const(pAllocateInfo->pNext, MEMORY_DEDICATED_ALLOCATE_INFO);
      if (dedicated_info != NULL) {
         struct terakan_image const * const dedicated_image =
            terakan_image_from_handle(dedicated_info->image);
         if (dedicated_image != NULL && !device->winsys_fn->bo->set_tiling_for_surface(
                                           device_memory->bo, &dedicated_image->surface)) {
            result = vk_error(device, VK_ERROR_UNKNOWN);
            goto fail_bo;
         }
      }
   }

   *pMemory = terakan_device_memory_to_handle(device_memory);
   return VK_SUCCESS;

fail_bo:
   terakan_bo_free(device_memory->bo, pAllocator);
fail_device_memory:
   vk_device_memory_destroy(&device->vk, pAllocator, &device_memory->vk);
   return result;
}
