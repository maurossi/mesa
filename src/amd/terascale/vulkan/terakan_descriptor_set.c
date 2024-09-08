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

#include "terakan_descriptor_set.h"

#include "terakan_buffer.h"
#include "terakan_descriptor.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_image.h"
#include "terakan_sampler.h"

#include "gallium/drivers/r600/evergreend.h"
#include "util/compiler.h"
#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

VKAPI_ATTR void VKAPI_CALL
terakan_UpdateDescriptorSets(UNUSED VkDevice const device, uint32_t const descriptorWriteCount,
                             VkWriteDescriptorSet const * const pDescriptorWrites,
                             uint32_t const descriptorCopyCount,
                             VkCopyDescriptorSet const * const pDescriptorCopies)
{
   for (uint32_t descriptor_write_index = 0; descriptor_write_index < descriptorWriteCount;
        ++descriptor_write_index) {
      VkWriteDescriptorSet const * const descriptor_write =
         &pDescriptorWrites[descriptor_write_index];

      struct terakan_descriptor_set const * const dst_set =
         terakan_descriptor_set_from_handle(descriptor_write->dstSet);
      struct terakan_descriptor_set_layout_binding const * const dst_binding =
         &dst_set->layout->bindings[descriptor_write->dstBinding];

      uint32_t const descriptor_count = descriptor_write->descriptorCount;

      struct terakan_descriptor_set_resource * const dst_resources =
         (struct terakan_descriptor_set_resource *)dst_set->descriptors +
         dst_binding->first_set_resource + descriptor_write->dstArrayElement;
      struct terakan_descriptor_set_sampler * const dst_samplers =
         (struct terakan_descriptor_set_sampler *)(dst_set->descriptors +
                                                   dst_set->layout->pool_first_sampler_offset_bytes) +
         dst_binding->first_set_sampler + descriptor_write->dstArrayElement;
      struct terakan_descriptor_set_rat * const dst_rats =
         (struct terakan_descriptor_set_rat *)(dst_set->descriptors +
                                               dst_set->layout->pool_first_rat_offset_bytes) +
         dst_binding->first_set_rat + descriptor_write->dstArrayElement;

      switch (descriptor_write->descriptorType) {
      case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE: {
         for (uint32_t descriptor_index = 0; descriptor_index < descriptor_count;
              ++descriptor_index) {
            struct terakan_descriptor_set_rat * const dst_rat = &dst_rats[descriptor_index];
            struct terakan_image_view const * const image_view = terakan_image_view_from_handle(
               descriptor_write->pImageInfo[descriptor_index].imageView);
            if (image_view != NULL &&
                G_028C70_FORMAT(image_view->color.info) != V_028C70_COLOR_INVALID) {
               dst_rat->bo = image_view->bo;
               memcpy(&dst_rat->color, &image_view->color, sizeof(struct terakan_color_descriptor));
               terakan_color_descriptor_image_view_to_storage_image(&dst_rat->color);
            } else {
               dst_rat->bo = NULL;
            }
         }
      }
         FALLTHROUGH;
      case VK_DESCRIPTOR_TYPE_SAMPLER:
      case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
      case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
      case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: {
         if (descriptor_write->descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER ||
             descriptor_write->descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
            for (uint32_t descriptor_index = 0; descriptor_index < descriptor_count;
                 ++descriptor_index) {
               dst_samplers[descriptor_index].sampler = terakan_sampler_from_handle(
                  descriptor_write->pImageInfo[descriptor_index].sampler);
            }
         }
         if (descriptor_write->descriptorType != VK_DESCRIPTOR_TYPE_SAMPLER) {
            for (uint32_t descriptor_index = 0; descriptor_index < descriptor_count;
                 ++descriptor_index) {
               struct terakan_descriptor_set_resource * const dst_resource =
                  &dst_resources[descriptor_index];
               struct terakan_image_view const * const image_view = terakan_image_view_from_handle(
                  descriptor_write->pImageInfo[descriptor_index].imageView);
               if (image_view != NULL &&
                   G_03001C_TYPE(image_view->resource[7]) == V_03001C_SQ_TEX_VTX_VALID_TEXTURE) {
                  dst_resource->bo = image_view->bo;
                  memcpy(dst_resource->resource, image_view->resource, sizeof(uint32_t) * 8);
               } else {
                  dst_resource->bo = NULL;
               }
            }
         }
      } break;

      case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: {
         for (uint32_t descriptor_index = 0; descriptor_index < descriptor_count;
              ++descriptor_index) {
            struct terakan_descriptor_set_rat * const dst_rat = &dst_rats[descriptor_index];
            struct terakan_buffer_view const * const buffer_view = terakan_buffer_view_from_handle(
               descriptor_write->pTexelBufferView[descriptor_index]);
            if (buffer_view != NULL &&
                G_028C70_FORMAT(buffer_view->color.info) != V_028C70_COLOR_INVALID) {
               dst_rat->bo = buffer_view->bo;
               memcpy(&dst_rat->color, &buffer_view->color,
                      sizeof(struct terakan_color_descriptor));
            } else {
               dst_rat->bo = NULL;
            }
         }
      }
         FALLTHROUGH;
      case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: {
         for (uint32_t descriptor_index = 0; descriptor_index < descriptor_count;
              ++descriptor_index) {
            struct terakan_descriptor_set_resource * const dst_resource =
               &dst_resources[descriptor_index];
            struct terakan_buffer_view const * const buffer_view = terakan_buffer_view_from_handle(
               descriptor_write->pTexelBufferView[descriptor_index]);
            if (buffer_view != NULL &&
                G_03001C_TYPE(buffer_view->resource[7]) == V_03001C_SQ_TEX_VTX_VALID_BUFFER) {
               dst_resource->bo = buffer_view->bo;
               memcpy(dst_resource->resource, buffer_view->resource, sizeof(uint32_t) * 8);
            } else {
               dst_resource->bo = NULL;
            }
         }
      } break;

      case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
      case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: {
         for (uint32_t descriptor_index = 0; descriptor_index < descriptor_count;
              ++descriptor_index) {
            struct terakan_descriptor_set_resource * const dst_resource =
               &dst_resources[descriptor_index];
            dst_resource->bo = terakan_buffer_create_uniform_buffer_descriptor(
               &descriptor_write->pBufferInfo[descriptor_index], dst_resource->resource);
         }
      } break;

      case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
      case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: {
         for (uint32_t descriptor_index = 0; descriptor_index < descriptor_count;
              ++descriptor_index) {
            struct terakan_descriptor_set_resource * const dst_resource =
               &dst_resources[descriptor_index];
            struct terakan_descriptor_set_rat * const dst_rat = &dst_rats[descriptor_index];
            struct terakan_bo const * const bo = terakan_buffer_create_storage_buffer_descriptor(
               &descriptor_write->pBufferInfo[descriptor_index], dst_resource->resource,
               &dst_rat->color);
            dst_resource->bo = bo;
            dst_rat->bo = bo;
         }
      } break;

      default:
         assert(!"Unsupported descriptor type");
      }
   }

   for (uint32_t descriptor_copy_index = 0; descriptor_copy_index < descriptorCopyCount;
        ++descriptor_copy_index) {
      VkCopyDescriptorSet const * const descriptor_copy = &pDescriptorCopies[descriptor_copy_index];
      struct terakan_descriptor_set const * const src_set =
         terakan_descriptor_set_from_handle(descriptor_copy->srcSet);
      struct terakan_descriptor_set_layout_binding const * const src_binding =
         &src_set->layout->bindings[descriptor_copy->srcBinding];
      VkDescriptorType const descriptor_type = src_binding->descriptor_type;

      struct terakan_descriptor_set const * const dst_set =
         terakan_descriptor_set_from_handle(descriptor_copy->srcSet);
      struct terakan_descriptor_set_layout_binding const * const dst_binding =
         &dst_set->layout->bindings[descriptor_copy->dstBinding];

      if (terakan_descriptor_type_has_resource(descriptor_type)) {
         memcpy(dst_set->descriptors +
                   sizeof(struct terakan_descriptor_set_resource) *
                      (dst_binding->first_set_resource + descriptor_copy->dstArrayElement),
                src_set->descriptors +
                   sizeof(struct terakan_descriptor_set_resource) *
                      (src_binding->first_set_resource + descriptor_copy->srcArrayElement),
                sizeof(struct terakan_descriptor_set_resource) * descriptor_copy->descriptorCount);
      }

      if (terakan_descriptor_type_has_sampler(descriptor_type)) {
         memcpy(dst_set->descriptors + dst_set->layout->pool_first_sampler_offset_bytes +
                   sizeof(struct terakan_descriptor_set_sampler) *
                      (dst_binding->first_set_sampler + descriptor_copy->dstArrayElement),
                src_set->descriptors + src_set->layout->pool_first_sampler_offset_bytes +
                   sizeof(struct terakan_descriptor_set_sampler) *
                      (src_binding->first_set_sampler + descriptor_copy->srcArrayElement),
                sizeof(struct terakan_descriptor_set_sampler) * descriptor_copy->descriptorCount);
      }

      if (terakan_descriptor_type_has_rat(descriptor_type)) {
         memcpy(dst_set->descriptors + dst_set->layout->pool_first_rat_offset_bytes +
                   sizeof(struct terakan_descriptor_set_rat) *
                      (dst_binding->first_set_rat + descriptor_copy->dstArrayElement),
                src_set->descriptors + src_set->layout->pool_first_rat_offset_bytes +
                   sizeof(struct terakan_descriptor_set_rat) *
                      (src_binding->first_set_rat + descriptor_copy->srcArrayElement),
                sizeof(struct terakan_descriptor_set_rat) * descriptor_copy->descriptorCount);
      }
   }
}

