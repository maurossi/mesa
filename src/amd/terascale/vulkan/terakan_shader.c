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

#include "terakan_shader.h"

#include "terakan_bo.h"
#include "terakan_descriptor.h"
#include "terakan_device.h"
#include "terakan_physical_device.h"

#include "compiler/glsl_types.h"
#include "spirv/nir_spirv.h"
#include "util/macros.h"
#include "vk_nir.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* From gl_nir_linker.c. */
static void
terakan_nir_shared_type_info(struct glsl_type const * const type, unsigned * const size,
                             unsigned * const align)
{
   assert(glsl_type_is_vector_or_scalar(type));
   uint32_t const comp_size = glsl_type_is_boolean(type) ? 4 : glsl_get_bit_size(type) / 8;
   unsigned const length = glsl_get_vector_elements(type);
   *size = comp_size * length;
   *align = comp_size * (length == 3 ? 4 : length);
}

nir_shader *
terakan_shader_spirv_to_nir(struct terakan_device * const device, size_t const spirv_size_bytes,
                            uint32_t const * const spirv, gl_shader_stage const stage,
                            char const * const entrypoint,
                            VkSpecializationInfo const * const specialization_info)
{
   struct terakan_physical_device const * const physical_device =
      container_of(device->vk.physical, struct terakan_physical_device const, vk);

   static struct spirv_to_nir_options const spirv_options = {
      .environment = NIR_SPIRV_VULKAN,

      /* TODO(Triang3l): Possibly SUBGROUP_SIZE_API_CONSTANT when the subgroup size is properly
       * exposed.
       */
      .subgroup_size = SUBGROUP_SIZE_UNIFORM,

      /* TODO(Triang3l): Capabilities when supported and tested. */

      .ubo_addr_format = nir_address_format_32bit_index_offset,
      .ssbo_addr_format = nir_address_format_32bit_index_offset,
      .push_const_addr_format = nir_address_format_32bit_offset,
      .shared_addr_format = nir_address_format_32bit_offset,

      .min_ubo_alignment = TERAKAN_KCACHE_HW_LINE_BYTES,
      .min_ssbo_alignment = sizeof(uint32_t),
   };

   nir_shader * nir =
      vk_spirv_to_nir(&device->vk, spirv, spirv_size_bytes, stage, entrypoint,
                      spirv_options.subgroup_size, specialization_info, &spirv_options,
                      stage == MESA_SHADER_FRAGMENT ? &physical_device->nir_options_fs
                                                    : &physical_device->nir_options_non_fs,
                      false, NULL);

   /* Relevant st_glsl_to_nir passes. */

   /* st_link_glsl_to_nir, but without linkage. */

   /* preprocess_shader. */

   if (nir->options->lower_all_io_to_temps) {
      NIR_PASS_V(nir, nir_lower_io_to_temporaries, nir_shader_get_entrypoint(nir), true, true);
   }

   NIR_PASS_V(nir, nir_lower_global_vars_to_local);

   NIR_PASS_V(nir, nir_split_var_copies);
   NIR_PASS_V(nir, nir_lower_var_copies);

   assert(nir->options->lower_to_scalar);
   NIR_PASS_V(nir, nir_remove_dead_variables,
              nir_var_function_temp | nir_var_shader_temp | nir_var_mem_shared, NULL);
   NIR_PASS_V(nir, nir_opt_copy_prop_vars);
   NIR_PASS_V(nir, nir_lower_alu_to_scalar, nir->options->lower_to_scalar_filter, NULL);

   NIR_PASS_V(nir, nir_opt_barrier_modes);

   if (nir->info.stage == MESA_SHADER_COMPUTE) {
      NIR_PASS_V(nir, nir_lower_vars_to_explicit_types, nir_var_mem_shared,
                 terakan_nir_shared_type_info);
      NIR_PASS_V(nir, nir_lower_explicit_io, nir_var_mem_shared, nir_address_format_32bit_offset);
   }

   /* Do a round of constant folding to clean up address calculations. */
   NIR_PASS_V(nir, nir_opt_constant_folding);

   assert(nir->options->lower_to_scalar);
   NIR_PASS_V(nir, nir_lower_load_const_to_scalar);

   /* gl_nir_opts from prelink_lowering with num_shaders == 1. */

   bool progress;
   do {
      progress = false;

      NIR_PASS_V(nir, nir_lower_vars_to_ssa);

      /* Remove things local to the shader in the hopes that we can cleanup other things. This pass
       * will also remove variables with only stores, so we might be able to make progress after it.
       */
      NIR_PASS(progress, nir, nir_remove_dead_variables,
               nir_var_function_temp | nir_var_shader_temp | nir_var_mem_shared, NULL);

      NIR_PASS(progress, nir, nir_opt_find_array_copies);
      NIR_PASS(progress, nir, nir_opt_copy_prop_vars);
      NIR_PASS(progress, nir, nir_opt_dead_write_vars);

      assert(nir->options->lower_to_scalar);
      NIR_PASS_V(nir, nir_lower_alu_to_scalar, nir->options->lower_to_scalar_filter, NULL);
      NIR_PASS_V(nir, nir_lower_phis_to_scalar, false);

      NIR_PASS_V(nir, nir_lower_alu);
      NIR_PASS_V(nir, nir_lower_pack);
      NIR_PASS(progress, nir, nir_copy_prop);
      NIR_PASS(progress, nir, nir_opt_remove_phis);
      NIR_PASS(progress, nir, nir_opt_dce);
      if (nir_opt_trivial_continues(nir)) {
         progress = true;
         NIR_PASS(progress, nir, nir_copy_prop);
         NIR_PASS(progress, nir, nir_opt_dce);
      }
      NIR_PASS(progress, nir, nir_opt_if, 0);
      NIR_PASS(progress, nir, nir_opt_dead_cf);
      NIR_PASS(progress, nir, nir_opt_cse);
      NIR_PASS(progress, nir, nir_opt_peephole_select, 8, true, true);

      NIR_PASS(progress, nir, nir_opt_phi_precision);
      NIR_PASS(progress, nir, nir_opt_algebraic);
      NIR_PASS(progress, nir, nir_opt_constant_folding);

      if (!nir->info.flrp_lowered) {
         assert(nir->options->lower_flrp16 && nir->options->lower_flrp32 &&
                nir->options->lower_flrp64);
         bool lower_flrp_progress = false;
         NIR_PASS(lower_flrp_progress, nir, nir_lower_flrp, 16 | 32 | 64, false);
         if (lower_flrp_progress) {
            NIR_PASS(progress, nir, nir_opt_constant_folding);
            progress = true;
         }
         /* Nothing should rematerialize any flrps, so we only need to do this lowering once. */
         nir->info.flrp_lowered = true;
      }

      NIR_PASS(progress, nir, nir_opt_undef);
      NIR_PASS(progress, nir, nir_opt_conditional_discard);

      assert(nir->options->max_unroll_iterations != 0);
      NIR_PASS(progress, nir, nir_opt_loop_unroll);
   } while (progress);

   NIR_PASS_V(nir, nir_lower_var_copies);

   /* prelink_lowering. */

   nir_opt_access_options const opt_access_options = {.is_vulkan = true};
   NIR_PASS_V(nir, nir_opt_access, &opt_access_options);

   NIR_PASS_V(nir, nir_lower_clip_cull_distance_arrays);

   /* st_link_glsl_to_nir. */

   NIR_PASS_V(nir, nir_lower_system_values);
   NIR_PASS_V(nir, nir_lower_compute_system_values, NULL);

   return nir;
}

void
terakan_shader_impl_finish(struct terakan_shader_impl * const shader,
                           VkAllocationCallbacks const * const allocator)
{
   if (shader->shader.arrays != NULL) {
      free(shader->shader.arrays);
   }

   terakan_bo_free(shader->static_state.program_bo, allocator);
}
