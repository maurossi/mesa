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

#ifndef TERAKAN_IMAGE_H
#define TERAKAN_IMAGE_H

#include "terakan_bo.h"
#include "terakan_descriptor.h"
#include "terakan_format.h"

#include "gallium/drivers/r600/evergreend.h"
#include "util/bitscan.h"
#include "util/macros.h"
#include "util/u_endian.h"
#include "util/u_math.h"
#include "vk_format.h"
#include "vk_image.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_LOG2 14
#define TERAKAN_IMAGE_MAX_WIDTH_HEIGHT      (1 << TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_LOG2)
/* Maximum depth or array layer count supported by TC. */
#define TERAKAN_IMAGE_MAX_SLICES_LOG2 13
#define TERAKAN_IMAGE_MAX_SLICES      (1 << TERAKAN_IMAGE_MAX_SLICES_LOG2)
/* Maximum depth or array layer count supported by CB and DB.
 * In Terakan, images can use more than this (up to TERAKAN_IMAGE_MAX_SLICES) if they can't
 * potentially be accessed by the application through image views referencing arbitrary Z or array
 * layer ranges that'd require CB or DB access.
 */
#define TERAKAN_IMAGE_MAX_TARGET_SLICES_LOG2 11
#define TERAKAN_IMAGE_MAX_TARGET_SLICES      (1 << TERAKAN_IMAGE_MAX_TARGET_SLICES_LOG2)

struct terakan_image_surface_level {
   uint32_t offset_in_memory_bytes_shr8;
   uint32_t slice_size_bytes_shr8;
   /* [2] is depth for 3D, array layers otherwise. */
   uint16_t aligned_extent_surfels[3];
   /* 2D is degraded to 1D for small mips. */
   uint8_t array_mode;
};

struct terakan_image_surface_tiling {
   /* ATTRIB register field values (log2, some being exponent-biased). */
   uint8_t attrib_tile_split; /* 0 = 2^6 bytes. */
   uint8_t attrib_bank_width;
   uint8_t attrib_bank_height;
   uint8_t attrib_macro_tile_aspect;

   /* Not including NUM_BANKS, assuming it's the same for all images on the device, because it can't
    * be changed in a useful way on DRM Radeon as of 2.50.0.
    *
    * In most cases, the R800 AddrLib uses the logical bank count, which is the number of banks
    * multiplied by the number of ranks from MC_ARB_RAMCFG. One exception is that under some other
    * conditions, for large micro-tiles and large numbers of logical banks, it can divide NUM_BANKS
    * by 2.
    * However, DRM Radeon 2.50.0 doesn't provide the number of ranks (only the number of banks) to
    * applications, and also it doesn't expose a field for setting NUM_BANKS in BO metadata.
    */

   /* CB non-display tiling = TC non-display tiling || array mode is linear. */
   bool tc_non_display;
};

struct terakan_image_surface_aspect {
   uint32_t alignment_bytes_shr8;
   uint32_t offset_in_memory_bytes_shr8;
   uint32_t size_bytes_shr8;

   uint8_t bytes_per_block;

   struct terakan_image_surface_tiling tiling;

   struct terakan_image_surface_level levels[TERAKAN_IMAGE_MAX_WIDTH_HEIGHT + 1];
};

struct terakan_image_surface {
   uint32_t alignment_bytes_shr8;
   uint32_t size_bytes_shr8;

   /* If the image has a combined depth and stencil format and has DB usage enabled, DB register
    * fields shared between depth and stencil are the same for the two aspects.
    */
   struct terakan_image_surface_aspect aspects[TERAKAN_FORMAT_MAX_ASPECTS];
};

/* TODO(Triang3l): Replace in favor of terakan_format_aspect_index, but need to handle errors when
 * using it, and never to use it with combined depth and stencil.
 */
static inline unsigned
terakan_image_surface_aspect_index(VkFormat const image_format, VkImageAspectFlagBits const aspect)
{
   if (aspect == VK_IMAGE_ASPECT_STENCIL_BIT) {
      return vk_format_has_depth(image_format) ? 1 : 0;
   }
   return 0;
}

struct terakan_image {
   struct vk_image vk;

   /* Derived from the format, but needed in many places, so stored. */
   struct terakan_format_info format_info;

   struct terakan_image_surface surface;

   struct terakan_bo const * bo;
   VkDeviceSize va;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_image, vk.base, VkImage, VK_OBJECT_TYPE_IMAGE)

static inline bool
terakan_image_is_big_endian(UNUSED struct terakan_image const * const image)
{
#if UTIL_ARCH_BIG_ENDIAN
   /* DB doesn't support endian swapping, but neither does it support linear images, so the
    * endianness doesn't matter to the host.
    * For other cases of tiled images, however, set the endian swap because host endianness is
    * likely to be expected when exporting the image (presentation in DRI expects it, the Gallium
    * R600 driver also uses the host endianness).
    */
   return !(image->vk.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
#else
   return false;
#endif
}

struct terakan_image_descriptor_create_info {
   struct terakan_image const * image;
   VkImageViewType view_type;
   struct terascale_format_info view_format;
   bool force_little_endian;
   unsigned image_aspect_index;
   uint32_t base_mip_level;
   /* Can be VK_REMAINING_MIP_LEVELS. */
   uint32_t level_count;
   uint32_t base_array_layer;
   /* Can be VK_REMAINING_ARRAY_LAYERS. */
   uint32_t layer_count;
};

/* For transfer purposes, cross-aspect reinterpretation is allowed,
 * VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT and VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT are
 * assumed to be always enabled for all images they're applicable to, and 2D array views of multiple
 * layers of 3D images are supported.
 */
bool terakan_image_create_resource_descriptor(
   struct terakan_image_descriptor_create_info const * descriptor_create_info,
   VkComponentMapping const * component_mapping, uint32_t descriptor_out[8]);

/* Returns the number of array layers accessible through the descriptor as color descriptors support
 * fewer layers than texture resource descriptors.
 *
 * Meta draws like copying can access all layers of the image by adding the result of this function
 * to baseArrayLayer, creating a new color descriptor, and performing the draw again for the next
 * subset of layers.
 *
 * For transfer purposes, cross-aspect reinterpretation is allowed, and
 * VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT and VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT are
 * assumed to be always enabled for all images they're applicable to.
 */
uint32_t terakan_image_create_color_descriptor(
   struct terakan_image_descriptor_create_info const * descriptor_create_info,
   struct terakan_color_descriptor * descriptor_out,
   struct terakan_color_meta_descriptor * meta_descriptor_out_opt);

bool terakan_image_create_depth_stencil_descriptor(
   VkImageViewCreateInfo const * image_view_create_info,
   struct terakan_depth_stencil_descriptor * descriptor_out);

struct terakan_image_view {
   struct vk_image_view vk;

   struct terakan_bo const * bo;

   uint32_t resource[8];

   struct terakan_color_descriptor color;
   struct terakan_color_meta_descriptor color_meta;

   struct terakan_depth_stencil_descriptor depth_stencil;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_image_view, vk.base, VkImageView, VK_OBJECT_TYPE_IMAGE_VIEW)

#ifdef __cplusplus
}
#endif

#endif
