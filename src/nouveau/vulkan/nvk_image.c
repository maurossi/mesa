#include "nvk_image.h"

#include "nvk_device.h"
#include "nvk_device_memory.h"
#include "nvk_format.h"
#include "nvk_physical_device.h"

#include "nil_format.h"
#include "vulkan/util/vk_format.h"

#include "clb197.h"

VkFormatFeatureFlags2
nvk_get_image_format_features(struct nvk_physical_device *pdev,
                              VkFormat vk_format, VkImageTiling tiling)
{
   VkFormatFeatureFlags2 features = 0;

   if (tiling != VK_IMAGE_TILING_OPTIMAL)
      return 0;

   enum pipe_format p_format = vk_format_to_pipe_format(vk_format);
   if (p_format == PIPE_FORMAT_NONE)
      return 0;

   if (!nil_format_supports_texturing(&pdev->info, p_format))
      return 0;

   /* You can't tile a non-power-of-two */
   if (!util_is_power_of_two_nonzero(util_format_get_blocksize(p_format)))
      return 0;

   features |= VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT;
   features |= VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT;
   features |= VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT;
   features |= VK_FORMAT_FEATURE_2_BLIT_SRC_BIT;

   if (nil_format_supports_filtering(&pdev->info, p_format)) {
      features |= VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
      if (pdev->info.cls_eng3d >= MAXWELL_B)
         features |= VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_MINMAX_BIT;
   }

   /* TODO: VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT */
   if (vk_format_has_depth(vk_format)) {
      features |= VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT;
   }

   if (nil_format_supports_color_targets(&pdev->info, p_format)) {
      features |= VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT;
      if (nil_format_supports_blending(&pdev->info, p_format))
         features |= VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT;
      features |= VK_FORMAT_FEATURE_2_BLIT_DST_BIT;
   }

   if (vk_format_is_depth_or_stencil(vk_format)) {
      if (vk_format == VK_FORMAT_D32_SFLOAT_S8_UINT)
         return 0; /* TODO */

      if (!nil_format_supports_depth_stencil(&pdev->info, p_format))
         return 0;

      features |= VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT;
   }

   if (nil_format_supports_storage(&pdev->info, p_format)) {
      features |= VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT |
                  VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT |
                  VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT;
   }

   if (p_format == PIPE_FORMAT_R32_UINT || p_format == PIPE_FORMAT_R32_SINT)
      features |= VK_FORMAT_FEATURE_2_STORAGE_IMAGE_ATOMIC_BIT;

   return features;
}

static VkFormatFeatureFlags2KHR
vk_image_usage_to_format_features(VkImageUsageFlagBits usage_flag)
{
   assert(util_bitcount(usage_flag) == 1);
   switch (usage_flag) {
   case VK_IMAGE_USAGE_TRANSFER_SRC_BIT:
      return VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT_KHR |
             VK_FORMAT_FEATURE_BLIT_SRC_BIT;
   case VK_IMAGE_USAGE_TRANSFER_DST_BIT:
      return VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT_KHR |
             VK_FORMAT_FEATURE_BLIT_DST_BIT;
   case VK_IMAGE_USAGE_SAMPLED_BIT:
      return VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT;
   case VK_IMAGE_USAGE_STORAGE_BIT:
      return VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT;
   case VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT:
      return VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT;
   case VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT:
      return VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT;
   default:
      return 0;
   }
}

