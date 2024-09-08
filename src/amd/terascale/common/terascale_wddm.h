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

#ifndef TERASCALE_WDDM_H
#define TERASCALE_WDDM_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Private driver data accepted in D3DKMT calls by the Radeon Software kernel-mode driver. */

/* D3DDDI_ALLOCATIONINFO private driver data.
 *
 * Consists of the header, optionally subresource private data, and the unknown structure with
 * zeros.
 */

enum {
   TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_NONE = 0,
   TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_DEVICE_HOST_VISIBLE = 1,
   TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_DEVICE_NON_HOST_VISIBLE = 2,
   TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_HOST_WRITE_COMBINED = 3,
   TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_HOST_CACHED = 4,
};

/* Can't be used with TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_DEVICE_NON_HOST_VISIBLE among the
 * allocation's memory types.
 */
#define TERASCALE_WDDM_ALLOCATION_PRIVATE_DATA_MEMORY_TYPE_FLAG_HOST_VISIBLE ((uint32_t)1 << 29)

struct terascale_wddm_allocation_private_data_header {
   /* Total size of all structures. */
   uint32_t private_data_size_bytes;

   /* 0x1 with only the header and terascale_wddm_allocation_private_data_unknown_struct in
    * the end.
    * 0x1 | 0xC if subresource data is present.
    */
   uint32_t flags;

   uint32_t unknown_0x8_0x80;
   uint32_t unknown_0xC_0;

   /* The lower bits are OR of (1 << (each memory type - 1)), not including unused (0) memory types.
    * May include TERASCALE_WDDM_ALLOCATION_PRIVATE_DATA_MEMORY_TYPE_FLAG_HOST_VISIBLE.
    */
   uint32_t memory_type_flags;

   uint32_t unknown_0x14_0;

   uint32_t alignment_bytes;

   /* 0x5 for the basic header + unknown_struct.
    * Has additional bits if subresource data is present.
    */
   uint32_t struct_flags;

   uint8_t memory_type_priority[4];
   uint32_t preferred_memory_type;

   uint32_t unknown_0x28_0[0x10];

   /* Not aligned to the allocation alignment, although in Direct3D 11 may be padded to the
    * resource's size requirement (just like naturally in Vulkan).
    */
   uint32_t size_bytes;

   /* For basic header + unknown_struct private data, zeros, but contains some values if subresource
    * data is present.
    */
   uint32_t unknown_0x6C[0x11];

   uint32_t unknown_struct_offset_bytes;

   uint32_t unknown_0xB4_0[0x4];
};

static_assert(
   sizeof(struct terascale_wddm_allocation_private_data_header) == 0xC4,
   "The sizes of private data structures must match those expected by the kernel-mode driver.");

static inline void
terascale_wddm_allocation_private_data_header_clear(
   struct terascale_wddm_allocation_private_data_header *const header)
{
   memset(header, 0, sizeof(*header));
   header->flags = 0x1;
   header->unknown_0x8_0x80 = 0x80;
   header->struct_flags = 0x5;
}

static inline void
terascale_wddm_allocation_private_data_header_set_memory_types(
   struct terascale_wddm_allocation_private_data_header *const header,
   const uint8_t memory_type_priority[4], const bool host_visible)
{
   memcpy(header->memory_type_priority, memory_type_priority, sizeof(uint8_t) * 4);

   uint32_t used_memory_types = 0b0;
   for (size_t memory_type_priority_index = 0; memory_type_priority_index < 4;
        ++memory_type_priority_index) {
      const uint8_t memory_type = memory_type_priority[memory_type_priority_index];
      assert(!(host_visible &&
               memory_type == TERASCALE_WDDM_ALLOCATION_MEMORY_TYPE_DEVICE_NON_HOST_VISIBLE));
      used_memory_types |= (uint32_t)1 << memory_type;
   }
   /* Flags expected by the kernel-mode driver are for memory types minus 1 (not including the
    * unused types).
    */
   header->memory_type_flags =
      (used_memory_types >> 1) |
      (host_visible ? TERASCALE_WDDM_ALLOCATION_PRIVATE_DATA_MEMORY_TYPE_FLAG_HOST_VISIBLE : 0);

   header->preferred_memory_type = memory_type_priority[0];
}

struct terascale_wddm_allocation_private_data_unknown_struct {
   uint32_t struct_size_bytes;

   uint32_t unknown_0x4_0[0x44];
};

