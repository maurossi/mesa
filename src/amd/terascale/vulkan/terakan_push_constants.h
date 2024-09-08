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

#ifndef TERAKAN_PUSH_CONSTANTS_H
#define TERAKAN_PUSH_CONSTANTS_H

#include "terakan_bo.h"
#include "terakan_descriptor.h"

#include "compiler/shader_enums.h"
#include "util/macros.h"
#include "vk_util.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum terakan_push_constants_driver_index {
   TERAKAN_PUSH_CONSTANT_DRIVER_INDEX_DRAW_ID,

   TERAKAN_PUSH_CONSTANT_DRIVER_INDEX_COUNT,
};

static_assert(TERAKAN_PUSH_CONSTANT_DRIVER_INDEX_COUNT <= 32,
              "Using driver push constant indices in a 32-bit bitfield.");

struct terakan_push_constants_driver {
   uint32_t draw_id;
};

/* Aligned to vec4 to avoid placing vectors in different kcache lines more likely to be accessed in
 * separate ALU clauses if they end up at the boundary.
 */
#define TERAKAN_PUSH_CONSTANTS_APP_BASE_BYTES                                                      \
   ALIGN_POT(sizeof(struct terakan_push_constants_driver), sizeof(float) * 4)
/* VUID-vkCmdPushConstants-size-00369: "size must be a multiple of 4" */
#define TERAKAN_PUSH_CONSTANTS_APP_SIZE_BYTES                                                      \
   ((TERAKAN_KCACHE_HW_MAX_BUFFER_SIZE_BYTES - TERAKAN_PUSH_CONSTANTS_APP_BASE_BYTES) &            \
    ~(uint32_t)(4 - 1))

struct terakan_push_constants_usage {
   uint32_t driver_constants;
   uint32_t app_extent_bytes;
};

static inline bool
terakan_push_constants_usage_empty(struct terakan_push_constants_usage const usage)
{
   return !usage.driver_constants && usage.app_extent_bytes == 0;
}

static inline struct terakan_push_constants_usage
terakan_push_constants_usage_union(struct terakan_push_constants_usage const a,
                                   struct terakan_push_constants_usage const b)
{
   struct terakan_push_constants_usage usage;
   usage.driver_constants = a.driver_constants | b.driver_constants;
   usage.app_extent_bytes = MAX2(a.app_extent_bytes, b.app_extent_bytes);
   return usage;
}

struct terakan_push_constants_state {
   struct {
      /* NULL if not up to date, the rest is undefined if that's the case. */
      char * mapping_if_up_to_date;
      struct terakan_bo const * bo;
      uint32_t va_kcache_lines;
      uint32_t size_kcache_lines;
      uint32_t app_up_to_date_bytes_written;
   } allocation;

   VkShaderStageFlags graphics_stages_using_push_constants;
   struct terakan_push_constants_usage usage_pre_rasterization;
   struct terakan_push_constants_usage usage_fragment;
   struct terakan_push_constants_usage usage_compute;

   VkShaderStageFlags up_to_date_push_constants_bound_to_stages;

   uint32_t driver_constants_modified;
   struct terakan_push_constants_driver driver_constants;

   /* Aligned to uint32_t for more optimal memcpy. Offsets and sizes are specified in bytes, but:
    * VUID-vkCmdPushConstants-offset-00368: "offset must be a multiple of 4"
    * VUID-vkCmdPushConstants-size-00369: "size must be a multiple of 4"
    */
   alignas(uint32_t) char app_constants[TERAKAN_PUSH_CONSTANTS_APP_SIZE_BYTES];
};

void terakan_push_constants_state_reset(struct terakan_push_constants_state * state);

struct terakan_gfx_command_writer;

void terakan_push_constants_apply(struct terakan_gfx_command_writer * command_writer,
                                  bool is_compute);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_PUSH_CONSTANTS_H */
