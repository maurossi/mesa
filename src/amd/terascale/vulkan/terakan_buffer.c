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

#include "terakan_buffer.h"

#include "terakan_descriptor.h"
#include "terakan_device.h"
#include "terakan_device_memory.h"
#include "terakan_entrypoints.h"
#include "terakan_format.h"
#include "terakan_physical_device.h"

#include "util/macros.h"
#include "util/u_endian.h"
#include "vk_alloc.h"
#include "vk_format.h"
#include "vk_log.h"
#include "vk_util.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct terakan_bo const *
terakan_buffer_create_uniform_buffer_descriptor(VkDescriptorBufferInfo const * const buffer_info,
                                                uint32_t resource_out[8])
{
   struct terakan_buffer const * const buffer = terakan_buffer_from_handle(buffer_info->buffer);
   if (buffer == NULL) {
      return NULL;
   }
   if (!terakan_descriptor_create_for_uniform_buffer(
          buffer->bo, buffer->va + buffer_info->offset,
          vk_buffer_range(&buffer->vk, buffer_info->offset, buffer_info->range), resource_out)) {
      return NULL;
   }
   return buffer->bo;
}

struct terakan_bo const *
terakan_buffer_create_storage_buffer_descriptor(VkDescriptorBufferInfo const * const buffer_info,
                                                uint32_t resource_out[8],
                                                struct terakan_color_descriptor * const color_out)
{
   struct terakan_buffer const * const buffer = terakan_buffer_from_handle(buffer_info->buffer);
   if (buffer == NULL) {
      return NULL;
   }
   if (!terakan_descriptor_create_for_storage_buffer(
          buffer->bo, buffer->va + buffer_info->offset,
          vk_buffer_range(&buffer->vk, buffer_info->offset, buffer_info->range),
          container_of(buffer->vk.base.device->physical, struct terakan_physical_device const, vk)
             ->tiling_info.pipe_interleave_bytes_log2,
          resource_out, color_out)) {
      return NULL;
   }
   return buffer->bo;
}

