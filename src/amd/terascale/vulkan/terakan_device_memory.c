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

#include "winsys/terakan_winsys.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_image.h"
#include "terakan_physical_device.h"

#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"
#include "vk_util.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

VKAPI_ATTR VkResult VKAPI_CALL
terakan_FlushMappedMemoryRanges(VkDevice const device, uint32_t const memoryRangeCount,
                                VkMappedMemoryRange const * const pMemoryRanges)
{
   /* All host-visible memory types are host-coherent currently. */
   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_InvalidateMappedMemoryRanges(VkDevice const device, uint32_t const memoryRangeCount,
                                     VkMappedMemoryRange const * const pMemoryRanges)
{
   /* All host-visible memory types are host-coherent currently. */
   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_UnmapMemory2KHR(VkDevice const deviceHandle,
                        VkMemoryUnmapInfoKHR const * const pMemoryUnmapInfo)
{
   struct terakan_device_memory * const device_memory =
      terakan_device_memory_from_handle(pMemoryUnmapInfo->memory);

   terakan_winsys_bo_unmap(device_memory->bo);

   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_MapMemory2KHR(VkDevice const deviceHandle, VkMemoryMapInfoKHR const * const pMemoryMapInfo,
                      void ** const ppData)
{
   struct terakan_device_memory * const device_memory =
      terakan_device_memory_from_handle(pMemoryMapInfo->memory);

   void * const mapping = terakan_winsys_bo_map(device_memory->bo);
   if (mapping == NULL) {
      return vk_error(terakan_device_from_handle(deviceHandle), VK_ERROR_MEMORY_MAP_FAILED);
   }

   *ppData = mapping;

   return VK_SUCCESS;
}

#if !defined(_WIN32)
VKAPI_ATTR VkResult VKAPI_CALL
terakan_GetMemoryFdKHR(VkDevice const device, VkMemoryGetFdInfoKHR const * const pGetFdInfo,
                       int * const pFd)
{
   struct terakan_winsys_bo * const bo = terakan_device_memory_from_handle(pGetFdInfo->memory)->bo;

   int const fd = bo->winsys->bo_fn->export_fd(
      bo, pGetFdInfo->handleType == VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT);
   if (fd < 0) {
      return vk_error(terakan_device_from_handle(device), VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   *pFd = fd;

   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_GetMemoryFdPropertiesKHR(VkDevice const deviceHandle,
                                 VkExternalMemoryHandleTypeFlagBits const handleType, int const fd,
                                 VkMemoryFdPropertiesKHR * const pMemoryFdProperties)
{
   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);

   if (handleType != VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT) {
      return vk_error(device, VK_ERROR_INVALID_EXTERNAL_HANDLE);
   }

   struct terakan_physical_device const * const physical_device =
      container_of(device->vk.physical, struct terakan_physical_device const, vk);

   bool vram_preferred;
   if (!physical_device->winsys->bo_fn->get_fd_vram_preference(physical_device->winsys, fd,
                                                               &vram_preferred)) {
      return vk_error(device, VK_ERROR_INVALID_EXTERNAL_HANDLE);
   }

   pMemoryFdProperties->memoryTypeBits =
      ((uint32_t)1 << physical_device->memory_properties.memoryTypeCount) - 1;
   /* The domain can be changed from the initial one in command submissions, so technically any
    * memory type should be fine for importing, but prefer not to move BOs between VRAM and GART.
    */
   if (physical_device->winsys->gpu_info.has_dedicated_vram) {
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
#endif

VKAPI_ATTR void VKAPI_CALL
terakan_FreeMemory(VkDevice const deviceHandle, VkDeviceMemory const deviceMemory,
                   VkAllocationCallbacks const * const pAllocator)
{
   struct terakan_device_memory * const device_memory =
      terakan_device_memory_from_handle(deviceMemory);

   if (device_memory == NULL) {
      return;
   }

   terakan_winsys_bo_free(device_memory->bo);

   vk_object_free(&terakan_device_from_handle(deviceHandle)->vk, pAllocator, device_memory);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_AllocateMemory(VkDevice const deviceHandle,
                       VkMemoryAllocateInfo const * const pAllocateInfo,
                       VkAllocationCallbacks const * pAllocator, VkDeviceMemory * const pMemory)
{
   VkResult result;

   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   struct terakan_device_memory * const device_memory = vk_object_alloc(
      &device->vk, pAllocator, sizeof(*device_memory), VK_OBJECT_TYPE_DEVICE_MEMORY);
   if (device_memory == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   struct terakan_physical_device const * const physical_device =
      container_of(device->vk.physical, struct terakan_physical_device const, vk);

   VkMemoryPropertyFlags const memory_property_flags =
      physical_device->memory_properties.memoryTypes[pAllocateInfo->memoryTypeIndex].propertyFlags;

   bool imported = false;
#if !defined(_WIN32)
   VkImportMemoryFdInfoKHR const * const import_fd_info =
      vk_find_struct_const(pAllocateInfo, IMPORT_MEMORY_FD_INFO_KHR);
   if (import_fd_info != NULL) {
      imported = true;
      assert(import_fd_info->handleType == VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT ||
             import_fd_info->handleType == VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT);
      device_memory->bo = physical_device->winsys->bo_fn->import_fd(
         physical_device->winsys, import_fd_info->fd, pAllocateInfo->allocationSize,
         (memory_property_flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0);
   }
#endif
   if (!imported) {
      VkExportMemoryAllocateInfo const * const export_info =
         vk_find_struct_const(pAllocateInfo, EXPORT_MEMORY_ALLOCATE_INFO);
      device_memory->bo = physical_device->winsys->bo_fn->allocate_device_memory(
         physical_device->winsys, pAllocateInfo->allocationSize,
         physical_device->winsys->gpu_info.buffer_image_bo_alignment, memory_property_flags,
         export_info != NULL ? export_info->handleTypes : 0);
   }
   if (device_memory->bo == NULL) {
      result = vk_error(device, VK_ERROR_OUT_OF_DEVICE_MEMORY);
      goto fail_device_memory;
   }

   if (!imported) {
      VkMemoryDedicatedAllocateInfo const * const dedicated_info =
         vk_find_struct_const(pAllocateInfo, MEMORY_DEDICATED_ALLOCATE_INFO);
      if (dedicated_info != NULL) {
         struct terakan_image const * const dedicated_image =
            terakan_image_from_handle(dedicated_info->image);
         if (dedicated_image != NULL && !physical_device->winsys->bo_fn->set_tiling_for_surface(
                                           device_memory->bo, &dedicated_image->surface)) {
            result = vk_error(device, VK_ERROR_UNKNOWN);
            goto fail_bo;
         }
      }
   }

   *pMemory = terakan_device_memory_to_handle(device_memory);

   return VK_SUCCESS;

fail_bo:
   terakan_winsys_bo_free(device_memory->bo);
fail_device_memory:
   vk_object_free(&device->vk, pAllocator, device_memory);
   return result;
}
