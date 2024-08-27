/*
 * Copyright © 2022 Collabora Ltd. and Red Hat Inc.
 * SPDX-License-Identifier: MIT
 */
#ifndef NVK_INSTANCE_H
#define NVK_INSTANCE_H 1

#include "nvk_private.h"

#include "nvk_debug.h"
#include "vk_instance.h"
#include "util/xmlconfig.h"

struct nvk_instance {
   struct vk_instance vk;

   enum nvk_debug debug_flags;

   struct driOptionCache dri_options;
   struct driOptionCache available_dri_options;

   uint8_t driver_build_sha[20];
   uint32_t force_vk_vendor;
};

VKAPI_ATTR VkResult VKAPI_CALL
nvk_CreateInstance(const VkInstanceCreateInfo *pCreateInfo,
                   const VkAllocationCallbacks *pAllocator,
                   VkInstance *pInstance);

VKAPI_ATTR VkResult VKAPI_CALL
nvk_EnumerateInstanceExtensionProperties(const char *pLayerName,
                                         uint32_t *pPropertyCount,
                                         VkExtensionProperties *pProperties);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
nvk_GetInstanceProcAddr(VkInstance _instance, const char *pName);

VK_DEFINE_HANDLE_CASTS(nvk_instance, vk.base, VkInstance, VK_OBJECT_TYPE_INSTANCE)

#endif
