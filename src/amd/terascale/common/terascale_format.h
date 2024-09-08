/*
 * Copyright © 2024 Vitaliy Triang3l Kuzmin
 *
 * Based in part on the address library for AMD drivers (AddrLib) which is:
 * Copyright (c) 2007-2024 Advanced Micro Devices, Inc. All Rights Reserved.
 * https://github.com/GPUOpen-Drivers/pal/tree/dc99f22e2999cbefb5d46bec9a8beb9a9b6fa5e8/src/core/imported/addrlib
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

#ifndef TERASCALE_FORMAT_H
#define TERASCALE_FORMAT_H

#include "util/format/u_format.h"
#include "util/macros.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum terascale_swizzle {
   TERASCALE_SWIZZLE_X = 0,
   TERASCALE_SWIZZLE_Y = 1,
   TERASCALE_SWIZZLE_Z = 2,
   TERASCALE_SWIZZLE_W = 3,
   TERASCALE_SWIZZLE_0 = 4,
   TERASCALE_SWIZZLE_1 = 5,
   /* Can be used in some places to mask out the component when writing. */
   TERASCALE_SWIZZLE_MASK = 7,
};

enum terascale_endian_swap {
   TERASCALE_ENDIAN_SWAP_NONE = 0,
   TERASCALE_ENDIAN_SWAP_8IN16 = 1,
   TERASCALE_ENDIAN_SWAP_8IN32 = 2,
   TERASCALE_ENDIAN_SWAP_8IN64 = 3,
};

enum terascale_format_index {
   TERASCALE_FORMAT_INDEX_INVALID = 0,
   TERASCALE_FORMAT_INDEX_8 = 1,
   TERASCALE_FORMAT_INDEX_4_4 = 2,
   TERASCALE_FORMAT_INDEX_3_3_2 = 3,
   TERASCALE_FORMAT_INDEX_16 = 5,
   TERASCALE_FORMAT_INDEX_16_FLOAT = 6,
   TERASCALE_FORMAT_INDEX_8_8 = 7,
   TERASCALE_FORMAT_INDEX_5_6_5 = 8,
   TERASCALE_FORMAT_INDEX_6_5_5 = 9,
   TERASCALE_FORMAT_INDEX_1_5_5_5 = 10,
   TERASCALE_FORMAT_INDEX_4_4_4_4 = 11,
   TERASCALE_FORMAT_INDEX_5_5_5_1 = 12,
   TERASCALE_FORMAT_INDEX_32 = 13,
   TERASCALE_FORMAT_INDEX_32_FLOAT = 14,
   TERASCALE_FORMAT_INDEX_16_16 = 15,
   TERASCALE_FORMAT_INDEX_16_16_FLOAT = 16,
   TERASCALE_FORMAT_INDEX_8_24 = 17,
   TERASCALE_FORMAT_INDEX_8_24_FLOAT = 18,
   TERASCALE_FORMAT_INDEX_24_8 = 19,
   TERASCALE_FORMAT_INDEX_24_8_FLOAT = 20,
   TERASCALE_FORMAT_INDEX_10_11_11 = 21,
   TERASCALE_FORMAT_INDEX_10_11_11_FLOAT = 22,
   TERASCALE_FORMAT_INDEX_11_11_10 = 23,
   TERASCALE_FORMAT_INDEX_11_11_10_FLOAT = 24,
   TERASCALE_FORMAT_INDEX_2_10_10_10 = 25,
   TERASCALE_FORMAT_INDEX_8_8_8_8 = 26,
   TERASCALE_FORMAT_INDEX_10_10_10_2 = 27,
   TERASCALE_FORMAT_INDEX_X24_8_32_FLOAT = 28,
   TERASCALE_FORMAT_INDEX_32_32 = 29,
   TERASCALE_FORMAT_INDEX_32_32_FLOAT = 30,
   TERASCALE_FORMAT_INDEX_16_16_16_16 = 31,
   TERASCALE_FORMAT_INDEX_16_16_16_16_FLOAT = 32,
   TERASCALE_FORMAT_INDEX_32_32_32_32 = 34,
   TERASCALE_FORMAT_INDEX_32_32_32_32_FLOAT = 35,
   TERASCALE_FORMAT_INDEX_1 = 37,
   TERASCALE_FORMAT_INDEX_1_REVERSED = 38,
   TERASCALE_FORMAT_INDEX_GB_GR = 39,
   TERASCALE_FORMAT_INDEX_BG_RG = 40,
   TERASCALE_FORMAT_INDEX_32_AS_8 = 41,
   TERASCALE_FORMAT_INDEX_32_AS_8_8 = 42,
   TERASCALE_FORMAT_INDEX_5_9_9_9_SHAREDEXP = 43,
   TERASCALE_FORMAT_INDEX_8_8_8 = 44,
   TERASCALE_FORMAT_INDEX_16_16_16 = 45,
   TERASCALE_FORMAT_INDEX_16_16_16_FLOAT = 46,
   TERASCALE_FORMAT_INDEX_32_32_32 = 47,
   TERASCALE_FORMAT_INDEX_32_32_32_FLOAT = 48,
   TERASCALE_FORMAT_INDEX_BC1 = 49,
   TERASCALE_FORMAT_INDEX_BC2 = 50,
   TERASCALE_FORMAT_INDEX_BC3 = 51,
   TERASCALE_FORMAT_INDEX_BC4 = 52,
   TERASCALE_FORMAT_INDEX_BC5 = 53,
   TERASCALE_FORMAT_INDEX_APC0 = 54,              /* R6xx */
   TERASCALE_FORMAT_INDEX_BC6 = 54,               /* R8xx */
   TERASCALE_FORMAT_INDEX_APC1 = 55,              /* R6xx */
   TERASCALE_FORMAT_INDEX_BC7 = 55,               /* R8xx */
   TERASCALE_FORMAT_INDEX_APC2 = 56,              /* R6xx */
   TERASCALE_FORMAT_INDEX_32_AS_32_32_32_32 = 56, /* R8xx */
   TERASCALE_FORMAT_INDEX_APC3 = 57,
   TERASCALE_FORMAT_INDEX_APC4 = 58,
   TERASCALE_FORMAT_INDEX_APC5 = 59,
   TERASCALE_FORMAT_INDEX_APC6 = 60,
   TERASCALE_FORMAT_INDEX_APC7 = 61,
   TERASCALE_FORMAT_INDEX_CTX1 = 62,
};

