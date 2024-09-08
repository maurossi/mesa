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

#include "terakan_format.h"

#include "terakan_entrypoints.h"
#include "terakan_image.h"
#include "terakan_physical_device.h"

#include "util/bitscan.h"
#include "util/macros.h"
#include "util/u_math.h"
#include "vk_format.h"
#include "vk_util.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TERAKAN_FORMAT_ASPECT_MAP_DECLARE(name, aspect_0, aspect_1, aspect_2)                      \
   [name] = {(aspect_0), (aspect_1), (aspect_2)},
VkImageAspectFlagBits const
   terakan_format_aspect_map_aspects[TERAKAN_FORMAT_ASPECT_MAP_COUNT][TERAKAN_FORMAT_MAX_ASPECTS] = {
      TERAKAN_FORMAT_ASPECT_MAP_DECLARE_ALL};
#undef TERAKAN_FORMAT_ASPECT_MAP_DECLARE

#define TERAKAN_FORMAT_ASPECT_MAP_DECLARE(name, aspect_0, aspect_1, aspect_2)                      \
   [name] = (aspect_0) | (aspect_1) | (aspect_2),
VkImageAspectFlags const terakan_format_aspect_map_aspect_masks[TERAKAN_FORMAT_ASPECT_MAP_COUNT] = {
   TERAKAN_FORMAT_ASPECT_MAP_DECLARE_ALL};
#undef TERAKAN_FORMAT_ASPECT_MAP_DECLARE

enum terakan_format_aspect_map
terakan_format_aspect_map_for_format_aspects(VkImageAspectFlags const format_aspects)
{
   for (unsigned aspect_map_index = 0; aspect_map_index < TERAKAN_FORMAT_ASPECT_MAP_COUNT;
        ++aspect_map_index) {
      if (terakan_format_aspect_map_aspect_masks[aspect_map_index] == format_aspects) {
         return (enum terakan_format_aspect_map)aspect_map_index;
      }
   }
   return TERAKAN_FORMAT_ASPECT_MAP_INVALID;
}

unsigned
terakan_format_aspect_index(enum terakan_format_aspect_map const aspect_map,
                            VkImageAspectFlagBits const aspect, unsigned result_if_not_present)
{
   assert(aspect_map < TERAKAN_FORMAT_ASPECT_MAP_COUNT);
   VkImageAspectFlagBits const * const map_aspects = terakan_format_aspect_map_aspects[aspect_map];
   for (unsigned aspect_index = 0; aspect_index < TERAKAN_FORMAT_MAX_ASPECTS; ++aspect_index) {
      VkImageAspectFlagBits const map_aspect = map_aspects[aspect_index];
      if (map_aspect == VK_IMAGE_ASPECT_NONE) {
         break;
      }
      if (map_aspect == aspect) {
         return aspect_index;
      }
   }
   return result_if_not_present;
}

bool
terakan_format_info_get(VkFormat const format, struct terakan_format_info * const info_out)
{
   enum terakan_format_aspect_map const aspect_map =
      terakan_format_aspect_map_for_format_aspects(vk_format_aspects(format));
   if (unlikely(aspect_map == TERAKAN_FORMAT_ASPECT_MAP_INVALID)) {
      return false;
   }
   info_out->aspect_map = aspect_map;
   memset(info_out->aspect_formats, 0, sizeof(info_out->aspect_formats));
   VkImageAspectFlagBits const * const map_aspects = terakan_format_aspect_map_aspects[aspect_map];
   for (unsigned aspect_index = 0; aspect_index < TERAKAN_FORMAT_MAX_ASPECTS; ++aspect_index) {
      VkImageAspectFlagBits const map_aspect = map_aspects[aspect_index];
      if (map_aspect == VK_IMAGE_ASPECT_NONE) {
         break;
      }
      struct terascale_format_info * const aspect_format_info =
         &info_out->aspect_formats[aspect_index];
      *aspect_format_info = terascale_format_info_r8xx[vk_format_to_pipe_format(
         vk_format_get_aspect_format(format, map_aspect))];
      if (unlikely(aspect_format_info->format == TERASCALE_FORMAT_INDEX_INVALID)) {
         return false;
      }
   }
   return true;
}

