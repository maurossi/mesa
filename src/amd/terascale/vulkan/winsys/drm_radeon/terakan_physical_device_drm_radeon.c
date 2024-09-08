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

#include "terakan_physical_device_drm_radeon.h"

#include "terakan_device_drm_radeon.h"
#include "terakan_instance.h"
#include "terakan_sync_completion.h"

#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <radeon_drm.h>

#ifdef MAJOR_IN_MKDEV
#include <sys/mkdev.h>
#endif
#ifdef MAJOR_IN_SYSMACROS
#include <sys/sysmacros.h>
#endif

static void
terakan_physical_device_drm_radeon_get_winsys_extensions(
   struct terakan_physical_device const * const device_base,
   struct vk_device_extension_table * const extensions, UNUSED struct vk_features * const features,
   struct vk_properties * const properties)
{
   struct terakan_physical_device_drm_radeon * const device =
      container_of(device_base, struct terakan_physical_device_drm_radeon, base);

   /* VK_KHR_external_memory_capabilities (#72, Vulkan 1.1, instance). */
   /* Same as ac_compute_device_uuid as of July 2023. */
   uint32_t const device_uuid_u32[] = {
      device->pci_bus_info.domain,
      device->pci_bus_info.bus,
      device->pci_bus_info.dev,
      device->pci_bus_info.func,
   };
   static_assert(sizeof(device_uuid_u32) <= sizeof(properties->deviceUUID),
                 "Computed device UUID must fit into the Vulkan UUID field.");
   memcpy(properties->deviceUUID, device_uuid_u32, sizeof(device_uuid_u32));

   /* VK_KHR_external_memory_fd (#75). */
   extensions->KHR_external_memory_fd = true;

   /* VK_EXT_external_memory_dma_buf (#126). */
   extensions->EXT_external_memory_dma_buf = true;

   /* VK_EXT_pci_bus_info (#213). */
   extensions->EXT_pci_bus_info = true;
   properties->pciDomain = device->pci_bus_info.domain;
   properties->pciBus = device->pci_bus_info.bus;
   properties->pciDevice = device->pci_bus_info.dev;
   properties->pciFunction = device->pci_bus_info.func;

   /* VK_EXT_physical_device_drm (#354). */
   extensions->EXT_physical_device_drm = true;
   if (device->has_primary_node) {
      properties->drmHasPrimary = VK_TRUE;
      properties->drmPrimaryMajor = device->primary_node_device_id_major;
      properties->drmPrimaryMinor = device->primary_node_device_id_minor;
   }
   properties->drmHasRender = VK_TRUE;
   properties->drmRenderMajor = device->render_node_device_id_major;
   properties->drmRenderMinor = device->render_node_device_id_minor;
}

static void
terakan_physical_device_drm_radeon_destroy(struct terakan_physical_device * const device_base)
{
   struct terakan_physical_device_drm_radeon * const device =
      container_of(device_base, struct terakan_physical_device_drm_radeon, base);

   close(device->render_node_validation_fd);

   vk_free(&device->base.vk.instance->alloc, device->render_node_path);

   terakan_physical_device_finish(&device->base);

   vk_free(&device->base.vk.instance->alloc, device);
}

static struct terakan_physical_device_winsys_fn const terakan_physical_device_drm_radeon_fn = {
   .get_winsys_extensions = terakan_physical_device_drm_radeon_get_winsys_extensions,
   .create_device = terakan_device_drm_radeon_create,
   .destroy = terakan_physical_device_drm_radeon_destroy,
};

VkResult
terakan_physical_device_drm_radeon_try_create(struct vk_instance * const instance_base,
                                              struct _drmDevice * const drm_device,
                                              struct vk_physical_device ** const device_out)
{
   /* Check if the GPU is supported. */
   if (!(drm_device->available_nodes & (1 << DRM_NODE_RENDER)) ||
       drm_device->bustype != DRM_BUS_PCI ||
       drm_device->deviceinfo.pci->vendor_id != TERAKAN_PHYSICAL_DEVICE_VENDOR_ID_ATI ||
       !terakan_physical_device_is_chip_family_supported(
          terakan_physical_device_get_chip_family(drm_device->deviceinfo.pci->device_id))) {
      return VK_ERROR_INCOMPATIBLE_DRIVER;
   }

   VkResult result;

   struct terakan_instance * const instance =
      container_of(instance_base, struct terakan_instance, vk);

