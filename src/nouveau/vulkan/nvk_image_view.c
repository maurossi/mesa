#include "nvk_image_view.h"

#include "nvk_device.h"
#include "nvk_physical_device.h"
#include "nvk_format.h"
#include "nvk_image.h"
#include "vulkan/util/vk_format.h"

static enum pipe_texture_target
vk_image_view_type_to_pipe_texture_target(VkImageViewType view_type)
{
   switch (view_type) {
   case VK_IMAGE_VIEW_TYPE_1D:         return PIPE_TEXTURE_1D;
   case VK_IMAGE_VIEW_TYPE_2D:         return PIPE_TEXTURE_2D;
   case VK_IMAGE_VIEW_TYPE_3D:         return PIPE_TEXTURE_3D;
   case VK_IMAGE_VIEW_TYPE_CUBE:       return PIPE_TEXTURE_CUBE;
   case VK_IMAGE_VIEW_TYPE_1D_ARRAY:   return PIPE_TEXTURE_1D_ARRAY;
   case VK_IMAGE_VIEW_TYPE_2D_ARRAY:   return PIPE_TEXTURE_2D_ARRAY;
   case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY: return PIPE_TEXTURE_CUBE_ARRAY;
   default:
      unreachable("Invalid image view type");
   }
}

static enum pipe_swizzle
vk_swizzle_to_pipe(VkComponentSwizzle swizzle)
{
   switch (swizzle) {
   case VK_COMPONENT_SWIZZLE_R:     return PIPE_SWIZZLE_X;
   case VK_COMPONENT_SWIZZLE_G:     return PIPE_SWIZZLE_Y;
   case VK_COMPONENT_SWIZZLE_B:     return PIPE_SWIZZLE_Z;
   case VK_COMPONENT_SWIZZLE_A:     return PIPE_SWIZZLE_W;
   case VK_COMPONENT_SWIZZLE_ONE:   return PIPE_SWIZZLE_1;
   case VK_COMPONENT_SWIZZLE_ZERO:  return PIPE_SWIZZLE_0;
   default:
      unreachable("Invalid component swizzle");
   }
}

static void
image_3d_view_as_2d_array(struct nil_image *image,
                          struct nil_view *view,
                          uint64_t *base_addr)
{
   assert(view->target == PIPE_TEXTURE_2D ||
          view->target == PIPE_TEXTURE_2D_ARRAY);
   assert(view->num_levels == 1);

   uint64_t offset_B;
   nil_image_3d_level_as_2d_array(image, view->base_level, image, &offset_B);
   *base_addr += offset_B;
   view->base_level = 0;
}

static enum pipe_format
get_stencil_format(enum pipe_format format)
{
   switch (format) {
   case PIPE_FORMAT_S8_UINT:              return PIPE_FORMAT_S8_UINT;
   case PIPE_FORMAT_Z24_UNORM_S8_UINT:    return PIPE_FORMAT_X24S8_UINT;
   case PIPE_FORMAT_S8_UINT_Z24_UNORM:    return PIPE_FORMAT_S8X24_UINT;
   case PIPE_FORMAT_Z32_FLOAT_S8X24_UINT: return PIPE_FORMAT_X32_S8X24_UINT;
   default: unreachable("Unsupported depth/stencil format");
   }
}

