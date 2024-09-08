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

#ifndef TERAKAN_STATE_RASTERIZATION_H
#define TERAKAN_STATE_RASTERIZATION_H

#include "gallium/drivers/r600/evergreend.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Viewport and rasterization state. */

static inline void
terakan_state_draw_finalize_scissor(uint16_t tl_br_xy[4])
{
   /* Hardware bug workarounds, see:
    * - evergreen_fix_scissor_coordinates in xf86-video-ati
    * - evergreen_apply_scissor_bug_workaround in the Gallium R600 driver
    *
    * On both R8xx and R9xx:
    * if (br.x == 0) {
    *    tl.x = 1;
    * }
    * if (br.y == 0) {
    *    tl.y = 1;
    * }
    *
    * This functionally changes nothing as it affects only empty rectangles.
    * For simplicity and to make the workaround even stronger, turn any empty rectangle into -1x-1
    * at (1, 1).
    *
    * The issue may be related to the GFX6 hardware bug when bottom or right of any scissor is <= 0
    * and PA_SU_HARDWARE_SCREEN_OFFSET is not 0, see si_emit_one_scissor in RadeonSI.
    *
    * On R9xx only:
    * if (br.x == 1 && br.y == 1) {
    *    br.x = 2;
    * }
    *
    * This is clearly non-conformant as it makes it impossible to specify a 1x1 scissor at (0, 0).
    * Not implementing it until a solid confirmation that it's necessary is provided.
    */
   /* TODO(Triang3l): Research the need for the 1x1 to 2x1 workaround on R9xx. */
   if (tl_br_xy[0] >= tl_br_xy[2] || tl_br_xy[1] >= tl_br_xy[3]) {
      tl_br_xy[0] = 1;
      tl_br_xy[1] = 1;
      tl_br_xy[2] = 0;
      tl_br_xy[3] = 0;
   }
}

struct terakan_state_draw_viewport {
   float pa_cl_vport_xy_scale_offset[2][2];
   float pa_cl_vport_z_gl_dx_scale_offset[2][2];
   float pa_cl_gb_vert_horz_clip_adj[2];
   uint16_t pa_sc_vport_scissor_tl_br_xy[2][2];
   float pa_sc_vport_z_min_max[2];
};

void terakan_state_draw_viewport_translate(VkViewport const * viewport,
                                           struct terakan_state_draw_viewport * hw_viewport_out);

#define TERAKAN_STATE_DRAW_RASTERIZER_DISCARD_ENABLE_PA_CL_CLIP_CNTL_CLEAR                         \
   ((uint32_t)C_028810_DX_RASTERIZATION_KILL)

static inline uint32_t
terakan_state_draw_rasterizer_discard_enable_pa_cl_clip_cntl(bool const rasterizer_discard_enable)
{
   return S_028810_DX_RASTERIZATION_KILL(rasterizer_discard_enable);
}

#define TERAKAN_STATE_DRAW_DEPTH_CLIP_ENABLE_PA_CL_CLIP_CNTL_CLEAR                                 \
   ((uint32_t)(C_028810_ZCLIP_NEAR_DISABLE & C_028810_ZCLIP_FAR_DISABLE))

static inline uint32_t
terakan_state_draw_depth_clip_enable_pa_cl_clip_cntl(bool const depth_clip_enable)
{
   return S_028810_ZCLIP_NEAR_DISABLE(!depth_clip_enable) |
          S_028810_ZCLIP_FAR_DISABLE(!depth_clip_enable);
}

#define TERAKAN_STATE_DRAW_POLYGON_MODE_PA_SU_SC_MODE_CNTL_CLEAR                                   \
   ((uint32_t)(C_028814_POLY_MODE & C_028814_POLYMODE_FRONT_PTYPE & C_028814_POLYMODE_BACK_PTYPE))

static inline uint32_t
terakan_state_draw_polygon_mode_pa_su_sc_mode_cntl(VkPolygonMode const polygon_mode)
{
   uint32_t hw_polygon_mode;
   switch (polygon_mode) {
   case VK_POLYGON_MODE_FILL:
      hw_polygon_mode = V_028814_X_DRAW_TRIANGLES;
      break;
   case VK_POLYGON_MODE_LINE:
      hw_polygon_mode = V_028814_X_DRAW_LINES;
      break;
   case VK_POLYGON_MODE_POINT:
      hw_polygon_mode = V_028814_X_DRAW_POINTS;
      break;
   default:
      assert(!"Unsupported polygon mode");
      hw_polygon_mode = V_028814_X_DRAW_TRIANGLES;
   }
   return S_028814_POLY_MODE(hw_polygon_mode != V_028814_X_DRAW_TRIANGLES
                                ? V_028814_X_DUAL_MODE
                                : V_028814_X_DISABLE_POLY_MODE) |
          S_028814_POLYMODE_FRONT_PTYPE(hw_polygon_mode) |
          S_028814_POLYMODE_BACK_PTYPE(hw_polygon_mode);
}

#define TERAKAN_STATE_DRAW_CULL_MODE_PA_SU_SC_MODE_CNTL_CLEAR                                      \
   ((uint32_t)(C_028814_CULL_FRONT & C_028814_CULL_BACK))

static inline uint32_t
terakan_state_draw_cull_mode_pa_su_sc_mode_cntl(VkCullModeFlags const cull_mode)
{
   return S_028814_CULL_FRONT((cull_mode & VK_CULL_MODE_FRONT_BIT) != 0) |
          S_028814_CULL_BACK((cull_mode & VK_CULL_MODE_BACK_BIT) != 0);
}

#define TERAKAN_STATE_DRAW_FRONT_FACE_PA_SU_SC_MODE_CNTL_CLEAR ((uint32_t)C_028814_FACE)

static inline uint32_t
terakan_state_draw_front_face_pa_su_sc_mode_cntl(VkFrontFace const front_face)
{
   return S_028814_FACE(front_face != VK_FRONT_FACE_COUNTER_CLOCKWISE);
}

#define TERAKAN_STATE_DRAW_PROVOKING_VERTEX_MODE_PA_SU_SC_MODE_CNTL_CLEAR                          \
   ((uint32_t)C_028814_PROVOKING_VTX_LAST)

static inline uint32_t
terakan_state_draw_provoking_vertex_mode_pa_su_sc_mode_cntl(
   VkProvokingVertexModeEXT const provoking_vertex_mode)
{
   return S_028814_PROVOKING_VTX_LAST(provoking_vertex_mode ==
                                      VK_PROVOKING_VERTEX_MODE_LAST_VERTEX_EXT);
}

#define TERAKAN_STATE_DRAW_DEPTH_BIAS_ENABLE_PA_SU_SC_MODE_CNTL_CLEAR                              \
   ((uint32_t)(C_028814_POLY_OFFSET_FRONT_ENABLE & C_028814_POLY_OFFSET_BACK_ENABLE &              \
               C_028814_POLY_OFFSET_PARA_ENABLE))

static inline uint32_t
terakan_state_draw_depth_bias_enable_pa_su_sc_mode_cntl(bool const depth_bias_enable)
{
   return S_028814_POLY_OFFSET_FRONT_ENABLE(depth_bias_enable) |
          S_028814_POLY_OFFSET_BACK_ENABLE(depth_bias_enable) |
          S_028814_POLY_OFFSET_PARA_ENABLE(depth_bias_enable);
}

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_STATE_RASTERIZATION_H */
