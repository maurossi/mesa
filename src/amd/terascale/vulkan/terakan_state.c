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

#include "terakan_state.h"

#include "meta/terakan_meta.h"
#include "terakan_bo.h"
#include "terakan_command_buffer.h"
#include "terakan_descriptor.h"
#include "terakan_device.h"
#include "terakan_format.h"
#include "terakan_hw_state.h"
#include "terakan_image.h"
#include "terakan_shader.h"
#include "terakan_state_rasterization.h"

#include "gallium/drivers/r600/evergreend.h"
#include "util/bitscan.h"
#include "util/macros.h"
#include "util/u_endian.h"
#include "util/u_math.h"

#include <assert.h>
#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void
terakan_state_translate_window_rect_unpacked(VkRect2D const * const rect, uint16_t tl_br_xy_out[4])
{
   if (unlikely(rect->offset.x > TERAKAN_IMAGE_MAX_WIDTH_HEIGHT - 1 ||
                rect->offset.y > TERAKAN_IMAGE_MAX_WIDTH_HEIGHT - 1)) {
      /* For top-left, the maximum value in the register is 2^n-1, not 2^n. */
      memset(tl_br_xy_out, 0, sizeof(*tl_br_xy_out) * 4);
      return;
   }

   tl_br_xy_out[0] = CLAMP(rect->offset.x, 0, TERAKAN_IMAGE_MAX_WIDTH_HEIGHT - 1);
   tl_br_xy_out[1] = CLAMP(rect->offset.y, 0, TERAKAN_IMAGE_MAX_WIDTH_HEIGHT - 1);
   tl_br_xy_out[2] = CLAMP(rect->offset.x + rect->extent.width, 0, TERAKAN_IMAGE_MAX_WIDTH_HEIGHT);
   tl_br_xy_out[3] = CLAMP(rect->offset.y + rect->extent.height, 0, TERAKAN_IMAGE_MAX_WIDTH_HEIGHT);
}

/* Section 28.12.3. "Depth Bias" of the Vulkan 1.3.279 specification says:
 *
 *    "r is the minimum resolvable difference that depends on the depth attachment representation."
 *
 *    "If no depth attachment is present, r is undefined."
 *
 * To simplify handling of an unused depth attachment, treat it as 32-bit floating-point.
 * This fallback specifically is chosen because without a depth attachment, the depth bias is still
 * visible to applications via gl_FragCoord.z, and it's a 32-bit floating-point number.
 */
#define TERAKAN_STATE_DRAW_PA_SU_POLY_OFFSET_DB_FMT_CNTL_NO_ATTACHMENT_FORMAT V_028040_Z_32_FLOAT

typedef void (*terakan_state_draw_apply_function)(
   struct terakan_gfx_command_writer * command_writer);

static void
terakan_state_draw_apply_vgt_index_type(struct terakan_gfx_command_writer * const command_writer)
{
   bool const modified =
      command_writer->hw_state_draw.vgt_index_type != command_writer->state_draw.vgt_index_type;
   command_writer->hw_state_draw.vgt_index_type = command_writer->state_draw.vgt_index_type;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_TYPE, modified);
}

static void
terakan_state_draw_apply_vgt_primitive_type(struct terakan_gfx_command_writer * const command_writer)
{
   bool const modified = command_writer->hw_state_draw.vgt_primitive_type !=
                         command_writer->state_draw.vgt_primitive_type;
   command_writer->hw_state_draw.vgt_primitive_type = command_writer->state_draw.vgt_primitive_type;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_VGT_PRIMITIVE_TYPE, modified);
}

static void
terakan_state_draw_apply_vgt_index_offset(struct terakan_gfx_command_writer * const command_writer)
{
   bool const modified =
      command_writer->hw_state_draw.vgt_index_offset != command_writer->state_draw.vgt_index_offset;
   command_writer->hw_state_draw.vgt_index_offset = command_writer->state_draw.vgt_index_offset;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_VGT_INDEX_OFFSET, modified);
}

static void
terakan_state_draw_apply_sq_pgm_ls_es_gs_vs(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_state_draw * const state = &command_writer->state_draw;

   command_writer->push_constants_state.graphics_stages_using_push_constants &=
      ~(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT |
        VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_GEOMETRY_BIT);
   command_writer->push_constants_state.usage_pre_rasterization =
      (struct terakan_push_constants_usage){};

   /* Vertex shader. */

   struct terakan_shader_impl const * const vs = state->sq_pgm_ls_es_gs_vs.vs_as_vs;
   assert(vs != NULL);
   if (likely(vs != NULL)) {
      bool const vs_static_modified = command_writer->hw_state_draw.sq_pgm_vs != &vs->static_state;
      command_writer->hw_state_draw.sq_pgm_vs = &vs->static_state;
      terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                    TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_VS, vs_static_modified);

      terakan_hw_state_draw_set_sq_constants_needed_by_vs(&command_writer->hw_state_draw, 0,
                                                          vs->resources_needed, vs->samplers_needed,
                                                          VK_SHADER_STAGE_FRAGMENT_BIT);

      if (!terakan_push_constants_usage_empty(vs->push_constants_usage)) {
         command_writer->push_constants_state.usage_pre_rasterization =
            terakan_push_constants_usage_union(
               command_writer->push_constants_state.usage_pre_rasterization,
               vs->push_constants_usage);
         command_writer->push_constants_state.graphics_stages_using_push_constants |=
            VK_SHADER_STAGE_VERTEX_BIT;
      }

      TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(SQ_PGM_FS, SQ_PGM_LS_ES_GS_VS);
      if (state->sq_pgm_fs.static_state == NULL) {
         for (unsigned attribute_word_index = 0;
              attribute_word_index < BITSET_WORDS(TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES);
              ++attribute_word_index) {
            if ((state->sq_pgm_fs.dynamic_state.from_apply_sq_pgm_ls_es_gs_vs
                    .attributes_needed_by_vs[attribute_word_index] ^
                 vs->vs.vertex_attributes_needed[attribute_word_index]) &
                state->sq_pgm_fs.dynamic_state.attributes_provided[attribute_word_index]) {
               terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_SQ_PGM_FS);
               break;
            }
         }
      }
      BITSET_COPY(
         state->sq_pgm_fs.dynamic_state.from_apply_sq_pgm_ls_es_gs_vs.attributes_needed_by_vs,
         vs->vs.vertex_attributes_needed);
   }
}

