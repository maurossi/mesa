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

#ifndef TERAKAN_VERTEX_INPUT_H
#define TERAKAN_VERTEX_INPUT_H

#include "terakan_bo.h"
#include "terakan_descriptor.h"

#include "util/bitset.h"
#include "util/macros.h"
#include "util/u_math.h"
#include "vk_limits.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

static_assert(
   MESA_VK_MAX_VERTEX_BINDINGS >= TERAKAN_RESOURCE_HW_COUNT_FETCH,
   "Expecting that the Mesa Vulkan runtime can expose all hardware vertex buffer bindings.");

/* Arbitrary limit, but roughly half the number of GPR vectors available in shaders, because GPRs
 * are used as the destinations in fetch shaders.
 */
#define TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES MIN2(MESA_VK_MAX_VERTEX_ATTRIBUTES, 64)

/* Compact representation of a vertex attribute. */
struct terakan_vertex_input_attribute {
   /* DST_GPR, DST_SEL_X/Y/Z/W, DATA_FORMAT, NUM_FORMAT_ALL, FORMAT_COMP_ALL.
    * DATA_FORMAT may be INVALID if the format is unsupported, in which case the binding at
    * buffer_id can be skipped where that may be relevant unless other attributes with a valid
    * format need it.
    */
   uint32_t word1_dst_gpr_and_format;
   uint16_t offset;
   /* Needs 5 bits. */
   uint8_t buffer_id;
};

bool
terakan_vertex_input_attribute_translate(uint32_t location, uint32_t binding, VkFormat format,
                                         uint32_t offset,
                                         struct terakan_vertex_input_attribute * attribute_out);

/* Possible code sizes per binding, assuming the worst case with all bindings requiring different
 * indices:
 * - R8xx:
 *   - Vertex-indexed:
 *     - 0 qw - sub-2048 stride.
 *     - 1 qw - 2048 stride (LSHL_INT by inline 1 to multiply the vertex index by 2).
 *   - Instance-indexed:
 *     - Sub-2048 stride:
 *       - 1 qw - divisor is 0 (MOV of 0).
 *       - 0 qw - divisor is 1.
 *       - Up to 9 qw - divisor is 2+. Worst case:
 *         + 2 qw - LSHR_INT (pre-shift) by a literal.
 *         + 2 qw - MIN_UINT 0xFFFFFFFE of the saturated increment.
 *         + 1 qw - ADD_INT 1 of the saturated increment.
 *         + 2 qw - MULHI_UINT by a literal.
 *         + 2 qw - LSHR_INT (post-shift) by a literal.
 *     - 2048 stride:
 *       - 1 qw - divisor is 0 (LSHL_INT by inline 1 to multiply the base index by 2).
 *       - 2 qw - divisor is 1 (LSHL_INT by inline 1 to multiply by 2, ADD_INT of the base index).
 *       - Up to 11 qw - divisor is 2+. Worst case:
 *         + All the same as for sub-2048 stride.
 *         + 1 qw - LSHL_INT by inline 1 to multiply by 2.
 *         + 1 qw - ADD_INT of the base index.
 * - R9xx:
 *   - 0 qw - vertex-indexed.
 *   - Instance-indexed:
 *     - 1 qw - divisor is 0 (MOV of 0).
 *     - 0 qw - divisor is 1.
 *       - Up to 12 qw - divisor is 2+. Worst case:
 *         + 2 qw - LSHR_INT (pre-shift) by a literal.
 *         + 2 qw - MIN_UINT 0xFFFFFFFE of the saturated increment.
 *         + 1 qw - ADD_INT 1 of the saturated increment.
 *         + 5 qw - MULHI_UINT (4-slot) by a literal.
 *         + 2 qw - LSHR_INT (post-shift) by a literal.
 */
#define TERAKAN_VERTEX_INPUT_FS_MAX_ALU_QWORDS (12 * TERAKAN_RESOURCE_HW_COUNT_FETCH)

/* An ALU clause can contain up to 0x80 qwords, but the last instruction group may overflow to the
 * next clause if there's less space available than needed for it (up to 7 qwords).
 */
#define TERAKAN_VERTEX_INPUT_FS_MAX_ALU_CLAUSES                                                    \
   DIV_ROUND_UP(TERAKAN_VERTEX_INPUT_FS_MAX_ALU_QWORDS, 0x80 - (7 - 1))

