/*
 * Copyright © 2023 Vitaliy Triang3l Kuzmin
 *
 * Based on Gallium Radeon DRM winsys which is:
 * Copyright © 2008 Jérôme Glisse
 * Copyright © 2009 Corbin Simpson <MostAwesomeDude@gmail.com>
 * Copyright © 2011 Marek Olšák <maraeo@gmail.com>
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

#include "terakan_winsys_drm_radeon.h"

#include "util/macros.h"
#include "util/u_debug.h"
#include "util/u_math.h"

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <xf86drm.h>
#include <radeon_drm.h>

static void
terakan_winsys_drm_radeon_cs_create_bo_reference(void * bo_reference_ptr,
                                                 struct terakan_winsys_bo const * bo_base,
                                                 bool is_reading, bool is_writing,
                                                 enum terakan_winsys_cs_bo_priority priority)
{
   struct drm_radeon_cs_reloc * const bo_reference = (struct drm_radeon_cs_reloc *)bo_reference_ptr;

   struct terakan_winsys_drm_radeon_bo const * const bo =
      container_of(bo_base, struct terakan_winsys_drm_radeon_bo const, base);

   bo_reference->handle = bo->handle;

   bo_reference->read_domains = is_reading ? bo->domains : 0;
   bo_reference->write_domain = is_writing ? bo->domains : 0;

   assert(((__u32)priority & ~(__u32)RADEON_RELOC_PRIO_MASK) == 0);
   bo_reference->flags = (__u32)priority;
}

static void
terakan_winsys_drm_radeon_cs_update_bo_reference(void * bo_reference_ptr,
                                                 struct terakan_winsys_bo const * bo_base,
                                                 bool is_reading, bool is_writing,
                                                 enum terakan_winsys_cs_bo_priority priority)
{
   struct drm_radeon_cs_reloc * const bo_reference = (struct drm_radeon_cs_reloc *)bo_reference_ptr;

   struct terakan_winsys_drm_radeon_bo const * const bo =
      container_of(bo_base, struct terakan_winsys_drm_radeon_bo const, base);

   assert(bo_reference->handle == bo->handle);

   if (is_reading) {
      bo_reference->read_domains |= bo->domains;
   }
   if (is_writing) {
      bo_reference->write_domain |= bo->domains;
   }

   assert(((__u32)priority & ~(__u32)RADEON_RELOC_PRIO_MASK) == 0);
   /* The flags only contain the priority. */
   bo_reference->flags = MAX2((__u32)priority, bo_reference->flags);
}

static VkResult
terakan_winsys_drm_radeon_cs_submit(struct terakan_winsys * const winsys_base,
                                    enum amd_ip_type const ip_type,
                                    uint32_t const bo_reference_count,
                                    void const * const bo_references,
                                    uint32_t const indirect_buffer_size_dwords,
                                    uint32_t const * const indirect_buffer,
                                    bool const is_end_of_frame)
{
   if (indirect_buffer_size_dwords == 0) {
      /* The kernel driver returns -EINVAL for zero-length indirect buffers. */
      return VK_SUCCESS;
   }

   /* Flags, ring, priority. */
   __u32 flags[3] = {0};

   switch (ip_type) {
   case AMD_IP_GFX:
      /* CP fetch requires 8 dword alignment, the provided indirect buffer must be padded with NOPs
       * externally if needed.
       */
      assert((indirect_buffer_size_dwords & 7) == 0);
      flags[0] |= RADEON_CS_KEEP_TILING_FLAGS;
      if (is_end_of_frame) {
         flags[0] |= RADEON_CS_END_OF_FRAME;
      }
      flags[1] = RADEON_CS_RING_GFX;
      break;

   default:
      assert(!"Unsupported queue type");
      return VK_ERROR_DEVICE_LOST;
   }

   struct drm_radeon_cs_chunk relocations_chunk = {
      .chunk_id = RADEON_CHUNK_ID_RELOCS,
      .length_dw =
         (__u32)((sizeof(struct drm_radeon_cs_reloc) / sizeof(__u32)) * bo_reference_count),
      .chunk_data = (__u64)bo_references,
   };

   struct drm_radeon_cs_chunk indirect_buffer_chunk = {
      .chunk_id = RADEON_CHUNK_ID_IB,
      .length_dw = indirect_buffer_size_dwords,
      .chunk_data = (__u64)(void const *)indirect_buffer,
   };

   struct drm_radeon_cs_chunk flags_chunk = {
      .chunk_id = RADEON_CHUNK_ID_FLAGS,
      .length_dw = ARRAY_SIZE(flags),
      .chunk_data = (__u64)(void const *)flags,
   };

   __u64 chunks[] = {
      (__u64)(void *)&relocations_chunk,
      (__u64)(void *)&indirect_buffer_chunk,
      (__u64)(void *)&flags_chunk,
   };

   struct drm_radeon_cs cs_arguments = {
      .num_chunks = ARRAY_SIZE(chunks),
      .chunks = (__u64)(void const *)chunks,
   };

   struct terakan_winsys_drm_radeon const * const winsys =
      container_of(winsys_base, struct terakan_winsys_drm_radeon const, base);

   int const cs_result =
      drmCommandWriteRead(winsys->fd, DRM_RADEON_CS, &cs_arguments, sizeof(cs_arguments));

   if (cs_result != 0) {
      if (cs_result == -ENOMEM) {
         fputs("terakan/drm_radeon: Not enough memory for command submission.\n", stderr);
         return VK_ERROR_OUT_OF_HOST_MEMORY;
      }

      fprintf(
         stderr,
         "terakan/drm_radeon: The kernel has rejected the command submission with error number %d, "
         "see dmesg for more information.\n",
         cs_result);
      if (debug_get_bool_option("TERAKAN_DUMP_CS", false)) {
         fputs("terakan/drm_radeon: Dumping the rejected command buffer...\n", stderr);
         for (uint32_t indirect_buffer_dword_index = 0;
              indirect_buffer_dword_index < indirect_buffer_size_dwords;
              ++indirect_buffer_dword_index) {
            fprintf(stderr, "0x%08" PRIX32 "\n", indirect_buffer[indirect_buffer_dword_index]);
         }
      }
      return VK_ERROR_DEVICE_LOST;
   }

   return VK_SUCCESS;
}

struct terakan_winsys_cs_fn const terakan_winsys_drm_radeon_cs_fn = {
   .create_bo_reference = terakan_winsys_drm_radeon_cs_create_bo_reference,
   .update_bo_reference = terakan_winsys_drm_radeon_cs_update_bo_reference,
   .submit = terakan_winsys_drm_radeon_cs_submit,
};