static void
terakan_state_draw_apply_sq_pgm_fs(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_state_draw const * const state = &command_writer->state_draw;

   struct terakan_vertex_input_static_state const * const static_vi = state->sq_pgm_fs.static_state;

   struct terakan_bo const * program_bo;
   uint32_t program_va_shr8;

   static_assert(
      BITSET_WORDBITS >= TERAKAN_RESOURCE_HW_COUNT_FETCH,
      "Assuming that the bindings mask as a single integer can fit into one bitset word.");
   BITSET_DECLARE(resources_needed, TERAKAN_RESOURCE_HW_COUNT_FETCH);
   resources_needed[0] =
      static_vi != NULL ? static_vi->bindings_needed_by_attributes_and_provided
                        : state->sq_pgm_fs.dynamic_state.bindings_needed_by_attributes_and_provided;

   uint32_t const bindings_with_2048_stride_workaround =
      state->sq_pgm_fs.bindings_with_2048_stride_workaround;
   if (static_vi != NULL &&
       !((static_vi->bindings_with_2048_stride_workaround ^ bindings_with_2048_stride_workaround) &
         static_vi->bindings_needed_by_attributes_and_provided)) {
      /* Use the static vertex input state fetch shader. */
      program_bo = static_vi->program_bo;
      program_va_shr8 = static_vi->program_va_shr8;
   } else {
      /* Dynamically create the fetch shader, allocated alongside push constants. */
      struct terakan_device const * const device =
         terakan_gfx_command_writer_device(command_writer);
      bool const is_r9xx = terakan_device_physical_device(device)->chip_family_info.is_r9xx;
      uint32_t fs_alu_qword_count, fs_alu_clause_count, fs_fetch_count;
      uint32_t fs_alu[2 * TERAKAN_VERTEX_INPUT_FS_MAX_ALU_QWORDS];
      uint8_t fs_alu_clause_qwords[TERAKAN_VERTEX_INPUT_FS_MAX_ALU_CLAUSES];
      uint32_t fs_fetch[4 * TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES];
      terakan_vertex_input_create_fs_alu_and_fetches(
         is_r9xx,
         static_vi != NULL ? static_vi->attributes_needed_and_provided
                           : state->sq_pgm_fs.dynamic_state.attributes_provided,
         static_vi != NULL ? static_vi->attributes : state->sq_pgm_fs.dynamic_state.attributes,
         static_vi != NULL ? static_vi->instance_bindings
                           : state->sq_pgm_fs.dynamic_state.instance_bindings,
         static_vi != NULL ? static_vi->instance_binding_divisors
                           : state->sq_pgm_fs.dynamic_state.instance_binding_divisors,
         bindings_with_2048_stride_workaround, &fs_alu_qword_count, fs_alu, &fs_alu_clause_count,
         fs_alu_clause_qwords, &fs_fetch_count, fs_fetch);
      uint64_t program_va;
      void * const fs_mapping =
         terakan_push_buffer_allocate(command_writer->base.command_buffer,
                                      terakan_vertex_input_fs_byte_count(
                                         fs_alu_qword_count, fs_alu_clause_count, fs_fetch_count),
                                      TERAKAN_SHADER_PROGRAM_ALIGNMENT, &program_bo, &program_va);
      if (unlikely(fs_mapping == NULL)) {
         /* Fall back to the empty fetch shader to avoid drawing with an uninitialized fetch shader
          * address.
          */
         program_bo = device->empty_vertex_input.program_bo;
         program_va_shr8 = device->empty_vertex_input.program_va_shr8;
         BITSET_ZERO(resources_needed);
      } else {
         program_va_shr8 = (uint32_t)(program_va >> 8);
         terakan_vertex_input_create_fs_program(is_r9xx, fs_alu_qword_count, fs_alu,
                                                fs_alu_clause_count, fs_alu_clause_qwords,
                                                fs_fetch_count, fs_fetch, fs_mapping);
      }
   }

   bool const program_modified = command_writer->hw_state_draw.sq_pgm_fs.bo != program_bo ||
                                 command_writer->hw_state_draw.sq_pgm_fs.va_shr8 != program_va_shr8;
   command_writer->hw_state_draw.sq_pgm_fs.bo = program_bo;
   command_writer->hw_state_draw.sq_pgm_fs.va_shr8 = program_va_shr8;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_FS, program_modified);

   terakan_hw_state_draw_set_sq_constants_needed_by_vi(&command_writer->hw_state_draw,
                                                       resources_needed);
}

static void
terakan_state_draw_apply_sq_resources_fs(struct terakan_gfx_command_writer * const command_writer)
{
   bool const is_r9xx =
      terakan_gfx_command_writer_physical_device(command_writer)->chip_family_info.is_r9xx;
   uint32_t resource[8] = {
      [3] = S_03000C_DST_SEL_X(TERASCALE_SWIZZLE_X) | S_03000C_DST_SEL_Y(TERASCALE_SWIZZLE_Y) |
            S_03000C_DST_SEL_Z(TERASCALE_SWIZZLE_Z) | S_03000C_DST_SEL_W(TERASCALE_SWIZZLE_W),
      [7] = S_03001C_TYPE(V_03001C_SQ_TEX_VTX_VALID_BUFFER),
      [TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD] = TERAKAN_BO_PRIORITY_VERTEX_BUFFER,
   };
   unsigned buffers_remaining = command_writer->state_draw.sq_resources_fs_pending;
   while (buffers_remaining) {
      int const buffer_index = u_bit_scan(&buffers_remaining);
      struct terakan_state_draw_sq_resource_fs const * const buffer =
         &command_writer->state_draw.sq_resources_fs[buffer_index];
      resource[0] = (uint32_t)buffer->va;
      resource[1] = buffer->size_bytes_minus_1;
      /* The stride field in the descriptor is 11 bits wide on R8xx, 12 bits wide on R9xx.
       * To support 2048 stride, which is mandatory on Vulkan and Direct3D, the fetch shader
       * multiplies the index by 2 on R8xx, so the stride in the descriptor should be divided by 2.
       */
      resource[2] =
         S_030008_BASE_ADDRESS_HI(buffer->va >> 32) |
         ((uint32_t)((buffer->stride >> (is_r9xx && buffer->stride >= 0x800 ? 1 : 0)) & 0xFFF)
          << 8);
      terakan_hw_state_draw_set_sq_resource_vi(&command_writer->hw_state_draw, buffer_index,
                                               buffer->bo, resource);
   }
   command_writer->state_draw.sq_resources_fs_pending = 0;
}

static void
terakan_state_draw_apply_sq_pgm_ps(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_shader_impl const * const fs = command_writer->state_draw.sq_pgm_ps.fs;

   struct terakan_shader_static const * const fs_static =
      fs != NULL ? &fs->static_state
                 : &terakan_gfx_command_writer_device(command_writer)
                       ->meta_shaders[TERAKAN_META_SHADER_EMPTY_OPAQUE_PS];
   bool const fs_static_modified = command_writer->hw_state_draw.sq_pgm_ps != fs_static;
   command_writer->hw_state_draw.sq_pgm_ps = fs_static;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_SQ_PGM_PS, fs_static_modified);

   terakan_hw_state_draw_set_sq_constants_needed_by_fs(&command_writer->hw_state_draw, 0,
                                                       fs != NULL ? fs->resources_needed : NULL,
                                                       fs != NULL ? fs->samplers_needed : 0b0);

   command_writer->push_constants_state.usage_fragment =
      fs != NULL ? fs->push_constants_usage : (struct terakan_push_constants_usage){};
   if (terakan_push_constants_usage_empty(command_writer->push_constants_state.usage_fragment)) {
      command_writer->push_constants_state.graphics_stages_using_push_constants &=
         ~VK_SHADER_STAGE_FRAGMENT_BIT;
   } else {
      command_writer->push_constants_state.graphics_stages_using_push_constants |=
         VK_SHADER_STAGE_FRAGMENT_BIT;
   }

   uint8_t const color_attachments_written_by_shader =
      fs != NULL ? fs->fs.fragment_data_uncompacted_locations : 0b0;
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(COLOR_ATTACHMENT_USAGE, SQ_PGM_PS);
   if (command_writer->state_draw.color_attachment_usage.from_apply_sq_pgm_ps.written_by_shader !=
       color_attachments_written_by_shader) {
      command_writer->state_draw.color_attachment_usage.from_apply_sq_pgm_ps.written_by_shader =
         color_attachments_written_by_shader;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_COLOR_ATTACHMENT_USAGE);
   }

   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(DB_SHADER_CONTROL, SQ_PGM_PS);
   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_INDEX_DB_SHADER_CONTROL);
}

static void
terakan_state_draw_apply_pa_cl_clip_cntl(struct terakan_gfx_command_writer * const command_writer)
{
   bool const modified =
      command_writer->hw_state_draw.pa_cl_clip_cntl != command_writer->state_draw.pa_cl_clip_cntl;
   command_writer->hw_state_draw.pa_cl_clip_cntl = command_writer->state_draw.pa_cl_clip_cntl;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL, modified);

   bool const dx_clip_space_def =
      G_028810_DX_CLIP_SPACE_DEF(command_writer->state_draw.pa_cl_clip_cntl) != 0;
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(VIEWPORT, PA_CL_CLIP_CNTL);
   if (command_writer->state_draw.viewport.from_apply_pa_cl_clip_cntl.dx_clip_space_def !=
       dx_clip_space_def) {
      command_writer->state_draw.viewport.from_apply_pa_cl_clip_cntl.dx_clip_space_def =
         dx_clip_space_def;
      command_writer->state_draw.viewport.viewports_pending.pa_cl_vport_z_scale_offset =
         BITFIELD_MASK(ARRAY_SIZE(command_writer->state_draw.viewport.viewports));
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_VIEWPORT);
   }
}

