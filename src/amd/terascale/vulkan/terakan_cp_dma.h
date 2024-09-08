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

#ifndef TERAKAN_CP_DMA_H
#define TERAKAN_CP_DMA_H

#include "terakan_bo.h"
#include "terakan_command_buffer.h"

#include <vulkan/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* According to RadeonSI, on pre-Fiji GCN hardware - but thus also possibly on TeraScale too - the
 * following values need to be aligned in copying for optimal performance , otherwise the CP DMA
 * engine may significantly slow down not only for the copy operations where unaligned values are
 * involved, but also for copy operations done after it:
 * - If the source is memory (not GDS), the source address.
 * - The total amount of data copied across operations. If it ends up unaligned, a throwaway copy
 *   operation needs to be done to restore the alignment of the internal counter.
 */
#define TERAKAN_CP_DMA_COPY_OPTIMAL_ALIGNMENT ((VkDeviceSize)1 << 5)

void terakan_cp_dma_sync_cp_me(struct terakan_gfx_command_writer * const command_writer);

/* Commands are executed without CP_SYNC. When completion needs to be awaited in CP ME,
 * terakan_cp_dma_sync_cp_me must be called (if CP PFP needs the result, PFP_SYNC_ME should also be
 * done afterwards).
 */

void terakan_cp_dma_copy(struct terakan_gfx_command_writer * command_writer,
                         struct terakan_bo const * src_bo, uint64_t src_va,
                         enum terakan_bo_priority src_bo_priority, struct terakan_bo const * dst_bo,
                         uint64_t dst_va, enum terakan_bo_priority dst_bo_priority,
                         VkDeviceSize size);

#ifdef __cplusplus
}
#endif

#endif /* TERAKAN_CP_DMA_H */