static void
terakan_descriptor_set_finish(struct terakan_descriptor_set * const set)
{
   vk_descriptor_set_layout_unref(set->base.device, &set->layout->vk);

   vk_object_base_finish(&set->base);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_FreeDescriptorSets(UNUSED VkDevice const device, VkDescriptorPool const descriptorPool,
                           uint32_t const descriptorSetCount,
                           VkDescriptorSet const * const pDescriptorSets)
{
   struct terakan_descriptor_pool * const pool =
      terakan_descriptor_pool_from_handle(descriptorPool);

   pool->sets_allocated -= descriptorSetCount;

   for (uint32_t array_set_index = 0; array_set_index < descriptorSetCount; ++array_set_index) {
      struct terakan_descriptor_set * const set =
         terakan_descriptor_set_from_handle(pDescriptorSets[array_set_index]);

      terakan_descriptor_set_finish(set);

      uint32_t const set_index = set - pool->sets;
      assert(set_index < pool->max_sets);

      if (set->pool_prev != UINT32_MAX) {
         struct terakan_descriptor_set * const set_prev = &pool->sets[set->pool_prev];
         assert(set_prev->pool_next == set_index);
         set_prev->pool_next = set->pool_next;
      } else {
         assert(pool->allocated_sets_head == set_index);
         pool->allocated_sets_head = set->pool_next;
      }
      if (set->pool_next != UINT32_MAX) {
         struct terakan_descriptor_set * const set_next = &pool->sets[set->pool_next];
         assert(set_next->pool_prev == set_index);
         set_next->pool_prev = set->pool_prev;
      } else {
         assert(pool->allocated_sets_tail == set_index);
         pool->allocated_sets_tail = set->pool_prev;
      }

      set->pool_next = pool->freed_sets_head;
      pool->freed_sets_head = set_index;
   }

   pool->sets_freed += descriptorSetCount;

   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_AllocateDescriptorSets(VkDevice const deviceHandle,
                               VkDescriptorSetAllocateInfo const * const pAllocateInfo,
                               VkDescriptorSet * const pDescriptorSets)
{
   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);
   struct terakan_descriptor_pool * const pool =
      terakan_descriptor_pool_from_handle(pAllocateInfo->descriptorPool);
   uint32_t const set_count = pAllocateInfo->descriptorSetCount;

   if (pool->max_sets - pool->sets_allocated < set_count) {
      for (uint32_t null_set_index = 0; null_set_index < set_count; ++null_set_index) {
         pDescriptorSets[null_set_index] = VK_NULL_HANDLE;
      }
      return vk_error(device, VK_ERROR_OUT_OF_POOL_MEMORY);
   }

   size_t descriptor_memory_after_tail_size = pool->descriptor_memory_size;
   if (pool->allocated_sets_tail != UINT32_MAX) {
      struct terakan_descriptor_set const * const allocated_tail_set =
         &pool->sets[pool->allocated_sets_tail];
      descriptor_memory_after_tail_size -= allocated_tail_set->descriptors -
                                           pool->descriptor_memory +
                                           allocated_tail_set->layout->pool_size_bytes;
   }

   uint32_t set_after_hole_index = UINT32_MAX;
   char * hole = NULL;
   size_t hole_size = 0;

   for (uint32_t array_set_index = 0; array_set_index < set_count; ++array_set_index) {
      struct terakan_descriptor_set_layout * const set_layout =
         terakan_descriptor_set_layout_from_handle(pAllocateInfo->pSetLayouts[array_set_index]);
      size_t const set_size = set_layout->pool_size_bytes;

      uint32_t set_index = pool->freed_sets_head;
      if (set_index != UINT32_MAX) {
         assert(pool->sets_freed != 0);
         --pool->sets_freed;
         pool->freed_sets_head = pool->sets[set_index].pool_next;
      } else {
         set_index = pool->sets_allocated + pool->sets_freed;
         assert(set_index < pool->max_sets);
      }
      ++pool->sets_allocated;
      struct terakan_descriptor_set * const set = &pool->sets[set_index];

      /* Try to quickly allocate linearly first, after the last set in the descriptor memory. */
      if (descriptor_memory_after_tail_size >= set_size) {
         set->descriptors = pool->descriptor_memory +
                            (pool->descriptor_memory_size - descriptor_memory_after_tail_size);
         descriptor_memory_after_tail_size -= pool->descriptor_memory_size;
         set->pool_prev = pool->allocated_sets_tail;
         set->pool_next = UINT32_MAX;
         if (pool->allocated_sets_tail != UINT32_MAX) {
            struct terakan_descriptor_set * const allocated_tail_set =
               &pool->sets[pool->allocated_sets_tail];
            assert(allocated_tail_set->pool_next == UINT32_MAX);
            allocated_tail_set->pool_next = set_index;
         } else {
            assert(pool->allocated_sets_head == UINT32_MAX);
            pool->allocated_sets_head = set_index;
         }
         pool->allocated_sets_tail = set_index;
      } else {
         /* Search for a large enough hole starting from where the oldest linearly allocated sets
          * will likely be.
          * If allocating multiple descriptor sets, try reusing the hole found for the previous
          * allocation, it may still have space.
          */
         assert(set_size != 0);
         if (hole_size < set_size) {
            set_after_hole_index = pool->allocated_sets_head;
            hole = pool->descriptor_memory;
            while (set_after_hole_index != UINT32_MAX) {
               struct terakan_descriptor_set const * const set_after_hole =
                  &pool->sets[set_after_hole_index];
               hole_size = set_after_hole->descriptors - hole;
               if (hole_size >= set_size) {
                  break;
               }
               /* This hole is not large enough, try the hole after set_after_hole next time. */
               hole = set_after_hole->descriptors + set_after_hole->layout->pool_size_bytes;
               set_after_hole_index = set_after_hole->pool_next;
            }

            if (set_after_hole_index == UINT32_MAX) {
               /* Failed to either allocate linearly or find a hole. */

               --pool->sets_allocated;
               set->pool_next = pool->freed_sets_head;
               pool->freed_sets_head = set_index;
               ++pool->sets_freed;

               terakan_FreeDescriptorSets(deviceHandle, pAllocateInfo->descriptorPool,
                                          array_set_index, pDescriptorSets);

               for (uint32_t null_set_index = 0; null_set_index < set_count; ++null_set_index) {
                  pDescriptorSets[null_set_index] = VK_NULL_HANDLE;
               }

               return vk_error(device, VK_ERROR_OUT_OF_POOL_MEMORY);
            }
         }

         assert(set_after_hole_index != UINT32_MAX);
         assert(hole != NULL);
         assert(hole_size >= set_size);

         set->descriptors = hole;
         struct terakan_descriptor_set * const set_after_hole = &pool->sets[set_after_hole_index];
         set->pool_prev = set_after_hole->pool_prev;
         set->pool_next = set_after_hole_index;
         set_after_hole->pool_prev = set_index;
         if (set->pool_prev != UINT32_MAX) {
            struct terakan_descriptor_set * const set_prev = &pool->sets[set->pool_prev];
            assert(set_prev->pool_next == set_after_hole_index);
            set_prev->pool_next = set_index;
         } else {
            assert(pool->allocated_sets_head == set_after_hole_index);
            pool->allocated_sets_head = set_index;
         }

         hole_size -= set_size;
         hole += set_size;
      }

      /* Allocated the set successfully, initialize it. */

      vk_object_base_init(&device->vk, &set->base, VK_OBJECT_TYPE_DESCRIPTOR_SET);

      vk_descriptor_set_layout_ref(&set_layout->vk);
      set->layout = set_layout;

      pDescriptorSets[array_set_index] = terakan_descriptor_set_to_handle(set);
   }

   return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_ATTR
terakan_ResetDescriptorPool(UNUSED VkDevice const device, VkDescriptorPool const descriptorPool,
                            UNUSED VkDescriptorPoolResetFlags const flags)
{
   struct terakan_descriptor_pool * const pool =
      terakan_descriptor_pool_from_handle(descriptorPool);

   for (uint32_t set_index = pool->allocated_sets_head; set_index != UINT32_MAX;
        set_index = pool->sets[set_index].pool_next) {
      terakan_descriptor_set_finish(&pool->sets[set_index]);
   }

   pool->sets_allocated = 0;
   pool->allocated_sets_head = UINT32_MAX;
   pool->allocated_sets_tail = UINT32_MAX;

   pool->sets_freed = 0;
   pool->freed_sets_head = UINT32_MAX;

   return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
terakan_DestroyDescriptorPool(VkDevice const deviceHandle, VkDescriptorPool const descriptorPool,
                              VkAllocationCallbacks const * const pAllocator)
{
   struct terakan_descriptor_pool * const pool =
      terakan_descriptor_pool_from_handle(descriptorPool);

   if (pool == NULL) {
      return;
   }

   for (uint32_t set_index = pool->allocated_sets_head; set_index != UINT32_MAX;
        set_index = pool->sets[set_index].pool_next) {
      terakan_descriptor_set_finish(&pool->sets[set_index]);
   }

   struct terakan_device const * const device = terakan_device_from_handle(deviceHandle);

   vk_object_base_finish(&pool->base);

   vk_free2(&device->vk.alloc, pAllocator, pool);
}

VKAPI_ATTR VkResult VKAPI_CALL
terakan_CreateDescriptorPool(VkDevice const deviceHandle,
                             VkDescriptorPoolCreateInfo const * const pCreateInfo,
                             VkAllocationCallbacks const * const pAllocator,
                             VkDescriptorPool * const pDescriptorPool)
{
   struct terakan_device * const device = terakan_device_from_handle(deviceHandle);

   size_t resource_count = 0, sampler_count = 0, rat_count = 0;
   for (uint32_t pool_size_index = 0; pool_size_index < pCreateInfo->poolSizeCount;
        ++pool_size_index) {
      VkDescriptorPoolSize const pool_size = pCreateInfo->pPoolSizes[pool_size_index];
      if (terakan_descriptor_type_has_resource(pool_size.type)) {
         resource_count += pool_size.descriptorCount;
      }
      if (terakan_descriptor_type_has_sampler(pool_size.type)) {
         sampler_count += pool_size.descriptorCount;
      }
      if (terakan_descriptor_type_has_rat(pool_size.type)) {
         rat_count += pool_size.descriptorCount;
      }
   }
   size_t const descriptor_memory_size =
      sizeof(struct terakan_descriptor_set_resource) * resource_count +
      sizeof(struct terakan_descriptor_set_sampler) * sampler_count +
      sizeof(struct terakan_descriptor_set_rat) * rat_count;

   VK_MULTIALLOC(multialloc);
   VK_MULTIALLOC_DECL(&multialloc, struct terakan_descriptor_pool, pool, 1);
   VK_MULTIALLOC_DECL(&multialloc, struct terakan_descriptor_set, sets, pCreateInfo->maxSets);
   void * descriptor_memory;
   vk_multialloc_add_size_align(&multialloc, &descriptor_memory, descriptor_memory_size,
                                TERAKAN_DESCRIPTOR_SET_DESCRIPTOR_ALIGNMENT);
   if (vk_multialloc_alloc2(&multialloc, &device->vk.alloc, pAllocator,
                            VK_SYSTEM_ALLOCATION_SCOPE_OBJECT) == NULL) {
      return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   vk_object_base_init(&device->vk, &pool->base, VK_OBJECT_TYPE_DESCRIPTOR_POOL);

   pool->descriptor_memory_size = descriptor_memory_size;
   pool->descriptor_memory = descriptor_memory;

   pool->sets = sets;
   pool->max_sets = pCreateInfo->maxSets;

   pool->sets_allocated = 0;
   pool->allocated_sets_head = UINT32_MAX;
   pool->allocated_sets_tail = UINT32_MAX;

   pool->sets_freed = 0;
   pool->freed_sets_head = UINT32_MAX;

   *pDescriptorPool = terakan_descriptor_pool_to_handle(pool);
   return VK_SUCCESS;
}