static void
terakan_state_draw_apply_viewport(struct terakan_gfx_command_writer * const command_writer)
{
   uint8_t scale_offset_z_min_max_lowest_modified =
      ARRAY_SIZE(command_writer->hw_state_draw.viewports);
   uint8_t scissor_lowest_modified = ARRAY_SIZE(command_writer->hw_state_draw.viewports);

   uint16_t const used_viewport_mask =
      (uint16_t)BITFIELD_MASK(command_writer->state_draw.viewport.count);

   /* PA_CL_VPORT_X/YSCALE/OFFSET */
   u_foreach_bit (
      viewport_index,
      command_writer->state_draw.viewport.viewports_pending.pa_cl_vport_xy_scale_offset &
         used_viewport_mask) {
      if (memcmp(
             command_writer->hw_state_draw.viewports[viewport_index].pa_cl_vport_xyz_scale_offset,
             command_writer->state_draw.viewport.viewports[viewport_index]
                .pa_cl_vport_xy_scale_offset,
             sizeof(float) * 2 * 2) != 0) {
         memcpy(
            command_writer->hw_state_draw.viewports[viewport_index].pa_cl_vport_xyz_scale_offset,
            command_writer->state_draw.viewport.viewports[viewport_index]
               .pa_cl_vport_xy_scale_offset,
            sizeof(float) * 2 * 2);
         scale_offset_z_min_max_lowest_modified =
            MIN2(viewport_index, scale_offset_z_min_max_lowest_modified);
      }
   }
   command_writer->state_draw.viewport.viewports_pending.pa_cl_vport_xy_scale_offset &=
      ~used_viewport_mask;

   /* PA_CL_VPORT_ZSCALE/OFFSET */
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(VIEWPORT, PA_CL_CLIP_CNTL);
   bool const dx_clip_space_def =
      command_writer->state_draw.viewport.from_apply_pa_cl_clip_cntl.dx_clip_space_def;
   u_foreach_bit (viewport_index,
                  command_writer->state_draw.viewport.viewports_pending.pa_cl_vport_z_scale_offset &
                     used_viewport_mask) {
      float const * const z_scale_offset =
         command_writer->state_draw.viewport.viewports[viewport_index]
            .pa_cl_vport_z_gl_dx_scale_offset[(int)dx_clip_space_def];
      if (memcmp(
             command_writer->hw_state_draw.viewports[viewport_index].pa_cl_vport_xyz_scale_offset[2],
             z_scale_offset, sizeof(float) * 2) != 0) {
         memcpy(
            command_writer->hw_state_draw.viewports[viewport_index].pa_cl_vport_xyz_scale_offset[2],
            z_scale_offset, sizeof(float) * 2);
         scale_offset_z_min_max_lowest_modified =
            MIN2(viewport_index, scale_offset_z_min_max_lowest_modified);
      }
   }
   command_writer->state_draw.viewport.viewports_pending.pa_cl_vport_z_scale_offset &=
      ~used_viewport_mask;

   /* PA_CL_GB_VERT/HORZ_CLIP/DISC_ADJ */
   if (command_writer->state_draw.viewport.pa_cl_gb_pending) {
      float pa_cl_gb_vert_horz_clip_disc_adj[][2] = {{FLT_MAX, 1.0f}, {FLT_MAX, 1.0f}};
      for (uint32_t viewport_index = 0; viewport_index < command_writer->state_draw.viewport.count;
           ++viewport_index) {
         for (int axis = 0; axis < 2; ++axis) {
            pa_cl_gb_vert_horz_clip_disc_adj[axis][0] =
               MIN2(command_writer->state_draw.viewport.viewports[viewport_index]
                       .pa_cl_gb_vert_horz_clip_adj[axis],
                    pa_cl_gb_vert_horz_clip_disc_adj[axis][0]);
         }
      }
      /* TODO(Triang3l): Discard rectangle ratio for points and lines. */
      bool const pa_cl_gb_modified =
         memcmp(command_writer->hw_state_draw.pa_cl_gb_vert_horz_clip_disc_adj,
                pa_cl_gb_vert_horz_clip_disc_adj, sizeof(pa_cl_gb_vert_horz_clip_disc_adj)) != 0;
      memcpy(command_writer->hw_state_draw.pa_cl_gb_vert_horz_clip_disc_adj,
             pa_cl_gb_vert_horz_clip_disc_adj, sizeof(pa_cl_gb_vert_horz_clip_disc_adj));
      terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                    TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_GB, pa_cl_gb_modified);
      command_writer->state_draw.viewport.pa_cl_gb_pending = false;
   }

   /* PA_SC_VPORT_SCISSOR */
   u_foreach_bit (viewport_index,
                  command_writer->state_draw.viewport.viewports_pending.pa_sc_vport_scissor &
                     used_viewport_mask) {
      uint16_t const * const viewport_scissor =
         command_writer->state_draw.viewport.viewports[viewport_index]
            .pa_sc_vport_scissor_tl_br_xy[0];
      uint16_t const * const generic_scissor =
         command_writer->state_draw.viewport.pa_sc_vport_generic_scissor_tl_br_xy[viewport_index][0];
      uint16_t scissor[] = {
         MAX2(viewport_scissor[0], generic_scissor[0]),
         MAX2(viewport_scissor[1], generic_scissor[1]),
         MIN2(viewport_scissor[2], generic_scissor[2]),
         MIN2(viewport_scissor[3], generic_scissor[3]),
      };
      terakan_state_draw_finalize_scissor(scissor);
      uint32_t const pa_sc_vport_scissor[] = {
         S_028250_TL_X(scissor[0]) | S_028250_TL_Y(scissor[1]) | S_028250_WINDOW_OFFSET_DISABLE(1),
         S_028254_BR_X(scissor[2]) | S_028254_BR_Y(scissor[3]),
      };
      if (memcmp(command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_scissor,
                 pa_sc_vport_scissor, sizeof(pa_sc_vport_scissor)) != 0) {
         memcpy(command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_scissor,
                pa_sc_vport_scissor, sizeof(pa_sc_vport_scissor));
         scissor_lowest_modified = MIN2(viewport_index, scissor_lowest_modified);
      }
   }
   command_writer->state_draw.viewport.viewports_pending.pa_sc_vport_scissor &= ~used_viewport_mask;

   /* PA_SC_VPORT_ZMIN/MAX */
   if (command_writer->state_draw.viewport.pa_sc_vport_z_min_0_max_1) {
      float const zero_one[] = {0.0f, 1.0f};
      u_foreach_bit (viewport_index,
                     command_writer->state_draw.viewport.viewports_pending.pa_sc_vport_z_min_max &
                        used_viewport_mask) {
         if (memcmp(command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_z_min_max,
                    zero_one, sizeof(float) * 2) != 0) {
            memcpy(command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_z_min_max,
                   zero_one, sizeof(float) * 2);
            scale_offset_z_min_max_lowest_modified =
               MIN2(viewport_index, scale_offset_z_min_max_lowest_modified);
         }
      }
   } else {
      u_foreach_bit (viewport_index,
                     command_writer->state_draw.viewport.viewports_pending.pa_sc_vport_z_min_max &
                        used_viewport_mask) {
         if (memcmp(
                command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_z_min_max,
                command_writer->state_draw.viewport.viewports[viewport_index].pa_sc_vport_z_min_max,
                sizeof(float) * 2) != 0) {
            memcpy(
               command_writer->hw_state_draw.viewports[viewport_index].pa_sc_vport_z_min_max,
               command_writer->state_draw.viewport.viewports[viewport_index].pa_sc_vport_z_min_max,
               sizeof(float) * 2);
            scale_offset_z_min_max_lowest_modified =
               MIN2(viewport_index, scale_offset_z_min_max_lowest_modified);
         }
      }
   }
   command_writer->state_draw.viewport.viewports_pending.pa_sc_vport_z_min_max &=
      ~used_viewport_mask;

   terakan_hw_state_draw_update_viewports(
      &command_writer->hw_state_draw, command_writer->state_draw.viewport.count,
      scale_offset_z_min_max_lowest_modified, scissor_lowest_modified);
}

static void
terakan_state_draw_apply_pa_su_sc_mode_cntl(struct terakan_gfx_command_writer * const command_writer)
{
   bool const modified = command_writer->hw_state_draw.pa_su_sc_mode_cntl !=
                         command_writer->state_draw.pa_su_sc_mode_cntl;
   command_writer->hw_state_draw.pa_su_sc_mode_cntl = command_writer->state_draw.pa_su_sc_mode_cntl;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_SC_MODE_CNTL, modified);
}

static void
terakan_state_draw_apply_pa_cl_vte_cntl(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t const pa_cl_vte_cntl = S_028818_VPORT_X_SCALE_ENA(1) | S_028818_VPORT_X_OFFSET_ENA(1) |
                                   S_028818_VPORT_Y_SCALE_ENA(1) | S_028818_VPORT_Y_OFFSET_ENA(1) |
                                   S_028818_VPORT_Z_SCALE_ENA(1) | S_028818_VPORT_Z_OFFSET_ENA(1) |
                                   S_028818_VTX_W0_FMT(1);
   bool const modified = command_writer->hw_state_draw.pa_cl_vte_cntl != pa_cl_vte_cntl;
   command_writer->hw_state_draw.pa_cl_vte_cntl = pa_cl_vte_cntl;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_PA_CL_VTE_CNTL, modified);
}

