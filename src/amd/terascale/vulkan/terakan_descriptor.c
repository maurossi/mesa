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

#include "terakan_descriptor.h"

#include "terakan_format.h"

#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600_formats.h"
#include "util/u_endian.h"
#include "util/u_math.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void
terakan_color_descriptor_calculate_buffer_base_pitch_view_dim(
   struct terakan_color_descriptor * const descriptor, VkDeviceSize const bo_address,
   VkDeviceSize const elements, unsigned const bpe, unsigned const tile_pipe_interleave_bytes_log2)
{
   VkDeviceSize const bo_address_aligned = bo_address >> tile_pipe_interleave_bytes_log2
                                                            << tile_pipe_interleave_bytes_log2;
   descriptor->base = (uint32_t)(bo_address_aligned >> 8);

   /* The pitch field is ignored by the hardware for buffers and can't store large buffer sizes, but
    * Linux Radeon 2.50.0 validates the pitch alignment regardless of whether the color surface is a
    * buffer RAT. Provide the smallest valid pitch. SLICE_MAX is not needed, with the smallest
    * possible value, after the division by the pitch, Linux Radeon 2.50.0 will consider the whole
    * surface zero-size.
    */
   descriptor->pitch = S_028C64_PITCH_TILE_MAX(
      terakan_format_pitch_alignment_linear_bytes(bpe, tile_pipe_interleave_bytes_log2) / bpe / 8 -
      1);

   VkDeviceSize const alignment_elements = (bo_address - bo_address_aligned) / bpe;
   /* Used by the driver, must be zeroed before being passed to the hardware. */
   descriptor->view = S_028C6C_SLICE_START(alignment_elements);

   assert(elements != 0);
   descriptor->dim = (uint32_t)(alignment_elements + elements - 1);
}

bool
terakan_descriptor_create_for_uniform_buffer(struct terakan_bo const * const bo,
                                             VkDeviceSize const bo_offset, VkDeviceSize const range,
                                             uint32_t resource_out[8])
{
   assert((bo_offset & (TERAKAN_CONSTANT_CACHE_LINE_BYTES - 1)) == 0);
   assert(range != VK_WHOLE_SIZE);
   if (bo == NULL || range == 0) {
      return false;
   }

   /* Align to the constant cache line size for consistent out-of-bounds behavior between the fetch
    * and the constant cache.
    */
   VkDeviceSize const range_aligned =
      ALIGN_POT(range, (VkDeviceSize)TERAKAN_CONSTANT_CACHE_LINE_BYTES);

   resource_out[0] = (uint32_t)bo_offset;
   resource_out[1] = (uint32_t)(range_aligned - 1);
   resource_out[2] = S_030008_BASE_ADDRESS_HI(bo_offset >> 32) |
                     S_030008_STRIDE(sizeof(uint32_t) * 4) | S_030008_DATA_FORMAT(FMT_32_32_32_32) |
                     S_030008_NUM_FORMAT_ALL(V_030008_SQ_NUM_FORMAT_INT) |
                     S_030008_ENDIAN_SWAP(UTIL_ARCH_BIG_ENDIAN ? ENDIAN_8IN32 : 0);
   resource_out[3] = S_03000C_DST_SEL_X(V_03000C_SQ_SEL_X) | S_03000C_DST_SEL_Y(V_03000C_SQ_SEL_Y) |
                     S_03000C_DST_SEL_Z(V_03000C_SQ_SEL_Z) | S_03000C_DST_SEL_W(V_03000C_SQ_SEL_W);
   resource_out[4] = (uint32_t)(range_aligned / (sizeof(uint32_t) * 4));
   resource_out[7] = S_03001C_TYPE(V_03001C_SQ_TEX_VTX_VALID_BUFFER);
   resource_out[TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD] = TERAKAN_BO_PRIORITY_UNIFORM_BUFFER;

   return true;
}

bool
terakan_descriptor_create_for_storage_buffer(struct terakan_bo const * const bo,
                                             VkDeviceSize const bo_offset, VkDeviceSize const range,
                                             unsigned const tile_pipe_interleave_bytes_log2,
                                             uint32_t resource_out[8],
                                             struct terakan_color_descriptor * const color_out)
{
   assert((bo_offset & (sizeof(uint32_t) - 1)) == 0);
   assert(range != VK_WHOLE_SIZE);
   if (bo == NULL || range == 0) {
      return false;
   }

   /* VK_EXT_robustness2 behavior: rounding up. */
   VkDeviceSize const range_aligned = ALIGN_POT(range, (VkDeviceSize)sizeof(uint32_t));

   resource_out[0] = (uint32_t)bo_offset;
   resource_out[1] = (uint32_t)(range_aligned - 1);
   resource_out[2] = S_030008_BASE_ADDRESS_HI(bo_offset >> 32) | S_030008_STRIDE(sizeof(uint32_t)) |
                     S_030008_DATA_FORMAT(FMT_32) |
                     S_030008_NUM_FORMAT_ALL(V_030008_SQ_NUM_FORMAT_INT) |
                     S_030008_ENDIAN_SWAP(UTIL_ARCH_BIG_ENDIAN ? ENDIAN_8IN32 : 0);
   /* XYZW DST_SEL to permit vertex fetches larger than FMT_32 with a different format in the fetch
    * instruction.
    */
   resource_out[3] = S_03000C_DST_SEL_X(V_03000C_SQ_SEL_X) | S_03000C_DST_SEL_Y(V_03000C_SQ_SEL_Y) |
                     S_03000C_DST_SEL_Z(V_03000C_SQ_SEL_Z) | S_03000C_DST_SEL_W(V_03000C_SQ_SEL_W);
   resource_out[4] = (uint32_t)(range_aligned / sizeof(uint32_t));
   resource_out[7] = S_03001C_TYPE(V_03001C_SQ_TEX_VTX_VALID_BUFFER);
   resource_out[TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD] = TERAKAN_BO_PRIORITY_SHADER_READ_BUFFER;

   terakan_color_descriptor_calculate_buffer_base_pitch_view_dim(
      color_out, bo_offset, range_aligned / sizeof(uint32_t), sizeof(uint32_t),
      tile_pipe_interleave_bytes_log2);
   color_out->info =
      S_028C70_FORMAT(V_028C70_COLOR_32) | S_028C70_ARRAY_MODE(V_028C70_ARRAY_LINEAR_ALIGNED) |
      S_028C70_NUMBER_TYPE(V_028C70_NUMBER_UINT) | S_028C70_COMP_SWAP(V_028C70_SWAP_STD) |
      S_028C70_BLEND_BYPASS(1) | S_028C70_SOURCE_FORMAT(V_028C70_EXPORT_4C_32BPC) |
      S_028C70_RAT(1) | S_028C70_RESOURCE_TYPE(V_028C70_BUFFER);
   color_out->attrib = S_028C74_NON_DISP_TILING_ORDER(1);

   return true;
}
