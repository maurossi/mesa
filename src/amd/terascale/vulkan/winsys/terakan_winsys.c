/*
 * Copyright © 2023 Vitaliy Triang3l Kuzmin
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

#include "terakan_winsys.h"

#include "util/u_atomic.h"

#include <stddef.h>

void
terakan_winsys_base_init(struct terakan_winsys * const winsys)
{
   winsys->last_bo_creation_number = 0;
}

void *
terakan_winsys_bo_map(struct terakan_winsys_bo * const bo)
{
   if (bo->mapping == NULL) {
      bo->mapping = bo->winsys->bo_fn->map_impl(bo);
   }
   return bo->mapping;
}

void
terakan_winsys_bo_unmap(struct terakan_winsys_bo * const bo)
{
   if (bo->mapping == NULL) {
      return;
   }
   bo->winsys->bo_fn->unmap_impl(bo);
   bo->mapping = NULL;
}

void
terakan_winsys_bo_free(struct terakan_winsys_bo * const bo)
{
   terakan_winsys_bo_unmap(bo);
   bo->winsys->bo_fn->free_impl(bo);
}

void
terakan_winsys_bo_base_init(struct terakan_winsys_bo * const bo,
                            struct terakan_winsys * const winsys)
{
   bo->winsys = winsys;

   bo->creation_number = p_atomic_inc_return(&winsys->last_bo_creation_number);

   bo->mapping = NULL;
}
