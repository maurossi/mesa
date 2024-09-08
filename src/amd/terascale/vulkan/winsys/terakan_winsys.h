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

#ifndef TERAKAN_WINSYS_H
#define TERAKAN_WINSYS_H

#include "terakan_gpu_info.h"

#include "ac_surface.h"
#include "amd_family.h"
#include "vk_sync.h"

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

/* Each group of two has the same priority (the maximum priority is 0xF).
 * Similar to RADEON_PRIO in the Gallium Radeon winsys.
 */
enum terakan_winsys_cs_bo_priority {
   TERAKAN_WINSYS_CS_BO_PRIORITY_FENCE_TRACE = 0,
   TERAKAN_WINSYS_CS_BO_PRIORITY_TRANSFORM_FEEDBACK_COUNTER = 0,

   TERAKAN_WINSYS_CS_BO_PRIORITY_QUERY = 1,

   TERAKAN_WINSYS_CS_BO_PRIORITY_DRAW_INDIRECT = 2,
   TERAKAN_WINSYS_CS_BO_PRIORITY_INDEX_BUFFER = 2,

   TERAKAN_WINSYS_CS_BO_PRIORITY_CP_DMA = 3,

   TERAKAN_WINSYS_CS_BO_PRIORITY_UNIFORM_BUFFER = 4,

   TERAKAN_WINSYS_CS_BO_PRIORITY_SHADER_READ_BUFFER = 5,
   TERAKAN_WINSYS_CS_BO_PRIORITY_VERTEX_BUFFER = 5,

   TERAKAN_WINSYS_CS_BO_PRIORITY_SHADER_RW_BUFFER = 6,
   TERAKAN_WINSYS_CS_BO_PRIORITY_SHADER_READ_IMAGE = 6,

   TERAKAN_WINSYS_CS_BO_PRIORITY_SHADER_RW_IMAGE = 7,
   TERAKAN_WINSYS_CS_BO_PRIORITY_SHADER_READ_IMAGE_MS = 7,

   TERAKAN_WINSYS_CS_BO_PRIORITY_COLOR_BUFFER = 8,
   TERAKAN_WINSYS_CS_BO_PRIORITY_DEPTH_BUFFER = 8,

   TERAKAN_WINSYS_CS_BO_PRIORITY_COLOR_BUFFER_MS = 9,
   TERAKAN_WINSYS_CS_BO_PRIORITY_DEPTH_BUFFER_MS = 9,

   TERAKAN_WINSYS_CS_BO_PRIORITY_SEPARATE_META = 10,
   /* The hardware can't hide instruction cache misses. */
   TERAKAN_WINSYS_CS_BO_PRIORITY_SHADER_BINARY = 10,

   TERAKAN_WINSYS_CS_BO_PRIORITY_SHADER_RINGS = 11,
   TERAKAN_WINSYS_CS_BO_PRIORITY_SCRATCH_BUFFER = 11,
};

struct terakan_winsys;
struct terakan_winsys_bo;

struct terakan_winsys_surface_fn {
   bool (*translate_image_create_info)(struct terakan_winsys const * winsys,
                                       VkImageCreateInfo const * image_create_info,
                                       struct radeon_surf * surface_out);
};

struct terakan_winsys_fn {
   struct vk_sync_type const * const * (*get_sync_types)(struct terakan_winsys * winsys);

   void (*destroy)(struct terakan_winsys * winsys);
};

struct terakan_winsys_bo_fn {
   /* Returns whether setting was successful. */
   bool (*set_tiling_for_surface)(struct terakan_winsys_bo * bo,
                                  struct radeon_surf const * surface);

   void * (*map_impl)(struct terakan_winsys_bo * bo);
   void (*unmap_impl)(struct terakan_winsys_bo * bo);

   /* Returns whether the wait was successful.
    * Waiting can be performed from any thread, however, the behavior is undefined while command
    * submissions referencing this BO are being performed - it must be ensured externally that
    * waiting is done after the submission whose completion needs to be awaited and before any other
    * submissions referencing this buffer.
    * In case of a GPU hang, must return in finite time.
    */
   bool (*wait_idle)(struct terakan_winsys_bo * bo);

   void (*free_impl)(struct terakan_winsys_bo * bo);

   /* For host-visible device-local memory, it's assumed that the kernel driver can evict
    * allocations to the GART automatically, and that host-visible device-local memory should be
    * preferred over copying to non-host-visible memory on the GPU if the CPU writes the data
    * frequently, as long as each submission doesn't reference too much host-visible device-local
    * memory. Falling back to trying HOST_VISIBLE alone if a HOST_VISIBLE | DEVICE_LOCAL allocation
    * attempt fails is also not necessary.
    */
   struct terakan_winsys_bo * (*allocate_device_memory)(
      struct terakan_winsys * winsys, VkDeviceSize size, VkDeviceSize alignment,
      VkMemoryPropertyFlags flags, VkExternalMemoryHandleTypeFlags export_handle_types);

#if !defined(_WIN32)
   int (*export_fd)(struct terakan_winsys_bo * bo, bool writable);
   /* Returns whether the query was successful. */
   bool (*get_fd_vram_preference)(struct terakan_winsys * winsys, int fd,
                                  bool * vram_preferred_out);
   struct terakan_winsys_bo * (*import_fd)(struct terakan_winsys * winsys, int fd,
                                           VkDeviceSize size, bool prefer_vram);
#endif
};

struct terakan_winsys_cs_fn {
   /* BO references are winsys-specific objects whose size is
    * terakan_gpu_info::cs_bo_reference_size.
    */
   void (*create_bo_reference)(void * bo_reference, struct terakan_winsys_bo const * bo,
                               bool is_reading, bool is_writing,
                               enum terakan_winsys_cs_bo_priority priority);
   void (*update_bo_reference)(void * bo_reference, struct terakan_winsys_bo const * bo,
                               bool is_reading, bool is_writing,
                               enum terakan_winsys_cs_bo_priority priority);

   /* Not exposing queue priorities as the Linux Radeon 2.50.0 kernel driver only provides a
    * high-priority DMA ring on R9xx.
    */
   VkResult (*submit)(struct terakan_winsys * winsys, enum amd_ip_type ip_type,
                      uint32_t bo_reference_count, void const * bo_references,
                      uint32_t indirect_buffer_size_dwords, uint32_t const * indirect_buffer,
                      bool is_end_of_frame);
};

struct terakan_winsys {
   struct terakan_winsys_fn const * fn;
   struct terakan_winsys_surface_fn const * surface_fn;
   struct terakan_winsys_bo_fn const * bo_fn;
   struct terakan_winsys_cs_fn const * cs_fn;

   struct terakan_gpu_info gpu_info;

   uint32_t last_bo_creation_number;
};

void terakan_winsys_base_init(struct terakan_winsys * winsys);

struct terakan_winsys_bo {
   struct terakan_winsys * winsys;

   /* Value of the monotonically increasing counter of created BOs given to this BO.
    * Only for purposes like hashing - must not be used as an unique identifier, as it may wrap
    * around.
    */
   uint32_t creation_number;

   void * mapping;
};

void * terakan_winsys_bo_map(struct terakan_winsys_bo * bo);
void terakan_winsys_bo_unmap(struct terakan_winsys_bo * bo);

/* If the buffer is currently mapped, freeing it implicitly unmaps it. */
void terakan_winsys_bo_free(struct terakan_winsys_bo * bo);

void terakan_winsys_bo_base_init(struct terakan_winsys_bo * bo, struct terakan_winsys * winsys);

#endif /* TERAKAN_WINSYS_H */
