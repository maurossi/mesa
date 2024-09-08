/*
 * Copyright © 2023 Vitaliy Triang3l Kuzmin
 *
 * Based in part on r800addrlib.h which is:
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

#include "terakan_gpu_info.h"

#include "terakan_descriptor.h"

#include "util/u_math.h"

#include <stddef.h>

bool
terakan_gpu_info_init_chip_family(struct terakan_gpu_info * const info, uint32_t const pci_id)
{
   enum radeon_family chip_family = CHIP_UNKNOWN;
   char const * chip_family_name = NULL;
   switch (pci_id) {
#define CHIPSET(chipset_pci_id, chipset_name, chipset_family)                                      \
   case chipset_pci_id:                                                                            \
      chip_family = CHIP_##chipset_family;                                                         \
      chip_family_name = #chipset_family;                                                          \
      break;
#include "pci_ids/r600_pci_ids.h"
#undef CHIPSET
   }
   /* Only Evergreen (R8xx) and Cayman (R9xx) are supported by Terakan. */
   if (!(chip_family >= CHIP_CEDAR && chip_family <= CHIP_ARUBA)) {
      return false;
   }

   info->pci_id = pci_id;
   info->chip_family = chip_family;
   info->chip_family_name = chip_family_name;
   info->gfx_level = chip_family >= CHIP_CAYMAN ? CAYMAN : EVERGREEN;

   switch (chip_family) {
   case CHIP_PALM:
   case CHIP_SUMO:
   case CHIP_SUMO2:
   case CHIP_ARUBA:
      info->has_dedicated_vram = false;
      break;
   default:
      info->has_dedicated_vram = true;
   }

   switch (chip_family) {
   case CHIP_CEDAR:
   case CHIP_PALM:
   case CHIP_SUMO:
   case CHIP_SUMO2:
   case CHIP_CAICOS:
      info->has_vertex_cache = false;
      break;
   default:
      /* Cayman vertex fetch always goes through the texture cache, but Linux Radeon 2.50.0 and the
       * Gallium R600 driver set SQ_CONFIG.VC_ENABLE to 1 on it.
       */
      info->has_vertex_cache = true;
   }

   if (info->gfx_level >= CAYMAN) {
      info->sq_max_threads = 256;
      info->sq_ps_threads_r8xx = 0;
   } else {
      switch (chip_family) {
      case CHIP_CEDAR:
      case CHIP_PALM:
      case CHIP_CAICOS:
         info->sq_max_threads = 192;
         break;
      default:
         info->sq_max_threads = 248;
      }

      switch (chip_family) {
      case CHIP_CEDAR:
      case CHIP_PALM:
      case CHIP_SUMO:
      case CHIP_SUMO2:
         info->sq_ps_threads_r8xx = 96;
         break;
      default:
         info->sq_ps_threads_r8xx = 128;
      }
   }

   switch (chip_family) {
   case CHIP_CEDAR:
   case CHIP_REDWOOD:
   case CHIP_PALM:
   case CHIP_SUMO:
   case CHIP_TURKS:
   case CHIP_CAICOS:
      info->sq_max_stack_entries = 256;
      break;
   default:
      info->sq_max_stack_entries = 512;
   }

   return true;
}

void
terakan_gpu_info_init_complete(struct terakan_gpu_info * const info)
{
   /* HwlComputeMaxBaseAlignments from the R800 AddrLib for images.
    * Maximum 8x8 micro-tile size is 8-sample and 16 byte-per-pixel.
    * With the largest tile size, the bank width and height can be treated as 1.
    */
   VkDeviceSize const max_image_alignment =
      (VkDeviceSize)1 << (MIN2(info->tile_row_bytes_log2, 3 + 3 + 3 + 4) + info->tile_banks_log2 +
                          info->tile_pipes_log2);
   info->buffer_image_bo_alignment = MAX2(TERAKAN_CONSTANT_CACHE_LINE_BYTES, max_image_alignment);
}
