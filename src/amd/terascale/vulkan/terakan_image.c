/*
 * Copyright © 2023 Vitaliy Triang3l Kuzmin
 *
 * Based in part on r600_texture.c which is:
 * Copyright 2010 Jerome Glisse <glisse@freedesktop.org>
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

#include "terakan_image.h"

#include "winsys/terakan_winsys.h"
#include "terakan_device.h"
#include "terakan_device_memory.h"
#include "terakan_entrypoints.h"
#include "terakan_format.h"
#include "terakan_gpu_info.h"
#include "terakan_physical_device.h"

#include "gallium/drivers/r600/evergreend.h"
#include "util/bitscan.h"
#include "util/macros.h"
#include "util/u_math.h"
#include "vk_alloc.h"
#include "vk_format.h"
#include "vk_log.h"
#include "vk_util.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

uint32_t
terakan_image_get_optimal_tiling_array_mode(VkImageCreateInfo const * const image_create_info)
{
   if (terakan_format_is_linear_only(image_create_info->format)) {
      return V_028C70_ARRAY_LINEAR_ALIGNED;
   }

   /* Multisampled images must be 2D-tiled. */
   if (image_create_info->samples > VK_SAMPLE_COUNT_1_BIT) {
      return V_028C70_ARRAY_2D_TILED_THIN1;
   }

   /* Handle common candidates for the linear mode.
    *
    * Depth / stencil attachments must be tiled.
    *
    * The "Tex2D UAV on cypress will fail/hang if tile mode is linear" note from the R800 AddrLib
    * must be taken into account if there's possibility of this image being used as a random access
    * target.
    *
    * 1D storage images must be linear according to the Gallium R600 driver, and overall linear is
    * more compact for them (storage image usage is not supported in Terakan for formats that must
    * be tiled).
    */
   if (!(image_create_info->usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) &&
       !terakan_format_is_tiled_only(image_create_info->format) &&
       image_create_info->imageType == VK_IMAGE_TYPE_1D) {
      return V_028C70_ARRAY_LINEAR_ALIGNED;
   }

   /* Make small textures 1D-tiled, similar to how that's done in the Gallium R600 driver. */
   /* TODO(Triang3l): Debug flag for disabling 2D tiling. */
   if (image_create_info->extent.width <= 16 || image_create_info->extent.height <= 16) {
      return V_028C70_ARRAY_1D_TILED_THIN1;
   }

   /* Surface computation will switch to 1D if needed. */
   return V_028C70_ARRAY_2D_TILED_THIN1;
}

VKAPI_ATTR void VKAPI_CALL
terakan_GetDeviceImageMemoryRequirements(VkDevice const deviceHandle,
                                         VkDeviceImageMemoryRequirements const * const pInfo,
                                         VkMemoryRequirements2 * const pMemoryRequirements)
{
   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);
   struct terakan_physical_device const * const physical_device =
      container_of(device->vk.physical, struct terakan_physical_device const, vk);

   struct radeon_surf surface;
   if (physical_device->winsys->surface_fn->translate_image_create_info(
          physical_device->winsys, pInfo->pCreateInfo, &surface)) {
      pMemoryRequirements->memoryRequirements.size = surface.total_size;
      pMemoryRequirements->memoryRequirements.alignment = (VkDeviceSize)1 << surface.alignment_log2;
   } else {
      assert(!"Failed to translate the image creation info into surface info");
      pMemoryRequirements->memoryRequirements.size = 1;
      pMemoryRequirements->memoryRequirements.alignment = 1;
   }

   pMemoryRequirements->memoryRequirements.memoryTypeBits =
      ((uint32_t)1 << physical_device->memory_properties.memoryTypeCount) - 1;

   vk_foreach_struct (ext, pMemoryRequirements->pNext) {
      switch (ext->sType) {
      case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS: {
         VkMemoryDedicatedRequirements * const dedicated_requirements =
            (VkMemoryDedicatedRequirements *)ext;
         VkExternalMemoryImageCreateInfo const * const external_memory_info =
            vk_find_struct_const(pInfo, EXTERNAL_MEMORY_IMAGE_CREATE_INFO);
         dedicated_requirements->requiresDedicatedAllocation =
            external_memory_info != NULL && external_memory_info->handleTypes != 0;
         dedicated_requirements->prefersDedicatedAllocation =
            dedicated_requirements->requiresDedicatedAllocation;
      } break;

      default:
         break;
      }
   }
}

