/**************************************************************************
 *
 * Copyright 2011 Intel Corporation
 * Copyright 2012 Francisco Jerez
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Kristian Høgsberg <krh@bitplanet.net>
 *    Benjamin Franzke <benjaminfranzke@googlemail.com>
 *
 **************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <xf86drm.h>
#include <unistd.h>
#include <fcntl.h>

#include "loader.h"
#include "target-helpers/drm_helper_public.h"
#include "frontend/drm_driver.h"
#include "pipe_loader_priv.h"

#include "util/os_file.h"
#include "util/u_memory.h"
#include "util/u_dl.h"
#include "util/u_debug.h"

#define DRM_RENDER_NODE_DEV_NAME_FORMAT "%s/renderD%d"
#define DRM_RENDER_NODE_MAX_NODES 63
#define DRM_RENDER_NODE_MIN_MINOR 128
#define DRM_RENDER_NODE_MAX_MINOR (DRM_RENDER_NODE_MIN_MINOR + DRM_RENDER_NODE_MAX_NODES)

struct pipe_loader_drm_device {
   struct pipe_loader_device base;
   const struct drm_driver_descriptor *dd;
#ifndef GALLIUM_STATIC_TARGETS
   struct util_dl_library *lib;
#endif
   int fd;
};

#define pipe_loader_drm_device(dev) ((struct pipe_loader_drm_device *)dev)

static const struct pipe_loader_ops pipe_loader_drm_ops;

#ifdef GALLIUM_STATIC_TARGETS
static const struct drm_driver_descriptor driver_descriptors[] = {
    {
        .driver_name = "i915",
        .create_screen = pipe_i915_create_screen,
    },
    {
        .driver_name = "iris",
        .create_screen = pipe_iris_create_screen,
        .driconf_xml = &iris_driconf_xml,
    },
    {
        .driver_name = "nouveau",
        .create_screen = pipe_nouveau_create_screen,
    },
    {
        .driver_name = "r300",
        .create_screen = pipe_r300_create_screen,
    },
    {
        .driver_name = "r600",
        .create_screen = pipe_r600_create_screen,
    },
    {
        .driver_name = "radeonsi",
        .create_screen = pipe_radeonsi_create_screen,
        .driconf_xml = &radeonsi_driconf_xml,
    },
    {
        .driver_name = "vmwgfx",
        .create_screen = pipe_vmwgfx_create_screen,
    },
    {
        .driver_name = "kgsl",
        .create_screen = pipe_freedreno_create_screen,
    },
    {
        .driver_name = "msm",
        .create_screen = pipe_freedreno_create_screen,
    },
    {
        .driver_name = "virtio_gpu",
        .create_screen = pipe_virgl_create_screen,
        .driconf_xml = &virgl_driconf_xml,
    },
    {
        .driver_name = "v3d",
        .create_screen = pipe_v3d_create_screen,
        .driconf_xml = &v3d_driconf_xml,
    },
    {
        .driver_name = "vc4",
        .create_screen = pipe_vc4_create_screen,
        .driconf_xml = &v3d_driconf_xml,
    },
    {
        .driver_name = "panfrost",
        .create_screen = pipe_panfrost_create_screen,
    },
    {
        .driver_name = "etnaviv",
        .create_screen = pipe_etna_create_screen,
    },
    {
        .driver_name = "tegra",
        .create_screen = pipe_tegra_create_screen,
    },
    {
        .driver_name = "lima",
        .create_screen = pipe_lima_create_screen,
    },
    {
        .driver_name = "zink",
        .create_screen = pipe_zink_create_screen,
    },
};

static const struct drm_driver_descriptor default_driver_descriptor = {
        .driver_name = "kmsro",
        .create_screen = pipe_kmsro_create_screen,
        .driconf_xml = &v3d_driconf_xml,
};

#endif

static const struct drm_driver_descriptor *
get_driver_descriptor(const char *driver_name, struct util_dl_library **plib)
{
#ifdef GALLIUM_STATIC_TARGETS
   for (int i = 0; i < ARRAY_SIZE(driver_descriptors); i++) {
      if (strcmp(driver_descriptors[i].driver_name, driver_name) == 0)
         return &driver_descriptors[i];
   }
   return &default_driver_descriptor;
#else
   *plib = pipe_loader_find_module(driver_name, PIPE_SEARCH_DIR);
   if (!*plib)
      return NULL;

   const struct drm_driver_descriptor *dd =
         (const struct drm_driver_descriptor *)
         util_dl_get_proc_address(*plib, "driver_descriptor");

   /* sanity check on the driver name */
   if (dd && strcmp(dd->driver_name, driver_name) == 0)
      return dd;
#endif

   return NULL;
}

