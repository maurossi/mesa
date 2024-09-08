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

#include "terakan_descriptor.h"

#include "terakan_format.h"

#include "gallium/drivers/r600/evergreend.h"
#include "util/u_endian.h"
#include "util/u_math.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void
terakan_color_descriptor_calculate_buffer_base_pitch_dim_offset(
   struct terakan_color_descriptor * const descriptor, uint64_t const va,
   VkDeviceSize const elements, unsigned const bytes_per_element,
   unsigned const tile_pipe_interleave_bytes_log2, uint32_t * const alignment_offset_elements_out)
{
   uint64_t const va_aligned = va >> tile_pipe_interleave_bytes_log2
                                        << tile_pipe_interleave_bytes_log2;
   descriptor->base = (uint32_t)(va_aligned >> 8);

   /* The pitch field is ignored by the hardware for buffers and can't store large buffer sizes, but
    * DRM Radeon 2.50.0 validates the pitch alignment regardless of whether the color surface is a
    * buffer UAV. Provide the smallest valid pitch. SLICE_MAX is not needed, with the smallest
    * possible value, after the division by the pitch, DRM Radeon 2.50.0 will consider the whole
    * surface zero-size.
    */
   descriptor->pitch =
      S_028C64_PITCH_TILE_MAX(terakan_format_pitch_alignment_linear_surfels(
                                 bytes_per_element, tile_pipe_interleave_bytes_log2) /
                                 8 -
                              1);

   uint32_t const alignment_elements = (va - va_aligned) / bytes_per_element;
   *alignment_offset_elements_out = alignment_elements;

   assert(elements != 0);
   descriptor->dim = (uint32_t)(alignment_elements + elements - 1);
}

bool
terakan_descriptor_create_for_uniform_buffer(struct terakan_bo const * const bo, uint64_t const va,
                                             VkDeviceSize const range, uint32_t resource_out[8])
{
   assert((va & (TERAKAN_KCACHE_HW_LINE_BYTES - 1)) == 0);
   assert(range != VK_WHOLE_SIZE);
   if (bo == NULL || range == 0) {
      return false;
   }

   /* Align to the kcache line size for consistent out-of-bounds behavior between the fetch and the
    * kcache.
    */
   VkDeviceSize const range_aligned = ALIGN_POT(range, (VkDeviceSize)TERAKAN_KCACHE_HW_LINE_BYTES);

   resource_out[0] = (uint32_t)va;
   resource_out[1] = (uint32_t)(range_aligned - 1);
   resource_out[2] = S_030008_BASE_ADDRESS_HI(va >> 32) | S_030008_STRIDE(1);
   resource_out[3] =
      S_03000C_DST_SEL_X(TERASCALE_SWIZZLE_X) | S_03000C_DST_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_03000C_DST_SEL_Z(TERASCALE_SWIZZLE_Z) | S_03000C_DST_SEL_W(TERASCALE_SWIZZLE_W);
   resource_out[4] = (uint32_t)(range_aligned / (sizeof(uint32_t) * 4));
   resource_out[7] = S_03001C_TYPE(V_03001C_SQ_TEX_VTX_VALID_BUFFER);
   resource_out[TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD] = TERAKAN_BO_PRIORITY_UNIFORM_BUFFER;

   return true;
}

bool
terakan_descriptor_create_for_storage_buffer(struct terakan_bo const * const bo, uint64_t const va,
                                             VkDeviceSize const range,
                                             unsigned const tile_pipe_interleave_bytes_log2,
                                             uint32_t resource_out[8],
                                             struct terakan_color_descriptor * const color_out)
{
   assert((va & (sizeof(uint32_t) - 1)) == 0);
   assert(range != VK_WHOLE_SIZE);
   if (bo == NULL || range == 0) {
      return false;
   }

   /* VK_EXT_robustness2 behavior: rounding up. */
   VkDeviceSize const range_aligned = ALIGN_POT(range, (VkDeviceSize)sizeof(uint32_t));

   resource_out[0] = (uint32_t)va;
   resource_out[1] = (uint32_t)(range_aligned - 1);
   resource_out[2] = S_030008_BASE_ADDRESS_HI(va >> 32) | S_030008_STRIDE(sizeof(uint32_t)) |
                     S_030008_DATA_FORMAT(TERASCALE_FORMAT_INDEX_32) |
                     S_030008_NUM_FORMAT_ALL(TERASCALE_FORMAT_SQ_NUM_FORMAT_INT) |
                     S_030008_ENDIAN_SWAP(UTIL_ARCH_BIG_ENDIAN ? TERASCALE_ENDIAN_SWAP_8IN32 : 0);
   /* XYZW DST_SEL to permit vertex fetches larger than TERASCALE_FORMAT_INDEX_32 with a different
    * format in the fetch instruction.
    */
   resource_out[3] =
      S_03000C_DST_SEL_X(TERASCALE_SWIZZLE_X) | S_03000C_DST_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_03000C_DST_SEL_Z(TERASCALE_SWIZZLE_Z) | S_03000C_DST_SEL_W(TERASCALE_SWIZZLE_W);
   resource_out[4] = (uint32_t)(range_aligned / sizeof(uint32_t));
   resource_out[7] = S_03001C_TYPE(V_03001C_SQ_TEX_VTX_VALID_BUFFER);
   resource_out[TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD] = TERAKAN_BO_PRIORITY_SHADER_READ_BUFFER;

   terakan_color_descriptor_calculate_buffer_base_pitch_view_dim(
      color_out, va, range_aligned / sizeof(uint32_t), sizeof(uint32_t),
      tile_pipe_interleave_bytes_log2);
   color_out->info = S_028C70_FORMAT(TERASCALE_FORMAT_INDEX_32) |
                     S_028C70_ARRAY_MODE(V_028C70_ARRAY_LINEAR_ALIGNED) |
                     S_028C70_NUMBER_TYPE(TERASCALE_FORMAT_NUMBER_TYPE_UINT) |
                     S_028C70_COMP_SWAP(TERASCALE_FORMAT_CB_COLOR_SWAP_STD) |
                     S_028C70_BLEND_BYPASS(1) | S_028C70_SOURCE_FORMAT(V_028C70_EXPORT_4C_32BPC) |
                     S_028C70_RAT(1) | S_028C70_RESOURCE_TYPE(V_028C70_BUFFER);
   color_out->attrib = S_028C74_NON_DISP_TILING_ORDER(1);

   return true;
}
