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

#ifndef TERAKAN_QUEUE_WDDM_H
#define TERAKAN_QUEUE_WDDM_H

#include "terakan_bo_wddm.h"
#include "terakan_queue.h"
#include "terakan_wddm_d3dkmthk.h"

#ifdef __cplusplus
extern "C" {
#endif

/* One allocation list entry with hAllocation = 0 referenced in the header entries of the patch
 * location list.
 */
#define TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_BO_REFERENCES 1
/* 2 header entries. */
#define TERAKAN_QUEUE_WDDM_SUBMISSION_RESERVED_RELOCATIONS 2

struct terakan_device_wddm;

struct terakan_queue_submission_context_wddm {
   struct terakan_queue_submission_context base;

   struct terakan_device_wddm const * device;

   D3DKMT_HANDLE context;

   void * command_buffer;
   D3DDDI_ALLOCATIONLIST * allocation_list;
   D3DDDI_PATCHLOCATIONLIST * patch_location_list;
};

struct terakan_queue_completion_submission_wddm_1_0 {
   struct terakan_queue_completion_submission base;

   struct terakan_bo_wddm * bo;
};

extern struct terakan_queue_winsys_fn const terakan_queue_wddm_fn;

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_QUEUE_WDDM_H */
