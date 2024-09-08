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

#include "terakan_meta.h"

#include "terakan_command_buffer.h"
#include "terakan_descriptor.h"
#include "terakan_entrypoints.h"

#include "gallium/drivers/r600/eg_sq.h"
#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600_opcodes.h"
#include "util/macros.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum {
   TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE,
   TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_R = TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE,
   TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_G,
   TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_B,
   TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_A,

   TERAKAN_META_CLEAR_COLOR_CONSTS_COUNT,
};

static uint32_t const terakan_meta_clear_color_ps_r8xx[] = {
   /* Control flow. */

   /* 0: Clear value loading. */

   S_SQ_CF_WORD0_ADDR(2) | S_SQ_CF_ALU_WORD0_KCACHE_BANK0(TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS) |
      S_SQ_CF_ALU_WORD0_KCACHE_MODE0(V_SQ_CF_KCACHE_LOCK_1),
   S_SQ_CF_ALU_WORD1_KCACHE_ADDR0(
      TERAKAN_KCACHE_DWORD_LINE(TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE)) |
      S_SQ_CF_ALU_WORD1_COUNT(5 - 2) | EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 1: Export the color and end the program. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_X) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_Z) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) | S_SQ_CF_ALLOC_EXPORT_WORD1_END_OF_PROGRAM(1) |
      EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* ALU clause. */

   /* Move the clear value from the kcache to the GPRs.
    *
    * 2:     R0.X = MOV CB[push].clear_value.X, unused 0
    * 3:     R0.Y = MOV CB[push].clear_value.Y, unused 0
    * 4:     R0.Z = MOV CB[push].clear_value.Z, unused 0
    * 5: (v) R0.W = MOV CB[push].clear_value.W, unused 0
    */

   TERAKAN_KCACHE_DWORD_WORD0_SRC0(TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_R) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),

   TERAKAN_KCACHE_DWORD_WORD0_SRC0(TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_G) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(1) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),

   TERAKAN_KCACHE_DWORD_WORD0_SRC0(TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_B) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(2) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),

   S_SQ_ALU_WORD0_LAST(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC0(TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_A) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(3) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),
};

static uint32_t const terakan_meta_clear_color_ps_r9xx[] = {
   /* Control flow. */

   /* 0: Clear value loading. */

   S_SQ_CF_WORD0_ADDR(3) | S_SQ_CF_ALU_WORD0_KCACHE_BANK0(TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS) |
      S_SQ_CF_ALU_WORD0_KCACHE_MODE0(V_SQ_CF_KCACHE_LOCK_1),
   S_SQ_CF_ALU_WORD1_KCACHE_ADDR0(
      TERAKAN_KCACHE_DWORD_LINE(TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE)) |
      S_SQ_CF_ALU_WORD1_COUNT(6 - 3) | EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU,

   /* 1: Export the color. */

   S_SQ_CF_ALLOC_EXPORT_WORD0_TYPE(V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL) |
      S_SQ_CF_ALLOC_EXPORT_WORD0_ARRAY_BASE(0) | S_SQ_CF_ALLOC_EXPORT_WORD0_RW_GPR(0),
   S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_X(TERASCALE_SWIZZLE_X) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Y(TERASCALE_SWIZZLE_Y) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_Z(TERASCALE_SWIZZLE_Z) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_SWIZ_SEL_W(TERASCALE_SWIZZLE_W) |
      S_SQ_CF_ALLOC_EXPORT_WORD1_BARRIER(1) | EG_V_SQ_CF_ALLOC_EXPORT_WORD1_SQ_CF_INST_EXPORT_DONE,

   /* 2: End the program. */

   0,
   S_SQ_CF_WORD1_BARRIER(1) | CM_V_SQ_CF_WORD1_SQ_CF_INST_END,

   /* ALU clause. */

   /* Move the clear value from the kcache to the GPRs.
    *
    * 3: R0.X = MOV CB[push].clear_value.X, unused 0
    * 4: R0.Y = MOV CB[push].clear_value.Y, unused 0
    * 5: R0.Z = MOV CB[push].clear_value.Z, unused 0
    * 6: R0.W = MOV CB[push].clear_value.W, unused 0
    */

   TERAKAN_KCACHE_DWORD_WORD0_SRC0(TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_R) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(0) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),

   TERAKAN_KCACHE_DWORD_WORD0_SRC0(TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_G) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(1) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),

   TERAKAN_KCACHE_DWORD_WORD0_SRC0(TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_B) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(2) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),

   S_SQ_ALU_WORD0_LAST(1) |
      TERAKAN_KCACHE_DWORD_WORD0_SRC0(TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE_A) |
      S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0),
   S_SQ_ALU_WORD1_DST_GPR(0) | S_SQ_ALU_WORD1_DST_CHAN(3) | S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
      S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV),
};

