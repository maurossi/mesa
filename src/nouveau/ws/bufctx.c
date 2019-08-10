#include "priv.h"

int
nouveau_ws_bufctx_new(struct nouveau_ws_client *client,
                      int bins,
                      struct nouveau_ws_bufctx **bctx)
{
   assert(false);
   return -1;
}

void
nouveau_ws_bufctx_del(struct nouveau_ws_bufctx **bctx)
{
   assert(false);
}

struct nouveau_ws_bufref *
nouveau_ws_bufctx_refn(struct nouveau_ws_bufctx *bctx,
                       int bin,
                       struct nouveau_ws_bo *bo,
                       uint32_t flags)
{
   assert(false);
   return NULL;
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
   assert(false);
}
