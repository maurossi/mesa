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

#ifndef TERAKAN_SYNC_COMPLETION_H
#define TERAKAN_SYNC_COMPLETION_H

#include "vk_sync.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_sync_completion {
   struct vk_sync vk;

   /* Values protected by completion_mutex of the device, with completion_condition broadcast on
    * update, but p_atomic_read can be used to quickly get the current value without locking the
    * mutex if no waiting is needed, thus p_atomic_set must be used for updating.
    */
   uint64_t pending_value;
   uint64_t current_value;
};

extern struct vk_sync_type const terakan_sync_completion_type;

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_SYNC_COMPLETION_H */
