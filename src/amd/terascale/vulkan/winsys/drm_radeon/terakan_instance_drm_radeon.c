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

#include "terakan_instance_drm_radeon.h"

#include "terakan_physical_device_drm_radeon.h"

#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <stddef.h>

static void
terakan_instance_drm_radeon_destroy(struct terakan_instance * const instance_base)
{
   struct terakan_instance_drm_radeon * const instance =
      container_of(instance_base, struct terakan_instance_drm_radeon, base);

   terakan_instance_finish(&instance->base);

   vk_free(&instance->base.vk.alloc, instance);
}

VkResult
terakan_instance_drm_radeon_create(VkInstanceCreateInfo const * const create_info,
                                   VkAllocationCallbacks const * const allocator,
                                   struct terakan_instance ** const instance_out)
{
   VkResult result;

   struct terakan_instance_drm_radeon * const instance =
      vk_alloc(allocator, sizeof(struct terakan_instance_drm_radeon),
               alignof(struct terakan_instance_drm_radeon), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (instance == NULL) {
      return vk_error(NULL, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   result = terakan_instance_init(&instance->base, create_info, terakan_instance_drm_radeon_destroy,
                                  allocator);
   if (result != VK_SUCCESS) {
      vk_free(allocator, instance);
      return result;
   }

   instance->base.vk.physical_devices.try_create_for_drm =
      terakan_physical_device_drm_radeon_try_create;

   *instance_out = &instance->base;
   return VK_SUCCESS;
}
