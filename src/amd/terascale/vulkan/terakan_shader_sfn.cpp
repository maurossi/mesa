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
#include "terakan_device.h"
#include "terakan_physical_device.h"

#include "compiler/shader_enums.h"
#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600_asm.h"
#include "gallium/drivers/r600/r600_isa.h"
#include "gallium/drivers/r600/sfn/sfn_assembler.h"
#include "gallium/drivers/r600/sfn/sfn_memorypool.h"
#include "gallium/drivers/r600/sfn/sfn_nir.h"
#include "gallium/include/pipe/p_shader_tokens.h"
#include "gallium/include/pipe/p_state.h"
#include "util/u_math.h"
#include "amd_family.h"
#include "nir.h"
#include "vk_log.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

VkResult
terakan_shader_impl_init_from_nir(terakan_shader_impl * const shader, terakan_device * const device,
                                  r600_shader_key const * const key, nir_shader * const nir,
                                  VkAllocationCallbacks const * const allocator)
{
   VkResult result;

   std::memset(shader, 0, sizeof(*shader));

   terakan_physical_device const & physical_device =
      *container_of(device->vk.physical, terakan_physical_device const, vk);
   terakan_physical_device_chip_family_info const & chip_family_info =
      physical_device.chip_family_info;
   amd_gfx_level const gfx_level = chip_family_info.is_r9xx ? CAYMAN : EVERGREEN;

   /* TODO(Triang3l): Fill stream output info from NIR. */
   pipe_stream_output_info so_info = {};

   r600::init_pool();

   r600_finalize_nir_common(nir, gfx_level);
   /* For r600_lower_and_optimize_nir, for fields like number bit sizes. */
   nir_shader_gather_info(nir, nir_shader_get_entrypoint(nir));
   r600_lower_and_optimize_nir(nir, key, gfx_level, &so_info);

   r600::Shader * const unscheduled_sfn_shader = r600::Shader::translate_from_nir(
      nir, &so_info, nullptr, *key, chip_family_info.is_r9xx ? ISA_CC_CAYMAN : ISA_CC_EVERGREEN,
      chip_family_info.chip_family);
   if (unscheduled_sfn_shader == nullptr) {
      r600::release_pool();
      return vk_errorf(device, VK_ERROR_UNKNOWN, "Failed to translate the shader from NIR");
   }

   r600_finalize_and_optimize_shader(unscheduled_sfn_shader);
   r600::Shader * const sfn_shader = r600_schedule_shader(unscheduled_sfn_shader);
   if (sfn_shader != unscheduled_sfn_shader) {
      delete unscheduled_sfn_shader;
   }
   if (sfn_shader == nullptr) {
      r600::release_pool();
      return vk_errorf(device, VK_ERROR_UNKNOWN, "Failed to schedule the shader");
   }

   sfn_shader->get_shader_info(&shader->shader);

   /* TODO(Triang3l): has_compressed_msaa_texturing. */
   r600_bytecode_init(&shader->shader.bc, gfx_level, chip_family_info.chip_family, false);

   /* We already schedule the code with this in mind, no need to handle this in the backend
    * assembler.
    */
   shader->shader.bc.ar_handling = AR_HANDLE_NORMAL;
   shader->shader.bc.r6xx_nop_after_rel_dst = 0;

   shader->shader.bc.type = shader->shader.processor_type;
   shader->shader.bc.isa = physical_device.isa;
   shader->shader.bc.ngpr = sfn_shader->required_registers();

   r600::Assembler assembler(&shader->shader, *key);
   if (!assembler.lower(sfn_shader)) {
      delete sfn_shader;

      r600::release_pool();

      r600_bytecode_clear(&shader->shader.bc);
      if (shader->shader.arrays != nullptr) {
         std::free(shader->shader.arrays);
      }

      return vk_errorf(device, VK_ERROR_UNKNOWN, "Failed to lower the shader to assembly");
   }

   delete sfn_shader;

   r600::release_pool();

   if (r600_bytecode_build(&shader->shader.bc) != 0) {
      r600_bytecode_clear(&shader->shader.bc);
      if (shader->shader.arrays != nullptr) {
         std::free(shader->shader.arrays);
      }
      return vk_errorf(device, VK_ERROR_UNKNOWN, "Failed to build the shader bytecode");
   }

   /* Fill shader registers. */

   shader->static_state.sq_pgm_resources[0] = S_028844_NUM_GPRS(shader->shader.bc.ngpr) |
                                              S_028844_STACK_SIZE(shader->shader.bc.nstack) |
                                              S_028844_DX10_CLAMP(1);
   /* TODO(Triang3l): Rounding modes from shader float controls. */
   shader->static_state.sq_pgm_resources[1] = S_028848_SINGLE_ROUND(V_SQ_ROUND_NEAREST_EVEN) |
                                              S_028848_DOUBLE_ROUND(V_SQ_ROUND_NEAREST_EVEN);

   /* TODO(Triang3l): Correct vertex pipeline stages. */
   switch (nir->info.stage) {
   case MESA_SHADER_VERTEX: {
      std::memset(shader->static_state.stage.vs.spi_vs_out_id, 0,
                  sizeof(shader->static_state.stage.vs.spi_vs_out_id));
      for (unsigned output_index = 0; output_index < shader->shader.noutput; ++output_index) {
         r600_shader_io const & output = shader->shader.output[output_index];
         if (output.export_param >= 0) {
            unsigned & parameter_spi_vs_out_id =
               shader->static_state.stage.vs.spi_vs_out_id[output.export_param / 4];
            unsigned const parameter_shift = (output.export_param & 3) * 8;
            assert(!(parameter_spi_vs_out_id & ((uint32_t)0xFF << parameter_shift)));
            parameter_spi_vs_out_id |= (uint32_t)output.spi_sid << parameter_shift;
         }
      }

      shader->static_state.stage.vs.spi_vs_out_config =
         S_0286C4_VS_EXPORT_COUNT(shader->shader.highest_export_param);

      uint32_t const clip_distances_enabled =
         (((uint32_t)1 << nir->info.clip_distance_array_size) - 1);
      uint32_t const cull_distances_enabled =
         (((uint32_t)1 << nir->info.cull_distance_array_size) - 1)
         << nir->info.clip_distance_array_size;
      uint32_t const clip_cull_distances_enabled = clip_distances_enabled | cull_distances_enabled;
      shader->static_state.stage.vs.pa_cl_vs_out_cntl =
         clip_distances_enabled | (cull_distances_enabled << 8) |
         S_02881C_USE_VTX_POINT_SIZE(shader->shader.vs_out_point_size) |
         S_02881C_USE_VTX_RENDER_TARGET_INDX(shader->shader.vs_out_layer) |
         S_02881C_USE_VTX_VIEWPORT_INDX(shader->shader.vs_out_viewport) |
         S_02881C_VS_OUT_MISC_VEC_ENA(shader->shader.vs_out_misc_write) |
         S_02881C_VS_OUT_CCDIST0_VEC_ENA((clip_cull_distances_enabled & 0b00001111) != 0) |
         S_02881C_VS_OUT_CCDIST1_VEC_ENA((clip_cull_distances_enabled & 0b11110000) != 0);
   } break;

   case MESA_SHADER_FRAGMENT: {
      bool export_z = false;
      for (unsigned output_index = 0; output_index < shader->shader.noutput; ++output_index) {
         gl_frag_result const frag_result = shader->shader.output[output_index].frag_result;
         if (frag_result == FRAG_RESULT_DEPTH || frag_result == FRAG_RESULT_STENCIL ||
             frag_result == FRAG_RESULT_SAMPLE_MASK) {
            export_z = true;
            break;
         }
      }
      /* Something must be exported, either at least one color or at least the DB export.
       * Explicitly ensuring that is not necessary in this code due to the + 1.
       */
      shader->static_state.stage.ps.sq_pgm_exports_ps =
         S_02884C_EXPORT_COLORS(shader->shader.ps_export_highest + 1) | S_02884C_EXPORT_Z(export_z);

      std::memset(shader->static_state.stage.ps.spi_ps_input_cntl, 0,
                  sizeof(shader->static_state.stage.ps.spi_ps_input_cntl));
      /* TODO(Triang3l): Build SPI_BARYC_CNTL in the shader compiler rather than independently here
       * because GPR allocation in the shader depends on it.
       */
      shader->static_state.stage.ps.spi_baryc_cntl = 0;
      uint32_t interpolator_count = 0;
      r600_shader_io const * position_input = nullptr;
      uint32_t face_and_sample_mask_gpr = UINT32_MAX;
      uint32_t sample_id_gpr = UINT32_MAX;
      for (unsigned input_index = 0; input_index < shader->shader.ninput; ++input_index) {
         r600_shader_io const & input = shader->shader.input[input_index];
         if (input.varying_slot == VARYING_SLOT_POS) {
            assert(position_input == nullptr);
            position_input = &input;
         } else if (input.varying_slot == VARYING_SLOT_FACE ||
                    input.system_value == SYSTEM_VALUE_SAMPLE_MASK_IN) {
            assert(face_and_sample_mask_gpr == UINT32_MAX || face_and_sample_mask_gpr == input.gpr);
            face_and_sample_mask_gpr = input.gpr;
         } else if (input.system_value == SYSTEM_VALUE_SAMPLE_ID) {
            assert(sample_id_gpr == UINT32_MAX);
            sample_id_gpr = input.gpr;
         } else if (input.spi_sid != 0) {
            interpolator_count = MAX2(input.lds_pos + 1, interpolator_count);
            shader->static_state.stage.ps.spi_ps_input_cntl[input.lds_pos] =
               S_028644_SEMANTIC(input.spi_sid) |
               S_028644_FLAT_SHADE(input.interpolate == TGSI_INTERPOLATE_CONSTANT) |
               S_028644_PT_SPRITE_TEX(input.varying_slot == VARYING_SLOT_PNTC);
            bool const interpolator_is_linear = input.interpolate == TGSI_INTERPOLATE_LINEAR;
            if (interpolator_is_linear || input.interpolate == TGSI_INTERPOLATE_PERSPECTIVE ||
                input.interpolate == TGSI_INTERPOLATE_COLOR) {
               switch (input.interpolate_location) {
               case TGSI_INTERPOLATE_LOC_CENTER:
                  shader->static_state.stage.ps.spi_baryc_cntl |= interpolator_is_linear
                                                                     ? S_0286E0_LINEAR_CENTER_ENA(1)
                                                                     : S_0286E0_PERSP_CENTER_ENA(1);
                  break;
               case TGSI_INTERPOLATE_LOC_CENTROID:
                  shader->static_state.stage.ps.spi_baryc_cntl |=
                     interpolator_is_linear ? S_0286E0_LINEAR_CENTROID_ENA(1)
                                            : S_0286E0_PERSP_CENTROID_ENA(1);
                  break;
               case TGSI_INTERPOLATE_LOC_SAMPLE:
                  shader->static_state.stage.ps.spi_baryc_cntl |= interpolator_is_linear
                                                                     ? S_0286E0_LINEAR_SAMPLE_ENA(1)
                                                                     : S_0286E0_PERSP_SAMPLE_ENA(1);
                  break;
               default:
                  break;
               }
            }
         }
      }
      if (!shader->static_state.stage.ps.spi_baryc_cntl) {
         shader->static_state.stage.ps.spi_baryc_cntl = S_0286E0_PERSP_CENTER_ENA(1);
      }
      constexpr uint32_t spi_baryc_cntl_persp_clear =
         C_0286E0_PERSP_CENTER_ENA & C_0286E0_PERSP_CENTROID_ENA & C_0286E0_PERSP_SAMPLE_ENA &
         C_0286E0_PERSP_PULL_MODEL_ENA;
      constexpr uint32_t spi_baryc_cntl_linear_clear =
         C_0286E0_LINEAR_CENTER_ENA & C_0286E0_LINEAR_CENTROID_ENA & C_0286E0_LINEAR_SAMPLE_ENA;
      shader->static_state.stage.ps.spi_ps_in_control[0] =
         S_0286CC_NUM_INTERP(MAX2(interpolator_count, 1)) |
         S_0286CC_PERSP_GRADIENT_ENA(
            (shader->static_state.stage.ps.spi_baryc_cntl & ~spi_baryc_cntl_persp_clear) != 0) |
         S_0286CC_LINEAR_GRADIENT_ENA(
            (shader->static_state.stage.ps.spi_baryc_cntl & ~spi_baryc_cntl_linear_clear) != 0);
      if (position_input != nullptr) {
         shader->static_state.stage.ps.spi_ps_in_control[0] |=
            S_0286CC_POSITION_ENA(1) |
            S_0286CC_POSITION_CENTROID(position_input->interpolate_location ==
                                       TGSI_INTERPOLATE_LOC_CENTROID) |
            S_0286CC_POSITION_SAMPLE(position_input->interpolate_location ==
                                     TGSI_INTERPOLATE_LOC_SAMPLE) |
            S_0286CC_POSITION_ADDR(position_input->gpr);
      }
      shader->static_state.stage.ps.spi_ps_in_control[1] = 0;
      if (face_and_sample_mask_gpr != UINT32_MAX) {
         shader->static_state.stage.ps.spi_ps_in_control[1] |=
            S_0286D0_FRONT_FACE_ENA(1) | S_0286D0_FRONT_FACE_ADDR(face_and_sample_mask_gpr);
      }
      if (sample_id_gpr != UINT32_MAX) {
         shader->static_state.stage.ps.spi_ps_in_control[1] |=
            S_0286D0_FIXED_PT_POSITION_ENA(1) | S_0286D0_FIXED_PT_POSITION_ADDR(sample_id_gpr);
      }
      shader->static_state.stage.ps.spi_input_z =
         S_0286D8_PROVIDE_Z_TO_SPI(position_input != nullptr);

      shader->static_state.stage.ps.cb_shader_mask = shader->shader.ps_color_export_mask;

      /* TODO(Triang3l): DB_SHADER_CONTROL. */
   } break;

   default:
      break;
   }

   /* Write the program to the BO. */
   size_t const program_size_bytes = sizeof(uint32_t) * shader->shader.bc.ndw;
   result = device->winsys_fn->bo->allocate_device_memory(
      device, program_size_bytes, TERAKAN_SHADER_PROGRAM_ALIGNMENT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      0, allocator, VK_SYSTEM_ALLOCATION_SCOPE_OBJECT, &shader->static_state.program_bo);
   if (result != VK_SUCCESS) {
      r600_bytecode_clear(&shader->shader.bc);
      if (shader->shader.arrays != nullptr) {
         std::free(shader->shader.arrays);
      }
      return vk_error(device, result);
   }
   shader->static_state.program_start = 0;
   {
      void * const program_bo_mapping = terakan_bo_map(shader->static_state.program_bo);
      if (program_bo_mapping == nullptr) {
         terakan_bo_free(shader->static_state.program_bo, allocator);
         r600_bytecode_clear(&shader->shader.bc);
         if (shader->shader.arrays != nullptr) {
            std::free(shader->shader.arrays);
         }
         return vk_error(device, VK_ERROR_OUT_OF_HOST_MEMORY);
      }
      util_memcpy_cpu_to_le32(program_bo_mapping, shader->shader.bc.bytecode, program_size_bytes);
      terakan_bo_unmap(device->meta_shaders_bo);
   }

   /* Don't need the bytecode structure after writing the binary. */
   r600_bytecode_clear(&shader->shader.bc);

   return VK_SUCCESS;
}
