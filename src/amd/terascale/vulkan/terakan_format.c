/*
 * Copyright © 2023 Vitaliy Triang3l Kuzmin
 *
 * Based in part on r600_state_common.c and r600_texture.c which are:
 * Copyright 2010 Red Hat Inc.
 *           2010 Jerome Glisse <glisse@freedesktop.org>
 *
 * Based in part on radv_formats.c which is:
 * Copyright © 2016 Red Hat.
 * Copyright © 2016 Bas Nieuwenhuizen
 *
 * Based in parts of r800addrlib.cpp which is:
 * Copyright (c) 2007-2023 Advanced Micro Devices, Inc. All Rights Reserved.
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
#include "util/format/u_format.h"
#include "util/u_endian.h"
#include "vk_format.h"
#include "vk_util.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

/* TODO(Triang3l): Big-endian. */

static bool
terakan_format_is_unsupported(VkFormat const format)
{
   /* Mapped to 16_UNORM PIPE_FORMAT, but not supported. */
   if (format == VK_FORMAT_R10X6_UNORM_PACK16 || format == VK_FORMAT_R10X6G10X6_UNORM_2PACK16) {
      return true;
   }

   return false;
}

bool
terakan_format_is_linear_only(VkFormat const format)
{
   if (vk_format_is_depth_or_stencil(format)) {
      /* Depth or stencil images are always tiled.
       * PIPE_FORMAT_Z16_UNORM_S8_UINT is considered 24-bit though, not power-of-two, but it's
       * stored as separate 16 UNORM and 8 UINT surfaces on the hardware.
       */
      return false;
   }

   /* According to the R800 AddrLib:
    * "Special format such as FMT_1 and FMT_32_32_32 can be linear only".
    *
    * In Terakan specifically, copying to tiled images is done via a color texture target with the
    * same number of bits per element - and they're supported only for power-of-two formats.
    * Copying to linear images is done using a buffer random access target, to which the three
    * components can be written separately.
    */
   unsigned const block_size_bits = vk_format_get_blocksizebits(format);
   if (block_size_bits < 8 || !util_is_power_of_two_or_zero(block_size_bits)) {
      return true;
   }

   /* According to the Gallium R600 and RadeonSI drivers, tiling doesn't work with the subsampled
    * formats.
    */
   if (vk_format_description(format)->layout == UTIL_FORMAT_LAYOUT_SUBSAMPLED) {
      return true;
   }

   return false;
}

bool
terakan_format_is_tiled_only(VkFormat const format)
{
   /* Combined depth and stencil images are always tiled, libdrm_radeon forces tiling for
    * RADEON_ZBUFFER or RADEON_SBUFFER (which both must be specified so the stencil layout is
    * computed, so can't omit them if depth / stencil attachment usage is not needed) even when a
    * linear array mode is requested.
    * According to the Gallium R600 driver, compressed textures must always be tiled.
    */
   if ((vk_format_has_depth(format) && vk_format_has_stencil(format)) ||
       vk_format_is_block_compressed(format)) {
      return true;
   }

   return false;
}

static int
terakan_format_get_depth_or_first_non_void_channel(
   struct util_format_description const * const description)
{
   /* If the format has depth, getters are for the depth aspect - for channel swizzle[0]. */
   return util_format_has_depth(description)
             ? description->swizzle[0]
             : util_format_get_first_non_void_channel(description->format);
}

