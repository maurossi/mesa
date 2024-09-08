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

#include "terakan_bo.h"

#include "terakan_device.h"

#include "util/u_atomic.h"

#include <stddef.h>

void *
terakan_bo_map(struct terakan_bo * const bo)
{
   if (bo->mapping == NULL) {
      bo->mapping = bo->device->winsys_fn->bo->map_impl(bo);
   }
   return bo->mapping;
}

void
terakan_bo_unmap(struct terakan_bo * const bo)
{
   if (bo->mapping == NULL) {
      return;
   }
   bo->device->winsys_fn->bo->unmap_impl(bo);
   bo->mapping = NULL;
}

void
terakan_bo_free(struct terakan_bo * const bo, VkAllocationCallbacks const * const allocator)
{
   terakan_bo_unmap(bo);
   bo->device->winsys_fn->bo->free_impl(bo, allocator);
}

void
terakan_bo_init(struct terakan_bo * const bo, struct terakan_device * const device)
{
   bo->device = device;

   bo->va = 0;

   bo->creation_number = p_atomic_inc_return(&device->last_bo_creation_number);

   bo->mapping = NULL;
}
