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

#include "util/bitscan.h"
#include "util/macros.h"

#include <assert.h>
#include <stdint.h>

nir_def *
terakan_nir_load_raw_resource_buffer(nir_builder * const b, unsigned const num_components,
                                     unsigned const bit_size, enum gl_access_qualifier const access,
                                     unsigned const resource_index_base,
                                     nir_def * const resource_index,
                                     unsigned const byte_address_base, nir_def * const byte_address,
                                     unsigned byte_address_alignment)
{
   assert(bit_size == 8 || bit_size == 16 || bit_size == 32);
   unsigned const component_bytes = bit_size / 8;

   static enum pipe_format const load_formats[][4] = {
      {
         PIPE_FORMAT_R8_UINT,
         PIPE_FORMAT_R8G8_UINT,
         PIPE_FORMAT_R8G8B8_UINT,
         PIPE_FORMAT_R8G8B8A8_UINT,
      },
      {
         PIPE_FORMAT_R16_UINT,
         PIPE_FORMAT_R16G16_UINT,
         PIPE_FORMAT_R16G16B16_UINT,
         PIPE_FORMAT_R16G16B16A16_UINT,
      },
      {
         PIPE_FORMAT_R32_UINT,
         PIPE_FORMAT_R32G32_UINT,
         PIPE_FORMAT_R32G32B32_UINT,
         PIPE_FORMAT_R32G32B32A32_UINT,
      },
   };
   enum pipe_format const * const load_formats_for_bit_size =
      load_formats[ffs(component_bytes) - 1];

   /* As fetching is performed using the vertex fetch instruction, ensure the vertex buffer
    * alignment described in the "4.4.6 Element Alignment" section of the Direct3D 11.3 Functional
    * Specification - min(nearest power of 2 greater than or equal to the format width, 4).
    * This means that with 4-byte alignment, formats of any width can be used.
    */
   byte_address_alignment = byte_address_alignment != 0 ? byte_address_alignment : component_bytes;
   assert(byte_address_alignment == 1 || byte_address_alignment == 2 ||
          byte_address_alignment >= 4);
   assert(byte_address_alignment >= component_bytes);
   unsigned const load_max_num_components =
      byte_address_alignment >= 4 ? 4 : byte_address_alignment / component_bytes;

   assert(num_components <= NIR_MAX_VEC_COMPONENTS);
   nir_def * components[NIR_MAX_VEC_COMPONENTS];

   unsigned load_byte_offset = byte_address_base;
   nir_def * load_byte_index = byte_address;
   for (unsigned first_component = 0; first_component < num_components;
        first_component += load_max_num_components) {
      unsigned const load_num_components =
         MIN2(num_components - first_component, load_max_num_components);

      /* Limit the base to the maximum value fetch instructions can store. */
      if (load_byte_offset > UINT16_MAX) {
         /* Add the caller-provided base for better common subexpression elimination if the same
          * base is used in multiple cases of loading.
          */
         load_byte_index = nir_iadd(b, byte_address, nir_imm_int(b, (int)byte_address_base));
         load_byte_offset -= byte_address_base;
         assert(load_byte_offset <= UINT16_MAX);
      }

      unsigned const load_bytes = component_bytes * load_num_components;
      /* TODO(Triang3l): Merge as many components as possible into a mega-fetch, and use
       * mini-fetches.
       */
      nir_def * const loaded_components = nir_load_buffer_resource_r600(
         b, load_num_components, bit_size, resource_index, load_byte_index, .access = access,
         .id_base = resource_index_base, .base = load_byte_offset, .component = 0,
         .format = load_formats_for_bit_size[load_num_components - 1],
         .mega_fetch_count_r600 = load_bytes);

      for (unsigned loaded_component_index = 0; loaded_component_index < load_num_components;
           ++loaded_component_index) {
         components[first_component + loaded_component_index] =
            nir_channel(b, loaded_components, loaded_component_index);
      }

      load_byte_offset += load_bytes;
   }

   return nir_vec(b, components, num_components);
}