uint32_t
terakan_format_color_get_format(VkFormat const format)
{
   if (terakan_format_is_unsupported(format)) {
      return V_028C70_COLOR_INVALID;
   }

   /* Not a plain format.*/
   if (format == VK_FORMAT_B10G11R11_UFLOAT_PACK32) {
      return V_028C70_COLOR_10_11_11_FLOAT;
   }

   struct util_format_description const * const description = vk_format_description(format);

   if (description->format == PIPE_FORMAT_NONE || description->layout != UTIL_FORMAT_LAYOUT_PLAIN) {
      return V_028C70_COLOR_INVALID;
   }

   switch (description->nr_channels) {
   case 1:
      switch (description->channel[0].size) {
      case 8:
         return V_028C70_COLOR_8;
      case 16:
         if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
            return V_028C70_COLOR_16_FLOAT;
         }
         return V_028C70_COLOR_16;
      case 32:
         if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
            return V_028C70_COLOR_32_FLOAT;
         }
         return V_028C70_COLOR_32;
      }
      break;

   case 2:
      if (description->channel[0].size == description->channel[1].size) {
         switch (description->channel[0].size) {
         case 8:
            return V_028C70_COLOR_8_8;
         case 16:
            if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
               return V_028C70_COLOR_16_16_FLOAT;
            }
            return V_028C70_COLOR_16_16;
         case 32:
            if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
               return V_028C70_COLOR_32_32_FLOAT;
            }
            return V_028C70_COLOR_32_32;
         }
      } else if (description->channel[0].size == 16 && description->channel[1].size == 8) {
         /* Depth aspect of VK_FORMAT_D16_UNORM_S8_UINT. */
         return V_028C70_COLOR_16;
      } else if (description->channel[0].size == 8 && description->channel[1].size == 24) {
         return V_028C70_COLOR_24_8;
      } else if (description->channel[0].size == 24 && description->channel[1].size == 8) {
         return V_028C70_COLOR_8_24;
      }
      break;

   case 3:
      if (description->channel[0].size == 5 && description->channel[1].size == 6 &&
          description->channel[2].size == 5) {
         return V_028C70_COLOR_5_6_5;
      } else if (description->channel[0].size == 32 && description->channel[1].size == 8 &&
                 description->channel[2].size == 24) {
         /* Depth aspect, not X24_8_32_FLOAT. */
         return V_028C70_COLOR_32_FLOAT;
      }
      break;

   case 4:
      if (description->channel[0].size == description->channel[1].size &&
          description->channel[0].size == description->channel[2].size) {
         if (description->channel[0].size == description->channel[3].size) {
            switch (description->channel[0].size) {
            case 4:
               return V_028C70_COLOR_4_4_4_4;
            case 8:
               return V_028C70_COLOR_8_8_8_8;
            case 16:
               if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
                  return V_028C70_COLOR_16_16_16_16_FLOAT;
               }
               return V_028C70_COLOR_16_16_16_16;
            case 32:
               if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
                  return V_028C70_COLOR_32_32_32_32_FLOAT;
               }
               return V_028C70_COLOR_32_32_32_32;
            }
         } else if (description->channel[0].size == 5 && description->channel[3].size == 1) {
            return V_028C70_COLOR_1_5_5_5;
         } else if (description->channel[0].size == 10 && description->channel[3].size == 2) {
            return V_028C70_COLOR_2_10_10_10;
         }
      } else if (description->channel[1].size == description->channel[2].size &&
                 description->channel[1].size == description->channel[3].size) {
         if (description->channel[0].size == 1 && description->channel[1].size == 5) {
            return V_028C70_COLOR_5_5_5_1;
         } else if (description->channel[0].size == 2 && description->channel[1].size == 10) {
            return V_028C70_COLOR_10_10_10_2;
         }
      }
      break;
   }

   return V_028C70_COLOR_INVALID;
}

uint32_t
terakan_format_color_get_number_type(VkFormat const format)
{
   /* Not a plain format.*/
   if (format == VK_FORMAT_B10G11R11_UFLOAT_PACK32) {
      return V_028C70_NUMBER_FLOAT;
   }

   struct util_format_description const * const description = vk_format_description(format);

   if (description->layout != UTIL_FORMAT_LAYOUT_PLAIN) {
      return UINT32_MAX;
   }

   if (description->colorspace == UTIL_FORMAT_COLORSPACE_SRGB) {
      return V_028C70_NUMBER_SRGB;
   }

   int const channel_index = terakan_format_get_depth_or_first_non_void_channel(description);
   if (channel_index < 0) {
      return UINT32_MAX;
   }
   struct util_format_channel_description const * const channel =
      &description->channel[channel_index];

   switch (channel->type) {
   case UTIL_FORMAT_TYPE_UNSIGNED:
      if (channel->normalized) {
         return V_028C70_NUMBER_UNORM;
      } else if (channel->pure_integer) {
         return V_028C70_NUMBER_UINT;
      }
      break;

   case UTIL_FORMAT_TYPE_SIGNED:
      if (channel->normalized) {
         return V_028C70_NUMBER_SNORM;
      } else if (channel->pure_integer) {
         return V_028C70_NUMBER_SINT;
      }
      break;

   case UTIL_FORMAT_TYPE_FLOAT:
      return V_028C70_NUMBER_FLOAT;

   default:
      break;
   }

   return UINT32_MAX;
}

