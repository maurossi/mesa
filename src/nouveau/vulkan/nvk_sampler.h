#ifndef NVK_SAMPLER_H
#define NVK_SAMPLER_H 1

#include "nvk_private.h"

#include "vulkan/runtime/vk_object.h"

struct nvk_sampler {
   struct vk_object_base base;

   uint32_t desc_index;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(nvk_sampler, base, VkSampler, VK_OBJECT_TYPE_SAMPLER)

#endif