static void
terakan_state_draw_apply_pa_sc_mode_cntl_0(struct terakan_gfx_command_writer * const command_writer)
{
   /* TODO(Triang3l): MSAA_ENABLE, LINE_STIPPLE_ENABLE from a variable. */
   uint32_t const pa_sc_mode_cntl_0 = S_028A48_VPORT_SCISSOR_ENABLE(1);
   bool const modified = command_writer->hw_state_draw.pa_sc_mode_cntl_0 != pa_sc_mode_cntl_0;
   command_writer->hw_state_draw.pa_sc_mode_cntl_0 = pa_sc_mode_cntl_0;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_MODE_CNTL_0, modified);
}

static void
terakan_state_draw_apply_pa_sc_aa_mask(struct terakan_gfx_command_writer * const command_writer)
{
   bool const modified =
      command_writer->hw_state_draw.pa_sc_aa_mask != command_writer->state_draw.pa_sc_aa_mask;
   command_writer->hw_state_draw.pa_sc_aa_mask = command_writer->state_draw.pa_sc_aa_mask;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_PA_SC_AA_MASK, modified);
}

static void
terakan_state_draw_apply_db_depth_stencil_buffer(
   struct terakan_gfx_command_writer * const command_writer)
{
   bool modified = command_writer->hw_state_draw.db_depth_stencil_buffer.bo !=
                   command_writer->state_draw.db_depth_stencil_buffer.bo;
   command_writer->hw_state_draw.db_depth_stencil_buffer.bo =
      command_writer->state_draw.db_depth_stencil_buffer.bo;

   uint32_t poly_offset_z_format = V_028040_Z_INVALID;

   if (command_writer->state_draw.db_depth_stencil_buffer.bo != NULL) {
      modified =
         modified || memcmp(&command_writer->hw_state_draw.db_depth_stencil_buffer.descriptor,
                            &command_writer->state_draw.db_depth_stencil_buffer.descriptor,
                            sizeof(struct terakan_depth_stencil_descriptor)) != 0;
      command_writer->hw_state_draw.db_depth_stencil_buffer.descriptor =
         command_writer->state_draw.db_depth_stencil_buffer.descriptor;

      poly_offset_z_format =
         G_028040_FORMAT(command_writer->state_draw.db_depth_stencil_buffer.descriptor.z_info);
   }

   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER, modified);

   if (poly_offset_z_format == V_028040_Z_INVALID) {
      poly_offset_z_format = TERAKAN_STATE_DRAW_PA_SU_POLY_OFFSET_DB_FMT_CNTL_NO_ATTACHMENT_FORMAT;
   }
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(PA_SU_POLY_OFFSET_DB_FMT_CNTL, DB_DEPTH_STENCIL_BUFFER);
   if (command_writer->state_draw.pa_su_poly_offset_db_fmt_cntl.from_apply_db_depth_stencil_buffer
          .poly_offset_z_format != poly_offset_z_format) {
      command_writer->state_draw.pa_su_poly_offset_db_fmt_cntl.from_apply_db_depth_stencil_buffer
         .poly_offset_z_format = poly_offset_z_format;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_DB_FMT_CNTL);
   }
}

static void
terakan_state_draw_apply_pa_su_poly_offset_db_fmt_cntl(
   struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t pa_su_poly_offset_db_fmt_cntl;
   if (command_writer->state_draw.pa_su_poly_offset_db_fmt_cntl.representation ==
       VK_DEPTH_BIAS_REPRESENTATION_FLOAT_EXT) {
      pa_su_poly_offset_db_fmt_cntl = S_028B78_POLY_OFFSET_NEG_NUM_DB_BITS(0);
   } else {
      TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(PA_SU_POLY_OFFSET_DB_FMT_CNTL, DB_DEPTH_STENCIL_BUFFER);
      uint32_t const z_format = command_writer->state_draw.pa_su_poly_offset_db_fmt_cntl
                                   .from_apply_db_depth_stencil_buffer.poly_offset_z_format;
      switch (z_format) {
      case V_028040_Z_16:
         pa_su_poly_offset_db_fmt_cntl = S_028B78_POLY_OFFSET_NEG_NUM_DB_BITS(-16);
         break;
      case V_028040_Z_24:
         pa_su_poly_offset_db_fmt_cntl = S_028B78_POLY_OFFSET_NEG_NUM_DB_BITS(-24);
         break;
      default:
         assert(z_format == V_028040_Z_32_FLOAT);
         if (command_writer->state_draw.pa_su_poly_offset_db_fmt_cntl.representation ==
             VK_DEPTH_BIAS_REPRESENTATION_LEAST_REPRESENTABLE_VALUE_FORCE_UNORM_EXT) {
            /* Number of mantissa bits plus one as defined in the specification, also matching the
             * unorm24 depth bias formula as the use case described in the VK_EXT_depth_bias_control
             * proposal is emulation of unorm24 depth using float32.
             */
            pa_su_poly_offset_db_fmt_cntl = S_028B78_POLY_OFFSET_NEG_NUM_DB_BITS(-24);
         } else {
            pa_su_poly_offset_db_fmt_cntl =
               S_028B78_POLY_OFFSET_NEG_NUM_DB_BITS(-23) | S_028B78_POLY_OFFSET_DB_IS_FLOAT_FMT(1);
         }
         break;
      }
      if (!G_028B78_POLY_OFFSET_DB_IS_FLOAT_FMT(pa_su_poly_offset_db_fmt_cntl) &&
          !command_writer->state_draw.pa_su_poly_offset_db_fmt_cntl.representation_exact) {
         /* According to testing done on Barts by drawing a primitive at Z = 0 with a
          * DB_IS_FLOAT_FMT(0) constant depth bias and reading gl_FragCoord.z, the hardware simply
          * exponent-biases the constant depth bias for unorm formats - that is, the bias ends up
          * being scaled by 1/(2^n), where n is -NEG_NUM_DB_BITS, or 1/0x10000 for unorm16 and
          * 1/0x1000000 for unorm24 if NEG_NUM_DB_BITS is minus the number of bits in the format.
          *
          * However, in a unorm format with n bits, the distance between adjacent values is
          * 1/(2^n - 1), or 1/0xFFFF for unorm16 and 1/0xFFFFFF for unorm24.
          *
          * Because of that, a depth bias of 1, if simply exponent-biased by minus the number of
          * bits, may be insufficient to separate overlapping primitives.
          *
          * For example, if the format is unorm16, and the incoming depth is 1.5 / 0xFFFF, applying
          * a bias of 1 / 0x10000 results in 2.4999... / 0xFFFF - and both 1.5 and 2.4999... end up
          * rounded to 2 in the unorm conversion.
          *
          * This issue is also described in the PA_SU_POLY_OFFSET_DB_FMT_CNTL setup comment in PAL:
          * https://github.com/GPUOpen-Drivers/pal/blob/abb22ae22308954cd9ce76b526c2f805e2dec1ba/src/core/hw/gfxip/gfx9/gfx9DepthStencilView.cpp#L254
          *
          * Vulkan provides two options for controlling this for a unorm depth bias representation:
          * - depthBiasExact = VK_TRUE: The bias is scaled by 2^-n, where n is the number of bits
          *   in the depth attachment if it's fixed-point. This is the hardware behavior when
          *   NEG_NUM_DB_BITS is minus the number of depth format bits.
          * - depthBiasExact = VK_FALSE: The bias is scaled by "the smallest difference in
          *   framebuffer coordinate z values that is guaranteed to remain distinct throughout
          *   polygon rasterization and in the depth attachment" which is an
          *   implementation-dependent value that must be at most 2 * 2^-n.
          *
          * To ensure separation in the case described above, implement depthBiasExact = VK_FALSE by
          * scaling the bias by 2 * 2^-n.
          *
          * Note that it's not possible to implement depthBiasExact = VK_FALSE with a perfectly
          * precise scale via adjusting PA_SU_POLY_OFFSET_FRONT/BACK_OFFSET, as unlike 0xFFFFFF
          * itself, 1/0xFFFFFF doesn't have a float32 representation distinguishable from
          * 1/0x1000000 or 1/0xFFFFFE, and multiplying an integer by 1/0x1000000 and then by
          * 0xFFFFFF and rounding produces a different value for half of the 0 to 1 range.
          */
         pa_su_poly_offset_db_fmt_cntl =
            (pa_su_poly_offset_db_fmt_cntl & C_028B78_POLY_OFFSET_NEG_NUM_DB_BITS) |
            S_028B78_POLY_OFFSET_NEG_NUM_DB_BITS(
               G_028B78_POLY_OFFSET_NEG_NUM_DB_BITS(pa_su_poly_offset_db_fmt_cntl) + 1);
      }
   }
   bool const modified =
      command_writer->hw_state_draw.pa_su_poly_offset_db_fmt_cntl != pa_su_poly_offset_db_fmt_cntl;
   command_writer->hw_state_draw.pa_su_poly_offset_db_fmt_cntl = pa_su_poly_offset_db_fmt_cntl;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_DB_FMT_CNTL,
                                 modified);
}

