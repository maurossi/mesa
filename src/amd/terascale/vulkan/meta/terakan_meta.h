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

#ifndef TERAKAN_META_H
#define TERAKAN_META_H

#include "terakan_descriptor.h"
#include "terakan_format.h"
#include "terakan_hw_state.h"
#include "terakan_shader.h"
#include "terakan_state.h"

#include "gallium/drivers/r600/evergreend.h"
#include "util/bitset.h"
#include "util/format/u_formats.h"
#include "vk_format.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TERAKAN_META_PA_CL_VTE_CNTL_2D (S_028818_VTX_XY_FMT(1) | S_028818_VTX_Z_FMT(1))

#define TERAKAN_META_DB_RENDER_OVERRIDE_DEFAULT 0

#define TERAKAN_META_SQ_PGM_RESOURCES_COMMON S_028844_DX10_CLAMP(1)
#define TERAKAN_META_SQ_PGM_RESOURCES_2_COMMON                                                     \
   (S_028848_SINGLE_ROUND(V_SQ_ROUND_NEAREST_EVEN) | S_028848_DOUBLE_ROUND(V_SQ_ROUND_NEAREST_EVEN))

#define TERAKAN_META_DB_SHADER_CONTROL_DEFAULT                                                     \
   (S_02880C_Z_ORDER(V_02880C_EARLY_Z_THEN_LATE_Z) |                                               \
    S_02880C_DB_SOURCE_FORMAT(V_02880C_EXPORT_DB_TWO) | S_02880C_DUAL_EXPORT_ENABLE(1))
/* S_02880C_EXEC_ON_NOOP to allow not only UAVs, which are specified in CB mask registers, but also
 * other shader memory export paths.
 */
#define TERAKAN_META_DB_SHADER_CONTROL_PS_MEMORY_EXPORT                                            \
   (S_02880C_Z_ORDER(V_02880C_LATE_Z) | S_02880C_EXEC_ON_HIER_FAIL(1) | S_02880C_EXEC_ON_NOOP(1) | \
    S_02880C_DB_SOURCE_FORMAT(V_02880C_EXPORT_DB_TWO) | S_02880C_DUAL_EXPORT_ENABLE(1))

struct terakan_meta_shader_description {
   uint32_t const * program;
   size_t program_size_bytes;
   struct terakan_shader_static static_registers;
};

struct terakan_meta_shader {
   struct terakan_meta_shader_description r8xx;
   struct terakan_meta_shader_description r9xx;

   uint16_t kcache_needed;
   BITSET_DECLARE(resources_needed, TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE);
   uint32_t samplers_needed;

   union {
      struct {
         uint32_t db_shader_control;
      } ps;
   } stage;
};

enum terakan_meta_shader_index {
   /* Exports (0, 0, 0, 1) to MRT 0 (all pixel shaders must perform at least one export).
    * Can be used as a null shader compatible with alpha to coverage.
    */
   TERAKAN_META_SHADER_EMPTY_OPAQUE_PS,

   /* Vertex index unpacked as X16Y16 into the position, Z = 0, W = 1.
    * Exports the instance ID as an integer in all components of the first parameter.
    */
   TERAKAN_META_SHADER_POSITION_FROM_INDEX_VS,
   /* Vertex index unpacked as X16Y16 into the position, Z = 0, W = 1, instance ID into the array
    * layer.
    * Exports the instance ID as an integer in all components of the first parameter.
    */
   TERAKAN_META_SHADER_POSITION_AND_LAYER_FROM_INDEX_VS,

   TERAKAN_META_SHADER_CLEAR_COLOR_PS,

   TERAKAN_META_SHADER_COPY_BUFFER_TO_IMAGE_PS,
   TERAKAN_META_SHADER_COPY_IMAGE_TO_BUFFER_PS,
   TERAKAN_META_SHADER_COPY_IMAGE_PS,
   TERAKAN_META_SHADER_COPY_EXPAND_3X_PS,

   TERAKAN_META_SHADER_COUNT,
};

extern struct terakan_meta_shader const terakan_meta_position_from_index_vs;
extern struct terakan_meta_shader const terakan_meta_position_and_layer_from_index_vs;
extern struct terakan_meta_shader const terakan_meta_clear_color_ps;
extern struct terakan_meta_shader const terakan_meta_copy_buffer_to_image_ps;
extern struct terakan_meta_shader const terakan_meta_copy_image_to_buffer_ps;
extern struct terakan_meta_shader const terakan_meta_copy_image_ps;
extern struct terakan_meta_shader const terakan_meta_copy_expand_3x_ps;

