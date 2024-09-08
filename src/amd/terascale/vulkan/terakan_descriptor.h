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

#ifndef TERAKAN_DESCRIPTOR_H
#define TERAKAN_DESCRIPTOR_H

#include "terakan_bo.h"

#include "gallium/drivers/r600/eg_sq.h"
#include "gallium/drivers/r600/evergreend.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MRTs [0, TERAKAN_COLOR_HW_RTV_COUNT) support both color attachments and storage buffers/images.
 * MRTs [TERAKAN_COLOR_HW_RTV_COUNT, TERAKAN_COLOR_HW_RTV_AND_UAV_COUNT) support only storage
 * buffers/images.
 *
 * RTV - Render Target View in Direct3D terms.
 * UAV - Unordered Access View in Direct3D, also known as Random Access Target (RAT) on TeraScale.
 */
#define TERAKAN_COLOR_HW_RTV_COUNT         8
#define TERAKAN_COLOR_HW_RTV_AND_UAV_COUNT 12

/* Limit the UAV count in pixel shaders by maxFragmentCombinedOutputResources, which includes
 * "output Location decorated color attachments", and with dual-source blending, both sources
 * correspond to the same color attachment in Vulkan, but in the hardware, dual-source blending uses
 * two separate MRT indices and CB_COLOR1_INFO's SOURCE_FORMAT, so with dual-source blending, two
 * rather than one RTV/UAV bindings are occupied by the first attachment.
 */
#define TERAKAN_COLOR_UAV_COUNT_PIXEL (TERAKAN_COLOR_HW_RTV_AND_UAV_COUNT - 1)

/* Constant cache (kcache) hardware properties. */
#define TERAKAN_KCACHE_HW_LINE_BYTES_LOG2          8
#define TERAKAN_KCACHE_HW_LINE_BYTES               (1 << TERAKAN_KCACHE_HW_LINE_BYTES_LOG2)
#define TERAKAN_KCACHE_HW_MAX_LINES_IN_BUFFER_LOG2 8
#define TERAKAN_KCACHE_HW_MAX_LINES_IN_BUFFER      (1 << TERAKAN_KCACHE_HW_MAX_LINES_IN_BUFFER_LOG2)
#define TERAKAN_KCACHE_HW_MAX_BUFFER_SIZE_BYTES_LOG2                                               \
   (TERAKAN_KCACHE_HW_LINE_BYTES_LOG2 + TERAKAN_KCACHE_HW_MAX_LINES_IN_BUFFER_LOG2)
#define TERAKAN_KCACHE_HW_MAX_BUFFER_SIZE_BYTES (1 << TERAKAN_KCACHE_HW_MAX_BUFFER_SIZE_BYTES_LOG2)
#define TERAKAN_KCACHE_HW_BUFFERS_PER_STAGE     16
/* "Indexed locks of banks 14 and 15 are ignored" according to the KCACHE_BANK_INDEX_MODE#
 * documentation.
 */
#define TERAKAN_KCACHE_HW_RELATIVE_INDEXABLE_BUFFERS 14

/* Kcache allocation. */
/* The buffer with driver and application and push constants is never accessed with a relative
 * index, place it near the end.
 */
#define TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS (TERAKAN_KCACHE_HW_BUFFERS_PER_STAGE - 1)
/* Number of kcache buffers starting from 0 allocated for uniform buffers from application pipeline
 * layouts.
 */
#define TERAKAN_KCACHE_MAX_UNIFORM_BUFFERS TERAKAN_KCACHE_BUFFER_PUSH_CONSTANTS

/* For easier writing of meta shaders.
 * offsetof doesn't produce a constant on MSVC 2022, so using dword offsets instead.
 */
#define TERAKAN_KCACHE_DWORD_LINE(offset_dwords)                                                   \
   ((offset_dwords) / (TERAKAN_KCACHE_HW_LINE_BYTES / sizeof(uint32_t)))
#define TERAKAN_KCACHE_DWORD_VECTOR(offset_dwords)                                                 \
   (((offset_dwords) & (TERAKAN_KCACHE_HW_LINE_BYTES / sizeof(uint32_t) - 1)) / 4)
#define TERAKAN_KCACHE_DWORD_SOURCE(offset_dwords)                                                 \
   (0x80 + TERAKAN_KCACHE_DWORD_VECTOR(offset_dwords))
