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

#include "terakan_instance_wddm.h"

#include "terakan_physical_device_wddm.h"

#include "util/macros.h"
#include "vk_alloc.h"
#include "vk_log.h"

#include <stddef.h>
#include <string.h>

#include <windows.h>

static void
terakan_instance_wddm_destroy(struct terakan_instance * const instance_base)
{
   struct terakan_instance_wddm * const instance =
      container_of(instance_base, struct terakan_instance_wddm, base);

   terakan_instance_finish(&instance->base);

   FreeLibrary(instance->gdi32_module);

   vk_free(&instance->base.vk.alloc, instance);
}

VkResult
terakan_instance_wddm_create(VkInstanceCreateInfo const * const create_info,
                             VkAllocationCallbacks const * const allocator,
                             struct terakan_instance ** const instance_out)
{
   VkResult result;

   struct terakan_instance_wddm * const instance =
      vk_alloc(allocator, sizeof(struct terakan_instance_wddm),
               alignof(struct terakan_instance_wddm), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (instance == NULL) {
      return vk_error(NULL, VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   /* Load optional function pointers. */

   memset(&instance->dll_fn, 0, sizeof(instance->dll_fn));

   instance->gdi32_module = LoadLibraryW(L"gdi32.dll");
   if (instance->gdi32_module == NULL) {
      result = vk_errorf(NULL, VK_ERROR_INITIALIZATION_FAILED, "Failed to load gdi32.dll");
      goto fail_alloc;
   }
   instance->dll_fn.d3dkmt_open_adapter_from_luid = (PFND3DKMT_OPENADAPTERFROMLUID)GetProcAddress(
      instance->gdi32_module, "D3DKMTOpenAdapterFromLuid");

   result =
      terakan_instance_init(&instance->base, create_info, terakan_instance_wddm_destroy, allocator);
   if (result != VK_SUCCESS) {
      goto fail_gdi32_module;
   }

   instance->base.vk.physical_devices.enumerate = terakan_physical_device_wddm_enumerate;

   *instance_out = &instance->base;
   return VK_SUCCESS;

fail_gdi32_module:
   FreeLibrary(instance->gdi32_module);
fail_alloc:
   vk_free(allocator, instance);
   return result;
}
