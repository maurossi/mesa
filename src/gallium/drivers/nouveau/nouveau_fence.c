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
nouveau_fence_new(struct nouveau_screen *screen, struct nouveau_fence **fence)
{
   *fence = CALLOC_STRUCT(nouveau_fence);
   if (!*fence)
      return false;

   (*fence)->screen = screen;
   (*fence)->ref = 1;
   list_inithead(&(*fence)->work);

   return true;
}

static void
nouveau_fence_trigger_work(struct nouveau_fence *fence)
{
   struct nouveau_fence_work *work, *tmp;

   LIST_FOR_EACH_ENTRY_SAFE(work, tmp, &fence->work, list) {
      work->func(work->data);
      list_del(&work->list);
      FREE(work);
   }
}

void
nouveau_fence_emit(struct nouveau_fence *fence)
{
   struct nouveau_screen *screen = fence->screen;
   struct nouveau_fence_list *fence_list = &screen->fence;

   simple_mtx_assert_locked(&fence_list->lock);
   assert(fence->state == NOUVEAU_FENCE_STATE_AVAILABLE);

   /* set this now, so that if fence.emit triggers a flush we don't recurse */
   fence->state = NOUVEAU_FENCE_STATE_EMITTING;

   ++fence->ref;

   if (fence_list->tail)
      fence_list->tail->next = fence;
   else
      fence_list->head = fence;

   fence_list->tail = fence;

   fence_list->emit(&screen->base, &fence->sequence);

   assert(fence->state == NOUVEAU_FENCE_STATE_EMITTING);
   fence->state = NOUVEAU_FENCE_STATE_EMITTED;
}

void
nouveau_fence_del(struct nouveau_fence *fence)
{
   struct nouveau_fence *it;
   struct nouveau_fence_list *fence_list = &fence->screen->fence;

   simple_mtx_assert_locked(&fence_list->lock);

   if (fence->state == NOUVEAU_FENCE_STATE_EMITTED ||
       fence->state == NOUVEAU_FENCE_STATE_FLUSHED) {
      if (fence == fence_list->head) {
         fence_list->head = fence->next;
         if (!fence_list->head)
            fence_list->tail = NULL;
      } else {
         for (it = fence_list->head; it && it->next != fence; it = it->next);
         it->next = fence->next;
         if (fence_list->tail == fence)
            fence_list->tail = it;
      }
   }

   if (!list_is_empty(&fence->work)) {
      debug_printf("WARNING: deleting fence with work still pending !\n");
      nouveau_fence_trigger_work(fence);
   }

   FREE(fence);
}

void
nouveau_fence_cleanup(struct nouveau_fence_list *fence_list)
{
   simple_mtx_assert_locked(&fence_list->lock);

   if (fence_list->current) {
      struct nouveau_fence *current = NULL;

      /* nouveau_fence_wait will create a new current fence, so wait on the
       * _current_ one, and remove both.
       */
      nouveau_fence_ref(fence_list->current, &current);
      nouveau_fence_wait(current, NULL);
      nouveau_fence_ref(NULL, &current);
      nouveau_fence_ref(NULL, &fence_list->current);
   }
}

void
nouveau_fence_update(struct nouveau_screen *screen, bool flushed)
{
   struct nouveau_fence *fence;
   struct nouveau_fence *next = NULL;
   struct nouveau_fence_list *fence_list = &screen->fence;

   simple_mtx_assert_locked(&fence_list->lock);
   u32 sequence = fence_list->update(&screen->base);

   if (fence_list->sequence_ack == sequence)
      return;
   fence_list->sequence_ack = sequence;

   for (fence = fence_list->head; fence; fence = next) {
      next = fence->next;
      sequence = fence->sequence;

      fence->state = NOUVEAU_FENCE_STATE_SIGNALLED;

      nouveau_fence_trigger_work(fence);
      nouveau_fence_ref(NULL, &fence);

      if (sequence == fence_list->sequence_ack)
         break;
   }
   fence_list->head = next;
   if (!next)
      fence_list->tail = NULL;

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
   struct nouveau_screen *screen = fence->screen;
   struct nouveau_fence_list *fence_list = &screen->fence;

   simple_mtx_assert_locked(&fence_list->lock);
   if (fence->state == NOUVEAU_FENCE_STATE_SIGNALLED)
      return true;

   if (fence->state >= NOUVEAU_FENCE_STATE_EMITTED)
      nouveau_fence_update(screen, false);

   return fence->state == NOUVEAU_FENCE_STATE_SIGNALLED;
}