#define TERASCALE_FORMAT_BIT(format) BITFIELD64_BIT(TERASCALE_FORMAT_INDEX_##format)

#define TERASCALE_FORMATS_8X1_BLOCK (TERASCALE_FORMAT_BIT(1) | TERASCALE_FORMAT_BIT(1_REVERSED))
#define TERASCALE_FORMATS_2X1_BLOCK (TERASCALE_FORMAT_BIT(GB_GR) | TERASCALE_FORMAT_BIT(BG_RG))
#define TERASCALE_FORMATS_EXPAND_3X                                                                \
   (TERASCALE_FORMAT_BIT(8_8_8) | TERASCALE_FORMAT_BIT(16_16_16) |                                 \
    TERASCALE_FORMAT_BIT(16_16_16_FLOAT) | TERASCALE_FORMAT_BIT(32_32_32) |                        \
    TERASCALE_FORMAT_BIT(32_32_32_FLOAT))
#define TERASCALE_FORMATS_4X4_BLOCK_R6XX                                                           \
   (TERASCALE_FORMAT_BIT(BC1) | TERASCALE_FORMAT_BIT(BC2) | TERASCALE_FORMAT_BIT(BC3) |            \
    TERASCALE_FORMAT_BIT(BC4) | TERASCALE_FORMAT_BIT(BC5) | TERASCALE_FORMAT_BIT(CTX1))
#define TERASCALE_FORMATS_4X4_BLOCK_R8XX                                                           \
   (TERASCALE_FORMAT_BIT(BC1) | TERASCALE_FORMAT_BIT(BC2) | TERASCALE_FORMAT_BIT(BC3) |            \
    TERASCALE_FORMAT_BIT(BC4) | TERASCALE_FORMAT_BIT(BC5) | TERASCALE_FORMAT_BIT(BC6) |            \
    TERASCALE_FORMAT_BIT(BC7) | TERASCALE_FORMAT_BIT(CTX1))
#define TERASCALE_FORMATS_BLOCK_R6XX                                                               \
   (TERASCALE_FORMATS_8X1_BLOCK | TERASCALE_FORMATS_2X1_BLOCK | TERASCALE_FORMATS_4X4_BLOCK_R6XX)
#define TERASCALE_FORMATS_BLOCK_R8XX                                                               \
   (TERASCALE_FORMATS_8X1_BLOCK | TERASCALE_FORMATS_2X1_BLOCK | TERASCALE_FORMATS_4X4_BLOCK_R8XX)

/* According to the R800 AddrLib:
 * "Special format such as FMT_1 and FMT_32_32_32 can be linear only".
 *
 * According to the Gallium R600 and RadeonSI drivers, tiling doesn't work with the subsampled
 * formats.
 */