uint32_t
terakan_format_color_get_swap(VkFormat const format)
{
   /* Not a plain format.*/
   if (format == VK_FORMAT_B10G11R11_UFLOAT_PACK32) {
      return V_028C70_SWAP_STD;
   }

   struct util_format_description const * const description = vk_format_description(format);

   if (description->colorspace == UTIL_FORMAT_COLORSPACE_ZS) {
      /* In Vulkan, both depth and stencil are in X (which one is accessed depends on the view
       * aspect).
       */
      return V_028C70_SWAP_STD;
   }

   if (description->layout != UTIL_FORMAT_LAYOUT_PLAIN) {
      return UINT32_MAX;
   }

   switch (description->nr_channels) {
   case 1:
      if (description->swizzle[0] == PIPE_SWIZZLE_X) {
         /* X___ */
         return V_028C70_SWAP_STD;
      } else if (description->swizzle[3] == PIPE_SWIZZLE_X) {
         /* ___X */
         return V_028C70_SWAP_ALT_REV;
      }
      break;

   case 2:
      if ((description->swizzle[0] == PIPE_SWIZZLE_X &&
           (description->swizzle[1] == PIPE_SWIZZLE_Y ||
            description->swizzle[1] == PIPE_SWIZZLE_NONE)) ||
          (description->swizzle[0] == PIPE_SWIZZLE_NONE &&
           description->swizzle[1] == PIPE_SWIZZLE_Y)) {
         /* XY__ */
         return V_028C70_SWAP_STD;
      } else if ((description->swizzle[0] == PIPE_SWIZZLE_Y &&
                  (description->swizzle[1] == PIPE_SWIZZLE_X ||
                   description->swizzle[1] == PIPE_SWIZZLE_NONE)) ||
                 (description->swizzle[0] == PIPE_SWIZZLE_NONE &&
                  description->swizzle[1] == PIPE_SWIZZLE_X)) {
         /* YX__ */
         return V_028C70_SWAP_STD_REV;
      } else if (description->swizzle[0] == PIPE_SWIZZLE_X &&
                 description->swizzle[3] == PIPE_SWIZZLE_Y) {
         /* X__Y */
         return V_028C70_SWAP_ALT;
      } else if (description->swizzle[0] == PIPE_SWIZZLE_Y &&
                 description->swizzle[3] == PIPE_SWIZZLE_X) {
         /* Y__X */
         return V_028C70_SWAP_ALT_REV;
      }
      break;

   case 3:
      if (description->swizzle[0] == PIPE_SWIZZLE_X) {
         /* XYZ_ */
         return V_028C70_SWAP_STD;
      } else if (description->swizzle[0] == PIPE_SWIZZLE_Z) {
         /* ZYX_ */
         return V_028C70_SWAP_STD_REV;
      }
      break;

   case 4:
      /* Check the middle channels, the [0] and [3] channels can be NONE. */
      if (description->swizzle[1] == PIPE_SWIZZLE_Y) {
         if (description->swizzle[2] == PIPE_SWIZZLE_Z) {
            /* XYZW */
            return V_028C70_SWAP_STD;
         } else if (description->swizzle[2] == PIPE_SWIZZLE_X) {
            /* ZYXW */
            return V_028C70_SWAP_ALT;
         }
      } else if (description->swizzle[1] == PIPE_SWIZZLE_Z) {
         if (description->swizzle[2] == PIPE_SWIZZLE_Y) {
            /* WZYX */
            return V_028C70_SWAP_STD_REV;
         } else if (description->swizzle[2] == PIPE_SWIZZLE_W) {
            /* YZWX */
            return V_028C70_SWAP_ALT_REV;
         }
      }
      break;
   }

   return UINT32_MAX;
}