   char const * const render_node_path = drm_device->nodes[DRM_NODE_RENDER];
   int render_node_fd = open(render_node_path, O_RDWR | O_CLOEXEC);
   if (render_node_fd < 0) {
      return vk_errorf(instance,
                       errno == ENOMEM ? VK_ERROR_OUT_OF_HOST_MEMORY : VK_ERROR_INCOMPATIBLE_DRIVER,
                       "Failed to open the DRM device '%s': %m", render_node_path);
   }

   /* Check if the kernel driver is supported. */

   {
      drmVersionPtr const drm_version = drmGetVersion(render_node_fd);
      if (drm_version == NULL) {
         result = vk_errorf(instance, VK_ERROR_INCOMPATIBLE_DRIVER,
                            "Failed to get the kernel driver version for the DRM device '%s': %m",
                            render_node_path);
         goto fail_render_node_fd;
      }
      if (strcmp(drm_version->name, "radeon") != 0) {
         result =
            vk_errorf(instance, VK_ERROR_INCOMPATIBLE_DRIVER,
                      "The DRM device '%s' doesn't use the Radeon kernel driver", render_node_path);
         drmFreeVersion(drm_version);
         goto fail_render_node_fd;
      }
      /* Check if the kernel driver version is supported before doing any DRM queries.
       * Same DRM version requirement as in the Gallium Radeon winsys as of May 2023.
       */
      if (drm_version->version_major != 2 || drm_version->version_minor < 50) {
         result = vk_errorf(
            instance, VK_ERROR_INCOMPATIBLE_DRIVER,
            "Device '%s' DRM version is %d.%d.%d, but this driver is only compatible with 2.50.0 "
            "(kernel 4.12) or later",
            render_node_path, drm_version->version_major, drm_version->version_minor,
            drm_version->version_patchlevel);
         drmFreeVersion(drm_version);
         goto fail_render_node_fd;
      }
      drmFreeVersion(drm_version);
   }

   if (instance->debug_flags & TERAKAN_DEBUG_STARTUP) {
      fprintf(stderr, "terakan: info: Found a compatible DRM device '%s'.\n", render_node_path);
   }

   /* Get the GPU and DRM info from the DRM device. */

   struct stat primary_node_stat = {};
   bool has_primary_node = (drm_device->available_nodes & (1 << DRM_NODE_PRIMARY)) != 0;
   if (has_primary_node && stat(drm_device->nodes[DRM_NODE_PRIMARY], &primary_node_stat) != 0) {
      vk_logw(VK_LOG_OBJS(instance), "Failed to stat the DRM primary node '%s': %m",
              drm_device->nodes[DRM_NODE_PRIMARY]);
      has_primary_node = false;
   }
   struct stat render_node_stat = {};
   if (stat(render_node_path, &render_node_stat) != 0) {
      result = vk_errorf(instance, VK_ERROR_INCOMPATIBLE_DRIVER,
                         "Failed to stat the DRM render node '%s': %m", render_node_path);
      goto fail_render_node_fd;
   }

   struct drm_radeon_gem_info gem_info = {};
   int const gem_info_result =
      drmCommandWriteRead(render_node_fd, DRM_RADEON_GEM_INFO, &gem_info, sizeof(gem_info));
   if (gem_info_result != 0) {
      result = vk_errorf(instance, VK_ERROR_INCOMPATIBLE_DRIVER,
                         "Failed to get the memory info for the DRM device '%s', error number %d",
                         render_node_path, gem_info_result);
      goto fail_render_node_fd;
   }

   __u32 tiling_config;
   struct drm_radeon_info tiling_config_info_arguments = {
      .request = RADEON_INFO_TILING_CONFIG,
      .value = (__u64)(void *)&tiling_config,
   };
   int const tiling_config_info_result =
      drmCommandWriteRead(render_node_fd, DRM_RADEON_INFO, &tiling_config_info_arguments,
                          sizeof(tiling_config_info_arguments));
   if (tiling_config_info_result != 0) {
      result = vk_errorf(
         instance, VK_ERROR_INCOMPATIBLE_DRIVER,
         "Failed to get the tiling configuration for the DRM device '%s', error number %d",
         render_node_path, tiling_config_info_result);
      goto fail_render_node_fd;
   }
   struct terakan_physical_device_tiling_info const tiling_info = {
      .pipes_log2 = tiling_config & 0xF,
      .banks_log2 = 2 + ((tiling_config >> 4) & 0xF),
      .pipe_interleave_bytes_log2 = 8 + ((tiling_config >> 8) & 0xF),
      .row_bytes_log2 = 10 + ((tiling_config >> 12) & 0xF),
   };

