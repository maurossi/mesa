#ifndef NOUVEAU_WINSYS_H
#define NOUVEAU_WINSYS_H

#include <assert.h>

#include "util/timespec.h"

#include "c11/threads.h"

#include <stdint.h>
#include <inttypes.h>

#include "pipe/p_defines.h"

#include "drm-uapi/drm.h"
#include <nouveau.h>
#include "nouveau_screen.h"

#ifndef NV04_PFIFO_MAX_PACKET_LEN
#define NV04_PFIFO_MAX_PACKET_LEN 2047
#endif

#define NOUVEAU_MIN_BUFFER_MAP_ALIGN      64
#define NOUVEAU_MIN_BUFFER_MAP_ALIGN_MASK (NOUVEAU_MIN_BUFFER_MAP_ALIGN - 1)

static inline uint32_t
PUSH_AVAIL(struct nouveau_pushbuf *push)
{
   return push->end - push->cur;
}

static inline bool
PUSH_SPACE(struct nouveau_screen *screen, struct nouveau_pushbuf *push, uint32_t size)
{
   assert(mtx_trylock(&screen->push_lock) == thrd_busy);
   /* Provide a buffer so that fences always have room to be emitted */
   size += 8;
   if (PUSH_AVAIL(push) < size)
      return nouveau_pushbuf_space(push, size, 0, 0) == 0;
   return true;
}

static inline void
PUSH_DATA(struct nouveau_screen *screen, struct nouveau_pushbuf *push, uint32_t data)
{
   assert(mtx_trylock(&screen->push_lock) == thrd_busy);
   *push->cur++ = data;
}

static inline void
PUSH_DATAp(struct nouveau_screen *screen, struct nouveau_pushbuf *push, const void *data, uint32_t size)
{
   assert(mtx_trylock(&screen->push_lock) == thrd_busy);
   memcpy(push->cur, data, size * 4);
   push->cur += size;
}

static inline void
PUSH_DATAb(struct nouveau_screen *screen, struct nouveau_pushbuf *push, const void *data, uint32_t size)
{
   assert(mtx_trylock(&screen->push_lock) == thrd_busy);
   memcpy(push->cur, data, size);
   push->cur += DIV_ROUND_UP(size, 4);
}

static inline void
PUSH_DATAf(struct nouveau_screen *screen, struct nouveau_pushbuf *push, float f)
{
   union { float f; uint32_t i; } u;
   u.f = f;
   PUSH_DATA(screen, push, u.i);
}

static inline void
PUSH_ACQ(struct nouveau_screen *screen, struct nouveau_pushbuf *push)
{
   // TODO: debug only
   struct timespec current = {};
//   clock_gettime(CLOCK_MONOTONIC, &current);
   current.tv_sec = 2;
   int res = mtx_timedlock(&screen->push_lock, &current);
   assert(res != thrd_busy);
}

static inline int
PUSH_KICK(struct nouveau_screen *screen, struct nouveau_pushbuf *push)
{
   assert(mtx_trylock(&screen->push_lock) == thrd_busy);
   return nouveau_pushbuf_kick(push, push->channel);
}

static inline int
PUSH_DONE(struct nouveau_screen *screen, struct nouveau_pushbuf *push)
{
   int res = PUSH_KICK(screen, push);
   assert(mtx_trylock(&screen->push_lock) == thrd_busy);
   mtx_unlock(&screen->push_lock);
   return res;
}

static inline void
PUSH_REL(struct nouveau_screen *screen, struct nouveau_pushbuf *push)
{
   assert(mtx_trylock(&screen->push_lock) == thrd_busy);
   mtx_unlock(&screen->push_lock);
}


#define NOUVEAU_RESOURCE_FLAG_LINEAR   (PIPE_RESOURCE_FLAG_DRV_PRIV << 0)
#define NOUVEAU_RESOURCE_FLAG_DRV_PRIV (PIPE_RESOURCE_FLAG_DRV_PRIV << 1)

static inline uint32_t
nouveau_screen_transfer_flags(unsigned pipe)
{
   uint32_t flags = 0;

   if (!(pipe & PIPE_MAP_UNSYNCHRONIZED)) {
      if (pipe & PIPE_MAP_READ)
         flags |= NOUVEAU_BO_RD;
      if (pipe & PIPE_MAP_WRITE)
         flags |= NOUVEAU_BO_WR;
      if (pipe & PIPE_MAP_DONTBLOCK)
         flags |= NOUVEAU_BO_NOBLOCK;
   }

   return flags;
}

extern struct nouveau_screen *
nv30_screen_create(struct nouveau_device *);

extern struct nouveau_screen *
nv50_screen_create(struct nouveau_device *);

extern struct nouveau_screen *
nvc0_screen_create(struct nouveau_device *);

#endif
