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

#ifndef TERAKAN_DESCRIPTOR_SET_H
#define TERAKAN_DESCRIPTOR_SET_H

#include "terakan_bo.h"
#include "terakan_descriptor.h"
#include "terakan_descriptor_set_layout.h"
#include "terakan_sampler.h"

#include "util/macros.h"
#include "util/u_math.h"
#include "vk_object.h"

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_descriptor_set_resource {
   struct terakan_bo const * bo;
   uint32_t resource[8];
};

struct terakan_descriptor_set_sampler {
   struct terakan_sampler const * sampler;
};

struct terakan_descriptor_set_rat {
   struct terakan_bo const * bo;
   struct terakan_color_descriptor color;
};

#define TERAKAN_DESCRIPTOR_SET_DESCRIPTOR_ALIGNMENT                                                \
   MAX3(alignof(struct terakan_descriptor_set_resource),                                           \
        alignof(struct terakan_descriptor_set_sampler),                                            \
        alignof(struct terakan_descriptor_set_rat))

static_assert(
   (sizeof(struct terakan_descriptor_set_resource) % TERAKAN_DESCRIPTOR_SET_DESCRIPTOR_ALIGNMENT) ==
         0 &&
      (sizeof(struct terakan_descriptor_set_sampler) %
       TERAKAN_DESCRIPTOR_SET_DESCRIPTOR_ALIGNMENT) == 0 &&
      (sizeof(struct terakan_descriptor_set_rat) % TERAKAN_DESCRIPTOR_SET_DESCRIPTOR_ALIGNMENT) ==
         0,
   "Assuming that descriptors of different types can be tightly packed in descriptor sets "
   "arbitrarily, and placing descriptors of any different types next to each other won't cause "
   "them to become misaligned. If this expectation stops being true (for instance, if 3-dword "
   "sampler descriptors start being used instead of pointers, and thus their size stops being a "
   "multiple of the BO pointer size on architectures with 64-bit pointers), wrap the descriptor "
   "structures in #pragma pack(pop, 4).");

struct terakan_descriptor_set {
   struct vk_object_base base;

   /* The part readable by writing and binding. */

   struct terakan_descriptor_set_layout * layout;

   char * descriptors;

   /* The part accessed exclusively by allocation or freeing. */

   /* UINT32_MAX if this set is the first or the last in the list it is in. */
   uint32_t pool_prev, pool_next;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_descriptor_set, base, VkDescriptorSet,
                               VK_OBJECT_TYPE_DESCRIPTOR_SET)

struct terakan_descriptor_pool {
   struct vk_object_base base;

   size_t descriptor_memory_size;
   char * descriptor_memory;

   struct terakan_descriptor_set * sets;
   uint32_t max_sets;

   uint32_t sets_allocated;
   /* Ordered by the location in the descriptor memory. Sets with zero descriptors are included.
    * Both are UINT32_MAX if no allocated sets.
    */
   uint32_t allocated_sets_head, allocated_sets_tail;

   uint32_t sets_freed;
   uint32_t freed_sets_head;
};

VK_DEFINE_NONDISP_HANDLE_CASTS(terakan_descriptor_pool, base, VkDescriptorPool,
                               VK_OBJECT_TYPE_DESCRIPTOR_POOL)

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_DESCRIPTOR_SET_H */
