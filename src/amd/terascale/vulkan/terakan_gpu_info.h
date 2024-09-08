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

#ifndef TERAKAN_GPU_INFO_H
#define TERAKAN_GPU_INFO_H

#include "amd_family.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#define TERAKAN_ATI_VENDOR_ID 0x1002

struct terakan_gpu_info {
   /* The following are initialized via terakan_gpu_info_init_chip_family. */

   uint32_t pci_id;
   enum radeon_family chip_family;
   char const * chip_family_name;
   enum amd_gfx_level gfx_level;

   bool has_dedicated_vram;

   bool has_vertex_cache;
   uint32_t sq_max_threads;
   uint32_t sq_ps_threads_r8xx;
   uint32_t sq_max_stack_entries;

   /* The following need to be set up by the winsys. */

   uint8_t tile_pipes_log2;
   uint8_t tile_banks_log2;
   uint8_t tile_pipe_interleave_bytes_log2;
   uint8_t tile_row_bytes_log2;

   size_t gart_page_size;
   uint64_t gart_size;
   uint64_t vram_size;
   uint64_t vram_visible; /* Included in vram_size. */

   VkDeviceSize bo_size_granularity;
   VkDeviceSize max_bo_size;
   size_t min_memory_map_alignment;

   size_t cs_bo_reference_size;
   size_t cs_bo_reference_alignment;

   uint32_t clock_crystal_frequency;

   /* The following are set up by terakan_gpu_info_init_complete. */

   VkDeviceSize buffer_image_bo_alignment;
};

/* Returns whether the GPU is supported by Terakan.
 * If it's not, the output info is unmodified.
 */
bool terakan_gpu_info_init_chip_family(struct terakan_gpu_info * info, uint32_t pci_id);

void terakan_gpu_info_init_complete(struct terakan_gpu_info * info);

#endif /* TERAKAN_GPU_INFO_H */
