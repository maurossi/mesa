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

#ifndef TERAKAN_PHYSICAL_DEVICE_H
#define TERAKAN_PHYSICAL_DEVICE_H

#include "terakan_instance.h"
#include "wsi_common.h"

#include "amd_family.h"
#include "vk_physical_device.h"
#include "vk_sync.h"
#include "vk_sync_binary.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_WIN32)
#define TERAKAN_PHYSICAL_DEVICE_HAS_WINSYS_DRM_RADEON
#endif

#define TERAKAN_PHYSICAL_DEVICE_VENDOR_ID_ATI 0x1002

enum radeon_family terakan_physical_device_get_chip_family(uint32_t pci_device_id);

static inline bool
terakan_physical_device_is_chip_family_supported(enum radeon_family const chip_family)
{
   return chip_family >= CHIP_CEDAR && chip_family <= CHIP_ARUBA;
}

char const * terakan_physical_device_chip_family_name(enum radeon_family chip_family);

struct terakan_physical_device_chip_family_info {
   enum radeon_family chip_family;

   bool is_r9xx;

   bool has_dedicated_vram;

   bool has_vertex_cache;
   uint32_t sq_max_threads;
   uint32_t sq_ps_threads_r8xx;
   uint32_t sq_max_stack_entries;
};

void terakan_physical_device_chip_family_info_init(
   enum radeon_family chip_family,
   struct terakan_physical_device_chip_family_info * chip_family_info_out);

struct terakan_physical_device_tiling_info {
   uint8_t pipes_log2;
   uint8_t banks_log2;
   uint8_t pipe_interleave_bytes_log2;
   uint8_t row_bytes_log2;
};

static inline bool
terakan_physical_device_tiling_info_equal(
   struct terakan_physical_device_tiling_info const * const a,
   struct terakan_physical_device_tiling_info const * const b)
{
   return a->pipes_log2 == b->pipes_log2 && a->banks_log2 == b->banks_log2 &&
          a->pipe_interleave_bytes_log2 == b->pipe_interleave_bytes_log2 &&
          a->row_bytes_log2 == b->row_bytes_log2;
}

struct terakan_physical_device;
struct terakan_device;

struct terakan_physical_device_winsys_fn {
   /* Called before vk_physical_device_init to get capabilities of additional extensions offered by
    * the winsys, as well as the device UUID. It's assumed that before the call, the fields this
    * function is supposed to set are zero in the capability structures.
    */
   void (*get_winsys_extensions)(struct terakan_physical_device const * device,
                                 struct vk_device_extension_table * extensions,
                                 struct vk_features * features, struct vk_properties * properties);

   VkResult (*create_device)(struct terakan_physical_device * physical_device,
                             VkDeviceCreateInfo const * create_info,
                             VkAllocationCallbacks const * allocator,
                             struct terakan_device ** device_out);

   void (*destroy)(struct terakan_physical_device * device);
};

/* Partially implemented by the winsys. */
struct terakan_physical_device {
   struct vk_physical_device vk;

   /* Private, use wrappers externally. */
   struct terakan_physical_device_winsys_fn const * winsys_fn;

   uint32_t pci_device_id;

   struct terakan_physical_device_chip_family_info chip_family_info;

   VkDeviceSize max_memory_allocation_size;

   struct terakan_physical_device_tiling_info tiling_info;
   VkDeviceSize buffer_image_bo_alignment;

   VkPhysicalDeviceMemoryProperties memory_properties;

   struct wsi_device wsi_device;
};

VK_DEFINE_HANDLE_CASTS(terakan_physical_device, vk.base, VkPhysicalDevice,
                       VK_OBJECT_TYPE_PHYSICAL_DEVICE)

VkExternalMemoryHandleTypeFlags terakan_physical_device_supported_external_memory_types(
   struct terakan_physical_device const * device);

void terakan_physical_device_finish(struct terakan_physical_device * device);

void terakan_physical_device_destroy(struct vk_physical_device * device);

/* vram_visible is included in vram_size.
 * clock_crystal_frequency can be 0 if not available, in this case timestamp queries will be
 * disabled.
 */
VkResult terakan_physical_device_init(
   struct terakan_physical_device * device, struct terakan_instance * instance,
   struct terakan_physical_device_winsys_fn const * winsys_fn_static, uint32_t pci_device_id,
   VkDeviceSize gtt_page_size, VkDeviceSize gtt_size, VkDeviceSize vram_size,
   VkDeviceSize vram_visible, VkDeviceSize max_memory_allocation_size,
   VkDeviceSize min_memory_map_alignment,
   struct terakan_physical_device_tiling_info const * tiling_info, uint32_t clock_crystal_frequency,
   struct vk_sync_type const * const * supported_sync_types_static);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_PHYSICAL_DEVICE_H */