struct terakan_meta_shader const terakan_meta_clear_color_ps = {
   .r8xx =
      {
         .program = terakan_meta_clear_color_ps_r8xx,
         .program_size_bytes = sizeof(terakan_meta_clear_color_ps_r8xx),
         .static_registers =
            {
               .sq_pgm_resources =
                  {
                     S_028844_NUM_GPRS(1) | TERAKAN_META_SQ_PGM_RESOURCES_COMMON,
                     TERAKAN_META_SQ_PGM_RESOURCES_2_COMMON,
                  },
               .stage =
                  {
                     .ps =
                        {
                           .sq_pgm_exports_ps = S_02884C_EXPORT_COLORS(1),
                           .spi_ps_in_control =
                              {
                                 S_0286CC_NUM_INTERP(1) | S_0286CC_LINEAR_GRADIENT_ENA(1),
                                 0,
                              },
                           .spi_baryc_cntl = S_0286E0_LINEAR_CENTER_ENA(1),
                           .cb_shader_mask = 0xF,
                        },
                  },
            },
      },
   .r9xx =
      {
         .program = terakan_meta_clear_color_ps_r9xx,
         .program_size_bytes = sizeof(terakan_meta_clear_color_ps_r9xx),
         .static_registers =
            {
               .sq_pgm_resources =
                  {
                     S_028844_NUM_GPRS(1) | TERAKAN_META_SQ_PGM_RESOURCES_COMMON,
                     TERAKAN_META_SQ_PGM_RESOURCES_2_COMMON,
                  },
               .stage =
                  {
                     .ps =
                        {
                           .sq_pgm_exports_ps = S_02884C_EXPORT_COLORS(1),
                           .spi_ps_in_control =
                              {
                                 S_0286CC_NUM_INTERP(1) | S_0286CC_LINEAR_GRADIENT_ENA(1),
                                 0,
                              },
                           .spi_baryc_cntl = S_0286E0_LINEAR_CENTER_ENA(1),
                           .cb_shader_mask = 0xF,
                        },
                  },
            },
      },
   .kcache_needed = (uint16_t)1 << TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS,
   .stage =
      {
         .ps =
            {
               .db_shader_control = TERAKAN_META_DB_SHADER_CONTROL_DEFAULT,
            },
      },
};

