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

#ifndef TERAKAN_SAMPLER_H
#define TERAKAN_SAMPLER_H

#include "gallium/drivers/r600/evergreend.h"
#include "util/u_math.h"
#include "vk_sampler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t
terakan_sampler_translate_filter(VkFilter const filter, bool const aniso)
{
   if (filter == VK_FILTER_LINEAR) {
      return aniso ? V_03C000_SQ_TEX_XY_FILTER_ANISO_BILINEAR : V_03C000_SQ_TEX_XY_FILTER_BILINEAR;
   }
   assert(filter == VK_FILTER_NEAREST);
   return aniso ? V_03C000_SQ_TEX_XY_FILTER_ANISO_POINT : V_03C000_SQ_TEX_XY_FILTER_POINT;
}

static inline uint32_t
terakan_sampler_translate_mipmap_mode(VkSamplerMipmapMode const mipmap_mode)
{
   if (mipmap_mode == VK_SAMPLER_MIPMAP_MODE_LINEAR) {
      return V_03C000_SQ_TEX_Z_FILTER_LINEAR;
   }
   assert(mipmap_mode == VK_SAMPLER_MIPMAP_MODE_NEAREST);
   return V_03C000_SQ_TEX_Z_FILTER_POINT;
}

static inline uint32_t
terakan_sampler_translate_address_mode(VkSamplerAddressMode const address_mode)
{
   switch (address_mode) {
   case VK_SAMPLER_ADDRESS_MODE_REPEAT:
      return V_03C000_SQ_TEX_WRAP;
   case VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
      return V_03C000_SQ_TEX_MIRROR;
   case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
      return V_03C000_SQ_TEX_CLAMP_LAST_TEXEL;
   case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
      return V_03C000_SQ_TEX_CLAMP_BORDER;
   case VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:
      return V_03C000_SQ_TEX_MIRROR_ONCE_LAST_TEXEL;
   default:
      assert(!"Unsupported sampler address mode");
      return V_03C000_SQ_TEX_WRAP;
   }
}

static inline int32_t
terakan_sampler_translate_mip_lod_bias(float const mip_lod_bias)
{
   return (int32_t)(CLAMP(mip_lod_bias, -0x1.0p5f, 0x1.0p5f - 0x1.0p-8f) * 0x1.0p8f);
}

static inline uint32_t
terakan_sampler_translate_min_max_lod(float const lod)
{
   return (uint32_t)(CLAMP(lod, 0.0f, 0x1.0p4f - 0x1.0p-8f) * 0x1.0p8f);
}

static inline uint32_t
terakan_sampler_translate_max_anisotropy(float const max_anisotropy)
{
   return util_logbase2((unsigned)CLAMP(max_anisotropy, 1.0f, 0x1.0p4f));
}

static inline uint32_t
terakan_sampler_translate_border_color(VkBorderColor const border_color)
{
   switch (border_color) {
   case VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK:
   case VK_BORDER_COLOR_INT_TRANSPARENT_BLACK:
      return V_03C000_SQ_TEX_BORDER_COLOR_TRANS_BLACK;
   case VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK:
   case VK_BORDER_COLOR_INT_OPAQUE_BLACK:
      return V_03C000_SQ_TEX_BORDER_COLOR_OPAQUE_BLACK;
   case VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE:
   case VK_BORDER_COLOR_INT_OPAQUE_WHITE:
      return V_03C000_SQ_TEX_BORDER_COLOR_OPAQUE_WHITE;
   case VK_BORDER_COLOR_FLOAT_CUSTOM_EXT:
   case VK_BORDER_COLOR_INT_CUSTOM_EXT:
      return V_03C000_SQ_TEX_BORDER_COLOR_REGISTER;
   default:
      assert(!"Unsupported sampler border color");
      return V_03C000_SQ_TEX_BORDER_COLOR_TRANS_BLACK;
   }
}

struct terakan_sampler {
   struct vk_sampler vk;

   uint32_t sampler[3];

   bool unnormalized_coordinates;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_sampler, vk.base, VkSampler, VK_OBJECT_TYPE_SAMPLER)

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_SAMPLER_H */
