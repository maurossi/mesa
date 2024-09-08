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

#ifndef TERAKAN_QUEUE_DRM_RADEON_H
#define TERAKAN_QUEUE_DRM_RADEON_H

#include "terakan_bo_drm_radeon.h"
#include "terakan_queue.h"

#include <radeon_drm.h>

#ifdef __cplusplus
extern "C" {
#endif

struct terakan_device_drm_radeon;

struct terakan_queue_submission_context_drm_radeon {
   struct terakan_queue_submission_context base;

   struct terakan_device_drm_radeon const * device;

   /* RADEON_CS_RING. */
   __u32 ring;
};

struct terakan_queue_completion_submission_drm_radeon {
   struct terakan_queue_completion_submission base;

   struct terakan_bo_drm_radeon * bo;
};

extern struct terakan_queue_winsys_fn const terakan_queue_drm_radeon_fn;

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_QUEUE_DRM_RADEON_H */