bool
terakan_format_color_is_blendable(uint32_t const color_format, uint32_t const number_type)
{
   return number_type != V_028C70_NUMBER_UINT && number_type != V_028C70_NUMBER_SINT &&
          color_format != V_028C70_COLOR_8_24 && color_format != V_028C70_COLOR_24_8 &&
          color_format != V_028C70_COLOR_X24_8_32_FLOAT;
}

uint32_t
terakan_format_depth_get_format(VkFormat const format)
{
   struct util_format_description const * const description = vk_format_description(format);

   if (!util_format_has_depth(description)) {
      return V_028040_Z_INVALID;
   }

   struct util_format_channel_description const * const depth_channel =
      &description->channel[description->swizzle[0]];
   if (depth_channel->type == UTIL_FORMAT_TYPE_UNSIGNED) {
      if (depth_channel->normalized) {
         switch (depth_channel->size) {
         case 16:
            return V_028040_Z_16;
         case 24:
            return V_028040_Z_24;
         }
      }
   } else if (depth_channel->type == UTIL_FORMAT_TYPE_FLOAT) {
      if (depth_channel->size == 32) {
         return V_028040_Z_32_FLOAT;
      }
   }

   return V_028040_Z_INVALID;
}

bool
terakan_format_has_stencil_8(VkFormat const format)
{
   struct util_format_description const * const description = vk_format_description(format);
   return util_format_has_stencil(description) &&
          description->channel[description->swizzle[1]].size == 8;
}

uint32_t
terakan_format_data_get_common_format(VkFormat const format)
{
   if (terakan_format_is_unsupported(format)) {
      return FMT_INVALID;
   }

   /* Not plain formats. */
   if (format == VK_FORMAT_B10G11R11_UFLOAT_PACK32) {
      return FMT_10_11_11_FLOAT;
   } else if (format == VK_FORMAT_E5B9G9R9_UFLOAT_PACK32) {
      return FMT_5_9_9_9_SHAREDEXP;
   }

   struct util_format_description const * const description = vk_format_description(format);

   if (description->format == PIPE_FORMAT_NONE || description->layout != UTIL_FORMAT_LAYOUT_PLAIN) {
      return FMT_INVALID;
   }

   switch (description->nr_channels) {
   case 1:
      switch (description->channel[0].size) {
      case 8:
         return FMT_8;
      case 16:
         if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
            return FMT_16_FLOAT;
         }
         return FMT_16;
      case 32:
         if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
            return FMT_32_FLOAT;
         }
         return FMT_32;
      }
      break;

   case 2:
      if (description->channel[0].size == description->channel[1].size) {
         switch (description->channel[0].size) {
         case 4:
            return FMT_4_4;
         case 8:
            return FMT_8_8;
         case 16:
            if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
               return FMT_16_16_FLOAT;
            }
            return FMT_16_16;
         case 32:
            if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
               return FMT_32_32_FLOAT;
            }
            return FMT_32_32;
         }
      } else if (description->channel[0].size == 16 && description->channel[1].size == 8) {
         /* Depth aspect of VK_FORMAT_D16_UNORM_S8_UINT. */
         return FMT_16;
      } else if (description->channel[0].size == 8 && description->channel[1].size == 24) {
         return FMT_24_8;
      } else if (description->channel[0].size == 24 && description->channel[1].size == 8) {
         return FMT_8_24;
      }
      break;

   case 3:
      if (description->channel[0].size == 5) {
         if (description->channel[1].size == 6 && description->channel[2].size == 5) {
            return FMT_5_6_5;
         } else if (description->channel[1].size == 5 && description->channel[2].size == 6) {
            return FMT_6_5_5;
         }
      } else if (description->channel[0].size == 32) {
         if (description->channel[1].size == 8 && description->channel[2].size == 24) {
            /* Depth aspect, not X24_8_32_FLOAT. */
            return FMT_32_FLOAT;
         } else if (description->channel[1].size == 32 && description->channel[2].size == 32) {
            if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
               return FMT_32_32_32_FLOAT;
            }
            return FMT_32_32_32;
         }
      }
      break;

   case 4:
      if (description->channel[0].size == description->channel[1].size &&
          description->channel[0].size == description->channel[2].size) {
         if (description->channel[0].size == description->channel[3].size) {
            switch (description->channel[0].size) {
            case 4:
               return FMT_4_4_4_4;
            case 8:
               return FMT_8_8_8_8;
            case 16:
               if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
                  return FMT_16_16_16_16_FLOAT;
               }
               return FMT_16_16_16_16;
            case 32:
               if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
                  return FMT_32_32_32_32_FLOAT;
               }
               return FMT_32_32_32_32;
            }
         } else if (description->channel[0].size == 5 && description->channel[3].size == 1) {
            return FMT_1_5_5_5;
         } else if (description->channel[0].size == 10 && description->channel[3].size == 2) {
            return FMT_2_10_10_10;
         }
      } else if (description->channel[1].size == description->channel[2].size &&
                 description->channel[1].size == description->channel[3].size) {
         if (description->channel[0].size == 1 && description->channel[1].size == 5) {
            return FMT_5_5_5_1;
         } else if (description->channel[0].size == 2 && description->channel[1].size == 10) {
            return FMT_10_10_10_2;
         }
      }
      break;
   }

   return FMT_INVALID;
}

