#include "priv.h"

#include "util/u_math.h"

int
nouveau_ws_pushbuf_new(struct nouveau_ws_client *client,
                       struct nouveau_ws_object *chan,
                       int nr,
                       uint32_t size,
                       bool immediate,
                       struct nouveau_ws_pushbuf **push)
{
   assert(chan && client && push);

   struct nouveau_ws_object_priv *pchan = object_priv(chan);
   struct drm_nouveau_gem_pushbuf req = {};
   struct nouveau_fifo *fifo = chan->data;
   int ret;

   if (chan->oclass != NOUVEAU_FIFO_CHANNEL_CLASS)
      return -EINVAL;

   struct nouveau_ws_pushbuf_priv *priv = CALLOC_STRUCT(nouveau_ws_pushbuf_priv);
   if (!priv)
      return -ENOMEM;

   req.channel = fifo->channel;
   req.nr_push = 0;
   ret = drmCommandWriteRead(pchan->fd, DRM_NOUVEAU_GEM_PUSHBUF, &req,
                             sizeof(req));
   if (ret)
      return ret;

   list_inithead(&priv->refns);
   priv->base.client = client;
   priv->base.channel = immediate ? chan : NULL;

   *push = &priv->base;
   return 0;
}

void
nouveau_ws_pushbuf_del(struct nouveau_ws_pushbuf **push)
{
   assert(push);
   if (!*push)
      return;
   assert(list_empty(&pushbuf_priv(*push)->refns));
   FREE(*push);
   *push = NULL;
}

int
nouveau_ws_pushbuf_space(struct nouveau_ws_pushbuf *pushbuf,
                         uint32_t dwords,
                         uint32_t relocs,
                         uint32_t pushes)
{
   assert(pushbuf && !relocs && !pushes);

   struct nouveau_ws_pushbuf_priv *priv = pushbuf_priv(pushbuf);

   if (!priv->buffer) {
      uint32_t size = MAX2(dwords, 0x400);
      priv->buffer = CALLOC(size, sizeof(*priv->buffer));
      priv->base.cur = priv->buffer;
      priv->base.end = priv->buffer + size;
   } else {
      uint32_t remaining = priv->base.end - priv->base.cur;
      if (remaining < dwords) {
         uint32_t old_size = priv->base.end - priv->buffer;
         uint32_t new_size = align(old_size + dwords - remaining, 0x400);
         uint32_t *new_buffer = REALLOC(priv->buffer, old_size, new_size);
         if (!new_buffer)
            return -ENOMEM;
         priv->base.cur = new_buffer + (priv->base.end - priv->base.cur);
         priv->base.end = new_buffer + new_size;
         FREE(priv->buffer);
         priv->buffer = new_buffer;
      }
   }

   return 0;
}

void
nouveau_ws_pushbuf_data(struct nouveau_ws_pushbuf *pushbuf,
                        struct nouveau_ws_bo *bo,
                        uint64_t offset,
                        uint64_t length)
{
   assert(false);
}

int
nouveau_ws_pushbuf_refn(struct nouveau_ws_pushbuf *pushbuf,
                        struct nouveau_ws_pushbuf_refn *refn,
                        int nr)
{
   assert(pushbuf && refn);

   for (int i = 0; i < nr; i++) {
      struct nouveau_ws_pushbuf_priv_refn *priv = CALLOC_STRUCT(nouveau_ws_pushbuf_priv_refn);
      priv->refn = refn[i];
      list_add(&priv->head, &pushbuf_priv(pushbuf)->refns);
   }

   return 0;
}

void
nouveau_ws_pushbuf_reloc(struct nouveau_ws_pushbuf *pushbuf,
                         struct nouveau_ws_bo *bo,
                         uint32_t data,
                         uint32_t flags,
                         uint32_t vor,
                         uint32_t tor)
{
   assert(false);
}

int
nouveau_ws_pushbuf_validate(struct nouveau_ws_pushbuf *push)
{
   assert(false);
   return -1;
}

static int
nouveau_ws_pushbuf_submit(struct nouveau_ws_pushbuf *push)
{
   assert(push && push->channel);

   if (push->channel->oclass != NOUVEAU_FIFO_CHANNEL_CLASS)
      return -EINVAL;

   if (push->kick_notify)
      push->kick_notify(push);

   struct nouveau_fifo *fifo = push->channel->data;
   struct drm_nouveau_gem_pushbuf req = {
      .channel = fifo->channel,
      .nr_buffers = 1,
      .buffers = 0, // TODO
      .nr_relocs = 0,
      .relocs = 0,
      .nr_push = 0,
      .push = 0,
   };

   int ret = drmCommandWriteRead(object_priv(push->channel)->fd, DRM_NOUVEAU_GEM_PUSHBUF, &req, sizeof(req));
   assert(!req.suffix0 && !req.suffix1);
   assert(!ret);
   return ret;
}

static int
nouveau_ws_pushbuf_flush(struct nouveau_ws_pushbuf *push)
{
   assert(push && push->channel);
   return nouveau_ws_pushbuf_submit(push);
}

int
nouveau_ws_pushbuf_kick(struct nouveau_ws_pushbuf *pushbuf,
                        struct nouveau_ws_object *chan)
{
   assert(pushbuf && pushbuf->channel);
   return nouveau_ws_pushbuf_flush(pushbuf);
}

struct nouveau_ws_bufctx *
nouveau_ws_pushbuf_bufctx(struct nouveau_ws_pushbuf *push,
                          struct nouveau_ws_bufctx *bctx)
{
   assert(false);
   return NULL;
}