#define TERAKAN_KCACHE_DWORD_COMPONENT(offset_dwords) ((offset_dwords) & 3)
#define TERAKAN_KCACHE_DWORD_WORD0_SRC0(offset_dwords)                                             \
   (S_SQ_ALU_WORD0_SRC0_SEL(TERAKAN_KCACHE_DWORD_SOURCE(offset_dwords)) |                          \
    S_SQ_ALU_WORD0_SRC0_CHAN(TERAKAN_KCACHE_DWORD_COMPONENT(offset_dwords)))
#define TERAKAN_KCACHE_DWORD_WORD0_SRC1(offset_dwords)                                             \
   (S_SQ_ALU_WORD0_SRC1_SEL(TERAKAN_KCACHE_DWORD_SOURCE(offset_dwords)) |                          \
    S_SQ_ALU_WORD0_SRC1_CHAN(TERAKAN_KCACHE_DWORD_COMPONENT(offset_dwords)))
#define TERAKAN_KCACHE_DWORD_WORD1_SRC2(offset_dwords)                                             \
   (S_SQ_ALU_WORD1_OP3_SRC2_SEL(TERAKAN_KCACHE_DWORD_SOURCE(offset_dwords)) |                      \
    S_SQ_ALU_WORD1_OP3_SRC2_CHAN(TERAKAN_KCACHE_DWORD_COMPONENT(offset_dwords)))

#define TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE 176
#define TERAKAN_RESOURCE_HW_COUNT_VERTEX        160
#define TERAKAN_RESOURCE_HW_COUNT_FETCH         32

#define TERAKAN_RESOURCE_HW_OFFSET_PS 0
#define TERAKAN_RESOURCE_HW_OFFSET_VSES                                                            \
   (TERAKAN_RESOURCE_HW_OFFSET_PS + TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE)
#define TERAKAN_RESOURCE_HW_OFFSET_GS                                                              \
   (TERAKAN_RESOURCE_HW_OFFSET_VSES + TERAKAN_RESOURCE_HW_COUNT_VERTEX)
#define TERAKAN_RESOURCE_HW_OFFSET_HS                                                              \
   (TERAKAN_RESOURCE_HW_OFFSET_GS + TERAKAN_RESOURCE_HW_COUNT_VERTEX)
#define TERAKAN_RESOURCE_HW_OFFSET_LS                                                              \
   (TERAKAN_RESOURCE_HW_OFFSET_HS + TERAKAN_RESOURCE_HW_COUNT_VERTEX)
#define TERAKAN_RESOURCE_HW_OFFSET_CS                                                              \
   (TERAKAN_RESOURCE_HW_OFFSET_LS + TERAKAN_RESOURCE_HW_COUNT_VERTEX)
#define TERAKAN_RESOURCE_HW_OFFSET_FS                                                              \
   (TERAKAN_RESOURCE_HW_OFFSET_CS + TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE)
#define TERAKAN_RESOURCE_HW_COUNT (TERAKAN_RESOURCE_HW_OFFSET_FS + TERAKAN_RESOURCE_HW_COUNT_FETCH)

#define TERAKAN_SAMPLER_HW_COUNT_PER_STAGE 18

#define TERAKAN_SAMPLER_HW_OFFSET_PS   (TERAKAN_SAMPLER_HW_COUNT_PER_STAGE * 0)
#define TERAKAN_SAMPLER_HW_OFFSET_VSES (TERAKAN_SAMPLER_HW_COUNT_PER_STAGE * 1)
#define TERAKAN_SAMPLER_HW_OFFSET_GS   (TERAKAN_SAMPLER_HW_COUNT_PER_STAGE * 2)
#define TERAKAN_SAMPLER_HW_OFFSET_HS   (TERAKAN_SAMPLER_HW_COUNT_PER_STAGE * 3)
#define TERAKAN_SAMPLER_HW_OFFSET_LS   (TERAKAN_SAMPLER_HW_COUNT_PER_STAGE * 4)
#define TERAKAN_SAMPLER_HW_OFFSET_CS   (TERAKAN_SAMPLER_HW_COUNT_PER_STAGE * 5)

/* Dynamically indexable immediate constant arrays in application shader code.
 * Also used for a single resource binding for meta draws or dispatches as it can be quickly
 * invalidated alongside the shader itself.
 */
#define TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META 0
/* Dynamically indexable driver and application push constants. */
#define TERAKAN_RESOURCE_RANGE_PUSH_CONSTANTS                                                      \
   (TERAKAN_RESOURCE_RANGE_SHADER_CONSTANT_ARRAYS_OR_META + 1)
/* VS: Base vertex and instance.
 * GS and VS/TES ALT_CONST: Ring buffer.
 * CS: Number of workgroups.
 * Not needed in FS, so can be used for an additional input attachment.
 */