uint32_t
terakan_format_data_get_number_format(VkFormat const format)
{
   struct util_format_description const * const description = vk_format_description(format);

   if (description->layout != UTIL_FORMAT_LAYOUT_PLAIN) {
      /* All supported non-plain formats are NORM. */
      return V_030010_SQ_NUM_FORMAT_NORM;
   }

   int const channel_index = terakan_format_get_depth_or_first_non_void_channel(description);
   if (channel_index < 0) {
      return UINT32_MAX;
   }
   struct util_format_channel_description const * const channel =
      &description->channel[channel_index];

   switch (channel->type) {
   case UTIL_FORMAT_TYPE_UNSIGNED:
   case UTIL_FORMAT_TYPE_SIGNED:
      if (channel->normalized) {
         return V_030010_SQ_NUM_FORMAT_NORM;
      } else if (channel->pure_integer) {
         return V_030010_SQ_NUM_FORMAT_INT;
      } else {
         return V_030010_SQ_NUM_FORMAT_SCALED;
      }
      break;

      /* Fixed-point formats are not supported. */

   case UTIL_FORMAT_TYPE_FLOAT:
      return V_030010_SQ_NUM_FORMAT_NORM;

   default:
      break;
   }

   return UINT32_MAX;
}

unsigned char const *
terakan_format_data_get_swizzle(VkFormat const format)
{
   if (vk_format_is_depth_or_stencil(format)) {
      /* In Vulkan, both depth and stencil are in X (which one is accessed depends on the view
       * aspect).
       */
      static unsigned char const depth_stencil_swizzle[] = {PIPE_SWIZZLE_X, PIPE_SWIZZLE_0,
                                                            PIPE_SWIZZLE_0, PIPE_SWIZZLE_1};
      return depth_stencil_swizzle;
   }
   return vk_format_description(format)->swizzle;
}

