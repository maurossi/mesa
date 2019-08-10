#include "priv.h"

#include <nvif/class.h>

static int
nouveau_ws_abi16_chan_nvc0(struct nouveau_ws_object *obj)
{
   assert(obj);

   struct nouveau_ws_object_priv *priv = object_priv(obj);
   struct drm_nouveau_channel_alloc req = {};
   struct nvc0_fifo *nvc0 = obj->data;
   int ret;

   ret = drmCommandWriteRead(priv->fd, DRM_NOUVEAU_CHANNEL_ALLOC, &req, sizeof(req));
   if (ret)
      return ret;

   nvc0->base.channel = req.channel;
   nvc0->base.pushbuf = req.pushbuf_domains;
   nvc0->notify = req.notifier_handle;
   obj->handle = req.channel;
   return 0;
}

bool
nouveau_ws_abi16_object(struct nouveau_ws_object *obj,
                        int (**func)(struct nouveau_ws_object *))
{
   assert(obj);
   assert(obj->parent);

   if (obj->oclass == NOUVEAU_FIFO_CHANNEL_CLASS &&
       obj->parent->oclass == NV_DEVICE) {
      *func = nouveau_ws_abi16_chan_nvc0;
      return true;
   }
   if (obj->parent->oclass == NOUVEAU_FIFO_CHANNEL_CLASS &&
       obj->oclass == NOUVEAU_NOTIFIER_CLASS) {
      assert(false);
      *func = NULL;
      return true;
   }

   *func = NULL;
   return false;
}
