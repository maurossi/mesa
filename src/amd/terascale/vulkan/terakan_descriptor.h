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

#ifndef TERAKAN_DESCRIPTOR_H
#define TERAKAN_DESCRIPTOR_H

#include "winsys/terakan_winsys.h"
#include "terakan_limits.h"

#include "gallium/drivers/r600/evergreend.h"

#include <stdint.h>

#define TERAKAN_CONSTANT_CACHE_LINE_BYTES_LOG2          8
#define TERAKAN_CONSTANT_CACHE_LINE_BYTES               (1 << TERAKAN_CONSTANT_CACHE_LINE_BYTES_LOG2)
#define TERAKAN_CONSTANT_CACHE_MAX_LINES_IN_BUFFER_LOG2 8
#define TERAKAN_CONSTANT_CACHE_MAX_LINES_IN_BUFFER                                                 \
   (1 << TERAKAN_CONSTANT_CACHE_MAX_LINES_IN_BUFFER_LOG2)
#define TERAKAN_CONSTANT_CACHE_MAX_BUFFER_SIZE_BYTES_LOG2                                          \
   (TERAKAN_CONSTANT_CACHE_LINE_BYTES_LOG2 + TERAKAN_CONSTANT_CACHE_MAX_LINES_IN_BUFFER_LOG2)
#define TERAKAN_CONSTANT_CACHE_MAX_BUFFER_SIZE_BYTES                                               \
   (1 << TERAKAN_CONSTANT_CACHE_MAX_BUFFER_SIZE_BYTES_LOG2)
#define TERAKAN_CONSTANT_CACHE_BUFFERS_PER_STAGE 16

#define TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE 176
#define TERAKAN_RESOURCE_HW_COUNT_VERTEX        160
#define TERAKAN_RESOURCE_HW_COUNT_FETCH         32

#define TERAKAN_RESOURCE_HW_OFFSET_PS 0
#define TERAKAN_RESOURCE_HW_OFFSET_VSES                                                            \
   (TERAKAN_RESOURCE_HW_OFFSET_PS + TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE)
#define TERAKAN_RESOURCE_HW_OFFSET_GS                                                              \
   (TERAKAN_RESOURCE_HW_OFFSET_VSES + TERAKAN_RESOURCE_HW_COUNT_VERTEX)
#define TERAKAN_RESOURCE_HW_OFFSET_HS                                                              \
   (TERAKAN_RESOURCE_HW_OFFSET_GS + TERAKAN_RESOURCE_HW_COUNT_VERTEX)
#define TERAKAN_RESOURCE_HW_OFFSET_LS                                                              \
   (TERAKAN_RESOURCE_HW_OFFSET_HS + TERAKAN_RESOURCE_HW_COUNT_VERTEX)
#define TERAKAN_RESOURCE_HW_OFFSET_CS                                                              \
   (TERAKAN_RESOURCE_HW_OFFSET_LS + TERAKAN_RESOURCE_HW_COUNT_VERTEX)
#define TERAKAN_RESOURCE_HW_OFFSET_FS                                                              \
   (TERAKAN_RESOURCE_HW_OFFSET_CS + TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE)
#define TERAKAN_RESOURCE_HW_COUNT (TERAKAN_RESOURCE_HW_OFFSET_FS + TERAKAN_RESOURCE_HW_COUNT_FETCH)

#define TERAKAN_SAMPLERS_PER_STAGE 18

/* Dynamically indexable immediate constant arrays in application shader code.
 * Also used for a single resource binding for meta draws or dispatches as it can be quickly
 * invalidated alongside the shader itself.
 */
#define TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META 0
/* Dynamically indexable internal and application's push constants. */
#define TERAKAN_RESOURCE_RANGE_PUSH_CONSTANTS                                                      \
   (TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META + 1)
/* VS: Base vertex and instance.
 * GS and VS/TES ALT_CONST: Ring buffer.
 * CS: Number of workgroups.
 * Not needed in FS, so can be used for an additional input attachment.
 */
#define TERAKAN_RESOURCE_RANGE_NON_PIXEL_STAGE_SPECIFIC (TERAKAN_RESOURCE_HW_COUNT_VERTEX - 1)
/* The 16 resources that fragment and compute shaders provide beyond the 160 available in vertex
 * stages can be fully allocated for resources needed only in those stages: 4 input attachments (the
 * minimum required by Vulkan) and 12 RAT IMMED buffers.
 */