VkResult
nvk_image_view_init(struct nvk_device *device,
                    struct nvk_image_view *view,
                    bool driver_internal,
                    const VkImageViewCreateInfo *pCreateInfo)
{
   VK_FROM_HANDLE(nvk_image, image, pCreateInfo->image);
   VkResult result;

   memset(view, 0, sizeof(*view));

   vk_image_view_init(&device->vk, &view->vk, driver_internal, pCreateInfo);

   struct nil_image nil_image = image->nil;
   uint64_t base_addr = nvk_image_base_address(image);

   enum pipe_format p_format = vk_format_to_pipe_format(view->vk.format);
   if (view->vk.aspects == VK_IMAGE_ASPECT_STENCIL_BIT)
      p_format = get_stencil_format(p_format);

   struct nil_view nil_view = {
      .target = vk_image_view_type_to_pipe_texture_target(view->vk.view_type),
      .format = p_format,
      .base_level = view->vk.base_mip_level,
      .num_levels = view->vk.level_count,
      .base_array_layer = view->vk.base_array_layer,
      .array_len = view->vk.layer_count,
      .swizzle = {
         vk_swizzle_to_pipe(view->vk.swizzle.r),
         vk_swizzle_to_pipe(view->vk.swizzle.g),
         vk_swizzle_to_pipe(view->vk.swizzle.b),
         vk_swizzle_to_pipe(view->vk.swizzle.a),
      },
   };

   if (nil_image.dim == NIL_IMAGE_DIM_3D &&
       nil_view.target != PIPE_TEXTURE_3D)
      image_3d_view_as_2d_array(&nil_image, &nil_view, &base_addr);

   if (view->vk.usage & (VK_IMAGE_USAGE_SAMPLED_BIT |
                         VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)) {
      uint32_t tic[8];
      nil_image_fill_tic(&nvk_device_physical(device)->info,
                         &nil_image, &nil_view, base_addr, tic);

      result = nvk_descriptor_table_add(device, &device->images,
                                        tic, sizeof(tic),
                                        &view->sampled_desc_index);
      if (result != VK_SUCCESS) {
         nvk_image_view_finish(device, view);
         return result;
      }
   }

   if (view->vk.usage & VK_IMAGE_USAGE_STORAGE_BIT) {
      /* For storage images, we can't have any cubes */
      if (view->vk.view_type == VK_IMAGE_VIEW_TYPE_CUBE ||
          view->vk.view_type == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY)
         nil_view.target = PIPE_TEXTURE_2D_ARRAY;

      if (view->vk.view_type == VK_IMAGE_VIEW_TYPE_3D) {
         /* Without VK_AMD_shader_image_load_store_lod, the client can only
          * get at the first LOD from the shader anyway.
          */
         assert(view->vk.base_array_layer == 0);
         assert(view->vk.layer_count = 1);
         nil_view.target = PIPE_TEXTURE_2D_ARRAY;
         nil_view.num_levels = 1;
         nil_view.base_array_layer = 0;
         nil_view.array_len = view->vk.extent.depth;
         image_3d_view_as_2d_array(&nil_image, &nil_view, &base_addr);
      }

      uint32_t tic[8];
      nil_image_fill_tic(&nvk_device_physical(device)->info,
                         &nil_image, &nil_view, base_addr, tic);

      result = nvk_descriptor_table_add(device, &device->images,
                                        tic, sizeof(tic),
                                        &view->storage_desc_index);
      if (result != VK_SUCCESS) {
         nvk_image_view_finish(device, view);
         return result;
      }
   }

   return VK_SUCCESS;
}

void
nvk_image_view_finish(struct nvk_device *device,
                      struct nvk_image_view *view)
{
   if (view->sampled_desc_index) {
      nvk_descriptor_table_remove(device, &device->images,
                                  view->sampled_desc_index);
   }

   if (view->storage_desc_index) {
      nvk_descriptor_table_remove(device, &device->images,
                                  view->storage_desc_index);
   }

   vk_image_view_finish(&view->vk);
}

VKAPI_ATTR VkResult VKAPI_CALL
nvk_CreateImageView(VkDevice _device,
                    const VkImageViewCreateInfo *pCreateInfo,
                    const VkAllocationCallbacks *pAllocator,
                    VkImageView *pView)
{
   VK_FROM_HANDLE(nvk_device, device, _device);
   struct nvk_image_view *view;
   VkResult result;

   view = vk_alloc2(&device->vk.alloc, pAllocator, sizeof(*view), 8,
                    VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (!view)
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);

   result = nvk_image_view_init(device, view, false, pCreateInfo);
   if (result != VK_SUCCESS) {
      vk_free2(&device->vk.alloc, pAllocator, view);
      return result;
   }

   *pView = nvk_image_view_to_handle(view);

   return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
nvk_DestroyImageView(VkDevice _device,
                     VkImageView imageView,
                     const VkAllocationCallbacks *pAllocator)
{
   VK_FROM_HANDLE(nvk_device, device, _device);
   VK_FROM_HANDLE(nvk_image_view, view, imageView);

   if (!view)
      return;

   nvk_image_view_finish(device, view);
   vk_free2(&device->vk.alloc, pAllocator, view);
}