static void
terakan_state_draw_apply_db_render_override(struct terakan_gfx_command_writer * const command_writer)
{
   bool const modified = command_writer->hw_state_draw.db_render_override !=
                         command_writer->state_draw.db_render_override;
   command_writer->hw_state_draw.db_render_override = command_writer->state_draw.db_render_override;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_DB_RENDER_OVERRIDE, modified);
}

static void
terakan_state_draw_apply_db_stencilrefmask(struct terakan_gfx_command_writer * const command_writer)
{
   bool const modified =
      memcmp(command_writer->hw_state_draw.db_stencilrefmask_front_back,
             command_writer->state_draw.db_stencilrefmask_front_back, sizeof(uint32_t) * 2) != 0;
   memcpy(command_writer->hw_state_draw.db_stencilrefmask_front_back,
          command_writer->state_draw.db_stencilrefmask_front_back, sizeof(uint32_t) * 2);
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_DB_STENCILREFMASK, modified);
}

static void
terakan_state_draw_apply_db_depth_control(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t db_depth_control = command_writer->state_draw.db_depth_control;

   /* Clear irrelevant state to avoid state changes if it ends up being different.
    * Also force BACKFACE_ENABLE for simplicity of changing the state, as in Vulkan front and back
    * face stencil state is always specified separately, to ensure it's never lost.
    */

   if (G_028800_STENCIL_ENABLE(db_depth_control)) {
      db_depth_control |= S_028800_BACKFACE_ENABLE(1);
   } else {
      db_depth_control &= C_028800_BACKFACE_ENABLE & C_028800_STENCILFUNC & C_028800_STENCILFAIL &
                          C_028800_STENCILZPASS & C_028800_STENCILZFAIL & C_028800_STENCILFUNC_BF &
                          C_028800_STENCILFAIL_BF & C_028800_STENCILZPASS_BF &
                          C_028800_STENCILZFAIL_BF;
   }

   if (!G_028800_Z_ENABLE(db_depth_control)) {
      db_depth_control &= C_028800_Z_WRITE_ENABLE & C_028800_ZFUNC;
   }

   bool const modified = command_writer->hw_state_draw.db_depth_control != db_depth_control;
   command_writer->hw_state_draw.db_depth_control = db_depth_control;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_CONTROL, modified);
}

static void
terakan_state_draw_apply_color_attachment_usage(
   struct terakan_gfx_command_writer * const command_writer)
{
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_BLEND_CONTROL, COLOR_ATTACHMENT_USAGE);
   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_INDEX_CB_BLEND_CONTROL);
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_COLOR_RTV, COLOR_ATTACHMENT_USAGE);
   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_INDEX_CB_COLOR_RTV);
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_TARGET_MASK, COLOR_ATTACHMENT_USAGE);
   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_INDEX_CB_TARGET_MASK);
}

static void
terakan_state_draw_apply_logic_op(struct terakan_gfx_command_writer * const command_writer)
{
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_BLEND_CONTROL, LOGIC_OP);
   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_INDEX_CB_BLEND_CONTROL);
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_COLOR_CONTROL, LOGIC_OP);
   terakan_state_draw_set_pending(&command_writer->state_draw,
                                  TERAKAN_STATE_DRAW_INDEX_CB_COLOR_CONTROL);
}

static void
terakan_state_draw_apply_cb_blend_control(struct terakan_gfx_command_writer * const command_writer)
{
   bool dual_source = false;

   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_BLEND_CONTROL, COLOR_ATTACHMENT_USAGE);
   unsigned attachments_remaining =
      command_writer->state_draw.color_attachment_usage.from_apply_sq_pgm_ps.written_by_shader;
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_BLEND_CONTROL, LOGIC_OP);
   if (command_writer->state_draw.logic_op.enable) {
      /* Section 27.2. "Logical Operations" of the Vulkan 1.3.278 specification says:
       *
       *     "If logicOpEnable is VK_TRUE, then a logical operation selected by logicOp is applied
       *     between each color attachment and the fragment’s corresponding output value, and
       *     blending of all attachments is treated as if it were disabled."
       *
       * Combining blending with any ROP3 other than 0xCC is also prohibited according to Radeon
       * Evergreen / Northern Islands Acceleration.
       */
      uint32_t hw_color_count = (uint32_t)util_bitcount(attachments_remaining);
      for (uint32_t hw_color_index = 0; hw_color_index < hw_color_count; ++hw_color_index) {
         terakan_hw_state_draw_set_cb_blend_control(&command_writer->hw_state_draw, hw_color_index,
                                                    0);
      }
   } else {
      for (uint32_t hw_color_index = 0; attachments_remaining; ++hw_color_index) {
         unsigned const attachment_index = (unsigned)u_bit_scan(&attachments_remaining);

         uint32_t cb_blend_control =
            command_writer->state_draw.cb_blend_control.attachments[attachment_index];

         if (attachment_index == 0 && G_028780_BLEND_CONTROL_ENABLE(cb_blend_control)) {
            uint32_t blend_factors_used =
               ((uint32_t)1 << G_028780_COLOR_SRCBLEND(cb_blend_control)) |
               ((uint32_t)1 << G_028780_COLOR_DESTBLEND(cb_blend_control));
            if (G_028780_SEPARATE_ALPHA_BLEND(cb_blend_control)) {
               blend_factors_used |= ((uint32_t)1 << G_028780_ALPHA_SRCBLEND(cb_blend_control)) |
                                     ((uint32_t)1 << G_028780_ALPHA_DESTBLEND(cb_blend_control));
            }
            uint32_t const dual_source_blend_factor_bits =
               ((uint32_t)1 << V_028780_BLEND_SRC1_COLOR) |
               ((uint32_t)1 << V_028780_BLEND_INV_SRC1_COLOR) |
               ((uint32_t)1 << V_028780_BLEND_SRC1_ALPHA) |
               ((uint32_t)1 << V_028780_BLEND_INV_SRC1_ALPHA);
            if (blend_factors_used & dual_source_blend_factor_bits) {
               TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_BLEND_CONTROL, COLOR_ATTACHMENT_USAGE);
               /* Only enable dual-source blending if the compaction in the shader makes it possible
                * to assume in the rest of the state logic that the dual-source blending factor is
                * exported at index 1 after the compaction.
                */
               if (likely(!(~command_writer->state_draw.color_attachment_usage.from_apply_sq_pgm_ps
                                .written_by_shader &
                            0b11))) {
                  dual_source = true;
               } else {
                  /* Section 30.1.2. "Dual-Source Blending" of the Vulkan 1.3.278 specification says:
                   *
                   *     "If the second color input to the blender is not written in the shader, or
                   *     if no output is bound to the second input of a blender, the value of the
                   *     second input is undefined."
                   *
                   * Disable dual-source blending for hardware state consistency, replacing factors
                   * specifically involving the undefined second input.
                   */
                  if (((uint32_t)1 << G_028780_COLOR_SRCBLEND(cb_blend_control)) &
                      dual_source_blend_factor_bits) {
                     cb_blend_control &= C_028780_COLOR_SRCBLEND;
                  }
                  if (((uint32_t)1 << G_028780_COLOR_DESTBLEND(cb_blend_control)) &
                      dual_source_blend_factor_bits) {
                     cb_blend_control &= C_028780_COLOR_DESTBLEND;
                  }
                  if (G_028780_SEPARATE_ALPHA_BLEND(cb_blend_control)) {
                     if (((uint32_t)1 << G_028780_ALPHA_SRCBLEND(cb_blend_control)) &
                         dual_source_blend_factor_bits) {
                        cb_blend_control &= C_028780_ALPHA_SRCBLEND;
                     }
                     if (((uint32_t)1 << G_028780_ALPHA_DESTBLEND(cb_blend_control)) &
                         dual_source_blend_factor_bits) {
                        cb_blend_control &= C_028780_ALPHA_DESTBLEND;
                     }
                  }
               }
            }
         }

         if (!G_028780_BLEND_CONTROL_ENABLE(cb_blend_control)) {
            /* Don't emit setting packets if the bits that have been changed don't matter anyway. */
            cb_blend_control = 0;
         }

         terakan_hw_state_draw_set_cb_blend_control(&command_writer->hw_state_draw, hw_color_index,
                                                    cb_blend_control);
      }
   }

   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_COLOR_RTV, CB_BLEND_CONTROL);
   if (command_writer->state_draw.cb_color_rtv.from_apply_cb_blend_control.dual_source_blend !=
       dual_source) {
      command_writer->state_draw.cb_color_rtv.from_apply_cb_blend_control.dual_source_blend =
         dual_source;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_CB_COLOR_RTV);
   }
}