VKAPI_ATTR VkResult VKAPI_CALL
nvk_GetPhysicalDeviceImageFormatProperties2(
   VkPhysicalDevice physicalDevice,
   const VkPhysicalDeviceImageFormatInfo2 *pImageFormatInfo,
   VkImageFormatProperties2 *pImageFormatProperties)
{
   VK_FROM_HANDLE(nvk_physical_device, pdevice, physicalDevice);

   /* Initialize to zero in case we return VK_ERROR_FORMAT_NOT_SUPPORTED */
   memset(&pImageFormatProperties->imageFormatProperties, 0,
          sizeof(pImageFormatProperties->imageFormatProperties));

   VkFormatFeatureFlags2KHR features =
      nvk_get_image_format_features(pdevice, pImageFormatInfo->format,
                                             pImageFormatInfo->tiling);
   if (features == 0)
      return VK_ERROR_FORMAT_NOT_SUPPORTED;

   if (vk_format_is_compressed(pImageFormatInfo->format) &&
       pImageFormatInfo->type != VK_IMAGE_TYPE_2D)
      return VK_ERROR_FORMAT_NOT_SUPPORTED;

   VkExtent3D maxExtent;
   uint32_t maxMipLevels;
   uint32_t maxArraySize;
   VkSampleCountFlags sampleCounts;
   switch (pImageFormatInfo->type) {
   case VK_IMAGE_TYPE_1D:
      maxExtent = (VkExtent3D) { 16384, 1, 1 },
      maxMipLevels = 15;
      maxArraySize = 2048;
      sampleCounts = VK_SAMPLE_COUNT_1_BIT;
      break;
   case VK_IMAGE_TYPE_2D:
      maxExtent = (VkExtent3D) { 16384, 16384, 1 };
      maxMipLevels = 15;
      maxArraySize = 2048;
      sampleCounts = VK_SAMPLE_COUNT_1_BIT |
                     VK_SAMPLE_COUNT_2_BIT |
                     VK_SAMPLE_COUNT_4_BIT |
                     VK_SAMPLE_COUNT_8_BIT;
      break;
   case VK_IMAGE_TYPE_3D:
      maxExtent = (VkExtent3D) { 2048, 2048, 2048 };
      maxMipLevels = 12;
      maxArraySize = 1;
      sampleCounts = VK_SAMPLE_COUNT_1_BIT;
      break;
   default:
      unreachable("Invalid image type");
   }

   /* From the Vulkan 1.2.199 spec:
    *
    *    "VK_IMAGE_CREATE_EXTENDED_USAGE_BIT specifies that the image can be
    *    created with usage flags that are not supported for the format the
    *    image is created with but are supported for at least one format a
    *    VkImageView created from the image can have."
    *
    * If VK_IMAGE_CREATE_EXTENDED_USAGE_BIT is set, views can be created with
    * different usage than the image so we can't always filter on usage.
    * There is one exception to this below for storage.
    */
   const VkImageUsageFlags image_usage = pImageFormatInfo->usage;
   VkImageUsageFlags view_usage = image_usage;
   if (pImageFormatInfo->flags & VK_IMAGE_CREATE_EXTENDED_USAGE_BIT)
      view_usage = 0;

   u_foreach_bit(b, view_usage) {
      VkFormatFeatureFlags2KHR usage_features =
         vk_image_usage_to_format_features(1 << b);
      if (usage_features && !(features & usage_features))
         return VK_ERROR_FORMAT_NOT_SUPPORTED;
   }

   pImageFormatProperties->imageFormatProperties = (VkImageFormatProperties) {
      .maxExtent = maxExtent,
      .maxMipLevels = maxMipLevels,
      .maxArrayLayers = maxArraySize,
      .sampleCounts = sampleCounts,
      .maxResourceSize = UINT32_MAX, /* TODO */
   };

   vk_foreach_struct(s, pImageFormatProperties->pNext) {
      switch (s->sType) {
      default:
         nvk_debug_ignored_stype(s->sType);
         break;
      }
   }

   return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
nvk_GetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t *pPropertyCount,
    VkSparseImageFormatProperties2 *pProperties)
{
   /* Sparse images are not yet supported. */
   *pPropertyCount = 0;
}

static enum nil_image_dim
vk_image_type_to_nil_dim(VkImageType type)
{
   switch (type) {
   case VK_IMAGE_TYPE_1D:  return NIL_IMAGE_DIM_1D;
   case VK_IMAGE_TYPE_2D:  return NIL_IMAGE_DIM_2D;
   case VK_IMAGE_TYPE_3D:  return NIL_IMAGE_DIM_3D;
   default:
      unreachable("Invalid image type");
   }
}