#define TERAKAN_RESOURCE_RANGE_NON_PIXEL_STAGE_SPECIFIC (TERAKAN_RESOURCE_HW_COUNT_VERTEX - 1)
/* Pixel and compute shaders provide additional 16 resource bindings beyond the 160 available in
 * vertex stages. Use them for resources specific to those stages. In fragment shaders, place IMMED
 * buffers of UAVs (11 with the limitations of maxFragmentCombinedOutputResources's interaction with
 * dual-source blending) there, and give the rest of that range to an extension of the mutable
 * resource type descriptor space for use as input attachments (at least 4 are mandatory in Vulkan).
 */
#define TERAKAN_RESOURCE_RANGE_UAV_IMMEDIATE_BASE_PIXEL                                            \
   (TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE - TERAKAN_COLOR_UAV_COUNT_PIXEL)
#define TERAKAN_RESOURCE_RANGE_UAV_IMMEDIATE_BASE_COMPUTE                                          \
   (TERAKAN_RESOURCE_HW_COUNT_PIXEL_COMPUTE - TERAKAN_COLOR_HW_RTV_AND_UAV_COUNT)
/* Resources from the application's pipeline layout:
 * - Sampled images, uniform texel buffers.
 * - Storage images, storage texel buffers, storage buffers - read-only when coherence with writable
 *   ones is not needed (vertex stages, or `restrict readonly` without `coherent`), as well as for
 *   UAV info queries.
 * - Uniform buffers - for dynamic indexing.
 * - Input attachments.
 */
#define TERAKAN_RESOURCE_RANGE_MUTABLE_BASE (TERAKAN_RESOURCE_RANGE_PUSH_CONSTANTS + 1)
#define TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL                                         \
   (TERAKAN_RESOURCE_RANGE_NON_PIXEL_STAGE_SPECIFIC - TERAKAN_RESOURCE_RANGE_MUTABLE_BASE)
#define TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL                                             \
   (TERAKAN_RESOURCE_RANGE_UAV_IMMEDIATE_BASE_PIXEL - TERAKAN_RESOURCE_RANGE_MUTABLE_BASE)
static_assert(
   TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL >=
      TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL,
   "The maximum number of mutable resource bindings per stage is expected to be bound by vertex "
   "stages, which have fewer hardware resource bindings.");
static_assert(
   TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL >= 15 + 4 + 128 + 8,
   "There should be enough non-pixel shader mutable resource bindings for the minimum Direct3D 11 "
   "binding counts plus Vulkan storage buffers.");
static_assert(
   TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL >= 15 + 4 + 128 + 8 + 4,
   "There should be enough pixel shader mutable resource bindings for the minimum Direct3D 11 "
   "binding counts plus Vulkan storage buffers and input attachments.");
#define TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_IN_PIPELINE                                       \
   (TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_NON_PIXEL * 5 +                                       \
    TERAKAN_RESOURCE_RANGE_MUTABLE_MAX_COUNT_PIXEL)

/* SQ_VTX_CONSTANT doesn't have words 5 and 6, so using word 5 for the BO priority. */
#define TERAKAN_RESOURCE_BUFFER_PRIORITY_WORD 5

/* Hardware CB_COLOR[0-11] registers.
 * Note that image views don't store RTV or UAV descriptors directly, instead they contain data for
 * both, but RTVs and UAVs each have fields they don't use, or require specific values for each
 * field.
 * Before setting CB_COLOR[0-11] to these descriptors, pass them through
 * terakan_color_descriptor_image_view_to_color_attachment or
 * terakan_color_descriptor_image_view_to_storage_image depending on the needed binding type.
 */
struct terakan_color_descriptor {
   uint32_t base;
   uint32_t pitch;
   uint32_t slice;
   /* Because according to Radeon Evergreen / Northern Islands Acceleration, buffer UAVs must use
    * the LINEAR_ALIGNED array mode (not LINEAR_GENERAL), for smaller alignments required by
    * Direct3D 11 (and even if disregarding Direct3D 11, by Vulkan itself as well - at most 256,
    * while the pipe interleave can potentially be 512 bytes), an offset needs to be added to
    * element indices in shaders. In buffer views within the driver, it's stored in SLICE_START in
    * elements (not in bytes unlike for LINEAR_GENERAL in the hardware), and must be zeroed before
    * being passed to the actual CB_COLOR registers.
    */
   uint32_t view;
   /* In image views, the INFO register is for a color attachment. */
   uint32_t info;
   uint32_t attrib;
   /* In image views, the DIM register is for a storage image. */
   uint32_t dim;
};

