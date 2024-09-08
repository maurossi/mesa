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

#ifndef TERAKAN_LIMITS_H
#define TERAKAN_LIMITS_H

#include "util/u_math.h"

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TERAKAN_LIMITS_HW_PARAMETER_CACHE_VECTOR_COUNT 32

#define TERAKAN_LIMITS_HW_LDS_SIMD_BANK_COUNT       32
#define TERAKAN_LIMITS_HW_LDS_SIMD_BANK_DWORD_COUNT 256
#define TERAKAN_LIMITS_HW_LDS_SIMD_DWORD_COUNT                                                     \
   (TERAKAN_LIMITS_HW_LDS_SIMD_BANK_DWORD_COUNT * TERAKAN_LIMITS_HW_LDS_SIMD_BANK_COUNT)

#define TERAKAN_LIMITS_HW_COMPUTE_GROUP_SIZE           1024
#define TERAKAN_LIMITS_HW_COMPUTE_GROUPS_PER_DIMENSION UINT16_MAX

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_LIMITS_H */
