#include <errno.h>
#include <stddef.h>

#include <nouveau/nouveau.h>
#include <xf86drm.h>

#include <ws/nouveau.h>

#include "util/list.h"
#include "util/u_memory.h"

#define CREATE_WRAPPER(T, ...)                \
struct nouveau_ws_##T##_priv {                \
   struct nouveau_ws_##T base;                \
   __VA_ARGS__                                \
};                                            \
                                              \
static inline struct nouveau_ws_##T##_priv *  \
T##_priv(struct nouveau_ws_##T *t)            \
{                                             \
   struct nouveau_ws_##T##_priv *priv = NULL; \
   priv = container_of(t, priv, base);        \
   return priv;                               \
}

CREATE_WRAPPER(bo,
   uint64_t map_handle;
   uint32_t refcnt;
   struct nouveau_ws_drm *drm;
)
CREATE_WRAPPER(bufctx)
CREATE_WRAPPER(bufref)
CREATE_WRAPPER(client)
CREATE_WRAPPER(device)
CREATE_WRAPPER(drm,
   bool nvif;
)
CREATE_WRAPPER(object,
   int fd;
   bool abi;
)
CREATE_WRAPPER(pushbuf,
   struct list_head refns; /* nouveau_ws_pusbuf_priv_refn */
   uint32_t *buffer;
)

struct nouveau_ws_pushbuf_priv_refn {
   struct list_head head;
   struct nouveau_ws_pushbuf_refn refn;
};

struct nv_device_v0;

bool
nouveau_ws_abi16_object(struct nouveau_ws_object *,
                        int (**)(struct nouveau_ws_object *));

int
nouveau_ws_object_new_root(struct nouveau_ws_drm *,
                           struct nouveau_ws_object **);
int
nouveau_ws_object_mthd(struct nouveau_ws_object *,
                       uint32_t mthd,
                       void *data,
                       uint32_t size);

#undef CREATE_WRAPPER
