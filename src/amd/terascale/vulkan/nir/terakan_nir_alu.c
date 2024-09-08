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
#include "nir_builder.h"

static bool
terakan_nir_lower_sin_cos_filter(nir_instr const * const instr, UNUSED void const * const cb_data)
{
   if (instr->type != nir_instr_type_alu) {
      return false;
   }
   nir_op const op = nir_instr_as_alu(instr)->op;
   return op == nir_op_fsin || op == nir_op_fcos;
}

static nir_def *
terakan_nir_lower_sin_cos_impl(nir_builder * const b, nir_instr * const instr,
                               UNUSED void * const cb_data)
{
   nir_alu_instr * const sin_cos = nir_instr_as_alu(instr);
   /* Multiply by 1/(2*PI) (the exact value from the GFX8+ inline constant) and make sure it's in
    * the accepted [-256, 256] range.
    */
   nir_def * const src =
      nir_ffract(b, nir_fmul(b, nir_ssa_for_alu_src(b, sin_cos, 0), nir_imm_int(b, 0x3E22F983)));
   return sin_cos->op == nir_op_fsin ? nir_fsin_amd(b, src) : nir_fcos_amd(b, src);
}

bool
terakan_nir_lower_sin_cos(nir_shader * const shader)
{
   return nir_shader_lower_instructions(shader, terakan_nir_lower_sin_cos_filter,
                                        terakan_nir_lower_sin_cos_impl, NULL);
}