#define TERASCALE_FORMATS_LINEAR_ONLY                                                              \
   (TERASCALE_FORMATS_8X1_BLOCK | TERASCALE_FORMATS_2X1_BLOCK | TERASCALE_FORMATS_EXPAND_3X)

/* According to the R800 AddrLib and to the Gallium R600 driver, compressed images must always be
 * tiled.
 */
#define TERASCALE_FORMATS_TILED_ONLY_R6XX TERASCALE_FORMATS_4X4_BLOCK_R6XX
#define TERASCALE_FORMATS_TILED_ONLY_R8XX TERASCALE_FORMATS_4X4_BLOCK_R8XX

/* APCn formats are skipped in the arrays for simplicity of handling their index reuse on R8xx. */
extern const uint8_t terascale_format_block_texels_log2[1 << 6][2];
/* * For 3x-expanded formats, the bytes per block array includes all three channels. */
extern const uint8_t terascale_format_bytes_per_block[1 << 6];
/* Endian swaps for big-endian hosts, up to 8IN32. For array formats, channels aren't reordered. */
extern const uint8_t terascale_format_big_endian_swap[1 << 6];
/* 32_AS formats are skipped in the channel count array for simplicity. */
extern const uint8_t terascale_format_channel_count[1 << 6];

enum terascale_format_number_type {
   TERASCALE_FORMAT_NUMBER_TYPE_UNORM = 0,
   TERASCALE_FORMAT_NUMBER_TYPE_SNORM = 1,
   TERASCALE_FORMAT_NUMBER_TYPE_USCALED = 2,
   TERASCALE_FORMAT_NUMBER_TYPE_SSCALED = 3,
   TERASCALE_FORMAT_NUMBER_TYPE_UINT = 4,
   TERASCALE_FORMAT_NUMBER_TYPE_SINT = 5,
   TERASCALE_FORMAT_NUMBER_TYPE_SRGB = 6,
   /* FLOAT signedness depends on the data format and FORMAT_COMP. */
   TERASCALE_FORMAT_NUMBER_TYPE_FLOAT = 7,
};

enum terascale_format_sq_num_format {
   TERASCALE_FORMAT_SQ_NUM_FORMAT_NORM = 0,
   TERASCALE_FORMAT_SQ_NUM_FORMAT_INT = 1,
   TERASCALE_FORMAT_SQ_NUM_FORMAT_SCALED = 2,
};

static inline enum terascale_format_sq_num_format
terascale_format_get_sq_num_format(const enum terascale_format_number_type number_type)
{
   switch (number_type) {
   case TERASCALE_FORMAT_NUMBER_TYPE_USCALED:
   case TERASCALE_FORMAT_NUMBER_TYPE_SSCALED:
      return TERASCALE_FORMAT_SQ_NUM_FORMAT_SCALED;
   case TERASCALE_FORMAT_NUMBER_TYPE_UINT:
   case TERASCALE_FORMAT_NUMBER_TYPE_SINT:
      return TERASCALE_FORMAT_SQ_NUM_FORMAT_INT;
   default:
      /* FLOAT is selected via DATA_FORMAT instead. */
      return TERASCALE_FORMAT_SQ_NUM_FORMAT_NORM;
   }
}

/* If FORMAT is among these formats, and NUMBER_TYPE is UNORM, SNORM or SRGB:
 * - On R6xx: BLEND_FLOAT32 can be disabled, SOURCE_FORMAT can be NORM.
 * - On R8xx: SOURCE_FORMAT can be 4C_16BPC.
 */
#define TERASCALE_FORMATS_CB_COLOR_EXPORT_16BPC_NORM                                               \
   (TERASCALE_FORMAT_BIT(8) | TERASCALE_FORMAT_BIT(4_4) | TERASCALE_FORMAT_BIT(3_3_2) |            \
    TERASCALE_FORMAT_BIT(8_8) | TERASCALE_FORMAT_BIT(5_6_5) | TERASCALE_FORMAT_BIT(6_5_5) |        \
    TERASCALE_FORMAT_BIT(1_5_5_5) | TERASCALE_FORMAT_BIT(4_4_4_4) |                                \
    TERASCALE_FORMAT_BIT(5_5_5_1) | TERASCALE_FORMAT_BIT(10_11_11) |                               \
    TERASCALE_FORMAT_BIT(11_11_10) | TERASCALE_FORMAT_BIT(2_10_10_10) |                            \
    TERASCALE_FORMAT_BIT(8_8_8_8) | TERASCALE_FORMAT_BIT(10_10_10_2))
