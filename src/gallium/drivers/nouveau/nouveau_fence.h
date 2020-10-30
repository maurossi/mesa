
#ifndef __NOUVEAU_FENCE_H__
#define __NOUVEAU_FENCE_H__

#include "util/u_inlines.h"
#include "util/list.h"

#define NOUVEAU_FENCE_STATE_AVAILABLE 0
#define NOUVEAU_FENCE_STATE_EMITTING  1
#define NOUVEAU_FENCE_STATE_EMITTED   2
#define NOUVEAU_FENCE_STATE_FLUSHED   3
#define NOUVEAU_FENCE_STATE_SIGNALLED 4

struct pipe_debug_callback;

struct nouveau_fence_list;
struct nouveau_pushbuf;
struct nouveau_screen;

struct nouveau_fence_work {
   struct list_head list;
   void (*func)(void *);
   void *data;
};

struct nouveau_fence {
   struct nouveau_fence *next;
   struct nouveau_fence_list *list;
   int state;
   int ref;
   uint32_t sequence;
   uint32_t work_count;
   struct list_head work;
};

struct nouveau_fence_list {
   struct nouveau_fence *head;
   struct nouveau_fence *tail;
   struct nouveau_fence *current;

   struct nouveau_screen *screen;
   struct nouveau_pushbuf *push;
   void *data;

   uint32_t sequence;
   uint32_t sequence_ack;
   void (*emit)(struct nouveau_fence_list *, struct nouveau_pushbuf *, uint32_t *sequence);
   uint32_t  (*update)(struct nouveau_fence_list *);
};

void nouveau_fence_emit(struct nouveau_fence *, struct nouveau_pushbuf *);
void nouveau_fence_del(struct nouveau_fence *);

bool nouveau_fence_new(struct nouveau_fence_list *list, struct nouveau_fence **);
bool nouveau_fence_work(struct nouveau_fence *, struct nouveau_pushbuf *, void (*)(void *), void *);
void nouveau_fence_update(struct nouveau_fence_list *, bool flushed);
void nouveau_fence_next(struct nouveau_fence_list *, struct nouveau_pushbuf *);
bool nouveau_fence_wait(struct nouveau_fence *, struct nouveau_pushbuf *push, struct pipe_debug_callback *);
bool nouveau_fence_signalled(struct nouveau_fence *);

void nouveau_fence_unref_bo(void *data); /* generic unref bo callback */

static inline void
nouveau_fence_list_init(struct nouveau_fence_list *list,
                        struct nouveau_screen *screen,
                        struct nouveau_pushbuf *push)
{
   list->screen = screen;
   list->push = push;
}

static inline void
nouveau_fence_ref(struct nouveau_fence *fence, struct nouveau_fence **ref)
{
   if (fence)
      ++fence->ref;

   if (*ref) {
      if (--(*ref)->ref == 0)
         nouveau_fence_del(*ref);
   }

   *ref = fence;
}

static inline struct nouveau_fence *
nouveau_fence(struct pipe_fence_handle *fence)
{
   return (struct nouveau_fence *)fence;
}

#endif // __NOUVEAU_FENCE_H__