extern struct terakan_meta_shader const * const terakan_meta_shaders[TERAKAN_META_SHADER_COUNT];

struct terakan_gfx_command_writer;

void terakan_meta_modify_state_draw_dword(struct terakan_gfx_command_writer * command_writer,
                                          enum terakan_state_draw_index invalidate_state_index,
                                          enum terakan_hw_state_draw_index hw_state_index,
                                          uint32_t * hw_state_item, uint32_t value);

void terakan_meta_set_pa_cl_vte_cntl(struct terakan_gfx_command_writer * command_writer,
                                     uint32_t pa_cl_vte_cntl);

void terakan_meta_set_db_render_override(struct terakan_gfx_command_writer * command_writer,
                                         uint32_t db_render_override);

void terakan_meta_set_db_depth_control(struct terakan_gfx_command_writer * command_writer,
                                       uint32_t db_depth_control);

void terakan_meta_set_db_shader_control(struct terakan_gfx_command_writer * command_writer,
                                        uint32_t db_shader_control);

static inline void
terakan_meta_set_db_shader_control_with_rtv(struct terakan_gfx_command_writer * const command_writer,
                                            uint32_t db_shader_control,
                                            uint32_t const cb_color_info)
{
   if (G_028C70_SOURCE_FORMAT(cb_color_info) == V_028C70_EXPORT_4C_32BPC) {
      db_shader_control &= C_02880C_DUAL_EXPORT_ENABLE;
   }
   terakan_meta_set_db_shader_control(command_writer, db_shader_control);
}

void terakan_meta_set_vs(struct terakan_gfx_command_writer * command_writer,
                         enum terakan_meta_shader_index shader_index);
/* set_db_shader_control may be set to false if it's, for instance, unknown whether the actual RTV
 * used will allow DUAL_EXPORT_ENABLE for its format, but in this case DB_SHADER_CONTROL must also
 * be set explicitly before drawing.
 */
void terakan_meta_set_ps(struct terakan_gfx_command_writer * command_writer,
                         enum terakan_meta_shader_index shader_index, bool set_db_shader_control);

void terakan_meta_begin_cb(struct terakan_gfx_command_writer * command_writer,
                           uint32_t cb_color_control_mode, uint32_t cb_target_mask,
                           uint8_t disable_blend_for_targets);

void terakan_meta_begin_2d(struct terakan_gfx_command_writer * command_writer,
                           uint32_t pa_cl_vte_cntl);

void terakan_meta_begin_rects(struct terakan_gfx_command_writer * command_writer);

void terakan_meta_begin_index_immediate_32(struct terakan_gfx_command_writer * command_writer);

static inline void
terakan_meta_begin_2d_immediate_rects(struct terakan_gfx_command_writer * const command_writer,
                                      uint32_t const pa_cl_vte_cntl,
                                      uint32_t const db_render_override,
                                      bool const clear_db_depth_control)
{
   terakan_meta_set_db_render_override(command_writer, db_render_override);
   terakan_meta_begin_2d(command_writer, pa_cl_vte_cntl);
   terakan_meta_begin_rects(command_writer);
   terakan_meta_begin_index_immediate_32(command_writer);
   if (clear_db_depth_control) {
      terakan_meta_set_db_depth_control(command_writer, 0);
   }
}

void terakan_meta_emit_rect_3_vertices_draw(struct terakan_gfx_command_writer * command_writer,
                                            VkRect2D const * rect, uint32_t instance_count);

static inline VkImageViewType
terakan_meta_transfer_image_view_type(VkImageType const image_type)
{
   return image_type == VK_IMAGE_TYPE_1D ? VK_IMAGE_VIEW_TYPE_1D_ARRAY
                                         : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
}

/* Callers are allowed, for simplicity, to make assumptions that the returned format is one of the
 * exact formats listed here, and hardcode certain descriptor fields based on that.
 */
static inline struct terascale_format_info
terakan_meta_transfer_image_block_format_info(unsigned const bytes_per_block)
{
   enum pipe_format format = PIPE_FORMAT_R8_UNORM;
   switch (bytes_per_block) {
   /* 16bpc color export. */
   case 1:
      format = PIPE_FORMAT_R8_UNORM;
      break;
   case 2:
      format = PIPE_FORMAT_R8G8_UNORM;
      break;
   case 4:
      format = PIPE_FORMAT_R8G8B8A8_UNORM;
      break;
   /* 32bpc color export. */
   case 8:
      format = PIPE_FORMAT_R32G32_UINT;
      break;
   case 16:
      format = PIPE_FORMAT_R32G32B32A32_UINT;
      break;
   default:
      assert(!"Non-power-of-two-byte image formats must use special logic for transfer operations");
   }
   return terascale_format_info_r8xx[format];
}

