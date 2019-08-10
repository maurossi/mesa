#include "priv.h"

int
nouveau_ws_drm_new(int fd,
                   struct nouveau_ws_drm **drm)
{
   struct nouveau_ws_drm_priv *priv = CALLOC_STRUCT(nouveau_ws_drm_priv);
   if (!priv)
      return -ENOMEM;

   priv->base.fd = fd;
   int ret = nouveau_ws_object_new_root(&priv->base, &priv->base.object);
   if (ret)
      goto err;

   drmVersionPtr ver = drmGetVersion(fd);
   if (!ver)
      goto err;

   priv->base.version = (ver->version_major << 0x18) |
                        (ver->version_minor << 0x08) |
                         ver->version_patchlevel;
   priv->nvif = priv->base.version >= 0x01000301;
   drmFreeVersion(ver);
   *drm = &priv->base;
   return 0;

err:
   FREE(priv);
   return -1;
}

void
nouveau_ws_drm_del(struct nouveau_ws_drm **drm)
{
   assert(drm);
   struct nouveau_ws_drm_priv *priv = drm_priv(*drm);
   FREE(priv);
   *drm = NULL;
}