VKAPI_ATTR void VKAPI_CALL
terakan_GetPhysicalDeviceFormatProperties2(VkPhysicalDevice const physicalDevice,
                                           VkFormat const format,
                                           VkFormatProperties2 * const pFormatProperties)
{
   VkFormatFeatureFlags2 image_linear_tiling_features = 0;
   VkFormatFeatureFlags2 image_optimal_tiling_features = 0;
   VkFormatFeatureFlags2 buffer_features = 0;

   struct terakan_format_info format_info;
   if (terakan_format_info_get(format, &format_info)) {
      /* Image features for the intersection of the aspects - gather hardware features for it. */

      uint64_t image_formats_used = 0b0;

      bool image_sq_vertex_fetch = true;

      bool image_sq_texture_fetch = true;
      /* Ignore if image_sq_texture_fetch is false. */
      bool image_sq_texture_fetch_linear_filter = true;

      bool image_cb_color = true;
      /* Ignore if cb_color is false. */
      bool image_cb_color_blend = true;
      bool image_cb_color_atomic = true;

      for (unsigned aspect_index = 0; aspect_index < TERAKAN_FORMAT_MAX_ASPECTS; ++aspect_index) {
         struct terascale_format_info const aspect_format_info =
            format_info.aspect_formats[aspect_index];
         if (aspect_format_info.format == TERASCALE_FORMAT_INDEX_INVALID) {
            break;
         }

         image_formats_used |= BITFIELD64_BIT(aspect_format_info.format);

         if (!aspect_format_info.supports_sq_vertex_fetch) {
            image_sq_vertex_fetch = false;
         }

         if (aspect_format_info.supports_sq_texture_fetch) {
            /* Section 49.2. "Format Properties" of the Vulkan 1.3.288 specification says:
             *
             *     "VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT
             *
             *     [...]
             *
             *     If the format being queried is a depth/stencil format, this bit only specifies
             *     that the depth aspect (not the stencil aspect) of an image of this format
             *     supports linear filtering, and that linear filtering of the depth aspect is
             *     supported whether depth compare is enabled in the sampler or not."
             */
            if (!((terakan_format_aspect_map_aspect_masks[format_info.aspect_map] &
                   (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) ==
                     (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) &&
                  terakan_format_aspect_map_aspects[format_info.aspect_map][aspect_index] !=
                     VK_IMAGE_ASPECT_DEPTH_BIT) &&
                (aspect_format_info.number_type == TERASCALE_FORMAT_NUMBER_TYPE_UINT ||
                 aspect_format_info.number_type == TERASCALE_FORMAT_NUMBER_TYPE_SINT)) {
               image_sq_texture_fetch_linear_filter = false;
            }
         } else {
            image_sq_texture_fetch = false;
         }

         if (aspect_format_info.supports_cb_color) {
            if (terascale_format_blend_bypass(
                   (enum terascale_format_number_type)aspect_format_info.number_type,
                   (enum terascale_format_index)aspect_format_info.format)) {
               image_cb_color_blend = false;
            }
            if (!terascale_format_supports_uav_atomic_int(&aspect_format_info)) {
               image_cb_color_atomic = false;
            }
         } else {
            image_cb_color = false;
         }
      }

      /* Translate hardware image features into Vulkan image features. */

      VkFormatFeatureFlags2 image_features = 0;
      VkFormatFeatureFlags2 image_tiled_only_features = 0;

      if (image_sq_texture_fetch) {
         image_features |= VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_2_BLIT_SRC_BIT;
         if (image_sq_texture_fetch_linear_filter) {
            image_features |= VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT |
                              VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT;
         }
         /* DEPTH_STENCIL_ATTACHMENT requires both depth / stencil attachment (DB, can't be linear)
          * and input attachment (SQ texture) usage.
          */
         if (terascale_get_r8xx_depth_stencil_format(vk_format_to_pipe_format(format), NULL,
                                                     NULL)) {
            image_tiled_only_features |= VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT;
         }
      }

      if (image_cb_color) {
         /* STORAGE requires a CB unordered access view, an SQ buffer / texture depending on the
          * type (for size queries as well as certain read-only usage cases), and an SQ buffer for
          * CB_IMMED.
          */
         if (image_sq_vertex_fetch && image_sq_texture_fetch) {
            VkFormatFeatureFlags2 const storage_image_features =
               VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT |
               VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT |
               VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT |
               (image_cb_color_atomic ? VK_FORMAT_FEATURE_2_STORAGE_IMAGE_ATOMIC_BIT : 0);
            /* According to R800 AddrLib, "Tex2D UAV on cypress will fail/hang if tile mode is
             * linear".
             * Hemlock is a variant of Cypress, and AddrLib doesn't distinguish between the two, for
             * safety, assume the bug is present on Hemlock too.
             */
            enum radeon_family const chip_family =
               terakan_physical_device_from_handle(physicalDevice)->chip_family_info.chip_family;
            if (chip_family == CHIP_CYPRESS || chip_family == CHIP_HEMLOCK) {
               image_tiled_only_features |= storage_image_features;
            } else {
               image_features |= storage_image_features;
            }
         }

         /* COLOR_ATTACHMENT requires both color attachment (CB color) and input attachment (SQ
          * texture) usage.
          */
         if (image_sq_texture_fetch) {
            image_features |= VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT;
            if (image_cb_color_blend) {
               image_features |= VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT;
            }
         }

         image_features |= VK_FORMAT_FEATURE_2_BLIT_DST_BIT;
      }

      bool const image_linear_only = (TERASCALE_FORMATS_LINEAR_ONLY & image_formats_used) != 0;
      bool const image_tiled_only = (TERASCALE_FORMATS_TILED_ONLY_R8XX & image_formats_used) != 0;
      if (!(image_linear_only && image_tiled_only)) {
         if (!image_tiled_only) {
            image_linear_tiling_features |= image_features;
         }
         image_optimal_tiling_features |= image_features;
         if (!image_linear_only) {
            image_optimal_tiling_features |= image_tiled_only_features;
         }
      }

      /* Buffer features - buffers can be created only with single-aspect formats.
       *
       * bufferFeatures must not support any features for depth or stencil (even though internally
       * for simplicity Terakan largely doesn't distinguish between depth / stencil formats and
       * color formats corresponding to their aspects) or block-compressed formats according to the
       * Vulkan specification.
       */

      if (util_bitcount(terakan_format_aspect_map_aspect_masks[format_info.aspect_map]) &&
          !vk_format_is_depth_or_stencil(format) && !vk_format_is_block_compressed(format)) {
         /* If there's only one aspect, its format inherently must match the whole format.
          * Assuming that makes it possible to skip terakan_format_info for buffers and just use the
          * application-provided format directly without decomposing it into aspects to get the
          * hardware format info.
          */
         assert(vk_format_get_aspect_format(
                   format, terakan_format_aspect_map_aspects[format_info.aspect_map][0]) == format);
         struct terascale_format_info const buffer_format_info = format_info.aspect_formats[0];
         if (buffer_format_info.supports_sq_vertex_fetch) {
            buffer_features |=
               VK_FORMAT_FEATURE_2_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT;
            /* STORAGE requires a CB unordered access view, and an SQ buffer for CB_IMMED. */
            if (buffer_format_info.supports_cb_color) {
               buffer_features |= VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_BIT |
                                  VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT |
                                  VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT;
               if (terascale_format_supports_uav_atomic_int(&buffer_format_info)) {
                  buffer_features |= VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
               }
            }
         }
      }

      /* TODO(Triang3l): On R9xx, handle disabling CB features for linear tiling for formats
       * requiring endian swap according to "Prohibited combinations" in the Programming Guide.
       * This, however, may result in R9xx completely not supporting Vulkan on big-endian hosts if
       * that applies to buffers too - research if that limitation is texture/RTV-specific, or if
       * endian swap is not supported for buffer UAVs as well.
       */
   }

   if (image_linear_tiling_features) {
      image_linear_tiling_features |=
         VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT;
   }
   if (image_optimal_tiling_features) {
      image_optimal_tiling_features |=
         VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT;
   }
   if (buffer_features) {
      buffer_features |=
         VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT;
   }

   pFormatProperties->formatProperties.linearTilingFeatures =
      (VkFormatFeatureFlags)image_linear_tiling_features;
   pFormatProperties->formatProperties.optimalTilingFeatures =
      (VkFormatFeatureFlags)image_optimal_tiling_features;
   pFormatProperties->formatProperties.bufferFeatures = (VkFormatFeatureFlags)buffer_features;

   VkFormatProperties3 * const format_properties_3 =
      vk_find_struct(pFormatProperties->pNext, FORMAT_PROPERTIES_3);
   if (format_properties_3 != NULL) {
      format_properties_3->linearTilingFeatures = image_linear_tiling_features;
      format_properties_3->optimalTilingFeatures = image_optimal_tiling_features;
      format_properties_3->bufferFeatures = buffer_features;
   }
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_GetPhysicalDeviceImageFormatProperties2(
   VkPhysicalDevice const physicalDevice,
   VkPhysicalDeviceImageFormatInfo2 const * const pImageFormatInfo,
   VkImageFormatProperties2 * const pImageFormatProperties)
{
   /* From the VkImageFormatProperties2 specification:
    *
    *    Filling imageFormatProperties with zero for unsupported formats is an exception to the
    *    usual rule that output structures have undefined contents on error. This exception was
    *    unintentional, but is preserved for backwards compatibility. This exception only applies to
    *    imageFormatProperties, not sType, pNext, or any structures chained from pNext.
    */
   pImageFormatProperties->imageFormatProperties = (VkImageFormatProperties){};

   VkFormatProperties3 format_properties_3;
   format_properties_3.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3;
   format_properties_3.pNext = NULL;
   VkFormatProperties2 format_properties_2;
   format_properties_2.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
   format_properties_2.pNext = &format_properties_3;
   terakan_GetPhysicalDeviceFormatProperties2(physicalDevice, pImageFormatInfo->format,
                                              &format_properties_2);

   VkFormatFeatureFlags2 features;
   switch (pImageFormatInfo->tiling) {
   case VK_IMAGE_TILING_OPTIMAL:
      features = format_properties_3.optimalTilingFeatures;
      break;
   case VK_IMAGE_TILING_LINEAR:
      features = format_properties_3.linearTilingFeatures;
      break;
   default:
      return VK_ERROR_FORMAT_NOT_SUPPORTED;
   }
   if (!features) {
      return VK_ERROR_FORMAT_NOT_SUPPORTED;
   }

   VkImageFormatProperties image_format_properties;

   bool const is_target =
      (pImageFormatInfo->usage & (VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)) != 0;

   image_format_properties.maxExtent.width = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT;
   switch (pImageFormatInfo->type) {
   case VK_IMAGE_TYPE_1D:
      image_format_properties.maxExtent.height = 1;
      image_format_properties.maxExtent.depth = 1;
      image_format_properties.maxArrayLayers =
         is_target ? TERAKAN_IMAGE_MAX_TARGET_SLICES : TERAKAN_IMAGE_MAX_SLICES;
      break;
   case VK_IMAGE_TYPE_2D:
      image_format_properties.maxExtent.height = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT;
      image_format_properties.maxExtent.depth = 1;
      image_format_properties.maxArrayLayers =
         is_target ? TERAKAN_IMAGE_MAX_TARGET_SLICES : TERAKAN_IMAGE_MAX_SLICES;
      break;
   case VK_IMAGE_TYPE_3D:
      image_format_properties.maxExtent.height = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT;
      image_format_properties.maxExtent.depth =
         (pImageFormatInfo->usage & VK_IMAGE_USAGE_STORAGE_BIT) ||
               (is_target && (pImageFormatInfo->flags & VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT))
            ? TERAKAN_IMAGE_MAX_TARGET_SLICES
            : TERAKAN_IMAGE_MAX_SLICES;
      image_format_properties.maxArrayLayers = 1;
      break;
   default:
      return VK_ERROR_FORMAT_NOT_SUPPORTED;
   }
   image_format_properties.maxMipLevels =
      util_logbase2(MAX3(image_format_properties.maxExtent.width,
                         image_format_properties.maxExtent.height,
                         image_format_properties.maxExtent.depth)) +
      1;

   image_format_properties.sampleCounts = VK_SAMPLE_COUNT_1_BIT;
   /* Multisampled images must be 2D-tiled. */
   if ((features & (VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT |
                    VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT)) &&
       pImageFormatInfo->type == VK_IMAGE_TYPE_2D &&
       !(pImageFormatInfo->flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) &&
       pImageFormatInfo->tiling == VK_IMAGE_TILING_OPTIMAL) {
      /* For linear-only formats, the optimal tiling is linear, check if that's not the case for the
       * requested format.
       */
      struct terakan_format_info format_info;
      if (terakan_format_info_get(pImageFormatInfo->format, &format_info)) {
         bool format_linear_only = false;
         for (unsigned aspect_index = 0; aspect_index < TERAKAN_FORMAT_MAX_ASPECTS;
              ++aspect_index) {
            struct terascale_format_info const aspect_format_info =
               format_info.aspect_formats[aspect_index];
            if (aspect_format_info.format == TERASCALE_FORMAT_INDEX_INVALID) {
               break;
            }
            if (TERASCALE_FORMATS_LINEAR_ONLY & BITFIELD64_BIT(aspect_format_info.format)) {
               format_linear_only = true;
               break;
            }
         }
         if (!format_linear_only) {
            image_format_properties.sampleCounts |=
               VK_SAMPLE_COUNT_2_BIT | VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_8_BIT;
         }
      }
   }

   struct terakan_physical_device const * const device =
      terakan_physical_device_from_handle(physicalDevice);

   image_format_properties.maxResourceSize = device->max_memory_allocation_size;

   VkExternalMemoryProperties external_properties = {};
   VkPhysicalDeviceExternalImageFormatInfo const * const external_info =
      vk_find_struct_const(pImageFormatInfo->pNext, PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO);
   if (external_info != NULL && external_info->handleType) {
      VkExternalMemoryHandleTypeFlags const supported_handle_types =
         terakan_physical_device_supported_external_memory_types(device);
      if (!(supported_handle_types & external_info->handleType)) {
         return VK_ERROR_FORMAT_NOT_SUPPORTED;
      }
      switch (external_info->handleType) {
      case VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT:
      case VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT: {
         external_properties.externalMemoryFeatures =
            VK_EXTERNAL_MEMORY_FEATURE_DEDICATED_ONLY_BIT |
            VK_EXTERNAL_MEMORY_FEATURE_EXPORTABLE_BIT | VK_EXTERNAL_MEMORY_FEATURE_IMPORTABLE_BIT;
         VkExternalMemoryHandleTypeFlags const supported_fd_types =
            supported_handle_types & (VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT |
                                      VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT);
         external_properties.exportFromImportedHandleTypes = supported_fd_types;
         external_properties.compatibleHandleTypes = supported_fd_types;
      } break;

      default:
         return VK_ERROR_FORMAT_NOT_SUPPORTED;
      }
   }

   /* The format is supported, fill the output structures. */

   pImageFormatProperties->imageFormatProperties = image_format_properties;

   vk_foreach_struct (ext, pImageFormatProperties->pNext) {
      switch (ext->sType) {
      case VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES: {
         /* From the VkPhysicalDeviceExternalImageFormatInfo specification:
          *
          *    If handleType is 0, vkGetPhysicalDeviceImageFormatProperties2 will behave as if
          *    VkPhysicalDeviceExternalImageFormatInfo was not present, and
          *    VkExternalImageFormatProperties will be ignored.
          */
         if (external_info != NULL && external_info->handleType) {
            ((VkExternalImageFormatProperties *)ext)->externalMemoryProperties =
               external_properties;
         }
      } break;

      default:
         break;
      }
   }

   return VK_SUCCESS;
}
