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

#ifndef TERAKAN_FORMAT_H
#define TERAKAN_FORMAT_H

#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600_formats.h"
#include "util/format/u_format.h"
#include "util/u_math.h"

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

bool terakan_format_is_linear_only(VkFormat format);
bool terakan_format_is_tiled_only(VkFormat format);

/* For images with both depth and stencil, these functions always return the values for the depth
 * aspect.
 * The stencil aspect is always 8 UINT with STD swap (Vulkan has both depth and stencil in X
 * depending on the view aspect).
 */
/* Returns COLOR_INVALID if invalid. */
uint32_t terakan_format_color_get_format(VkFormat format);
/* Returns UINT32_MAX if invalid. */
uint32_t terakan_format_color_get_number_type(VkFormat format);
/* Returns UINT32_MAX if invalid. */
uint32_t terakan_format_color_get_swap(VkFormat format);
bool terakan_format_color_is_blendable(uint32_t color_format, uint32_t number_type);

#define TERAKAN_FORMAT_COLOR_16BPC_EXPORT_NORM_FORMATS                                             \
   (((uint64_t)1 << V_028C70_COLOR_8) | ((uint64_t)1 << V_028C70_COLOR_8_8) |                      \
    ((uint64_t)1 << V_028C70_COLOR_5_6_5) | ((uint64_t)1 << V_028C70_COLOR_1_5_5_5) |              \
    ((uint64_t)1 << V_028C70_COLOR_4_4_4_4) | ((uint64_t)1 << V_028C70_COLOR_5_5_5_1) |            \
    ((uint64_t)1 << V_028C70_COLOR_2_10_10_10) | ((uint64_t)1 << V_028C70_COLOR_8_8_8_8) |         \
    ((uint64_t)1 << V_028C70_COLOR_10_10_10_2))
#define TERAKAN_FORMAT_COLOR_16BPC_EXPORT_FLOAT_FORMATS                                            \
   (((uint64_t)1 << V_028C70_COLOR_16) | ((uint64_t)1 << V_028C70_COLOR_16_FLOAT) |                \
    ((uint64_t)1 << V_028C70_COLOR_16_16) | ((uint64_t)1 << V_028C70_COLOR_16_16_FLOAT) |          \
    ((uint64_t)1 << V_028C70_COLOR_10_11_11) | ((uint64_t)1 << V_028C70_COLOR_10_11_11_FLOAT) |    \
    ((uint64_t)1 << V_028C70_COLOR_16_16_16_16) |                                                  \
    ((uint64_t)1 << V_028C70_COLOR_16_16_16_16_FLOAT))

/* Depth and stencil are always stored in memory separately, with the stencil buffer being 8-bit.
 * VK_FORMAT_D16_UNORM_S8_UINT = 16 UNORM + 8 UINT.
 * VK_FORMAT_D24_UNORM_S8_UINT = 8_24 UNORM + 8 UINT.
 * VK_FORMAT_D32_SFLOAT_S8_UINT = 32 FLOAT + 8 UINT.
 */

/* Returns Z_INVALID if invalid. */
uint32_t terakan_format_depth_get_format(VkFormat format);
bool terakan_format_has_stencil_8(VkFormat format);

#define TERAKAN_FORMAT_DATA_VERTEX_ONLY_FORMATS                                                    \
   (((uint64_t)1 << FMT_3_3_2) | ((uint64_t)1 << FMT_8_24_FLOAT) |                                 \
    ((uint64_t)1 << FMT_24_8_FLOAT) | ((uint64_t)1 << FMT_10_11_11) |                              \
    ((uint64_t)1 << FMT_11_11_10) | ((uint64_t)1 << FMT_11_11_10_FLOAT) |                          \
    ((uint64_t)1 << FMT_8_8_8) | ((uint64_t)1 << FMT_16_16_16) |                                   \
    ((uint64_t)1 << FMT_16_16_16_FLOAT))

