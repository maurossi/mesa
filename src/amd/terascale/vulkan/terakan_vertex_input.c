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

#include "terakan_vertex_input.h"

#include "terakan_buffer.h"
#include "terakan_command_buffer.h"
#include "terakan_descriptor.h"
#include "terakan_device.h"
#include "terakan_entrypoints.h"
#include "terakan_format.h"
#include "terakan_physical_device.h"

#include "gallium/drivers/r600/eg_sq.h"
#include "gallium/drivers/r600/evergreend.h"
#include "gallium/drivers/r600/r600_asm.h"
#include "gallium/drivers/r600/r600_opcodes.h"
#include "util/bitscan.h"
#include "util/bitset.h"
#include "util/fast_idiv_by_const.h"
#include "util/format/u_format.h"
#include "util/macros.h"
#include "util/u_endian.h"
#include "util/u_math.h"
#include "vk_format.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

bool
terakan_vertex_input_attribute_translate(uint32_t const location, uint32_t const binding,
                                         VkFormat const format, uint32_t const offset,
                                         struct terakan_vertex_input_attribute * const attribute_out)
{
   assert(location < TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES);
   assert(binding < TERAKAN_RESOURCE_HW_COUNT_FETCH);
   assert(offset <= UINT16_MAX);
   if (unlikely(location >= TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES ||
                binding >= TERAKAN_RESOURCE_HW_COUNT_FETCH || offset > UINT16_MAX)) {
      return false;
   }
   struct terascale_format_info const format_info =
      terascale_format_info_r8xx[vk_format_to_pipe_format(format)];
   if (unlikely(!format_info.supports_sq_vertex_fetch)) {
      return false;
   }
   attribute_out->word1_dst_gpr_and_format =
      S_SQ_VTX_WORD1_GPR_DST_GPR(1 + location) | S_SQ_VTX_WORD1_DST_SEL_X(format_info.swizzle_r) |
      S_SQ_VTX_WORD1_DST_SEL_Y(format_info.swizzle_g) |
      S_SQ_VTX_WORD1_DST_SEL_Z(format_info.swizzle_b) |
      S_SQ_VTX_WORD1_DST_SEL_W(format_info.swizzle_a) |
      S_SQ_VTX_WORD1_DATA_FORMAT(format_info.format) |
      S_SQ_VTX_WORD1_NUM_FORMAT_ALL(terascale_format_get_sq_num_format(
         (enum terascale_format_number_type)format_info.number_type)) |
      S_SQ_VTX_WORD1_FORMAT_COMP_ALL(format_info.channels_signed != 0b0);
   attribute_out->offset = offset;
   attribute_out->buffer_id = binding;
   return true;
}

static int
terakan_vertex_input_compare_attributes(void const * const a, void const * const b)
{
   struct terakan_vertex_input_attribute const * const a_attribute =
      (struct terakan_vertex_input_attribute const *)a;
   struct terakan_vertex_input_attribute const * const b_attribute =
      (struct terakan_vertex_input_attribute const *)b;

   /* Keep accesses to the same buffer near each other. */
   if (a_attribute->buffer_id < b_attribute->buffer_id) {
      return -1;
   }
   if (b_attribute->buffer_id < a_attribute->buffer_id) {
      return 1;
   }

   /* Load sequentially so mega and mini fetches can be specified easily. */
   if (a_attribute->offset < b_attribute->offset) {
      return -1;
   }
   if (b_attribute->offset < a_attribute->offset) {
      return 1;
   }

   uint32_t const a_dst_gpr = G_SQ_VTX_WORD1_GPR_DST_GPR(a_attribute->word1_dst_gpr_and_format);
   uint32_t const b_dst_gpr = G_SQ_VTX_WORD1_GPR_DST_GPR(b_attribute->word1_dst_gpr_and_format);
   if (a_dst_gpr < b_dst_gpr) {
      return -1;
   }
   if (b_dst_gpr < a_dst_gpr) {
      return 1;
   }

   return 0;
}

struct terakan_vertex_input_pre_mulhi_chain {
   /* Fields ordered by alignment. */
   uint32_t addresses;
   uint32_t first_op;
   uint16_t total_post_mulhi_chain_ops;
   uint8_t total_post_mulhi_chains;
   uint8_t ops_remaining;
};

static int
terakan_vertex_input_compare_pre_mulhi_chains(void const * const a, void const * const b)
{
   struct terakan_vertex_input_pre_mulhi_chain const * const a_chain =
      (struct terakan_vertex_input_pre_mulhi_chain const *)a;
   struct terakan_vertex_input_pre_mulhi_chain const * const b_chain =
      (struct terakan_vertex_input_pre_mulhi_chain const *)b;

   /* Schedule chains that unlock more post-MULHI_UINT chains themselves the earliest to maximize
    * slot utilization.
    */
   if (a_chain->total_post_mulhi_chains > b_chain->total_post_mulhi_chains) {
      return -1;
   }
   if (b_chain->total_post_mulhi_chains > a_chain->total_post_mulhi_chains) {
      return 1;
   }

   /* Schedule chains that have more dependent operations earlier to minimize program length. */
   if (a_chain->total_post_mulhi_chain_ops > b_chain->total_post_mulhi_chain_ops) {
      return -1;
   }
   if (b_chain->total_post_mulhi_chain_ops > a_chain->total_post_mulhi_chain_ops) {
      return 1;
   }

   /* Schedule shorter chains earlier to minimize program length. */
   if (a_chain->ops_remaining > b_chain->ops_remaining) {
      return -1;
   }
   if (b_chain->ops_remaining > a_chain->ops_remaining) {
      return 1;
   }

   /* Coarsely sort by the last address. */
   if (a_chain->addresses < b_chain->addresses) {
      return -1;
   }
   if (b_chain->addresses < a_chain->addresses) {
      return 1;
   }

   return 0;
}

struct terakan_vertex_input_post_mulhi_chain {
   /* Fields ordered by alignment. */
   uint32_t first_op;
   uint8_t ops_remaining;
   uint8_t address;
};

static int
terakan_vertex_input_compare_post_mulhi_chains(void const * const a, void const * const b)
{
   struct terakan_vertex_input_post_mulhi_chain const * const a_chain =
      (struct terakan_vertex_input_post_mulhi_chain const *)a;
   struct terakan_vertex_input_post_mulhi_chain const * const b_chain =
      (struct terakan_vertex_input_post_mulhi_chain const *)b;

   /* As new chains become unlocked, schedule them from the longest to the shortest to minimize
    * program length.
    */
   if (a_chain->ops_remaining > b_chain->ops_remaining) {
      return -1;
   }
   if (b_chain->ops_remaining > a_chain->ops_remaining) {
      return 1;
   }

   /* Sort by the address, coarsely similarly to pre-MULHI_UINT chain sorting. */
   if (a_chain->address < b_chain->address) {
      return -1;
   }
   if (b_chain->address < a_chain->address) {
      return 1;
   }

   return 0;
}