uint32_t
terakan_format_texture_get_format(VkFormat const format)
{
   uint32_t const common_format = terakan_format_data_get_common_format(format);
   if (common_format != FMT_INVALID) {
      return common_format;
   }

   /* Handle texture-only formats. */

   switch (format) {
   case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
   case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
   case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
   case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
      return FMT_BC1;

   case VK_FORMAT_BC2_UNORM_BLOCK:
   case VK_FORMAT_BC2_SRGB_BLOCK:
      return FMT_BC2;

   case VK_FORMAT_BC3_UNORM_BLOCK:
   case VK_FORMAT_BC3_SRGB_BLOCK:
      return FMT_BC3;

   case VK_FORMAT_BC4_UNORM_BLOCK:
   case VK_FORMAT_BC4_SNORM_BLOCK:
      return FMT_BC4;

   case VK_FORMAT_BC5_UNORM_BLOCK:
   case VK_FORMAT_BC5_SNORM_BLOCK:
      return FMT_BC5;

   case VK_FORMAT_BC6H_UFLOAT_BLOCK:
   case VK_FORMAT_BC6H_SFLOAT_BLOCK:
      return FMT_BC6;

   case VK_FORMAT_BC7_UNORM_BLOCK:
   case VK_FORMAT_BC7_SRGB_BLOCK:
      return FMT_BC7;

   case VK_FORMAT_G8B8G8R8_422_UNORM:
      return FMT_BG_RG;

   case VK_FORMAT_B8G8R8G8_422_UNORM:
      return FMT_GB_GR;

   default:
      break;
   }

   return FMT_INVALID;
}

uint32_t
terakan_format_texture_get_word4_signs(VkFormat const format)
{
   /* Not plain formats. */
   switch (format) {
   case VK_FORMAT_BC4_SNORM_BLOCK:
      return V_030010_SQ_FORMAT_COMP_SIGNED;
   case VK_FORMAT_BC5_SNORM_BLOCK:
      return V_030010_SQ_FORMAT_COMP_SIGNED * 0b0101;
   case VK_FORMAT_BC6H_SFLOAT_BLOCK:
      return V_030010_SQ_FORMAT_COMP_SIGNED * 0b010101;
   default:
      break;
   }

   struct util_format_description const * const description = vk_format_description(format);

   if (description->layout != UTIL_FORMAT_LAYOUT_PLAIN) {
      /* All supported non-plain texture formats except for the ones already handled are unsigned or
       * float.
       */
      return 0;
   }

   uint32_t word4_signs = 0;
   for (size_t channel_index = 0; channel_index < 4; ++channel_index) {
      if (description->channel[channel_index].type == UTIL_FORMAT_TYPE_SIGNED) {
         word4_signs |= V_030010_SQ_FORMAT_COMP_SIGNED << (2 * channel_index);
      }
   }

   return word4_signs;
}

uint32_t
terakan_format_vertex_get_format(VkFormat const format)
{
   uint32_t const common_format = terakan_format_data_get_common_format(format);
   if (common_format != FMT_INVALID) {
      return common_format;
   }

   /* Handle vertex-only formats. */

   if (terakan_format_is_unsupported(format)) {
      return FMT_INVALID;
   }

   struct util_format_description const * const description = vk_format_description(format);

   if (description->format == PIPE_FORMAT_NONE || description->layout != UTIL_FORMAT_LAYOUT_PLAIN) {
      return FMT_INVALID;
   }

   if (description->nr_channels == 3) {
      if (description->channel[0].size == description->channel[1].size) {
         if (description->channel[0].size == description->channel[2].size) {
            switch (description->channel[0].size) {
            case 8:
               return FMT_8_8_8;
            case 16:
               if (description->channel[0].type == UTIL_FORMAT_TYPE_FLOAT) {
                  return FMT_16_16_16_FLOAT;
               }
               return FMT_16_16_16;
            }
         } else if (description->channel[0].size == 11 && description->channel[2].size == 10) {
            return FMT_10_11_11;
         }
      } else if (description->channel[1].size == description->channel[2].size) {
         if (description->channel[0].size == 2 && description->channel[1].size == 3) {
            return FMT_3_3_2;
         } else if (description->channel[0].size == 10 && description->channel[1].size == 11) {
            return FMT_11_11_10;
         }
      }
   }

   return FMT_INVALID;
}