static inline void
terascale_wddm_allocation_private_data_unknown_struct_clear(
   struct terascale_wddm_allocation_private_data_unknown_struct *const unknown_struct)
{
   memset(unknown_struct, 0, sizeof(*unknown_struct));
   unknown_struct->struct_size_bytes = sizeof(*unknown_struct);
}

static_assert(
   sizeof(struct terascale_wddm_allocation_private_data_unknown_struct) == 0x114,
   "The sizes of private data structures must match those expected by the kernel-mode driver.");

/* D3DKMTCreateAllocation call private driver data. */

struct terascale_wddm_create_allocation_private_data {
   uint32_t unknown_0x0_0[0x3];
   uint32_t unknown_0xC_0x78;
   uint32_t unknown_0x10_0[0xC];
};

static_assert(
   sizeof(struct terascale_wddm_create_allocation_private_data) == 0x40,
   "The sizes of private data structures must match those expected by the kernel-mode driver.");

static inline void
terascale_wddm_create_allocation_private_data_clear(
   struct terascale_wddm_create_allocation_private_data *const private_data)
{
   memset(private_data, 0, sizeof(*private_data));
   private_data->unknown_0xC_0x78 = 0x78;
}

/* D3DKMTCreateContext private driver data. */

struct terascale_wddm_create_context_private_data {
   uint32_t private_data_size_bytes;
   uint32_t unknown_0x4_0x2002;
   uint32_t command_buffer_size_bytes;
   uint32_t allocation_list_size_elements;
   uint32_t patch_location_list_size_elements;
   uint32_t unknown_0x14_0[0xB];
};

static_assert(
   sizeof(struct terascale_wddm_create_context_private_data) == 0x40,
   "The sizes of private data structures must match those expected by the kernel-mode driver.");

static inline void
terascale_wddm_create_context_private_data_clear(
   struct terascale_wddm_create_context_private_data *const private_data)
{
   memset(private_data, 0, sizeof(*private_data));
   private_data->private_data_size_bytes = sizeof(*private_data),
   private_data->unknown_0x4_0x2002 = 0x2002;
}

/* D3DKMTRender patch locations.
 *
 * SlotId has the slot type left-shifted by 10, and in some cases, a slot-specific index in the
 * lower bits.
 *
 * PatchOffset is in bytes.
 * SplitOffset is always 0.
 *
 * For allocation-relative addresses in packets:
 * - [39:8] addresses need one patch location with DriverId 0.
 * - 40-bit addresses need two patch locations with different SlotId values, the first for [31:0]
 *   with DriverId 0x800, the second for [39:32] with DriverId 0.
 * - AllocationOffset is the entire dword PatchOffset points to, including any bits not belonging to
 *   the address itself.
 *
 * There are also other types of patch locations, among which:
 *
 * - Patch location list header for allocation 0 with hAllocation = 0, PatchOffset = 0 required in
 *   every submission (without it, STATUS_GRAPHICS_DRIVER_MISMATCH is returned).
 *   In Radeon Software 15.301.1901 for Direct3D 11 submissions:
 *   - SlotId (0x1D << 10) | 3, DriverId 0, AllocationOffset (1 << 25).
 *   - SlotId (0x8F << 10) | 1, DriverId 0, AllocationOffset 0.
 *
 * - Two optional pairs with PatchOffset = CommandLength at the end of the patch location list,
 *   possibly for signaling purposes:
 *   - For the last allocation in the list (unextended private data structure, size 0x1000 bytes,
 *     alignment 0x10 bytes, host-cached memory type only, also host-mapped without synchronization
 *     by the Direct3D 11 driver):
 *     - SlotId (0x4E << 10) | 0, DriverId 0, AllocationOffset being a value monotonically
 *       increasing with every submission referencing this specific allocation object starting
 *       from 2.
 *     - SlotId (0x4E << 10) | 1, DriverId 0, AllocationOffset 0, though not precisely known,
 *       possibly the upper 32 bits of the increasing value.
 *   - For the allocation 0 with hAllocation = 0:
 *     - SlotId (0xAB << 10) | 2, DriverId 0, AllocationOffset 0, though not precisely known,
 *       possibly the upper 32 bits of the handle.
 *     - SlotId (0xAB << 10) | 1, DriverId 0, AllocationOffset being some Windows handle.
 *
 * - SlotId (0x15 << 10), DriverId 0x300, AllocationOffset 0, for some allocation, with PatchOffset
 *   pointing to the beginning of a sequence consisting of:
 *   - Type-2 NOP.
 *   - Type-3 NOP containing various packets in its body.
 *   - Another type-2 NOP.
 *   Examples of the packets inside that type-3 NOP are predicated setting of GB_BACKEND_MAP to the
 *   default value for the device or to 0 inside some memory barrier packet sequences, as well some
 *   other register setting or even some drawing.
 *
 * In this header, packing SlotId and DriverId in one 64-bit value for usage simplicity, with SlotId
 * in the lower 32 bits, DriverId in the upper. The lower bits of SlotId can be specified via OR
 * with the whole IDs value.
 */