void
terakan_vertex_input_create_fs_alu_and_fetches(
   bool const is_r9xx, BITSET_WORD const * const attributes_needed_and_valid,
   struct terakan_vertex_input_attribute const * const attributes, uint32_t instance_bindings,
   uint32_t const * const instance_binding_divisors, uint32_t bindings_with_2048_stride_workaround,
   uint32_t * const alu_qword_count_out, uint32_t * const alu_out,
   uint32_t * const alu_clause_count_out, uint8_t * const alu_clause_qwords_out,
   uint32_t * const fetch_count_out, uint32_t * const fetch_out)
{
   uint32_t bindings_needed_by_attributes = 0b0;
   uint8_t sorted_attribute_count = 0;
   struct terakan_vertex_input_attribute sorted_attributes[TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES];
   /* Gather needed bindings and sort attributes to maximize access locality and to make logic
    * simpler (be able to assume that fetches are ordered by binding index when reusing calculations
    * between bindings).
    */
   {
      unsigned attribute_index;
      BITSET_FOREACH_SET (attribute_index, attributes_needed_and_valid,
                          TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES) {
         struct terakan_vertex_input_attribute const * const attribute =
            &attributes[attribute_index];
         bindings_needed_by_attributes |= BITFIELD_BIT(attribute->buffer_id);
         sorted_attributes[sorted_attribute_count++] = *attribute;
      }
   }
   instance_bindings &= bindings_needed_by_attributes;
   bindings_with_2048_stride_workaround &= bindings_needed_by_attributes;
   qsort(sorted_attributes, sorted_attribute_count, sizeof(sorted_attributes[0]),
         terakan_vertex_input_compare_attributes);

   /* Get the GPRs that will be last overwritten by fetches from each binding by iterating
    * attributes from the first to the last fetch instruction that will be emitted.
    */
   uint8_t binding_last_gprs[TERAKAN_RESOURCE_HW_COUNT_FETCH];
   for (uint8_t sorted_attribute_index = 0; sorted_attribute_index < sorted_attribute_count;
        ++sorted_attribute_index) {
      struct terakan_vertex_input_attribute const * const attribute =
         &sorted_attributes[sorted_attribute_index];
      binding_last_gprs[attribute->buffer_id] =
         G_SQ_VTX_WORD1_GPR_DST_GPR(attribute->word1_dst_gpr_and_format);
   }

   /* The indices passed to fetch instructions can be some of the following:
    * - The R8xx 2048 stride workaround is not needed:
    *   - Per-vertex attribute - vertex index hardware input (R0.X).
    *   - Instance divisor 0 - zero (need to MOV the inline constant 0).
    *   - Instance divisor 1 - instance index hardware input (R0.W).
    *   - Power of 2 instance divisor - result of LSHR_INT Rresult, R0.W, literal log2(divisor).
    *   - Other instance divisors - result of util_fast_udiv32 of R0.W:
    *     Optionally (pre-shift):
    *       LSHR_INT Rresult, R0.W, literal
    *     Optionally (saturated increment):
    *       MIN_UINT Rresult, R0.W/Rresult, 0xFFFFFFFE
    *       ADD_INT Rresult, Rresult, 1
    *     MULHI_UINT Rresult, R0.W/Rresult, literal
    *     Optionally (post-shift):
    *       LSHR_INT Rresult, Rresult, literal
    * - 2048 stride, which is not supported on R8xx hardware, but is mandatory - adjust the index so
    *   1024 stride can be used instead:
    *   - Per-vertex attribute - vertex index multiplied by 2 (LSHL_INT Rresult, R0.X, 1).
    *   - Per-instance attribute - still using SQ_VTX_FETCH_INSTANCE_DATA instead of multiplying the
    *     base by 2 using the ALU for read port allocation reasons explained later:
    *     - Divisor 0 - pre-loaded base instance (R0.Z).
    *     - Divisor 1 - result of:
    *       LSHL_INT Rresult, R0.W, 1
    *       ADD_INT Rresult, Rresult, R0.Z
    *     - Other instance divisors - result of:
    *       LSHL_INT Rresult, Rresult, 1
    *       ADD_INT Rresult, Rresult, R0.Z
    *
    * These operations can be grouped into a few kinds:
    * - (XYZWT) OP2 Rresult, R0.#, literal
    * - (XYZWT) OP2 Rresult, Rresult, literal
    * - (T on R8xx, 4-slot on R9xx) MULHI_UINT Rresult, R0.W, literal
    * - (T on R8xx, 4-slot on R9xx) MULHI_UINT Rresult, Rresult, literal
    * - (XYZWT) MOV Rresult, 0
    * - (XYZWT) ADD_INT Rresult, Rresult, R0.Z
    *
    * All of these instructions may accept:
    * - At most one operand from a component of R0.
    * - At most one operand from the intermediate result.
    * - At most one constant.
    *
    * Therefore, it should be possible to allocate read ports for all 3 cycles without collisions in
    * all of these cases.
    * The transcendental slot instruction can't load GPR operands on cycle 0 if it uses a constant,
    * so cycle 0 should preferably be used for the needs of XYZW slot instructions.
    * So, cycles can be allocated unambiguously the following way:
    * - Cycle 0 - intermediate result for XYZW slot instructions (example: R1.X, R2.Y, R3.Z, R4.W).
    * - Cycle 1 - intermediate result for T slot instructions (like R5.X without a collision with
    *   R1.X in the above example).
    * - Cycle 2 - R0.
    *
    * This cycle allocation is the reason why SQ_VTX_FETCH_INSTANCE_DATA is still used for
    * multiplying the base instance by 2 - otherwise, with a divisor of 1, if ALU did
    * `(gl_InstanceID + gl_BaseInstance) * 2` instead of `gl_InstanceID * 2 + gl_BaseInstance`,
    * there would've been an ADD_INT Rresult, R0.W, R0.Z instruction, requiring two cycles in XYZW
    * slots for R0 sources, and causing a potential collision with another instruction in the group
    * accessing Z or W at a different GPR index.
    *
    * In order not to cause collisions between intermediate result GPR loads, XYZW lanes don't start
    * working on the 5th fetch index until all needed operations are done for any of the 4 indices
    * before it.
    * The transcendental slot instruction can do pending work for any index, however, as long as the
    * group doesn't require 5 different literals in this case. This may happen even if the index
    * hasn't been given a vector lane so far - in this case the destination component will be
    * inserted into preceding transcendental instructions for it once it's first used on XYZW slots.
    *
    * MULHI_UINT is done only on the transcendental slot on R8xx, and occupies all 4 slots on R9xx.
    * It's done as soon as possible once the input for it is ready, prioritized over using the
    * transcendental slot as yet another slot for XYZWT instructions, because a long chain of
    * instructions may depend on its result.
    */

   /* Gather unique binding-relative address formulas that can be shared between bindings, and
    * construct operation chains for them.
    */

   struct address_gpr_set {
      uint8_t current_gpr;
      uint8_t current_component;
      /* 0 can be used as a special value. */
      uint8_t result_gpr;
   };

   enum address_gpr_set_index {
      address_gpr_set_index_generic_stride,
      address_gpr_set_index_2048_stride,

      address_gpr_set_index_count,
   };

   struct address {
      /* Irrelevant fields must be zeroed for simplicity of comparisons. */
      struct util_fast_udiv_info divisor_info;

      struct address_gpr_set gpr_sets[address_gpr_set_index_count];
      uint8_t current_gpr_set;

      /* UINT8_MAX if neither MULHI_UINT nor a post-MULHI_UINT chain is needed. */
      uint8_t sorted_mulhi_and_post_mulhi_chain;
   };

   struct address addresses[TERAKAN_RESOURCE_HW_COUNT_FETCH];
   uint8_t address_count = 0;

   uint8_t binding_addresses[TERAKAN_RESOURCE_HW_COUNT_FETCH];

   enum alu_op {
      alu_op_lshr_pre,
      alu_op_min_uint32_max_minus_1,
      alu_op_add_1,
      /* The MULHI_UINT part of the division is handled specially between the chain of
       * pre-multiplication and the chain of post-multiplication operations.
       */
      alu_op_lshr_post,
      /* Use the result of LSHL_INT of the current GPR set's intermediate result by 1 as the source
       * for the 2048 stride GPR set, and do subsequent operations with the 2048 stride GPR set.
       */
      alu_op_lshl_1_and_use_2048_stride_gpr,
      alu_op_add_base_instance,
      alu_op_mov_0,
   };

   /* The maximum counts are the numbers of alu_ops on the longest possible path. */
   uint8_t alu_ops[6 * TERAKAN_RESOURCE_HW_COUNT_FETCH];
   uint32_t alu_op_count = 0;

   struct terakan_vertex_input_pre_mulhi_chain pre_mulhi_chains[TERAKAN_RESOURCE_HW_COUNT_FETCH];
   uint8_t pre_mulhi_chain_count = 0;

   struct terakan_vertex_input_post_mulhi_chain post_mulhi_chains[TERAKAN_RESOURCE_HW_COUNT_FETCH];
   uint8_t post_mulhi_chain_count = 0;

   /* Per-vertex bindings. */
   uint32_t const vertex_bindings = bindings_needed_by_attributes & ~instance_bindings;
   uint8_t vertex_address_index;
   if (vertex_bindings) {
      vertex_address_index = address_count++;
      struct address * const vertex_address = &addresses[vertex_address_index];

      vertex_address->divisor_info = (struct util_fast_udiv_info){};

      /* Always using R0.X directly for generic-stride vertex fetch. */
      vertex_address->gpr_sets[address_gpr_set_index_generic_stride].current_gpr = 0;
      vertex_address->gpr_sets[address_gpr_set_index_generic_stride].current_component = 0;
      vertex_address->gpr_sets[address_gpr_set_index_generic_stride].result_gpr = 0;
      uint32_t const vertex_bindings_with_2048_stride_workaround =
         vertex_bindings & bindings_with_2048_stride_workaround;

      if (vertex_bindings_with_2048_stride_workaround) {
         vertex_address->gpr_sets[address_gpr_set_index_2048_stride].current_gpr = 0;
         vertex_address->gpr_sets[address_gpr_set_index_2048_stride].current_component = 0;
         vertex_address->gpr_sets[address_gpr_set_index_2048_stride].result_gpr =
            binding_last_gprs[util_last_bit(vertex_bindings_with_2048_stride_workaround)];

         struct terakan_vertex_input_post_mulhi_chain * const vertex_2048_stride_chain =
            &post_mulhi_chains[post_mulhi_chain_count++];
         vertex_2048_stride_chain->address = vertex_address_index;
         vertex_2048_stride_chain->first_op = alu_op_count;
         alu_ops[alu_op_count++] = alu_op_lshl_1_and_use_2048_stride_gpr;
         vertex_2048_stride_chain->ops_remaining =
            alu_op_count - vertex_2048_stride_chain->first_op;
      }

      vertex_address->current_gpr_set = address_gpr_set_index_generic_stride;

      {
         unsigned vertex_bindings_remaining = vertex_bindings;
         while (vertex_bindings_remaining) {
            binding_addresses[u_bit_scan(&vertex_bindings_remaining)] = vertex_address_index;
         }
      }
   }

   /* Per-instance bindings. */
   {
      unsigned instance_bindings_remaining = instance_bindings;
      while (instance_bindings_remaining) {
         uint8_t const binding_index = u_bit_scan(&instance_bindings_remaining);
         uint32_t const binding_bit = BITFIELD_BIT(binding_index);

         uint8_t const instance_address_index = address_count++;
         struct address * const instance_address = &addresses[instance_address_index];

         uint32_t const divisor = instance_binding_divisors[binding_index];

         instance_address->gpr_sets[address_gpr_set_index_generic_stride].current_gpr = 0;
         instance_address->gpr_sets[address_gpr_set_index_generic_stride].current_component = 3;
         instance_address->gpr_sets[address_gpr_set_index_generic_stride].result_gpr = 0;
         instance_address->gpr_sets[address_gpr_set_index_2048_stride].current_gpr = 0;
         /* If the divisor is 0, passing the base instance in R0.Z directly to the fetch for 2048
          * stride: base instance from R0.Z + base instance from SQ_VTX_FETCH_INSTANCE_DATA =
          * 2 * base instance.
          * alu_op_lshl_1_and_use_2048_stride_gpr is not inserted in this case, and the generic
          * stride GPR set thus won't be copied to the 2048 stride one.
          */
         instance_address->gpr_sets[address_gpr_set_index_2048_stride].current_component =
            divisor == 0 ? 2 : 3;
         instance_address->gpr_sets[address_gpr_set_index_2048_stride].result_gpr = 0;
         instance_address->current_gpr_set = address_gpr_set_index_generic_stride;

         /* Use the same address for all bindings with the same divisor. */
         {
            /* By including the current binding itself, the logic can be reused for both the first
             * and the other bindings.
             */
            unsigned dedup_instance_bindings_remaining = instance_bindings_remaining | binding_bit;
            while (dedup_instance_bindings_remaining) {
               uint8_t const dedup_binding_index = u_bit_scan(&dedup_instance_bindings_remaining);
               if (instance_binding_divisors[dedup_binding_index] != divisor) {
                  continue;
               }
               uint32_t const dedup_binding_bit = BITFIELD_BIT(dedup_binding_index);

               instance_address
                  ->gpr_sets[bindings_with_2048_stride_workaround & dedup_binding_bit
                                ? address_gpr_set_index_2048_stride
                                : address_gpr_set_index_generic_stride]
                  .result_gpr = binding_last_gprs[dedup_binding_index];

               binding_addresses[dedup_binding_index] = instance_address_index;

               instance_bindings_remaining &= ~dedup_binding_bit;
            }
         }
         if (instance_address->gpr_sets[address_gpr_set_index_generic_stride].result_gpr == 0) {
            /* The address is used only for 2048-stride bindings, write the results of both the
             * generic part and the 2048 stride adjustment to the same GPR.
             */
            instance_address->gpr_sets[address_gpr_set_index_generic_stride].result_gpr =
               instance_address->gpr_sets[address_gpr_set_index_2048_stride].result_gpr;
         }

         instance_address->divisor_info = (struct util_fast_udiv_info){};
         if (divisor != 0) {
            if (IS_POT(divisor)) {
               instance_address->divisor_info.post_shift = ffs(divisor) - 1;
            } else {
               instance_address->divisor_info = util_compute_fast_udiv_info(divisor, 32, 32);
            }
         }

         /* Fill the new chains, but append them only if they are actually needed. */

         struct terakan_vertex_input_post_mulhi_chain * const new_post_mulhi_chain =
            &post_mulhi_chains[post_mulhi_chain_count];
         new_post_mulhi_chain->address = instance_address_index;
         new_post_mulhi_chain->first_op = alu_op_count;
         if (divisor == 0) {
            /* If there are bindings with 0 divisor and the 2048 stride workaround not needed, MOV 0
             * to the generic stride GPR set.
             * The address can be needed for both non-2048 and 2048 strides, in this case the GPR
             * set for 2048 stride will just read from R0.Z from the 2048 stride GPR set.
             */
            if (instance_address->gpr_sets[address_gpr_set_index_generic_stride].result_gpr !=
                instance_address->gpr_sets[address_gpr_set_index_2048_stride].result_gpr) {
               alu_ops[alu_op_count++] = alu_op_mov_0;
            }
         } else {
            if (instance_address->divisor_info.post_shift != 0) {
               alu_ops[alu_op_count++] = alu_op_lshr_post;
            }
            if (instance_address->gpr_sets[address_gpr_set_index_2048_stride].result_gpr != 0) {
               alu_ops[alu_op_count++] = alu_op_lshl_1_and_use_2048_stride_gpr;
               alu_ops[alu_op_count++] = alu_op_add_base_instance;
            }
         }
         new_post_mulhi_chain->ops_remaining = alu_op_count - new_post_mulhi_chain->first_op;
         if (new_post_mulhi_chain->ops_remaining != 0) {
            ++post_mulhi_chain_count;
         }

         if (instance_address->divisor_info.multiplier != 0) {
            struct terakan_vertex_input_pre_mulhi_chain * const new_pre_mulhi_chain =
               &pre_mulhi_chains[pre_mulhi_chain_count];
            new_pre_mulhi_chain->addresses |= BITFIELD_BIT(instance_address_index);
            new_pre_mulhi_chain->total_post_mulhi_chain_ops = new_post_mulhi_chain->ops_remaining;
            new_pre_mulhi_chain->total_post_mulhi_chains =
               new_post_mulhi_chain->ops_remaining != 0 ? 1 : 0;
            new_pre_mulhi_chain->first_op = alu_op_count;
            if (instance_address->divisor_info.pre_shift != 0) {
               alu_ops[alu_op_count++] = alu_op_lshr_pre;
            }
            if (instance_address->divisor_info.increment != 0) {
               assert(instance_address->divisor_info.increment == 1);
               alu_ops[alu_op_count++] = alu_op_min_uint32_max_minus_1;
               alu_ops[alu_op_count++] = alu_op_add_1;
            }
            new_pre_mulhi_chain->ops_remaining = alu_op_count - new_pre_mulhi_chain->first_op;
            if (new_pre_mulhi_chain->ops_remaining != 0) {
               /* Deduplicate operations preceding MULHI_UINT, as MULHI_UINT is scheduled in a way
                * that lets it read from any component of any GPR, so if multiple bindings require
                * the same preparations for MULHI_UINT, there's no need to do them multiple times.
                */
               bool is_pre_mulhi_chain_dup = false;
               for (uint8_t dedup_pre_mulhi_chain_index = 0;
                    dedup_pre_mulhi_chain_index < pre_mulhi_chain_count;
                    ++dedup_pre_mulhi_chain_index) {
                  struct terakan_vertex_input_pre_mulhi_chain * const dedup_pre_mulhi_chain =
                     &pre_mulhi_chains[dedup_pre_mulhi_chain_index];
                  if (dedup_pre_mulhi_chain->ops_remaining != new_pre_mulhi_chain->ops_remaining ||
                      memcmp(alu_ops + dedup_pre_mulhi_chain->first_op,
                             alu_ops + new_pre_mulhi_chain->first_op,
                             sizeof(alu_ops[0]) * new_pre_mulhi_chain->ops_remaining) != 0 ||
                      addresses[ffs(dedup_pre_mulhi_chain->addresses) - 1].divisor_info.pre_shift !=
                         instance_address->divisor_info.pre_shift) {
                     continue;
                  }
                  is_pre_mulhi_chain_dup = true;
                  dedup_pre_mulhi_chain->addresses |= new_pre_mulhi_chain->addresses;
                  dedup_pre_mulhi_chain->total_post_mulhi_chain_ops +=
                     new_pre_mulhi_chain->total_post_mulhi_chain_ops;
                  dedup_pre_mulhi_chain->total_post_mulhi_chains +=
                     new_pre_mulhi_chain->total_post_mulhi_chains;
                  break;
               }
               if (!is_pre_mulhi_chain_dup) {
                  ++pre_mulhi_chain_count;
               }
            }
         }
      }
   }

   /* Sort chains of operations so that operations that more other operations depend on are done
    * earlier, and that near the end, it's unlikely that a long lone chain gets scheduled.
    */
   qsort(pre_mulhi_chains, pre_mulhi_chain_count, sizeof(pre_mulhi_chains[0]),
         terakan_vertex_input_compare_pre_mulhi_chains);
   qsort(post_mulhi_chains, post_mulhi_chain_count, sizeof(post_mulhi_chains[0]),
         terakan_vertex_input_compare_post_mulhi_chains);

   for (uint8_t address_index = 0; address_index < address_count; ++address_index) {
      addresses[address_index].sorted_mulhi_and_post_mulhi_chain = UINT8_MAX;
   }

   /* Assign MULHI_UINTs and post-MULHI_UINT chains to addresses after sorting.
    * MULHI_UINTs are prioritized the same as the post-MULHI_UINT chains that depend on them, so
    * MULHI_UINTs with the index < post_mulhi_chain_count share indices with their respective
    * post-MULHI_UINT chains.
    * Beyond post_mulhi_chain_count, so as lowest-priority, allocate MULHI_UINTs for addresses that
    * don't need any ALU operations after their MULHI_UINTs.
    */
   uint8_t mulhi_addresses[TERAKAN_RESOURCE_HW_COUNT_FETCH];
   for (uint8_t post_mulhi_chain_index = 0; post_mulhi_chain_index < post_mulhi_chain_count;
        ++post_mulhi_chain_index) {
      uint8_t const address_index = post_mulhi_chains[post_mulhi_chain_index].address;
      addresses[address_index].sorted_mulhi_and_post_mulhi_chain = post_mulhi_chain_index;
      mulhi_addresses[post_mulhi_chain_index] = address_index;
   }
   uint32_t mulhis_needed = 0b0;
   uint8_t next_mulhi_without_post_chain_index = post_mulhi_chain_count;
   for (uint8_t address_index = 0; address_index < address_count; ++address_index) {
      struct address * const address = &addresses[address_index];
      if (address->divisor_info.multiplier == 0) {
         continue;
      }
      if (address->sorted_mulhi_and_post_mulhi_chain == UINT8_MAX) {
         address->sorted_mulhi_and_post_mulhi_chain = next_mulhi_without_post_chain_index++;
      }
      mulhi_addresses[address->sorted_mulhi_and_post_mulhi_chain] = address_index;
      mulhis_needed |= BITFIELD_BIT(address->sorted_mulhi_and_post_mulhi_chain);
   }

   /* Enqueue MULHI_UINTs that may be executed immediately, by first enqueuing all, and then
    * dequeueing those that have pre-MULHI_UINT chains.
    * Also gather the GPRs that pre-MULHI_UINT chains must write to because their results may be
    * reused between multiple operations, so their results must not be overwritten until they're not
    * needed anymore (this assumes that MULHI_UINTs are scheduled strictly from LSB to MSB of
    * mulhis_ready).
    * In addition, obtain which post-MULHI_UINT chains don't depend on anything and can be scheduled
    * immediately.
    */
   uint8_t pre_mulhi_chain_result_gprs[TERAKAN_RESOURCE_HW_COUNT_FETCH];
   uint32_t mulhis_ready = mulhis_needed;
   uint32_t post_mulhi_chains_ready = BITFIELD_MASK(post_mulhi_chain_count) & ~mulhis_needed;
   for (uint8_t pre_mulhi_chain_index = 0; pre_mulhi_chain_index < pre_mulhi_chain_count;
        ++pre_mulhi_chain_index) {
      struct terakan_vertex_input_pre_mulhi_chain const * const pre_mulhi_chain =
         &pre_mulhi_chains[pre_mulhi_chain_index];
      uint8_t pre_mulhi_chain_last_mulhi = 0;
      unsigned addresses_remaining = pre_mulhi_chain->addresses;
      while (addresses_remaining) {
         struct address const * const address = &addresses[u_bit_scan(&addresses_remaining)];
         assert(address->sorted_mulhi_and_post_mulhi_chain != UINT8_MAX);
         uint32_t const mulhi_and_post_mulhi_chain_bit =
            BITFIELD_BIT(address->sorted_mulhi_and_post_mulhi_chain);
         mulhis_ready &= ~mulhi_and_post_mulhi_chain_bit;
         post_mulhi_chains_ready &= ~mulhi_and_post_mulhi_chain_bit;
         pre_mulhi_chain_last_mulhi =
            MAX2(address->sorted_mulhi_and_post_mulhi_chain, pre_mulhi_chain_last_mulhi);
      }
      pre_mulhi_chain_result_gprs[pre_mulhi_chain_index] =
         addresses[mulhi_addresses[pre_mulhi_chain_last_mulhi]]
            .gpr_sets[address_gpr_set_index_generic_stride]
            .result_gpr;
   }
   uint32_t pre_mulhi_chains_ready = BITFIELD_MASK(pre_mulhi_chain_count);

   /* Schedule ALU operations. */

   uint32_t alu_dword_count = 0;
   uint32_t alu_clause_count = 0;
   uint8_t alu_current_clause_qwords = 0;

   uint8_t scheduled_components = 0b0;
   uint8_t scheduled_component_chains[4];
   bool scheduled_component_chains_are_pre[4];
   uint32_t address_previous_write_trans_word1_offsets[TERAKAN_RESOURCE_HW_COUNT_FETCH] = {};

   while (pre_mulhi_chains_ready || mulhis_ready || post_mulhi_chains_ready ||
          scheduled_components) {
      uint8_t slot_count_to_try_scheduling = is_r9xx ? 4 : 5;

      uint32_t const group_start = alu_dword_count;

      uint32_t group_literals[4];
      uint8_t group_literal_count = 0;

      /* Do the multiplication as early as possible. */

      /* Note that MULHI_UINTs must be executed strictly from LSB to MSB of mulhis_ready.
       * If this is changed, the way the target GPRs of pre-MULHI_UINT chains are selected must be
       * updated accordingly, because the result of one pre-MULHI_UINT chain may be used by multiple
       * MULHI_UINTs, and it must not be overwritten while it's still needed.
       */
      uint8_t const mulhi_index = mulhis_ready ? ffs(mulhis_ready) - 1 : UINT8_MAX;
      unsigned mulhi_divisor_sel;
      if (mulhi_index != UINT8_MAX) {
         struct address * const mulhi_address = &addresses[mulhi_addresses[mulhi_index]];

         uint32_t const group_multiplier = (uint32_t)mulhi_address->divisor_info.multiplier;
         r600_bytecode_special_constants(group_multiplier, &mulhi_divisor_sel);
         if (mulhi_divisor_sel == V_SQ_ALU_SRC_LITERAL) {
            assert(group_literal_count == 0);
            group_literals[group_literal_count++] = group_multiplier;
         }

         if (is_r9xx) {
            /* Emit the 4-slot MULHI_UINT, writing to all components so post-multiplication
             * operations don't have to patch this instruction.
             */
            struct address_gpr_set * const mulhi_gpr_set =
               &mulhi_address->gpr_sets[mulhi_address->current_gpr_set];
            uint32_t const mulhi_group_start = alu_dword_count;
            uint32_t const r9xx_mulhi[] = {
               S_SQ_ALU_WORD0_SRC0_SEL(mulhi_gpr_set->current_gpr) |
                  S_SQ_ALU_WORD0_SRC0_CHAN(mulhi_gpr_set->current_component) |
                  S_SQ_ALU_WORD0_SRC1_SEL(mulhi_divisor_sel) | S_SQ_ALU_WORD0_SRC1_CHAN(0),
               S_SQ_ALU_WORD1_DST_GPR(mulhi_gpr_set->result_gpr) |
                  S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
                  S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULHI_UINT)};
            for (uint8_t mulhi_slot = 0; mulhi_slot < 4; ++mulhi_slot) {
               alu_out[alu_dword_count++] = r9xx_mulhi[0] | S_SQ_ALU_WORD0_LAST(mulhi_slot == 3);
               alu_out[alu_dword_count++] = r9xx_mulhi[1] | S_SQ_ALU_WORD1_DST_CHAN(mulhi_slot);
            }
            if (mulhi_divisor_sel == V_SQ_ALU_SRC_LITERAL) {
               assert(group_literal_count == 1);
               alu_out[alu_dword_count++] = group_literals[0];
               alu_out[alu_dword_count++] = 0;
            }
            uint8_t const mulhi_group_qwords = (alu_dword_count - mulhi_group_start) / 2;
            if (alu_current_clause_qwords + mulhi_group_qwords > 0x80) {
               if (alu_current_clause_qwords != 0) {
                  alu_clause_qwords_out[alu_clause_count++] = alu_current_clause_qwords;
                  alu_current_clause_qwords = 0;
               }
               alu_current_clause_qwords += mulhi_group_qwords;
            }
            mulhi_gpr_set->current_gpr = mulhi_gpr_set->result_gpr;

            uint32_t const mulhi_index_bit = BITFIELD_BIT(mulhi_index);
            mulhis_ready &= ~mulhi_index_bit;
            if (mulhi_index < post_mulhi_chain_count) {
               post_mulhi_chains_ready |= mulhi_index_bit;
            }

            continue;
         }

         slot_count_to_try_scheduling = 4;
      }

      for (uint8_t slot_index = 0; slot_index < slot_count_to_try_scheduling; ++slot_index) {
         bool const slot_is_trans = slot_index == 4;

         bool slot_chain_is_pre;
         uint8_t slot_chain_index = UINT8_MAX;
         if (slot_is_trans) {
            if (alu_dword_count - group_start < 2 * 4) {
               /* For instructions that can be executed by either the vector or the transcendental
                * units, the assignment is ambiguous - if the last instruction doesn't write to the
                * same component as any other instruction in the group, on R8xx it's assigned to the
                * vector unit rather than the transcendental one (like with PREFER_VECTOR enabled on
                * R6xx/R7xx).
                * However, which unit an instruction gets assigned to determines how the bank
                * swizzle field is interpreted - as one of ALU_VEC or as one of ALU_SCL
                * permutations, and if that's not taken into account, read port conflicts may
                * happen.
                * Here, the transcendental unit executes operations from pending chains before those
                * chains are pinned to a specific component (pinned chains are guaranteed to have
                * one operation executed in each instruction group unless they exceed the literal
                * limit, but that's only possible if the transcendental unit is already occupied by
                * MULHI_UINT, so there's no need to try to run them in the transcendental slot), so
                * the actual component the transcendental slot instruction should write to isn't
                * known at this point - it will be patched in if the chain gets pinned to a vector
                * component later. Thus, to ensure that the instruction gets assigned to the
                * transcendental slot unambiguously, all unused vector slots would have to be filled
                * with NOPs explicitly.
                * But if nothing was scheduled to any of the XYZW slots, this generally means (more
                * precisely, assuming that the literal count never overflows in XYZW slots alone,
                * always means) that scheduling of pending chains to XYZW slots has already failed,
                * so it will likely fail for the transcendental slot too - so no need to even try
                * anyway.
                */
               break;
            }
         } else {
            if (scheduled_components & ((uint8_t)1 << slot_index)) {
               /* Continue the chain started in this slot, using the same component for both reads
                * and writes of the intermediate result until the chain is done or becomes ready for
                * MULHI_UINT so that no read port conflicts may happen between different chains.
                * Note that if allocation fails for some reason such as literal count overflow,
                * because of that, an attempt to fill this slot with an operation for some unpinned
                * chain shouldn't be made as that'd result in multiple chains pinned to the same
                * component - just leave this slot unused (an exception can be made for chains
                * with only one operation though, reading only from either R0 or the result of an
                * arbitrary-destination MULHI_UINT, but no need to complicate the logic with that).
                */
               slot_chain_is_pre = scheduled_component_chains_are_pre[slot_index];
               slot_chain_index = scheduled_component_chains[slot_index];
            }
         }
         if (slot_chain_index == UINT8_MAX) {
            /* Try to execute the highest-priority pending chain.
             * Though it should be possible to try every pending chain if scheduling of one fails,
             * like due to an overflow of the literal count limit, the possibility of that and the
             * difference potentially made by that are extremely insignificant, so just try the
             * first one. This code may also be executed frequently (in vkCmdSetVertexInputEXT).
             * Using multiple different instance index divisors, especially more than 4 of them, is
             * a situation that probably never happens in reality anyway. All the complex scheduling
             * done here is in unnecessary in most cases anyway, but it would've been just wrong to
             * completely ignore the core principle of the design of the ALU in this hardware and
             * simply generate "scalar" shaders with no more than one XYZW slot per group occupied
             * if that actually happens.
             */
            if (pre_mulhi_chains_ready) {
               slot_chain_is_pre = true;
               slot_chain_index = ffs(pre_mulhi_chains_ready) - 1;
            } else if (post_mulhi_chains_ready) {
               slot_chain_is_pre = false;
               slot_chain_index = ffs(post_mulhi_chains_ready) - 1;
            } else {
               /* Nothing to do in this slot, no chain started in it, and no new chains.
                * Other slots may still have chains started in them, however, so continue.
                */
               continue;
            }
         }

         struct terakan_vertex_input_pre_mulhi_chain * const slot_pre_mulhi_chain =
            slot_chain_is_pre ? &pre_mulhi_chains[slot_index] : NULL;
         struct terakan_vertex_input_post_mulhi_chain * const slot_post_mulhi_chain =
            slot_chain_is_pre ? NULL : &post_mulhi_chains[slot_index];

         /* Pre-MULHI_UINT chains are deduplicated between addresses, all relevant inputs are the
          * same in all addresses.
          */
         uint8_t const slot_address_index = slot_chain_is_pre
                                               ? ffs(slot_pre_mulhi_chain->addresses) - 1
                                               : slot_post_mulhi_chain->address;
         struct address * const slot_address = &addresses[slot_address_index];

         enum alu_op const slot_op =
            (enum alu_op)alu_ops[slot_chain_is_pre ? slot_pre_mulhi_chain->first_op
                                                   : slot_post_mulhi_chain->first_op];

         /* Allocate the literal needed by the operation. */
         uint32_t slot_literal;
         switch (slot_op) {
         case alu_op_lshr_pre:
            slot_literal = slot_address->divisor_info.pre_shift;
            break;
         case alu_op_min_uint32_max_minus_1:
            slot_literal = UINT32_MAX - 1;
            break;
         case alu_op_add_1:
         case alu_op_lshl_1_and_use_2048_stride_gpr:
            slot_literal = 1;
            break;
         case alu_op_lshr_post:
            slot_literal = slot_address->divisor_info.post_shift;
            break;
         case alu_op_mov_0:
            slot_literal = 0;
            break;
         default:
            /* Initialize to a value that can be represented as an inline constant not to allocate a
             * new literal.
             */
            slot_literal = 0;
            break;
         }
         unsigned slot_literal_sel;
         r600_bytecode_special_constants(slot_literal, &slot_literal_sel);
         uint8_t slot_literal_component = 0;
         if (slot_literal_sel == V_SQ_ALU_SRC_LITERAL) {
            for (slot_literal_component = 0; slot_literal_component < group_literal_count;
                 ++slot_literal_component) {
               if (group_literals[slot_literal_component] == slot_literal) {
                  break;
               }
            }
            if (slot_literal_component >= group_literal_count) {
               if (group_literal_count >= 4) {
                  /* Too many literals, can't schedule an operation with a new literal. */
                  continue;
               }
               slot_literal_component = group_literal_count++;
            }
         }

         uint32_t slot_words[2] = {0, S_SQ_ALU_WORD1_OP2_WRITE_MASK(1)};

         if (slot_op == alu_op_mov_0) {
            /* No GPR sources, use the inline 0 both as the source and to fill the unused (but still
             * subject to the cycle restrictions) second operand.
             */
            slot_words[0] |=
               S_SQ_ALU_WORD0_SRC0_SEL(V_SQ_ALU_SRC_0) | S_SQ_ALU_WORD0_SRC1_SEL(V_SQ_ALU_SRC_0);
            slot_words[1] |= S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MOV);
         } else {
            struct address_gpr_set * const source_gpr_set =
               &slot_address->gpr_sets[slot_address->current_gpr_set];
            /* If the last write for the address was done on the transcendental slot to an arbitrary
             * component, but now a read port of a certain component is being used, make that
             * transcendental unit instruction write to that component.
             */
            if (!slot_is_trans) {
               uint32_t const address_previous_write_trans_word1_offset =
                  address_previous_write_trans_word1_offsets[slot_address_index];
               if (address_previous_write_trans_word1_offset != 0) {
                  uint32_t * const address_previous_write_trans_word1 =
                     &alu_out[address_previous_write_trans_word1_offset];
                  /* Don't do this if, for example, that write was for a different GPR set, but the
                   * current GPR set needs to, for instance, read from R0 instead.
                   */
                  if (G_SQ_ALU_WORD1_DST_GPR(*address_previous_write_trans_word1) ==
                      source_gpr_set->current_gpr) {
                     source_gpr_set->current_component = slot_index;
                     *address_previous_write_trans_word1 =
                        (*address_previous_write_trans_word1 & C_SQ_ALU_WORD1_DST_CHAN) |
                        S_SQ_ALU_WORD1_DST_CHAN(source_gpr_set->current_component);
                  }
               }
            }
            slot_words[0] |= S_SQ_ALU_WORD0_SRC0_SEL(source_gpr_set->current_gpr) |
                             S_SQ_ALU_WORD0_SRC0_CHAN(source_gpr_set->current_component);
            if (slot_op == alu_op_add_base_instance) {
               /* Reading from R0.Z on the same cycle that intermediate results are loaded on may
                * cause a read port collision if the intermediate result is also in Z. This should
                * never happen though as the base instance index addition should be done only after
                * LSHL_INT by 1 being a part of the 2048 stride workaround.
                */
               assert(G_SQ_ALU_WORD0_SRC0_SEL(slot_words[0]) != 0);
               slot_words[0] |= S_SQ_ALU_WORD0_SRC1_SEL(0) | S_SQ_ALU_WORD0_SRC1_CHAN(2);
               slot_words[1] |=
                  S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT);
            } else {
               slot_words[0] |= S_SQ_ALU_WORD0_SRC1_SEL(slot_literal_sel) |
                                S_SQ_ALU_WORD0_SRC1_CHAN(slot_literal_component);
               uint32_t word1_op = EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_NOP;
               switch (slot_op) {
               case alu_op_lshr_pre:
               case alu_op_lshr_post:
                  word1_op = EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_LSHR_INT;
                  break;
               case alu_op_min_uint32_max_minus_1:
                  word1_op = EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MIN_UINT;
                  break;
               case alu_op_add_1:
                  word1_op = EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_ADD_INT;
                  break;
               case alu_op_lshl_1_and_use_2048_stride_gpr:
                  word1_op = EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_LSHL_INT;
                  break;
               default:
                  unreachable("Unhandled ALU operation");
               }
               slot_words[1] |= S_SQ_ALU_WORD1_OP2_ALU_INST(word1_op);
            }
            /* Cycle 0:
             * - XYZW - intermediate result.
             * - T - no GPRs (GPR reads for the transcendental unit are not allowed on cycle 0 if
             *       using one constant).
             * Cycle 1:
             * - XYZW - no GPRs.
             * - T - intermediate result.
             * Cycle 2:
             * - XYZWT - R0.
             *
             * Source 0 may be an intermediate result or R0.
             * Source 1 may be a constant or R0 (but can't be R0 when source 0 is R0).
             */
            slot_words[1] |=
               S_SQ_ALU_WORD1_BANK_SWIZZLE(G_SQ_ALU_WORD0_SRC0_SEL(slot_words[0]) == 0
                                              ? (slot_is_trans ? SQ_ALU_SCL_212 : SQ_ALU_VEC_201)
                                              : (slot_is_trans ? SQ_ALU_SCL_122 : SQ_ALU_VEC_021));
         }

         /* Specify the destination and do the needed updates for it. */
         if (slot_op == alu_op_lshl_1_and_use_2048_stride_gpr) {
            assert(!slot_chain_is_pre);
            /* The result of this operation and subsequent ones is needed for 2048-stride bindings,
             * not for generic-stride ones.
             */
            slot_address->current_gpr_set = address_gpr_set_index_2048_stride;
         }
         /* Pre-MULHI_UINT chains are deduplicated and have their own set of result GPR indices that
          * prevents premature overwriting.
          */
         slot_words[1] |= S_SQ_ALU_WORD1_DST_GPR(
            slot_chain_is_pre ? pre_mulhi_chain_result_gprs[slot_address_index]
                              : slot_address->gpr_sets[slot_address->current_gpr_set].result_gpr);
         /* For the transcendental slot, the destination component will be written by the first use
          * on XYZW slots - write to X by default.
          */
         if (!slot_is_trans) {
            slot_words[1] |= S_SQ_ALU_WORD1_DST_CHAN(slot_index);
         }
         {
            unsigned update_addresses_remaining = slot_chain_is_pre
                                                     ? slot_pre_mulhi_chain->addresses
                                                     : BITFIELD_BIT(slot_address_index);
            while (update_addresses_remaining) {
               uint8_t const update_address_index = u_bit_scan(&update_addresses_remaining);
               struct address * const update_address = &addresses[update_address_index];
               struct address_gpr_set * const update_gpr_set =
                  &update_address->gpr_sets[update_address->current_gpr_set];
               update_gpr_set->current_gpr = G_SQ_ALU_WORD1_DST_GPR(slot_words[1]);
               update_gpr_set->current_component = G_SQ_ALU_WORD1_DST_CHAN(slot_words[1]);
               address_previous_write_trans_word1_offsets[update_address_index] =
                  slot_is_trans ? alu_dword_count + 1 : 0;
            }
         }

         /* Emit the operation. */
         alu_out[alu_dword_count++] = slot_words[0];
         alu_out[alu_dword_count++] = slot_words[1];

         /* Go to the next operation or chain. */
         bool chain_ended;
         if (slot_chain_is_pre) {
            pre_mulhi_chains_ready &= ~BITFIELD_BIT(slot_chain_index);
            ++slot_pre_mulhi_chain->first_op;
            chain_ended = --slot_pre_mulhi_chain->ops_remaining == 0;
            if (chain_ended) {
               unsigned new_mulhi_addresses_remaining = slot_pre_mulhi_chain->addresses;
               while (new_mulhi_addresses_remaining) {
                  mulhis_ready |= BITFIELD_BIT(addresses[u_bit_scan(&new_mulhi_addresses_remaining)]
                                                  .sorted_mulhi_and_post_mulhi_chain);
               }
            }
         } else {
            post_mulhi_chains_ready &= ~BITFIELD_BIT(slot_chain_index);
            ++slot_post_mulhi_chain->first_op;
            chain_ended = --slot_post_mulhi_chain->ops_remaining == 0;
         }
         if (!slot_is_trans) {
            if (chain_ended) {
               scheduled_components &= ~((uint8_t)1 << slot_index);
            } else {
               scheduled_component_chains[slot_index] = slot_chain_index;
               scheduled_component_chains_are_pre[slot_index] = slot_chain_is_pre;
               scheduled_components |= (uint8_t)1 << slot_index;
            }
         }
      }

      if (mulhi_index != UINT8_MAX) {
         assert(!is_r9xx);

         uint8_t const mulhi_address_index = mulhi_addresses[mulhi_index];
         struct address * const mulhi_address = &addresses[mulhi_address_index];

         /* Emit the MULHI_UINT on the transcendental unit (writing to the X component for now, will
          * be changed later if needed).
          */
         struct address_gpr_set * const mulhi_gpr_set =
            &mulhi_address->gpr_sets[mulhi_address->current_gpr_set];
         alu_out[alu_dword_count++] = S_SQ_ALU_WORD0_SRC0_SEL(mulhi_gpr_set->current_gpr) |
                                      S_SQ_ALU_WORD0_SRC0_CHAN(mulhi_gpr_set->current_component) |
                                      S_SQ_ALU_WORD0_SRC1_SEL(mulhi_divisor_sel) |
                                      S_SQ_ALU_WORD0_SRC1_CHAN(0);
         alu_out[alu_dword_count++] =
            S_SQ_ALU_WORD1_DST_GPR(mulhi_gpr_set->result_gpr) |
            S_SQ_ALU_WORD1_BANK_SWIZZLE(mulhi_gpr_set->current_gpr == 0 ? SQ_ALU_SCL_212
                                                                        : SQ_ALU_SCL_122) |
            S_SQ_ALU_WORD1_OP2_WRITE_MASK(1) |
            S_SQ_ALU_WORD1_OP2_ALU_INST(EG_V_SQ_ALU_WORD1_OP2_SQ_OP2_INST_MULHI_UINT);
         mulhi_gpr_set->current_gpr = mulhi_gpr_set->result_gpr;
         mulhi_gpr_set->current_component = 0;
         address_previous_write_trans_word1_offsets[mulhi_address_index] = alu_dword_count - 1;

         uint32_t const mulhi_index_bit = BITFIELD_BIT(mulhi_index);
         mulhis_ready &= ~mulhi_index_bit;
         if (mulhi_index < post_mulhi_chain_count) {
            post_mulhi_chains_ready |= mulhi_index_bit;
         }
      }

      if (alu_dword_count != group_start) {
         alu_out[alu_dword_count - 2] |= S_SQ_ALU_WORD0_LAST(1);

         /* Emit the literals (must be padded to qwords). */
         memcpy(alu_out + alu_dword_count, group_literals, sizeof(uint32_t) * group_literal_count);
         alu_dword_count += group_literal_count;
         if (group_literal_count & 1) {
            alu_out[alu_dword_count++] = 0;
         }

         uint8_t const group_qwords = (alu_dword_count - group_start) / 2;
         if (alu_current_clause_qwords + group_qwords > 0x80) {
            if (alu_current_clause_qwords != 0) {
               alu_clause_qwords_out[alu_clause_count++] = alu_current_clause_qwords;
               alu_current_clause_qwords = 0;
            }
            alu_current_clause_qwords += group_qwords;
         }
      }
   }

   if (alu_current_clause_qwords != 0) {
      alu_clause_qwords_out[alu_clause_count++] = alu_current_clause_qwords;
      alu_current_clause_qwords = 0;
   }

   *alu_qword_count_out = alu_dword_count / 2;
   *alu_clause_count_out = alu_clause_count;

   /* Emit the fetch instructions. */
   uint32_t fetch_dword_count = 0;
   for (uint32_t sorted_attribute_index = 0; sorted_attribute_index < sorted_attribute_count;
        ++sorted_attribute_index) {
      struct terakan_vertex_input_attribute const * const attribute =
         &sorted_attributes[sorted_attribute_index];
      uint8_t const binding_index = attribute->buffer_id;
      uint32_t const binding_bit = BITFIELD_BIT(binding_index);
      struct address const * const address = &addresses[binding_addresses[binding_index]];
      struct address_gpr_set const * const gpr_set =
         &address->gpr_sets[bindings_with_2048_stride_workaround & binding_bit
                               ? address_gpr_set_index_2048_stride
                               : address_gpr_set_index_generic_stride];
      fetch_out[fetch_dword_count++] =
         S_SQ_VTX_WORD0_VTX_INST(0) |
         S_SQ_VTX_WORD0_FETCH_TYPE(instance_bindings & binding_bit ? SQ_VTX_FETCH_INSTANCE_DATA
                                                                   : SQ_VTX_FETCH_NO_INDEX_OFFSET) |
         S_SQ_VTX_WORD0_BUFFER_ID(binding_index) | S_SQ_VTX_WORD0_SRC_GPR(gpr_set->current_gpr) |
         S_SQ_VTX_WORD0_SRC_SEL_X(gpr_set->current_component);
      fetch_out[fetch_dword_count++] = attribute->word1_dst_gpr_and_format;
      fetch_out[fetch_dword_count++] =
         S_SQ_VTX_WORD2_OFFSET(attribute->offset) |
         S_SQ_VTX_WORD2_ENDIAN_SWAP(
            UTIL_ARCH_BIG_ENDIAN ? terascale_format_big_endian_swap[G_SQ_VTX_WORD1_DATA_FORMAT(
                                      attribute->word1_dst_gpr_and_format)]
                                 : TERASCALE_ENDIAN_SWAP_NONE);
      fetch_out[fetch_dword_count++] = 0;
   }
   *fetch_count_out = fetch_dword_count / 4;

   /* Calculate mega-fetch sizes (including for the fetches that are specified to be mini-fetches,
    * as the processor may convert them into mega-fetches) by going in reverse, assuming that
    * fetches for the same binding are consecutive and are ordered by offset, and trying to include
    * the fetch plus the chain of the subsequent mini-fetches in its mega-fetch count.
    */
   if (!is_r9xx) {
      uint32_t const fetch_count = fetch_dword_count / 4;
      if (fetch_count != 0) {
         {
            uint32_t * const last_fetch = fetch_out + 4 * (fetch_count - 1);
            last_fetch[0] |= S_SQ_VTX_WORD0_MEGA_FETCH_COUNT(
               terascale_format_bytes_per_block[G_SQ_VTX_WORD1_DATA_FORMAT(last_fetch[1])] - 1);
            /* Make all fetches mega by default, converting to mini when coalescing with the
             * preceding mega-fetch.
             */
            last_fetch[2] |= S_SQ_VTX_WORD2_MEGA_FETCH(1);
         }
         for (uint32_t fetches_remaining = fetch_count - 1; fetches_remaining != 0;
              --fetches_remaining) {
            uint32_t * const current_fetch = fetch_out + 4 * (fetches_remaining - 1);
            uint32_t * const next_fetch = current_fetch + 4;
            unsigned char const fetch_bytes =
               terascale_format_bytes_per_block[G_SQ_VTX_WORD1_DATA_FORMAT(current_fetch[1])];
            current_fetch[0] |= S_SQ_VTX_WORD0_MEGA_FETCH_COUNT(fetch_bytes - 1);
            current_fetch[2] |= S_SQ_VTX_WORD2_MEGA_FETCH(1);
            if (G_SQ_VTX_WORD0_BUFFER_ID(current_fetch[0]) !=
                G_SQ_VTX_WORD0_BUFFER_ID(next_fetch[0])) {
               continue;
            }
            uint32_t const fetch_start_to_current_mega_fetch_extent =
               G_SQ_VTX_WORD2_OFFSET(next_fetch[2]) - G_SQ_VTX_WORD2_OFFSET(current_fetch[2]) +
               (G_SQ_VTX_WORD0_MEGA_FETCH_COUNT(next_fetch[0]) + 1);
            uint32_t const mega_fetch_count =
               MAX2(fetch_start_to_current_mega_fetch_extent, fetch_bytes);
            if (mega_fetch_count > 1 << 6) {
               continue;
            }
            current_fetch[0] = (current_fetch[0] & C_SQ_VTX_WORD0_MEGA_FETCH_COUNT) |
                               S_SQ_VTX_WORD0_MEGA_FETCH_COUNT(mega_fetch_count - 1);
            next_fetch[2] &= C_SQ_VTX_WORD2_MEGA_FETCH;
         }
      }
   }
}

