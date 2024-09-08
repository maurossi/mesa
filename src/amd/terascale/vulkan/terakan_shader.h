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

#ifndef TERAKAN_SHADER_H
#define TERAKAN_SHADER_H

#include "terakan_bo.h"
#include "terakan_descriptor.h"
#include "terakan_pipeline_layout.h"
#include "terakan_push_constants.h"
#include "terakan_vertex_input.h"

#include "gallium/drivers/r600/r600_shader_common.h"
#include "util/bitset.h"
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
   uint32_t program_va_shr8;

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
         /* This must include all color exports done by the shader, otherwise there will be hangs
          * (tested with dEQP-VK.pipeline.monolithic.blend.dual_source.* on Barts).
          * The CB_SHADER_MASK rules for dual-source blending described in Radeon Evergreen /
          * Northern Islands Acceleration are incorrect.
          */
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

   struct terakan_push_constants_usage push_constants_usage;

   BITSET_DECLARE(resources_needed, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE);
   uint32_t samplers_needed;

   struct {
      BITSET_DECLARE(vertex_attributes_needed, TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES);
   } vs;

   struct {
      /* DUAL_EXPORT_ENABLE specifies only whether it can potentially be enabled for the shader. */
      uint32_t db_shader_control;

      uint8_t fragment_data_uncompacted_locations;
   } fs;

   struct r600_shader shader;
};

struct terakan_device;

/* Converts SPIR-V to NIR and performs pre-link lowerings. */
nir_shader * terakan_shader_spirv_to_nir(struct terakan_device * device, size_t spirv_size_bytes,
                                         uint32_t const * spirv, gl_shader_stage stage,
                                         char const * entrypoint,
                                         VkSpecializationInfo const * specialization_info);

void terakan_shader_lower_and_optimize_post_link(
   nir_shader * nir, struct terakan_pipeline_layout const * pipeline_layout,
   BITSET_WORD * resources_needed, uint32_t * samplers_needed,
   uint8_t * fragment_data_uncompacted_locations_out);

void terakan_shader_impl_finish(struct terakan_shader_impl * shader,
                                VkAllocationCallbacks const * allocator);

/* Compiles the shader into the microcode written to a BO, and fills the info from the backend
 * compiler. Assumes that everything in the shader not intended to be filled directly from the NIR
 * has been zeroed prior to the call.
 */
VkResult terakan_shader_impl_compile(struct terakan_shader_impl * shader,
                                     struct terakan_device * device,
                                     union r600_shader_key const * key, nir_shader * nir,
                                     VkAllocationCallbacks const * allocator);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_SHADER_H */
