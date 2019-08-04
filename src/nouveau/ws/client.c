#include "priv.h"

int
nouveau_ws_client_new(struct nouveau_ws_device *dev,
                      struct nouveau_ws_client **client)
{
   assert(client);

   struct nouveau_ws_client_priv *priv = CALLOC_STRUCT(nouveau_ws_client_priv);
   if (!priv)
      return -1;

   struct nouveau_ws_device_priv *pdev = nouveau_ws_device_priv(dev);
   int ret = nouveau_client_new(pdev->device, &priv->client);
   if (ret) {
      FREE(priv);
      return ret;
   }
   *client = &priv->base;
   priv->base.device = dev;
   return 0;
}

void
nouveau_ws_client_del(struct nouveau_ws_client **client)
{
   assert(client);
   if (!*client)
      return;

   struct nouveau_ws_client_priv *priv = nouveau_ws_client_priv(*client);
   nouveau_client_del(&priv->client);
   *client = NULL;
   FREE(priv);
}