/* Creates a buffer resource descriptor (without the actual memory range) for reading the full texel
 * of a 3x-expanded image accepting the index in surfels.
 */
static inline void
terakan_meta_transfer_expand_3x_resource(unsigned const bytes_per_surfel, uint32_t resource_out[8])
{
   enum terascale_format_index data_format;
   switch (bytes_per_surfel) {
   case 1:
      data_format = TERASCALE_FORMAT_INDEX_8_8_8;
      break;
   case 2:
      data_format = TERASCALE_FORMAT_INDEX_16_16_16;
      break;
   case 4:
      data_format = TERASCALE_FORMAT_INDEX_32_32_32;
      break;
   default:
      assert(!"Unsupported 3x-expanded format surfel byte count");
      data_format = TERASCALE_FORMAT_INDEX_INVALID;
   }

   memset(resource_out, 0, sizeof(uint32_t) * 8);
   resource_out[2] = S_030008_STRIDE(bytes_per_surfel) | S_030008_DATA_FORMAT(data_format) |
                     S_030008_NUM_FORMAT_ALL(TERASCALE_FORMAT_SQ_NUM_FORMAT_INT);
   resource_out[3] =
      S_03000C_DST_SEL_X(TERASCALE_SWIZZLE_X) | S_03000C_DST_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_03000C_DST_SEL_Z(TERASCALE_SWIZZLE_Z) | S_03000C_DST_SEL_W(TERASCALE_SWIZZLE_1),
   resource_out[7] = S_03001C_TYPE(V_03001C_SQ_TEX_VTX_VALID_BUFFER);
   resource_out[TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD] = TERAKAN_BO_PRIORITY_SHADER_READ_BUFFER;
}

/* Creates a buffer resource descriptor (without the actual memory range) for accessing one surfel
 * of a 3x-expanded image.
 */
static inline struct terakan_color_descriptor
terakan_meta_transfer_expand_3x_uav(unsigned const bytes_per_surfel,
                                    unsigned const tile_pipe_interleave_bytes_log2)
{
   enum terascale_format_index format;
   switch (bytes_per_surfel) {
   case 1:
      format = TERASCALE_FORMAT_INDEX_8;
      break;
   case 2:
      format = TERASCALE_FORMAT_INDEX_16;
      break;
   case 4:
      format = TERASCALE_FORMAT_INDEX_32;
      break;
   default:
      assert(!"Unsupported 3x-expanded format surfel byte count");
      format = TERASCALE_FORMAT_INDEX_INVALID;
   }

   struct terakan_color_descriptor descriptor = {};
   /* The pitch field is ignored by the hardware for buffers and can't store large buffer sizes, but
    * DRM Radeon 2.50.0 validates the pitch alignment regardless of whether the color surface is a
    * buffer UAV. Provide the smallest valid pitch.
    */
   descriptor.pitch =
      S_028C64_PITCH_TILE_MAX(terakan_format_pitch_alignment_linear_surfels(
                                 bytes_per_surfel, tile_pipe_interleave_bytes_log2) /
                                 8 -
                              1);
   descriptor.info = S_028C70_FORMAT(format) | S_028C70_ARRAY_MODE(V_028C70_ARRAY_LINEAR_ALIGNED) |
                     S_028C70_NUMBER_TYPE(TERASCALE_FORMAT_NUMBER_TYPE_UINT) |
                     S_028C70_BLEND_BYPASS(1) | S_028C70_SOURCE_FORMAT(V_028C70_EXPORT_4C_32BPC) |
                     S_028C70_RAT(1) | S_028C70_RESOURCE_TYPE(V_028C70_BUFFER);
   descriptor.attrib = S_028C74_NON_DISP_TILING_ORDER(1);
   return descriptor;
}

void terakan_meta_copy_expand_3x_buffer_to_image(
   struct terakan_gfx_command_writer * command_writer,
   VkCopyBufferToImageInfo2 const * copy_buffer_to_image_info);
void terakan_meta_copy_expand_3x_image_to_buffer(
   struct terakan_gfx_command_writer * command_writer,
   VkCopyImageToBufferInfo2 const * copy_image_to_buffer_info);
void terakan_meta_copy_expand_3x_image(struct terakan_gfx_command_writer * command_writer,
                                       VkCopyImageInfo2 const * copy_image_info);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_META_H */
