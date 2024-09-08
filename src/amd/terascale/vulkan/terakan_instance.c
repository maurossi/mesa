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

#include "terakan_instance.h"

#include "terakan_descriptor.h"
#include "terakan_entrypoints.h"
#include "terakan_limits.h"
#include "terakan_physical_device.h"

#include "util/macros.h"
#include "util/u_debug.h"
#include "util/u_math.h"
#include "vk_alloc.h"
#include "vk_log.h"
#include "wsi_common.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static struct debug_control const terakan_debug_options[] = {{"startup", TERAKAN_DEBUG_STARTUP},
                                                             {NULL, 0}};

static struct vk_instance_extension_table const terakan_instance_extensions_supported = {
   .EXT_debug_report = true,
   .EXT_debug_utils = true,
   .KHR_external_memory_capabilities = true,
   .KHR_get_physical_device_properties2 = true,

#if defined(TERAKAN_USE_WSI_PLATFORM)
   .KHR_surface = true,
#if defined(VK_USE_PLATFORM_XCB_KHR)
   .KHR_xcb_surface = true,
#endif
#if defined(VK_USE_PLATFORM_XLIB_KHR)
   .KHR_xlib_surface = true,
#endif
#endif
};

VKAPI_ATTR VkResult VKAPI_CALL
terakan_EnumerateInstanceExtensionProperties(char const * const pLayerName,
                                             uint32_t * const pPropertyCount,
                                             VkExtensionProperties * const pProperties)
{
   if (pLayerName != NULL) {
      return vk_error(NULL, VK_ERROR_LAYER_NOT_PRESENT);
   }

   return vk_enumerate_instance_extension_properties(&terakan_instance_extensions_supported,
                                                     pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_EnumerateInstanceLayerProperties(uint32_t * const pPropertyCount,
                                         VkLayerProperties * const pProperties)
{
   if (pProperties == NULL) {
      *pPropertyCount = 0;
      return VK_SUCCESS;
   }

   /* None supported at this time. */
   return vk_error(NULL, VK_ERROR_LAYER_NOT_PRESENT);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_EnumerateInstanceVersion(uint32_t * const pApiVersion)
{
   *pApiVersion = TERAKAN_API_VERSION;
   return VK_SUCCESS;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
terakan_GetInstanceProcAddr(VkInstance const instanceHandle, char const * const pName)
{
   struct vk_instance const * const instance = vk_instance_from_handle(instanceHandle);
   return vk_instance_get_proc_addr(instance, &terakan_instance_entrypoints, pName);
}

VKAPI_ATTR void VKAPI_CALL
terakan_DestroyInstance(VkInstance const instanceHandle,
                        VkAllocationCallbacks const * const pAllocator)
{
   struct terakan_instance * const instance = terakan_instance_from_handle(instanceHandle);

   if (instance == NULL) {
      return;
   }

   vk_instance_finish(&instance->vk);
   vk_free(&instance->vk.alloc, instance);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreateInstance(VkInstanceCreateInfo const * const pCreateInfo,
                       VkAllocationCallbacks const * pAllocator, VkInstance * const pInstance)
{
   VkResult result;

   if (pAllocator == NULL) {
      pAllocator = vk_default_allocator();
   }

   struct terakan_instance * const instance =
      vk_alloc(pAllocator, sizeof(struct terakan_instance), alignof(struct terakan_instance),
               VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (instance == NULL) {
      return vk_error(NULL, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   struct vk_instance_dispatch_table dispatch_table;
   vk_instance_dispatch_table_from_entrypoints(&dispatch_table, &terakan_instance_entrypoints,
                                               true);
   vk_instance_dispatch_table_from_entrypoints(&dispatch_table, &wsi_instance_entrypoints, false);

   result = vk_instance_init(&instance->vk, &terakan_instance_extensions_supported, &dispatch_table,
                             pCreateInfo, pAllocator);
   if (result != VK_SUCCESS) {
      vk_free(pAllocator, instance);
      return vk_error(NULL, result);
   }

   instance->debug_flags = parse_debug_string(getenv("TERAKAN_DEBUG"), terakan_debug_options);

   /* Allocate binding spaces. */
   /* TODO(Triang3l): Binding space allocation configuration via environment variables or
    * application-specific workarounds.
    */

   /* Vulkan requires at least 4 storage buffers and 4 storage images.
    *
    * Direct3D 11.0 provides 8 Unordered Access Views (UAVs) in compute shaders and 8 combined RTVs
    * and UAVs in pixel shaders.
    *
    * By default, expose 4 storage buffers and 8 storage images.
    *
    * The RATionale is that all Direct3D 11 UAV types, including structured and raw buffers, can be
    * implemented as Vulkan storage images, so there need to be at least as many storage image
    * bindings as Direct3D 11.0 UAVs.
    *
    * Each UAV in Direct3D 11 can also have a 32-bit counter. Vulkan doesn't have a similar concept,
    * a separate storage buffer/image binding is generally used for the counter. Unfortunately,
    * there are not enough hardware RATs to make it possible to do that. However, there are 4
    * storage buffer slots, and the translation layer may use one of them to store counters of all
    * of the bound UAVs.
    */
   instance->max_per_stage_storage_buffers = 4;

   /* Direct3D 11 limit. */
   instance->max_per_stage_uniform_buffers = 15;
   /* 4 is the Vulkan minimum, but use all space not usable by vertex stages. */
   instance->max_per_stage_input_attachments =
      MAX2(TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL -
              TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL,
           4);
   instance->max_per_stage_sampled_images =
      MIN2(TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL - TERAKAN_LIMITS_HW_COLOR_RAT_COUNT -
              instance->max_per_stage_uniform_buffers,
           TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL - TERAKAN_LIMITS_HW_COLOR_RAT_COUNT -
              instance->max_per_stage_uniform_buffers - instance->max_per_stage_input_attachments);

   /* Initialize physical device management. */

   instance->vk.physical_devices.try_create_for_drm = terakan_physical_device_try_create_for_drm;

   instance->vk.physical_devices.destroy = terakan_physical_device_destroy;

   if (instance->debug_flags & TERAKAN_DEBUG_STARTUP) {
      fputs("terakan: info: Created an instance.\n", stderr);
   }

   *pInstance = terakan_instance_to_handle(instance);

   return VK_SUCCESS;
}

/* Windows will use a DLL definition file to avoid build errors. */
#if defined(_WIN32)
#undef PUBLIC
#define PUBLIC
#endif

/* The loader wants us to expose a second GetInstanceProcAddr function
 * to work around certain LD_PRELOAD issues seen in apps.
 */
PUBLIC
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vk_icdGetInstanceProcAddr(VkInstance const instanceHandle, char const * const pName)
{
   return terakan_GetInstanceProcAddr(instanceHandle, pName);
}
