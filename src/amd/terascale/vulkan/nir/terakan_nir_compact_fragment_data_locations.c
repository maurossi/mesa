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

#include "terakan_nir.h"

#include "util/macros.h"

#include <stdbool.h>
#include <stdint.h>

bool
terakan_nir_compact_fragment_data_locations(nir_shader * const shader,
                                            uint8_t * const uncompacted_locations_out)
{
   if (shader->info.stage != MESA_SHADER_FRAGMENT) {
      *uncompacted_locations_out = 0b0;
      nir_shader_preserve_all_metadata(shader);
      return false;
   }

   NIR_PASS_V(shader, nir_lower_io_arrays_to_elements_no_indirects, true);

   uint8_t uncompacted_locations = 0b0;
   nir_foreach_shader_out_variable (var, shader) {
      gl_frag_result const location = (gl_frag_result)var->data.location;
      if (location >= FRAG_RESULT_DATA0 && location <= FRAG_RESULT_DATA7) {
         /* Dual-source blending factor is always exported at index 1 by SFN.
          * Enable `location = 0, index = 0` too regardless of whether the shader actually writes to
          * it so the bit count until the dual-source blending factor in the uncompacted location
          * mask is always 1 to unconditionally match SFN's export index assignment and CB register
          * setup.
          */
         if (var->data.index != 0) {
            uncompacted_locations |= 0b11;
         } else {
            uncompacted_locations |= (uint8_t)BITFIELD_BIT((int)location - (int)FRAG_RESULT_DATA0);
         }
      }
   }
   *uncompacted_locations_out = uncompacted_locations;
   if (!uncompacted_locations) {
      nir_shader_preserve_all_metadata(shader);
      return false;
   }

   bool any_location_compacted = false;
   nir_foreach_shader_out_variable (var, shader) {
      if (var->data.index != 0) {
         /* Dual-source blending factor is always exported at index 1 by SFN. */
         continue;
      }
      gl_frag_result const uncompacted_location = (gl_frag_result)var->data.location;
      if (uncompacted_location >= FRAG_RESULT_DATA0 && uncompacted_location <= FRAG_RESULT_DATA7) {
         var->data.location =
            (int)FRAG_RESULT_DATA0 +
            util_bitcount(uncompacted_locations &
                          BITFIELD_MASK((int)uncompacted_location - (int)FRAG_RESULT_DATA0));
         any_location_compacted |= var->data.location != uncompacted_location;
      }
   }

   nir_shader_preserve_all_metadata(shader);
   return any_location_compacted;
}
