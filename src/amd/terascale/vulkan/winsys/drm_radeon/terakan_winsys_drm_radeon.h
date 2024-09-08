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

#ifndef TERAKAN_WINSYS_DRM_RADEON_H
#define TERAKAN_WINSYS_DRM_RADEON_H

#include "../terakan_winsys.h"

#include "c11/threads.h"
#include "util/hash_table.h"
#include "vk_sync.h"
#include "vk_sync_binary.h"

#include <stddef.h>
#include <xf86drm.h>
#include <vulkan/vulkan_core.h>
#include <radeon_surface.h>

struct terakan_winsys_drm_radeon_bo {
   struct terakan_winsys_bo base;

   VkDeviceSize size;

   __u32 domains;

   __u32 handle;

   bool handle_shareable;
};

extern struct terakan_winsys_surface_fn const terakan_winsys_drm_radeon_surface_fn;

extern struct terakan_winsys_bo_fn const terakan_winsys_drm_radeon_bo_fn;

extern struct terakan_winsys_cs_fn const terakan_winsys_drm_radeon_cs_fn;

struct terakan_winsys_drm_radeon {
   struct terakan_winsys base;

   /* Not owned by the winsys. */
   int fd;

   struct radeon_surface_manager * surface_manager;

   /* Implementing reference counting for shared BO handles, since drmPrimeFDToHandle returns the
    * same handle when importing the same BO multiple times even with different file descriptors,
    * and GEM has no implicit reference counting for BO handles.
    * Reference count updates must be done atomically with drmPrimeFDToHandle and
    * DRM_IOCTL_GEM_CLOSE using the mutex.
    */
   mtx_t shared_bo_mutex;
   struct hash_table * shared_bo_reference_counts;

   struct vk_sync_binary_type sync_type_binary;
   struct vk_sync_type const * sync_types[3];
};

struct terakan_winsys * terakan_winsys_drm_radeon_create(int fd);

#endif /* TERAKAN_WINSYS_DRM_RADEON_H */