#define TERAKAN_VERTEX_INPUT_FS_MAX_FETCH_CLAUSES                                                  \
   DIV_ROUND_UP(TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES, 0x40)

#define TERAKAN_VERTEX_INPUT_FS_MAX_CF_QWORDS                                                      \
   (TERAKAN_VERTEX_INPUT_FS_MAX_ALU_CLAUSES + TERAKAN_VERTEX_INPUT_FS_MAX_FETCH_CLAUSES + 1)

#define TERAKAN_VERTEX_INPUT_FS_MAX_QWORDS                                                         \
   (ALIGN_POT(TERAKAN_VERTEX_INPUT_FS_MAX_CF_QWORDS + TERAKAN_VERTEX_INPUT_FS_MAX_ALU_QWORDS, 2) + \
    2 * TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES)

/* alu_out must have at least TERAKAN_VERTEX_INPUT_FS_MAX_ALU_QWORDS qwords.
 * alu_clause_qwords_out must have at least TERAKAN_VERTEX_INPUT_FS_MAX_ALU_CLAUSES elements.
 * fetch_out must have at least as many pairs of qwords as there are used attributes.
 * alu_out and fetch_out shouldn't point to uncached memory, as reads may be done internally.
 *
 * It is important that attributes_needed_by_vs must not contain any attribute locations beyond the
 * available number of GPRs, as index calculations may be deduplicated by the function.
 *
 * For bindings in `bindings_with_2048_stride_workaround`, different index calculation will be
 * generated so those bindings can be fetched from with a stride of 1024 instead, as it's not
 * possible to specify a stride of 2048 in R8xx fetch constants (although R9xx expands the stride
 * field to 12 bits), but 2048 is the minimum requirement on Vulkan. The vertex shader must load the
 * first instance to R0.Z before calling the fetch shader if any per-instance input binding with
 * 2048 stride is used.
 * `bindings_with_2048_stride_workaround` can have bits undefined for unused bindings.
 */
void terakan_vertex_input_create_fs_alu_and_fetches(
   bool is_r9xx, BITSET_WORD const * attributes_needed_and_valid,
   struct terakan_vertex_input_attribute const * attributes, uint32_t instance_bindings,
   uint32_t const * instance_binding_divisors, uint32_t bindings_with_2048_stride_workaround,
   uint32_t * alu_qword_count_out, uint32_t * alu_out, uint32_t * alu_clause_count_out,
   uint8_t * alu_clause_qwords_out, uint32_t * fetch_count_out, uint32_t * fetch_out);

static inline uint32_t
terakan_vertex_input_fs_byte_count(uint32_t const alu_qword_count, uint32_t const alu_clause_count,
                                   uint32_t const fetch_count)
{
   return sizeof(uint32_t) * 2 *
          (ALIGN_POT(alu_clause_count + DIV_ROUND_UP(fetch_count, 0x40) + 1 + alu_qword_count, 2) +
           2 * fetch_count);
}

/* Combines the result of terakan_vertex_input_create_fs_alu_and_fetches into a fetch shader
 * program.
 * The size of the program can be computed using terakan_vertex_input_fs_byte_count.
 * program_out can point to write-combined memory.
 */
void terakan_vertex_input_create_fs_program(bool is_r9xx, uint32_t alu_qword_count,
                                            void const * alu, uint32_t alu_clause_count,
                                            uint8_t const * alu_clause_qwords, uint32_t fetch_count,
                                            void const * fetch, void * program_out);

struct terakan_vertex_input_static_state {
   struct terakan_bo * program_bo;
   uint32_t program_va_shr8;

   BITSET_DECLARE(attributes_needed_and_provided, TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES);
   /* The values are undefined for attributes not in attributes_needed_and_provided. */
   struct terakan_vertex_input_attribute attributes[TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES];

   uint32_t bindings_needed_by_attributes_and_provided;
   /* The bits are 0 for bindings not in bindings_needed_by_attributes_and_provided. */
   uint32_t instance_bindings;
   /* The values are undefined for bindings not in instance_bindings. */
   uint32_t instance_binding_divisors[TERAKAN_RESOURCE_HW_COUNT_FETCH];

   /* If the stride is dynamic, in the static vertex input fetch shader, all bindings are assumed
    * not to need the 2048 stride workaround (this is 0).
    * The bits are 0 for bindings not in bindings_needed_by_attributes_and_provided.
    */
   uint32_t bindings_with_2048_stride_workaround;
};

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_VERTEX_INPUT_H */