   __u32 clock_crystal_frequency;
   struct drm_radeon_info clock_crystal_frequency_info_arguments = {
      .request = RADEON_INFO_CLOCK_CRYSTAL_FREQ,
      .value = (__u64)(void *)&clock_crystal_frequency,
   };
   if (drmCommandWriteRead(render_node_fd, DRM_RADEON_INFO, &clock_crystal_frequency_info_arguments,
                           sizeof(clock_crystal_frequency_info_arguments)) != 0) {
      /* Disable timestamp queries in case of failure. */
      clock_crystal_frequency = 0;
   }

   /* Initialize the physical device object. */

   struct terakan_physical_device_drm_radeon * const device = vk_alloc(
      &instance->vk.alloc, sizeof(struct terakan_physical_device_drm_radeon),
      alignof(struct terakan_physical_device_drm_radeon), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (device == NULL) {
      result = vk_error(instance, VK_ERROR_OUT_OF_HOST_MEMORY);
      goto fail_render_node_fd;
   }

   device->pci_bus_info = *drm_device->businfo.pci;

   device->has_primary_node = has_primary_node;
   if (has_primary_node) {
      device->primary_node_device_id_major = major(primary_node_stat.st_rdev);
      device->primary_node_device_id_minor = minor(primary_node_stat.st_rdev);
   } else {
      device->primary_node_device_id_major = 0;
      device->primary_node_device_id_minor = 0;
   }

   device->render_node_device_id_major = major(render_node_stat.st_rdev);
   device->render_node_device_id_minor = minor(render_node_stat.st_rdev);

   device->render_node_path = vk_alloc(&instance->vk.alloc, strlen(render_node_path) + 1,
                                       alignof(char), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (device->render_node_path == NULL) {
      result = vk_error(instance, VK_ERROR_OUT_OF_HOST_MEMORY);
      goto fail_alloc;
   }
   strcpy(device->render_node_path, render_node_path);

   device->render_node_validation_fd = render_node_fd;

   size_t sync_type_count = 0;
   assert(sync_type_count < ARRAY_SIZE(device->sync_types));
   device->sync_types[sync_type_count++] = &terakan_sync_completion_type;
   device->sync_type_binary = vk_sync_binary_get_type(&terakan_sync_completion_type);
   assert(sync_type_count < ARRAY_SIZE(device->sync_types));
   device->sync_types[sync_type_count++] = &device->sync_type_binary.sync;
   assert(sync_type_count < ARRAY_SIZE(device->sync_types));
   device->sync_types[sync_type_count++] = NULL;

   /* TTM aligns the BO size to the GPU page size, hence the GTT page size and the granularity of
    * the maximum BO size.
    *
    * BO size is limited by the fact that the command buffer parser in the Radeon kernel driver
    * stores 32-bit offsets to bindings within buffer objects, and for the indirect arguments base
    * (SET_BASE packet), it doesn't apply the offset during relocation, so DRAW_INDIRECT and
    * DRAW_INDEX_INDIRECT need a 32-bit BO-relative offset rather than an offset within the Vulkan
    * buffer.
    */
   VkDeviceSize const page_size = (VkDeviceSize)sysconf(_SC_PAGESIZE);
   result = terakan_physical_device_init(
      &device->base, instance, &terakan_physical_device_drm_radeon_fn,
      drm_device->deviceinfo.pci->device_id, page_size, (VkDeviceSize)gem_info.gart_size,
      (VkDeviceSize)gem_info.vram_size, (VkDeviceSize)gem_info.vram_visible,
      UINT32_MAX & ~(page_size - 1), page_size, &tiling_info, clock_crystal_frequency,
      device->sync_types);
   if (result != VK_SUCCESS) {
      goto fail_render_node_path;
   }

   *device_out = &device->base.vk;
   return VK_SUCCESS;

fail_render_node_path:
   vk_free(&instance->vk.alloc, device->render_node_path);
fail_alloc:
   vk_free(&instance->vk.alloc, device);
fail_render_node_fd:
   close(render_node_fd);
   return result;
}
