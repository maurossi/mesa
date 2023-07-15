#ifndef NVK_IMAGE_H
#define NVK_IMAGE_H 1

#include "nvk_private.h"
#include "nvk_device_memory.h"

#include "nouveau_bo.h"
#include "nouveau_push.h"
#include "vulkan/runtime/vk_image.h"

#define NVK_MAX_MIP_LEVELS 7

/* x can either be 0x0 or 0xe
 *   0x0: 64 blocks
 *   0xe: 16 blocks (not quite sure how that's even used, so we don't use it)
 *
 * tile size on y depends on the is_fermi flag:
 *   !is_fermi: 4 << y
 *    is_fermi: 8 << y (required Pascal+)
 *
 * tile size on z is 1 << z
 */
struct nvk_tile {
   uint8_t z:4;
   uint8_t y:4;
   uint8_t x:4;
   bool is_fermi:1;
   bool is_tiled:1;
};

struct nvk_format;

struct nvk_image_level {
   VkDeviceSize offset;
   VkExtent3D extent;

   uint32_t row_stride;
   uint32_t layer_stride;
   struct nvk_tile tile;
};

struct nvk_image {
   struct vk_image vk;
   struct nvk_device_memory *mem;
   VkDeviceSize offset;

   VkDeviceSize min_size;

   struct nvk_format *format;
   struct nvk_image_level level[NVK_MAX_MIP_LEVELS];
};

VK_DEFINE_HANDLE_CASTS(nvk_image, vk.base, VkImage, VK_OBJECT_TYPE_IMAGE)

static void
nvk_push_image_ref(struct nouveau_ws_push *push,
                   const struct nvk_image *image,
                   enum nouveau_ws_bo_map_flags flags)
{
   nouveau_ws_push_ref(push, image->mem->bo, flags);
}

static inline uint64_t
nvk_image_base_address(struct nvk_image *image, uint32_t level)
{
   return image->mem->bo->offset + image->offset + image->level[level].offset;
}

#endif
