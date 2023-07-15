#ifndef NVK_INSTANCE_H
#define NVK_INSTANCE_H 1

#include "nvk_private.h"

#include "vulkan/runtime/vk_instance.h"

struct nvk_instance {
   struct vk_instance vk;
};

VK_DEFINE_HANDLE_CASTS(nvk_instance, vk.base, VkInstance, VK_OBJECT_TYPE_INSTANCE)

#endif