static void
terakan_state_draw_apply_cb_color_rtv(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t attachment_format_masks = 0b0;

   bool cb_dual_export_allowed = true;

   /* TODO(Triang3l): Merge application of color attachment bindings with color write mask
    * application, unbinding color attachments with writing disabled to skip 32bpc exports for them.
    */

   {
      TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_COLOR_RTV, CB_BLEND_CONTROL);
      TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_COLOR_RTV, COLOR_ATTACHMENT_USAGE);
      unsigned attachments_remaining =
         command_writer->state_draw.cb_color_rtv.from_apply_cb_blend_control.dual_source_blend
            ? 0b1
            : command_writer->state_draw.color_attachment_usage.from_apply_sq_pgm_ps
                 .written_by_shader;
      for (uint32_t hw_color_index = 0; attachments_remaining; ++hw_color_index) {
         unsigned const attachment_index = (unsigned)u_bit_scan(&attachments_remaining);
         struct terakan_state_draw_cb_color const * const attachment =
            &command_writer->state_draw.cb_color_rtv.attachments[attachment_index];
         /* Even if the attachment doesn't have a non-VK_NULL_HANDLE image bound and is thus
          * expected to be disabled in CB_TARGET_MASK, unbind it in hardware if the shader exports
          * to it because CB_COLOR# also has effect on the export itself (SOURCE_FORMAT at least).
          */
         terakan_hw_state_draw_set_cb_color(&command_writer->hw_state_draw, hw_color_index,
                                            attachment->bo, &attachment->color, &attachment->meta,
                                            false);
         if (attachment->bo != NULL) {
            attachment_format_masks |=
               (uint32_t)(terascale_format_cb_color_export_component_masks
                             [terascale_format_channel_count[G_028C70_FORMAT(attachment->color.info)]]
                             [G_028C70_COMP_SWAP(attachment->color.info)])
               << (4 * attachment_index);
            cb_dual_export_allowed &=
               G_028C70_SOURCE_FORMAT(attachment->color.info) != V_028C70_EXPORT_4C_32BPC;
         }
      }
   }

   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_COLOR_RTV, CB_BLEND_CONTROL);
   if (command_writer->state_draw.cb_color_rtv.from_apply_cb_blend_control.dual_source_blend) {
      /* Unbind MRT 1, and set SOURCE_FORMAT of it to match that of MRT 0 so 2 exported quads come
       * to blending in the same format according to Radeon Evergreen / Northern Islands
       * Acceleration.
       */
      terakan_hw_state_draw_set_cb_color1_dual_source(
         &command_writer->hw_state_draw,
         command_writer->state_draw.cb_color_rtv.attachments[0].bo != NULL
            ? G_028C70_SOURCE_FORMAT(
                 command_writer->state_draw.cb_color_rtv.attachments[0].color.info)
            : V_028C70_EXPORT_4C_16BPC);

      /* Unbind all targets above 1 for safety if the shader writes to them. */
      TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_COLOR_RTV, COLOR_ATTACHMENT_USAGE);
      uint32_t const attachment_count = (uint32_t)util_bitcount(
         command_writer->state_draw.color_attachment_usage.from_apply_sq_pgm_ps.written_by_shader);
      for (uint32_t hw_color_index = 2; hw_color_index < attachment_count; ++hw_color_index) {
         terakan_hw_state_draw_set_cb_color(&command_writer->hw_state_draw, hw_color_index, NULL,
                                            NULL, NULL, false);
      }
   }

   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_TARGET_MASK, CB_COLOR_RTV);
   if (command_writer->state_draw.cb_target_mask.from_apply_cb_color_rtv.attachment_format_masks !=
       attachment_format_masks) {
      command_writer->state_draw.cb_target_mask.from_apply_cb_color_rtv.attachment_format_masks =
         attachment_format_masks;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_CB_TARGET_MASK);
   }

   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(DB_SHADER_CONTROL, CB_COLOR_RTV);
   if (command_writer->state_draw.db_shader_control.from_apply_cb_color_rtv.cb_dual_export_allowed !=
       cb_dual_export_allowed) {
      command_writer->state_draw.db_shader_control.from_apply_cb_color_rtv.cb_dual_export_allowed =
         cb_dual_export_allowed;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_DB_SHADER_CONTROL);
   }
}

static void
terakan_state_draw_apply_cb_target_mask(struct terakan_gfx_command_writer * const command_writer)
{
   uint32_t cb_target_mask_rtv = 0b0;

   uint8_t const attachment_write_enable =
      command_writer->state_draw.cb_target_mask.attachment_write_enable;
   uint32_t const attachment_format_masks =
      command_writer->state_draw.cb_target_mask.from_apply_cb_color_rtv.attachment_format_masks;
   {
      TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_TARGET_MASK, COLOR_ATTACHMENT_USAGE);
      unsigned attachments_remaining =
         command_writer->state_draw.color_attachment_usage.from_apply_sq_pgm_ps.written_by_shader;
      for (uint32_t hw_color_index = 0; attachments_remaining; ++hw_color_index) {
         TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_TARGET_MASK, CB_COLOR_RTV);
         unsigned const attachment_index = (unsigned)u_bit_scan(&attachments_remaining);
         if (!(attachment_write_enable & ((uint8_t)1 << attachment_index))) {
            continue;
         }
         uint32_t const attachment_format_mask =
            (attachment_format_masks >> (4 * attachment_index)) & 0b1111;
         uint32_t attachment_target_mask =
            attachment_format_mask &
            command_writer->state_draw.cb_target_mask.attachment_write_masks[attachment_index];
         if (!attachment_target_mask) {
            /* No components present in the attachment format are enabled, ignore (here, and via
             * `any_rtv_enabled`, in CB_COLOR_CONTROL MODE setup) the write mask specified by the
             * application for the missing components.
             * One of the particular cases of this is an attachment image not being bound also, in
             * which case CB_TARGET_MASK should be disabled, so Terakan can simply leave CB_COLOR#
             * registers undefined or unchanged for unbound targets without explicitly setting their
             * format to INVALID.
             */
            continue;
         }
         /* Force missing components to written, to clearly specify in case all present components
          * are enabled that a read-modify-write is not needed regardless of whether writing to
          * the missing components is enabled by the application. This is based purely on a guess
          * about a hypothetical situation though, for additional safety - the actual effect of a
          * non-1111 mask on the CB performance compared to a 1111 one in reality hasn't been
          * tested.
          */
         attachment_target_mask |= attachment_format_mask ^ 0b1111;
         cb_target_mask_rtv |= attachment_target_mask << (4 * hw_color_index);
      }
   }

   /* TODO(Triang3l): UAVs. */

   bool const modified = command_writer->hw_state_draw.cb_target_mask != cb_target_mask_rtv;
   command_writer->hw_state_draw.cb_target_mask = cb_target_mask_rtv;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_CB_TARGET_MASK, modified);

   bool const any_rtv_enabled = cb_target_mask_rtv != 0;
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_COLOR_CONTROL, CB_TARGET_MASK);
   if (command_writer->state_draw.cb_color_control.from_apply_cb_target_mask.any_rtv_enabled !=
       any_rtv_enabled) {
      command_writer->state_draw.cb_color_control.from_apply_cb_target_mask.any_rtv_enabled =
         any_rtv_enabled;
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_CB_COLOR_CONTROL);
   }
}

