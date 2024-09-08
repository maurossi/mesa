/*
 * Copyright © 2023 Vitaliy Triang3l Kuzmin
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

#include "gallium/drivers/r600/evergreend.h"
#include "util/bitscan.h"
#include "util/u_math.h"
#include "ac_surface.h"
#include "vk_format.h"
#include "vk_image.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES_LOG2 14
#define TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES                                                   \
   (1 << TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES_LOG2)
#define TERAKAN_IMAGE_MAX_DEPTH_2D_SLICES_LOG2 13
#define TERAKAN_IMAGE_MAX_DEPTH_2D_SLICES      (1 << TERAKAN_IMAGE_MAX_DEPTH_2D_SLICES_LOG2)
#define TERAKAN_IMAGE_MAX_TARGET_SLICES_LOG2   11
#define TERAKAN_IMAGE_MAX_TARGET_SLICES        (1 << TERAKAN_IMAGE_MAX_TARGET_SLICES_LOG2)

static inline uint32_t
terakan_image_array_mode_ac_to_hw(enum radeon_surf_mode const mode)
{
   switch ((uint32_t)mode) {
   default:
   case 0: /* RADEON_SURF_MODE_LINEAR declared in radeon_drm.h. */
      return V_028C70_ARRAY_LINEAR_GENERAL;
   case RADEON_SURF_MODE_LINEAR_ALIGNED:
      return V_028C70_ARRAY_LINEAR_ALIGNED;
   case RADEON_SURF_MODE_1D:
      return V_028C70_ARRAY_1D_TILED_THIN1;
   case RADEON_SURF_MODE_2D:
      return V_028C70_ARRAY_2D_TILED_THIN1;
   }
}

static inline unsigned
terakan_image_tile_split_bytes_to_hw(uint32_t const tile_split)
{
   assert(tile_split >= ((uint32_t)1 << 6) && tile_split <= ((uint32_t)1 << 12) &&
          util_is_power_of_two_or_zero(tile_split));
   return util_logbase2(tile_split) - 6;
}

uint32_t terakan_image_get_optimal_tiling_array_mode(VkImageCreateInfo const * image_create_info);

struct terakan_image {
   struct vk_image vk;

   /* Bytes per element (bpe) is:
    * - For depth / stencil, for the depth aspect (2 for 16_UNORM, 4 for 24_UNORM / 32_SFLOAT).
    *   If depth and stencil are combined, stencil BPE must be assumed to be 1 (not explicitly
    *   stored).
    * - 1 for S8.
    * - 3 for R8G8B8, 6 for R16G16B16, 9 for R32G32B32.
    *
    * 1 and 1_REVERSED are completely unsupported (as of May 2023, Vulkan doesn't have any 1-bit
    * formats).
    *
    * The stencil fields are valid only if has_stencil is true.
    *
    * u.legacy.num_banks may be zero - use terakan_physical_device_tiling_info::banks_log2 instead.
    */
   struct radeon_surf surface;

   struct terakan_bo const * bo;
   VkDeviceSize bo_offset;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_image, vk.base, VkImage, VK_OBJECT_TYPE_IMAGE)

bool terakan_image_uses_tc_non_display_tiling(bool is_r9xx, VkFormat image_format,
                                              bool level_is_linear);
bool terakan_image_uses_cb_non_display_tiling(bool is_r9xx, VkFormat image_format,
                                              bool level_is_linear);

bool terakan_image_create_resource_descriptor(VkImageViewCreateInfo const * image_view_create_info,
                                              uint32_t descriptor_out[8]);

/* Returns the number of array layers accessible through the descriptor as color descriptors support
 * fewer layers than texture resource descriptors.
 * Meta draws like copying can access all layers of the image by adding the result of this function
 * to baseArrayLayer, creating a new color descriptor, and performing the draw again for the next
 * subset of layers.
 * On failure, returns 0.
 */
uint32_t terakan_image_create_color_descriptor(
   VkImageViewCreateInfo const * image_view_create_info,
   struct terakan_color_descriptor * descriptor_out,
   struct terakan_color_meta_descriptor * meta_descriptor_out_opt);

struct terakan_device;

struct terakan_image_winsys_fn {
   bool (*get_surface_info)(struct terakan_device const * device,
                            VkImageCreateInfo const * image_create_info,
                            struct radeon_surf * surface_out);
};

struct terakan_image_view {
   struct vk_image_view vk;

   struct terakan_bo const * bo;

   uint32_t resource[8];

   struct terakan_color_descriptor color;
   struct terakan_color_meta_descriptor color_meta;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_image_view, vk.base, VkImageView, VK_OBJECT_TYPE_IMAGE_VIEW)

#ifdef __cplusplus
}
#endif

#endif
