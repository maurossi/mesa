#include "priv.h"

#include <util/u_math.h>

#define NOUVEAU_WS_PB_INITIAL_SIZE 0x1000
#define NOUVEAU_WS_PB_BLOCK_SIZE   0x0400

static void
nouveau_ws_pushbuf_kick_notify(struct nouveau_pushbuf *pb)
{
   assert(pb->user_priv);
   struct nouveau_ws_pushbuf *push = pb->user_priv;
   if (push->kick_notify)
      push->kick_notify(push);
}

static int
nouveau_ws_pushbuf_flush(struct nouveau_ws_pushbuf_priv *priv)
{
   uint32_t size = priv->base.cur - priv->buffer;
   struct nouveau_ws_client_priv *pclient = nouveau_ws_client_priv(priv->base.client);
   uint32_t available_space = priv->pushbuf->end - priv->pushbuf->cur;

   if (available_space < size) {
      int ret = nouveau_pushbuf_space(priv->pushbuf, size, 0, 0);
      if (ret)
         return ret;
      available_space = priv->pushbuf->end - priv->pushbuf->cur;
   }

   assert(priv->pushbuf->end - priv->pushbuf->cur >= size);
   memcpy(priv->pushbuf->cur, priv->buffer, size * sizeof(*priv->buffer));
   priv->pushbuf->cur += size;
   priv->base.cur = priv->buffer;
   return 0;
}

int
nouveau_ws_pushbuf_new(struct nouveau_ws_client *client,
                       struct nouveau_ws_object *chan,
                       int nr,
                       uint32_t size,
                       bool immediate,
                       struct nouveau_ws_pushbuf **push)
{
   assert(push);

   struct nouveau_ws_pushbuf_priv *priv = CALLOC_STRUCT(nouveau_ws_pushbuf_priv);
   if (!priv)
      return -1;

   struct nouveau_ws_client_priv *pclient = nouveau_ws_client_priv(client);
   struct nouveau_ws_object_priv *pchan = nouveau_ws_object_priv(chan);

   int ret = nouveau_pushbuf_new(pclient->client, pchan->object, nr, size, immediate, &priv->pushbuf);
   if (ret) {
      FREE(priv);
      return ret;
   }

   priv->pushbuf->user_priv = &priv->base;
   priv->pushbuf->rsvd_kick = 0;
   *push = &priv->base;
   priv->base.client = client;
   priv->base.channel = chan;
   return 0;
}

void
nouveau_ws_pushbuf_del(struct nouveau_ws_pushbuf **push)
{
   assert(push);
   if (!*push)
      return;

   struct nouveau_ws_pushbuf_priv *priv = nouveau_ws_pushbuf_priv(*push);
   *push = NULL;
   FREE(priv->buffer);
   FREE(priv);
}

int
nouveau_ws_pushbuf_space(struct nouveau_ws_pushbuf *pushbuf,
                         uint32_t dwords,
                         uint32_t relocs,
                         uint32_t pushes)
{
   assert(pushbuf);
   assert(!relocs);
   assert(!pushes);
   struct nouveau_ws_pushbuf_priv *priv = nouveau_ws_pushbuf_priv(pushbuf);
   if (priv->size) {
      if (priv->base.end - priv->base.cur >= dwords)
         return 0;
      nouveau_ws_pushbuf_flush(priv);
      if (priv->base.end - priv->base.cur >= dwords)
         return 0;
      uint32_t old_size = priv->size;
      uint32_t new_size = align((old_size + dwords) * 1.25, NOUVEAU_WS_PB_BLOCK_SIZE);
      uint32_t *new_buffer = REALLOC(priv->buffer, old_size * sizeof(*priv->buffer), new_size * sizeof(*priv->buffer));
      if (!new_buffer)
         return -1;
      priv->size = new_size;
      priv->base.cur = new_buffer + (priv->base.cur - priv->buffer);
      priv->base.end = new_buffer + new_size;
      priv->buffer = new_buffer;
   } else {
      uint32_t size = align(MAX2(NOUVEAU_WS_PB_INITIAL_SIZE, dwords), NOUVEAU_WS_PB_BLOCK_SIZE);
      priv->buffer = CALLOC(sizeof(*priv->buffer), size);
      if (!priv->buffer)
         return -1;
      priv->size = size;
      priv->base.cur = priv->buffer;
      priv->base.end = priv->buffer + priv->size;
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
   struct nouveau_pushbuf_refn drm_refn = {
      .bo = nouveau_ws_bo_priv(refn->bo)->bo,
      .flags = refn->flags,
   };
   struct nouveau_ws_pushbuf_priv *priv = nouveau_ws_pushbuf_priv(pushbuf);
   return nouveau_pushbuf_refn(priv->pushbuf, &drm_refn, nr);
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
   assert(push);

   struct nouveau_ws_pushbuf_priv *priv = nouveau_ws_pushbuf_priv(push);
   return nouveau_pushbuf_validate(priv->pushbuf);
}

int
nouveau_ws_pushbuf_kick(struct nouveau_ws_pushbuf *pushbuf,
                        struct nouveau_ws_object *chan)
{
   assert(pushbuf && chan);
   struct nouveau_ws_pushbuf_priv *priv = nouveau_ws_pushbuf_priv(pushbuf);
   struct nouveau_ws_client_priv *pclient = nouveau_ws_client_priv(priv->base.client);
   struct nouveau_ws_object_priv *pchan = nouveau_ws_object_priv(chan);

   int ret = nouveau_ws_pushbuf_flush(priv);
   if (ret)
      return ret;
   priv->pushbuf->kick_notify = nouveau_ws_pushbuf_kick_notify;
   ret = nouveau_pushbuf_kick(priv->pushbuf, pchan->object);
   priv->pushbuf->kick_notify = NULL;
   return ret;
}

struct nouveau_ws_bufctx *
nouveau_ws_pushbuf_bufctx(struct nouveau_ws_pushbuf *push,
                          struct nouveau_ws_bufctx *bctx)
{
   assert(push);
   struct nouveau_ws_pushbuf_priv *ppush = nouveau_ws_pushbuf_priv(push);
   struct nouveau_ws_bufctx_priv *pbctx = nouveau_ws_bufctx_priv(bctx);
   struct nouveau_ws_bufctx *old = push->bufctx;

   nouveau_pushbuf_bufctx(ppush->pushbuf, pbctx ? pbctx->bufctx : NULL);
   push->bufctx = bctx;

   return old;
}