/* If FORMAT is among these formats, and NUMBER_TYPE is FLOAT:
 * - On R6xx: BLEND_FLOAT32 can be disabled.
 * - On R8xx: SOURCE_FORMAT can be 4C_16BPC.
 */
#define TERASCALE_FORMATS_CB_COLOR_EXPORT_16BPC_FLOAT                                              \
   (TERASCALE_FORMAT_BIT(16) | TERASCALE_FORMAT_BIT(16_FLOAT) | TERASCALE_FORMAT_BIT(16_16) |      \
    TERASCALE_FORMAT_BIT(16_16_FLOAT) | TERASCALE_FORMAT_BIT(10_11_11) |                           \
    TERASCALE_FORMAT_BIT(10_11_11_FLOAT) | TERASCALE_FORMAT_BIT(11_11_10) |                        \
    TERASCALE_FORMAT_BIT(11_11_10_FLOAT) | TERASCALE_FORMAT_BIT(16_16_16_16) |                     \
    TERASCALE_FORMAT_BIT(16_16_16_16_FLOAT))

static inline bool
terascale_format_blend_bypass(const enum terascale_format_number_type number_type,
                              const enum terascale_format_index format)
{
   if (number_type == TERASCALE_FORMAT_NUMBER_TYPE_UINT ||
       number_type == TERASCALE_FORMAT_NUMBER_TYPE_SINT) {
      return true;
   }
   /* The stencil channel is always UINT. */
   switch (format) {
   case TERASCALE_FORMAT_INDEX_8_24:
   case TERASCALE_FORMAT_INDEX_8_24_FLOAT:
   case TERASCALE_FORMAT_INDEX_24_8:
   case TERASCALE_FORMAT_INDEX_24_8_FLOAT:
   case TERASCALE_FORMAT_INDEX_X24_8_32_FLOAT:
      return true;
   default:
      break;
   }
   return false;
}

enum terascale_format_cb_color_swap {
   /* Listing shader export RGBA > data XYZW. */

   /* 1 channel:  r > x | ----- | ----- | -----  (x < r)
    * 2 channels: r > x | g > y | ----- | -----  (x < r | y < g)
    * 3 channels: r > x | g > y | b > z | -----  (x < r | y < g | z < b)
    * 4 channels: r > x | g > y | b > z | a > w  (x < r | y < g | z < b | w < a)
    */
   TERASCALE_FORMAT_CB_COLOR_SWAP_STD = 0,

   /* 1 channel:  ----- | g > x | ----- | -----  (x < g)
    * 2 channels: r > x | ----- | ----- | a > y  (x < r | y < a)
    * 3 channels: r > x | g > y | ----- | a > z  (x < r | y < g | z < a)
    * 4 channels: r > z | g > y | b > x | a > w  (x < b | y < g | z < r | w < a)
    */
   TERASCALE_FORMAT_CB_COLOR_SWAP_ALT = 1,

   /* 1 channel:  ----- | ----- | b > x | -----  (x < b)
    * 2 channels: r > y | g > x | ----- | -----  (x < g | y < r)
    * 3 channels: r > z | g > y | b > x | -----  (x < b | y < g | z < r)
    * 4 channels: r > w | g > z | b > y | a > x  (x < a | y < b | z < g | w < r)
    */
   TERASCALE_FORMAT_CB_COLOR_SWAP_STD_REV = 2,

   /* 1 channel:  ----- | ----- | ----- | a > x  (x < a)
    * 2 channels: r > y | ----- | ----- | a > x  (x < a | y < r)
    * 3 channels: r > z | g > y | ----- | a > x  (x < a | y < g | z < r)
    * 4 channels: r > y | g > z | b > w | a > x  (x < a | y < r | z < g | w < b)
    */
   TERASCALE_FORMAT_CB_COLOR_SWAP_ALT_REV = 3,
};

/* [Format channel count][swap]. */
extern const uint8_t terascale_format_cb_color_export_component_masks[4 + 1][4];

struct terascale_format_info {
   uint32_t format : 6;

   /* NUMBER_TYPE for CB_COLOR, and for deriving NUM_FORMAT_ALL and FORCE_DEGAMMA for SQ fetch
    * (note that SQ fetch signedness should be obtained from `channels_signed` instead).
    */
   uint32_t number_type : 3;