/* Skipping the translation into the surface structure if it has already been done. */
VKAPI_ATTR void VKAPI_CALL
terakan_GetImageMemoryRequirements2(VkDevice const deviceHandle,
                                    VkImageMemoryRequirementsInfo2 const * const pInfo,
                                    VkMemoryRequirements2 * const pMemoryRequirements)
{
   struct terakan_image const * const image = terakan_image_from_handle(pInfo->image);
   pMemoryRequirements->memoryRequirements.size = image->surface.total_size;
   pMemoryRequirements->memoryRequirements.alignment = (VkDeviceSize)1
                                                       << image->surface.alignment_log2;

   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);
   struct terakan_physical_device const * const physical_device =
      container_of(device->vk.physical, struct terakan_physical_device const, vk);
   pMemoryRequirements->memoryRequirements.memoryTypeBits =
      ((uint32_t)1 << physical_device->memory_properties.memoryTypeCount) - 1;

   vk_foreach_struct (ext, pMemoryRequirements->pNext) {
      switch (ext->sType) {
      case VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS: {
         VkMemoryDedicatedRequirements * const dedicated_requirements =
            (VkMemoryDedicatedRequirements *)ext;
         dedicated_requirements->requiresDedicatedAllocation = image->vk.external_handle_types != 0;
         dedicated_requirements->prefersDedicatedAllocation =
            dedicated_requirements->requiresDedicatedAllocation;
      } break;

      default:
         break;
      }
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_GetImageSubresourceLayout(VkDevice const device, VkImage const imageHandle,
                                  VkImageSubresource const * pSubresource,
                                  VkSubresourceLayout * const pLayout)
{
   struct terakan_image const * const image = terakan_image_from_handle(imageHandle);

   bool const is_stencil_layout =
      pSubresource->aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT && image->surface.has_stencil;
   struct legacy_surf_level const * const level =
      is_stencil_layout ? &image->surface.u.legacy.zs.stencil_level[pSubresource->mipLevel]
                        : &image->surface.u.legacy.level[pSubresource->mipLevel];

   VkDeviceSize const slice_size = sizeof(uint32_t) * (VkDeviceSize)level->slice_size_dw;

   pLayout->offset = 256 * (VkDeviceSize)level->offset_256B + slice_size * pSubresource->arrayLayer;

   pLayout->size = slice_size;
   if (image->vk.image_type == VK_IMAGE_TYPE_3D) {
      pLayout->size *= u_minify(image->vk.extent.depth, pSubresource->mipLevel);
   }

   /* nblk is expected to have already been aligned appropriately in the surface computation.
    * vkGetImageSubresourceLayout is for linear images only - no need to take MSAA samples into
    * account.
    */
   pLayout->rowPitch = (is_stencil_layout ? 1 : image->surface.bpe) * (VkDeviceSize)level->nblk_x;

   pLayout->arrayPitch = slice_size;
   pLayout->depthPitch = slice_size;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_BindImageMemory2(VkDevice const device, uint32_t const bindInfoCount,
                         VkBindImageMemoryInfo const * const pBindInfos)
{
   for (uint32_t bind_info_index = 0; bind_info_index < bindInfoCount; ++bind_info_index) {
      VkBindImageMemoryInfo const * const bind_info = &pBindInfos[bind_info_index];
      struct terakan_image * const image = terakan_image_from_handle(bind_info->image);
      image->bo = terakan_device_memory_from_handle(bind_info->memory)->bo;
      image->bo_offset = bind_info->memoryOffset;
   }

   return VK_SUCCESS;
}

bool
terakan_image_uses_tc_non_display_tiling(enum amd_gfx_level const gfx_level,
                                         VkFormat const image_format, bool const level_is_linear)
{
   if (level_is_linear) {
      /* Linear textures must use display tiling, but it's not supported for 128bpp at all on R9xx.
       */
      return gfx_level >= CAYMAN && vk_format_get_blocksizebits(image_format) >= 128;
   }
   /* Depth, stencil and FMask implicitly utilize non-display tiling. */
   if (vk_format_is_depth_or_stencil(image_format)) {
      return true;
   }
   /* Non-display tiling is supported for 8, 16, 32, 64 and 128bpe textures. */
   unsigned const block_size_bits = vk_format_get_blocksizebits(image_format);
   return block_size_bits >= 8 && util_is_power_of_two_or_zero(block_size_bits);
}

bool
terakan_image_uses_cb_non_display_tiling(enum amd_gfx_level const gfx_level,
                                         VkFormat const image_format, bool const level_is_linear)
{
   return level_is_linear || vk_format_is_depth_or_stencil(image_format) ||
          (gfx_level >= CAYMAN && vk_format_get_blocksizebits(image_format) >= 128);
}

bool
terakan_image_create_resource_descriptor(VkImageViewCreateInfo const * const image_view_create_info,
                                         uint32_t descriptor_out[8])
{
   struct terakan_image const * const image =
      terakan_image_from_handle(image_view_create_info->image);

   uint32_t dimension;
   uint32_t layer_count = 1;
   switch (image_view_create_info->viewType) {
   case VK_IMAGE_VIEW_TYPE_1D:
      dimension = V_030000_SQ_TEX_DIM_1D_ARRAY;
      break;
   case VK_IMAGE_VIEW_TYPE_2D:
      dimension = image->vk.samples > VK_SAMPLE_COUNT_1_BIT ? V_030000_SQ_TEX_DIM_2D_ARRAY_MSAA
                                                            : V_030000_SQ_TEX_DIM_2D_ARRAY;
      break;
   case VK_IMAGE_VIEW_TYPE_3D:
      dimension = V_030000_SQ_TEX_DIM_3D;
      break;
   case VK_IMAGE_VIEW_TYPE_CUBE:
      dimension = V_030000_SQ_TEX_DIM_CUBEMAP;
      layer_count = 6;
      break;
   case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
      dimension = V_030000_SQ_TEX_DIM_1D_ARRAY;
      layer_count = image_view_create_info->subresourceRange.layerCount;
      break;
   case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
      dimension = image->vk.samples > VK_SAMPLE_COUNT_1_BIT
                     ? V_030000_SQ_TEX_DIM_2D_ARRAY_MSAA
                     : V_030000_SQ_TEX_DIM_2D_ARRAY;
      layer_count = image_view_create_info->subresourceRange.layerCount;
      break;
   case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
      dimension = V_030000_SQ_TEX_DIM_CUBEMAP;
      layer_count = image_view_create_info->subresourceRange.layerCount;
      break;
   default:
      assert(!"Unsupported image view type");
      return false;
   }
   if (image->vk.array_layers <= 1) {
      switch (dimension) {
      case V_030000_SQ_TEX_DIM_1D_ARRAY:
         dimension = V_030000_SQ_TEX_DIM_1D;
         break;
      case V_030000_SQ_TEX_DIM_2D_ARRAY:
         dimension = V_030000_SQ_TEX_DIM_2D;
         break;
      case V_030000_SQ_TEX_DIM_2D_ARRAY_MSAA:
         dimension = V_030000_SQ_TEX_DIM_2D_MSAA;
         break;
      default:
         break;
      }
   }

   bool const is_stencil_aspect =
      image_view_create_info->subresourceRange.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT;

   uint32_t data_format, signs, number_format;
   if (is_stencil_aspect && vk_format_has_depth(image_view_create_info->format)) {
      /* The getters return the values for the depth aspect, not stencil. */
      data_format = FMT_8;
      number_format = V_030010_SQ_NUM_FORMAT_INT;
      signs = S_030010_FORMAT_COMP_X(V_030010_SQ_FORMAT_COMP_UNSIGNED);
   } else {
      data_format = terakan_format_texture_get_format(image_view_create_info->format);
      if (data_format == FMT_INVALID) {
         return false;
      }
      number_format = terakan_format_data_get_number_format(image_view_create_info->format);
      if (number_format == UINT32_MAX) {
         return false;
      }
      signs = terakan_format_texture_get_word4_signs(image_view_create_info->format);
   }

   bool const is_stencil_layout = is_stencil_aspect && image->surface.has_stencil;
   struct legacy_surf_level const * const level =
      is_stencil_layout
         ? &image->surface.u.legacy.zs
               .stencil_level[image_view_create_info->subresourceRange.baseMipLevel]
         : &image->surface.u.legacy.level[image_view_create_info->subresourceRange.baseMipLevel];

   struct terakan_gpu_info const * const gpu_info =
      &container_of(image->vk.base.device->physical, struct terakan_physical_device const, vk)
          ->winsys->gpu_info;

   bool const non_display_tiling = terakan_image_uses_tc_non_display_tiling(
      gpu_info->gfx_level, image->vk.format, level->mode <= RADEON_SURF_MODE_LINEAR_ALIGNED);

   uint32_t width =
      u_minify(image->vk.extent.width, image_view_create_info->subresourceRange.baseMipLevel);
   uint32_t height =
      u_minify(image->vk.extent.height, image_view_create_info->subresourceRange.baseMipLevel);
   /* Allowing uncompressed views of compressed and subsampled images. */
   if (!vk_format_is_compressed(image_view_create_info->format)) {
      width = (width + (image->surface.blk_w - 1)) / image->surface.blk_w;
      height = (height + (image->surface.blk_h - 1)) / image->surface.blk_h;
   }

   /* nblk is expected to have already been aligned appropriately in the surface computation. */
   descriptor_out[0] =
      S_030000_DIM(dimension) |
      (gpu_info->gfx_level >= CAYMAN ? CM_S_030000_NON_DISP_TILING_ORDER(non_display_tiling)
                                     : S_030000_NON_DISP_TILING_ORDER(non_display_tiling)) |
      S_030000_PITCH(level->nblk_x / 8 - 1) | S_030000_TEX_WIDTH(width - 1);

   descriptor_out[1] = S_030004_ARRAY_MODE(terakan_image_array_mode_ac_to_hw(level->mode));
   if (dimension != V_030000_SQ_TEX_DIM_1D) {
      if (dimension == V_030000_SQ_TEX_DIM_1D_ARRAY) {
         descriptor_out[1] |= S_030004_TEX_HEIGHT(image->vk.array_layers - 1);
      } else {
         descriptor_out[1] |= S_030004_TEX_HEIGHT(height - 1);
         if (image->vk.image_type == VK_IMAGE_TYPE_3D) {
            assert(image_view_create_info->viewType == VK_IMAGE_VIEW_TYPE_2D ||
                  image_view_create_info->viewType == VK_IMAGE_VIEW_TYPE_2D_ARRAY ||
                  image_view_create_info->viewType == VK_IMAGE_VIEW_TYPE_3D);
            descriptor_out[1] |= S_030004_TEX_DEPTH(image->vk.extent.depth - 1);
         } else {
            descriptor_out[1] |= S_030004_TEX_DEPTH(image->vk.array_layers - 1);
         }
      }
   }

   uint32_t const image_bo_offset_256b = image->bo_offset / 256;

   /* Base level is 0 - offsetting the base address instead, so single-level 2D views of 3D images
    * can be created.
    */
   descriptor_out[2] = S_030008_BASE_ADDRESS(image_bo_offset_256b + level->offset_256B);

   unsigned char const * const format_swizzle =
      terakan_format_data_get_swizzle(image_view_create_info->format);

   descriptor_out[4] =
      signs | S_030010_NUM_FORMAT_ALL(number_format) |
      S_030010_SRF_MODE_ALL(V_030010_SRF_MODE_ZERO_CLAMP_MINUS_ONE) |
      S_030010_FORCE_DEGAMMA(vk_format_description(image_view_create_info->format)->colorspace ==
                             UTIL_FORMAT_COLORSPACE_SRGB) |
      S_030010_DST_SEL_X(terakan_format_data_component_swizzle_to_dst_sel(
         image_view_create_info->components.r, VK_COMPONENT_SWIZZLE_R, format_swizzle)) |
      S_030010_DST_SEL_Y(terakan_format_data_component_swizzle_to_dst_sel(
         image_view_create_info->components.g, VK_COMPONENT_SWIZZLE_G, format_swizzle)) |
      S_030010_DST_SEL_Z(terakan_format_data_component_swizzle_to_dst_sel(
         image_view_create_info->components.b, VK_COMPONENT_SWIZZLE_B, format_swizzle)) |
      S_030010_DST_SEL_W(terakan_format_data_component_swizzle_to_dst_sel(
         image_view_create_info->components.a, VK_COMPONENT_SWIZZLE_A, format_swizzle));

   descriptor_out[5] =
      S_030014_BASE_ARRAY(image_view_create_info->subresourceRange.baseArrayLayer) |
      S_030014_LAST_ARRAY(
         (layer_count == VK_REMAINING_ARRAY_LAYERS
             ? image->vk.array_layers
             : image_view_create_info->subresourceRange.baseArrayLayer + layer_count) -
         1);

   descriptor_out[6] = S_030018_TILE_SPLIT(terakan_image_tile_split_bytes_to_hw(
      is_stencil_layout ? image->surface.u.legacy.stencil_tile_split
                        : image->surface.u.legacy.tile_split));

   descriptor_out[7] =
      S_03001C_DATA_FORMAT(data_format) |
      S_03001C_MACRO_TILE_ASPECT(util_logbase2(image->surface.u.legacy.mtilea)) |
      S_03001C_BANK_WIDTH(util_logbase2(image->surface.u.legacy.bankw)) |
      S_03001C_BANK_HEIGHT(util_logbase2(image->surface.u.legacy.bankh)) |
      S_03001C_DEPTH_SAMPLE_ORDER(vk_format_is_depth_or_stencil(image->vk.format) &&
                                  level->mode > RADEON_SURF_MODE_LINEAR_ALIGNED) |
      S_03001C_NUM_BANKS(gpu_info->tile_banks_log2 - 1) |
      S_03001C_TYPE(V_03001C_SQ_TEX_VTX_VALID_TEXTURE);

   if (dimension == V_030000_SQ_TEX_DIM_2D_MSAA || dimension == V_030000_SQ_TEX_DIM_2D_ARRAY_MSAA) {
      /* MIP_ADDRESS is used for the FMask address instead. */
      /* TODO(Triang3l): FMask. */
      descriptor_out[3] = S_03000C_MIP_ADDRESS(0);

      unsigned const samples_log2 = util_logbase2((uint32_t)image->vk.samples);
      if (gpu_info->gfx_level >= CAYMAN) {
         descriptor_out[4] |= S_030010_LOG2_NUM_FRAGMENTS(samples_log2);
      }
      /* LAST_LEVEL is used for the sample count instead. */
      descriptor_out[5] |= S_030014_LAST_LEVEL(samples_log2);
   } else {
      uint32_t const second_level_index =
         MIN2(image_view_create_info->subresourceRange.baseMipLevel + 1, image->vk.mip_levels - 1);
      descriptor_out[3] = S_03000C_MIP_ADDRESS(
         image_bo_offset_256b +
         (is_stencil_layout
             ? image->surface.u.legacy.zs.stencil_level[second_level_index].offset_256B
             : image->surface.u.legacy.level[second_level_index].offset_256B));

      uint32_t const last_level =
         (image_view_create_info->subresourceRange.levelCount == VK_REMAINING_MIP_LEVELS
             ? image->vk.mip_levels - image_view_create_info->subresourceRange.baseMipLevel
             : image_view_create_info->subresourceRange.levelCount) -
         1;
      if (last_level != 0) {
         descriptor_out[5] |= S_030014_LAST_LEVEL(last_level);
         descriptor_out[6] |= S_030018_MAX_ANISO_RATIO(4);
      }
   }

   return true;
}

uint32_t
terakan_image_create_color_descriptor(
   VkImageViewCreateInfo const * const image_view_create_info,
   struct terakan_color_descriptor * const descriptor_out,
   struct terakan_color_meta_descriptor * const meta_descriptor_out_opt)
{
   bool const is_stencil_aspect =
      image_view_create_info->subresourceRange.aspectMask == VK_IMAGE_ASPECT_STENCIL_BIT;

   uint32_t color_format, number_type, swap;
   if (is_stencil_aspect && vk_format_has_depth(image_view_create_info->format)) {
      /* The getters return the values for the depth aspect, not stencil. */
      color_format = V_028C70_COLOR_8;
      number_type = V_028C70_NUMBER_UINT;
      swap = V_028C70_SWAP_STD;
   } else {
      color_format = terakan_format_color_get_format(image_view_create_info->format);
      if (color_format == V_028C70_COLOR_INVALID) {
         return 0;
      }
      number_type = terakan_format_color_get_number_type(image_view_create_info->format);
      if (number_type == UINT32_MAX) {
         return 0;
      }
      swap = terakan_format_color_get_swap(image_view_create_info->format);
      if (swap == UINT32_MAX) {
         return 0;
      }
   }

   struct terakan_image const * const image =
      terakan_image_from_handle(image_view_create_info->image);

   bool const is_stencil_layout = is_stencil_aspect && image->surface.has_stencil;
   struct legacy_surf_level const * const level =
      is_stencil_layout
         ? &image->surface.u.legacy.zs
               .stencil_level[image_view_create_info->subresourceRange.baseMipLevel]
         : &image->surface.u.legacy.level[image_view_create_info->subresourceRange.baseMipLevel];
   /* Only LINEAR_ALIGNED is currently supported for linear, not LINEAR_GENERAL. */
   assert(level->mode != (enum radeon_surf_mode)0);

   /* Color descriptors support fewer slices than texture resource descriptors, but meta draws may
    * still need to access all the slices. Between the slices, there's bank rotation in the tiling,
    * so it's not possible to just adjust the base pointer directly to baseArrayLayer all the time,
    * only by numbers of slices aligned to the rotation granularity on the chip revision.
    * Restrict the descriptor to the range of TERAKAN_IMAGE_MAX_TARGET_SLICES slices that includes
    * baseArrayLayer.
    */
   uint32_t const create_info_slice_start = image_view_create_info->subresourceRange.baseArrayLayer;
   uint32_t const base_slice_start =
      create_info_slice_start & ~(uint32_t)(TERAKAN_IMAGE_MAX_TARGET_SLICES - 1);
   descriptor_out->base =
      (uint32_t)(image->bo_offset / 256 + level->offset_256B +
                 (VkDeviceSize)level->slice_size_dw * base_slice_start / (256 / sizeof(uint32_t)));

   /* nblk is expected to have already been aligned appropriately in the surface computation. */
   descriptor_out->pitch = S_028C64_PITCH_TILE_MAX(level->nblk_x / 8 - 1);

   /* Linear pitch is always at least 64 elements, micro-tiles are 8x8. */
   descriptor_out->slice = S_028C68_SLICE_TILE_MAX(level->nblk_x * level->nblk_y / 64 - 1);

   uint32_t const view_slice_start = create_info_slice_start - base_slice_start;
   uint32_t const create_info_slice_max =
      (image_view_create_info->subresourceRange.layerCount == VK_REMAINING_ARRAY_LAYERS
          ? image->vk.array_layers
          : create_info_slice_start + image_view_create_info->subresourceRange.layerCount) -
      1;
   uint32_t const view_slice_max =
      MIN2(create_info_slice_max - base_slice_start, TERAKAN_IMAGE_MAX_TARGET_SLICES - 1);
   descriptor_out->view =
      S_028C6C_SLICE_START(view_slice_start) | S_028C6C_SLICE_MAX(view_slice_max);

   bool blend_clamp = false;
   uint32_t source_format = V_028C70_EXPORT_4C_32BPC;
   switch (number_type) {
   case V_028C70_NUMBER_UNORM:
   case V_028C70_NUMBER_SNORM:
   case V_028C70_NUMBER_SRGB:
      blend_clamp = true;
      if (TERAKAN_FORMAT_COLOR_16BPC_EXPORT_NORM_FORMATS & ((uint64_t)1 << color_format)) {
         source_format = V_028C70_EXPORT_4C_16BPC;
      }
      break;
   case V_028C70_NUMBER_FLOAT:
      if (TERAKAN_FORMAT_COLOR_16BPC_EXPORT_FLOAT_FORMATS & ((uint64_t)1 << color_format)) {
         source_format = V_028C70_EXPORT_4C_16BPC;
      }
      break;
   default:
      break;
   }
   descriptor_out->info = S_028C70_FORMAT(color_format) |
                          S_028C70_ARRAY_MODE(terakan_image_array_mode_ac_to_hw(level->mode)) |
                          S_028C70_NUMBER_TYPE(number_type) | S_028C70_COMP_SWAP(swap) |
                          S_028C70_SIMPLE_FLOAT(1) | S_028C70_SOURCE_FORMAT(source_format);
   if (terakan_format_color_is_blendable(color_format, number_type)) {
      descriptor_out->info |= S_028C70_BLEND_CLAMP(blend_clamp);
   } else {
      descriptor_out->info |= S_028C70_BLEND_BYPASS(1);
   }

   struct terakan_gpu_info const * const gpu_info =
      &container_of(image->vk.base.device->physical, struct terakan_physical_device const, vk)
          ->winsys->gpu_info;
   descriptor_out->attrib =
      S_028C74_NON_DISP_TILING_ORDER(terakan_image_uses_cb_non_display_tiling(
         gpu_info->gfx_level, image->vk.format, level->mode <= RADEON_SURF_MODE_LINEAR_ALIGNED)) |
      S_028C74_TILE_SPLIT(terakan_image_tile_split_bytes_to_hw(
         is_stencil_layout ? image->surface.u.legacy.stencil_tile_split
                           : image->surface.u.legacy.tile_split)) |
      S_028C74_NUM_BANKS(gpu_info->tile_banks_log2 - 1) |
      S_028C74_BANK_WIDTH(util_logbase2(image->surface.u.legacy.bankw)) |
      S_028C74_BANK_HEIGHT(util_logbase2(image->surface.u.legacy.bankh)) |
      S_028C74_MACRO_TILE_ASPECT(util_logbase2(image->surface.u.legacy.mtilea)) |
      S_028C74_FMASK_BANK_HEIGHT(util_logbase2(image->surface.u.legacy.bankh));
   if (gpu_info->gfx_level >= CAYMAN) {
      /* Cayman has EQAA, and additionally doesn't support displayable tiling for 128 bits per pixel
       * color targets.
       */
      unsigned const samples_log2 = util_logbase2((uint32_t)image->vk.samples);
      enum pipe_swizzle const alpha_swizzle =
         (enum pipe_swizzle)vk_format_description(image_view_create_info->format)->swizzle[3];
      descriptor_out->attrib |= S_028C74_NUM_SAMPLES(samples_log2) |
                                S_028C74_NUM_FRAGMENTS(samples_log2) |
                                S_028C74_FORCE_DST_ALPHA_1(alpha_swizzle == PIPE_SWIZZLE_1 ||
                                                           alpha_swizzle == PIPE_SWIZZLE_NONE);
   }

   /* Allowing uncompressed views of compressed and subsampled images. */
   descriptor_out->dim =
      S_028C78_WIDTH_MAX(
         (u_minify(image->vk.extent.width, image_view_create_info->subresourceRange.baseMipLevel) +
          (image->surface.blk_w - 1)) /
            image->surface.blk_w -
         1) |
      S_028C78_HEIGHT_MAX(
         (u_minify(image->vk.extent.height, image_view_create_info->subresourceRange.baseMipLevel) +
          (image->surface.blk_h - 1)) /
            image->surface.blk_h -
         1);

   if (meta_descriptor_out_opt != NULL) {
      meta_descriptor_out_opt->cmask = descriptor_out->base;
      meta_descriptor_out_opt->cmask_slice = S_028C80_TILE_MAX(0);
      meta_descriptor_out_opt->fmask = descriptor_out->base;
      meta_descriptor_out_opt->fmask_slice =
         S_028C88_TILE_MAX(G_028C68_SLICE_TILE_MAX(descriptor_out->slice));
   }

   /* TODO(Triang3l): CMask, FMask. */

   return view_slice_max - view_slice_start + 1;
}

VKAPI_ATTR void VKAPI_CALL
terakan_DestroyImage(VkDevice const deviceHandle, VkImage const imageHandle,
                     VkAllocationCallbacks const * const pAllocator)
{
   struct terakan_image * const image = terakan_image_from_handle(imageHandle);

   if (image == NULL) {
      return;
   }

   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);

   vk_image_finish(&image->vk);

   vk_free2(&device->vk.alloc, pAllocator, image);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreateImage(VkDevice const deviceHandle, VkImageCreateInfo const * const pCreateInfo,
                    VkAllocationCallbacks const * const pAllocator, VkImage * const pImage)
{
   VkResult result;

   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   struct terakan_image * const image =
      vk_alloc2(&device->vk.alloc, pAllocator, sizeof(struct terakan_image),
                alignof(struct terakan_image), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (image == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   vk_image_init(&device->vk, &image->vk, pCreateInfo);

   struct terakan_winsys const * const winsys =
      container_of(device->vk.physical, struct terakan_physical_device const, vk)->winsys;
   if (!winsys->surface_fn->translate_image_create_info(winsys, pCreateInfo, &image->surface)) {
      result = vk_errorf(device, VK_ERROR_UNKNOWN,
                         "Failed to translate the image creation info into surface info");
      goto fail_image;
   }

   image->bo = NULL;
   image->bo_offset = 0;

   *pImage = terakan_image_to_handle(image);

   return VK_SUCCESS;

fail_image:
   vk_image_finish(&image->vk);
   vk_free2(&device->vk.alloc, pAllocator, image);
   return result;
}

VKAPI_ATTR void VKAPI_CALL
terakan_DestroyImageView(VkDevice const deviceHandle, VkImageView const imageView,
                         VkAllocationCallbacks const * const pAllocator)
{
   struct terakan_image_view * const image_view = terakan_image_view_from_handle(imageView);

   if (image_view == NULL) {
      return;
   }

   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);

   vk_image_view_finish(&image_view->vk);

   vk_free2(&device->vk.alloc, pAllocator, image_view);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreateImageView(VkDevice const deviceHandle,
                        VkImageViewCreateInfo const * const pCreateInfo,
                        VkAllocationCallbacks const * const pAllocator, VkImageView * const pView)
{
   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   struct terakan_image_view * const image_view =
      vk_alloc2(&device->vk.alloc, pAllocator, sizeof(struct terakan_image_view),
                alignof(struct terakan_image_view), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
   if (image_view == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   vk_image_view_init(&device->vk, &image_view->vk, false, pCreateInfo);

   struct terakan_image const * const image = terakan_image_from_handle(pCreateInfo->image);

   image_view->bo = image->bo;

   if (!terakan_image_create_resource_descriptor(pCreateInfo, image_view->resource)) {
      memset(image_view->resource, 0, sizeof(image_view->resource));
   }

   if (terakan_image_create_color_descriptor(pCreateInfo, &image_view->color,
                                             &image_view->color_meta) == 0) {
      memset(&image_view->color, 0, sizeof(image_view->color));
      memset(&image_view->color_meta, 0, sizeof(image_view->color_meta));
   }

   /* TODO(Triang3l): Other descriptor types. */

   *pView = terakan_image_view_to_handle(image_view);

   return VK_SUCCESS;
}
