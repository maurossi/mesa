#include "priv.h"

int
nouveau_ws_client_new(struct nouveau_ws_device *dev,
                      struct nouveau_ws_client **client)
{
   assert(dev);
   assert(client);

   struct nouveau_ws_client_priv *priv = CALLOC_STRUCT(nouveau_ws_client_priv);
   if (!priv)
      return -ENOMEM;

   priv->base.device = dev;
   *client = &priv->base;
   return 0;
}

void
nouveau_ws_client_del(struct nouveau_ws_client **client)
{
   assert(client);
   if (!*client)
      return;
   FREE(*client);
   *client = NULL;
}