static bool
nouveau_fence_kick(struct nouveau_fence *fence)
{
   struct nouveau_screen *screen = fence->screen;
   struct nouveau_fence_list *fence_list = &screen->fence;
   struct nouveau_pushbuf *push = screen->pushbuf;

   simple_mtx_assert_locked(&fence_list->lock);
   simple_mtx_assert_locked(&pushbuf_data(push)->push_lock);

   /* wtf, someone is waiting on a fence in flush_notify handler? */
   assert(fence->state != NOUVEAU_FENCE_STATE_EMITTING);

   if (fence->state < NOUVEAU_FENCE_STATE_EMITTED) {
      /* sadly kicking a push buffer modifies fence state, so we have to
       * temporarily unlock
       */
      simple_mtx_unlock(&fence_list->lock);
      PUSH_SPACE(push, 8);
      simple_mtx_lock(&fence_list->lock);
      /* The space allocation might trigger a flush, which could emit the
       * current fence. So check again.
       */
      if (fence->state < NOUVEAU_FENCE_STATE_EMITTED)
         nouveau_fence_emit(fence);
   }

   if (fence->state < NOUVEAU_FENCE_STATE_FLUSHED) {
      /* sadly kicking a push buffer modifies fence state, so we have to
       * temporarily unlock
       */
      simple_mtx_unlock(&fence_list->lock);
      bool res = PUSH_KICK(screen->pushbuf);
      simple_mtx_lock(&fence_list->lock);
      if (res)
         return false;
   }

   if (fence == fence_list->current)
      nouveau_fence_next(screen);

   nouveau_fence_update(screen, false);

   return true;
}

bool
nouveau_fence_wait(struct nouveau_fence *fence, struct pipe_debug_callback *debug)
{
   struct nouveau_screen *screen = fence->screen;
   struct nouveau_fence_list *fence_list = &screen->fence;
   uint32_t spins = 0;
   int64_t start = 0;

   simple_mtx_assert_locked(&fence_list->lock);

   if (debug && debug->debug_message)
      start = os_time_get_nano();

   if (!nouveau_fence_kick(fence))
      return false;

   /* waiting on not flushed fences makes no sense */
   assert(fence->state >= NOUVEAU_FENCE_STATE_EMITTED);

   do {
      if (fence->state == NOUVEAU_FENCE_STATE_SIGNALLED) {
         if (debug && debug->debug_message)
            pipe_debug_message(debug, PERF_INFO,
                               "stalled %.3f ms waiting for fence",
                               (os_time_get_nano() - start) / 1000000.f);
         return true;
      }
      if (!spins)
         NOUVEAU_DRV_STAT(screen, any_non_kernel_fence_sync_count, 1);
      spins++;
#ifdef PIPE_OS_UNIX
      if (!(spins % 8)) /* donate a few cycles */
         sched_yield();
#endif

      nouveau_fence_update(screen, false);
   } while (spins < NOUVEAU_FENCE_MAX_SPINS);

   debug_printf("Wait on fence %u (ack = %u, next = %u) timed out !\n",
                fence->sequence,
                fence_list->sequence_ack, fence_list->sequence);

   return false;
}

void
nouveau_fence_next(struct nouveau_screen *screen)
{
   struct nouveau_fence_list *fence_list = &screen->fence;

   simple_mtx_assert_locked(&fence_list->lock);

   if (fence_list->current->state < NOUVEAU_FENCE_STATE_EMITTING) {
      if (fence_list->current->ref > 1)
         nouveau_fence_emit(fence_list->current);
      else
         return;
   }

   nouveau_fence_ref(NULL, &fence_list->current);

   nouveau_fence_new(screen, &fence_list->current);
}

void
nouveau_fence_unref_bo(void *data)
{
   struct nouveau_bo *bo = data;

   nouveau_bo_ref(NULL, &bo);
}

bool
nouveau_fence_work(struct nouveau_fence *fence,
                   void (*func)(void *), void *data)
{
   struct nouveau_fence_work *work;

   if (fence)
      simple_mtx_assert_locked(&fence->screen->fence.lock);
   if (!fence || fence->state == NOUVEAU_FENCE_STATE_SIGNALLED) {
      func(data);
      return true;
   }

   work = CALLOC_STRUCT(nouveau_fence_work);
   if (!work)
      return false;
   work->func = func;
   work->data = data;
   list_add(&work->list, &fence->work);
   p_atomic_inc(&fence->work_count);
   if (fence->work_count > 64)
      nouveau_fence_kick(fence);
   return true;
}

void
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