uint32_t
terakan_format_vertex_get_sign(VkFormat const format)
{
   struct util_format_description const * const description = vk_format_description(format);

   if (description->layout != UTIL_FORMAT_LAYOUT_PLAIN) {
      /* All supported non-plain vertex formats are unsigned or float. */
      return 0;
   }

   int const channel_index = terakan_format_get_depth_or_first_non_void_channel(description);
   if (channel_index < 0) {
      return 0;
   }
   struct util_format_channel_description const * const channel =
      &description->channel[channel_index];

   return (uint32_t)(channel->type == UTIL_FORMAT_TYPE_SIGNED);
}

VKAPI_ATTR void VKAPI_CALL
terakan_GetPhysicalDeviceFormatProperties2(VkPhysicalDevice const physicalDevice,
                                           VkFormat const format,
                                           VkFormatProperties2 * const pFormatProperties)
{
   struct terakan_physical_device const * const device =
      terakan_physical_device_from_handle(physicalDevice);

   VkFormatFeatureFlags2 image_features = 0;
   VkFormatFeatureFlags2 image_optimal_only_features = 0;
   VkFormatFeatureFlags2 buffer_features = 0;

   bool const is_tiled_only = terakan_format_is_tiled_only(format);

   uint32_t const color_format = terakan_format_color_get_format(format);
   uint32_t const color_number_type = terakan_format_color_get_number_type(format);
   bool const color_supported = color_format != V_028C70_COLOR_INVALID &&
                                color_number_type != UINT32_MAX &&
                                terakan_format_color_get_swap(format) != UINT32_MAX;

   uint32_t const depth_format = terakan_format_depth_get_format(format);
   bool const has_stencil_8 = terakan_format_has_stencil_8(format);
   bool const is_depth_stencil = depth_format != V_028040_Z_INVALID || has_stencil_8;

   uint32_t const data_number_format = terakan_format_data_get_number_format(format);
   uint32_t const texture_format =
      data_number_format != UINT32_MAX ? terakan_format_texture_get_format(format) : FMT_INVALID;
   uint32_t const vertex_format =
      data_number_format != UINT32_MAX ? terakan_format_vertex_get_format(format) : FMT_INVALID;

   if (color_supported) {
      image_features |= VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_2_BLIT_DST_BIT;
      if (terakan_format_color_is_blendable(color_format, color_number_type)) {
         image_features |= VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT;
      }
      /* Storage images and buffers require both a random access target and a texture resource.
       * 1D storage images must be linear according to the Gallium R600 driver, so don't permit
       * storage image usage for tiled-only formats at all (as well as for depth / stencil formats,
       * because depth / stencil attachments must be tiled).
       */
      if (texture_format != FMT_INVALID && !is_tiled_only && !is_depth_stencil) {
         VkFormatFeatureFlags2 const storage_image_features =
            VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT |
            VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT |
            VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT;
         /* According to R800 AddrLib, "Tex2D UAV on cypress will fail/hang if tile mode is
          * linear".
          */
         if (device->chip_family_info.chip_family == CHIP_CYPRESS) {
            image_optimal_only_features |= storage_image_features;
         } else {
            image_features |= storage_image_features;
         }
      }
      if (vertex_format != FMT_INVALID) {
         buffer_features |= VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_BIT |
                            VK_FORMAT_FEATURE_2_STORAGE_READ_WITHOUT_FORMAT_BIT |
                            VK_FORMAT_FEATURE_2_STORAGE_WRITE_WITHOUT_FORMAT_BIT;
      }
      if (format == VK_FORMAT_R32_UINT || format == VK_FORMAT_R32_SINT) {
         image_features |= VK_FORMAT_FEATURE_2_STORAGE_IMAGE_ATOMIC_BIT;
         buffer_features |= VK_FORMAT_FEATURE_2_STORAGE_TEXEL_BUFFER_ATOMIC_BIT;
      }
   }

   if (is_depth_stencil) {
      image_optimal_only_features |= VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT;
   }

   if (texture_format != FMT_INVALID) {
      image_features |= VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_2_BLIT_SRC_BIT;
      if (data_number_format != V_030010_SQ_NUM_FORMAT_INT) {
         image_features |= VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
      }
      if (depth_format != V_028040_Z_INVALID) {
         image_features |= VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_DEPTH_COMPARISON_BIT;
      }
   }

   if (vertex_format != FMT_INVALID) {
      buffer_features |=
         VK_FORMAT_FEATURE_2_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_2_VERTEX_BUFFER_BIT;
   }

   if (image_features) {
      image_features |= VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT;
   }
   if (buffer_features) {
      buffer_features |=
         VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT;
   }

   if (is_depth_stencil || vk_format_is_block_compressed(format)) {
      /* bufferFeatures must not support any features for depth or stencil or block-compressed
       * formats according to the Vulkan specification. Internally, for simplicity of both
       * implementation and usage, Terakan format logic commonly doesn't distinguish between R8 and
       * S8, R16 and D16, R32 and D32, and also provides depth aspect information for combined depth
       * and stencil formats.
       */
      buffer_features = 0;
   }

   VkFormatFeatureFlags2 const image_linear_features = is_tiled_only ? 0 : image_features;
   /* If the format supports only linear images, linear tiling is considered optimal. */
   VkFormatFeatureFlags2 const image_optimal_features =
      image_features | (terakan_format_is_linear_only(format) ? 0 : image_optimal_only_features);

   pFormatProperties->formatProperties.linearTilingFeatures = image_linear_features;
   pFormatProperties->formatProperties.optimalTilingFeatures =
      (VkFormatFeatureFlags)image_optimal_features;
   pFormatProperties->formatProperties.bufferFeatures = (VkFormatFeatureFlags)buffer_features;

   VkFormatProperties3 * const format_properties_3 =
      vk_find_struct(pFormatProperties->pNext, FORMAT_PROPERTIES_3);
   if (format_properties_3 != NULL) {
      format_properties_3->linearTilingFeatures = image_linear_features;
      format_properties_3->optimalTilingFeatures = image_optimal_features;
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

   image_format_properties.maxExtent.width = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   switch (pImageFormatInfo->type) {
   case VK_IMAGE_TYPE_1D:
      image_format_properties.maxExtent.height = 1;
      image_format_properties.maxExtent.depth = 1;
      image_format_properties.maxArrayLayers =
         is_target ? TERAKAN_IMAGE_MAX_TARGET_SLICES : TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
      break;
   case VK_IMAGE_TYPE_2D:
      image_format_properties.maxExtent.height = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
      image_format_properties.maxExtent.depth = 1;
      image_format_properties.maxArrayLayers =
         is_target ? TERAKAN_IMAGE_MAX_TARGET_SLICES : TERAKAN_IMAGE_MAX_DEPTH_2D_SLICES;
      break;
   case VK_IMAGE_TYPE_3D:
      image_format_properties.maxExtent.height = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
      image_format_properties.maxExtent.depth =
         (pImageFormatInfo->usage & VK_IMAGE_USAGE_STORAGE_BIT) ||
               (is_target && (pImageFormatInfo->flags & VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT))
            ? TERAKAN_IMAGE_MAX_TARGET_SLICES
            : TERAKAN_IMAGE_MAX_DEPTH_2D_SLICES;
      image_format_properties.maxArrayLayers = 1;
      break;
   default:
      return VK_ERROR_FORMAT_NOT_SUPPORTED;
   }
   image_format_properties.maxMipLevels = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES_LOG2 + 1;

   image_format_properties.sampleCounts = VK_SAMPLE_COUNT_1_BIT;
   /* Multisampled images must be 2D-tiled.
    * For linear-only formats, the optimal tiling is linear tiling, so check both conditions.
    */
   if ((features & (VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT |
                    VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT)) &&
       pImageFormatInfo->type == VK_IMAGE_TYPE_2D &&
       !(pImageFormatInfo->flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) &&
       pImageFormatInfo->tiling == VK_IMAGE_TILING_OPTIMAL &&
       !terakan_format_is_linear_only(pImageFormatInfo->format)) {
      image_format_properties.sampleCounts |=
         VK_SAMPLE_COUNT_2_BIT | VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_8_BIT;
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