static void
terakan_state_draw_apply_cb_color_control(struct terakan_gfx_command_writer * const command_writer)
{
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_COLOR_CONTROL, CB_TARGET_MASK);
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(CB_COLOR_CONTROL, LOGIC_OP);
   /* TODO(Triang3l): Enable CB_NORMAL too if any UAV is used. */
   /* Ignore the logical operation specified by the application and skip potential ROP3 state
    * changes if no RTV is written to, and thus it's irrelevant.
    */
   uint32_t const cb_color_control =
      S_028808_MODE(
         command_writer->state_draw.cb_color_control.from_apply_cb_target_mask.any_rtv_enabled
            ? V_028808_CB_NORMAL
            : V_028808_CB_DISABLE) |
      S_028808_ROP3(
         command_writer->state_draw.cb_color_control.from_apply_cb_target_mask.any_rtv_enabled &&
               command_writer->state_draw.logic_op.enable
            ? command_writer->state_draw.logic_op.rop3
            : 0xCC);
   bool const modified = command_writer->hw_state_draw.cb_color_control != cb_color_control;
   command_writer->hw_state_draw.cb_color_control = cb_color_control;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_CB_COLOR_CONTROL, modified);
}

static void
terakan_state_draw_apply_db_shader_control(struct terakan_gfx_command_writer * const command_writer)
{
   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(DB_SHADER_CONTROL, SQ_PGM_PS);
   uint32_t db_shader_control = command_writer->state_draw.sq_pgm_ps.fs != NULL
                                   ? command_writer->state_draw.sq_pgm_ps.fs->fs.db_shader_control
                                   : S_02880C_Z_ORDER(V_02880C_EARLY_Z_THEN_LATE_Z) |
                                        S_02880C_DUAL_EXPORT_ENABLE(1) |
                                        S_02880C_DB_SOURCE_FORMAT(V_02880C_EXPORT_DB_TWO);

   TERAKAN_STATE_DRAW_ASSERT_DEPENDS_ON(DB_SHADER_CONTROL, CB_COLOR_RTV);
   if (!command_writer->state_draw.db_shader_control.from_apply_cb_color_rtv.cb_dual_export_allowed) {
      db_shader_control &= C_02880C_DUAL_EXPORT_ENABLE;
   }

   bool const modified = command_writer->hw_state_draw.db_shader_control != db_shader_control;
   command_writer->hw_state_draw.db_shader_control = db_shader_control;
   terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                 TERAKAN_HW_STATE_DRAW_INDEX_DB_SHADER_CONTROL, modified);
}

static terakan_state_draw_apply_function const
   terakan_state_draw_apply_functions[TERAKAN_STATE_DRAW_INDEX_COUNT] = {
      [TERAKAN_STATE_DRAW_INDEX_VGT_INDEX_TYPE] = terakan_state_draw_apply_vgt_index_type,
      [TERAKAN_STATE_DRAW_INDEX_VGT_PRIMITIVE_TYPE] = terakan_state_draw_apply_vgt_primitive_type,
      [TERAKAN_STATE_DRAW_INDEX_VGT_INDEX_OFFSET] = terakan_state_draw_apply_vgt_index_offset,
      [TERAKAN_STATE_DRAW_INDEX_SQ_PGM_LS_ES_GS_VS] = terakan_state_draw_apply_sq_pgm_ls_es_gs_vs,
      [TERAKAN_STATE_DRAW_INDEX_SQ_PGM_FS] = terakan_state_draw_apply_sq_pgm_fs,
      [TERAKAN_STATE_DRAW_INDEX_SQ_RESOURCES_FS] = terakan_state_draw_apply_sq_resources_fs,
      [TERAKAN_STATE_DRAW_INDEX_SQ_PGM_PS] = terakan_state_draw_apply_sq_pgm_ps,
      [TERAKAN_STATE_DRAW_INDEX_PA_CL_CLIP_CNTL] = terakan_state_draw_apply_pa_cl_clip_cntl,
      [TERAKAN_STATE_DRAW_INDEX_VIEWPORT] = terakan_state_draw_apply_viewport,
      [TERAKAN_STATE_DRAW_INDEX_PA_SU_SC_MODE_CNTL] = terakan_state_draw_apply_pa_su_sc_mode_cntl,
      [TERAKAN_STATE_DRAW_INDEX_PA_CL_VTE_CNTL] = terakan_state_draw_apply_pa_cl_vte_cntl,
      [TERAKAN_STATE_DRAW_INDEX_PA_SC_MODE_CNTL_0] = terakan_state_draw_apply_pa_sc_mode_cntl_0,
      [TERAKAN_STATE_DRAW_INDEX_PA_SC_AA_MASK] = terakan_state_draw_apply_pa_sc_aa_mask,
      [TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER] =
         terakan_state_draw_apply_db_depth_stencil_buffer,
      [TERAKAN_STATE_DRAW_INDEX_PA_SU_POLY_OFFSET_DB_FMT_CNTL] =
         terakan_state_draw_apply_pa_su_poly_offset_db_fmt_cntl,
      [TERAKAN_STATE_DRAW_INDEX_DB_RENDER_OVERRIDE] = terakan_state_draw_apply_db_render_override,
      [TERAKAN_STATE_DRAW_INDEX_DB_STENCILREFMASK] = terakan_state_draw_apply_db_stencilrefmask,
      [TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_CONTROL] = terakan_state_draw_apply_db_depth_control,
      [TERAKAN_STATE_DRAW_INDEX_COLOR_ATTACHMENT_USAGE] =
         terakan_state_draw_apply_color_attachment_usage,
      [TERAKAN_STATE_DRAW_INDEX_LOGIC_OP] = terakan_state_draw_apply_logic_op,
      [TERAKAN_STATE_DRAW_INDEX_CB_BLEND_CONTROL] = terakan_state_draw_apply_cb_blend_control,
      [TERAKAN_STATE_DRAW_INDEX_CB_COLOR_RTV] = terakan_state_draw_apply_cb_color_rtv,
      [TERAKAN_STATE_DRAW_INDEX_CB_TARGET_MASK] = terakan_state_draw_apply_cb_target_mask,
      [TERAKAN_STATE_DRAW_INDEX_CB_COLOR_CONTROL] = terakan_state_draw_apply_cb_color_control,
      [TERAKAN_STATE_DRAW_INDEX_DB_SHADER_CONTROL] = terakan_state_draw_apply_db_shader_control,
};

void
terakan_state_draw_apply_pending(struct terakan_gfx_command_writer * const command_writer)
{
   struct terakan_state_draw * const state = &command_writer->state_draw;
   /* Manual loop instead of BITSET_FOREACH_SET, reading the pending bits at each iteration so
    * application of state items with lower indices can make state items with higher indices pending
    * if the latter depend on the former.
    */
   unsigned next_state_index = 0;
   while (next_state_index < TERAKAN_STATE_DRAW_INDEX_COUNT) {
      unsigned const next_state_word_index = BITSET_BITWORD(next_state_index);
      BITSET_WORD const next_state_word_remaining =
         state->state_pending[next_state_word_index] & ~(BITSET_BIT(next_state_index) - 1);
      if (!next_state_word_remaining) {
         next_state_index = BITSET_WORDBITS * (next_state_word_index + 1);
         continue;
      }
      unsigned const state_index =
         BITSET_WORDBITS * next_state_word_index + (ffs(next_state_word_remaining) - 1);
      if (state_index >= TERAKAN_STATE_DRAW_INDEX_COUNT) {
         /* Ignore the bits beyond the end of the bitset. */
         break;
      }
      terakan_state_draw_apply_functions[state_index](command_writer);
      BITSET_CLEAR(state->state_pending, state_index);
      next_state_index = state_index + 1;
   }
}

void
terakan_state_draw_reset(struct terakan_state_draw * const state,
                         struct terakan_device const * device)
{
   /* Initialize the state to the default values, corresponding to one of the following that's
    * applicable:
    * - For optional features, the feature is not enabled.
    * - For state configured via a structure in a pNext chain, the structure is missing.
    * - The field in the original structure is zero (like if it was zeroed via memset or {}).
    *
    * While Vulkan drivers are not required to do this because complete state for all enabled
    * features must be configured before a draw (via a pipeline object or dynamic state), do this
    * for simplicity:
    * - Certain state items correspond to hardware registers containing multiple fields, and those
    *   fields are configured individually separately by state setters. In this case the rest of the
    *   fields (the ones not exposed to the application) in those registers must be set to their
    *   default values.
    * - Optional features not enabled on the device don't need to be handled explicitly, especially
    *   when the application uses only dynamic state and never calls the setters for certain state
    *   items.
    * - There may be conditional logic based on the latest values of the state items in some
    *   places, make sure it doesn't read uninitialized variables.
    */

