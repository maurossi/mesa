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

#ifndef TERAKAN_FORMAT_H
#define TERAKAN_FORMAT_H

#include "amd/terascale/common/terascale_format.h"
#include "util/format/u_format.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* "Surfel" (surface element) here means what "element" is in AddrLib terms: a block, or, for
 * 3x-expanded formats (8_8_8, 16_16_16, 32_32_32), a channel.
 */

static inline bool
terakan_format_is_expand_3x(unsigned const bytes_per_block)
{
   return bytes_per_block == 3 || bytes_per_block == (3 << 1) || bytes_per_block == (3 << 2);
}

static inline unsigned
terakan_format_surfels_per_block(unsigned const bytes_per_block)
{
   return terakan_format_is_expand_3x(bytes_per_block) ? 3 : 1;
}

static inline enum terascale_swizzle
terakan_format_apply_component_swizzle(struct terascale_format_info const format_info,
                                       VkComponentSwizzle component_swizzle,
                                       VkComponentSwizzle const identity_swizzle)
{
   if (component_swizzle == VK_COMPONENT_SWIZZLE_IDENTITY) {
      component_swizzle = identity_swizzle;
   }
   switch (component_swizzle) {
   case VK_COMPONENT_SWIZZLE_R:
      return (enum terascale_swizzle)format_info.swizzle_r;
   case VK_COMPONENT_SWIZZLE_G:
      return (enum terascale_swizzle)format_info.swizzle_g;
   case VK_COMPONENT_SWIZZLE_B:
      return (enum terascale_swizzle)format_info.swizzle_b;
   case VK_COMPONENT_SWIZZLE_A:
      return (enum terascale_swizzle)format_info.swizzle_a;
   case VK_COMPONENT_SWIZZLE_ONE:
      return TERASCALE_SWIZZLE_1;
   default:
      return TERASCALE_SWIZZLE_0;
   }
}

/* Mappings of Vulkan format aspect masks to aspect indices used within Terakan.
 * Aspect 0 for simplicity is considered the "main" aspect, which is always present in any format
 * supported by Terakan.
 */

#define TERAKAN_FORMAT_MAX_ASPECTS 3

/* The aspect list for each aspect map entry must not have gaps. */
#define TERAKAN_FORMAT_ASPECT_MAP_DECLARE_ALL                                                      \
   TERAKAN_FORMAT_ASPECT_MAP_DECLARE(TERAKAN_FORMAT_ASPECT_MAP_0_COLOR, VK_IMAGE_ASPECT_COLOR_BIT, \
                                     VK_IMAGE_ASPECT_NONE, VK_IMAGE_ASPECT_NONE)                   \
   TERAKAN_FORMAT_ASPECT_MAP_DECLARE(TERAKAN_FORMAT_ASPECT_MAP_0_DEPTH, VK_IMAGE_ASPECT_DEPTH_BIT, \
                                     VK_IMAGE_ASPECT_NONE, VK_IMAGE_ASPECT_NONE)                   \
   TERAKAN_FORMAT_ASPECT_MAP_DECLARE(TERAKAN_FORMAT_ASPECT_MAP_0_STENCIL,                          \
                                     VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_ASPECT_NONE,            \
                                     VK_IMAGE_ASPECT_NONE)                                         \
   TERAKAN_FORMAT_ASPECT_MAP_DECLARE(TERAKAN_FORMAT_ASPECT_MAP_0_DEPTH_1_STENCIL,                  \
                                     VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT,       \
                                     VK_IMAGE_ASPECT_NONE)                                         \
   TERAKAN_FORMAT_ASPECT_MAP_DECLARE(TERAKAN_FORMAT_ASPECT_MAP_0_Y_1_CBCR,                         \
                                     VK_IMAGE_ASPECT_PLANE_0_BIT, VK_IMAGE_ASPECT_PLANE_1_BIT,     \
                                     VK_IMAGE_ASPECT_NONE)                                         \
   TERAKAN_FORMAT_ASPECT_MAP_DECLARE(TERAKAN_FORMAT_ASPECT_MAP_0_Y_1_CB_2_CR,                      \
                                     VK_IMAGE_ASPECT_PLANE_0_BIT, VK_IMAGE_ASPECT_PLANE_1_BIT,     \
                                     VK_IMAGE_ASPECT_PLANE_2_BIT)

#define TERAKAN_FORMAT_ASPECT_MAP_DECLARE(name, aspect_0, aspect_1, aspect_2) name,
enum terakan_format_aspect_map {
   /* clang-format off */
   TERAKAN_FORMAT_ASPECT_MAP_DECLARE_ALL
   TERAKAN_FORMAT_ASPECT_MAP_COUNT,
   /* clang-format on */
   TERAKAN_FORMAT_ASPECT_MAP_INVALID = TERAKAN_FORMAT_ASPECT_MAP_COUNT,
};
#undef TERAKAN_FORMAT_ASPECT_MAP_DECLARE

extern VkImageAspectFlagBits const
   terakan_format_aspect_map_aspects[TERAKAN_FORMAT_ASPECT_MAP_COUNT][TERAKAN_FORMAT_MAX_ASPECTS];
extern VkImageAspectFlags const
   terakan_format_aspect_map_aspect_masks[TERAKAN_FORMAT_ASPECT_MAP_COUNT];

/* Returns TERAKAN_FORMAT_ASPECT_MAP_INVALID if there's no supported aspect map for the format. */
enum terakan_format_aspect_map
terakan_format_aspect_map_for_format_aspects(VkImageAspectFlags format_aspects);

unsigned terakan_format_aspect_index(enum terakan_format_aspect_map aspect_map,
                                     VkImageAspectFlagBits aspect, unsigned result_if_not_present);

struct terakan_format_info {
   enum terakan_format_aspect_map aspect_map;
   struct terascale_format_info aspect_formats[TERAKAN_FORMAT_MAX_ASPECTS];
};

/* Returns whether the format is supported.
 *
 * Note that all supported depth / stencil formats should have a corresponding SQ / CB format, so
 * it's okay not to try to implement depth / stencil usage for the format too if this function
 * returns false.
 *
 * If the format is not supported, the output is left in an undefined state.
 *
 * Image formats must be decomposed into aspect formats to get the corresponding hardware register
 * values for them, but for buffers, using this function is not necessary, and the
 * application-provided format can be used directly without decomposing it into aspects, as only
 * single-aspect formats are supported for buffers, and thus the aspect format matches the
 * application-provided format.
 */
bool terakan_format_info_get(VkFormat format, struct terakan_format_info * info_out);

static inline uint32_t
terakan_format_pitch_alignment_linear_surfels(unsigned const bytes_per_surfel,
                                              unsigned const tile_pipe_interleave_bytes_log2)
{
   /* From AddrLib's R800Lib::HwlGetPitchAlignmentLinear. */
   uint32_t const tile_pipe_interleave_bytes = (uint32_t)1 << tile_pipe_interleave_bytes_log2;
   uint32_t const surfels_per_pipe_interleave = tile_pipe_interleave_bytes / bytes_per_surfel;
   return MAX2(surfels_per_pipe_interleave, 64);
}

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_FORMAT_H */