void terakan_color_descriptor_calculate_buffer_base_pitch_dim_offset(
   struct terakan_color_descriptor * descriptor, uint64_t va, VkDeviceSize elements,
   unsigned bytes_per_element, unsigned tile_pipe_interleave_bytes_log2,
   uint32_t * alignment_offset_elements_out);

static inline void
terakan_color_descriptor_calculate_buffer_base_pitch_view_dim(
   struct terakan_color_descriptor * const descriptor, uint64_t const va,
   VkDeviceSize const elements, unsigned const bytes_per_element,
   unsigned const tile_pipe_interleave_bytes_log2)
{
   uint32_t alignment_offset_elements;
   terakan_color_descriptor_calculate_buffer_base_pitch_dim_offset(
      descriptor, va, elements, bytes_per_element, tile_pipe_interleave_bytes_log2,
      &alignment_offset_elements);
   /* Used by the driver, must be zeroed before being passed to the hardware. */
   descriptor->view = S_028C6C_SLICE_START(alignment_offset_elements);
}

static inline void
terakan_color_descriptor_image_view_to_color_attachment(
   struct terakan_color_descriptor * const descriptor)
{
   descriptor->info &= C_028C70_RESOURCE_TYPE;
   /* The meaning of DIM depends on RESOURCE_TYPE, but it's used only for UAVs.
    * DIM is ignored for color attachments, scissor must be used to prevent out-of-bounds access.
    */
   descriptor->dim = 0;
}

static inline void
terakan_color_descriptor_image_view_to_storage_image(
   struct terakan_color_descriptor * const descriptor)
{
   descriptor->info &= C_028C70_FAST_CLEAR & C_028C70_COMPRESSION & C_028C70_BLEND_CLAMP &
                       C_028C70_SIMPLE_FLOAT & C_028C70_SOURCE_FORMAT;
   descriptor->info |=
      S_028C70_BLEND_BYPASS(1) | S_028C70_SOURCE_FORMAT(V_028C70_EXPORT_4C_32BPC) | S_028C70_RAT(1);
   descriptor->attrib &= C_028C74_FORCE_DST_ALPHA_1;
}

/* Additional hardware CB_COLOR[0-7] registers. */
struct terakan_color_meta_descriptor {
   uint32_t cmask;
   uint32_t cmask_slice;
   /* For single-sampled images, FMASK must be equal to BASE. */
   uint32_t fmask;
   uint32_t fmask_slice;
};

static inline struct terakan_color_meta_descriptor
terakan_color_meta_descriptor_create_disabled(struct terakan_color_descriptor const * const color)
{
   struct terakan_color_meta_descriptor descriptor;
   descriptor.cmask = color->base;
   descriptor.cmask_slice = S_028C80_TILE_MAX(0);
   descriptor.fmask = color->base;
   descriptor.fmask_slice = S_028C88_TILE_MAX(G_028C68_SLICE_TILE_MAX(color->slice));
   return descriptor;
}

struct terakan_depth_stencil_descriptor {
   uint32_t view;
   /* DB_Z_INFO contains some configuration shared between depth and stencil, even if the image
    * contains only stencil.
    */
   uint32_t z_info;
   uint32_t stencil_info;
   uint32_t z_base;
   uint32_t stencil_base;
   uint32_t size;
   uint32_t slice;
};

static inline bool
terakan_descriptor_type_has_resource(VkDescriptorType const descriptor_type)
{
   return descriptor_type != VK_DESCRIPTOR_TYPE_SAMPLER;
}

static inline bool
terakan_descriptor_type_has_sampler(VkDescriptorType const descriptor_type)
{
   return descriptor_type == VK_DESCRIPTOR_TYPE_SAMPLER ||
          descriptor_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
}

static inline bool
terakan_descriptor_type_has_uav(VkDescriptorType const descriptor_type)
{
   return descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
          descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER ||
          descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
          descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
}

bool terakan_descriptor_create_for_uniform_buffer(struct terakan_bo const * bo, uint64_t va,
                                                  VkDeviceSize range, uint32_t resource_out[8]);

bool terakan_descriptor_create_for_storage_buffer(struct terakan_bo const * bo, uint64_t va,
                                                  VkDeviceSize range,
                                                  unsigned tile_pipe_interleave_bytes_log2,
                                                  uint32_t resource_out[8],
                                                  struct terakan_color_descriptor * color_out);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_DESCRIPTOR_H */
