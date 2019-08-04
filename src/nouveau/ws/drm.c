#include "priv.h"

int
nouveau_ws_drm_new(int fd,
                   struct nouveau_ws_drm **drm)
{
   assert(drm);

   struct nouveau_ws_drm_priv *priv = CALLOC_STRUCT(nouveau_ws_drm_priv);
   if (!priv)
      return -1;

   int ret = nouveau_drm_new(fd, &priv->drm);
   if (ret) {
      FREE(priv);
      return ret;
   }

   priv->base.object = nouveau_ws_from_object(&priv->drm->client, NULL);
   priv->base.fd = priv->drm->fd;
   priv->base.version = priv->drm->version;
   *drm = &priv->base;
   return 0;
}

void
nouveau_ws_drm_del(struct nouveau_ws_drm **drm)
{
   assert(drm);

   if (!*drm)
      return;

   struct nouveau_ws_drm_priv *priv = nouveau_ws_drm_priv(*drm);
   nouveau_drm_del(&priv->drm);
   FREE(priv->base.object);
   FREE(priv);
   *drm = NULL;
}
