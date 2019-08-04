#include "priv.h"

int
nouveau_ws_device_new(struct nouveau_ws_drm *drm,
                      int32_t oclass,
                      void *data,
                      uint32_t size,
                      struct nouveau_ws_device **dev)
{
   assert(dev && drm);
   struct nouveau_ws_drm_priv *pdrm = nouveau_ws_drm_priv(drm);

   struct nouveau_ws_device_priv *priv = CALLOC_STRUCT(nouveau_ws_device_priv);
   if (!priv)
      return -1;

   int ret = nouveau_device_new(&pdrm->drm->client, oclass, data, size, &priv->device);
   if (ret) {
      FREE(priv);
      return ret;
   }
   priv->base.drm = drm;
   priv->base.object = nouveau_ws_from_object(&priv->device->object, drm->object);
   priv->base.chipset = priv->device->chipset;
   priv->base.vram_size = priv->device->vram_size;
   *dev = &priv->base;
   return 0;
}

void
nouveau_ws_device_del(struct nouveau_ws_device **dev)
{
   assert(dev);
   if (!*dev)
      return;

   struct nouveau_ws_device_priv *priv = nouveau_ws_device_priv(*dev);
   nouveau_device_del(&priv->device);
   *dev = NULL;
   FREE(priv->base.object);
   FREE(priv);
}
