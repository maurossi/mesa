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

#include "terakan_sync_completion.h"

#include "util/macros.h"
#include "util/u_memory.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <xf86drm.h>
#include <radeon_drm.h>

static bool
terakan_winsys_drm_radeon_get_drm_value(int const fd, unsigned const request,
                                        char const * const error_name, uint32_t * const out)
{
   struct drm_radeon_info info = {
      .request = request,
      .value = (__u64)(void const *)out,
   };
   int const result = drmCommandWriteRead(fd, DRM_RADEON_INFO, &info, sizeof(info));
   if (result != 0) {
      if (error_name != NULL) {
         fprintf(stderr,
                 "terakan/drm_radeon: Failed to get %s from the kernel driver, error number %d.\n",
                 error_name, result);
      }
      return false;
   }
   return true;
}

static struct vk_sync_type const * const *
terakan_winsys_drm_radeon_get_sync_types(struct terakan_winsys * const winsys_base)
{
   struct terakan_winsys_drm_radeon const * const winsys =
      container_of(winsys_base, struct terakan_winsys_drm_radeon const, base);
   return winsys->sync_types;
}

static void
terakan_winsys_drm_radeon_destroy(struct terakan_winsys * const winsys_base)
{
   struct terakan_winsys_drm_radeon * const winsys =
      container_of(winsys_base, struct terakan_winsys_drm_radeon, base);

   _mesa_hash_table_destroy(winsys->shared_bo_reference_counts, NULL);
   mtx_destroy(&winsys->shared_bo_mutex);

   radeon_surface_manager_free(winsys->surface_manager);

   FREE(winsys);
}

static struct terakan_winsys_fn const terakan_winsys_drm_radeon_fn = {
   .get_sync_types = terakan_winsys_drm_radeon_get_sync_types,
   .destroy = terakan_winsys_drm_radeon_destroy,
};

