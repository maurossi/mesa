#include "priv.h"

int
nouveau_ws_bufctx_new(struct nouveau_ws_client *client,
                      int bins,
                      struct nouveau_ws_bufctx **bctx)
{
   assert(client && bctx);

   struct nouveau_ws_bufctx_priv *priv = CALLOC_STRUCT(nouveau_ws_bufctx_priv);
   if (!priv)
      return -1;

   list_inithead(&priv->base.current);
   list_inithead(&priv->base.pending);
   list_inithead(&priv->refs);
   struct nouveau_ws_client_priv *pclient = nouveau_ws_client_priv(client);
   int ret = nouveau_bufctx_new(pclient->client, bins, &priv->bufctx);
   if (ret) {
      FREE(priv);
      return ret;
   }
   *bctx = &priv->base;
   return 0;
}

void
nouveau_ws_bufctx_del(struct nouveau_ws_bufctx **bctx)
{
   assert(bctx);
   if (!*bctx)
      return;

   struct nouveau_ws_bufctx_priv *priv = nouveau_ws_bufctx_priv(*bctx);
   list_for_each_entry_safe(struct nouveau_ws_bufref, ref, &priv->refs, thead) {
      FREE(ref);
   }
   nouveau_bufctx_del(&priv->bufctx);
   *bctx = NULL;
   FREE(priv);
}

struct nouveau_ws_bufref *
nouveau_ws_bufctx_refn(struct nouveau_ws_bufctx *bctx,
                       int bin,
                       struct nouveau_ws_bo *bo,
                       uint32_t flags)
{
   assert(bctx && bo);
   struct nouveau_ws_bufctx_priv *pbctx = nouveau_ws_bufctx_priv(bctx);
   struct nouveau_ws_bo_priv *pbo = nouveau_ws_bo_priv(bo);

   struct nouveau_ws_bufref_priv *priv = CALLOC_STRUCT(nouveau_ws_bufref_priv);
   if (!priv)
      return NULL;

   struct nouveau_bufref *bufref = nouveau_bufctx_refn(pbctx->bufctx, bin, pbo->bo, flags);
   if (!bufref) {
      FREE(priv);
      return NULL;
   }

   priv->bufref = bufref;
   priv->base.flags = bufref->flags;
   list_addtail(&priv->base.thead, &pbctx->refs);
   return &priv->base;
}

struct nouveau_ws_bufref *
nouveau_ws_bufctx_mthd(struct nouveau_ws_bufctx *bctx,
                       int bin,
                       uint32_t packet,
                       struct nouveau_ws_bo *bo,
                       uint64_t data,
                       uint32_t flags,
                       uint32_t vor,
                       uint32_t tor)
{
   assert(false);
   return NULL;
}

void
nouveau_ws_bufctx_reset(struct nouveau_ws_bufctx *bctx,
                        int bin)
{
   assert(bctx);

   struct nouveau_ws_bufctx_priv *priv = nouveau_ws_bufctx_priv(bctx);
   nouveau_bufctx_reset(priv->bufctx, bin);
}
