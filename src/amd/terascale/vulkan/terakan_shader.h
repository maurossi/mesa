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

#ifndef TERAKAN_SHADER_H
#define TERAKAN_SHADER_H

#include "terakan_bo.h"

#include "gallium/drivers/r600/r600_shader_common.h"
#include "nir.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TERAKAN_SHADER_PROGRAM_ALIGNMENT_LOG2 8
#define TERAKAN_SHADER_PROGRAM_ALIGNMENT      (1 << TERAKAN_SHADER_PROGRAM_ALIGNMENT_LOG2)

/* Fields that don't depend on any other state. */
struct terakan_shader_static {
   struct terakan_bo * program_bo;
   uint32_t program_start;

   uint32_t sq_pgm_resources[2];

   union {
      struct {
         uint32_t spi_vs_out_id[10];
         uint32_t spi_vs_out_config;
         uint32_t pa_cl_vs_out_cntl;
      } vs;

      struct {
         uint32_t sq_pgm_exports_ps;
         uint32_t spi_ps_input_cntl[32];
         uint32_t spi_ps_in_control[2];
         uint32_t spi_input_z;
         uint32_t spi_baryc_cntl;
         uint32_t cb_shader_mask;
      } ps;
   } stage;
};

/* Shader implementation common for both pipelines (to be used in a pipeline-cached wrapper) and
 * shader objects (for an uncached wrapper implementing VkShaderEXT).
 */
struct terakan_shader_impl {
   /* This object owns the BO in `static_state`. */
   /* TODO(Triang3l): Shader suballocation. */
   struct terakan_shader_static static_state;

   struct r600_shader shader;
};

struct terakan_device;

nir_shader * terakan_shader_spirv_to_nir(struct terakan_device * device, size_t spirv_size_bytes,
                                         uint32_t const * spirv, gl_shader_stage stage,
                                         char const * entrypoint,
                                         VkSpecializationInfo const * specialization_info);

void terakan_shader_impl_finish(struct terakan_shader_impl * shader,
                                VkAllocationCallbacks const * allocator);

/* Modifies the input NIR, clone externally if needed. */
VkResult terakan_shader_impl_init_from_nir(struct terakan_shader_impl * shader,
                                           struct terakan_device * device,
                                           union r600_shader_key const * key, nir_shader * nir,
                                           VkAllocationCallbacks const * allocator);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_SHADER_H */