static bool
pipe_loader_drm_probe_fd_nodup(struct pipe_loader_device **dev, int fd)
{
   struct pipe_loader_drm_device *ddev = CALLOC_STRUCT(pipe_loader_drm_device);
   int vendor_id, chip_id;

   if (!ddev)
      return false;

   if (loader_get_pci_id_for_fd(fd, &vendor_id, &chip_id)) {
      ddev->base.type = PIPE_LOADER_DEVICE_PCI;
      ddev->base.u.pci.vendor_id = vendor_id;
      ddev->base.u.pci.chip_id = chip_id;
   } else {
      ddev->base.type = PIPE_LOADER_DEVICE_PLATFORM;
   }
   ddev->base.ops = &pipe_loader_drm_ops;
   ddev->fd = fd;

   ddev->base.driver_name = loader_get_driver_for_fd(fd);
   if (!ddev->base.driver_name)
      goto fail;

   /* For the closed source AMD OpenGL driver, we want libgbm to load
    * "amdgpu_dri.so", but we want Gallium multimedia drivers to load
    * "radeonsi". So change amdgpu to radeonsi for Gallium.
    */
   if (strcmp(ddev->base.driver_name, "amdgpu") == 0) {
      FREE(ddev->base.driver_name);
      ddev->base.driver_name = strdup("radeonsi");
   }

   struct util_dl_library **plib = NULL;
#ifndef GALLIUM_STATIC_TARGETS
   plib = &ddev->lib;
#endif
   ddev->dd = get_driver_descriptor(ddev->base.driver_name, plib);

   /* kmsro supports lots of drivers, try as a fallback */
   if (!ddev->dd)
      ddev->dd = get_driver_descriptor("kmsro", plib);

   if (!ddev->dd)
      goto fail;

   *dev = &ddev->base;
   return true;

  fail:
#ifndef GALLIUM_STATIC_TARGETS
   if (ddev->lib)
      util_dl_close(ddev->lib);
#endif
   FREE(ddev->base.driver_name);
   FREE(ddev);
   return false;
}

bool
pipe_loader_drm_probe_fd(struct pipe_loader_device **dev, int fd)
{
   bool ret;
   int new_fd;

   if (fd < 0 || (new_fd = os_dupfd_cloexec(fd)) < 0)
     return false;

   ret = pipe_loader_drm_probe_fd_nodup(dev, new_fd);
   if (!ret)
      close(new_fd);

   return ret;
}

static int
open_drm_render_node_minor(int minor)
{
   char path[PATH_MAX];
   snprintf(path, sizeof(path), DRM_RENDER_NODE_DEV_NAME_FORMAT, DRM_DIR_NAME,
            minor);
   return loader_open_device(path);
}

int
pipe_loader_drm_probe(struct pipe_loader_device **devs, int ndev)
{
   int i, j, fd;

   for (i = DRM_RENDER_NODE_MIN_MINOR, j = 0;
        i <= DRM_RENDER_NODE_MAX_MINOR; i++) {
      struct pipe_loader_device *dev;

      fd = open_drm_render_node_minor(i);
      if (fd < 0)
         continue;

      if (!pipe_loader_drm_probe_fd_nodup(&dev, fd)) {
         close(fd);
         continue;
      }

      if (j < ndev) {
         devs[j] = dev;
      } else {
         close(fd);
         dev->ops->release(&dev);
      }
      j++;
   }

   return j;
}

static void
pipe_loader_drm_release(struct pipe_loader_device **dev)
{
   struct pipe_loader_drm_device *ddev = pipe_loader_drm_device(*dev);

#ifndef GALLIUM_STATIC_TARGETS
   if (ddev->lib)
      util_dl_close(ddev->lib);
#endif

   close(ddev->fd);
   FREE(ddev->base.driver_name);
   pipe_loader_base_release(dev);
}

static const char *
pipe_loader_drm_get_driconf_xml(struct pipe_loader_device *dev)
{
   struct pipe_loader_drm_device *ddev = pipe_loader_drm_device(dev);

   if (!ddev->dd->driconf_xml)
      return NULL;

   return *ddev->dd->driconf_xml;
}

static struct pipe_screen *
pipe_loader_drm_create_screen(struct pipe_loader_device *dev,
                              const struct pipe_screen_config *config)
{
   struct pipe_loader_drm_device *ddev = pipe_loader_drm_device(dev);

   return ddev->dd->create_screen(ddev->fd, config);
}

char *
pipe_loader_drm_get_driinfo_xml(const char *driver_name)
{
   char *xml = NULL;
   struct util_dl_library *lib = NULL;
   const struct drm_driver_descriptor *dd =
      get_driver_descriptor(driver_name, &lib);

   if (dd && dd->driconf_xml && *dd->driconf_xml)
      xml = strdup(*dd->driconf_xml);

   if (lib)
      util_dl_close(lib);
   return xml;
}

static const struct pipe_loader_ops pipe_loader_drm_ops = {
   .create_screen = pipe_loader_drm_create_screen,
   .get_driconf_xml = pipe_loader_drm_get_driconf_xml,
   .release = pipe_loader_drm_release
};

PUBLIC struct pipe_screen *load_pipe_screen(struct pipe_loader_device **dev, int fd)
{
   struct pipe_screen *pscreen = NULL;
   if (pipe_loader_drm_probe_fd(dev, fd)) {
      pscreen = pipe_loader_create_screen(*dev);
   }
   return pscreen;
}
