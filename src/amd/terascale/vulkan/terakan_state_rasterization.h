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

#ifndef TERAKAN_STATE_RASTERIZATION_H
#define TERAKAN_STATE_RASTERIZATION_H

#include "gallium/drivers/r600/evergreend.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#define TERAKAN_STATE_DRAW_DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_PA_CL_CLIP_CNTL_CLEAR                    \
   ((uint32_t)C_028810_DX_CLIP_SPACE_DEF)

static inline uint32_t
terakan_state_draw_depth_clip_negative_one_to_one_pa_cl_clip_cntl(bool const negative_one_to_one)
{
   return S_028810_DX_CLIP_SPACE_DEF(!negative_one_to_one);
}

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

#endif /* TERAKAN_STATE_RASTERIZATION_H */