struct terakan_winsys *
terakan_winsys_drm_radeon_create(int const fd)
{
   /* Check if the kernel driver version is supported before doing any DRM queries.
    * Same DRM version requirement as in the Gallium Radeon winsys as of May 2023.
    */
   {
      drmVersionPtr const drm_version = drmGetVersion(fd);
      if (drm_version == NULL) {
         fputs("terakan/drm_radeon: Failed to get the kernel driver version for the DRM device.",
               stderr);
         return NULL;
      }
      if (drm_version->version_major != 2 || drm_version->version_minor < 50) {
         fprintf(
            stderr,
            "terakan/drm_radeon: DRM version is %d.%d.%d, but this driver is only compatible with "
            "2.50.0 (kernel 4.12) or later.\n",
            drm_version->version_major, drm_version->version_minor,
            drm_version->version_patchlevel);
         drmFreeVersion(drm_version);
         return NULL;
      }
      drmFreeVersion(drm_version);
   }

   /* Get the PCI device ID. */
   uint32_t pci_id;
   if (!terakan_winsys_drm_radeon_get_drm_value(fd, RADEON_INFO_DEVICE_ID, "PCI device ID",
                                                &pci_id)) {
      return NULL;
   }

   /* Check if the device is supported, and initialize the new winsys. */

   struct terakan_winsys_drm_radeon * winsys = MALLOC_STRUCT(terakan_winsys_drm_radeon);
   if (winsys == NULL) {
      fputs("terakan/drm_radeon: Failed to allocate memory for the winsys structure.\n", stderr);
      return NULL;
   }
   if (!terakan_gpu_info_init_chip_family(&winsys->base.gpu_info, pci_id)) {
      /* Some other ATI/AMD GPU, not supported by Terakan. */
      goto fail_alloc;
   }
   winsys->fd = fd;

   terakan_winsys_base_init(&winsys->base);

   winsys->base.fn = &terakan_winsys_drm_radeon_fn;
   winsys->base.surface_fn = &terakan_winsys_drm_radeon_surface_fn;
   winsys->base.bo_fn = &terakan_winsys_drm_radeon_bo_fn;
   winsys->base.cs_fn = &terakan_winsys_drm_radeon_cs_fn;

   /* Get tiling configuration. */
   uint32_t tiling_config;
   if (!terakan_winsys_drm_radeon_get_drm_value(fd, RADEON_INFO_TILING_CONFIG,
                                                "tiling configuration", &tiling_config)) {
      goto fail_alloc;
   }
   winsys->base.gpu_info.tile_pipes_log2 = tiling_config & 0xF;
   winsys->base.gpu_info.tile_banks_log2 = 2 + ((tiling_config >> 4) & 0xF);
   winsys->base.gpu_info.tile_pipe_interleave_bytes_log2 = 8 + ((tiling_config >> 8) & 0xF);
   winsys->base.gpu_info.tile_row_bytes_log2 = 10 + ((tiling_config >> 12) & 0xF);

   /* Get memory info. */
   size_t const page_size = (size_t)sysconf(_SC_PAGESIZE);
   /* TTM aligns the BO size to the CPU page size. */
   winsys->base.gpu_info.gart_page_size = page_size;
   {
      struct drm_radeon_gem_info gem_info = {0};
      int const read_gem_info_result =
         drmCommandWriteRead(fd, DRM_RADEON_GEM_INFO, &gem_info, sizeof(gem_info));
      if (read_gem_info_result != 0) {
         fprintf(stderr,
                 "terakan/drm_radeon: Failed to get the memory management info, error number %d.\n",
                 read_gem_info_result);
         goto fail_alloc;
      }
      winsys->base.gpu_info.gart_size = gem_info.gart_size;
      winsys->base.gpu_info.vram_size = gem_info.vram_size;
      winsys->base.gpu_info.vram_visible = gem_info.vram_visible;
   }
   winsys->base.gpu_info.bo_size_granularity = (VkDeviceSize)page_size;
   /* The command buffer parser in the Radeon kernel driver stores 32-bit offsets to bindings within
    * buffer objects, and for the indirect arguments base (SET_BASE packet), it doesn't apply the
    * offset during relocation, so DRAW_INDIRECT and DRAW_INDEX_INDIRECT need a 32-bit BO-relative
    * offset rather than an offset within the Vulkan buffer.
    * Also, it aligns BO sizes (accepted as unsigned long - 32-bit on 32-bit architectures) to the
    * CPU page size.
    */
   winsys->base.gpu_info.max_bo_size = UINT32_MAX & ~(VkDeviceSize)(page_size - 1);
   winsys->base.gpu_info.min_memory_map_alignment = page_size;

   winsys->base.gpu_info.cs_bo_reference_size = sizeof(struct drm_radeon_cs_reloc);
   winsys->base.gpu_info.cs_bo_reference_alignment = alignof(struct drm_radeon_cs_reloc);

   /* Get the timestamp frequency. In case of failure, timestamp queries will be disabled. */
   if (!terakan_winsys_drm_radeon_get_drm_value(fd, RADEON_INFO_CLOCK_CRYSTAL_FREQ, NULL,
                                                &winsys->base.gpu_info.clock_crystal_frequency)) {
      winsys->base.gpu_info.clock_crystal_frequency = 0;
   }

   /* Complete GPU info initialization. */
   terakan_gpu_info_init_complete(&winsys->base.gpu_info);

   /* Initialize the surface manager. */
   winsys->surface_manager = radeon_surface_manager_new(fd);
   if (winsys->surface_manager == NULL) {
      fputs("terakan/drm_radeon: Failed to create the surface manager.\n", stderr);
      goto fail_alloc;
   }

   /* Initialize buffer object sharing. */
   if (mtx_init(&winsys->shared_bo_mutex, mtx_plain) != thrd_success) {
      fputs(
         "terakan/drm_radeon: Failed to initialize the shared buffer reference counting mutex.\n",
         stderr);
      goto fail_surface_manager;
   }
   winsys->shared_bo_reference_counts = _mesa_pointer_hash_table_create(NULL);
   if (winsys->shared_bo_reference_counts == NULL) {
      fputs("terakan/drm_radeon: Failed to create the shared buffer reference count table.\n",
            stderr);
      goto fail_shared_bo_mutex;
   }

   /* Initialize synchronization. */
   size_t sync_type_count = 0;
   assert(sync_type_count < ARRAY_SIZE(winsys->sync_types));
   winsys->sync_types[sync_type_count++] = &terakan_sync_completion_type;
   winsys->sync_type_binary = vk_sync_binary_get_type(&terakan_sync_completion_type);
   assert(sync_type_count < ARRAY_SIZE(winsys->sync_types));
   winsys->sync_types[sync_type_count++] = &winsys->sync_type_binary.sync;
   assert(sync_type_count < ARRAY_SIZE(winsys->sync_types));
   winsys->sync_types[sync_type_count++] = NULL;

   return &winsys->base;

fail_shared_bo_mutex:
   mtx_destroy(&winsys->shared_bo_mutex);
fail_surface_manager:
   radeon_surface_manager_free(winsys->surface_manager);
fail_alloc:
   FREE(winsys);
   return NULL;
}
