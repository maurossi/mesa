#include "priv.h"

int
nouveau_ws_pushbuf_new(struct nouveau_ws_client *client,
                       struct nouveau_ws_object *chan,
                       int nr,
                       uint32_t size,
                       bool immediate,
                       struct nouveau_ws_pushbuf **push)
{
   assert(false);
   return -1;
}

void
nouveau_ws_pushbuf_del(struct nouveau_ws_pushbuf **push)
{
   assert(false);
}

int
nouveau_ws_pushbuf_space(struct nouveau_ws_pushbuf *pushbuf,
                         uint32_t dwords,
                         uint32_t relocs,
                         uint32_t pushes)
{
   assert(false);
   return -1;
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
   assert(false);
   return -1;
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

int
nouveau_ws_pushbuf_kick(struct nouveau_ws_pushbuf *pushbuf,
                        struct nouveau_ws_object *chan)
{
   assert(false);
   return -1;
}

struct nouveau_ws_bufctx *
nouveau_ws_pushbuf_bufctx(struct nouveau_ws_pushbuf *push,
                          struct nouveau_ws_bufctx *bctx)
{
   assert(false);
   return NULL;
}