void
terakan_vertex_input_create_fs_program(bool const is_r9xx, uint32_t const alu_qword_count,
                                       void const * const alu, uint32_t const alu_clause_count,
                                       uint8_t const * const alu_clause_qwords,
                                       uint32_t const fetch_count, void const * const fetch,
                                       void * const program_out)
{
   uint32_t * next_dword = (uint32_t *)program_out;

   uint32_t const first_alu_address = alu_clause_count + DIV_ROUND_UP(fetch_count, 0x40) + 1;

   uint32_t next_clause_address = first_alu_address;

   for (uint32_t alu_clause_index = 0; alu_clause_index < alu_clause_count; ++alu_clause_index) {
      uint8_t const current_alu_clause_qwords = alu_clause_qwords[alu_clause_index];
      assert(current_alu_clause_qwords != 0);
      *(next_dword++) = util_cpu_to_le32(S_SQ_CF_ALU_WORD0_ADDR(next_clause_address));
      *(next_dword++) =
         util_cpu_to_le32(S_SQ_CF_ALU_WORD1_COUNT(current_alu_clause_qwords - 1) |
                          S_SQ_CF_ALU_WORD1_BARRIER(1) | EG_V_SQ_CF_ALU_WORD1_SQ_CF_INST_ALU);
      next_clause_address += current_alu_clause_qwords;
   }
   assert(alu_qword_count == next_clause_address - first_alu_address);

   next_clause_address = ALIGN_POT(next_clause_address, 2);
   for (uint32_t fetch_index = 0; fetch_index < fetch_count; fetch_index += 0x40) {
      uint32_t const clause_fetch_count = MIN2(fetch_index + 0x40, fetch_count) - fetch_index;
      *(next_dword++) = util_cpu_to_le32(S_SQ_CF_WORD0_ADDR(next_clause_address));
      /* Always with the barrier flag because a fetch can overwrite a GPR with the index used for
       * previous fetches.
       */
      *(next_dword++) = util_cpu_to_le32(
         S_SQ_CF_WORD1_COUNT(clause_fetch_count - 1) | S_SQ_CF_WORD1_BARRIER(1) |
         (is_r9xx ? EG_V_SQ_CF_WORD1_SQ_CF_INST_TEX : EG_V_SQ_CF_WORD1_SQ_CF_INST_VTX));
      next_clause_address += 2 * clause_fetch_count;
   }

   *(next_dword++) = 0;
   *(next_dword++) =
      util_cpu_to_le32(S_SQ_CF_WORD1_BARRIER(1) | EG_V_SQ_CF_WORD1_SQ_CF_INST_RETURN);

   util_memcpy_cpu_to_le32(next_dword, alu, sizeof(uint32_t) * (2 * alu_qword_count));
   next_dword += 2 * alu_qword_count;

   if ((next_dword - (uint32_t *)program_out) & 1) {
      *(next_dword++) = 0;
   }
   util_memcpy_cpu_to_le32(next_dword, fetch, sizeof(uint32_t) * 4 * fetch_count);
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdSetVertexInputEXT(
   VkCommandBuffer const commandBuffer, uint32_t const vertexBindingDescriptionCount,
   VkVertexInputBindingDescription2EXT const * const pVertexBindingDescriptions,
   uint32_t const vertexAttributeDescriptionCount,
   VkVertexInputAttributeDescription2EXT const * const pVertexAttributeDescriptions)
{
   struct terakan_gfx_command_writer * const command_writer =
      terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx;
   struct terakan_state_draw * const state = &command_writer->state_draw;

   bool fetch_shader_creation_needed = false;

   uint32_t bindings_provided = 0b0;
   uint32_t bindings_with_2048_stride_workaround = 0b0;
   for (uint32_t binding_description_index = 0;
        binding_description_index < vertexBindingDescriptionCount; ++binding_description_index) {
      VkVertexInputBindingDescription2EXT const * const binding_description =
         &pVertexBindingDescriptions[binding_description_index];
      assert(binding_description->binding < TERAKAN_RESOURCE_HW_COUNT_FETCH);
      uint32_t const binding_bit = BITFIELD_BIT(binding_description->binding);
      bindings_provided |= binding_bit;
      struct terakan_state_draw_sq_resource_fs * const binding_resource =
         &state->sq_resources_fs[binding_description->binding];
      uint16_t const binding_stride = (uint16_t)binding_description->stride;
      if (binding_resource->stride != binding_stride && binding_resource->bo != NULL) {
         state->sq_resources_fs_pending |= binding_bit;
         terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_SQ_RESOURCES_FS);
      }
      binding_resource->stride = binding_stride;
      if (binding_stride >= 2048) {
         bindings_with_2048_stride_workaround |= binding_bit;
      }
   }

   uint32_t bindings_needed_by_attributes = 0b0;
   BITSET_DECLARE(attributes_provided, TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES);
   BITSET_ZERO(attributes_provided);
   for (uint32_t attribute_description_index = 0;
        attribute_description_index < vertexAttributeDescriptionCount;
        ++attribute_description_index) {
      VkVertexInputAttributeDescription2EXT const * const attribute_description =
         &pVertexAttributeDescriptions[attribute_description_index];

      struct terakan_vertex_input_attribute attribute;
      if (unlikely(!terakan_vertex_input_attribute_translate(
             attribute_description->location, attribute_description->binding,
             attribute_description->format, attribute_description->offset, &attribute))) {
         continue;
      }

      uint32_t const attribute_binding_bit = BITFIELD_BIT(attribute_description->binding);
      assert(bindings_provided & attribute_binding_bit);
      if (unlikely(!(bindings_provided & attribute_binding_bit))) {
         continue;
      }
      bindings_needed_by_attributes |= attribute_binding_bit;

      struct terakan_vertex_input_attribute * const state_attribute =
         &state->sq_pgm_fs.dynamic_state.attributes[attribute_description->location];

      if (!fetch_shader_creation_needed &&
          BITSET_TEST(
             state->sq_pgm_fs.dynamic_state.from_apply_sq_pgm_ls_es_gs_vs.attributes_needed_by_vs,
             attribute_description->location)) {
         fetch_shader_creation_needed |=
            state_attribute->word1_dst_gpr_and_format != attribute.word1_dst_gpr_and_format ||
            state_attribute->offset != attribute.offset ||
            state_attribute->buffer_id != attribute.buffer_id;
      }
      *state_attribute = attribute;

      BITSET_SET(attributes_provided, attribute_description->location);
   }

   uint32_t const bindings_needed_by_attributes_and_provided =
      bindings_needed_by_attributes & bindings_provided;

   struct terakan_device const * const device = terakan_gfx_command_writer_device(command_writer);
   bool const is_r9xx = terakan_device_physical_device(device)->chip_family_info.is_r9xx;

   if (!bindings_needed_by_attributes_and_provided) {
      /* Don't create an empty fetch shader, use the static one instead. */
      if (state->sq_pgm_fs.static_state != &device->empty_vertex_input) {
         state->sq_pgm_fs.static_state = &device->empty_vertex_input;
         terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_SQ_PGM_FS);
      }
      if (!is_r9xx) {
         state->sq_pgm_fs.bindings_with_2048_stride_workaround =
            bindings_with_2048_stride_workaround;
      }
      return;
   }

   fetch_shader_creation_needed |= state->sq_pgm_fs.static_state != NULL;
   state->sq_pgm_fs.static_state = NULL;

   if (!fetch_shader_creation_needed) {
      for (unsigned attribute_word_index = 0;
           attribute_word_index < BITSET_WORDS(TERAKAN_VERTEX_INPUT_MAX_ATTRIBUTES);
           ++attribute_word_index) {
         fetch_shader_creation_needed |=
            ((state->sq_pgm_fs.dynamic_state.attributes_provided[attribute_word_index] ^
              attributes_provided[attribute_word_index]) &
             state->sq_pgm_fs.dynamic_state.from_apply_sq_pgm_ls_es_gs_vs
                .attributes_needed_by_vs[attribute_word_index]) != 0;
      }
   }
   BITSET_COPY(state->sq_pgm_fs.dynamic_state.attributes_provided, attributes_provided);

   fetch_shader_creation_needed |=
      state->sq_pgm_fs.dynamic_state.bindings_needed_by_attributes_and_provided !=
      bindings_needed_by_attributes_and_provided;
   state->sq_pgm_fs.dynamic_state.bindings_needed_by_attributes_and_provided =
      bindings_needed_by_attributes_and_provided;

   uint32_t instance_bindings = 0b0;
   for (uint32_t binding_description_index = 0;
        binding_description_index < vertexBindingDescriptionCount; ++binding_description_index) {
      VkVertexInputBindingDescription2EXT const * const binding_description =
         &pVertexBindingDescriptions[binding_description_index];
      uint32_t const binding_bit = BITFIELD_BIT(binding_description->binding);
      if (!(bindings_needed_by_attributes_and_provided & binding_bit)) {
         continue;
      }
      if (binding_description->inputRate == VK_VERTEX_INPUT_RATE_INSTANCE) {
         instance_bindings |= binding_bit;
         fetch_shader_creation_needed |=
            state->sq_pgm_fs.dynamic_state.instance_binding_divisors[binding_description->binding] !=
            binding_description->divisor;
         state->sq_pgm_fs.dynamic_state.instance_binding_divisors[binding_description->binding] =
            binding_description->divisor;
      }
   }
   fetch_shader_creation_needed |=
      state->sq_pgm_fs.dynamic_state.instance_bindings != instance_bindings;
   state->sq_pgm_fs.dynamic_state.instance_bindings = instance_bindings;

   if (!is_r9xx) {
      fetch_shader_creation_needed |=
         (state->sq_pgm_fs.bindings_with_2048_stride_workaround &
          bindings_needed_by_attributes_and_provided) != bindings_with_2048_stride_workaround;
      state->sq_pgm_fs.bindings_with_2048_stride_workaround = bindings_with_2048_stride_workaround;
   }

   if (fetch_shader_creation_needed) {
      terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_SQ_PGM_FS);
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_CmdBindVertexBuffers2(VkCommandBuffer const commandBuffer, uint32_t const firstBinding,
                              uint32_t const bindingCount, VkBuffer const * const pBuffers,
                              VkDeviceSize const * const pOffsets,
                              VkDeviceSize const * const pSizes,
                              VkDeviceSize const * const pStrides)
{
   assert(firstBinding <= TERAKAN_RESOURCE_HW_COUNT_FETCH);
   assert(bindingCount <= TERAKAN_RESOURCE_HW_COUNT_FETCH - firstBinding);

   struct terakan_gfx_command_writer * const command_writer =
      terakan_command_buffer_from_handle(commandBuffer)->command_writer.gfx;
   struct terakan_state_draw * const state = &command_writer->state_draw;

   state->sq_resources_fs_pending |= BITFIELD_RANGE(firstBinding, bindingCount);
   terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_SQ_RESOURCES_FS);

   /* Update the buffers. */
   for (uint32_t buffer_index = 0; buffer_index < bindingCount; ++buffer_index) {
      struct terakan_state_draw_sq_resource_fs * const binding =
         &state->sq_resources_fs[firstBinding + buffer_index];
      binding->bo = NULL;
      struct terakan_buffer const * const buffer =
         terakan_buffer_from_handle(pBuffers[buffer_index]);
      if (buffer == NULL) {
         continue;
      }
      VkDeviceSize const offset = pOffsets[buffer_index];
      assert(offset <= buffer->vk.size);
      if (offset > buffer->vk.size) {
         continue;
      }
      VkDeviceSize size = pSizes != NULL ? pSizes[buffer_index] : VK_WHOLE_SIZE;
      assert(size == VK_WHOLE_SIZE || size <= buffer->vk.size - offset);
      size = MIN3(size, buffer->vk.size - offset, (VkDeviceSize)UINT32_MAX + 1);
      if (size == 0) {
         continue;
      }
      binding->bo = buffer->bo;
      binding->va = buffer->va + offset;
      binding->size_bytes_minus_1 = size - 1;
   }

   /* Update the strides regardless of whether the bindings are null so the new strides are
    * preserved if vkCmdBindVertexBuffers or vkCmdBindVertexBuffers2 is called without the strides
    * next time.
    */
   if (pStrides != NULL) {
      uint32_t bindings_with_2048_stride_workaround =
         state->sq_pgm_fs.bindings_with_2048_stride_workaround;
      for (uint32_t buffer_index = 0; buffer_index < bindingCount; ++buffer_index) {
         uint32_t const binding_index = firstBinding + buffer_index;
         uint32_t const binding_bit = BITFIELD_BIT(binding_index);
         uint16_t const stride = (uint16_t)pStrides[buffer_index];
         state->sq_resources_fs[binding_index].stride = stride;
         if (stride >= 2048) {
            bindings_with_2048_stride_workaround |= binding_bit;
         } else {
            bindings_with_2048_stride_workaround &= ~binding_bit;
         }
      }
      if (!terakan_gfx_command_writer_physical_device(command_writer)->chip_family_info.is_r9xx) {
         /* If whether the workaround needs to be applied to any currently needed bindings is
          * changed, update the fetch shader.
          */
         if ((state->sq_pgm_fs.bindings_with_2048_stride_workaround ^
              bindings_with_2048_stride_workaround) &
             (state->sq_pgm_fs.static_state != NULL
                 ? state->sq_pgm_fs.static_state->bindings_needed_by_attributes_and_provided
                 : state->sq_pgm_fs.dynamic_state.bindings_needed_by_attributes_and_provided)) {
            terakan_state_draw_set_pending(state, TERAKAN_STATE_DRAW_INDEX_SQ_PGM_FS);
         }
         state->sq_pgm_fs.bindings_with_2048_stride_workaround =
            bindings_with_2048_stride_workaround;
      }
   }
}