#define TERASCALE_WDDM_PATCH_IDS(slot_type, driver_id)                                             \
   (((slot_type) << 10) | ((uint64_t)(driver_id) << 32))
/* For 40-bit address patch locations, the lower bits of the slot ID must be the same for the low
 * and the high bits patch locations.
 */
#define TERASCALE_WDDM_PATCH_IDS_40_BIT_LO(slot_type) TERASCALE_WDDM_PATCH_IDS(slot_type, 0x800)
#define TERASCALE_WDDM_PATCH_IDS_40_BIT_HI(slot_type) TERASCALE_WDDM_PATCH_IDS(slot_type, 0)

/* GFX. */

/* The lower bits are the MRT index.
 * CB_COLOR#_FMASK for single-sample targets (equals to base) also uses the CB_COLOR#_BASE slot.
 */
#define TERASCALE_WDDM_PATCH_IDS_CB_COLOR_BASE TERASCALE_WDDM_PATCH_IDS(0x1, 0)
/* For single-sample targets (without FMASK), for which FMASK equals to BASE, the same IDs as for
 * BASE should be used instead of the FMASK ones.
 */
#define TERASCALE_WDDM_PATCH_IDS_CB_COLOR_FMASK TERASCALE_WDDM_PATCH_IDS(0x36, 0)
#define TERASCALE_WDDM_PATCH_IDS_CB_COLOR_CMASK TERASCALE_WDDM_PATCH_IDS(0x37, 0)

/* The lower bits are the UAV index. Same as for CB_COLOR#_BASE. */
#define TERASCALE_WDDM_PATCH_IDS_CB_IMMED_BASE TERASCALE_WDDM_PATCH_IDS(0x1, 0)

#define TERASCALE_WDDM_PATCH_IDS_DB_Z_STENCIL_BASE TERASCALE_WDDM_PATCH_IDS(0x3, 0)

/* The exact meaning of the lower bits is unknown, possibly an abstract resource index within the
 * stage (like the UAV index for resources corresponding to UAVs).
 */
#define TERASCALE_WDDM_PATCH_IDS_SQ_TEX_RESOURCE_BASE TERASCALE_WDDM_PATCH_IDS(0x5, 0)
/* Same for texture resource mips and FMask. */
#define TERASCALE_WDDM_PATCH_IDS_SQ_TEX_RESOURCE_MIP TERASCALE_WDDM_PATCH_IDS(0x20, 0)

#define TERASCALE_WDDM_PATCH_IDS_INDEX_BASE_LO TERASCALE_WDDM_PATCH_IDS_40_BIT_LO(0x12)
#define TERASCALE_WDDM_PATCH_IDS_INDEX_BASE_HI TERASCALE_WDDM_PATCH_IDS_40_BIT_HI(0x60)

#define TERASCALE_WDDM_PATCH_IDS_CP_DMA_SRC_LO TERASCALE_WDDM_PATCH_IDS_40_BIT_LO(0x13)
#define TERASCALE_WDDM_PATCH_IDS_CP_DMA_SRC_HI TERASCALE_WDDM_PATCH_IDS_40_BIT_HI(0x6E)
#define TERASCALE_WDDM_PATCH_IDS_CP_DMA_DST_LO TERASCALE_WDDM_PATCH_IDS_40_BIT_LO(0x14)
#define TERASCALE_WDDM_PATCH_IDS_CP_DMA_DST_HI TERASCALE_WDDM_PATCH_IDS_40_BIT_HI(0x6F)

