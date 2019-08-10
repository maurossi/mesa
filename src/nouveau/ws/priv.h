#include <stddef.h>

#include <nouveau/nouveau.h>
#include <ws/nouveau.h>

#include "c11/threads.h"
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

CREATE_WRAPPER(bo)
CREATE_WRAPPER(bufctx)
CREATE_WRAPPER(bufref)
CREATE_WRAPPER(client)
CREATE_WRAPPER(device)
CREATE_WRAPPER(drm)
CREATE_WRAPPER(object)
CREATE_WRAPPER(pushbuf)

#undef CREATE_WRAPPER
