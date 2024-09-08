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

#ifndef TERAKAN_STATE_INPUT_ASSEMBLY_H
#define TERAKAN_STATE_INPUT_ASSEMBLY_H

#include "gallium/drivers/r600/evergreend.h"

#include <assert.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t
terakan_state_draw_primitive_topology_vgt_primitive_type(
   VkPrimitiveTopology const primitive_topology)
{
   uint32_t hw_primitive_type;
   switch (primitive_topology) {
   case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
      hw_primitive_type = V_008958_DI_PT_POINTLIST;
      break;
   case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
      hw_primitive_type = V_008958_DI_PT_LINELIST;
      break;
   case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
      hw_primitive_type = V_008958_DI_PT_LINESTRIP;
      break;
   case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
      hw_primitive_type = V_008958_DI_PT_TRILIST;
      break;
   case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
      hw_primitive_type = V_008958_DI_PT_TRISTRIP;
      break;
   case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
      hw_primitive_type = V_008958_DI_PT_TRIFAN;
      break;
   case VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY:
      hw_primitive_type = V_008958_DI_PT_LINELIST_ADJ;
      break;
   case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY:
      hw_primitive_type = V_008958_DI_PT_LINESTRIP_ADJ;
      break;
   case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY:
      hw_primitive_type = V_008958_DI_PT_TRILIST_ADJ;
      break;
   case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY:
      hw_primitive_type = V_008958_DI_PT_TRISTRIP_ADJ;
      break;
   case VK_PRIMITIVE_TOPOLOGY_PATCH_LIST:
      hw_primitive_type = V_008958_DI_PT_PATCH;
      break;
   default:
      assert(!"Unsupported primitive topology");
      hw_primitive_type = V_008958_DI_PT_NONE;
   }
   return S_008958_PRIM_TYPE(hw_primitive_type);
}

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_STATE_INPUT_ASSEMBLY_H */
