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

#include "terakan_buffer.h"

#include "terakan_descriptor.h"
#include "terakan_device.h"
#include "terakan_device_memory.h"
#include "terakan_entrypoints.h"
#include "terakan_physical_device.h"

#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"
#include "vk_util.h"

#include <stddef.h>
#include <stdint.h>

VKAPI_ATTR void VKAPI_CALL
terakan_GetPhysicalDeviceExternalBufferProperties(
   VkPhysicalDevice const physicalDevice,
   VkPhysicalDeviceExternalBufferInfo const * const pExternalBufferInfo,
   VkExternalBufferProperties * const pExternalBufferProperties)
{
   switch (pExternalBufferInfo->handleType) {
#if !defined(_WIN32)
   case VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT:
   case VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT:
      pExternalBufferProperties->externalMemoryProperties.externalMemoryFeatures =
         VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT | VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT;
      pExternalBufferProperties->externalMemoryProperties.exportFromImportedHandleTypes =
         VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT |
         VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT;
      pExternalBufferProperties->externalMemoryProperties.compatibleHandleTypes =
         VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT |
         VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT;
      break;
#endif

   default:
      pExternalBufferProperties->externalMemoryProperties.externalMemoryFeatures = 0;
      pExternalBufferProperties->externalMemoryProperties.exportFromImportedHandleTypes = 0;
      /* From the VkExternalMemoryProperties specification:
       *
       *    compatibleHandleTypes must include at least handleType.
       */
      pExternalBufferProperties->externalMemoryProperties.compatibleHandleTypes =
         pExternalBufferInfo->handleType;
      break;
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_GetDeviceBufferMemoryRequirements(VkDevice const deviceHandle,
                                          VkDeviceBufferMemoryRequirements const * const pInfo,
                                          VkMemoryRequirements2 * const pMemoryRequirements)
{
   pMemoryRequirements->memoryRequirements.size = pInfo->pCreateInfo->size;

   VkDeviceSize alignment;
   VkBufferUsageFlags const usage = pInfo->pCreateInfo->usage;
   /* From the largest to the smallest alignment. */
   if (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
      alignment = TERAKAN_CONSTANT_CACHE_LINE_BYTES;
   } else if (usage &
              (VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT)) {
      /* Largest random access target element (R32G32B32A32) alignment, including for image to
       * buffer transfers.
       */
      alignment = sizeof(uint32_t) * 4;
   } else {
      alignment = sizeof(uint32_t);
   }
   pMemoryRequirements->memoryRequirements.alignment = alignment;

   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);
   struct terakan_physical_device const * const physical_device =
      container_of(device->vk.physical, struct terakan_physical_device const, vk);
   pMemoryRequirements->memoryRequirements.memoryTypeBits =
      ((uint32_t)1 << physical_device->memory_properties.memoryTypeCount) - 1;

   vk_foreach_struct (ext, pMemoryRequirements->pNext) {
      switch (ext->sType) {
      case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS: {
         VkMemoryDedicatedRequirements * const dedicated_requirements =
            (VkMemoryDedicatedRequirements *)ext;
         dedicated_requirements->requiresDedicatedAllocation = VK_FALSE;
         dedicated_requirements->prefersDedicatedAllocation = VK_FALSE;
      } break;

      default:
         break;
      }
   }
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_BindBufferMemory2(VkDevice const device, uint32_t const bindInfoCount,
                          VkBindBufferMemoryInfo const * const pBindInfos)
{
   for (uint32_t bind_info_index = 0; bind_info_index < bindInfoCount; ++bind_info_index) {
      VkBindBufferMemoryInfo const * const bind_info = &pBindInfos[bind_info_index];
      struct terakan_buffer * const buffer = terakan_buffer_from_handle(bind_info->buffer);
      buffer->bo = terakan_device_memory_from_handle(bind_info->memory)->bo;
      buffer->bo_offset = bind_info->memoryOffset;
   }

   return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
terakan_DestroyBuffer(VkDevice const deviceHandle, VkBuffer const bufferHandle,
                      VkAllocationCallbacks const * const pAllocator)
{
   struct terakan_buffer * const buffer = terakan_buffer_from_handle(bufferHandle);

   if (buffer == NULL) {
      return;
   }

   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);

   vk_buffer_finish(&buffer->vk);

   vk_free2(&device->vk.alloc, pAllocator, buffer);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreateBuffer(VkDevice const deviceHandle, VkBufferCreateInfo const * const pCreateInfo,
                     VkAllocationCallbacks const * const pAllocator, VkBuffer * const pBuffer)
{
   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   struct terakan_buffer * const buffer =
      vk_alloc2(&device->vk.alloc, pAllocator, sizeof(struct terakan_buffer),
                alignof(struct terakan_buffer), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (buffer == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   vk_buffer_init(&device->vk, &buffer->vk, pCreateInfo);

   buffer->bo = NULL;
   buffer->bo_offset = 0;

   *pBuffer = terakan_buffer_to_handle(buffer);

   return VK_SUCCESS;
}