#define TERAKAN_FORMAT_DATA_SUBSAMPLED_FORMATS                                                     \
   (((uint64_t)1 << FMT_GB_GR) | ((uint64_t)1 << FMT_BG_RG))

#define TERAKAN_FORMAT_DATA_BLOCK_COMPRESSED_FORMATS                                               \
   (((uint64_t)1 << FMT_BC1) | ((uint64_t)1 << FMT_BC2) | ((uint64_t)1 << FMT_BC3) |               \
    ((uint64_t)1 << FMT_BC4) | ((uint64_t)1 << FMT_BC5) | ((uint64_t)1 << FMT_BC6) |               \
    ((uint64_t)1 << FMT_BC7))

#define TERAKAN_FORMAT_DATA_TEXTURE_ONLY_FORMATS                                                   \
   (TERAKAN_FORMAT_DATA_SUBSAMPLED_FORMATS | TERAKAN_FORMAT_DATA_BLOCK_COMPRESSED_FORMATS)

/* For images with both depth and stencil, these functions always return the values for the depth
 * aspect.
 * The stencil aspect is always 8 UINT with X001 destination component selection (Vulkan has both
 * depth and stencil in X depending on the view aspect).
 */
/* Returns a format that can be used for both vertex and texture fetching, or FMT_INVALID if
 * invalid.
 */
uint32_t terakan_format_data_get_common_format(VkFormat format);
/* Returns UINT32_MAX if invalid. */
uint32_t terakan_format_data_get_number_format(VkFormat format);
unsigned char const * terakan_format_data_get_swizzle(VkFormat format);

static inline uint32_t
terakan_format_data_pipe_swizzle_to_dst_sel(enum pipe_swizzle swizzle,
                                            enum pipe_swizzle const missing_substitute)
{
   if (swizzle == PIPE_SWIZZLE_NONE) {
      swizzle = missing_substitute;
   }
   if (swizzle >= PIPE_SWIZZLE_X && swizzle <= PIPE_SWIZZLE_W) {
      return V_03000C_SQ_SEL_X + ((unsigned)swizzle - (unsigned)PIPE_SWIZZLE_X);
   }
   return swizzle == PIPE_SWIZZLE_1 ? V_03000C_SQ_SEL_1 : V_03000C_SQ_SEL_0;
}

static inline uint32_t
terakan_format_data_component_swizzle_to_dst_sel(VkComponentSwizzle component_swizzle,
                                                 VkComponentSwizzle const identity_swizzle,
                                                 unsigned char const format_swizzle[4])
{
   if (component_swizzle == VK_COMPONENT_SWIZZLE_IDENTITY) {
      component_swizzle = identity_swizzle;
   }
   if (component_swizzle >= VK_COMPONENT_SWIZZLE_R && component_swizzle <= VK_COMPONENT_SWIZZLE_A) {
      return terakan_format_data_pipe_swizzle_to_dst_sel(
         (enum pipe_swizzle)
            format_swizzle[(unsigned)component_swizzle - (unsigned)VK_COMPONENT_SWIZZLE_R],
         identity_swizzle == VK_COMPONENT_SWIZZLE_A ? PIPE_SWIZZLE_1 : PIPE_SWIZZLE_0);
   }
   return component_swizzle == VK_COMPONENT_SWIZZLE_ONE ? V_03000C_SQ_SEL_1 : V_03000C_SQ_SEL_0;
}

/* Returns FMT_INVALID if invalid. */
uint32_t terakan_format_texture_get_format(VkFormat format);
uint32_t terakan_format_texture_get_word4_signs(VkFormat format);

/* Returns FMT_INVALID if invalid. */
uint32_t terakan_format_vertex_get_format(VkFormat format);
uint32_t terakan_format_vertex_get_sign(VkFormat format);

static inline uint32_t
terakan_format_pitch_alignment_linear_bytes(unsigned const bpe,
                                            unsigned const tile_pipe_interleave_bytes_log2)
{
   return MAX2((uint32_t)bpe << 6, (uint32_t)1 << tile_pipe_interleave_bytes_log2);
}

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_FORMAT_H */