   /* FORMAT_COMP for SQ texture fetch, bit 0 for X, bit 3 for W.
    *
    * For SQ vertex fetch, FORMAT_COMP_ALL can be set to 1 if any component is signed
    * (mixed signedness formats have `supports_sq_vertex_fetch` set to 0).
    *
    * For floating-point formats, this is nonzero only if it makes a difference (such as for BC6).
    *
    * Void and nonexistent channels have the signedness of the first non-void channel copied for
    * simplicity and consistency. The single-bit channel of 1_5_5_5 and 5_5_5_1 is always unsigned,
    * however.
    */
   uint32_t channels_signed : 4;

   /* XYZW01.
    * May be different from the swizzle of the source pipe_format (especially because endianness is
    * handled via the endian swap field of the registers, always assuming that the data format
    * itself is little-endian since that's the case in the hardware, rather than using channel
    * reordering).
    */
   uint32_t swizzle_r : 3;
   uint32_t swizzle_g : 3;
   uint32_t swizzle_b : 3;
   uint32_t swizzle_a : 3;
   uint32_t cb_color_swap : 2;

   /* Whether the combination of the other fields is supported in each scenario. */
   uint32_t supports_sq_vertex_fetch : 1;
   uint32_t supports_sq_texture_fetch : 1;
   /* Formats with existing but unused alpha bits are still reported as supporting CB usage if other
    * conditions are met, and if the client API exposes them, it must handle this case:
    * if `swizzle_a` is PIPE_SWIZZLE_1, and `terascale_format_channel_count[format]` is 4,
    * FORCE_DST_ALPHA_1 must be enabled in CB_COLOR#_ATTRIB on R9xx, or CB_BLEND#_CONTROL must have
    * DST_ALPHA/COLOR and ONE_MINUS_DST_ALPHA/COLOR in the alpha equation replaced with ONE and ZERO
    * respectively.
    */
   uint32_t supports_cb_color : 1;
};

/* For combined depth / stencil formats, index with the combined format. */
extern const struct terascale_format_info terascale_format_info_r6xx[PIPE_FORMAT_COUNT];
/* For combined depth / stencil formats, index with the format of the specific aspect. */
extern const struct terascale_format_info terascale_format_info_r8xx[PIPE_FORMAT_COUNT];

static inline bool
terascale_format_supports_uav_atomic_int(const struct terascale_format_info *const format_info)
{
   /* SQ vertex fetch is required to read from the CB_IMMED buffer. */
   return format_info->supports_cb_color && format_info->supports_sq_vertex_fetch &&
          format_info->format == TERASCALE_FORMAT_INDEX_32 &&
          (format_info->number_type == TERASCALE_FORMAT_NUMBER_TYPE_UINT ||
           format_info->number_type == TERASCALE_FORMAT_NUMBER_TYPE_SINT) &&
          format_info->cb_color_swap == TERASCALE_FORMAT_CB_COLOR_SWAP_STD;
}

enum terascale_r6xx_depth_stencil_format {
   TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_INVALID = 0,
   TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_16 = 1,
   TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_X8_24 = 2,
   TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_8_24 = 3,
   TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_X8_24_FLOAT = 4,
   TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_8_24_FLOAT = 5,
   TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_32_FLOAT = 6,
   TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_X24_8_32_FLOAT = 7,
};

static inline bool
terascale_r6xx_depth_stencil_format_has_stencil(
   const enum terascale_r6xx_depth_stencil_format format)
{
   return format == TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_8_24 ||
          format == TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_8_24_FLOAT ||
          format == TERASCALE_R6XX_DEPTH_STENCIL_FORMAT_X24_8_32_FLOAT;
}

enum terascale_r6xx_depth_stencil_format
terascale_get_r6xx_depth_stencil_format(enum pipe_format format);

enum terascale_r8xx_depth_format {
   TERASCALE_R8XX_DEPTH_FORMAT_INVALID = 0,
   TERASCALE_R8XX_DEPTH_FORMAT_16 = 1,
   TERASCALE_R8XX_DEPTH_FORMAT_24 = 2,
   TERASCALE_R8XX_DEPTH_FORMAT_32_FLOAT = 3,
};

/* Disregards void aspects in the pipe_format completely.
 * Output arguments are optional.
 * If the format is supported (for depth only, for stencil only, or for both), returns true.
 * Otherwise returns false and doesn't modify the output arguments.
 */
bool terascale_get_r8xx_depth_stencil_format(enum pipe_format format,
                                             enum terascale_r8xx_depth_format *depth_format_out_opt,
                                             bool *has_stencil_out_opt);

#ifdef __cplusplus
}
#endif

#endif /* TERASCALE_FORMAT_H */
