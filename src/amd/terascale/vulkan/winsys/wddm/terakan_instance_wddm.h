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

#ifndef TERAKAN_INSTANCE_WDDM_H
#define TERAKAN_INSTANCE_WDDM_H

#include "terakan_instance.h"

#include "terakan_wddm_d3dkmthk.h"

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_instance_wddm {
   struct terakan_instance base;

   /* Windows functions not available in Windows Vista are loaded dynamically. */

   HMODULE gdi32_module;

   struct {
      /* Added in Windows 8 and Windows Server 2012. */

      PFND3DKMT_OPENADAPTERFROMLUID d3dkmt_open_adapter_from_luid;
   } dll_fn;
};

VkResult terakan_instance_wddm_create(VkInstanceCreateInfo const * create_info,
                                      VkAllocationCallbacks const * allocator,
                                      struct terakan_instance ** instance_out);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_INSTANCE_WDDM_H */