#define TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_ES TERASCALE_WDDM_PATCH_IDS(0x21, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_GS TERASCALE_WDDM_PATCH_IDS(0x22, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_VS TERASCALE_WDDM_PATCH_IDS(0x23, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_PS TERASCALE_WDDM_PATCH_IDS(0x24, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_FS TERASCALE_WDDM_PATCH_IDS(0x25, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_LS TERASCALE_WDDM_PATCH_IDS(0x7C, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_HS TERASCALE_WDDM_PATCH_IDS(0x7D, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_PGM_START_CS TERASCALE_WDDM_PATCH_IDS(0x7E, 0)

/* Can be seen via a shader with a large dynamically indexed local array variable. */
#define TERASCALE_WDDM_PATCH_IDS_SQ_GSTMP_RING_BASE TERASCALE_WDDM_PATCH_IDS(0x26, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_PSTMP_RING_BASE TERASCALE_WDDM_PATCH_IDS(0x27, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_VSTMP_RING_BASE TERASCALE_WDDM_PATCH_IDS(0x28, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_ESTMP_RING_BASE TERASCALE_WDDM_PATCH_IDS(0x29, 0)
/* The SQ_LSTMP_RING_BASE patch constant slot is used for both LS and CS. */
#define TERASCALE_WDDM_PATCH_IDS_SQ_LSTMP_RING_BASE TERASCALE_WDDM_PATCH_IDS(0x7F, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_HSTMP_RING_BASE TERASCALE_WDDM_PATCH_IDS(0x80, 0)

/* The lower bits are the constant buffer index. */
#define TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_LS_VS TERASCALE_WDDM_PATCH_IDS(0x2A, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_GS    TERASCALE_WDDM_PATCH_IDS(0x2B, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_PS    TERASCALE_WDDM_PATCH_IDS(0x2C, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_HS    TERASCALE_WDDM_PATCH_IDS(0x82, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_ALU_CONST_CACHE_CS    TERASCALE_WDDM_PATCH_IDS(0x84, 0)

#define TERASCALE_WDDM_PATCH_IDS_SQ_ESGS_RING_BASE TERASCALE_WDDM_PATCH_IDS(0x2D, 0)
#define TERASCALE_WDDM_PATCH_IDS_SQ_GSVS_RING_BASE TERASCALE_WDDM_PATCH_IDS(0x2E, 0)

/* The exact meaning of the lower bits is unknown, possibly an abstract resource index within the
 * stage (such as the vertex buffer index in Direct3D 11 fetch shaders, the UAV index for resources
 * corresponding to UAVs).
 */
#define TERASCALE_WDDM_PATCH_IDS_SQ_VTX_CONSTANT_BASE_LO TERASCALE_WDDM_PATCH_IDS_40_BIT_LO(0x30)
#define TERASCALE_WDDM_PATCH_IDS_SQ_VTX_CONSTANT_BASE_HI TERASCALE_WDDM_PATCH_IDS_40_BIT_HI(0x62)

#define TERASCALE_WDDM_PATCH_IDS_DB_HTILE_DATA_BASE TERASCALE_WDDM_PATCH_IDS(0x33, 0)

/* Can be seen by ending a Direct3D 11 pipeline statistics query after drawing. */
#define TERASCALE_WDDM_PATCH_IDS_SURFACE_SYNC_COHER_BASE TERASCALE_WDDM_PATCH_IDS(0x34, 0)

/* Can be seen via Direct3D 11 queries. */
#define TERASCALE_WDDM_PATCH_IDS_EVENT_WRITE_LO     TERASCALE_WDDM_PATCH_IDS_40_BIT_LO(0x3E)
#define TERASCALE_WDDM_PATCH_IDS_EVENT_WRITE_HI     TERASCALE_WDDM_PATCH_IDS_40_BIT_HI(0x66)
#define TERASCALE_WDDM_PATCH_IDS_EVENT_WRITE_EOP_LO TERASCALE_WDDM_PATCH_IDS_40_BIT_LO(0x3F)
#define TERASCALE_WDDM_PATCH_IDS_EVENT_WRITE_EOP_HI TERASCALE_WDDM_PATCH_IDS_40_BIT_HI(0x67)

/* Same as for EVENT_WRITE. */
#define TERASCALE_WDDM_PATCH_IDS_SET_BASE_LO TERASCALE_WDDM_PATCH_IDS_40_BIT_LO(0x3E)
#define TERASCALE_WDDM_PATCH_IDS_SET_BASE_HI TERASCALE_WDDM_PATCH_IDS_40_BIT_HI(0x66)

#ifdef __cplusplus
}
#endif

#endif /* TERASCALE_WDDM_H */