#define TERAKAN_RESOURCE_RANGE_RAT_IMMEDIATE_BASE                                                  \
   (TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE - TERAKAN_LIMITS_HW_COLOR_RAT_COUNT)
/* Resources from the application's pipeline layout:
 * - Sampled images, uniform texel buffers.
 * - Storage images, storage texel buffers, storage buffers - read-only when coherence with writable
 *   ones is not needed (vertex stages, or `restrict readonly` without `coherent`), as well as for
 *   RAT info queries.
 * - Uniform buffers - for dynamic indexing.
 * - Input attachments.
 */
#define TERAKAN_RESOURCE_RANGE_MUTABLE_BASE (TERAKAN_RESOURCE_RANGE_PUSH_CONSTANTS + 1)
#define TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL                                         \
   (TERAKAN_RESOURCE_RANGE_NON_PIXEL_STAGE_SPECIFIC - TERAKAN_RESOURCE_RANGE_MUTABLE_BASE)
#define TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL                                             \
   (TERAKAN_RESOURCE_RANGE_RAT_IMMEDIATE_BASE - TERAKAN_RESOURCE_RANGE_MUTABLE_BASE)
static_assert(
   TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL >=
      TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL,
   "The maximum number of mutable resource bindings per stage is expected to be bound by vertex "
   "stages, which have fewer hardware resource bindings.");
static_assert(
   TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL >= 15 + 4 + 128 + 8,
   "There should be enough non-pixel shader mutable resource bindings for the minimum Direct3D 11 "
   "binding counts plus Vulkan storage buffers.");
static_assert(
   TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL >= 15 + 4 + 128 + 8 + 4,
   "There should be enough pixel shader mutable resource bindings for the minimum Direct3D 11 "
   "binding counts plus Vulkan storage buffers and input attachments.");

/* SQ_VTX_CONSTANT doesn't have words 5 and 6, so using word 5 for the BO priority. */
#define TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD 5

/* Hardware CB_COLOR[0-11] registers.
 * Note that image views don't store color buffer or RAT descriptors directly, instead they contain
 * data for both, but color buffers and RATs each have fields they don't use, or require specific
 * values for each field.
 * Before setting CB_COLOR[0-11] to these descriptors, pass them through
 * terakan_color_descriptor_image_view_to_color_attachment or
 * terakan_color_descriptor_image_view_to_storage_image depending on the needed binding type.
 */
struct terakan_color_descriptor {
   uint32_t base;
   uint32_t pitch;
   uint32_t slice;
   uint32_t view;
   /* In image views, the INFO register is for a color attachment. */
   uint32_t info;
   uint32_t attrib;
   /* In image views, the DIM register is for a storage image. */
   uint32_t dim;
};

static inline void
terakan_color_descriptor_image_view_to_color_attachment(
   struct terakan_color_descriptor * const descriptor)
{
   /* The meaning of DIM depends on RESOURCE_TYPE, but it's used only for RATs.
    * DIM is ignored for color attachments, scissor must be used to prevent out-of-bounds access.
    */
   descriptor->dim = 0;
}

/* The resource type must be the one actually requested by the shader in the binding declaration. */
static inline void
terakan_color_descriptor_image_view_to_storage_image(
   struct terakan_color_descriptor * const descriptor, uint32_t const resource_type)
{
   descriptor->info = (descriptor->info & (C_028C70_FAST_CLEAR & C_028C70_SOURCE_FORMAT)) |
                      S_028C70_SOURCE_FORMAT(V_028C70_EXPORT_4C_32BPC) | S_028C70_RAT(1) |
                      S_028C70_RESOURCE_TYPE(resource_type);
   descriptor->attrib &= C_028C74_FORCE_DST_ALPHA_1;
}

/* Additional hardware CB_COLOR[0-7] registers. */
struct terakan_color_meta_descriptor {
   uint32_t cmask;
   uint32_t cmask_slice;
   uint32_t fmask;
   uint32_t fmask_slice;
};

#endif /* TERAKAN_DESCRIPTOR_H */
