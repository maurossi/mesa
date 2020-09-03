/**********************************************************
 * Copyright 2009-2015 VMware, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************/


#include "vmw_screen.h"
#include "vmw_fence.h"
#include "vmw_context.h"

#include "util/os_file.h"
#include "util/u_memory.h"
#include "util/u_pointer.h"
#include "pipe/p_compiler.h"
#include "util/u_hash_table.h"
#ifdef MAJOR_IN_MKDEV
#include <sys/mkdev.h>
#endif
#ifdef MAJOR_IN_SYSMACROS
#include <sys/sysmacros.h>
#endif
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

static struct hash_table *fd_hash = NULL;
static mtx_t fd_hash_mutex = _MTX_INITIALIZER_NP;

/* Called from vmw_drm_create_screen(), creates and initializes the
 * vmw_winsys_screen structure, which is the main entity in this
 * module.
 * First, check whether a vmw_winsys_screen object already exists for
 * this device node, and in that case return that one, making sure that we
 * have our own file descriptor open to DRM.
 */

struct vmw_winsys_screen *
vmw_winsys_create( int fd )
{
   struct vmw_winsys_screen *vws;
   const char *getenv_val;

   mtx_lock(&fd_hash_mutex);
   if (fd_hash == NULL) {
      fd_hash = util_hash_table_create_fd_keys();
      if (fd_hash == NULL)
         goto out_no_vws;
   }

   vws = util_hash_table_get(fd_hash, intptr_to_pointer(fd));
   if (vws) {
      vws->open_count++;
      goto out_unlock;
   }

   vws = CALLOC_STRUCT(vmw_winsys_screen);
   if (!vws)
      goto out_no_vws;

   vws->open_count = 1;
   vws->ioctl.drm_fd = os_dupfd_cloexec(fd);
   vws->force_coherent = FALSE;
   if (!vmw_ioctl_init(vws))
      goto out_no_ioctl;

   vws->base.have_gb_dma = !vws->force_coherent;
   vws->base.need_to_rebind_resources = FALSE;
   vws->base.have_transfer_from_buffer_cmd = vws->base.have_vgpu10;
   vws->base.have_constant_buffer_offset_cmd = FALSE;
   getenv_val = getenv("SVGA_FORCE_KERNEL_UNMAPS");
   vws->cache_maps = !getenv_val || strcmp(getenv_val, "0") == 0;
   vws->fence_ops = vmw_fence_ops_create(vws);
   if (!vws->fence_ops)
      goto out_no_fence_ops;

   if(!vmw_pools_init(vws))
      goto out_no_pools;

   if (!vmw_winsys_screen_init_svga(vws))
      goto out_no_svga;

   _mesa_hash_table_insert(fd_hash, intptr_to_pointer(vws->ioctl.drm_fd), vws);

   cnd_init(&vws->cs_cond);
   mtx_init(&vws->cs_mutex, mtx_plain);

out_unlock:
   mtx_unlock(&fd_hash_mutex);

   return vws;
out_no_svga:
   vmw_pools_cleanup(vws);
out_no_pools:
   vws->fence_ops->destroy(vws->fence_ops);
out_no_fence_ops:
   vmw_ioctl_cleanup(vws);
out_no_ioctl:
   close(vws->ioctl.drm_fd);
   FREE(vws);
out_no_vws:
   mtx_unlock(&fd_hash_mutex);
   return NULL;
}

void
vmw_winsys_destroy(struct vmw_winsys_screen *vws)
{
   mtx_lock(&fd_hash_mutex);

   if (--vws->open_count == 0) {
      _mesa_hash_table_remove_key(fd_hash, intptr_to_pointer(vws->ioctl.drm_fd));
      if (_mesa_hash_table_num_entries(fd_hash) == 0) {
         _mesa_hash_table_destroy(fd_hash, NULL);
         fd_hash = NULL;
      }
      vmw_pools_cleanup(vws);
      vws->fence_ops->destroy(vws->fence_ops);
      vmw_ioctl_cleanup(vws);
      close(vws->ioctl.drm_fd);
      mtx_destroy(&vws->cs_mutex);
      cnd_destroy(&vws->cs_cond);
      FREE(vws);
   }

   mtx_unlock(&fd_hash_mutex);
}