static VkResult
nvk_image_init(struct nvk_device *device,
               struct nvk_image *image,
               const VkImageCreateInfo *pCreateInfo)
{
   vk_image_init(&device->vk, &image->vk, pCreateInfo);

   if ((image->vk.usage & (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) &&
       image->vk.samples > 1) {
      image->vk.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
      image->vk.stencil_usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
   }

   if (image->vk.usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
      image->vk.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
   if (image->vk.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
      image->vk.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

   enum nil_image_usage_flags usage = 0; /* TODO */
   if (pCreateInfo->tiling == VK_IMAGE_TILING_LINEAR)
      usage |= NIL_IMAGE_USAGE_LINEAR_BIT;
   if (pCreateInfo->flags & VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT)
      usage |= NIL_IMAGE_USAGE_2D_VIEW_BIT;
   if (pCreateInfo->flags & VK_IMAGE_CREATE_2D_VIEW_COMPATIBLE_BIT_EXT)
      usage |= NIL_IMAGE_USAGE_2D_VIEW_BIT;

   /* We treat 3D storage images as 2D arrays.  One day, we may wire up actual
    * 3D storage image support but baseArrayLayer gets tricky.
    */
   if (image->vk.usage & VK_IMAGE_USAGE_STORAGE_BIT)
      usage |= NIL_IMAGE_USAGE_2D_VIEW_BIT;

   image->plane_count = vk_format_get_plane_count(pCreateInfo->format);
   image->disjoint = image->plane_count > 1 &&
                     (pCreateInfo->flags & VK_IMAGE_CREATE_DISJOINT_BIT);

   const struct vk_format_ycbcr_info *ycbcr_info =
      vk_format_get_ycbcr_info(pCreateInfo->format);
   for (uint8_t plane = 0; plane < image->plane_count; plane++) {
      VkFormat format = ycbcr_info ?
         ycbcr_info->planes[plane].format : pCreateInfo->format;
      const uint8_t width_scale = ycbcr_info ?
         ycbcr_info->planes[plane].denominator_scales[0] : 1;
      const uint8_t height_scale = ycbcr_info ?
         ycbcr_info->planes[plane].denominator_scales[1] : 1;
      struct nil_image_init_info nil_info = {
         .dim = vk_image_type_to_nil_dim(pCreateInfo->imageType),
         .format = vk_format_to_pipe_format(format),
         .extent_px = {
            .w = pCreateInfo->extent.width / width_scale,
            .h = pCreateInfo->extent.height / height_scale,
            .d = pCreateInfo->extent.depth,
            .a = pCreateInfo->arrayLayers,
         },
         .levels = pCreateInfo->mipLevels,
         .samples = pCreateInfo->samples,
         .usage = usage,
      };

      ASSERTED bool ok = nil_image_init(&nvk_device_physical(device)->info,
                                       &image->planes[plane].nil, &nil_info);
      assert(ok);
   }

   return VK_SUCCESS;
}

static void nvk_image_finish(struct nvk_image *image)
{
   vk_image_finish(&image->vk);
}

VKAPI_ATTR VkResult VKAPI_CALL
nvk_CreateImage(VkDevice _device,
                const VkImageCreateInfo *pCreateInfo,
                const VkAllocationCallbacks *pAllocator,
                VkImage *pImage)
{
   VK_FROM_HANDLE(nvk_device, device, _device);
   struct nvk_image *image;
   VkResult result;

   image = vk_zalloc2(&device->vk.alloc, pAllocator, sizeof(*image), 8,
                      VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (!image)
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);

   result = nvk_image_init(device, image, pCreateInfo);
   if (result != VK_SUCCESS) {
      vk_free2(&device->vk.alloc, pAllocator, image);
      return result;
   }
   for (uint8_t plane = 0; plane < image->plane_count; plane++) {
      if (image->planes[plane].nil.pte_kind) {
         assert(device->pdev->mem_heaps[0].flags &
             VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);

         const VkMemoryAllocateInfo alloc_info = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = image->planes[plane].nil.size_B,
            .memoryTypeIndex = 0,
         };
         const struct nvk_memory_tiling_info tile_info = {
            .tile_mode = image->planes[plane].nil.tile_mode,
            .pte_kind = image->planes[plane].nil.pte_kind,
         };
         result = nvk_allocate_memory(device, &alloc_info, &tile_info,
                                   pAllocator, &image->planes[plane].internal);
         if (result != VK_SUCCESS) {
            nvk_image_finish(image);
            vk_free2(&device->vk.alloc, pAllocator, image);
            return result;
         }

         image->planes[plane].mem = image->planes[plane].internal;
         image->planes[plane].offset = 0;
      }
   }

   *pImage = nvk_image_to_handle(image);

   return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
nvk_DestroyImage(VkDevice _device,
                 VkImage _image,
                 const VkAllocationCallbacks *pAllocator)
{
   VK_FROM_HANDLE(nvk_device, device, _device);
   VK_FROM_HANDLE(nvk_image, image, _image);

   if (!image)
      return;

   for (uint8_t plane = 0; plane < image->plane_count; plane++) {
      if (image->planes[plane].internal)
         nvk_free_memory(device, image->planes[plane].internal, pAllocator);
   }

   nvk_image_finish(image);
   vk_free2(&device->vk.alloc, pAllocator, image);
}

VKAPI_ATTR void VKAPI_CALL
nvk_GetImageMemoryRequirements2(VkDevice _device,
                                const VkImageMemoryRequirementsInfo2 *pInfo,
                                VkMemoryRequirements2 *pMemoryRequirements)
{
   VK_FROM_HANDLE(nvk_device, device, _device);
   VK_FROM_HANDLE(nvk_image, image, pInfo->image);

   uint32_t memory_types = (1 << device->pdev->mem_type_cnt) - 1;

   // TODO hope for the best?

   VkImageAspectFlags aspects = image->vk.aspects;

   uint64_t size_B, align_B;
   if (image->disjoint) {
      const VkImagePlaneMemoryRequirementsInfo *plane_memory_req_info =
        vk_find_struct_const(pInfo->pNext, IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO);
      aspects = plane_memory_req_info->planeAspect;
      uint8_t plane = nvk_image_aspects_to_plane(image, aspects);
      size_B = image->planes[plane].nil.size_B;
      align_B = image->planes[plane].nil.align_B;
   } else {
      size_B = align_B = 0;
      for (unsigned plane = 0; plane < image->plane_count; plane++) {
         align_B = MAX2(align_B, image->planes[plane].nil.align_B);
         size_B = ALIGN_POT(size_B, image->planes[plane].nil.align_B);
         size_B += image->planes[plane].nil.size_B;
      }
   }

   pMemoryRequirements->memoryRequirements.memoryTypeBits = memory_types;
   pMemoryRequirements->memoryRequirements.alignment = align_B;
   pMemoryRequirements->memoryRequirements.size = size_B;

   vk_foreach_struct_const(ext, pMemoryRequirements->pNext) {
      switch (ext->sType) {
      case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS: {
         VkMemoryDedicatedRequirements *dedicated = (void *)ext;
         dedicated->prefersDedicatedAllocation = false;
         dedicated->requiresDedicatedAllocation = false;
         break;
      }
      default:
         nvk_debug_ignored_stype(ext->sType);
         break;
      }
   }
}

VKAPI_ATTR void VKAPI_CALL 
nvk_GetDeviceImageMemoryRequirements(VkDevice _device,
                                     const VkDeviceImageMemoryRequirementsKHR *pInfo,
                                     VkMemoryRequirements2 *pMemoryRequirements)
{
   VK_FROM_HANDLE(nvk_device, device, _device);
   ASSERTED VkResult result;
   struct nvk_image image;

   result = nvk_image_init(device, &image, pInfo->pCreateInfo);
   assert(result == VK_SUCCESS);

   VkImageMemoryRequirementsInfo2 info2 = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
      .image = nvk_image_to_handle(&image),
   };

   nvk_GetImageMemoryRequirements2(_device, &info2, pMemoryRequirements);
   nvk_image_finish(&image);
}

VKAPI_ATTR void VKAPI_CALL
nvk_GetImageSparseMemoryRequirements2(VkDevice device,
                                      const VkImageSparseMemoryRequirementsInfo2* pInfo,
                                      uint32_t* pSparseMemoryRequirementCount,
                                      VkSparseImageMemoryRequirements2* pSparseMemoryRequirements)
{
   /* We dont support sparse images yet, this is a stub to get KHR_get_memory_requirements2 */
   *pSparseMemoryRequirementCount = 0;
}

VKAPI_ATTR void VKAPI_CALL
nvk_GetDeviceImageSparseMemoryRequirements(VkDevice device,
                                           const VkDeviceImageMemoryRequirementsKHR* pInfo,
                                           uint32_t *pSparseMemoryRequirementCount,
                                           VkSparseImageMemoryRequirements2 *pSparseMemoryRequirements)
{
   /* Sparse images are not supported so this is just a stub for now. */
   *pSparseMemoryRequirementCount = 0;
}

VKAPI_ATTR void VKAPI_CALL
nvk_GetImageSubresourceLayout(VkDevice device,
                              VkImage _image,
                              const VkImageSubresource *pSubresource,
                              VkSubresourceLayout *pLayout)
{
   VK_FROM_HANDLE(nvk_image, image, _image);

   uint8_t plane = nvk_image_aspects_to_plane(image, pSubresource->aspectMask);

   *pLayout = (VkSubresourceLayout) {
      .offset = nil_image_level_layer_offset_B(&image->planes[plane].nil,
                                               pSubresource->mipLevel,
                                               pSubresource->arrayLayer),
      .size = nil_image_level_size_B(&image->planes[plane].nil, pSubresource->mipLevel),
      .rowPitch = image->planes[plane].nil.levels[pSubresource->mipLevel].row_stride_B,
      .arrayPitch = image->planes[plane].nil.array_stride_B,
      .depthPitch = nil_image_level_depth_stride_B(&image->planes[plane].nil,
                                                   pSubresource->mipLevel),
   };
}

VKAPI_ATTR VkResult VKAPI_CALL
nvk_BindImageMemory2(VkDevice _device,
                     uint32_t bindInfoCount,
                     const VkBindImageMemoryInfo *pBindInfos)
{
   for (uint32_t i = 0; i < bindInfoCount; ++i) {
      VK_FROM_HANDLE(nvk_device_memory, mem, pBindInfos[i].memory);
      VK_FROM_HANDLE(nvk_image, image, pBindInfos[i].image);

      if (image->disjoint) {
         const VkBindImagePlaneMemoryInfo *plane_info =
            vk_find_struct_const(pBindInfos[i].pNext, BIND_IMAGE_PLANE_MEMORY_INFO);
         uint8_t plane = nvk_image_aspects_to_plane(image, plane_info->planeAspect);
         if (image->planes[plane].internal == NULL) {
            image->planes[plane].mem = mem;
            image->planes[plane].offset = pBindInfos[i].memoryOffset;
         }
      } else {
         uint64_t offset_B = 0;
         for (unsigned plane = 0; plane < image->plane_count; plane++) {
            offset_B = ALIGN_POT(offset_B, image->planes[plane].nil.align_B);
            if (image->planes[plane].internal == NULL) {
               image->planes[plane].mem = mem;
               image->planes[plane].offset = pBindInfos[i].memoryOffset + offset_B;
            }
            offset_B += image->planes[plane].nil.size_B;
         }
      }
   }

   return VK_SUCCESS;
}
