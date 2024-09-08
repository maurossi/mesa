/*
 * Copyright © 2024 Vitaliy Triang3l Kuzmin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "terakan_sync_completion.h"

#include "terakan_device.h"

#include "util/macros.h"
#include "util/os_time.h"
#include "util/timespec.h"
#include "util/u_atomic.h"

#include <assert.h>
#include <stdbool.h>
#include <time.h>

static VkResult
terakan_sync_completion_move(UNUSED struct vk_device * const device,
                             struct vk_sync * const dst_base, struct vk_sync * const src_base)
{
   struct terakan_sync_completion * const dst =
      container_of(dst_base, struct terakan_sync_completion, vk);
   struct terakan_sync_completion * const src =
      container_of(src_base, struct terakan_sync_completion, vk);

   dst->pending_value = src->pending_value;
   dst->current_value = src->current_value;

   src->pending_value = 0;
   src->current_value = 0;

   return VK_SUCCESS;
}

static VkResult
terakan_sync_completion_signal(struct vk_device * const device_base,
                               struct vk_sync * const sync_base, uint64_t const value)
{
   struct terakan_device * const device = container_of(device_base, struct terakan_device, vk);
   struct terakan_sync_completion * const sync =
      container_of(sync_base, struct terakan_sync_completion, vk);

   mtx_lock(&device->completion_mutex);
   assert(value < sync->pending_value);
   assert(value > sync->current_value);
   p_atomic_set(&sync->pending_value, value);
   p_atomic_set(&sync->current_value, value);
   mtx_unlock(&device->completion_mutex);
   cnd_broadcast(&device->completion_condition);

   return VK_SUCCESS;
}

static VkResult
terakan_sync_completion_get_value(struct vk_device * const device, struct vk_sync * const sync_base,
                                  uint64_t * const value_out)
{
   struct terakan_sync_completion const * const sync =
      container_of(sync_base, struct terakan_sync_completion, vk);
   *value_out = p_atomic_read(&sync->current_value);
   return VK_SUCCESS;
}

static VkResult
terakan_sync_completion_wait_many(struct vk_device * const device_base, uint32_t const wait_count,
                                  struct vk_sync_wait const * const waits,
                                  enum vk_sync_wait_flags const wait_flags,
                                  uint64_t const abs_timeout_ns)
{
   bool const wait_any = (wait_flags & VK_SYNC_WAIT_ANY) != 0;

   /* Fast path without locking the mutex if the signals have already been performed. */
   {
      uint32_t wait_index;
      for (wait_index = 0; wait_index < wait_count; ++wait_index) {
         struct vk_sync_wait const * const wait = &waits[wait_index];
         struct terakan_sync_completion const * const sync =
            container_of(wait->sync, struct terakan_sync_completion const, vk);
         uint64_t const sync_value = p_atomic_read(
            wait_flags & VK_SYNC_WAIT_PENDING ? &sync->pending_value : &sync->current_value);
         if ((sync_value >= wait->wait_value) == wait_any) {
            /* Awaited if wait-any, timed out if wait-all. */
            break;
         }
      }
      if ((wait_index < wait_count) == wait_any) {
         /* Any awaited if wait-any, or all not timed out if wait-all. */
         return VK_SUCCESS;
      }
   }

   struct terakan_device * const device = container_of(device_base, struct terakan_device, vk);

   mtx_lock(&device->completion_mutex);
   while (true) {
      if (device->completion_lost) {
         mtx_unlock(&device->completion_mutex);
         return VK_ERROR_DEVICE_LOST;
      }

      uint32_t wait_index;
      for (wait_index = 0; wait_index < wait_count; ++wait_index) {
         struct vk_sync_wait const * const wait = &waits[wait_index];
         struct terakan_sync_completion const * const sync =
            container_of(wait->sync, struct terakan_sync_completion const, vk);
         uint64_t const sync_value =
            wait_flags & VK_SYNC_WAIT_PENDING ? sync->pending_value : sync->current_value;
         if ((sync_value >= wait->wait_value) == wait_any) {
            /* Awaited if wait-any, timed out if wait-all. */
            break;
         }
      }
      if ((wait_index < wait_count) == wait_any) {
         /* Any awaited if wait-any, or all not timed out if wait-all. */
         mtx_unlock(&device->completion_mutex);
         return VK_SUCCESS;
      }

      if (abs_timeout_ns == 0) {
         break;
      }
      int condition_wait_result;
      if (abs_timeout_ns == OS_TIMEOUT_INFINITE) {
         condition_wait_result = cnd_wait(&device->completion_condition, &device->completion_mutex);
      } else {
         /* cnd_timedwait uses CLOCK_REALTIME, while abs_timeout_ns is provided for CLOCK_MONOTONIC.
          * Convert from one to the other.
          */
         uint64_t const now_ns = os_time_get_nano();
         if (now_ns > abs_timeout_ns) {
            break;
         }
         uint64_t const rel_timeout_ns = abs_timeout_ns - now_ns;
         struct timespec now_ts;
         if (timespec_get(&now_ts, TIME_UTC) == 0) {
            vk_device_set_lost(
               &device->vk,
               "Failed to get the current time to await the submission condition variable");
            device->completion_lost = true;
            mtx_unlock(&device->completion_mutex);
            cnd_broadcast(&device->completion_condition);
            return VK_ERROR_DEVICE_LOST;
         }
         struct timespec abs_timeout_ts;
         if (timespec_add_nsec(&abs_timeout_ts, &now_ts, rel_timeout_ns)) {
            /* Overflowed, treat a very long wait as infinite. */
            condition_wait_result =
               cnd_wait(&device->completion_condition, &device->completion_mutex);
         } else {
            condition_wait_result = cnd_timedwait(&device->completion_condition,
                                                  &device->completion_mutex, &abs_timeout_ts);
            if (condition_wait_result == thrd_timedout) {
               /* Might have been woken up spuriously by the system time being changed forward.
                * Go to the next iteration, which will check the monotonic clock.
                */
               continue;
            }
         }
      }
      if (condition_wait_result != thrd_success) {
         vk_device_set_lost(&device->vk, "Failed to await the submission condition variable");
         device->completion_lost = true;
         mtx_unlock(&device->completion_mutex);
         cnd_broadcast(&device->completion_condition);
         return VK_ERROR_DEVICE_LOST;
      }
   }
   mtx_unlock(&device->completion_mutex);
   return VK_TIMEOUT;
}

static void
terakan_sync_completion_finish(struct vk_device * const device, struct vk_sync * const sync)
{
}

static VkResult
terakan_sync_completion_init(struct vk_device * const device, struct vk_sync * const sync_base,
                             uint64_t const initial_value)
{
   struct terakan_sync_completion * const sync =
      container_of(sync_base, struct terakan_sync_completion, vk);

   sync->pending_value = initial_value;
   sync->current_value = initial_value;

   return VK_SUCCESS;
}

struct vk_sync_type const terakan_sync_completion_type = {
   .size = sizeof(struct terakan_sync_completion),
   .features = VK_SYNC_FEATURE_TIMELINE | VK_SYNC_FEATURE_GPU_WAIT | VK_SYNC_FEATURE_CPU_WAIT |
               VK_SYNC_FEATURE_CPU_SIGNAL | VK_SYNC_FEATURE_WAIT_ANY | VK_SYNC_FEATURE_WAIT_PENDING,
   .init = terakan_sync_completion_init,
   .finish = terakan_sync_completion_finish,
   .move = terakan_sync_completion_move,
   .signal = terakan_sync_completion_signal,
   .get_value = terakan_sync_completion_get_value,
   .wait_many = terakan_sync_completion_wait_many,
};
