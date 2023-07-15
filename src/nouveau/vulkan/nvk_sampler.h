#ifndef NVK_SAMPLER_H
#define NVK_SAMPLER_H 1

#include "nvk_private.h"

#include "vulkan/runtime/vk_object.h"

struct nvk_sampler {
   struct vk_object_base base;

   uint32_t desc_index;

   /** Number of planes for multi-plane images.
    * Hard-coded as 1 as a placeholder until YCbCr conversion
    * structs are implemented
    */
   uint8_t plane_count;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(nvk_sampler, base, VkSampler, VK_OBJECT_TYPE_SAMPLER)

#endif