VKAPI_ATTR void VKAPI_CALL
terakan_CmdClearAttachments(VkCommandBuffer const commandBuffer, uint32_t const attachmentCount,
                            VkClearAttachment const * const pAttachments, uint32_t const rectCount,
                            VkClearRect const * const pRects)
{
   struct terakan_gfx_command_writer * const command_writer =
      terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx;

   /* Setup depth / stencil clearing.
    * If possible, it will be cleared alongside a color attachment.
    * For clearing depth, in addition to depth clamping (which is overall recommended for optimal
    * performance according to PAL), using vertex Z (applied via the viewport offset to zero
    * returned from the shader) matching the clamping bounds so the clear rectangle can be expressed
    * as a plane in HTile.
    */

   VkClearDepthStencilValue depth_stencil_clear_value = {};
   VkImageAspectFlags depth_stencil_clear_aspects = 0;
   for (uint32_t attachment_index = 0; attachment_index < attachmentCount; ++attachment_index) {
      VkClearAttachment const * const attachment = &pAttachments[attachment_index];
      if (attachment->aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT) {
         depth_stencil_clear_value.depth = attachment->clearValue.depthStencil.depth;
         depth_stencil_clear_aspects |= VK_IMAGE_ASPECT_DEPTH_BIT;
      }
      if (attachment->aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT) {
         depth_stencil_clear_value.stencil = attachment->clearValue.depthStencil.stencil;
         depth_stencil_clear_aspects |= VK_IMAGE_ASPECT_STENCIL_BIT;
      }
   }
   if (command_writer->state_draw.db_depth_stencil_buffer.bo != NULL) {
      if (G_028040_FORMAT(command_writer->state_draw.db_depth_stencil_buffer.descriptor.z_info) ==
          V_028040_Z_INVALID) {
         depth_stencil_clear_aspects &= ~VK_IMAGE_ASPECT_DEPTH_BIT;
      }
      if (G_028044_FORMAT(
             command_writer->state_draw.db_depth_stencil_buffer.descriptor.stencil_info) ==
          V_028044_STENCIL_INVALID) {
         depth_stencil_clear_aspects &= ~VK_IMAGE_ASPECT_STENCIL_BIT;
      }
   } else {
      depth_stencil_clear_aspects = 0;
   }

   /* depth_stencil_base_layer is undefined if not clearing depth / stencil. */
   uint32_t const depth_stencil_base_layer =
      G_028008_SLICE_START(command_writer->state_draw.db_depth_stencil_buffer.descriptor.view);

   if (depth_stencil_clear_aspects) {
      assert(command_writer->state_draw.db_depth_stencil_buffer.bo != NULL);
      struct terakan_depth_stencil_descriptor depth_stencil_descriptor =
         command_writer->state_draw.db_depth_stencil_buffer.descriptor;
      depth_stencil_descriptor.view =
         (depth_stencil_descriptor.view & C_028008_SLICE_START) |
         S_028008_SLICE_START(depth_stencil_base_layer + pRects[0].baseArrayLayer);
      bool const depth_stencil_binding_modified =
         command_writer->hw_state_draw.db_depth_stencil_buffer.bo !=
            command_writer->state_draw.db_depth_stencil_buffer.bo ||
         memcmp(&command_writer->hw_state_draw.db_depth_stencil_buffer.descriptor,
                &depth_stencil_descriptor, sizeof(struct terakan_depth_stencil_descriptor)) != 0;
      /* Make the depth / stencil buffer binding state pending regardless of whether the binding was
       * modified because rectangles may have different base array layers.
       */
      terakan_state_draw_set_pending(&command_writer->state_draw,
                                     TERAKAN_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER);
      if (depth_stencil_binding_modified) {
         command_writer->hw_state_draw.db_depth_stencil_buffer.bo =
            command_writer->state_draw.db_depth_stencil_buffer.bo;
         command_writer->hw_state_draw.db_depth_stencil_buffer.descriptor =
            depth_stencil_descriptor;
      }
      terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                    TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER,
                                    depth_stencil_binding_modified);

      uint32_t db_depth_control = 0;

      if (depth_stencil_clear_aspects & VK_IMAGE_ASPECT_DEPTH_BIT) {
         /* Make sure the clear isn't discarded (which may also result in the color clear skipped
          * too) due to a NaN vertex position, and flush denormals so the value is surely the same
          * in every part of the pipeline it passes.
          */
         if (unlikely(isnan(depth_stencil_clear_value.depth) ||
                      fabsf(depth_stencil_clear_value.depth) < 0x1.0p-126)) {
            depth_stencil_clear_value.depth = 0.0f;
         }

         terakan_state_draw_set_pending(&command_writer->state_draw,
                                        TERAKAN_STATE_DRAW_INDEX_VIEWPORT);
         struct terakan_hw_state_draw_viewport * const viewport =
            &command_writer->hw_state_draw.viewports[0];
         bool viewport_z_modified = false;
         if (memcmp(&viewport->pa_cl_vport_xyz_scale_offset[2][1], &depth_stencil_clear_value.depth,
                    sizeof(float)) != 0) {
            command_writer->state_draw.viewport.viewports_pending.pa_cl_vport_z_scale_offset |=
               BITFIELD_BIT(0);
            viewport_z_modified = true;
            viewport->pa_cl_vport_xyz_scale_offset[2][1] = depth_stencil_clear_value.depth;
         }
         if (memcmp(&viewport->pa_sc_vport_z_min_max[0], &depth_stencil_clear_value.depth,
                    sizeof(float)) != 0 ||
             memcmp(&viewport->pa_sc_vport_z_min_max[1], &depth_stencil_clear_value.depth,
                    sizeof(float)) != 0) {
            command_writer->state_draw.viewport.viewports_pending.pa_sc_vport_z_min_max |=
               BITFIELD_BIT(0);
            viewport_z_modified = true;
            viewport->pa_sc_vport_z_min_max[0] = depth_stencil_clear_value.depth;
            viewport->pa_sc_vport_z_min_max[1] = depth_stencil_clear_value.depth;
         }
         terakan_hw_state_draw_update_viewports(
            &command_writer->hw_state_draw, 1,
            viewport_z_modified ? 0 : ARRAY_SIZE(command_writer->hw_state_draw.viewports),
            ARRAY_SIZE(command_writer->hw_state_draw.viewports));

         db_depth_control |= S_028800_Z_ENABLE(1) | S_028800_Z_WRITE_ENABLE(1) |
                             S_028800_ZFUNC((uint32_t)VK_COMPARE_OP_ALWAYS);
      }

      if (depth_stencil_clear_aspects & VK_IMAGE_ASPECT_STENCIL_BIT) {
         uint32_t const db_stencilrefmask = S_028430_STENCILREF(depth_stencil_clear_value.stencil) |
                                            S_028430_STENCILWRITEMASK(UINT8_MAX);
         /* Not using the values for back faces, but the hw_state_draw item must be initialized
          * fully.
          */
         bool const db_stencilrefmask_modified =
            command_writer->hw_state_draw.db_stencilrefmask_front_back[0] != db_stencilrefmask ||
            command_writer->hw_state_draw.db_stencilrefmask_front_back[1] != db_stencilrefmask;
         if (db_stencilrefmask_modified) {
            terakan_state_draw_set_pending(&command_writer->state_draw,
                                           TERAKAN_STATE_DRAW_INDEX_DB_STENCILREFMASK);
            command_writer->hw_state_draw.db_stencilrefmask_front_back[0] = db_stencilrefmask;
            command_writer->hw_state_draw.db_stencilrefmask_front_back[1] = db_stencilrefmask;
         }
         terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                       TERAKAN_HW_STATE_DRAW_INDEX_DB_STENCILREFMASK,
                                       db_stencilrefmask_modified);

         db_depth_control |= S_028800_STENCIL_ENABLE(1) |
                             S_028800_STENCILFUNC(V_028800_STENCILFUNC_ALWAYS) |
                             S_028800_STENCILFAIL(V_028800_STENCIL_REPLACE) |
                             S_028800_STENCILZPASS(V_028800_STENCIL_REPLACE) |
                             S_028800_STENCILZFAIL(V_028800_STENCIL_REPLACE);
      }

      terakan_meta_set_db_depth_control(command_writer, db_depth_control);
   }

   terakan_meta_begin_2d_immediate_rects(
      command_writer,
      TERAKAN_META_PA_CL_VTE_CNTL_2D |
         (depth_stencil_clear_aspects & VK_IMAGE_ASPECT_DEPTH_BIT ? S_028818_VPORT_Z_OFFSET_ENA(1)
                                                                  : 0),
      TERAKAN_META_DB_RENDER_OVERRIDE_DEFAULT, false);

   terakan_meta_set_vs(command_writer, TERAKAN_META_SHADER_POSITION_AND_LAYER_FROM_INDEX_VS);

   /* TODO(Triang3l): Clear CMask and update the stored fast clear color for COPY_DW into the
    * registers if clearing the entire image.
    */

   command_writer->push_constants_state.up_to_date_push_constants_bound_to_stages &=
      ~VK_SHADER_STAGE_FRAGMENT_BIT;

   uint32_t constants[TERAKAN_META_CLEAR_COLOR_CONSTS_COUNT] = {};
   struct terakan_bo const * constants_bo = NULL;
   uint32_t constants_va_lines;

   for (uint32_t attachment_index = 0; attachment_index < attachmentCount; ++attachment_index) {
      VkClearAttachment const * const attachment = &pAttachments[attachment_index];
      if (!(attachment->aspectMask & VK_IMAGE_ASPECT_COLOR_BIT)) {
         continue;
      }

      if (memcmp(&constants[TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE],
                 attachment->clearValue.color.uint32, sizeof(uint32_t) * 4) != 0) {
         constants_bo = NULL;
         memcpy(&constants[TERAKAN_META_CLEAR_COLOR_CONST_CLEAR_VALUE],
                attachment->clearValue.color.uint32, sizeof(uint32_t) * 4);
      }

      if (constants_bo == NULL) {
         void * const constants_mapping = terakan_push_buffer_allocate_kcache(
            command_writer->base.command_buffer, sizeof(constants), &constants_bo,
            &constants_va_lines);
         if (unlikely(constants_mapping == NULL)) {
            return;
         }
         memcpy(constants_mapping, constants, sizeof(constants));
         terakan_hw_state_draw_set_sq_kcache_fs(
            &command_writer->hw_state_draw, TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS,
            DIV_ROUND_UP(sizeof(constants), TERAKAN_KCACHE_HW_LINE_BYTES), constants_bo,
            constants_va_lines);
      }

      terakan_meta_set_ps(command_writer, TERAKAN_META_SHADER_CLEAR_COLOR_PS, false);

      terakan_meta_begin_cb(command_writer, V_028808_CB_NORMAL, 0xF, 0b1);

      struct terakan_state_draw_cb_color const * const attachment_cb_color =
         &command_writer->state_draw.cb_color_rtv.attachments[attachment->colorAttachment];
      struct terakan_color_descriptor cb_color = attachment_cb_color->color;
      uint32_t const attachment_base_layer = G_028C6C_SLICE_START(cb_color.view);
      cb_color.view = (cb_color.view & C_028C6C_SLICE_START) |
                      S_028C6C_SLICE_START(attachment_base_layer + pRects[0].baseArrayLayer);
      terakan_hw_state_draw_set_cb_color(&command_writer->hw_state_draw, 0, attachment_cb_color->bo,
                                         &cb_color, &attachment_cb_color->meta, false);
      terakan_meta_set_db_shader_control_with_rtv(
         command_writer, terakan_meta_clear_color_ps.stage.ps.db_shader_control, cb_color.info);

      if (!depth_stencil_clear_aspects) {
         /* No depth or stencil to clear, or previously cleared alongside color.
          * Disable depth / stencil clearing, including disabling the viewport Z offset.
          * After this, the draws for this attachment must not be aborted for reasons other than
          * those causing the command buffer to enter an error state, or the depth / stencil clear
          * won't happen at all as it's already disabled in the hardware state.
          */
         terakan_meta_set_pa_cl_vte_cntl(command_writer, TERAKAN_META_PA_CL_VTE_CNTL_2D);
         terakan_meta_set_db_depth_control(command_writer, 0);
      }

      for (uint32_t rect_index = 0; rect_index < rectCount; ++rect_index) {
         VkClearRect const * const rect = &pRects[rect_index];
         if (rect_index != 0 && rect->baseArrayLayer != pRects[rect_index - 1].baseArrayLayer) {
            if (depth_stencil_clear_aspects) {
               command_writer->hw_state_draw.db_depth_stencil_buffer.descriptor.view =
                  (command_writer->hw_state_draw.db_depth_stencil_buffer.descriptor.view &
                   C_028008_SLICE_START) |
                  S_028008_SLICE_START(depth_stencil_base_layer + rect->baseArrayLayer);
               terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                             TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER,
                                             true);
            }
            cb_color.view = (cb_color.view & C_028C6C_SLICE_START) |
                            S_028C6C_SLICE_START(attachment_base_layer + rect->baseArrayLayer);
            terakan_hw_state_draw_set_cb_color(&command_writer->hw_state_draw, 0,
                                               attachment_cb_color->bo, &cb_color,
                                               &attachment_cb_color->meta, false);
         }
         terakan_meta_emit_rect_3_vertices_draw(command_writer, &rect->rect, rect->layerCount);
      }

      /* If there was depth or stencil to clear, it's cleared now alongside the color attachment. */
      depth_stencil_clear_aspects = 0;
   }

   if (depth_stencil_clear_aspects) {
      /* Depth/stencil was not cleared alongside color. Clear it now. */
      terakan_meta_set_ps(command_writer, TERAKAN_META_SHADER_EMPTY_OPAQUE_PS, true);
      terakan_meta_begin_cb(command_writer, V_028808_CB_DISABLE, 0x0, 0b0);
      for (uint32_t rect_index = 0; rect_index < rectCount; ++rect_index) {
         VkClearRect const * const rect = &pRects[rect_index];
         if (rect_index != 0 && rect->baseArrayLayer != pRects[rect_index - 1].baseArrayLayer) {
            command_writer->hw_state_draw.db_depth_stencil_buffer.descriptor.view =
               (command_writer->hw_state_draw.db_depth_stencil_buffer.descriptor.view &
                C_028008_SLICE_START) |
               S_028008_SLICE_START(depth_stencil_base_layer + rect->baseArrayLayer);
            terakan_hw_state_draw_written(&command_writer->hw_state_draw,
                                          TERAKAN_HW_STATE_DRAW_INDEX_DB_DEPTH_STENCIL_BUFFER,
                                          true);
         }
         terakan_meta_emit_rect_3_vertices_draw(command_writer, &rect->rect, rect->layerCount);
      }
   }
}