   /* No VK_DYNAMIC_STATE_DEPTH_CLIP_ENABLE_EXT,
    * no VkPipelineRasterizationDepthClipStateCreateInfoEXT */
   state->cmd_set_depth_clamp_enable_sets_depth_clip_enable = true;

   /* indexType = VK_INDEX_TYPE_UINT16 */
#if UTIL_ARCH_BIG_ENDIAN
   state->vgt_index_type = VGT_INDEX_16 | VGT_DMA_SWAP_16_BIT;
#else
   state->vgt_index_type = VGT_INDEX_16;
#endif

   /* topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST */
   state->vgt_primitive_type = S_008958_PRIM_TYPE(V_008958_DI_PT_POINTLIST);

   /* firstVertex or vertexOffset = 0 */
   state->vgt_index_offset = 0;

   state->sq_pgm_ls_es_gs_vs.vs_as_vs = NULL;

   /* vertexBindingDescriptionCount = 0, vertexAttributeDescriptionCount = 0 */
   memset(&state->sq_pgm_fs, 0, sizeof(state->sq_pgm_fs));
   state->sq_pgm_fs.static_state = &device->empty_vertex_input;

   /* pBuffers[...] = VK_NULL_HANDLE
    * Also make sure all vertex buffer bindings are cleared regardless of what's bound in
    * hw_state_draw at the time of this call.
    */
   memset(state->sq_resources_fs, 0, sizeof(state->sq_resources_fs));
   state->sq_resources_fs_pending = BITFIELD_MASK(TERAKAN_RESOURCE_HW_COUNT_FETCH);

   state->sq_pgm_ps.fs = NULL;

   state->pa_cl_clip_cntl =
      /* negativeOneToOne = VK_FALSE */
      S_028810_DX_CLIP_SPACE_DEF(1) |
      /* rasterizerDiscardEnable = VK_FALSE */
      S_028810_DX_RASTERIZATION_KILL(0) |
      /* Always enabled. */
      S_028810_DX_LINEAR_ATTR_CLIP_ENA(1) |
      /* depthClampEnable = VK_FALSE, no VkPipelineRasterizationDepthClipStateCreateInfoEXT */
      S_028810_ZCLIP_NEAR_DISABLE(0) | S_028810_ZCLIP_FAR_DISABLE(0);

   /* State for depthClampEnable must be initialized accurately because depthClamp is an optional
    * feature, when the feature is disabled, depthClampEnable = VK_FALSE must be assumed if never
    * set by the application.
    */
   bool const depth_range_unrestricted = device->vk.enabled_extensions.EXT_depth_range_unrestricted;

   state->viewport.count = 0;
   uint16_t const all_viewports_pending = BITFIELD_MASK(ARRAY_SIZE(state->viewport.viewports));
   state->viewport.viewports_pending.pa_cl_vport_xy_scale_offset = all_viewports_pending;
   state->viewport.viewports_pending.pa_cl_vport_z_scale_offset = all_viewports_pending;
   state->viewport.viewports_pending.pa_sc_vport_scissor = all_viewports_pending;
   state->viewport.viewports_pending.pa_sc_vport_z_min_max = all_viewports_pending;
   state->viewport.pa_cl_gb_pending = true;
   /* depthClampEnable = VK_FALSE */
   state->viewport.pa_sc_vport_z_min_0_max_1 = !depth_range_unrestricted;
   /* negativeOneToOne = VK_FALSE */
   state->viewport.from_apply_pa_cl_clip_cntl.dx_clip_space_def = true;
   memset(state->viewport.viewports, 0, sizeof(state->viewport.viewports));
   memset(state->viewport.pa_sc_vport_generic_scissor_tl_br_xy, 0,
          sizeof(state->viewport.pa_sc_vport_generic_scissor_tl_br_xy));

   state->pa_su_sc_mode_cntl =
      /* cullMode = VK_CULL_MODE_NONE */
      S_028814_CULL_FRONT(0) | S_028814_CULL_BACK(0) |
      /* frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE */
      S_028814_FACE(0) |
      /* polygonMode = VK_POLYGON_MODE_FILL */
      S_028814_POLY_MODE(V_028814_X_DISABLE_POLY_MODE) |
      S_028814_POLYMODE_FRONT_PTYPE(V_028814_X_DRAW_TRIANGLES) |
      S_028814_POLYMODE_BACK_PTYPE(V_028814_X_DRAW_TRIANGLES) |
      /* depthBiasEnable = VK_FALSE */
      S_028814_POLY_OFFSET_FRONT_ENABLE(0) | S_028814_POLY_OFFSET_BACK_ENABLE(0) |
      S_028814_POLY_OFFSET_PARA_ENABLE(0) |
      /* provokingVertexMode = VK_PROVOKING_VERTEX_MODE_FIRST_VERTEX_EXT */
      S_028814_PROVOKING_VTX_LAST(0);

   /* pSampleMask = NULL */
   state->pa_sc_aa_mask = UINT16_MAX;

   /* pDepthAttachment = NULL
    * pStencilAttachment = NULL
    */
   state->db_depth_stencil_buffer.bo = NULL;
   state->pa_su_poly_offset_db_fmt_cntl.from_apply_db_depth_stencil_buffer.poly_offset_z_format =
      TERAKAN_STATE_DRAW_PA_SU_POLY_OFFSET_DB_FMT_CNTL_NO_ATTACHMENT_FORMAT;

   /* No VkDepthBiasRepresentationInfoEXT */
   state->pa_su_poly_offset_db_fmt_cntl.representation =
      VK_DEPTH_BIAS_REPRESENTATION_LEAST_REPRESENTABLE_VALUE_FORMAT_EXT;
   state->pa_su_poly_offset_db_fmt_cntl.representation_exact = false;

   /* depthClampEnable = VK_FALSE */
   state->db_render_override = S_02800C_DISABLE_VIEWPORT_CLAMP(depth_range_unrestricted);

   /* depthTestEnable = VK_FALSE
    * depthWriteEnable = VK_FALSE
    * depthCompareOp = VK_COMPARE_OP_NEVER
    * stencilTestEnable = VK_FALSE
    * front, back = {
    *    failOp, passOp, depthFailOp = VK_STENCIL_OP_KEEP
    *    compareOp = VK_COMPARE_OP_NEVER
    *    compareMask = 0b0
    *    writeMask = 0b0
    *    reference = 0b0
    * }
    */
   memset(state->db_stencilrefmask_front_back, 0, sizeof(state->db_stencilrefmask_front_back));
   state->db_depth_control = 0;

   state->color_attachment_usage.from_apply_sq_pgm_ps.written_by_shader = 0b0;

   /* logicOpEnable = VK_FALSE */
   state->logic_op.enable = false;
   /* logicOp = VK_LOGIC_OP_CLEAR */
   state->logic_op.rop3 = 0x00;

   /* blendEnable = VK_FALSE
    * src/dstColor/AlphaBlendFactor = VK_BLEND_FACTOR_ZERO
    * color/alphaBlendOp = VK_BLEND_OP_ADD
    */
   memset(state->cb_blend_control.attachments, 0, sizeof(state->cb_blend_control.attachments));
   state->cb_color_rtv.from_apply_cb_blend_control.dual_source_blend = false;

   /* pColorAttachments[...].imageView = VK_NULL_HANDLE */
   memset(state->cb_color_rtv.attachments, 0, sizeof(state->cb_color_rtv.attachments));
   state->cb_target_mask.from_apply_cb_color_rtv.attachment_format_masks = 0b0;
   state->db_shader_control.from_apply_cb_color_rtv.cb_dual_export_allowed = true;

   /* colorWriteEnable feature disabled */
   state->cb_target_mask.attachment_write_enable =
      (uint8_t)BITFIELD_MASK(TERAKAN_COLOR_HW_RTV_COUNT);
   /* colorWriteMask = 0 */
   memset(state->cb_target_mask.attachment_write_masks, 0,
          sizeof(state->cb_target_mask.attachment_write_masks));

   state->cb_color_control.from_apply_cb_target_mask.any_rtv_enabled = false;

   /* Make all state items pending so the defaults are applied before the first draw, even for
    * unsupported features.
    */
   BITSET_ONES(state->state_pending);
}
