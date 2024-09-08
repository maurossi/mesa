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

#include "terakan_device.h"

#include "winsys/terakan_winsys.h"
#include "terakan_command_buffer.h"
#include "terakan_entrypoints.h"
#include "terakan_physical_device.h"
#include "terakan_queue.h"

#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"
#include "wsi_common.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

static void
terakan_device_destroy(struct terakan_device * const device)
{
   if (device->queue_graphics != NULL) {
      terakan_queue_destroy(device->queue_graphics);
   }

   mtx_lock(&device->completion_mutex);
   device->completion_lost = true;
   mtx_unlock(&device->completion_mutex);
   cnd_broadcast(&device->completion_condition);

   cnd_destroy(&device->completion_condition);
   mtx_destroy(&device->completion_mutex);

   terakan_winsys_bo_free(device->meta_shaders_bo);

   vk_device_finish(&device->vk);

   vk_free(&device->vk.alloc, device);
}

VKAPI_ATTR void VKAPI_CALL
terakan_DestroyDevice(VkDevice const deviceHandle, VkAllocationCallbacks const * const pAllocator)
{
   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   if (device == NULL) {
      return;
   }

   terakan_device_destroy(device);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreateDevice(VkPhysicalDevice const physicalDevice,
                     VkDeviceCreateInfo const * const pCreateInfo,
                     VkAllocationCallbacks const * const pAllocator, VkDevice * const pDevice)
{
   VkResult result;

   struct terakan_physical_device * const physical_device =
      terakan_physical_device_from_handle(physicalDevice);

   struct terakan_device * const device =
      vk_alloc2(&physical_device->vk.instance->alloc, pAllocator, sizeof(struct terakan_device),
                alignof(struct terakan_device), VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (device == NULL) {
      return vk_error(physical_device->vk.instance, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   struct vk_device_dispatch_table dispatch_table;
   vk_device_dispatch_table_from_entrypoints(&dispatch_table, &terakan_device_entrypoints, true);
   vk_device_dispatch_table_from_entrypoints(&dispatch_table, &wsi_device_entrypoints, false);

   result =
      vk_device_init(&device->vk, &physical_device->vk, &dispatch_table, pCreateInfo, pAllocator);
   if (result != VK_SUCCESS) {
      goto fail_alloc;
   }

   device->vk.command_buffer_ops = &terakan_command_buffer_ops;

   bool const is_r9xx = physical_device->winsys->gpu_info.gfx_level >= CAYMAN;

   VkDeviceSize meta_shaders_bo_size = 0;
   for (size_t meta_shader_index = 0; meta_shader_index < TERAKAN_META_SHADER_COUNT;
        ++meta_shader_index) {
      struct terakan_shader_static * const device_meta_shader =
         &device->meta_shaders[meta_shader_index];
      struct terakan_meta_shader const * const meta_shader =
         terakan_meta_shaders[meta_shader_index];
      struct terakan_meta_shader_description const * const meta_shader_description =
         is_r9xx ? &meta_shader->r9xx : &meta_shader->r8xx;
      *device_meta_shader = meta_shader_description->static_registers;
      VkDeviceSize const meta_shader_offset =
         ALIGN_POT(meta_shaders_bo_size, (VkDeviceSize)TERAKAN_SHADER_PROGRAM_ALIGNMENT);
      device_meta_shader->program_start =
         meta_shader_offset >> TERAKAN_SHADER_PROGRAM_ALIGNMENT_LOG2;
      meta_shaders_bo_size = meta_shader_offset + meta_shader_description->program_size_bytes;
   }
   device->meta_shaders_bo = physical_device->winsys->bo_fn->allocate_device_memory(
      physical_device->winsys, meta_shaders_bo_size, TERAKAN_SHADER_PROGRAM_ALIGNMENT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      0);
   if (device->meta_shaders_bo == NULL) {
      result = vk_errorf(physical_device->vk.instance, VK_ERROR_OUT_OF_DEVICE_MEMORY,
                         "Failed to allocate memory for internal shaders");
      goto fail_device;
   }
   {
      char * const meta_shaders_bo_mapping = terakan_winsys_bo_map(device->meta_shaders_bo);
      if (meta_shaders_bo_mapping == NULL) {
         result = vk_errorf(physical_device->vk.instance, VK_ERROR_OUT_OF_HOST_MEMORY,
                            "Failed to map the internal shaders buffer object");
         goto fail_meta_shaders_bo;
      }
      for (size_t meta_shader_index = 0; meta_shader_index < TERAKAN_META_SHADER_COUNT;
           ++meta_shader_index) {
         struct terakan_shader_static * const device_meta_shader =
            &device->meta_shaders[meta_shader_index];
         device_meta_shader->program_bo = device->meta_shaders_bo;
         struct terakan_meta_shader const * const meta_shader =
            terakan_meta_shaders[meta_shader_index];
         struct terakan_meta_shader_description const * const meta_shader_description =
            is_r9xx ? &meta_shader->r9xx : &meta_shader->r8xx;
         memcpy(meta_shaders_bo_mapping + ((VkDeviceSize)device_meta_shader->program_start
                                           << TERAKAN_SHADER_PROGRAM_ALIGNMENT_LOG2),
                meta_shader_description->program, meta_shader_description->program_size_bytes);
      }
      terakan_winsys_bo_unmap(device->meta_shaders_bo);
   }

   if (mtx_init(&device->completion_mutex, mtx_plain) != thrd_success) {
      result = vk_errorf(physical_device->vk.instance, VK_ERROR_OUT_OF_HOST_MEMORY,
                         "Failed to initialize the submission mutex");
      goto fail_meta_shaders_bo;
   }
   if (cnd_init(&device->completion_condition) != thrd_success) {
      result = vk_errorf(physical_device->vk.instance, VK_ERROR_OUT_OF_HOST_MEMORY,
                         "Failed to initialize the submission condition variable");
      goto fail_completion_mutex;
   }

   device->completion_lost = false;

   device->queue_graphics = NULL;

   /* The device can be finished at this point in case of an error. Create optional queues. */

   for (uint32_t queue_create_info_index = 0;
        queue_create_info_index < pCreateInfo->queueCreateInfoCount; ++queue_create_info_index) {
      VkDeviceQueueCreateInfo const * const queue_create_info =
         &pCreateInfo->pQueueCreateInfos[queue_create_info_index];
      if (queue_create_info->queueFamilyIndex == 0) {
         if (queue_create_info->queueCount != 1 || device->queue_graphics != NULL) {
            terakan_device_destroy(device);
            return vk_errorf(physical_device->vk.instance, VK_ERROR_INITIALIZATION_FAILED,
                             "Only one graphics queue can be created");
         }
         result = terakan_queue_create(device, queue_create_info, 0, &device->queue_graphics);
         if (result != VK_SUCCESS) {
            terakan_device_destroy(device);
            return result;
         }
      } else {
         terakan_device_destroy(device);
         return vk_errorf(physical_device->vk.instance, VK_ERROR_INITIALIZATION_FAILED,
                          "Unknown queue family requested");
      }
   }

   *pDevice = terakan_device_to_handle(device);

   return VK_SUCCESS;

fail_completion_mutex:
   mtx_destroy(&device->completion_mutex);
fail_meta_shaders_bo:
   terakan_winsys_bo_free(device->meta_shaders_bo);
fail_device:
   vk_device_finish(&device->vk);
fail_alloc:
   vk_free(&device->vk.alloc, device);
   return result;
}
