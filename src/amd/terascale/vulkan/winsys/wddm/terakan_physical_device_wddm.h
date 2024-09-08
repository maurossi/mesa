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

#ifndef TERAKAN_PHYSICAL_DEVICE_WDDM_H
#define TERAKAN_PHYSICAL_DEVICE_WDDM_H

#include "terakan_physical_device.h"
#include "terakan_wddm_d3dkmthk.h"

#include "vk_sync.h"
#include "vk_sync_binary.h"

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_physical_device_wddm {
   struct terakan_physical_device base;

   LUID adapter_luid;

   D3DKMT_HANDLE d3dkmt_adapter;

   D3DKMT_ADAPTERADDRESS adapter_address;

   struct vk_sync_binary_type sync_type_binary;
   struct vk_sync_type const * sync_types[3];
};

VkResult terakan_physical_device_wddm_enumerate(struct vk_instance * instance);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_PHYSICAL_DEVICE_WDDM_H */