VKAPI_ATTR void VKAPI_CALL
terakan_GetPhysicalDeviceExternalBufferProperties(
   UNUSED VkPhysicalDevice const physicalDevice,
   VkPhysicalDeviceExternalBufferInfo const * const pExternalBufferInfo,
   VkExternalBufferProperties * const pExternalBufferProperties)
{
   pExternalBufferProperties->externalMemoryProperties.externalMemoryFeatures = 0;
   pExternalBufferProperties->externalMemoryProperties.exportFromImportedHandleTypes = 0;
   pExternalBufferProperties->externalMemoryProperties.compatibleHandleTypes = 0;

   struct terakan_physical_device const * const device =
      terakan_physical_device_from_handle(physicalDevice);

   VkExternalMemoryHandleTypeFlags const supported_handle_types =
      terakan_physical_device_supported_external_memory_types(device);

   if (supported_handle_types & pExternalBufferInfo->handleType) {
      switch (pExternalBufferInfo->handleType) {
      case VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT:
      case VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT: {
         pExternalBufferProperties->externalMemoryProperties.externalMemoryFeatures =
            VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT | VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT;
         VkExternalMemoryHandleTypeFlags const supported_fd_types =
            supported_handle_types & (VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT |
                                      VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT);
         pExternalBufferProperties->externalMemoryProperties.exportFromImportedHandleTypes =
            supported_fd_types;
         pExternalBufferProperties->externalMemoryProperties.compatibleHandleTypes =
            supported_fd_types;
      } break;

      default:
         break;
      }
   }

   /* From the VkExternalMemoryProperties specification:
    *
    *    compatibleHandleTypes must include at least handleType.
    */
   pExternalBufferProperties->externalMemoryProperties.compatibleHandleTypes |=
      pExternalBufferInfo->handleType;
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
      alignment = TERAKAN_KCACHE_HW_LINE_BYTES;
   } else if (usage &
              (VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT)) {
      /* Largest random access target element (R32G32B32A32) alignment, including for image to
       * buffer transfers.
       */
      alignment = sizeof(uint32_t) * 4;
   } else {
      /* Required in many places: largest in vertex fetch instructions, largest possible vertex
       * index size, a storage buffer vertex fetch and UAV element, indirect buffer alignment.
       * Additionally, because storage buffers use 32-bit UAVs, and with VK_EXT_robustness2 their
       * ranges are rounded up (not down) to robustStorageBufferAccessSizeAlignment, which is
       * sizeof(uint32_t) because of that, aligning all buffers and images to sizeof(uint32_t)
       * prevents writes to the end of storage buffers with an unaligned size from affecting the
       * resource placed next to them.
       */
      alignment = sizeof(uint32_t);
   }
   pMemoryRequirements->memoryRequirements.alignment = alignment;

   struct terakan_physical_device const * const physical_device =
      terakan_device_physical_device(terakan_device_from_handle(deviceHandle));
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
terakan_BindBufferMemory2(UNUSED VkDevice const device, uint32_t const bindInfoCount,
                          VkBindBufferMemoryInfo const * const pBindInfos)
{
   for (uint32_t bind_info_index = 0; bind_info_index < bindInfoCount; ++bind_info_index) {
      VkBindBufferMemoryInfo const * const bind_info = &pBindInfos[bind_info_index];
      struct terakan_buffer * const buffer = terakan_buffer_from_handle(bind_info->buffer);
      buffer->bo = terakan_device_memory_from_handle(bind_info->memory)->bo;
      buffer->va = buffer->bo->va + bind_info->memoryOffset;
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
   buffer->va = 0;

   *pBuffer = terakan_buffer_to_handle(buffer);
   return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
terakan_DestroyBufferView(VkDevice const deviceHandle, VkBufferView const bufferView,
                          VkAllocationCallbacks const * const pAllocator)
{
   struct terakan_buffer_view * const buffer_view = terakan_buffer_view_from_handle(bufferView);

   if (buffer_view == NULL) {
      return;
   }

   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);

   vk_buffer_view_finish(&buffer_view->vk);

   vk_free2(&device->vk.alloc, pAllocator, buffer_view);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreateBufferView(VkDevice const deviceHandle,
                         VkBufferViewCreateInfo const * const pCreateInfo,
                         VkAllocationCallbacks const * const pAllocator, VkBufferView * const pView)
{
   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   struct terakan_buffer_view * const buffer_view =
      vk_alloc2(&device->vk.alloc, pAllocator, sizeof(struct terakan_buffer_view),
                alignof(struct terakan_buffer_view), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (buffer_view == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   vk_buffer_view_init(&device->vk, &buffer_view->vk, pCreateInfo);

   struct terakan_buffer const * const buffer = terakan_buffer_from_handle(pCreateInfo->buffer);

   buffer_view->bo = buffer->bo;

   memset(buffer_view->resource, 0, sizeof(buffer_view->resource));
   memset(&buffer_view->color, 0, sizeof(buffer_view->color));

   /* As of Vulkan 1.3.262, the requirement that `range` must be greater than 0 applies only to
    * non-VK_WHOLE_SIZE `range` (VUID-VkBufferViewCreateInfo-range-00928).
    */
   if (buffer_view->vk.elements != 0) {
      struct terascale_format_info const format_info =
         terascale_format_info_r8xx[vk_format_to_pipe_format(pCreateInfo->format)];
      if (format_info.supports_sq_vertex_fetch) {
         uint64_t const va = buffer->va + pCreateInfo->offset;

         unsigned const bytes_per_element = terascale_format_bytes_per_block[format_info.format];

         enum terascale_endian_swap const endian_swap =
            UTIL_ARCH_BIG_ENDIAN ? terascale_format_big_endian_swap[format_info.format]
                                 : TERASCALE_ENDIAN_SWAP_NONE;

         buffer_view->resource[0] = (uint32_t)va;
         buffer_view->resource[1] = (uint32_t)(bytes_per_element * buffer_view->vk.elements - 1);
         buffer_view->resource[2] =
            S_030008_BASE_ADDRESS_HI(va >> 32) | S_030008_STRIDE(bytes_per_element) |
            S_030008_DATA_FORMAT(format_info.format) |
            S_030008_NUM_FORMAT_ALL(terascale_format_get_sq_num_format(
               (enum terascale_format_number_type)format_info.number_type)) |
            S_030008_FORMAT_COMP_ALL(format_info.channels_signed != 0b0) |
            S_030008_ENDIAN_SWAP(endian_swap);
         buffer_view->resource[3] =
            S_03000C_DST_SEL_X(format_info.swizzle_r) | S_03000C_DST_SEL_Y(format_info.swizzle_g) |
            S_03000C_DST_SEL_Z(format_info.swizzle_b) | S_03000C_DST_SEL_W(format_info.swizzle_a);
         buffer_view->resource[4] = (uint32_t)buffer_view->vk.elements;
         buffer_view->resource[7] = S_03001C_TYPE(V_03001C_SQ_TEX_VTX_VALID_BUFFER);
         buffer_view->resource[TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD] =
            TERAKAN_BO_PRIORITY_SHADER_READ_BUFFER;

         /* The vertex buffer format is also used for the UAV IMMED buffer, so creating the UAV
          * inside the vertex format supported conditional too.
          */

         if (format_info.supports_cb_color && pCreateInfo->offset % bytes_per_element == 0) {
            terakan_color_descriptor_calculate_buffer_base_pitch_view_dim(
               &buffer_view->color, va, buffer_view->vk.elements, bytes_per_element,
               terakan_device_physical_device(device)->tiling_info.pipe_interleave_bytes_log2);
            buffer_view->color.info =
               S_028C70_ENDIAN(endian_swap) | S_028C70_FORMAT(format_info.format) |
               S_028C70_ARRAY_MODE(V_028C70_ARRAY_LINEAR_ALIGNED) |
               S_028C70_NUMBER_TYPE(format_info.number_type) |
               S_028C70_COMP_SWAP(format_info.cb_color_swap) | S_028C70_BLEND_BYPASS(1) |
               S_028C70_SOURCE_FORMAT(V_028C70_EXPORT_4C_32BPC) | S_028C70_RAT(1) |
               S_028C70_RESOURCE_TYPE(V_028C70_BUFFER);
            buffer_view->color.attrib = S_028C74_NON_DISP_TILING_ORDER(1);
         }
      }
   }

   *pView = terakan_buffer_view_to_handle(buffer_view);
   return VK_SUCCESS;
}
