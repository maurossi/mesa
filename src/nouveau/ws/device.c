#include "priv.h"

#include <nvif/class.h>
#include <nvif/cl0080.h>

int
nouveau_ws_device_new(struct nouveau_ws_drm *drm,
                      struct nouveau_ws_device **dev)
{
   struct nv_device_info_v0 info = {
      .version = 0,
   };
   struct nv_device_v0 v0 = {
      .device = ~0ull,
   };
   int ret;

   struct nouveau_ws_drm_priv *pdrm = drm_priv(drm);
   struct nouveau_ws_device_priv *priv = CALLOC_STRUCT(nouveau_ws_device_priv);
   if (!priv)
      return -ENOMEM;

   struct nouveau_ws_device *ldev = &priv->base;
   priv->base.drm = drm;
   assert(pdrm->nvif);

   ret = nouveau_ws_object_new(drm->object, 0, NV_DEVICE, &v0, sizeof(v0), &priv->base.object);
   if (ret)
      goto err;

   ret = nouveau_ws_object_mthd(priv->base.object, NV_DEVICE_V0_INFO, &info, sizeof(info));
   if (ret)
      goto err;
   priv->base.chipset = info.chipset;

   ret = nouveau_ws_getparam(&priv->base, NOUVEAU_GETPARAM_FB_SIZE, &priv->base.vram_size);
   if (ret)
      goto err;

   *dev = &priv->base;
   return 0;

err:
   nouveau_ws_device_del(&ldev);
   return -1;
}

void
nouveau_ws_device_del(struct nouveau_ws_device **dev)
{
   assert(dev);
   if (!*dev)
      return;
   struct nouveau_ws_device_priv *priv = device_priv(*dev);
   if (priv->base.drm) {
      int fd = priv->base.drm->fd;
      if (fd >= 0)
         drmClose(priv->base.drm->fd);
   }
   nouveau_ws_object_del(&priv->base.object);
   FREE(priv);
   *dev = NULL;
}
