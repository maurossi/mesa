/*
 * Copyright 2010 Christoph Bumiller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "nouveau_screen.h"
#include "nouveau_winsys.h"
#include "nouveau_fence.h"
#include "util/os_time.h"

#ifdef PIPE_OS_UNIX
#include <sched.h>
#endif

bool
nouveau_fence_new(struct nouveau_fence_list *list, struct nouveau_fence **fence)
{
   *fence = CALLOC_STRUCT(nouveau_fence);
   if (!*fence)
      return false;

   (*fence)->list = list;
   (*fence)->ref = 1;
   LIST_INITHEAD(&(*fence)->work);

   return true;
}

static void
nouveau_fence_trigger_work(struct nouveau_fence *fence)
{
   struct nouveau_fence_work *work, *tmp;

   LIST_FOR_EACH_ENTRY_SAFE(work, tmp, &fence->work, list) {
      work->func(work->data);
      LIST_DEL(&work->list);
      FREE(work);
   }
}

void
nouveau_fence_emit(struct nouveau_fence *fence, struct nouveau_pushbuf *push)
{
   struct nouveau_fence_list *list = fence->list;

   assert(fence->state == NOUVEAU_FENCE_STATE_AVAILABLE);

   /* set this now, so that if fence.emit triggers a flush we don't recurse */
   fence->state = NOUVEAU_FENCE_STATE_EMITTING;

   ++fence->ref;

   if (list->tail)
      list->tail->next = fence;
   else
      list->head = fence;

   list->tail = fence;

   list->emit(list, push, &fence->sequence);

   assert(fence->state == NOUVEAU_FENCE_STATE_EMITTING);
   fence->state = NOUVEAU_FENCE_STATE_EMITTED;
}

void
nouveau_fence_del(struct nouveau_fence *fence)
{
   struct nouveau_fence *it;
   struct nouveau_fence_list *list = fence->list;

   if (fence->state == NOUVEAU_FENCE_STATE_EMITTED ||
       fence->state == NOUVEAU_FENCE_STATE_FLUSHED) {
      if (fence == list->head) {
         list->head = fence->next;
         if (!list->head)
            list->tail = NULL;
      } else {
         for (it = list->head; it && it->next != fence; it = it->next);
         it->next = fence->next;
         if (list->tail == fence)
            list->tail = it;
      }
   }

   if (!LIST_IS_EMPTY(&fence->work)) {
      debug_printf("WARNING: deleting fence with work still pending !\n");
      nouveau_fence_trigger_work(fence);
   }

   FREE(fence);
}

void
nouveau_fence_update(struct nouveau_fence_list *list, bool flushed)
{
   struct nouveau_fence *fence;
   struct nouveau_fence *next = NULL;
   u32 sequence = list->update(list);

   if (list->sequence_ack == sequence)
      return;
   list->sequence_ack = sequence;

   for (fence = list->head; fence; fence = next) {
      next = fence->next;
      sequence = fence->sequence;

      fence->state = NOUVEAU_FENCE_STATE_SIGNALLED;

      nouveau_fence_trigger_work(fence);
      nouveau_fence_ref(NULL, &fence);

      if (sequence == list->sequence_ack)
         break;
   }
   list->head = next;
   if (!next)
      list->tail = NULL;

   if (flushed) {
      for (fence = next; fence; fence = fence->next)
         if (fence->state == NOUVEAU_FENCE_STATE_EMITTED)
            fence->state = NOUVEAU_FENCE_STATE_FLUSHED;
   }
}

#define NOUVEAU_FENCE_MAX_SPINS (1 << 31)

bool
nouveau_fence_signalled(struct nouveau_fence *fence)
{
   if (fence->state == NOUVEAU_FENCE_STATE_SIGNALLED)
      return true;

   if (fence->state >= NOUVEAU_FENCE_STATE_EMITTED)
      nouveau_fence_update(fence->list, false);

   return fence->state == NOUVEAU_FENCE_STATE_SIGNALLED;
}

static bool
nouveau_fence_kick(struct nouveau_fence *fence, struct nouveau_pushbuf *push)
{
   /* wtf, someone is waiting on a fence in flush_notify handler? */
   assert(fence->state != NOUVEAU_FENCE_STATE_EMITTING);

   if (fence->state < NOUVEAU_FENCE_STATE_EMITTED) {
      PUSH_SPACE(push, 8);
      /* The space allocation might trigger a flush, which could emit the
       * current fence. So check again.
       */
      if (fence->state < NOUVEAU_FENCE_STATE_EMITTED)
         nouveau_fence_emit(fence, push);
   }

   if (fence->state < NOUVEAU_FENCE_STATE_FLUSHED)
      if (nouveau_pushbuf_kick(push, push->channel))
         return false;

   if (fence == fence->list->current)
      nouveau_fence_next(fence->list, push);

   nouveau_fence_update(fence->list, false);

   return true;
}

bool
nouveau_fence_wait(struct nouveau_fence *fence, struct nouveau_pushbuf *push, struct pipe_debug_callback *debug)
{
   uint32_t spins = 0;
   int64_t start = 0;

   if (debug && debug->debug_message)
      start = os_time_get_nano();

   if (!nouveau_fence_kick(fence, push))
      return false;

   do {
      if (fence->state == NOUVEAU_FENCE_STATE_SIGNALLED) {
         if (debug && debug->debug_message)
            pipe_debug_message(debug, PERF_INFO,
                               "stalled %.3f ms waiting for fence",
                               (os_time_get_nano() - start) / 1000000.f);
         return true;
      }
      if (!spins)
         NOUVEAU_DRV_STAT(fence->list->screen, any_non_kernel_fence_sync_count, 1);
      spins++;
#ifdef PIPE_OS_UNIX
      if (!(spins % 8)) /* donate a few cycles */
         sched_yield();
#endif

      nouveau_fence_update(fence->list, false);
   } while (spins < NOUVEAU_FENCE_MAX_SPINS);

   debug_printf("Wait on fence %u (ack = %u, next = %u) timed out !\n",
                fence->sequence,
                fence->list->sequence_ack, fence->list->sequence);

   return false;
}

void
nouveau_fence_next(struct nouveau_fence_list *list, struct nouveau_pushbuf *push)
{
   if (list->current->state < NOUVEAU_FENCE_STATE_EMITTING) {
      if (list->current->ref > 1)
         nouveau_fence_emit(list->current, push);
      else
         return;
   }

   nouveau_fence_ref(NULL, &list->current);

   nouveau_fence_new(list, &list->current);
}

void
nouveau_fence_unref_bo(void *data)
{
   struct nouveau_bo *bo = data;

   nouveau_bo_ref(NULL, &bo);
}

bool
nouveau_fence_work(struct nouveau_fence *fence,
                   struct nouveau_pushbuf *push,
                   void (*func)(void *), void *data)
{
   struct nouveau_fence_work *work;

   if (!fence || fence->state == NOUVEAU_FENCE_STATE_SIGNALLED) {
      func(data);
      return true;
   }

   work = CALLOC_STRUCT(nouveau_fence_work);
   if (!work)
      return false;
   work->func = func;
   work->data = data;
   LIST_ADD(&work->list, &fence->work);
   p_atomic_inc(&fence->work_count);
   if (fence->work_count > 64)
      nouveau_fence_kick(fence, push);
   return true;
}
