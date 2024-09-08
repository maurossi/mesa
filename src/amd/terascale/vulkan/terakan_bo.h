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

#ifndef TERAKAN_BO_H
#define TERAKAN_BO_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Each group of two has the same priority (the maximum priority is 0xF).
 * Similar to RADEON_PRIO in the Gallium Radeon winsys.
 */
enum terakan_bo_priority {
   TERAKAN_BO_PRIORITY_SYNC = 0,
   TERAKAN_BO_PRIORITY_TRANSFORM_FEEDBACK_COUNTER = 0,

   TERAKAN_BO_PRIORITY_QUERY = 1,

   TERAKAN_BO_PRIORITY_DRAW_INDIRECT = 2,
   TERAKAN_BO_PRIORITY_INDEX_BUFFER = 2,

   TERAKAN_BO_PRIORITY_CP_DMA = 3,

   TERAKAN_BO_PRIORITY_UNIFORM_BUFFER = 4,

   TERAKAN_BO_PRIORITY_SHADER_READ_BUFFER = 5,
   TERAKAN_BO_PRIORITY_VERTEX_BUFFER = 5,

   TERAKAN_BO_PRIORITY_SHADER_RW_BUFFER = 6,
   TERAKAN_BO_PRIORITY_SHADER_READ_IMAGE = 6,

   TERAKAN_BO_PRIORITY_SHADER_RW_IMAGE = 7,
   TERAKAN_BO_PRIORITY_SHADER_READ_IMAGE_MS = 7,

   TERAKAN_BO_PRIORITY_COLOR_BUFFER = 8,
   TERAKAN_BO_PRIORITY_DEPTH_BUFFER = 8,

   TERAKAN_BO_PRIORITY_COLOR_BUFFER_MS = 9,
   TERAKAN_BO_PRIORITY_DEPTH_BUFFER_MS = 9,

   TERAKAN_BO_PRIORITY_SEPARATE_META = 10,
   /* The hardware can't hide instruction cache misses. */
   TERAKAN_BO_PRIORITY_SHADER_BINARY = 10,

   TERAKAN_BO_PRIORITY_SHADER_RINGS = 11,
   TERAKAN_BO_PRIORITY_SCRATCH_BUFFER = 11,
};

struct terakan_device;

struct terakan_bo_tiling {
   uint32_t pitch_bytes;

   uint8_t array_mode;

   /* ATTRIB register field values (log2, some being exponent-biased). */
   uint8_t attrib_tile_split;         /* 0 = 2^6 bytes. */
   uint8_t attrib_stencil_tile_split; /* 0 = 2^6 bytes. */
   uint8_t attrib_bank_width;
   uint8_t attrib_bank_height;
   uint8_t attrib_macro_tile_aspect;
};

/* Partially implemented by the winsys. */
struct terakan_bo {
   struct terakan_device * device;

   /* 0 if virtual memory is not supported, in which case addresses in packets are relative to the
    * start of the BO.
    */
   uint64_t va;

   /* Value of the monotonically increasing counter of created BOs given to this BO.
    * Only for purposes like hashing - must not be used as an unique identifier, as it may wrap
    * around.
    */
   uint32_t creation_number;

   void * mapping;
};

void * terakan_bo_map(struct terakan_bo * bo);
void terakan_bo_unmap(struct terakan_bo * bo);

/* If the buffer is currently mapped, freeing it implicitly unmaps it. */
void terakan_bo_free(struct terakan_bo * bo, VkAllocationCallbacks const * allocator);

void terakan_bo_init(struct terakan_bo * bo, struct terakan_device * device);

/* Non-`impl` functions are public. */
struct terakan_bo_winsys_fn {
   /* Returns whether setting was successful. */
   bool (*set_tiling)(struct terakan_bo * bo, struct terakan_bo_tiling const * tiling);

   /* Only needed if the winsys supports VK_KHR_external_memory_fd. */
   int (*export_fd)(struct terakan_bo * bo, bool writable);

   void * (*map_impl)(struct terakan_bo * bo);
   void (*unmap_impl)(struct terakan_bo * bo);

   void (*free_impl)(struct terakan_bo * bo, VkAllocationCallbacks const * allocator);

   /* For host-visible device-local memory, it's assumed that the kernel driver can evict
    * allocations to the GTT automatically, and that host-visible device-local memory should be
    * preferred over copying to non-host-visible memory on the GPU if the CPU writes the data
    * frequently, as long as each submission doesn't reference too much host-visible device-local
    * memory. Falling back to trying HOST_VISIBLE alone if a HOST_VISIBLE | DEVICE_LOCAL allocation
    * attempt fails is also not necessary.
    * On failure, returns VK_ERROR_OUT_OF_HOST_MEMORY or VK_ERROR_OUT_OF_DEVICE_MEMORY.
    */
   VkResult (*allocate_device_memory)(struct terakan_device * device, VkDeviceSize size,
                                      VkDeviceSize alignment, VkMemoryPropertyFlags flags,
                                      VkExternalMemoryHandleTypeFlags export_handle_types,
                                      VkAllocationCallbacks const * allocator,
                                      VkSystemAllocationScope scope, struct terakan_bo ** bo_out);

   /* Only needed if the winsys supports VK_KHR_external_memory_fd.
    * Returns whether the query was successful.
    */
   bool (*get_fd_vram_preference)(struct terakan_device * device, int fd,
                                  bool * vram_preferred_out);
   /* Only needed if the winsys supports VK_KHR_external_memory_fd.
    * On failure, returns VK_ERROR_OUT_OF_HOST_MEMORY or VK_ERROR_INVALID_EXTERNAL_HANDLE.
    */
   VkResult (*import_fd)(struct terakan_device * device, int fd, VkDeviceSize size,
                         bool prefer_vram, VkAllocationCallbacks const * allocator,
                         VkSystemAllocationScope scope, struct terakan_bo ** bo_out);
};

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_BO_H */
