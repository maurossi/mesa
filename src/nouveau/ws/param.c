#include "priv.h"

int
nouveau_ws_getparam(struct nouveau_ws_device *dev,
                    uint64_t param,
                    uint64_t *value)
{
   assert(dev);
   struct nouveau_ws_device_priv *priv = nouveau_ws_device_priv(dev);
   return nouveau_getparam(priv->device, param, value);
}
