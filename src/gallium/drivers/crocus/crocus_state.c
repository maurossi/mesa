/*
 * Copyright © 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file crocus_state.c
 *
 * ============================= GENXML CODE =============================
 *              [This file is compiled once per generation.]
 * =======================================================================
 *
 * This is the main state upload code.
 *
 * Gallium uses Constant State Objects, or CSOs, for most state.  Large,
 * complex, or highly reusable state can be created once, and bound and
 * rebound multiple times.  This is modeled with the pipe->create_*_state()
 * and pipe->bind_*_state() hooks.  Highly dynamic or inexpensive state is
 * streamed out on the fly, via pipe->set_*_state() hooks.
 *
 * OpenGL involves frequently mutating context state, which is mirrored in
 * core Mesa by highly mutable data structures.  However, most applications
 * typically draw the same things over and over - from frame to frame, most
 * of the same objects are still visible and need to be redrawn.  So, rather
 * than inventing new state all the time, applications usually mutate to swap
 * between known states that we've seen before.
 *
 * Gallium isolates us from this mutation by tracking API state, and
 * distilling it into a set of Constant State Objects, or CSOs.  Large,
 * complex, or typically reusable state can be created once, then reused
 * multiple times.  Drivers can create and store their own associated data.
 * This create/bind model corresponds to the pipe->create_*_state() and
 * pipe->bind_*_state() driver hooks.
 *
 * Some state is cheap to create, or expected to be highly dynamic.  Rather
 * than creating and caching piles of CSOs for these, Gallium simply streams
 * them out, via the pipe->set_*_state() driver hooks.
 *
 * To reduce draw time overhead, we try to compute as much state at create
 * time as possible.  Wherever possible, we translate the Gallium pipe state
 * to 3DSTATE commands, and store those commands in the CSO.  At draw time,
 * we can simply memcpy them into a batch buffer.
 *
 * No hardware matches the abstraction perfectly, so some commands require
 * information from multiple CSOs.  In this case, we can store two copies
 * of the packet (one in each CSO), and simply | together their DWords at
 * draw time.  Sometimes the second set is trivial (one or two fields), so
 * we simply pack it at draw time.
 *
 * There are two main components in the file below.  First, the CSO hooks
 * create/bind/track state.  The second are the draw-time upload functions,
 * crocus_upload_render_state() and crocus_upload_compute_state(), which read
 * the context state and emit the commands into the actual batch.
 */

#include <stdio.h>
#include <errno.h>

#if HAVE_VALGRIND
#include <valgrind.h>
#include <memcheck.h>
#define VG(x) x
#ifdef DEBUG
#define __gen_validate_value(x) VALGRIND_CHECK_MEM_IS_DEFINED(&(x), sizeof(x))
#endif
#else
#define VG(x)
#endif

#include "pipe/p_defines.h"
#include "pipe/p_state.h"
#include "pipe/p_context.h"
#include "pipe/p_screen.h"
#include "util/u_dual_blend.h"
#include "util/u_inlines.h"
#include "util/format/u_format.h"
#include "util/u_framebuffer.h"
#include "util/u_transfer.h"
#include "util/u_upload_mgr.h"
#include "util/u_viewport.h"
#include "util/u_helpers.h"
#include "util/u_memory.h"
#include "util/u_prim.h"
#include "util/half_float.h"
#include "drm-uapi/i915_drm.h"
#include "nir.h"
#include "intel/compiler/brw_compiler.h"
#include "intel/common/intel_l3_config.h"
#include "intel/common/intel_sample_positions.h"
#include "crocus_batch.h"
#include "crocus_context.h"
#include "crocus_defines.h"
#include "crocus_pipe.h"
#include "crocus_resource.h"

#include "crocus_genx_macros.h"
#include "intel/common/intel_guardband.h"

static uint32_t
mocs(const struct crocus_bo *bo, const struct isl_device *dev)
{
   return bo && bo->external ? dev->mocs.external : dev->mocs.internal;
}

/**
 * Statically assert that PIPE_* enums match the hardware packets.
 * (As long as they match, we don't need to translate them.)
 */
UNUSED static void pipe_asserts()
{
#define PIPE_ASSERT(x) STATIC_ASSERT((int)x)

   /* pipe_logicop happens to match the hardware. */
   PIPE_ASSERT(PIPE_LOGICOP_CLEAR == LOGICOP_CLEAR);
   PIPE_ASSERT(PIPE_LOGICOP_NOR == LOGICOP_NOR);
   PIPE_ASSERT(PIPE_LOGICOP_AND_INVERTED == LOGICOP_AND_INVERTED);
   PIPE_ASSERT(PIPE_LOGICOP_COPY_INVERTED == LOGICOP_COPY_INVERTED);
   PIPE_ASSERT(PIPE_LOGICOP_AND_REVERSE == LOGICOP_AND_REVERSE);
   PIPE_ASSERT(PIPE_LOGICOP_INVERT == LOGICOP_INVERT);
   PIPE_ASSERT(PIPE_LOGICOP_XOR == LOGICOP_XOR);
   PIPE_ASSERT(PIPE_LOGICOP_NAND == LOGICOP_NAND);
   PIPE_ASSERT(PIPE_LOGICOP_AND == LOGICOP_AND);
   PIPE_ASSERT(PIPE_LOGICOP_EQUIV == LOGICOP_EQUIV);
   PIPE_ASSERT(PIPE_LOGICOP_NOOP == LOGICOP_NOOP);
   PIPE_ASSERT(PIPE_LOGICOP_OR_INVERTED == LOGICOP_OR_INVERTED);
   PIPE_ASSERT(PIPE_LOGICOP_COPY == LOGICOP_COPY);
   PIPE_ASSERT(PIPE_LOGICOP_OR_REVERSE == LOGICOP_OR_REVERSE);
   PIPE_ASSERT(PIPE_LOGICOP_OR == LOGICOP_OR);
   PIPE_ASSERT(PIPE_LOGICOP_SET == LOGICOP_SET);

   /* pipe_blend_func happens to match the hardware. */
   PIPE_ASSERT(PIPE_BLENDFACTOR_ONE == BLENDFACTOR_ONE);
   PIPE_ASSERT(PIPE_BLENDFACTOR_SRC_COLOR == BLENDFACTOR_SRC_COLOR);
   PIPE_ASSERT(PIPE_BLENDFACTOR_SRC_ALPHA == BLENDFACTOR_SRC_ALPHA);
   PIPE_ASSERT(PIPE_BLENDFACTOR_DST_ALPHA == BLENDFACTOR_DST_ALPHA);
   PIPE_ASSERT(PIPE_BLENDFACTOR_DST_COLOR == BLENDFACTOR_DST_COLOR);
   PIPE_ASSERT(PIPE_BLENDFACTOR_SRC_ALPHA_SATURATE == BLENDFACTOR_SRC_ALPHA_SATURATE);
   PIPE_ASSERT(PIPE_BLENDFACTOR_CONST_COLOR == BLENDFACTOR_CONST_COLOR);
   PIPE_ASSERT(PIPE_BLENDFACTOR_CONST_ALPHA == BLENDFACTOR_CONST_ALPHA);
   PIPE_ASSERT(PIPE_BLENDFACTOR_SRC1_COLOR == BLENDFACTOR_SRC1_COLOR);
   PIPE_ASSERT(PIPE_BLENDFACTOR_SRC1_ALPHA == BLENDFACTOR_SRC1_ALPHA);
   PIPE_ASSERT(PIPE_BLENDFACTOR_ZERO == BLENDFACTOR_ZERO);
   PIPE_ASSERT(PIPE_BLENDFACTOR_INV_SRC_COLOR == BLENDFACTOR_INV_SRC_COLOR);
   PIPE_ASSERT(PIPE_BLENDFACTOR_INV_SRC_ALPHA == BLENDFACTOR_INV_SRC_ALPHA);
   PIPE_ASSERT(PIPE_BLENDFACTOR_INV_DST_ALPHA == BLENDFACTOR_INV_DST_ALPHA);
   PIPE_ASSERT(PIPE_BLENDFACTOR_INV_DST_COLOR == BLENDFACTOR_INV_DST_COLOR);
   PIPE_ASSERT(PIPE_BLENDFACTOR_INV_CONST_COLOR == BLENDFACTOR_INV_CONST_COLOR);
   PIPE_ASSERT(PIPE_BLENDFACTOR_INV_CONST_ALPHA == BLENDFACTOR_INV_CONST_ALPHA);
   PIPE_ASSERT(PIPE_BLENDFACTOR_INV_SRC1_COLOR == BLENDFACTOR_INV_SRC1_COLOR);
   PIPE_ASSERT(PIPE_BLENDFACTOR_INV_SRC1_ALPHA == BLENDFACTOR_INV_SRC1_ALPHA);

   /* pipe_blend_func happens to match the hardware. */
   PIPE_ASSERT(PIPE_BLEND_ADD == BLENDFUNCTION_ADD);
   PIPE_ASSERT(PIPE_BLEND_SUBTRACT == BLENDFUNCTION_SUBTRACT);
   PIPE_ASSERT(PIPE_BLEND_REVERSE_SUBTRACT == BLENDFUNCTION_REVERSE_SUBTRACT);
   PIPE_ASSERT(PIPE_BLEND_MIN == BLENDFUNCTION_MIN);
   PIPE_ASSERT(PIPE_BLEND_MAX == BLENDFUNCTION_MAX);

   /* pipe_stencil_op happens to match the hardware. */
   PIPE_ASSERT(PIPE_STENCIL_OP_KEEP == STENCILOP_KEEP);
   PIPE_ASSERT(PIPE_STENCIL_OP_ZERO == STENCILOP_ZERO);
   PIPE_ASSERT(PIPE_STENCIL_OP_REPLACE == STENCILOP_REPLACE);
   PIPE_ASSERT(PIPE_STENCIL_OP_INCR == STENCILOP_INCRSAT);
   PIPE_ASSERT(PIPE_STENCIL_OP_DECR == STENCILOP_DECRSAT);
   PIPE_ASSERT(PIPE_STENCIL_OP_INCR_WRAP == STENCILOP_INCR);
   PIPE_ASSERT(PIPE_STENCIL_OP_DECR_WRAP == STENCILOP_DECR);
   PIPE_ASSERT(PIPE_STENCIL_OP_INVERT == STENCILOP_INVERT);

#if GEN_GEN >= 6
   /* pipe_sprite_coord_mode happens to match 3DSTATE_SBE */
   PIPE_ASSERT(PIPE_SPRITE_COORD_UPPER_LEFT == UPPERLEFT);
   PIPE_ASSERT(PIPE_SPRITE_COORD_LOWER_LEFT == LOWERLEFT);
#endif
#undef PIPE_ASSERT
}

static unsigned
translate_prim_type(enum pipe_prim_type prim, uint8_t verts_per_patch)
{
   static const unsigned map[] = {
      [PIPE_PRIM_POINTS]                   = _3DPRIM_POINTLIST,
      [PIPE_PRIM_LINES]                    = _3DPRIM_LINELIST,
      [PIPE_PRIM_LINE_LOOP]                = _3DPRIM_LINELOOP,
      [PIPE_PRIM_LINE_STRIP]               = _3DPRIM_LINESTRIP,
      [PIPE_PRIM_TRIANGLES]                = _3DPRIM_TRILIST,
      [PIPE_PRIM_TRIANGLE_STRIP]           = _3DPRIM_TRISTRIP,
      [PIPE_PRIM_TRIANGLE_FAN]             = _3DPRIM_TRIFAN,
      [PIPE_PRIM_QUADS]                    = _3DPRIM_QUADLIST,
      [PIPE_PRIM_QUAD_STRIP]               = _3DPRIM_QUADSTRIP,
      [PIPE_PRIM_POLYGON]                  = _3DPRIM_POLYGON,
#if GEN_GEN >= 6
      [PIPE_PRIM_LINES_ADJACENCY]          = _3DPRIM_LINELIST_ADJ,
      [PIPE_PRIM_LINE_STRIP_ADJACENCY]     = _3DPRIM_LINESTRIP_ADJ,
      [PIPE_PRIM_TRIANGLES_ADJACENCY]      = _3DPRIM_TRILIST_ADJ,
      [PIPE_PRIM_TRIANGLE_STRIP_ADJACENCY] = _3DPRIM_TRISTRIP_ADJ,
#endif
#if GEN_GEN >= 7
      [PIPE_PRIM_PATCHES]                  = _3DPRIM_PATCHLIST_1 - 1,
#endif
   };

   return map[prim] + (prim == PIPE_PRIM_PATCHES ? verts_per_patch : 0);
}

static unsigned
translate_compare_func(enum pipe_compare_func pipe_func)
{
   static const unsigned map[] = {
      [PIPE_FUNC_NEVER]    = COMPAREFUNCTION_NEVER,
      [PIPE_FUNC_LESS]     = COMPAREFUNCTION_LESS,
      [PIPE_FUNC_EQUAL]    = COMPAREFUNCTION_EQUAL,
      [PIPE_FUNC_LEQUAL]   = COMPAREFUNCTION_LEQUAL,
      [PIPE_FUNC_GREATER]  = COMPAREFUNCTION_GREATER,
      [PIPE_FUNC_NOTEQUAL] = COMPAREFUNCTION_NOTEQUAL,
      [PIPE_FUNC_GEQUAL]   = COMPAREFUNCTION_GEQUAL,
      [PIPE_FUNC_ALWAYS]   = COMPAREFUNCTION_ALWAYS,
   };
   return map[pipe_func];
}

static unsigned
translate_shadow_func(enum pipe_compare_func pipe_func)
{
   /* Gallium specifies the result of shadow comparisons as:
    *
    *    1 if ref <op> texel,
    *    0 otherwise.
    *
    * The hardware does:
    *
    *    0 if texel <op> ref,
    *    1 otherwise.
    *
    * So we need to flip the operator and also negate.
    */
   static const unsigned map[] = {
      [PIPE_FUNC_NEVER]    = PREFILTEROPALWAYS,
      [PIPE_FUNC_LESS]     = PREFILTEROPLEQUAL,
      [PIPE_FUNC_EQUAL]    = PREFILTEROPNOTEQUAL,
      [PIPE_FUNC_LEQUAL]   = PREFILTEROPLESS,
      [PIPE_FUNC_GREATER]  = PREFILTEROPGEQUAL,
      [PIPE_FUNC_NOTEQUAL] = PREFILTEROPEQUAL,
      [PIPE_FUNC_GEQUAL]   = PREFILTEROPGREATER,
      [PIPE_FUNC_ALWAYS]   = PREFILTEROPNEVER,
   };
   return map[pipe_func];
}

static unsigned
translate_cull_mode(unsigned pipe_face)
{
   static const unsigned map[4] = {
      [PIPE_FACE_NONE]           = CULLMODE_NONE,
      [PIPE_FACE_FRONT]          = CULLMODE_FRONT,
      [PIPE_FACE_BACK]           = CULLMODE_BACK,
      [PIPE_FACE_FRONT_AND_BACK] = CULLMODE_BOTH,
   };
   return map[pipe_face];
}

#if GEN_GEN >= 6
static unsigned
translate_fill_mode(unsigned pipe_polymode)
{
   static const unsigned map[4] = {
      [PIPE_POLYGON_MODE_FILL]           = FILL_MODE_SOLID,
      [PIPE_POLYGON_MODE_LINE]           = FILL_MODE_WIREFRAME,
      [PIPE_POLYGON_MODE_POINT]          = FILL_MODE_POINT,
      [PIPE_POLYGON_MODE_FILL_RECTANGLE] = FILL_MODE_SOLID,
   };
   return map[pipe_polymode];
}
#endif

static unsigned
translate_mip_filter(enum pipe_tex_mipfilter pipe_mip)
{
   static const unsigned map[] = {
      [PIPE_TEX_MIPFILTER_NEAREST] = MIPFILTER_NEAREST,
      [PIPE_TEX_MIPFILTER_LINEAR]  = MIPFILTER_LINEAR,
      [PIPE_TEX_MIPFILTER_NONE]    = MIPFILTER_NONE,
   };
   return map[pipe_mip];
}

static uint32_t
translate_wrap(unsigned pipe_wrap, bool either_nearest)
{
   static const unsigned map[] = {
      [PIPE_TEX_WRAP_REPEAT]                 = TCM_WRAP,
      [PIPE_TEX_WRAP_CLAMP]                  = TCM_CLAMP_BORDER,
      [PIPE_TEX_WRAP_CLAMP_TO_EDGE]          = TCM_CLAMP,
      [PIPE_TEX_WRAP_CLAMP_TO_BORDER]        = TCM_CLAMP_BORDER,
      [PIPE_TEX_WRAP_MIRROR_REPEAT]          = TCM_MIRROR,
      [PIPE_TEX_WRAP_MIRROR_CLAMP_TO_EDGE]   = TCM_MIRROR_ONCE,

      /* These are unsupported. */
      [PIPE_TEX_WRAP_MIRROR_CLAMP]           = -1,
      [PIPE_TEX_WRAP_MIRROR_CLAMP_TO_BORDER] = -1,
   };
   if (pipe_wrap == PIPE_TEX_WRAP_CLAMP && either_nearest)
      return TCM_CLAMP;
   return map[pipe_wrap];
}

static void *
upload_state(struct u_upload_mgr *uploader,
             struct crocus_state_ref *ref,
             unsigned size,
             unsigned alignment)
{
  //   assert(0);
  return NULL;
}

/**
 * Equiv if brw_state_batch
 */
static uint32_t *
stream_state(struct crocus_batch *batch,
             unsigned size,
             unsigned alignment,
             uint32_t *out_offset)
{
   uint32_t offset = ALIGN(batch->state.used, alignment);

   if (offset + size >= BATCH_SZ && !batch->no_wrap) {
      crocus_batch_flush(batch);
      offset = ALIGN(batch->state.used, alignment);
   } else if (offset + size >= batch->state.bo->size) {
      const unsigned new_size =
         MIN2(batch->state.bo->size + batch->state.bo->size / 2,
              MAX_STATE_SIZE);
      crocus_grow_buffer(batch, true, new_size);
      assert(offset + size < batch->state.bo->size);
   }

   crocus_record_state_size(batch->state_sizes, offset, size);

   batch->state.used = offset + size;
   *out_offset = offset;

   return (uint32_t *)batch->state.map + (offset >> 2);
}

/**
 * stream_state() + memcpy.
 */
static uint32_t
emit_state(struct crocus_batch *batch,
           const void *data,
           unsigned size,
           unsigned alignment)
{
   unsigned offset = 0;
   uint32_t *map =
      stream_state(batch, size, alignment, &offset);

   if (map)
      memcpy(map, data, size);

   return offset;
}

#if GEN_GEN <= 5
static void
upload_pipelined_state_pointers(struct crocus_batch *batch,
                                uint32_t vs_offset, uint32_t sf_offset,
                                uint32_t clip_offset, uint32_t wm_offset, uint32_t cc_offset)
{
#if GEN_GEN == 5
   /* Need to flush before changing clip max threads for errata. */
   crocus_emit_cmd(batch, GENX(MI_FLUSH), foo);
#endif

   crocus_emit_cmd(batch, GENX(3DSTATE_PIPELINED_POINTERS), pp) {
      pp.PointertoVSState = ro_bo(batch->state.bo, vs_offset);
      pp.GSEnable = false;
      pp.ClipEnable = true;
      pp.PointertoCLIPState = ro_bo(batch->state.bo, clip_offset);
      pp.PointertoSFState = ro_bo(batch->state.bo, sf_offset);
      pp.PointertoWMState = ro_bo(batch->state.bo, wm_offset);
      pp.PointertoColorCalcState = ro_bo(batch->state.bo, cc_offset);
   }
}

#endif
/**
 * Did field 'x' change between 'old_cso' and 'new_cso'?
 *
 * (If so, we may want to set some dirty flags.)
 */
#define cso_changed(x) (!old_cso || (old_cso->x != new_cso->x))
#define cso_changed_memcmp(x) \
   (!old_cso || memcmp(old_cso->x, new_cso->x, sizeof(old_cso->x)) != 0)

static void
flush_before_state_base_change(struct crocus_batch *batch)
{
#if GEN_GEN >= 6
   /* Flush before emitting STATE_BASE_ADDRESS.
    *
    * This isn't documented anywhere in the PRM.  However, it seems to be
    * necessary prior to changing the surface state base adress.  We've
    * seen issues in Vulkan where we get GPU hangs when using multi-level
    * command buffers which clear depth, reset state base address, and then
    * go render stuff.
    *
    * Normally, in GL, we would trust the kernel to do sufficient stalls
    * and flushes prior to executing our batch.  However, it doesn't seem
    * as if the kernel's flushing is always sufficient and we don't want to
    * rely on it.
    *
    * We make this an end-of-pipe sync instead of a normal flush because we
    * do not know the current status of the GPU.  On Haswell at least,
    * having a fast-clear operation in flight at the same time as a normal
    * rendering operation can cause hangs.  Since the kernel's flushing is
    * insufficient, we need to ensure that any rendering operations from
    * other processes are definitely complete before we try to do our own
    * rendering.  It's a bit of a big hammer but it appears to work.
    */
   crocus_emit_end_of_pipe_sync(batch,
                              "change STATE_BASE_ADDRESS (flushes)",
                              PIPE_CONTROL_RENDER_TARGET_FLUSH |
                              PIPE_CONTROL_DEPTH_CACHE_FLUSH |
                              PIPE_CONTROL_DATA_CACHE_FLUSH);
#endif
}

static void
flush_after_state_base_change(struct crocus_batch *batch)
{
   /* After re-setting the surface state base address, we have to do some
    * cache flusing so that the sampler engine will pick up the new
    * SURFACE_STATE objects and binding tables. From the Broadwell PRM,
    * Shared Function > 3D Sampler > State > State Caching (page 96):
    *
    *    Coherency with system memory in the state cache, like the texture
    *    cache is handled partially by software. It is expected that the
    *    command stream or shader will issue Cache Flush operation or
    *    Cache_Flush sampler message to ensure that the L1 cache remains
    *    coherent with system memory.
    *
    *    [...]
    *
    *    Whenever the value of the Dynamic_State_Base_Addr,
    *    Surface_State_Base_Addr are altered, the L1 state cache must be
    *    invalidated to ensure the new surface or sampler state is fetched
    *    from system memory.
    *
    * The PIPE_CONTROL command has a "State Cache Invalidation Enable" bit
    * which, according the PIPE_CONTROL instruction documentation in the
    * Broadwell PRM:
    *
    *    Setting this bit is independent of any other bit in this packet.
    *    This bit controls the invalidation of the L1 and L2 state caches
    *    at the top of the pipe i.e. at the parsing time.
    *
    * Unfortunately, experimentation seems to indicate that state cache
    * invalidation through a PIPE_CONTROL does nothing whatsoever in
    * regards to surface state and binding tables.  In stead, it seems that
    * invalidating the texture cache is what is actually needed.
    *
    * XXX:  As far as we have been able to determine through
    * experimentation, shows that flush the texture cache appears to be
    * sufficient.  The theory here is that all of the sampling/rendering
    * units cache the binding table in the texture cache.  However, we have
    * yet to be able to actually confirm this.
    */
#if GEN_GEN >= 6   
   crocus_emit_end_of_pipe_sync(batch,
                              "change STATE_BASE_ADDRESS (invalidates)",
                              PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE |
                              PIPE_CONTROL_CONST_CACHE_INVALIDATE |
                              PIPE_CONTROL_STATE_CACHE_INVALIDATE);
#endif
}

static void
_crocus_emit_lri(struct crocus_batch *batch, uint32_t reg, uint32_t val)
{
   crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_IMM), lri) {
      lri.RegisterOffset = reg;
      lri.DataDWord      = val;
   }
}
#define crocus_emit_lri(b, r, v) _crocus_emit_lri(b, GENX(r##_num), v)

static void
_crocus_emit_lrr(struct crocus_batch *batch, uint32_t dst, uint32_t src)
{
#if GEN_VERSIONx10 == 75
   crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_REG), lrr) {
      lrr.SourceRegisterAddress = src;
      lrr.DestinationRegisterAddress = dst;
   }
#else
   unreachable("unsupported");
#endif
}

static void
crocus_load_register_reg32(struct crocus_batch *batch, uint32_t dst,
                         uint32_t src)
{
   _crocus_emit_lrr(batch, dst, src);
}

static void
crocus_load_register_reg64(struct crocus_batch *batch, uint32_t dst,
                         uint32_t src)
{
   _crocus_emit_lrr(batch, dst, src);
   _crocus_emit_lrr(batch, dst + 4, src + 4);
}

static void
crocus_load_register_imm32(struct crocus_batch *batch, uint32_t reg,
                         uint32_t val)
{
   _crocus_emit_lri(batch, reg, val);
}

static void
crocus_load_register_imm64(struct crocus_batch *batch, uint32_t reg,
                         uint64_t val)
{
   _crocus_emit_lri(batch, reg + 0, val & 0xffffffff);
   _crocus_emit_lri(batch, reg + 4, val >> 32);
}

/**
 * Emit MI_LOAD_REGISTER_MEM to load a 32-bit MMIO register from a buffer.
 */
static void
crocus_load_register_mem32(struct crocus_batch *batch, uint32_t reg,
                         struct crocus_bo *bo, uint32_t offset)
{
#if GEN_GEN >= 7
   crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_MEM), lrm) {
      lrm.RegisterAddress = reg;
      lrm.MemoryAddress = ro_bo(bo, offset);
   }
#else
   unreachable("unsupported");
#endif
}

/**
 * Load a 64-bit value from a buffer into a MMIO register via
 * two MI_LOAD_REGISTER_MEM commands.
 */
static void
crocus_load_register_mem64(struct crocus_batch *batch, uint32_t reg,
                         struct crocus_bo *bo, uint32_t offset)
{
   crocus_load_register_mem32(batch, reg + 0, bo, offset + 0);
   crocus_load_register_mem32(batch, reg + 4, bo, offset + 4);
}

static void
crocus_store_register_mem32(struct crocus_batch *batch, uint32_t reg,
                          struct crocus_bo *bo, uint32_t offset,
                          bool predicated)
{
   crocus_emit_cmd(batch, GENX(MI_STORE_REGISTER_MEM), srm) {
      srm.RegisterAddress = reg;
      srm.MemoryAddress = rw_bo(bo, offset);
#if GEN_VERSIONx10 == 75
      srm.PredicateEnable = predicated;
#else
      if (predicated)
         unreachable("unsupported predication");
#endif
   }
}

static void
crocus_store_register_mem64(struct crocus_batch *batch, uint32_t reg,
                          struct crocus_bo *bo, uint32_t offset,
                          bool predicated)
{
   crocus_store_register_mem32(batch, reg + 0, bo, offset + 0, predicated);
   crocus_store_register_mem32(batch, reg + 4, bo, offset + 4, predicated);
}

static void
crocus_store_data_imm32(struct crocus_batch *batch,
                      struct crocus_bo *bo, uint32_t offset,
                      uint32_t imm)
{
   crocus_emit_cmd(batch, GENX(MI_STORE_DATA_IMM), sdi) {
      sdi.Address = rw_bo(bo, offset);
#if GEN_GEN >= 6
      sdi.ImmediateData = imm;
#endif
   }
}

static void
crocus_store_data_imm64(struct crocus_batch *batch,
                      struct crocus_bo *bo, uint32_t offset,
                      uint64_t imm)
{
   /* Can't use crocus_emit_cmd because MI_STORE_DATA_IMM has a length of
    * 2 in genxml but it's actually variable length and we need 5 DWords.
    */
   void *map = crocus_get_command_space(batch, 4 * 5);
   _crocus_pack_command(batch, GENX(MI_STORE_DATA_IMM), map, sdi) {
      sdi.DWordLength = 5 - 2;
      sdi.Address = rw_bo(bo, offset);
#if GEN_GEN >= 6
      sdi.ImmediateData = imm;
#endif
   }
}

static void
crocus_copy_mem_mem(struct crocus_batch *batch,
                  struct crocus_bo *dst_bo, uint32_t dst_offset,
                  struct crocus_bo *src_bo, uint32_t src_offset,
                  unsigned bytes)
{
//#if GEN_VERSIONx10 == 75
// support should be possible, see gen_mi_builder
//#else
   unreachable("unsupported");
//#endif
}

#if GEN_GEN <= 5
#define URB_VS 0
#define URB_GS 1
#define URB_CLP 2
#define URB_SF 3
#define URB_CS 4

static const struct {
   GLuint min_nr_entries;
   GLuint preferred_nr_entries;
   GLuint min_entry_size;
   GLuint max_entry_size;
} limits[URB_CS+1] = {
   { 16, 32, 1, 5 },			/* vs */
   { 4, 8,  1, 5 },			/* gs */
   { 5, 10,  1, 5 },			/* clp */
   { 1, 8,  1, 12 },		        /* sf */
   { 1, 4,  1, 32 }			/* cs */
};

static bool check_urb_layout(struct crocus_context *ice)
{
   ice->urb.vs_start = 0;
   ice->urb.gs_start = ice->urb.nr_vs_entries * ice->urb.vsize;
   ice->urb.clip_start = ice->urb.gs_start + ice->urb.nr_gs_entries * ice->urb.vsize;
   ice->urb.sf_start = ice->urb.clip_start + ice->urb.nr_clip_entries * ice->urb.vsize;
   ice->urb.cs_start = ice->urb.sf_start + ice->urb.nr_sf_entries * ice->urb.sfsize;

   return ice->urb.cs_start + ice->urb.nr_cs_entries *
      ice->urb.csize <= ice->urb.size;
}


static void
crocus_calculate_urb_fence(struct crocus_batch *batch, unsigned csize,
                           unsigned vsize, unsigned sfsize)
{
   const struct gen_device_info *devinfo = &batch->screen->devinfo;
   struct crocus_context *ice = batch->ice;
     if (csize < limits[URB_CS].min_entry_size)
      csize = limits[URB_CS].min_entry_size;

   if (vsize < limits[URB_VS].min_entry_size)
      vsize = limits[URB_VS].min_entry_size;

   if (sfsize < limits[URB_SF].min_entry_size)
      sfsize = limits[URB_SF].min_entry_size;

   if (ice->urb.vsize < vsize ||
       ice->urb.sfsize < sfsize ||
       ice->urb.csize < csize ||
       (ice->urb.constrained && (ice->urb.vsize > vsize ||
				 ice->urb.sfsize > sfsize ||
				 ice->urb.csize > csize))) {


      ice->urb.csize = csize;
      ice->urb.sfsize = sfsize;
      ice->urb.vsize = vsize;

      ice->urb.nr_vs_entries = limits[URB_VS].preferred_nr_entries;
      ice->urb.nr_gs_entries = limits[URB_GS].preferred_nr_entries;
      ice->urb.nr_clip_entries = limits[URB_CLP].preferred_nr_entries;
      ice->urb.nr_sf_entries = limits[URB_SF].preferred_nr_entries;
      ice->urb.nr_cs_entries = limits[URB_CS].preferred_nr_entries;

      ice->urb.constrained = 0;

      if (devinfo->gen == 5) {
         ice->urb.nr_vs_entries = 128;
         ice->urb.nr_sf_entries = 48;
         if (check_urb_layout(ice)) {
            goto done;
         } else {
            ice->urb.constrained = 1;
            ice->urb.nr_vs_entries = limits[URB_VS].preferred_nr_entries;
            ice->urb.nr_sf_entries = limits[URB_SF].preferred_nr_entries;
         }
      } else if (devinfo->is_g4x) {
         ice->urb.nr_vs_entries = 64;
         if (check_urb_layout(ice)) {
            goto done;
         } else {
            ice->urb.constrained = 1;
            ice->urb.nr_vs_entries = limits[URB_VS].preferred_nr_entries;
         }
      }

      if (!check_urb_layout(ice)) {
         ice->urb.nr_vs_entries = limits[URB_VS].min_nr_entries;
         ice->urb.nr_gs_entries = limits[URB_GS].min_nr_entries;
         ice->urb.nr_clip_entries = limits[URB_CLP].min_nr_entries;
         ice->urb.nr_sf_entries = limits[URB_SF].min_nr_entries;
         ice->urb.nr_cs_entries = limits[URB_CS].min_nr_entries;

         /* Mark us as operating with constrained nr_entries, so that next
          * time we recalculate we'll resize the fences in the hope of
          * escaping constrained mode and getting back to normal performance.
          */
         ice->urb.constrained = 1;

         if (!check_urb_layout(ice)) {
            /* This is impossible, given the maximal sizes of urb
             * entries and the values for minimum nr of entries
             * provided above.
             */
            fprintf(stderr, "couldn't calculate URB layout!\n");
            exit(1);
         }

         if (unlikely(INTEL_DEBUG & (DEBUG_URB|DEBUG_PERF)))
            fprintf(stderr, "URB CONSTRAINED\n");
      }

done:
      if (unlikely(INTEL_DEBUG & DEBUG_URB))
         fprintf(stderr,
                 "URB fence: %d ..VS.. %d ..GS.. %d ..CLP.. %d ..SF.. %d ..CS.. %d\n",
                 ice->urb.vs_start,
                 ice->urb.gs_start,
                 ice->urb.clip_start,
                 ice->urb.sf_start,
                 ice->urb.cs_start,
                 ice->urb.size);
   }
}

static void recalculate_urb_fence( struct crocus_batch *batch)
{
   struct crocus_context *ice = batch->ice;
   crocus_calculate_urb_fence(batch, 0,
                              brw_vue_prog_data(ice->shaders.prog[MESA_SHADER_VERTEX]->prog_data)->urb_entry_size,
                              ((struct brw_sf_prog_data *)ice->shaders.sf_prog->prog_data)->urb_entry_size);
}

static void
crocus_upload_urb_fence(struct crocus_batch *batch)
{
   crocus_emit_cmd(batch, GENX(URB_FENCE), urb) {
      urb.VSUnitURBReallocationRequest = 1;
      urb.GSUnitURBReallocationRequest = 1;
      urb.CLIPUnitURBReallocationRequest = 1;
      urb.SFUnitURBReallocationRequest = 1;
      urb.VFEUnitURBReallocationRequest = 1;
      urb.CSUnitURBReallocationRequest = 1;

      urb.VSFence = batch->ice->urb.gs_start;
      urb.GSFence = batch->ice->urb.clip_start;
      urb.CLIPFence = batch->ice->urb.sf_start;
      urb.SFFence = batch->ice->urb.cs_start;
      urb.CSFence = batch->ice->urb.size;
   }
}
#endif

#if GEN_GEN == 7

#define IVB_L3SQCREG1_SQGHPCI_DEFAULT     0x00730000
#define VLV_L3SQCREG1_SQGHPCI_DEFAULT     0x00d30000
#define HSW_L3SQCREG1_SQGHPCI_DEFAULT     0x00610000

static void
setup_l3_config(struct crocus_batch *batch, const struct intel_l3_config *cfg)
{
   const struct gen_device_info *devinfo = &batch->screen->devinfo;
   const bool has_dc = cfg->n[INTEL_L3P_DC] || cfg->n[INTEL_L3P_ALL];
   const bool has_is = cfg->n[INTEL_L3P_IS] || cfg->n[INTEL_L3P_RO] ||
                       cfg->n[INTEL_L3P_ALL];
   const bool has_c = cfg->n[INTEL_L3P_C] || cfg->n[INTEL_L3P_RO] ||
                      cfg->n[INTEL_L3P_ALL];
   const bool has_t = cfg->n[INTEL_L3P_T] || cfg->n[INTEL_L3P_RO] ||
                      cfg->n[INTEL_L3P_ALL];
   const bool has_slm = cfg->n[INTEL_L3P_SLM];

   /* According to the hardware docs, the L3 partitioning can only be changed
    * while the pipeline is completely drained and the caches are flushed,
    * which involves a first PIPE_CONTROL flush which stalls the pipeline...
    */
   crocus_emit_pipe_control_flush(batch, "l3_config",
                                  PIPE_CONTROL_DATA_CACHE_FLUSH |
                                  PIPE_CONTROL_CS_STALL);

   /* ...followed by a second pipelined PIPE_CONTROL that initiates
    * invalidation of the relevant caches.  Note that because RO invalidation
    * happens at the top of the pipeline (i.e. right away as the PIPE_CONTROL
    * command is processed by the CS) we cannot combine it with the previous
    * stalling flush as the hardware documentation suggests, because that
    * would cause the CS to stall on previous rendering *after* RO
    * invalidation and wouldn't prevent the RO caches from being polluted by
    * concurrent rendering before the stall completes.  This intentionally
    * doesn't implement the SKL+ hardware workaround suggesting to enable CS
    * stall on PIPE_CONTROLs with the texture cache invalidation bit set for
    * GPGPU workloads because the previous and subsequent PIPE_CONTROLs
    * already guarantee that there is no concurrent GPGPU kernel execution
    * (see SKL HSD 2132585).
    */
   crocus_emit_pipe_control_flush(batch, "l3 config",
                                  PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE |
                                  PIPE_CONTROL_CONST_CACHE_INVALIDATE |
                                  PIPE_CONTROL_INSTRUCTION_INVALIDATE |
                                  PIPE_CONTROL_STATE_CACHE_INVALIDATE);

   /* Now send a third stalling flush to make sure that invalidation is
    * complete when the L3 configuration registers are modified.
    */
   crocus_emit_pipe_control_flush(batch, "l3 config",
                                  PIPE_CONTROL_DATA_CACHE_FLUSH |
                                  PIPE_CONTROL_CS_STALL);


   assert(!cfg->n[INTEL_L3P_ALL]);

   /* When enabled SLM only uses a portion of the L3 on half of the banks,
    * the matching space on the remaining banks has to be allocated to a
    * client (URB for all validated configurations) set to the
    * lower-bandwidth 2-bank address hashing mode.
    */
   const bool urb_low_bw = has_slm && !devinfo->is_baytrail;
   assert(!urb_low_bw || cfg->n[INTEL_L3P_URB] == cfg->n[INTEL_L3P_SLM]);
      
   /* Minimum number of ways that can be allocated to the URB. */
   const unsigned n0_urb = (devinfo->is_baytrail ? 32 : 0);
   assert(cfg->n[INTEL_L3P_URB] >= n0_urb);

   uint32_t l3sqcr1, l3cr2, l3cr3;

   crocus_pack_state(GENX(L3SQCREG1), &l3sqcr1, reg) {
      reg.ConvertDC_UC = !has_dc;
      reg.ConvertIS_UC = !has_is;
      reg.ConvertC_UC = !has_c;
      reg.ConvertT_UC = !has_t;
   };

   l3sqcr1 |= GEN_VERSIONx10 == 75 ? HSW_L3SQCREG1_SQGHPCI_DEFAULT :
      devinfo->is_baytrail ? VLV_L3SQCREG1_SQGHPCI_DEFAULT :
      IVB_L3SQCREG1_SQGHPCI_DEFAULT;

   crocus_pack_state(GENX(L3CNTLREG2), &l3cr2, reg) {
      reg.SLMEnable = has_slm;
      reg.URBLowBandwidth = urb_low_bw;
      reg.URBAllocation = cfg->n[INTEL_L3P_URB] - n0_urb;
#if !GEN_VERSIONx10 == 75
      reg.ALLAllocation = cfg->n[INTEL_L3P_ALL];
#endif
      reg.ROAllocation = cfg->n[INTEL_L3P_RO];
      reg.DCAllocation = cfg->n[INTEL_L3P_DC];
   };

   crocus_pack_state(GENX(L3CNTLREG3), &l3cr3, reg) {
      reg.ISAllocation = cfg->n[INTEL_L3P_IS];
      reg.ISLowBandwidth = 0;
      reg.CAllocation = cfg->n[INTEL_L3P_C];
      reg.CLowBandwidth = 0;
      reg.TAllocation = cfg->n[INTEL_L3P_T];
      reg.TLowBandwidth = 0;
   };

   /* Set up the L3 partitioning. */
   crocus_emit_lri(batch, L3SQCREG1, l3sqcr1);
   crocus_emit_lri(batch, L3CNTLREG2, l3cr2);
   crocus_emit_lri(batch, L3CNTLREG3, l3cr3);

#if 0
      if (can_do_hsw_l3_atomics(brw->screen)) {
         /* Enable L3 atomics on HSW if we have a DC partition, otherwise keep
          * them disabled to avoid crashing the system hard.
          */
         crocus_emit_lri(batch, HSW_SCRATCH1,
         BEGIN_BATCH(5);
         OUT_BATCH(MI_LOAD_REGISTER_IMM | (5 - 2));
         OUT_BATCH(HSW_SCRATCH1);
         OUT_BATCH(has_dc ? 0 : HSW_SCRATCH1_L3_ATOMIC_DISABLE);
         OUT_BATCH(HSW_ROW_CHICKEN3);
         OUT_BATCH(REG_MASK(HSW_ROW_CHICKEN3_L3_ATOMIC_DISABLE) |
                   (has_dc ? 0 : HSW_ROW_CHICKEN3_L3_ATOMIC_DISABLE));
         ADVANCE_BATCH();
                         }
      
#endif
}

static void
emit_l3_state(struct crocus_batch *batch)
{
   struct crocus_context *ice = batch->ice;
   const struct intel_l3_config *const cfg = batch->screen->l3_config_3d;

   setup_l3_config(batch, cfg);
//      update_urb_size(brw, cfg);
//      ice->l3.config = cfg;

   if (unlikely(INTEL_DEBUG & DEBUG_L3)) {
//      fprintf(stderr, "L3 config transition (%f > %f): ", dw, dw_threshold);
      intel_dump_l3_config(cfg, stderr);
   }
}

/**
 * Emit a PIPE_CONTROL command for gen7 with the CS Stall bit set.
 */
static void
gen7_emit_cs_stall_flush(struct crocus_batch *batch)
{
  crocus_emit_pipe_control_write(batch,
				 "workaround",
				 PIPE_CONTROL_CS_STALL
				 | PIPE_CONTROL_WRITE_IMMEDIATE,
				 batch->screen->workaround_bo, 0, 0);
}
#endif

static void
emit_pipeline_select(struct crocus_batch *batch, uint32_t pipeline)
{
#if GEN_GEN >= 6
   /* From "BXML » GT » MI » vol1a GPU Overview » [Instruction]
    * PIPELINE_SELECT [DevBWR+]":
    *
    *    "Project: DEVSNB+
    *
    *     Software must ensure all the write caches are flushed through a
    *     stalling PIPE_CONTROL command followed by another PIPE_CONTROL
    *     command to invalidate read only caches prior to programming
    *     MI_PIPELINE_SELECT command to change the Pipeline Select Mode."
    */
   const unsigned dc_flush =
         batch->screen->devinfo.gen >= 7 ? PIPE_CONTROL_DATA_CACHE_FLUSH : 0;
    crocus_emit_pipe_control_flush(batch,
                                 "workaround: PIPELINE_SELECT flushes (1/2)",
                                 PIPE_CONTROL_RENDER_TARGET_FLUSH |
                                 PIPE_CONTROL_DEPTH_CACHE_FLUSH |
                                 dc_flush |
                                 PIPE_CONTROL_CS_STALL);

    crocus_emit_pipe_control_flush(batch,
                                 "workaround: PIPELINE_SELECT flushes (2/2)",
                                 PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE |
                                 PIPE_CONTROL_CONST_CACHE_INVALIDATE |
                                 PIPE_CONTROL_STATE_CACHE_INVALIDATE |
                                 PIPE_CONTROL_INSTRUCTION_INVALIDATE);
#else
    /* From "BXML » GT » MI » vol1a GPU Overview » [Instruction]
     * PIPELINE_SELECT [DevBWR+]":
     *
     *   Project: PRE-DEVSNB
     *
     *   Software must ensure the current pipeline is flushed via an
     *   MI_FLUSH or PIPE_CONTROL prior to the execution of PIPELINE_SELECT.
     */    
    crocus_emit_cmd(batch, GENX(MI_FLUSH), foo);
#endif

    crocus_emit_cmd(batch, GENX(PIPELINE_SELECT), sel) {
       sel.PipelineSelection = pipeline;
    }

#if GEN_GEN == 7 && !(GEN_VERSIONx10 == 75)
    if (pipeline == _3D) {
       gen7_emit_cs_stall_flush(batch);

       crocus_emit_cmd(batch, GENX(3DPRIMITIVE), prim) {
	  prim.PrimitiveTopologyType = _3DPRIM_POINTLIST;
       };
    }
#endif
}

#if GEN_GEN == 7
static void
crocus_alloc_push_constants(struct crocus_batch *batch)
{
#if GEN_VERSIONx10 == 75
   const unsigned push_constant_kb = batch->screen->devinfo.gt == 3 ? 32 : 16;
#else
   const unsigned push_constant_kb = 16;
#endif
   unsigned size_per_stage = push_constant_kb / 5;

   /* For now, we set a static partitioning of the push constant area,
    * assuming that all stages could be in use.
    *
    * TODO: Try lazily allocating the HS/DS/GS sections as needed, and
    *       see if that improves performance by offering more space to
    *       the VS/FS when those aren't in use.  Also, try dynamically
    *       enabling/disabling it like i965 does.  This would be more
    *       stalls and may not actually help; we don't know yet.
    */
   for (int i = 0; i <= MESA_SHADER_FRAGMENT; i++) {
      crocus_emit_cmd(batch, GENX(3DSTATE_PUSH_CONSTANT_ALLOC_VS), alloc) {
         alloc._3DCommandSubOpcode = 18 + i;
         alloc.ConstantBufferOffset = size_per_stage * i;
         alloc.ConstantBufferSize = i == MESA_SHADER_FRAGMENT ? (push_constant_kb - 4 * size_per_stage) : size_per_stage;
      }
   }

   /* From p292 of the Ivy Bridge PRM (11.2.4 3DSTATE_PUSH_CONSTANT_ALLOC_PS):
    *
    *     A PIPE_CONTROL command with the CS Stall bit set must be programmed
    *     in the ring after this instruction.
    *
    * No such restriction exists for Haswell or Baytrail.
    */
   if (!(GEN_VERSIONx10 == 75) && !batch->screen->devinfo.is_baytrail)
      gen7_emit_cs_stall_flush(batch);
}
#endif

/**
 * Upload the initial GPU state for a render context.
 *
 * This sets some invariant state that needs to be programmed a particular
 * way, but we never actually change.
 */
static void
crocus_init_render_context(struct crocus_batch *batch)
{
   UNUSED const struct gen_device_info *devinfo = &batch->screen->devinfo;

   emit_pipeline_select(batch, _3D);

   crocus_emit_cmd(batch, GENX(STATE_SIP), foo);

#if GEN_GEN >= 5 || GEN_US_G4X
   /* Use the legacy AA line coverage computation. */
   crocus_emit_cmd(batch, GENX(3DSTATE_AA_LINE_PARAMETERS), foo);
#endif

   /* No polygon stippling offsets are necessary. */
   /* TODO: may need to set an offset for origin-UL framebuffers */
   crocus_emit_cmd(batch, GENX(3DSTATE_POLY_STIPPLE_OFFSET), foo);

#if GEN_GEN == 7
   crocus_alloc_push_constants(batch);
#endif
}

#if GEN_GEN == 7
static void
crocus_init_compute_context(struct crocus_batch *batch)
{
   UNUSED const struct gen_device_info *devinfo = &batch->screen->devinfo;

   emit_pipeline_select(batch, GPGPU);

}
#endif

struct crocus_depth_buffer_state {
   /* Depth/HiZ/Stencil related hardware packets. */
   uint32_t packets[GENX(3DSTATE_DEPTH_BUFFER_length)
#if GEN_GEN >= 5
                    + GENX(3DSTATE_STENCIL_BUFFER_length) +
                    GENX(3DSTATE_HIER_DEPTH_BUFFER_length) +
                    GENX(3DSTATE_CLEAR_PARAMS_length)
#endif
                    ];
};

/**
 * Generation-specific context state (ice->state.genx->...).
 *
 * Most state can go in crocus_context directly, but these encode hardware
 * packets which vary by generation.
 */
struct crocus_genx_state {
   struct crocus_depth_buffer_state depth_buffer;

#if GEN_GEN == 7
   uint32_t so_buffers[4 * GENX(3DSTATE_SO_BUFFER_length)];
#endif

   struct {
#if GEN_GEN == 7
      // TODO: verify this is needed, originally only for gen8
      struct brw_image_param image_param[PIPE_MAX_SHADER_IMAGES];
#endif
   } shaders[MESA_SHADER_STAGES];
};

/**
 * The pipe->set_blend_color() driver hook.
 *
 * This corresponds to our COLOR_CALC_STATE.
 */
static void
crocus_set_blend_color(struct pipe_context *ctx,
                     const struct pipe_blend_color *state)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;

   /* Our COLOR_CALC_STATE is exactly pipe_blend_color, so just memcpy */
   memcpy(&ice->state.blend_color, state, sizeof(struct pipe_blend_color));
#if GEN_GEN <= 5
   ice->state.dirty |= CROCUS_DIRTY_GEN4_CONSTANT_COLOR;
#else
   ice->state.dirty |= CROCUS_DIRTY_COLOR_CALC_STATE;
#endif
}

/**
 * Gallium CSO for blend state (see pipe_blend_state).
 */
struct crocus_blend_state {
   /** Partial BLEND_STATE */
#if GEN_GEN >= 6
   uint32_t blend_state[GENX(BLEND_STATE_length) +
                        BRW_MAX_DRAW_BUFFERS * GENX(BLEND_STATE_ENTRY_length)];
#else
   struct pipe_blend_state blend_state;
#endif
   bool alpha_to_coverage; /* for shader key */

   /** Bitfield of whether blending is enabled for RT[i] - for aux resolves */
   uint8_t blend_enables;

   /** Bitfield of whether color writes are enabled for RT[i] */
   uint8_t color_write_enables;

   /** Does RT[0] use dual color blending? */
   bool dual_color_blending;
};

static enum pipe_blendfactor
fix_blendfactor(enum pipe_blendfactor f, bool alpha_to_one)
{
   if (alpha_to_one) {
      if (f == PIPE_BLENDFACTOR_SRC1_ALPHA)
         return PIPE_BLENDFACTOR_ONE;

      if (f == PIPE_BLENDFACTOR_INV_SRC1_ALPHA)
         return PIPE_BLENDFACTOR_ZERO;
   }

   return f;
}

/**
 * The pipe->create_blend_state() driver hook.
 *
 * Translates a pipe_blend_state into crocus_blend_state.
 */
static void *
crocus_create_blend_state(struct pipe_context *ctx,
                        const struct pipe_blend_state *state)
{
   struct crocus_blend_state *cso = malloc(sizeof(struct crocus_blend_state));
#if GEN_GEN >= 6
   uint32_t *blend_entry = cso->blend_state + GENX(BLEND_STATE_length);
#endif
   cso->blend_enables = 0;
   cso->color_write_enables = 0;
   STATIC_ASSERT(BRW_MAX_DRAW_BUFFERS <= 8);

   cso->alpha_to_coverage = state->alpha_to_coverage;

   bool indep_alpha_blend = false;
#if GEN_GEN >= 6
   for (int i = 0; i < BRW_MAX_DRAW_BUFFERS; i++) {
      const struct pipe_rt_blend_state *rt =
         &state->rt[state->independent_blend_enable ? i : 0];

      enum pipe_blendfactor src_rgb =
         fix_blendfactor(rt->rgb_src_factor, state->alpha_to_one);
      enum pipe_blendfactor src_alpha =
         fix_blendfactor(rt->alpha_src_factor, state->alpha_to_one);
      enum pipe_blendfactor dst_rgb =
         fix_blendfactor(rt->rgb_dst_factor, state->alpha_to_one);
      enum pipe_blendfactor dst_alpha =
         fix_blendfactor(rt->alpha_dst_factor, state->alpha_to_one);

      if (rt->rgb_func != rt->alpha_func ||
          src_rgb != src_alpha || dst_rgb != dst_alpha)
         indep_alpha_blend = true;

      if (rt->blend_enable)
         cso->blend_enables |= 1u << i;

      if (rt->colormask)
         cso->color_write_enables |= 1u << i;
#if GEN_GEN >= 6
      crocus_pack_state(GENX(BLEND_STATE_ENTRY), blend_entry, be) {
         be.LogicOpEnable = state->logicop_enable;
         be.LogicOpFunction = state->logicop_func;

         be.ColorClampRange = COLORCLAMP_RTFORMAT;
         be.PreBlendColorClampEnable = true;
         be.PostBlendColorClampEnable = true;

         be.ColorBufferBlendEnable = rt->blend_enable;

         be.ColorBlendFunction          = rt->rgb_func;
         be.AlphaBlendFunction          = rt->alpha_func;
         be.SourceBlendFactor           = src_rgb;
         be.SourceAlphaBlendFactor      = src_alpha;
         be.DestinationBlendFactor      = dst_rgb;
         be.DestinationAlphaBlendFactor = dst_alpha;

         be.WriteDisableRed   = !(rt->colormask & PIPE_MASK_R);
         be.WriteDisableGreen = !(rt->colormask & PIPE_MASK_G);
         be.WriteDisableBlue  = !(rt->colormask & PIPE_MASK_B);
         be.WriteDisableAlpha = !(rt->colormask & PIPE_MASK_A);

         be.AlphaToCoverageEnable = state->alpha_to_coverage;
         be.IndependentAlphaBlendEnable = indep_alpha_blend;
         be.AlphaToOneEnable = state->alpha_to_one;
         be.AlphaToCoverageDitherEnable = state->alpha_to_coverage;
         be.ColorDitherEnable = state->dither;

         /* bl.AlphaTestEnable and bs.AlphaTestFunction are filled in later. */
         // Except they're not... fix that. Can't be done here since it needs
         // to be conditional on non-integer RT's
      }
#endif

      blend_entry += GENX(BLEND_STATE_ENTRY_length);
   }

   cso->dual_color_blending = util_blend_state_is_dual(state, 0);
#else
   cso->blend_state = *state;
   for (int i = 0; i < BRW_MAX_DRAW_BUFFERS; i++) {
      const struct pipe_rt_blend_state *rt =
         &state->rt[state->independent_blend_enable ? i : 0];
      if (rt->blend_enable)
         cso->blend_enables |= 1u << i;
      if (rt->colormask)
         cso->color_write_enables |= 1u << i;
   }
#endif
   return cso;
}

/**
 * The pipe->bind_blend_state() driver hook.
 *
 * Bind a blending CSO and flag related dirty bits.
 */
static void
crocus_bind_blend_state(struct pipe_context *ctx, void *state)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_blend_state *cso = state;

   ice->state.cso_blend = cso;
   ice->state.blend_enables = cso ? cso->blend_enables : 0;

   ice->state.dirty |= CROCUS_DIRTY_PS_BLEND;
   ice->state.dirty |= CROCUS_DIRTY_BINDINGS_FS;
   ice->state.dirty |= CROCUS_DIRTY_BLEND_STATE;
   ice->state.dirty |= CROCUS_DIRTY_COLOR_CALC_STATE;
   ice->state.dirty |= CROCUS_DIRTY_RENDER_RESOLVES_AND_FLUSHES;
   ice->state.dirty |= ice->state.dirty_for_nos[CROCUS_NOS_BLEND];
}

/**
 * Return true if the FS writes to any color outputs which are not disabled
 * via color masking.
 */
static bool
has_writeable_rt(const struct crocus_blend_state *cso_blend,
                 const struct shader_info *fs_info)
{
   if (!fs_info)
      return false;

   unsigned rt_outputs = fs_info->outputs_written >> FRAG_RESULT_DATA0;

   if (fs_info->outputs_written & BITFIELD64_BIT(FRAG_RESULT_COLOR))
      rt_outputs = (1 << BRW_MAX_DRAW_BUFFERS) - 1;

   return cso_blend->color_write_enables & rt_outputs;
}

/**
 * Gallium CSO for depth, stencil, and alpha testing state.
 */
struct crocus_depth_stencil_alpha_state {
   /** Partial 3DSTATE_WM_DEPTH_STENCIL. */
   struct pipe_depth_stencil_alpha_state cso;

   /** Outbound to resolve and cache set tracking. */
   bool depth_writes_enabled;
   bool stencil_writes_enabled;

   /** Outbound to Gen8-9 PMA stall equations */
   bool depth_test_enabled;
};

/**
 * The pipe->create_depth_stencil_alpha_state() driver hook.
 *
 * We encode most of 3DSTATE_WM_DEPTH_STENCIL, and just save off the alpha
 * testing state since we need pieces of it in a variety of places.
 */
static void *
crocus_create_zsa_state(struct pipe_context *ctx,
                      const struct pipe_depth_stencil_alpha_state *state)
{
   struct crocus_depth_stencil_alpha_state *cso =
      malloc(sizeof(struct crocus_depth_stencil_alpha_state));

   bool two_sided_stencil = state->stencil[1].enabled;

   cso->cso = *state;
   cso->depth_writes_enabled = state->depth_writemask;
   cso->depth_test_enabled = state->depth_enabled;
   cso->stencil_writes_enabled =
      state->stencil[0].writemask != 0 ||
      (two_sided_stencil && state->stencil[1].writemask != 0);

   /* The state tracker needs to optimize away EQUAL writes for us. */
   assert(!(state->depth_func == PIPE_FUNC_EQUAL && state->depth_writemask));

   return cso;
}

/**
 * The pipe->bind_depth_stencil_alpha_state() driver hook.
 *
 * Bind a depth/stencil/alpha CSO and flag related dirty bits.
 */
static void
crocus_bind_zsa_state(struct pipe_context *ctx, void *state)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_depth_stencil_alpha_state *old_cso = ice->state.cso_zsa;
   struct crocus_depth_stencil_alpha_state *new_cso = state;

   if (new_cso) {
      if (cso_changed(cso.alpha_ref_value))
         ice->state.dirty |= CROCUS_DIRTY_COLOR_CALC_STATE;

      if (cso_changed(cso.alpha_enabled))
         ice->state.dirty |= CROCUS_DIRTY_PS_BLEND | CROCUS_DIRTY_BLEND_STATE;

      if (cso_changed(cso.alpha_func))
         ice->state.dirty |= CROCUS_DIRTY_BLEND_STATE;

      if (cso_changed(depth_writes_enabled))
         ice->state.dirty |= CROCUS_DIRTY_RENDER_RESOLVES_AND_FLUSHES;

#if GEN_GEN <= 5
      ice->state.dirty |= CROCUS_DIRTY_COLOR_CALC_STATE;
#endif

      ice->state.depth_writes_enabled = new_cso->depth_writes_enabled;
      ice->state.stencil_writes_enabled = new_cso->stencil_writes_enabled;
   }

   ice->state.cso_zsa = new_cso;
   ice->state.dirty |= CROCUS_DIRTY_CC_VIEWPORT;
   ice->state.dirty |= CROCUS_DIRTY_WM_DEPTH_STENCIL;
   ice->state.dirty |= ice->state.dirty_for_nos[CROCUS_NOS_DEPTH_STENCIL_ALPHA];
}

/**
 * Gallium CSO for rasterizer state.
 */
struct crocus_rasterizer_state {
   struct pipe_rasterizer_state state;
#if GEN_GEN >= 6
   uint32_t sf[GENX(3DSTATE_SF_length)];
   uint32_t clip[GENX(3DSTATE_CLIP_length)];
#endif
   uint32_t line_stipple[GENX(3DSTATE_LINE_STIPPLE_length)];

   uint8_t num_clip_plane_consts;
   bool clip_halfz; /* for CC_VIEWPORT */
   bool depth_clip_near; /* for CC_VIEWPORT */
   bool depth_clip_far; /* for CC_VIEWPORT */
   bool flatshade; /* for shader state */
   bool flatshade_first; /* for stream output */
   bool clamp_fragment_color; /* for shader state */
   bool light_twoside; /* for shader state */
   bool rasterizer_discard; /* for 3DSTATE_STREAMOUT and 3DSTATE_CLIP */
   bool half_pixel_center; /* for 3DSTATE_MULTISAMPLE */
   bool line_stipple_enable;
   bool poly_stipple_enable;
   bool multisample;
   bool force_persample_interp;
   bool conservative_rasterization;
   bool fill_mode_point_or_line;
   enum pipe_sprite_coord_mode sprite_coord_mode; /* PIPE_SPRITE_* */
   uint16_t sprite_coord_enable;
};

static float
get_line_width(const struct pipe_rasterizer_state *state)
{
   float line_width = state->line_width;

   /* From the OpenGL 4.4 spec:
    *
    * "The actual width of non-antialiased lines is determined by rounding
    *  the supplied width to the nearest integer, then clamping it to the
    *  implementation-dependent maximum non-antialiased line width."
    */
   if (!state->multisample && !state->line_smooth)
      line_width = roundf(state->line_width);

   if (!state->multisample && state->line_smooth && line_width < 1.5f) {
      /* For 1 pixel line thickness or less, the general anti-aliasing
       * algorithm gives up, and a garbage line is generated.  Setting a
       * Line Width of 0.0 specifies the rasterization of the "thinnest"
       * (one-pixel-wide), non-antialiased lines.
       *
       * Lines rendered with zero Line Width are rasterized using the
       * "Grid Intersection Quantization" rules as specified by the
       * "Zero-Width (Cosmetic) Line Rasterization" section of the docs.
       */
      line_width = 0.0f;
   }

   return line_width;
}

/**
 * The pipe->create_rasterizer_state() driver hook.
 */
static void *
crocus_create_rasterizer_state(struct pipe_context *ctx,
                             const struct pipe_rasterizer_state *state)
{
   struct crocus_rasterizer_state *cso =
      malloc(sizeof(struct crocus_rasterizer_state));

   cso->multisample = state->multisample;
   cso->force_persample_interp = state->force_persample_interp;
   cso->clip_halfz = state->clip_halfz;
   cso->depth_clip_near = state->depth_clip_near;
   cso->depth_clip_far = state->depth_clip_far;
   cso->flatshade = state->flatshade;
   cso->flatshade_first = state->flatshade_first;
   cso->clamp_fragment_color = state->clamp_fragment_color;
   cso->light_twoside = state->light_twoside;
   cso->rasterizer_discard = state->rasterizer_discard;
   cso->half_pixel_center = state->half_pixel_center;
   cso->sprite_coord_mode = state->sprite_coord_mode;
   cso->sprite_coord_enable = state->sprite_coord_enable;
   cso->line_stipple_enable = state->line_stipple_enable;
   cso->poly_stipple_enable = state->poly_stipple_enable;
   cso->conservative_rasterization =
      state->conservative_raster_mode == PIPE_CONSERVATIVE_RASTER_POST_SNAP;

   cso->fill_mode_point_or_line =
      state->fill_front == PIPE_POLYGON_MODE_LINE ||
      state->fill_front == PIPE_POLYGON_MODE_POINT ||
      state->fill_back == PIPE_POLYGON_MODE_LINE ||
      state->fill_back == PIPE_POLYGON_MODE_POINT;

   if (state->clip_plane_enable != 0)
      cso->num_clip_plane_consts = util_logbase2(state->clip_plane_enable) + 1;
   else
      cso->num_clip_plane_consts = 0;

   float line_width = get_line_width(state);

#if GEN_GEN >= 6
   crocus_pack_command(GENX(3DSTATE_SF), cso->sf, sf) {
      sf.StatisticsEnable = true;
      sf.AALineDistanceMode = AALINEDISTANCE_TRUE;
      sf.LineEndCapAntialiasingRegionWidth =
         state->line_smooth ? _10pixels : _05pixels;
      sf.LastPixelEnable = state->line_last_pixel;
      sf.LineWidth = line_width;
      sf.PointWidthSource = state->point_size_per_vertex ? Vertex : State;
      sf.PointWidth = state->point_size;

      if (state->flatshade_first) {
         sf.TriangleFanProvokingVertexSelect = 1;
      } else {
         sf.TriangleStripListProvokingVertexSelect = 2;
         sf.TriangleFanProvokingVertexSelect = 2;
         sf.LineStripListProvokingVertexSelect = 1;
      }

      sf.FrontWinding = state->front_ccw ? 1 : 0; // Or the other way...
      sf.CullMode = translate_cull_mode(state->cull_face);

#if GEN_GEN == 6
      sf.AttributeSwizzleEnable = true;
      if (state->sprite_coord_mode == PIPE_SPRITE_COORD_LOWER_LEFT)
         sf.PointSpriteTextureCoordinateOrigin = LOWERLEFT;
      else
         sf.PointSpriteTextureCoordinateOrigin = UPPERLEFT;
#endif

#if GEN_GEN >= 6
      sf.GlobalDepthOffsetEnableSolid = state->offset_tri;
      sf.GlobalDepthOffsetEnableWireframe = state->offset_line;
      sf.GlobalDepthOffsetEnablePoint = state->offset_point;
      sf.GlobalDepthOffsetConstant = state->offset_units * 2;
      sf.GlobalDepthOffsetScale = state->offset_scale;
      sf.GlobalDepthOffsetClamp = state->offset_clamp;

      sf.FrontFaceFillMode = translate_fill_mode(state->fill_front);
      sf.BackFaceFillMode = translate_fill_mode(state->fill_back);
#endif

#if GEN_VERSIONx10 == 75
      sf.LineStippleEnable = state->line_stipple_enable;
#endif
   }
#endif
   cso->state = *state;

#if GEN_GEN >= 6
   crocus_pack_command(GENX(3DSTATE_CLIP), cso->clip, cl) {
      /* cl.NonPerspectiveBarycentricEnable is filled in at draw time from
       * the FS program; cl.ForceZeroRTAIndexEnable is filled in from the FB.
       */
#if GEN_GEN >= 7
      cl.EarlyCullEnable = true;
#endif

#if GEN_GEN == 7
      cl.FrontWinding = state->front_ccw ? 1 : 0;
      cl.CullMode = translate_cull_mode(state->cull_face);
#endif
      cl.UserClipDistanceClipTestEnableBitmask = state->clip_plane_enable;
      cl.APIMode = state->clip_halfz ? APIMODE_D3D : APIMODE_OGL;
      cl.GuardbandClipTestEnable = true;
      cl.ClipEnable = true;
      cl.MinimumPointWidth = 0.125;
      cl.MaximumPointWidth = 255.875;
      cl.ViewportZClipTestEnable = (state->depth_clip_near || state->depth_clip_far);

      if (state->flatshade_first) {
         cl.TriangleFanProvokingVertexSelect = 1;
      } else {
         cl.TriangleStripListProvokingVertexSelect = 2;
         cl.TriangleFanProvokingVertexSelect = 2;
         cl.LineStripListProvokingVertexSelect = 1;
      }
   }
#endif

   /* Remap from 0..255 back to 1..256 */
   const unsigned line_stipple_factor = state->line_stipple_factor + 1;

   crocus_pack_command(GENX(3DSTATE_LINE_STIPPLE), cso->line_stipple, line) {
      if (state->line_stipple_enable) {
         line.LineStipplePattern = state->line_stipple_pattern;
         line.LineStippleInverseRepeatCount = 1.0f / line_stipple_factor;
         line.LineStippleRepeatCount = line_stipple_factor;
      }
   }

   return cso;
}

/**
 * The pipe->bind_rasterizer_state() driver hook.
 *
 * Bind a rasterizer CSO and flag related dirty bits.
 */
static void
crocus_bind_rasterizer_state(struct pipe_context *ctx, void *state)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_rasterizer_state *old_cso = ice->state.cso_rast;
   struct crocus_rasterizer_state *new_cso = state;

   if (new_cso) {
      /* Try to avoid re-emitting 3DSTATE_LINE_STIPPLE, it's non-pipelined */
      if (cso_changed_memcmp(line_stipple))
         ice->state.dirty |= CROCUS_DIRTY_LINE_STIPPLE;

      if (cso_changed(half_pixel_center))
         ice->state.dirty |= CROCUS_DIRTY_MULTISAMPLE;

      if (cso_changed(line_stipple_enable) || cso_changed(poly_stipple_enable))
         ice->state.dirty |= CROCUS_DIRTY_WM;

      if (cso_changed(rasterizer_discard))
         ice->state.dirty |= CROCUS_DIRTY_STREAMOUT | CROCUS_DIRTY_CLIP;

      if (cso_changed(flatshade_first))
         ice->state.dirty |= CROCUS_DIRTY_STREAMOUT;

      if (cso_changed(depth_clip_near) || cso_changed(depth_clip_far) ||
          cso_changed(clip_halfz))
         ice->state.dirty |= CROCUS_DIRTY_CC_VIEWPORT;

      if (cso_changed(sprite_coord_enable) ||
          cso_changed(sprite_coord_mode) ||
          cso_changed(light_twoside))
         ice->state.dirty |= CROCUS_DIRTY_SBE;

      if (cso_changed(conservative_rasterization))
         ice->state.dirty |= CROCUS_DIRTY_FS;
   }

   ice->state.cso_rast = new_cso;
   ice->state.dirty |= CROCUS_DIRTY_RASTER;
   ice->state.dirty |= CROCUS_DIRTY_CLIP;
#if GEN_GEN <= 5
   ice->state.dirty |= CROCUS_DIRTY_WM | CROCUS_DIRTY_VS;
#endif
   ice->state.dirty |= ice->state.dirty_for_nos[CROCUS_NOS_RASTERIZER];
}

/**
 * Return true if the given wrap mode requires the border color to exist.
 *
 * (We can skip uploading it if the sampler isn't going to use it.)
 */
static bool
wrap_mode_needs_border_color(unsigned wrap_mode)
{
   return wrap_mode == TCM_CLAMP_BORDER;
}

/**
 * Gallium CSO for sampler state.
 */
struct crocus_sampler_state {
   struct pipe_sampler_state pstate;
   union pipe_color_union border_color;
   bool needs_border_color;
   unsigned wrap_s;
   unsigned wrap_t;
   unsigned wrap_r;
   unsigned mag_img_filter;
   float min_lod;
};

/**
 * The pipe->create_sampler_state() driver hook.
 *
 * We fill out SAMPLER_STATE (except for the border color pointer), and
 * store that on the CPU.  It doesn't make sense to upload it to a GPU
 * buffer object yet, because 3DSTATE_SAMPLER_STATE_POINTERS requires
 * all bound sampler states to be in contiguous memor.
 */
static void *
crocus_create_sampler_state(struct pipe_context *ctx,
                          const struct pipe_sampler_state *state)
{
   struct crocus_sampler_state *cso = CALLOC_STRUCT(crocus_sampler_state);
   struct crocus_context *ice = (struct crocus_context *) ctx;
   if (!cso)
      return NULL;

   STATIC_ASSERT(PIPE_TEX_FILTER_NEAREST == MAPFILTER_NEAREST);
   STATIC_ASSERT(PIPE_TEX_FILTER_LINEAR == MAPFILTER_LINEAR);

   bool either_nearest = state->min_img_filter == PIPE_TEX_FILTER_NEAREST ||
      state->mag_img_filter == PIPE_TEX_FILTER_NEAREST;
   cso->wrap_s = translate_wrap(state->wrap_s, either_nearest);
   cso->wrap_t = translate_wrap(state->wrap_t, either_nearest);
   cso->wrap_r = translate_wrap(state->wrap_r, either_nearest);

   cso->pstate = *state;

   memcpy(&cso->border_color, &state->border_color, sizeof(cso->border_color));

   cso->needs_border_color = wrap_mode_needs_border_color(cso->wrap_s) ||
                             wrap_mode_needs_border_color(cso->wrap_t) ||
                             wrap_mode_needs_border_color(cso->wrap_r);

   if (state->seamless_cube_map) {
      // TODO haswell workaround
      cso->wrap_s = TCM_CUBE;
      cso->wrap_t = TCM_CUBE;
      cso->wrap_r = TCM_CUBE;
   }
   cso->min_lod = state->min_lod;
   cso->mag_img_filter = state->mag_img_filter;

   // XXX: explain this code ported from ilo...I don't get it at all...
   if (state->min_mip_filter == PIPE_TEX_MIPFILTER_NONE &&
       state->min_lod > 0.0f) {
      cso->min_lod = 0.0f;
      cso->mag_img_filter = state->min_img_filter;
   }

   return cso;
}

/**
 * The pipe->bind_sampler_states() driver hook.
 */
static void
crocus_bind_sampler_states(struct pipe_context *ctx,
                         enum pipe_shader_type p_stage,
                         unsigned start, unsigned count,
                         void **states)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   gl_shader_stage stage = stage_from_pipe(p_stage);
   struct crocus_shader_state *shs = &ice->state.shaders[stage];

   assert(start + count <= CROCUS_MAX_TEXTURE_SAMPLERS);

   bool dirty = false;

   for (int i = 0; i < count; i++) {
      if (shs->samplers[start + i] != states[i]) {
         shs->samplers[start + i] = states[i];
         dirty = true;
      }
   }

   if (dirty) {
      ice->state.dirty |= CROCUS_DIRTY_WM;
      ice->state.dirty |= CROCUS_DIRTY_SAMPLER_STATES_VS << stage;
      ice->state.dirty |= ice->state.dirty_for_nos[CROCUS_NOS_TEXTURES];
   }
}

static void
crocus_upload_sampler_state(struct crocus_batch *batch,
                            struct crocus_sampler_state *cso,
                            uint32_t border_color_offset,
                            void *map)
{
   struct pipe_sampler_state *state = &cso->pstate;
   _crocus_pack_state(batch, GENX(SAMPLER_STATE), map, samp) {
      samp.TCXAddressControlMode = cso->wrap_s;
      samp.TCYAddressControlMode = cso->wrap_t;
      samp.TCZAddressControlMode = cso->wrap_r;
      samp.CubeSurfaceControlMode = state->seamless_cube_map;
#if GEN_GEN >= 6
      samp.NonnormalizedCoordinateEnable = !state->normalized_coords;
#endif
      samp.MinModeFilter = state->min_img_filter;
      samp.MagModeFilter = cso->mag_img_filter;
      samp.MipModeFilter = translate_mip_filter(state->min_mip_filter);
      samp.MaximumAnisotropy = RATIO21;

      if (state->max_anisotropy >= 2) {
         if (state->min_img_filter == PIPE_TEX_FILTER_LINEAR) {
            samp.MinModeFilter = MAPFILTER_ANISOTROPIC;
#if GEN_GEN >= 7
            samp.AnisotropicAlgorithm = EWAApproximation;
#endif
         }

         if (state->mag_img_filter == PIPE_TEX_FILTER_LINEAR)
            samp.MagModeFilter = MAPFILTER_ANISOTROPIC;

         samp.MaximumAnisotropy =
            MIN2((state->max_anisotropy - 2) / 2, RATIO161);
      }

      /* Set address rounding bits if not using nearest filtering. */
      if (state->min_img_filter != PIPE_TEX_FILTER_NEAREST) {
         samp.UAddressMinFilterRoundingEnable = true;
         samp.VAddressMinFilterRoundingEnable = true;
         samp.RAddressMinFilterRoundingEnable = true;
      }

      if (state->mag_img_filter != PIPE_TEX_FILTER_NEAREST) {
         samp.UAddressMagFilterRoundingEnable = true;
         samp.VAddressMagFilterRoundingEnable = true;
         samp.RAddressMagFilterRoundingEnable = true;
      }

      if (state->compare_mode == PIPE_TEX_COMPARE_R_TO_TEXTURE)
         samp.ShadowFunction = translate_shadow_func(state->compare_func);

      const float hw_max_lod = GEN_GEN >= 7 ? 14 : 13;

      samp.LODPreClampEnable = true;
      samp.MinLOD = CLAMP(cso->min_lod, 0, hw_max_lod);
      samp.MaxLOD = CLAMP(state->max_lod, 0, hw_max_lod);
      samp.TextureLODBias = CLAMP(state->lod_bias, -16, 15);

#if GEN_GEN < 6
      samp.BorderColorPointer =
         ro_bo(batch->state.bo, border_color_offset);
#else
      samp.BorderColorPointer = border_color_offset;
#endif
   }
}

static void
crocus_upload_border_color(struct crocus_batch *batch,
                           struct crocus_sampler_state *cso,
                           struct crocus_sampler_view *tex,
                           uint32_t *bc_offset)
{
   /* We may need to swizzle the border color for format faking.
    * A/LA formats are faked as R/RG with 000R or R00G swizzles.
    * This means we need to move the border color's A channel into
    * the R or G channels so that those read swizzles will move it
    * back into A.
    */
   union pipe_color_union *color = &cso->border_color;
   union pipe_color_union tmp;
   if (tex) {
      enum pipe_format internal_format = tex->res->internal_format;

      if (util_format_is_alpha(internal_format)) {
         unsigned char swz[4] = {
            PIPE_SWIZZLE_W, PIPE_SWIZZLE_0,
            PIPE_SWIZZLE_0, PIPE_SWIZZLE_0
         };
         util_format_apply_color_swizzle(&tmp, color, swz, true);
         color = &tmp;
      } else if (util_format_is_luminance_alpha(internal_format) &&
                 internal_format != PIPE_FORMAT_L8A8_SRGB) {
         unsigned char swz[4] = {
            PIPE_SWIZZLE_X, PIPE_SWIZZLE_W,
            PIPE_SWIZZLE_0, PIPE_SWIZZLE_0
         };
         util_format_apply_color_swizzle(&tmp, color, swz, true);
         color = &tmp;
      }
   }
   int alignment = 32;
   //TODO  if (GEN_VERSIONx10 == 75 && (is_integer_format || is_stencil_sampling)) {
   //alignment = 512;
   uint32_t *sdc = stream_state(batch,
                                GENX(SAMPLER_BORDER_COLOR_STATE_length) * sizeof(uint32_t),
                                alignment, bc_offset);
   struct GENX(SAMPLER_BORDER_COLOR_STATE) state = { 0 };

#define ASSIGN(dst, src) \
   do {                  \
      dst = src;         \
   } while (0)

#define ASSIGNu16(dst, src) \
   do {                     \
      dst = (uint16_t)src;  \
   } while (0)

#define ASSIGNu8(dst, src) \
   do {                    \
      dst = (uint8_t)src;  \
   } while (0)

#define BORDER_COLOR_ATTR(macro, _color_type, src)              \
   macro(state.BorderColor ## _color_type ## Red, src[0]);   \
   macro(state.BorderColor ## _color_type ## Green, src[1]);   \
   macro(state.BorderColor ## _color_type ## Blue, src[2]);   \
   macro(state.BorderColor ## _color_type ## Alpha, src[3]);

#if GEN_VERSIONx10 == 75
   //TODO
#elif GEN_GEN == 5 || GEN_GEN == 6
   BORDER_COLOR_ATTR(UNCLAMPED_FLOAT_TO_UBYTE, Unorm, color->f);
   BORDER_COLOR_ATTR(UNCLAMPED_FLOAT_TO_USHORT, Unorm16, color->f);
   BORDER_COLOR_ATTR(UNCLAMPED_FLOAT_TO_SHORT, Snorm16, color->f);

#define MESA_FLOAT_TO_HALF(dst, src)            \
   dst = _mesa_float_to_half(src);

   BORDER_COLOR_ATTR(MESA_FLOAT_TO_HALF, Float16, color->f);

#undef MESA_FLOAT_TO_HALF

   state.BorderColorSnorm8Red   = state.BorderColorSnorm16Red >> 8;
   state.BorderColorSnorm8Green = state.BorderColorSnorm16Green >> 8;
   state.BorderColorSnorm8Blue  = state.BorderColorSnorm16Blue >> 8;
   state.BorderColorSnorm8Alpha = state.BorderColorSnorm16Alpha >> 8;

   BORDER_COLOR_ATTR(ASSIGN, Float, color->f);

#elif GEN_GEN == 4
   BORDER_COLOR_ATTR(ASSIGN, , color->f);
#endif

#undef ASSIGN
#undef BORDER_COLOR_ATTR

   GENX(SAMPLER_BORDER_COLOR_STATE_pack)(batch, sdc, &state);
}

/**
 * Upload the sampler states into a contiguous area of GPU memory, for
 * for 3DSTATE_SAMPLER_STATE_POINTERS_*.
 *
 * Also fill out the border color state pointers.
 */
static void
crocus_upload_sampler_states(struct crocus_context *ice,
                             struct crocus_batch *batch, gl_shader_stage stage)
{
   struct crocus_shader_state *shs = &ice->state.shaders[stage];
   const struct shader_info *info = crocus_get_shader_info(ice, stage);

   /* We assume the state tracker will call pipe->bind_sampler_states()
    * if the program's number of textures changes.
    */
   unsigned count = info ? BITSET_LAST_BIT(info->textures_used) : 0;

   if (!count)
      return;

   /* Assemble the SAMPLER_STATEs into a contiguous table that lives
    * in the dynamic state memory zone, so we can point to it via the
    * 3DSTATE_SAMPLER_STATE_POINTERS_* commands.
    */
   unsigned size = count * 4 * GENX(SAMPLER_STATE_length);
   uint32_t *map = stream_state(batch, size, 32, &ice->state.shaders[stage].sampler_offset);

   if (unlikely(!map))
      return;

   crocus_record_state_size(batch->state_sizes, shs->sampler_table.offset, size);

   ice->state.need_border_colors &= ~(1 << stage);

   for (int i = 0; i < count; i++) {
      struct crocus_sampler_state *state = shs->samplers[i];
      struct crocus_sampler_view *tex = shs->textures[i];

      if (!state) {
         memset(map, 0, 4 * GENX(SAMPLER_STATE_length));
      } else {
         unsigned border_color_offset = 0;
         if (state->needs_border_color) {
            ice->state.need_border_colors |= 1 << stage;
            crocus_upload_border_color(batch, state, tex, &border_color_offset);
         }

         crocus_upload_sampler_state(batch, state, border_color_offset, map);
      }

      map += GENX(SAMPLER_STATE_length);
   }
}

#if GEN_VERSIONx10 == 75
static enum isl_channel_select
fmt_swizzle(const struct crocus_format_info *fmt, enum pipe_swizzle swz)
{
   switch (swz) {
   case PIPE_SWIZZLE_X: return fmt->swizzle.r;
   case PIPE_SWIZZLE_Y: return fmt->swizzle.g;
   case PIPE_SWIZZLE_Z: return fmt->swizzle.b;
   case PIPE_SWIZZLE_W: return fmt->swizzle.a;
   case PIPE_SWIZZLE_1: return SCS_ONE;
   case PIPE_SWIZZLE_0: return SCS_ZERO;
   default: unreachable("invalid swizzle");
   }
}
#endif

static void
fill_buffer_surface_state(struct isl_device *isl_dev,
                          struct crocus_resource *res,
                          void *map,
                          enum isl_format format,
                          struct isl_swizzle swizzle,
                          unsigned offset,
                          unsigned size)
{
   const struct isl_format_layout *fmtl = isl_format_get_layout(format);
   const unsigned cpp = format == ISL_FORMAT_RAW ? 1 : fmtl->bpb / 8;

   /* The ARB_texture_buffer_specification says:
    *
    *    "The number of texels in the buffer texture's texel array is given by
    *
    *       floor(<buffer_size> / (<components> * sizeof(<base_type>)),
    *
    *     where <buffer_size> is the size of the buffer object, in basic
    *     machine units and <components> and <base_type> are the element count
    *     and base data type for elements, as specified in Table X.1.  The
    *     number of texels in the texel array is then clamped to the
    *     implementation-dependent limit MAX_TEXTURE_BUFFER_SIZE_ARB."
    *
    * We need to clamp the size in bytes to MAX_TEXTURE_BUFFER_SIZE * stride,
    * so that when ISL divides by stride to obtain the number of texels, that
    * texel count is clamped to MAX_TEXTURE_BUFFER_SIZE.
    */
   unsigned final_size =
      MIN3(size, res->bo->size - res->offset - offset,
           CROCUS_MAX_TEXTURE_BUFFER_SIZE * cpp);

   isl_buffer_fill_state(isl_dev, map,
                         .address = res->bo->gtt_offset + res->offset + offset,
                         .size_B = final_size,
                         .format = format,
                         .swizzle = swizzle,
                         .stride_B = cpp,
                         .mocs = mocs(res->bo, isl_dev));
}

#define SURFACE_STATE_ALIGNMENT 64

/**
 * Allocate several contiguous SURFACE_STATE structures, one for each
 * supported auxiliary surface mode.  This only allocates the CPU-side
 * copy, they will need to be uploaded later after they're filled in.
 */
static void
alloc_surface_states(struct crocus_surface_state *surf_state,
                     unsigned aux_usages)
{
   const unsigned surf_size = 4 * GENX(RENDER_SURFACE_STATE_length);

   /* If this changes, update this to explicitly align pointers */
   // TODO STATIC_ASSERT(surf_size == SURFACE_STATE_ALIGNMENT);

   assert(aux_usages != 0);

   /* In case we're re-allocating them... */
   free(surf_state->cpu);

   surf_state->num_states = util_bitcount(aux_usages);
   surf_state->cpu = calloc(surf_state->num_states, surf_size);
   surf_state->ref.offset = 0;
   pipe_resource_reference(&surf_state->ref.res, NULL);

   assert(surf_state->cpu);
}

/**
 * Upload the CPU side SURFACE_STATEs into a GPU buffer.
 */
static void
upload_surface_states(struct u_upload_mgr *mgr,
                      struct crocus_surface_state *surf_state)
{
   const unsigned surf_size = 4 * GENX(RENDER_SURFACE_STATE_length);
   const unsigned bytes = surf_state->num_states * surf_size;

   void *map =
      upload_state(mgr, &surf_state->ref, bytes, SURFACE_STATE_ALIGNMENT);

   surf_state->ref.offset +=
      crocus_bo_offset_from_base_address(crocus_resource_bo(surf_state->ref.res));

   if (map)
      memcpy(map, surf_state->cpu, bytes);
}

/**
 * Update resource addresses in a set of SURFACE_STATE descriptors,
 * and re-upload them if necessary.
 */
static bool
update_surface_state_addrs(struct u_upload_mgr *mgr,
                           struct crocus_surface_state *surf_state,
                           struct crocus_bo *bo)
{
   if (surf_state->bo_address == bo->gtt_offset)
      return false;

   STATIC_ASSERT(GENX(RENDER_SURFACE_STATE_SurfaceBaseAddress_start) % 32 == 0);
   STATIC_ASSERT(GENX(RENDER_SURFACE_STATE_SurfaceBaseAddress_bits) == 32);
   // TODO: redo for relocs

   uint32_t *ss_addr = (uint32_t *) &surf_state->cpu[GENX(RENDER_SURFACE_STATE_SurfaceBaseAddress_start) / 32];

   /* First, update the CPU copies.  We assume no other fields exist in
    * the QWord containing Surface Base Address.
    */
   for (unsigned i = 0; i < surf_state->num_states; i++) {
      *ss_addr = *ss_addr - surf_state->bo_address + bo->gtt_offset;
      ss_addr = ((void *) ss_addr) + SURFACE_STATE_ALIGNMENT;
   }

   /* Next, upload the updated copies to a GPU buffer. */
   //   upload_surface_states(mgr, surf_state);

   surf_state->bo_address = bo->gtt_offset;

   return true;
}

#if GEN_GEN == 7
// TODO: check if this is needed for gen7, originally gen8
/**
 * Return an ISL surface for use with non-coherent render target reads.
 *
 * In a few complex cases, we can't use the SURFACE_STATE for normal render
 * target writes.  We need to make a separate one for sampling which refers
 * to the single slice of the texture being read.
 */
static void
get_rt_read_isl_surf(const struct gen_device_info *devinfo,
                     struct crocus_resource *res,
                     enum pipe_texture_target target,
                     struct isl_view *view,
                     uint32_t *offset_to_tile,
                     uint32_t *tile_x_sa,
                     uint32_t *tile_y_sa,
                     struct isl_surf *surf)
{
   *surf = res->surf;

   const enum isl_dim_layout dim_layout =
      crocus_get_isl_dim_layout(devinfo, res->surf.tiling, target);

   surf->dim = target_to_isl_surf_dim(target);

   if (surf->dim_layout == dim_layout)
      return;

   /* The layout of the specified texture target is not compatible with the
    * actual layout of the miptree structure in memory -- You're entering
    * dangerous territory, this can only possibly work if you only intended
    * to access a single level and slice of the texture, and the hardware
    * supports the tile offset feature in order to allow non-tile-aligned
    * base offsets, since we'll have to point the hardware to the first
    * texel of the level instead of relying on the usual base level/layer
    * controls.
    */
   assert(view->levels == 1 && view->array_len == 1);
   assert(*tile_x_sa == 0 && *tile_y_sa == 0);

   *offset_to_tile = crocus_resource_get_tile_offsets(res, view->base_level,
                                                    view->base_array_layer,
                                                    tile_x_sa, tile_y_sa);
   const unsigned l = view->base_level;

   surf->logical_level0_px.width = minify(surf->logical_level0_px.width, l);
   surf->logical_level0_px.height = surf->dim <= ISL_SURF_DIM_1D ? 1 :
      minify(surf->logical_level0_px.height, l);
   surf->logical_level0_px.depth = surf->dim <= ISL_SURF_DIM_2D ? 1 :
      minify(surf->logical_level0_px.depth, l);

   surf->logical_level0_px.array_len = 1;
   surf->levels = 1;
   surf->dim_layout = dim_layout;

   view->base_level = 0;
   view->base_array_layer = 0;
}
#endif

static void
fill_surface_state(struct isl_device *isl_dev,
                   void *map,
                   struct crocus_resource *res,
                   struct isl_surf *surf,
                   struct isl_view *view,
                   unsigned aux_usage,
                   uint32_t extra_main_offset,
                   uint32_t tile_x_sa,
                   uint32_t tile_y_sa)
{
   struct isl_surf_fill_state_info f = {
      .surf = surf,
      .view = view,
      .mocs = mocs(res->bo, isl_dev),
      .address = res->bo->gtt_offset + res->offset + extra_main_offset,
      .x_offset_sa = tile_x_sa,
      .y_offset_sa = tile_y_sa,
   };

   assert(!crocus_resource_unfinished_aux_import(res));

   if (aux_usage != ISL_AUX_USAGE_NONE) {
      f.aux_surf = &res->aux.surf;
      f.aux_usage = aux_usage;
      f.aux_address = res->aux.bo->gtt_offset + res->aux.offset;

      struct crocus_bo *clear_bo = NULL;
      uint64_t clear_offset = 0;
      f.clear_color =
         crocus_resource_get_clear_color(res, &clear_bo, &clear_offset);
      if (clear_bo) {
         f.clear_address = clear_bo->gtt_offset + clear_offset;
         f.use_clear_address = isl_dev->info->gen > 9;
      }
   }

   isl_surf_fill_state_s(isl_dev, map, &f);
}

/**
 * The pipe->create_sampler_view() driver hook.
 */
static struct pipe_sampler_view *
crocus_create_sampler_view(struct pipe_context *ctx,
                         struct pipe_resource *tex,
                         const struct pipe_sampler_view *tmpl)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_screen *screen = (struct crocus_screen *)ctx->screen;
   const struct gen_device_info *devinfo = &screen->devinfo;
   struct crocus_sampler_view *isv = calloc(1, sizeof(struct crocus_sampler_view));

   if (!isv)
      return NULL;

   /* initialize base object */
   isv->base = *tmpl;
   isv->base.context = ctx;
   isv->base.texture = NULL;
   pipe_reference_init(&isv->base.reference, 1);
   pipe_resource_reference(&isv->base.texture, tex);

   if (util_format_is_depth_or_stencil(tmpl->format)) {
      struct crocus_resource *zres, *sres;
      const struct util_format_description *desc =
         util_format_description(tmpl->format);

      crocus_get_depth_stencil_resources(devinfo, tex, &zres, &sres);

      tex = util_format_has_depth(desc) ? &zres->base : &sres->base;
   }

   isv->res = (struct crocus_resource *) tex;

   alloc_surface_states(&isv->surface_state, isv->res->aux.sampler_usages);

   isv->surface_state.bo_address = isv->res->bo->gtt_offset;

   isl_surf_usage_flags_t usage = ISL_SURF_USAGE_TEXTURE_BIT;

   if (isv->base.target == PIPE_TEXTURE_CUBE ||
       isv->base.target == PIPE_TEXTURE_CUBE_ARRAY)
      usage |= ISL_SURF_USAGE_CUBE_BIT;

   const struct crocus_format_info fmt =
      crocus_format_for_usage(devinfo, tmpl->format, usage);

   isv->clear_color = isv->res->aux.clear_color;

   isv->view = (struct isl_view) {
      .format = fmt.fmt,
#if GEN_VERSIONx10 == 75
      .swizzle = (struct isl_swizzle) {
         .r = fmt_swizzle(&fmt, tmpl->swizzle_r),
         .g = fmt_swizzle(&fmt, tmpl->swizzle_g),
         .b = fmt_swizzle(&fmt, tmpl->swizzle_b),
         .a = fmt_swizzle(&fmt, tmpl->swizzle_a),
      },
#else
      /* swizzling handled in shader code */
      .swizzle = ISL_SWIZZLE_IDENTITY,
#endif
      .usage = usage,
   };

   void *map = isv->surface_state.cpu;

   /* Fill out SURFACE_STATE for this view. */
   if (tmpl->target != PIPE_BUFFER) {
      isv->view.base_level = tmpl->u.tex.first_level;
      isv->view.levels = tmpl->u.tex.last_level - tmpl->u.tex.first_level + 1;
      // XXX: do I need to port f9fd0cf4790cb2a530e75d1a2206dbb9d8af7cb2?
      isv->view.base_array_layer = tmpl->u.tex.first_layer;
      isv->view.array_len =
         tmpl->u.tex.last_layer - tmpl->u.tex.first_layer + 1;

      if (crocus_resource_unfinished_aux_import(isv->res))
         crocus_resource_finish_aux_import(&screen->base, isv->res);

      unsigned aux_modes = isv->res->aux.sampler_usages;
      while (aux_modes) {
         enum isl_aux_usage aux_usage = u_bit_scan(&aux_modes);

         /* If we have a multisampled depth buffer, do not create a sampler
          * surface state with HiZ.
          */
         fill_surface_state(&screen->isl_dev, map, isv->res, &isv->res->surf,
                            &isv->view, aux_usage, 0, 0, 0);

         map += SURFACE_STATE_ALIGNMENT;
      }
   } else {
      fill_buffer_surface_state(&screen->isl_dev, isv->res, map,
                                isv->view.format, isv->view.swizzle,
                                tmpl->u.buf.offset, tmpl->u.buf.size);
   }

//   upload_surface_states(ice->state.surface_uploader, &isv->surface_state);

   return &isv->base;
}

static void
crocus_sampler_view_destroy(struct pipe_context *ctx,
                          struct pipe_sampler_view *state)
{
   struct crocus_sampler_view *isv = (void *) state;
   pipe_resource_reference(&state->texture, NULL);
   pipe_resource_reference(&isv->surface_state.ref.res, NULL);
   free(isv->surface_state.cpu);
   free(isv);
}

/**
 * The pipe->create_surface() driver hook.
 *
 * In Gallium nomenclature, "surfaces" are a view of a resource that
 * can be bound as a render target or depth/stencil buffer.
 */
static struct pipe_surface *
crocus_create_surface(struct pipe_context *ctx,
                    struct pipe_resource *tex,
                    const struct pipe_surface *tmpl)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_screen *screen = (struct crocus_screen *)ctx->screen;
   const struct gen_device_info *devinfo = &screen->devinfo;

   isl_surf_usage_flags_t usage = 0;
   if (tmpl->writable)
      usage = ISL_SURF_USAGE_STORAGE_BIT;
   else if (util_format_is_depth_or_stencil(tmpl->format))
      usage = ISL_SURF_USAGE_DEPTH_BIT;
   else
      usage = ISL_SURF_USAGE_RENDER_TARGET_BIT;

   const struct crocus_format_info fmt =
      crocus_format_for_usage(devinfo, tmpl->format, usage);

   if ((usage & ISL_SURF_USAGE_RENDER_TARGET_BIT) &&
       !isl_format_supports_rendering(devinfo, fmt.fmt)) {
      /* Framebuffer validation will reject this invalid case, but it
       * hasn't had the opportunity yet.  In the meantime, we need to
       * avoid hitting ISL asserts about unsupported formats below.
       */
      return NULL;
   }

   struct crocus_surface *surf = calloc(1, sizeof(struct crocus_surface));
   struct pipe_surface *psurf = &surf->base;
   struct crocus_resource *res = (struct crocus_resource *) tex;

   if (!surf)
      return NULL;

   pipe_reference_init(&psurf->reference, 1);
   pipe_resource_reference(&psurf->texture, tex);
   psurf->context = ctx;
   psurf->format = tmpl->format;
   psurf->width = tex->width0;
   psurf->height = tex->height0;
   psurf->texture = tex;
   psurf->u.tex.first_layer = tmpl->u.tex.first_layer;
   psurf->u.tex.last_layer = tmpl->u.tex.last_layer;
   psurf->u.tex.level = tmpl->u.tex.level;

   uint32_t array_len = tmpl->u.tex.last_layer - tmpl->u.tex.first_layer + 1;

   struct isl_view *view = &surf->view;
   *view = (struct isl_view) {
      .format = fmt.fmt,
      .base_level = tmpl->u.tex.level,
      .levels = 1,
      .base_array_layer = tmpl->u.tex.first_layer,
      .array_len = array_len,
      .swizzle = ISL_SWIZZLE_IDENTITY,
      .usage = usage,
   };

#if GEN_GEN == 7
   // TODO: check if this is needed
   enum pipe_texture_target target = (tex->target == PIPE_TEXTURE_3D &&
                                      array_len == 1) ? PIPE_TEXTURE_2D :
                                     tex->target == PIPE_TEXTURE_1D_ARRAY ?
                                     PIPE_TEXTURE_2D_ARRAY : tex->target;

   struct isl_view *read_view = &surf->read_view;
   *read_view = (struct isl_view) {
      .format = fmt.fmt,
      .base_level = tmpl->u.tex.level,
      .levels = 1,
      .base_array_layer = tmpl->u.tex.first_layer,
      .array_len = array_len,
      .swizzle = ISL_SWIZZLE_IDENTITY,
      .usage = ISL_SURF_USAGE_TEXTURE_BIT,
   };
#endif

   surf->clear_color = res->aux.clear_color;

   /* Bail early for depth/stencil - we don't want SURFACE_STATE for them. */
   if (res->surf.usage & (ISL_SURF_USAGE_DEPTH_BIT |
                          ISL_SURF_USAGE_STENCIL_BIT))
      return psurf;


   alloc_surface_states(&surf->surface_state, res->aux.possible_usages);
   surf->surface_state.bo_address = res->bo->gtt_offset;

#if GEN_GEN == 7
   // TODO: check if this is needed
   alloc_surface_states(&surf->surface_state_read, res->aux.possible_usages);
   surf->surface_state_read.bo_address = res->bo->gtt_offset;
#endif

   if (!isl_format_is_compressed(res->surf.format)) {
      if (crocus_resource_unfinished_aux_import(res))
         crocus_resource_finish_aux_import(&screen->base, res);

      void *map = surf->surface_state.cpu;
      UNUSED void *map_read = surf->surface_state_read.cpu;

      /* This is a normal surface.  Fill out a SURFACE_STATE for each possible
       * auxiliary surface mode and return the pipe_surface.
       */
      unsigned aux_modes = res->aux.possible_usages;
      while (aux_modes) {
         enum isl_aux_usage aux_usage = u_bit_scan(&aux_modes);
         fill_surface_state(&screen->isl_dev, map, res, &res->surf,
                            view, aux_usage, 0, 0, 0);
         map += SURFACE_STATE_ALIGNMENT;

#if GEN_GEN == 7
         // TODO: check if this is needed
         struct isl_surf surf;
         uint32_t offset_to_tile = 0, tile_x_sa = 0, tile_y_sa = 0;
         get_rt_read_isl_surf(devinfo, res, target, read_view,
                              &offset_to_tile, &tile_x_sa, &tile_y_sa, &surf);
         fill_surface_state(&screen->isl_dev, map_read, res, &surf, read_view,
                            aux_usage, offset_to_tile, tile_x_sa, tile_y_sa);
         map_read += SURFACE_STATE_ALIGNMENT;
#endif
      }

      upload_surface_states(ice->state.surface_uploader, &surf->surface_state);

#if GEN_GEN == 7
      // TODO: check if this is needed
      upload_surface_states(ice->state.surface_uploader,
                            &surf->surface_state_read);
#endif

      return psurf;
   }

   /* The resource has a compressed format, which is not renderable, but we
    * have a renderable view format.  We must be attempting to upload blocks
    * of compressed data via an uncompressed view.
    *
    * In this case, we can assume there are no auxiliary buffers, a single
    * miplevel, and that the resource is single-sampled.  Gallium may try
    * and create an uncompressed view with multiple layers, however.
    */
   assert(!isl_format_is_compressed(fmt.fmt));
   assert(res->aux.possible_usages == 1 << ISL_AUX_USAGE_NONE);
   assert(res->surf.samples == 1);
   assert(view->levels == 1);

   struct isl_surf isl_surf;
   uint32_t offset_B = 0, tile_x_sa = 0, tile_y_sa = 0;

   if (view->base_level > 0) {
      /* We can't rely on the hardware's miplevel selection with such
       * a substantial lie about the format, so we select a single image
       * using the Tile X/Y Offset fields.  In this case, we can't handle
       * multiple array slices.
       *
       * On Broadwell, HALIGN and VALIGN are specified in pixels and are
       * hard-coded to align to exactly the block size of the compressed
       * texture.  This means that, when reinterpreted as a non-compressed
       * texture, the tile offsets may be anything and we can't rely on
       * X/Y Offset.
       *
       * Return NULL to force the state tracker to take fallback paths.
       */
      // TODO: check if the gen7 check is right, originally gen8
      if (view->array_len > 1 || GEN_GEN == 7)
         return NULL;

      const bool is_3d = res->surf.dim == ISL_SURF_DIM_3D;
      isl_surf_get_image_surf(&screen->isl_dev, &res->surf,
                              view->base_level,
                              is_3d ? 0 : view->base_array_layer,
                              is_3d ? view->base_array_layer : 0,
                              &isl_surf,
                              &offset_B, &tile_x_sa, &tile_y_sa);

      /* We use address and tile offsets to access a single level/layer
       * as a subimage, so reset level/layer so it doesn't offset again.
       */
      view->base_array_layer = 0;
      view->base_level = 0;
   } else {
      /* Level 0 doesn't require tile offsets, and the hardware can find
       * array slices using QPitch even with the format override, so we
       * can allow layers in this case.  Copy the original ISL surface.
       */
      memcpy(&isl_surf, &res->surf, sizeof(isl_surf));
   }

   /* Scale down the image dimensions by the block size. */
   const struct isl_format_layout *fmtl =
      isl_format_get_layout(res->surf.format);
   isl_surf.format = fmt.fmt;
   isl_surf.logical_level0_px = isl_surf_get_logical_level0_el(&isl_surf);
   isl_surf.phys_level0_sa = isl_surf_get_phys_level0_el(&isl_surf);
   tile_x_sa /= fmtl->bw;
   tile_y_sa /= fmtl->bh;

   psurf->width = isl_surf.logical_level0_px.width;
   psurf->height = isl_surf.logical_level0_px.height;

   struct isl_surf_fill_state_info f = {
      .surf = &isl_surf,
      .view = view,
      .mocs = mocs(res->bo, &screen->isl_dev),
      .address = res->bo->gtt_offset + offset_B,
      .x_offset_sa = tile_x_sa,
      .y_offset_sa = tile_y_sa,
   };

   isl_surf_fill_state_s(&screen->isl_dev, surf->surface_state.cpu, &f);

   upload_surface_states(ice->state.surface_uploader, &surf->surface_state);

   return psurf;
}

#if GEN_GEN >= 7
static void
fill_default_image_param(struct brw_image_param *param)
{
   memset(param, 0, sizeof(*param));
   /* Set the swizzling shifts to all-ones to effectively disable swizzling --
    * See emit_address_calculation() in brw_fs_surface_builder.cpp for a more
    * detailed explanation of these parameters.
    */
   param->swizzling[0] = 0xff;
   param->swizzling[1] = 0xff;
}

static void
fill_buffer_image_param(struct brw_image_param *param,
                        enum pipe_format pfmt,
                        unsigned size)
{
   const unsigned cpp = util_format_get_blocksize(pfmt);

   fill_default_image_param(param);
   param->size[0] = size / cpp;
   param->stride[0] = cpp;
}
#else
#define isl_surf_fill_image_param(x, ...)
#define fill_default_image_param(x, ...)
#define fill_buffer_image_param(x, ...)
#endif

/**
 * The pipe->set_shader_images() driver hook.
 */
static void
crocus_set_shader_images(struct pipe_context *ctx,
                         enum pipe_shader_type p_stage,
                         unsigned start_slot, unsigned count,
                         unsigned unbind_num_trailing_slots,
                         const struct pipe_image_view *p_images)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_screen *screen = (struct crocus_screen *)ctx->screen;
   const struct gen_device_info *devinfo = &screen->devinfo;
   gl_shader_stage stage = stage_from_pipe(p_stage);
   struct crocus_shader_state *shs = &ice->state.shaders[stage];
#if GEN_GEN == 7
   // TODO
   struct crocus_genx_state *genx = ice->state.genx;
   struct brw_image_param *image_params = genx->shaders[stage].image_param;
#endif

   shs->bound_image_views &= ~u_bit_consecutive(start_slot, count);

   for (unsigned i = 0; i < count; i++) {
      struct crocus_image_view *iv = &shs->image[start_slot + i];

      if (p_images && p_images[i].resource) {
         const struct pipe_image_view *img = &p_images[i];
         struct crocus_resource *res = (void *) img->resource;

         util_copy_image_view(&iv->base, img);

         shs->bound_image_views |= 1 << (start_slot + i);

         res->bind_history |= PIPE_BIND_SHADER_IMAGE;
         res->bind_stages |= 1 << stage;

         isl_surf_usage_flags_t usage = ISL_SURF_USAGE_STORAGE_BIT;
         enum isl_format isl_fmt =
            crocus_format_for_usage(devinfo, img->format, usage).fmt;

         bool untyped_fallback = false;

         if (img->shader_access & PIPE_IMAGE_ACCESS_READ) {
            /* On Gen8, try to use typed surfaces reads (which support a
             * limited number of formats), and if not possible, fall back
             * to untyped reads.
             */
            // TODO
            untyped_fallback = GEN_GEN == 7 &&
               !isl_has_matching_typed_storage_image_format(devinfo, isl_fmt);

            if (untyped_fallback)
               isl_fmt = ISL_FORMAT_RAW;
            else
               isl_fmt = isl_lower_storage_image_format(devinfo, isl_fmt);
         }

         alloc_surface_states(&iv->surface_state, 1 << ISL_AUX_USAGE_NONE);
         iv->surface_state.bo_address = res->bo->gtt_offset;

         void *map = iv->surface_state.cpu;

         if (res->base.target != PIPE_BUFFER) {
            struct isl_view view = {
               .format = isl_fmt,
               .base_level = img->u.tex.level,
               .levels = 1,
               .base_array_layer = img->u.tex.first_layer,
               .array_len = img->u.tex.last_layer - img->u.tex.first_layer + 1,
               .swizzle = ISL_SWIZZLE_IDENTITY,
               .usage = usage,
            };

            if (untyped_fallback) {
               fill_buffer_surface_state(&screen->isl_dev, res, map,
                                         isl_fmt, ISL_SWIZZLE_IDENTITY,
                                         0, res->bo->size);
            } else {
               /* Images don't support compression */
               unsigned aux_modes = 1 << ISL_AUX_USAGE_NONE;
               while (aux_modes) {
                  enum isl_aux_usage usage = u_bit_scan(&aux_modes);

                  fill_surface_state(&screen->isl_dev, map, res, &res->surf,
                                     &view, usage, 0, 0, 0);

                  map += SURFACE_STATE_ALIGNMENT;
               }
            }

            isl_surf_fill_image_param(&screen->isl_dev,
                                      &image_params[start_slot + i],
                                      &res->surf, &view);
         } else {
            util_range_add(&res->base, &res->valid_buffer_range, img->u.buf.offset,
                           img->u.buf.offset + img->u.buf.size);

            fill_buffer_surface_state(&screen->isl_dev, res, map,
                                      isl_fmt, ISL_SWIZZLE_IDENTITY,
                                      img->u.buf.offset, img->u.buf.size);
            fill_buffer_image_param(&image_params[start_slot + i],
                                    img->format, img->u.buf.size);
         }

         upload_surface_states(ice->state.surface_uploader, &iv->surface_state);
      } else {
         pipe_resource_reference(&iv->base.resource, NULL);
         pipe_resource_reference(&iv->surface_state.ref.res, NULL);
         fill_default_image_param(&image_params[start_slot + i]);
      }
   }

   ice->state.dirty |= CROCUS_DIRTY_BINDINGS_VS << stage;
   ice->state.dirty |=
      stage == MESA_SHADER_COMPUTE ? CROCUS_DIRTY_COMPUTE_RESOLVES_AND_FLUSHES
                                   : CROCUS_DIRTY_RENDER_RESOLVES_AND_FLUSHES;

   /* Broadwell also needs brw_image_params re-uploaded */
   ice->state.dirty |= CROCUS_DIRTY_CONSTANTS_VS << stage;
   shs->sysvals_need_upload = true;
}


/**
 * The pipe->set_sampler_views() driver hook.
 */
static void
crocus_set_sampler_views(struct pipe_context *ctx,
                         enum pipe_shader_type p_stage,
                         unsigned start, unsigned count,
                         unsigned unbind_num_trailing_slots,
                         struct pipe_sampler_view **views)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   gl_shader_stage stage = stage_from_pipe(p_stage);
   struct crocus_shader_state *shs = &ice->state.shaders[stage];

   shs->bound_sampler_views &= ~u_bit_consecutive(start, count);

   for (unsigned i = 0; i < count; i++) {
      struct pipe_sampler_view *pview = views ? views[i] : NULL;
      pipe_sampler_view_reference((struct pipe_sampler_view **)
                                  &shs->textures[start + i], pview);
      struct crocus_sampler_view *view = (void *) pview;
      if (view) {
         view->res->bind_history |= PIPE_BIND_SAMPLER_VIEW;
         view->res->bind_stages |= 1 << stage;

         shs->bound_sampler_views |= 1 << (start + i);

         update_surface_state_addrs(ice->state.surface_uploader,
                                    &view->surface_state, view->res->bo);
      }
   }

   ice->state.dirty |= (CROCUS_DIRTY_BINDINGS_VS << stage);
   ice->state.dirty |=
      stage == MESA_SHADER_COMPUTE ? CROCUS_DIRTY_COMPUTE_RESOLVES_AND_FLUSHES
                                   : CROCUS_DIRTY_RENDER_RESOLVES_AND_FLUSHES;
   ice->state.dirty |= ice->state.dirty_for_nos[CROCUS_NOS_TEXTURES];
}

/**
 * The pipe->set_tess_state() driver hook.
 */
static void
crocus_set_tess_state(struct pipe_context *ctx,
                    const float default_outer_level[4],
                    const float default_inner_level[2])
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_shader_state *shs = &ice->state.shaders[MESA_SHADER_TESS_CTRL];

   memcpy(&ice->state.default_outer_level[0], &default_outer_level[0], 4 * sizeof(float));
   memcpy(&ice->state.default_inner_level[0], &default_inner_level[0], 2 * sizeof(float));

   ice->state.dirty |= CROCUS_DIRTY_CONSTANTS_TCS;
   shs->sysvals_need_upload = true;
}

static void
crocus_surface_destroy(struct pipe_context *ctx, struct pipe_surface *p_surf)
{
   struct crocus_surface *surf = (void *) p_surf;
   pipe_resource_reference(&p_surf->texture, NULL);
   pipe_resource_reference(&surf->surface_state.ref.res, NULL);
   pipe_resource_reference(&surf->surface_state_read.ref.res, NULL);
   free(surf->surface_state.cpu);
   free(surf);
}

static void
crocus_set_clip_state(struct pipe_context *ctx,
                    const struct pipe_clip_state *state)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_shader_state *shs = &ice->state.shaders[MESA_SHADER_VERTEX];
   struct crocus_shader_state *gshs = &ice->state.shaders[MESA_SHADER_GEOMETRY];
   struct crocus_shader_state *tshs = &ice->state.shaders[MESA_SHADER_TESS_EVAL];

   memcpy(&ice->state.clip_planes, state, sizeof(*state));

   ice->state.dirty |= CROCUS_DIRTY_CONSTANTS_VS | CROCUS_DIRTY_CONSTANTS_GS |
                       CROCUS_DIRTY_CONSTANTS_TES;
   shs->sysvals_need_upload = true;
   gshs->sysvals_need_upload = true;
   tshs->sysvals_need_upload = true;
}

/**
 * The pipe->set_polygon_stipple() driver hook.
 */
static void
crocus_set_polygon_stipple(struct pipe_context *ctx,
                         const struct pipe_poly_stipple *state)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   memcpy(&ice->state.poly_stipple, state, sizeof(*state));
   ice->state.dirty |= CROCUS_DIRTY_POLYGON_STIPPLE;
}

/**
 * The pipe->set_sample_mask() driver hook.
 */
static void
crocus_set_sample_mask(struct pipe_context *ctx, unsigned sample_mask)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;

   /* We only support 16x MSAA, so we have 16 bits of sample maks.
    * st/mesa may pass us 0xffffffff though, meaning "enable all samples".
    */
   ice->state.sample_mask = sample_mask & 0xff;
   ice->state.dirty |= CROCUS_DIRTY_SAMPLE_MASK;
}

/**
 * The pipe->set_scissor_states() driver hook.
 *
 * This corresponds to our SCISSOR_RECT state structures.  It's an
 * exact match, so we just store them, and memcpy them out later.
 */
static void
crocus_set_scissor_states(struct pipe_context *ctx,
                        unsigned start_slot,
                        unsigned num_scissors,
                        const struct pipe_scissor_state *rects)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;

   for (unsigned i = 0; i < num_scissors; i++) {
      if (rects[i].minx == rects[i].maxx || rects[i].miny == rects[i].maxy) {
         /* If the scissor was out of bounds and got clamped to 0 width/height
          * at the bounds, the subtraction of 1 from maximums could produce a
          * negative number and thus not clip anything.  Instead, just provide
          * a min > max scissor inside the bounds, which produces the expected
          * no rendering.
          */
         ice->state.scissors[start_slot + i] = (struct pipe_scissor_state) {
            .minx = 1, .maxx = 0, .miny = 1, .maxy = 0,
         };
      } else {
         ice->state.scissors[start_slot + i] = (struct pipe_scissor_state) {
            .minx = rects[i].minx,     .miny = rects[i].miny,
            .maxx = rects[i].maxx - 1, .maxy = rects[i].maxy - 1,
         };
      }
   }

   ice->state.dirty |= CROCUS_DIRTY_SCISSOR_RECT;
   ice->state.dirty |= CROCUS_DIRTY_SF_CL_VIEWPORT;
}

/**
 * The pipe->set_stencil_ref() driver hook.
 *
 * This is added to 3DSTATE_WM_DEPTH_STENCIL dynamically at draw time.
 */
static void
crocus_set_stencil_ref(struct pipe_context *ctx,
                       const struct pipe_stencil_ref ref)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   ice->state.stencil_ref = ref;
   ice->state.dirty |= CROCUS_DIRTY_COLOR_CALC_STATE;
}

static float
viewport_extent(const struct pipe_viewport_state *state, int axis, float sign)
{
   return copysignf(state->scale[axis], sign) + state->translate[axis];
}

/**
 * The pipe->set_viewport_states() driver hook.
 *
 * This corresponds to our SF_CLIP_VIEWPORT states.  We can't calculate
 * the guardband yet, as we need the framebuffer dimensions, but we can
 * at least fill out the rest.
 */
static void
crocus_set_viewport_states(struct pipe_context *ctx,
                         unsigned start_slot,
                         unsigned count,
                         const struct pipe_viewport_state *states)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;

   memcpy(&ice->state.viewports[start_slot], states, sizeof(*states) * count);

   ice->state.dirty |= CROCUS_DIRTY_SF_CL_VIEWPORT;
   ice->state.dirty |= CROCUS_DIRTY_RASTER;

   if (ice->state.cso_rast && (!ice->state.cso_rast->depth_clip_near ||
                               !ice->state.cso_rast->depth_clip_far))
      ice->state.dirty |= CROCUS_DIRTY_CC_VIEWPORT;
}

/**
 * The pipe->set_framebuffer_state() driver hook.
 *
 * Sets the current draw FBO, including color render targets, depth,
 * and stencil buffers.
 */
static void
crocus_set_framebuffer_state(struct pipe_context *ctx,
                           const struct pipe_framebuffer_state *state)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_screen *screen = (struct crocus_screen *)ctx->screen;
   struct isl_device *isl_dev = &screen->isl_dev;
   struct pipe_framebuffer_state *cso = &ice->state.framebuffer;
   struct crocus_resource *zres;
   struct crocus_resource *stencil_res;

   unsigned samples = util_framebuffer_get_num_samples(state);
   unsigned layers = util_framebuffer_get_num_layers(state);

   if (cso->samples != samples) {
      ice->state.dirty |= CROCUS_DIRTY_MULTISAMPLE;
   }

   if (cso->nr_cbufs != state->nr_cbufs) {
      ice->state.dirty |= CROCUS_DIRTY_BLEND_STATE;
   }

   if ((cso->layers == 0) != (layers == 0)) {
      ice->state.dirty |= CROCUS_DIRTY_CLIP;
   }

   if (cso->width != state->width || cso->height != state->height) {
      ice->state.dirty |= CROCUS_DIRTY_SF_CL_VIEWPORT;
      ice->state.dirty |= CROCUS_DIRTY_RASTER;
      ice->state.dirty |= CROCUS_DIRTY_DRAWING_RECTANGLE;
   }

   if (cso->zsbuf || state->zsbuf) {
      ice->state.dirty |= CROCUS_DIRTY_DEPTH_BUFFER;
   }

   /* wm thread dispatch enable */
   ice->state.dirty |= CROCUS_DIRTY_WM;
   util_copy_framebuffer_state(cso, state);
   cso->samples = samples;
   cso->layers = layers;
#if 0
   struct crocus_depth_buffer_state *cso_z = &ice->state.genx->depth_buffer;

   struct isl_view view = {
      .base_level = 0,
      .levels = 1,
      .base_array_layer = 0,
      .array_len = 1,
      .swizzle = ISL_SWIZZLE_IDENTITY,
   };

   struct isl_depth_stencil_hiz_emit_info info = { .view = &view };

   if (cso->zsbuf) {
     crocus_get_depth_stencil_resources(devinfo, cs->zsbuf->texture, &zres,
					&stencil_res);

      view.base_level = cso->zsbuf->u.tex.level;
      view.base_array_layer = cso->zsbuf->u.tex.first_layer;
      view.array_len =
         cso->zsbuf->u.tex.last_layer - cso->zsbuf->u.tex.first_layer + 1;

      if (zres) {
         view.usage |= ISL_SURF_USAGE_DEPTH_BIT;

         info.depth_surf = &zres->surf;
         
         info.depth_address = zres->bo->gtt_offset + zres->offset;
         info.mocs = mocs(zres->bo, isl_dev);

         view.format = zres->surf.format;

         if (crocus_resource_level_has_hiz(zres, view.base_level)) {
            info.hiz_usage = zres->aux.usage;
            info.hiz_surf = &zres->aux.surf;
            info.hiz_address = zres->aux.bo->gtt_offset + zres->aux.offset;
         }
      }

      if (stencil_res) {
         view.usage |= ISL_SURF_USAGE_STENCIL_BIT;
         info.stencil_aux_usage = stencil_res->aux.usage;
         info.stencil_surf = &stencil_res->surf;
         info.stencil_address = stencil_res->bo->gtt_offset + stencil_res->offset;
         if (!zres) {
            view.format = stencil_res->surf.format;
            info.mocs = mocs(stencil_res->bo, isl_dev);
         }
      }
   }

   isl_emit_depth_stencil_hiz_s(isl_dev, cso_z->packets, &info);
#endif
   /* Render target change */
   ice->state.dirty |= CROCUS_DIRTY_BINDINGS_FS;

   ice->state.dirty |= CROCUS_DIRTY_RENDER_RESOLVES_AND_FLUSHES;

   ice->state.dirty |= ice->state.dirty_for_nos[CROCUS_NOS_FRAMEBUFFER];
}

/**
 * The pipe->set_constant_buffer() driver hook.
 *
 * This uploads any constant data in user buffers, and references
 * any UBO resources containing constant data.
 */
static void
crocus_set_constant_buffer(struct pipe_context *ctx,
                           enum pipe_shader_type p_stage, unsigned index,
                           bool take_ownership,
                           const struct pipe_constant_buffer *input)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   gl_shader_stage stage = stage_from_pipe(p_stage);
   struct crocus_shader_state *shs = &ice->state.shaders[stage];
   struct pipe_shader_buffer *cbuf = &shs->constbuf[index];

   /* TODO: Only do this if the buffer changes? */
   pipe_resource_reference(&shs->constbuf_surf_state[index].res, NULL);

   util_copy_constant_buffer(&shs->constbufs[index], input, take_ownership);

   if (input && input->buffer_size && (input->buffer || input->user_buffer)) {
      shs->bound_cbufs |= 1u << index;

      if (input->user_buffer) {
         void *map = NULL;
         pipe_resource_reference(&cbuf->buffer, NULL);
         u_upload_alloc(ice->ctx.const_uploader, 0, input->buffer_size, 64,
                        &cbuf->buffer_offset, &cbuf->buffer, (void **) &map);

         if (!cbuf->buffer) {
            /* Allocation was unsuccessful - just unbind */
            crocus_set_constant_buffer(ctx, p_stage, index, false, NULL);
            return;
         }

         assert(map);
         memcpy(map, input->user_buffer, input->buffer_size);
      } else if (input->buffer) {
         pipe_resource_reference(&cbuf->buffer, input->buffer);

         cbuf->buffer_offset = input->buffer_offset;
      }

      cbuf->buffer_size =
         MIN2(input->buffer_size,
              crocus_resource_bo(cbuf->buffer)->size - cbuf->buffer_offset);

      struct crocus_resource *res = (void *) cbuf->buffer;
      res->bind_history |= PIPE_BIND_CONSTANT_BUFFER;
      res->bind_stages |= 1 << stage;
   } else {
      shs->bound_cbufs &= ~(1u << index);
      pipe_resource_reference(&cbuf->buffer, NULL);
   }

   ice->state.dirty |= CROCUS_DIRTY_CONSTANTS_VS << stage;
}

static void
upload_sysvals(struct crocus_context *ice,
                gl_shader_stage stage)
{
   UNUSED struct crocus_genx_state *genx = ice->state.genx;
   struct crocus_shader_state *shs = &ice->state.shaders[stage];

   struct crocus_compiled_shader *shader = ice->shaders.prog[stage];
   if (!shader || shader->num_system_values == 0)
      return;

   assert(shader->num_cbufs > 0);

   unsigned sysval_cbuf_index = shader->num_cbufs - 1;
   struct pipe_shader_buffer *cbuf = &shs->constbuf[sysval_cbuf_index];
   unsigned upload_size = shader->num_system_values * sizeof(uint32_t);
   uint32_t *map = NULL;

   assert(sysval_cbuf_index < PIPE_MAX_CONSTANT_BUFFERS);
   u_upload_alloc(ice->ctx.const_uploader, 0, upload_size, 64,
                  &cbuf->buffer_offset, &cbuf->buffer, (void **) &map);

   for (int i = 0; i < shader->num_system_values; i++) {
      uint32_t sysval = shader->system_values[i];
      uint32_t value = 0;

      if (BRW_PARAM_DOMAIN(sysval) == BRW_PARAM_DOMAIN_IMAGE) {
#if GEN_GEN == 7
         // TODO
         unsigned img = BRW_PARAM_IMAGE_IDX(sysval);
         unsigned offset = BRW_PARAM_IMAGE_OFFSET(sysval);
         struct brw_image_param *param =
            &genx->shaders[stage].image_param[img];

         assert(offset < sizeof(struct brw_image_param));
         value = ((uint32_t *) param)[offset];
#endif
      } else if (sysval == BRW_PARAM_BUILTIN_ZERO) {
         value = 0;
      } else if (BRW_PARAM_BUILTIN_IS_CLIP_PLANE(sysval)) {
         int plane = BRW_PARAM_BUILTIN_CLIP_PLANE_IDX(sysval);
         int comp  = BRW_PARAM_BUILTIN_CLIP_PLANE_COMP(sysval);
         value = fui(ice->state.clip_planes.ucp[plane][comp]);
      } else if (sysval == BRW_PARAM_BUILTIN_PATCH_VERTICES_IN) {
         if (stage == MESA_SHADER_TESS_CTRL) {
            value = ice->state.vertices_per_patch;
         } else {
            assert(stage == MESA_SHADER_TESS_EVAL);
            const struct shader_info *tcs_info =
               crocus_get_shader_info(ice, MESA_SHADER_TESS_CTRL);
            if (tcs_info)
               value = tcs_info->tess.tcs_vertices_out;
            else
               value = ice->state.vertices_per_patch;
         }
      } else if (sysval >= BRW_PARAM_BUILTIN_TESS_LEVEL_OUTER_X &&
                 sysval <= BRW_PARAM_BUILTIN_TESS_LEVEL_OUTER_W) {
         unsigned i = sysval - BRW_PARAM_BUILTIN_TESS_LEVEL_OUTER_X;
         value = fui(ice->state.default_outer_level[i]);
      } else if (sysval == BRW_PARAM_BUILTIN_TESS_LEVEL_INNER_X) {
         value = fui(ice->state.default_inner_level[0]);
      } else if (sysval == BRW_PARAM_BUILTIN_TESS_LEVEL_INNER_Y) {
         value = fui(ice->state.default_inner_level[1]);
      } else {
         assert(!"unhandled system value");
      }

      *map++ = value;
   }

   cbuf->buffer_size = upload_size;
   crocus_upload_ubo_ssbo_surf_state(ice, cbuf,
                                   &shs->constbuf_surf_state[sysval_cbuf_index], false);

   shs->sysvals_need_upload = false;
}

/**
 * The pipe->set_shader_buffers() driver hook.
 *
 * This binds SSBOs and ABOs.  Unfortunately, we need to stream out
 * SURFACE_STATE here, as the buffer offset may change each time.
 */
static void
crocus_set_shader_buffers(struct pipe_context *ctx,
                        enum pipe_shader_type p_stage,
                        unsigned start_slot, unsigned count,
                        const struct pipe_shader_buffer *buffers,
                        unsigned writable_bitmask)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   gl_shader_stage stage = stage_from_pipe(p_stage);
   struct crocus_shader_state *shs = &ice->state.shaders[stage];

   unsigned modified_bits = u_bit_consecutive(start_slot, count);

   shs->bound_ssbos &= ~modified_bits;
   shs->writable_ssbos &= ~modified_bits;
   shs->writable_ssbos |= writable_bitmask << start_slot;

   for (unsigned i = 0; i < count; i++) {
      if (buffers && buffers[i].buffer) {
         struct crocus_resource *res = (void *) buffers[i].buffer;
         struct pipe_shader_buffer *ssbo = &shs->ssbo[start_slot + i];
         struct crocus_state_ref *surf_state =
            &shs->ssbo_surf_state[start_slot + i];
         pipe_resource_reference(&ssbo->buffer, &res->base);
         ssbo->buffer_offset = buffers[i].buffer_offset;
         ssbo->buffer_size =
            MIN2(buffers[i].buffer_size, res->bo->size - ssbo->buffer_offset);

         shs->bound_ssbos |= 1 << (start_slot + i);

         crocus_upload_ubo_ssbo_surf_state(ice, ssbo, surf_state, true);

         res->bind_history |= PIPE_BIND_SHADER_BUFFER;
         res->bind_stages |= 1 << stage;

         util_range_add(&res->base, &res->valid_buffer_range, ssbo->buffer_offset,
                        ssbo->buffer_offset + ssbo->buffer_size);
      } else {
         pipe_resource_reference(&shs->ssbo[start_slot + i].buffer, NULL);
         pipe_resource_reference(&shs->ssbo_surf_state[start_slot + i].res,
                                 NULL);
      }
   }

   ice->state.dirty |= CROCUS_DIRTY_BINDINGS_VS << stage;
}

static void
crocus_delete_state(struct pipe_context *ctx, void *state)
{
   free(state);
}

/**
 * The pipe->set_vertex_buffers() driver hook.
 *
 * This translates pipe_vertex_buffer to our 3DSTATE_VERTEX_BUFFERS packet.
 */
static void
crocus_set_vertex_buffers(struct pipe_context *ctx,
                          unsigned start_slot, unsigned count,
                          unsigned unbind_num_trailing_slots,
                          bool take_ownership,
                          const struct pipe_vertex_buffer *buffers)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_screen *screen = (struct crocus_screen *)ctx->screen;

   util_set_vertex_buffers_mask(ice->state.vertex_buffers, &ice->state.bound_vertex_buffers,
                                buffers, start_slot, count, unbind_num_trailing_slots,
                                take_ownership);

//   res->bind_history |= PIPE_BIND_VERTEX_BUFFER;   
   ice->state.dirty |= CROCUS_DIRTY_VERTEX_BUFFERS;
}

/**
 * Gallium CSO for vertex elements.
 */
struct crocus_vertex_element_state {
   uint32_t vertex_elements[1 + 33 * GENX(VERTEX_ELEMENT_STATE_length)];
   uint32_t edgeflag_ve[GENX(VERTEX_ELEMENT_STATE_length)];
   uint32_t instance_divisor[33];
   uint32_t vbo_index[33];
   enum isl_format fmt[33]; // workarounds
   unsigned count;
};

/**
 * The pipe->create_vertex_elements() driver hook.
 *
 * This translates pipe_vertex_element to our 3DSTATE_VERTEX_ELEMENTS
 * and 3DSTATE_VF_INSTANCING commands. The vertex_elements and vf_instancing
 * arrays are ready to be emitted at draw time if no EdgeFlag or SGVs are
 * needed. In these cases we will need information available at draw time.
 * We setup edgeflag_ve and edgeflag_vfi as alternatives last
 * 3DSTATE_VERTEX_ELEMENT and 3DSTATE_VF_INSTANCING that can be used at
 * draw time if we detect that EdgeFlag is needed by the Vertex Shader.
 */
static void *
crocus_create_vertex_elements(struct pipe_context *ctx,
                            unsigned count,
                            const struct pipe_vertex_element *state)
{
   struct crocus_screen *screen = (struct crocus_screen *)ctx->screen;
   const struct gen_device_info *devinfo = &screen->devinfo;
   struct crocus_vertex_element_state *cso =
      malloc(sizeof(struct crocus_vertex_element_state));

   cso->count = count;

   crocus_pack_command(GENX(3DSTATE_VERTEX_ELEMENTS), cso->vertex_elements, ve) {
      ve.DWordLength =
         1 + GENX(VERTEX_ELEMENT_STATE_length) * MAX2(count, 1) - 2;
   }

   uint32_t *ve_pack_dest = &cso->vertex_elements[1];

   if (count == 0) {
      crocus_pack_state(GENX(VERTEX_ELEMENT_STATE), ve_pack_dest, ve) {
         ve.Valid = true;
         ve.SourceElementFormat = ISL_FORMAT_R32G32B32A32_FLOAT;
         ve.Component0Control = VFCOMP_STORE_0;
         ve.Component1Control = VFCOMP_STORE_0;
         ve.Component2Control = VFCOMP_STORE_0;
         ve.Component3Control = VFCOMP_STORE_1_FP;
      }
   }

   for (int i = 0; i < count; i++) {
      const struct crocus_format_info fmt =
         crocus_format_for_usage(devinfo, state[i].src_format, 0);
      unsigned comp[4] = { VFCOMP_STORE_SRC, VFCOMP_STORE_SRC,
                           VFCOMP_STORE_SRC, VFCOMP_STORE_SRC };
      enum isl_format actual_fmt = fmt.fmt;
      cso->fmt[i] = fmt.fmt;

#if !(GEN_VERSIONx10 == 75)
      if (fmt.fmt == ISL_FORMAT_R10G10B10A2_USCALED ||
          fmt.fmt == ISL_FORMAT_R10G10B10A2_SSCALED ||
          fmt.fmt == ISL_FORMAT_R10G10B10A2_UNORM ||
          fmt.fmt == ISL_FORMAT_R10G10B10A2_SNORM ||
          fmt.fmt == ISL_FORMAT_R10G10B10A2_SINT ||
          fmt.fmt == ISL_FORMAT_B10G10R10A2_USCALED ||
          fmt.fmt == ISL_FORMAT_B10G10R10A2_SSCALED ||
          fmt.fmt == ISL_FORMAT_B10G10R10A2_UNORM ||
          fmt.fmt == ISL_FORMAT_B10G10R10A2_SNORM ||
          fmt.fmt == ISL_FORMAT_B10G10R10A2_UINT ||
          fmt.fmt == ISL_FORMAT_B10G10R10A2_SINT)
         actual_fmt = ISL_FORMAT_R10G10B10A2_UINT;
#endif

      cso->vbo_index[i] = state[i].vertex_buffer_index;
      cso->instance_divisor[i] = state[i].instance_divisor;
      switch (isl_format_get_num_channels(actual_fmt)) {
      case 0: comp[0] = VFCOMP_STORE_0; /* fallthrough */
      case 1: comp[1] = VFCOMP_STORE_0; /* fallthrough */
      case 2: comp[2] = VFCOMP_STORE_0; /* fallthrough */
      case 3:
         comp[3] = isl_format_has_int_channel(fmt.fmt) ? VFCOMP_STORE_1_INT
                                                       : VFCOMP_STORE_1_FP;
         break;
      }
      crocus_pack_state(GENX(VERTEX_ELEMENT_STATE), ve_pack_dest, ve) {
#if GEN_GEN >= 6
         ve.EdgeFlagEnable = false;
#endif
         ve.VertexBufferIndex = state[i].vertex_buffer_index;
         ve.Valid = true;
         ve.SourceElementOffset = state[i].src_offset;
         ve.SourceElementFormat = actual_fmt;
         ve.Component0Control = comp[0];
         ve.Component1Control = comp[1];
         ve.Component2Control = comp[2];
         ve.Component3Control = comp[3];
#if GEN_GEN < 5
         ve.DestinationElementOffset = i * 4;
#endif
      }

      ve_pack_dest += GENX(VERTEX_ELEMENT_STATE_length);
   }

   /* An alternative version of the last VE and VFI is stored so it
    * can be used at draw time in case Vertex Shader uses EdgeFlag
    */
   if (count) {
      const unsigned edgeflag_index = count - 1;
      const struct crocus_format_info fmt =
         crocus_format_for_usage(devinfo, state[edgeflag_index].src_format, 0);
      crocus_pack_state(GENX(VERTEX_ELEMENT_STATE), cso->edgeflag_ve, ve) {
#if GEN_GEN >= 6
         ve.EdgeFlagEnable = true ;
#endif
         ve.VertexBufferIndex = state[edgeflag_index].vertex_buffer_index;
         ve.Valid = true;
         ve.SourceElementOffset = state[edgeflag_index].src_offset;
         ve.SourceElementFormat = fmt.fmt;
         ve.Component0Control = VFCOMP_STORE_SRC;
         ve.Component1Control = VFCOMP_STORE_0;
         ve.Component2Control = VFCOMP_STORE_0;
         ve.Component3Control = VFCOMP_STORE_0;
      }
   }

   return cso;
}

/**
 * The pipe->bind_vertex_elements_state() driver hook.
 */
static void
crocus_bind_vertex_elements_state(struct pipe_context *ctx, void *state)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_vertex_element_state *old_cso = ice->state.cso_vertex_elements;
   struct crocus_vertex_element_state *new_cso = state;

   /* 3DSTATE_VF_SGVs overrides the last VE, so if the count is changing,
    * we need to re-emit it to ensure we're overriding the right one.
    */
/*
   if (new_cso && cso_changed(count))
      ice->state.dirty |= CROCUS_DIRTY_VF_SGVS;
*/
   ice->state.cso_vertex_elements = state;
   ice->state.dirty |= CROCUS_DIRTY_VERTEX_ELEMENTS;
#if !(GEN_VERSIONx10 == 75)
   ice->state.dirty |= CROCUS_DIRTY_UNCOMPILED_VS;
#endif
}

#if GEN_GEN >= 7
/**
 * The pipe->create_stream_output_target() driver hook.
 *
 * "Target" here refers to a destination buffer.  We translate this into
 * a 3DSTATE_SO_BUFFER packet.  We can handle most fields, but don't yet
 * know which buffer this represents, or whether we ought to zero the
 * write-offsets, or append.  Those are handled in the set() hook.
 */
static struct pipe_stream_output_target *
crocus_create_stream_output_target(struct pipe_context *ctx,
                                 struct pipe_resource *p_res,
                                 unsigned buffer_offset,
                                 unsigned buffer_size)
{
   struct crocus_resource *res = (void *) p_res;
   struct crocus_stream_output_target *cso = calloc(1, sizeof(*cso));
   if (!cso)
      return NULL;

   res->bind_history |= PIPE_BIND_STREAM_OUTPUT;

   pipe_reference_init(&cso->base.reference, 1);
   pipe_resource_reference(&cso->base.buffer, p_res);
   cso->base.buffer_offset = buffer_offset;
   cso->base.buffer_size = buffer_size;
   cso->base.context = ctx;

   util_range_add(&res->base, &res->valid_buffer_range, buffer_offset,
                  buffer_offset + buffer_size);

   upload_state(ctx->stream_uploader, &cso->offset, sizeof(uint32_t), 4);

   return &cso->base;
}

static void
crocus_stream_output_target_destroy(struct pipe_context *ctx,
                                  struct pipe_stream_output_target *state)
{
   struct crocus_stream_output_target *cso = (void *) state;

   pipe_resource_reference(&cso->base.buffer, NULL);
   pipe_resource_reference(&cso->offset.res, NULL);

   free(cso);
}

/**
 * The pipe->set_stream_output_targets() driver hook.
 *
 * At this point, we know which targets are bound to a particular index,
 * and also whether we want to append or start over.  We can finish the
 * 3DSTATE_SO_BUFFER packets we started earlier.
 */
static void
crocus_set_stream_output_targets(struct pipe_context *ctx,
                               unsigned num_targets,
                               struct pipe_stream_output_target **targets,
                               const unsigned *offsets)
{
   struct crocus_context *ice = (struct crocus_context *) ctx;
   struct crocus_genx_state *genx = ice->state.genx;
   uint32_t *so_buffers = genx->so_buffers;
   struct crocus_screen *screen = (struct crocus_screen *)ctx->screen;

   const bool active = num_targets > 0;
   if (ice->state.streamout_active != active) {
      ice->state.streamout_active = active;
      ice->state.dirty |= CROCUS_DIRTY_STREAMOUT;

      /* We only emit 3DSTATE_SO_DECL_LIST when streamout is active, because
       * it's a non-pipelined command.  If we're switching streamout on, we
       * may have missed emitting it earlier, so do so now.  (We're already
       * taking a stall to update 3DSTATE_SO_BUFFERS anyway...)
       */
      if (active) {
         ice->state.dirty |= CROCUS_DIRTY_SO_DECL_LIST;
      } else {
         uint32_t flush = 0;
         for (int i = 0; i < PIPE_MAX_SO_BUFFERS; i++) {
            struct crocus_stream_output_target *tgt =
               (void *) ice->state.so_target[i];
            if (tgt) {
               struct crocus_resource *res = (void *) tgt->base.buffer;

               flush |= crocus_flush_bits_for_history(res);
               crocus_dirty_for_history(ice, res);
            }
         }
         crocus_emit_pipe_control_flush(&ice->batches[CROCUS_BATCH_RENDER],
                                      "make streamout results visible", flush);
      }
   }

   ice->state.so_targets = num_targets;
   for (int i = 0; i < 4; i++) {
      pipe_so_target_reference(&ice->state.so_target[i],
                               i < num_targets ? targets[i] : NULL);
   }

   /* No need to update 3DSTATE_SO_BUFFER unless SOL is active. */
   if (!active)
      return;

   for (unsigned i = 0; i < 4; i++,
        so_buffers += GENX(3DSTATE_SO_BUFFER_length)) {

      struct crocus_stream_output_target *tgt = (void *) ice->state.so_target[i];
      unsigned offset = offsets[i];

      if (!tgt) {
         crocus_pack_command(GENX(3DSTATE_SO_BUFFER), so_buffers, sob) {
            sob.SOBufferIndex = i;
         }
         continue;
      }

      struct crocus_resource *res = (void *) tgt->base.buffer;

      /* Note that offsets[i] will either be 0, causing us to zero
       * the value in the buffer, or 0xFFFFFFFF, which happens to mean
       * "continue appending at the existing offset."
       */
      assert(offset == 0 || offset == 0xFFFFFFFF);

      /* We might be called by Begin (offset = 0), Pause, then Resume
       * (offset = 0xFFFFFFFF) before ever drawing (where these commands
       * will actually be sent to the GPU).  In this case, we don't want
       * to append - we still want to do our initial zeroing.
       */
      if (!tgt->zeroed)
         offset = 0;
      /* REMOVE 3D state so buffer from here */
   }

   ice->state.dirty |= CROCUS_DIRTY_SO_BUFFERS;
}

/**
 * An crocus-vtable helper for encoding the 3DSTATE_SO_DECL_LIST and
 * 3DSTATE_STREAMOUT packets.
 *
 * 3DSTATE_SO_DECL_LIST is a list of shader outputs we want the streamout
 * hardware to record.  We can create it entirely based on the shader, with
 * no dynamic state dependencies.
 *
 * 3DSTATE_STREAMOUT is an annoying mix of shader-based information and
 * state-based settings.  We capture the shader-related ones here, and merge
 * the rest in at draw time.
 */
static uint32_t *
crocus_create_so_decl_list(const struct pipe_stream_output_info *info,
                         const struct brw_vue_map *vue_map)
{
   struct GENX(SO_DECL) so_decl[MAX_VERTEX_STREAMS][128];
   int buffer_mask[MAX_VERTEX_STREAMS] = {0, 0, 0, 0};
   int next_offset[MAX_VERTEX_STREAMS] = {0, 0, 0, 0};
   int decls[MAX_VERTEX_STREAMS] = {0, 0, 0, 0};
   int max_decls = 0;
   STATIC_ASSERT(ARRAY_SIZE(so_decl[0]) >= MAX_PROGRAM_OUTPUTS);

   memset(so_decl, 0, sizeof(so_decl));

   /* Construct the list of SO_DECLs to be emitted.  The formatting of the
    * command feels strange -- each dword pair contains a SO_DECL per stream.
    */
   for (unsigned i = 0; i < info->num_outputs; i++) {
      const struct pipe_stream_output *output = &info->output[i];
      const int buffer = output->output_buffer;
      const int varying = output->register_index;
      const unsigned stream_id = output->stream;
      assert(stream_id < MAX_VERTEX_STREAMS);

      buffer_mask[stream_id] |= 1 << buffer;

      assert(vue_map->varying_to_slot[varying] >= 0);

      /* Mesa doesn't store entries for gl_SkipComponents in the Outputs[]
       * array.  Instead, it simply increments DstOffset for the following
       * input by the number of components that should be skipped.
       *
       * Our hardware is unusual in that it requires us to program SO_DECLs
       * for fake "hole" components, rather than simply taking the offset
       * for each real varying.  Each hole can have size 1, 2, 3, or 4; we
       * program as many size = 4 holes as we can, then a final hole to
       * accommodate the final 1, 2, or 3 remaining.
       */
      int skip_components = output->dst_offset - next_offset[buffer];

      while (skip_components > 0) {
         so_decl[stream_id][decls[stream_id]++] = (struct GENX(SO_DECL)) {
            .HoleFlag = 1,
            .OutputBufferSlot = output->output_buffer,
            .ComponentMask = (1 << MIN2(skip_components, 4)) - 1,
         };
         skip_components -= 4;
      }

      next_offset[buffer] = output->dst_offset + output->num_components;

      so_decl[stream_id][decls[stream_id]++] = (struct GENX(SO_DECL)) {
         .OutputBufferSlot = output->output_buffer,
         .RegisterIndex = vue_map->varying_to_slot[varying],
         .ComponentMask =
            ((1 << output->num_components) - 1) << output->start_component,
      };

      if (decls[stream_id] > max_decls)
         max_decls = decls[stream_id];
   }

   unsigned dwords = GENX(3DSTATE_STREAMOUT_length) + (3 + 2 * max_decls);
   uint32_t *map = ralloc_size(NULL, sizeof(uint32_t) * dwords);
   uint32_t *so_decl_map = map + GENX(3DSTATE_STREAMOUT_length);

   crocus_pack_command(GENX(3DSTATE_STREAMOUT), map, sol) {
      int urb_entry_read_offset = 0;
      int urb_entry_read_length = (vue_map->num_slots + 1) / 2 -
         urb_entry_read_offset;

      /* We always read the whole vertex.  This could be reduced at some
       * point by reading less and offsetting the register index in the
       * SO_DECLs.
       */
      sol.Stream0VertexReadOffset = urb_entry_read_offset;
      sol.Stream0VertexReadLength = urb_entry_read_length - 1;
      sol.Stream1VertexReadOffset = urb_entry_read_offset;
      sol.Stream1VertexReadLength = urb_entry_read_length - 1;
      sol.Stream2VertexReadOffset = urb_entry_read_offset;
      sol.Stream2VertexReadLength = urb_entry_read_length - 1;
      sol.Stream3VertexReadOffset = urb_entry_read_offset;
      sol.Stream3VertexReadLength = urb_entry_read_length - 1;

      // TODO: Double-check that stride == 0 means no buffer. Probably this
      // needs to go elsewhere, where the buffer enable stuff is actually
      // known.
      sol.SOBufferEnable0 = !!info->stride[0];
      sol.SOBufferEnable1 = !!info->stride[1];
      sol.SOBufferEnable2 = !!info->stride[2];
      sol.SOBufferEnable3 = !!info->stride[3];
   }

   crocus_pack_command(GENX(3DSTATE_SO_DECL_LIST), so_decl_map, list) {
      list.DWordLength = 3 + 2 * max_decls - 2;
      list.StreamtoBufferSelects0 = buffer_mask[0];
      list.StreamtoBufferSelects1 = buffer_mask[1];
      list.StreamtoBufferSelects2 = buffer_mask[2];
      list.StreamtoBufferSelects3 = buffer_mask[3];
      list.NumEntries0 = decls[0];
      list.NumEntries1 = decls[1];
      list.NumEntries2 = decls[2];
      list.NumEntries3 = decls[3];
   }

   for (int i = 0; i < max_decls; i++) {
      crocus_pack_state(GENX(SO_DECL_ENTRY), so_decl_map + 3 + i * 2, entry) {
         entry.Stream0Decl = so_decl[0][i];
         entry.Stream1Decl = so_decl[1][i];
         entry.Stream2Decl = so_decl[2][i];
         entry.Stream3Decl = so_decl[3][i];
      }
   }

   return map;
}
#endif

static void
crocus_compute_sbe_urb_read_interval(uint64_t fs_input_slots,
                                   const struct brw_vue_map *last_vue_map,
                                   bool two_sided_color,
                                   unsigned *out_offset,
                                   unsigned *out_length)
{
   /* The compiler computes the first URB slot without considering COL/BFC
    * swizzling (because it doesn't know whether it's enabled), so we need
    * to do that here too.  This may result in a smaller offset, which
    * should be safe.
    */
   const unsigned first_slot =
      brw_compute_first_urb_slot_required(fs_input_slots, last_vue_map);

   /* This becomes the URB read offset (counted in pairs of slots). */
   assert(first_slot % 2 == 0);
   *out_offset = first_slot / 2;

   /* We need to adjust the inputs read to account for front/back color
    * swizzling, as it can make the URB length longer.
    */
   for (int c = 0; c <= 1; c++) {
      if (fs_input_slots & (VARYING_BIT_COL0 << c)) {
         /* If two sided color is enabled, the fragment shader's gl_Color
          * (COL0) input comes from either the gl_FrontColor (COL0) or
          * gl_BackColor (BFC0) input varyings.  Mark BFC as used, too.
          */
         if (two_sided_color)
            fs_input_slots |= (VARYING_BIT_BFC0 << c);

         /* If front color isn't written, we opt to give them back color
          * instead of an undefined value.  Switch from COL to BFC.
          */
         if (last_vue_map->varying_to_slot[VARYING_SLOT_COL0 + c] == -1) {
            fs_input_slots &= ~(VARYING_BIT_COL0 << c);
            fs_input_slots |= (VARYING_BIT_BFC0 << c);
         }
      }
   }

   /* Compute the minimum URB Read Length necessary for the FS inputs.
    *
    * From the Sandy Bridge PRM, Volume 2, Part 1, documentation for
    * 3DSTATE_SF DWord 1 bits 15:11, "Vertex URB Entry Read Length":
    *
    * "This field should be set to the minimum length required to read the
    *  maximum source attribute.  The maximum source attribute is indicated
    *  by the maximum value of the enabled Attribute # Source Attribute if
    *  Attribute Swizzle Enable is set, Number of Output Attributes-1 if
    *  enable is not set.
    *  read_length = ceiling((max_source_attr + 1) / 2)
    *
    *  [errata] Corruption/Hang possible if length programmed larger than
    *  recommended"
    *
    * Similar text exists for Ivy Bridge.
    *
    * We find the last URB slot that's actually read by the FS.
    */
   unsigned last_read_slot = last_vue_map->num_slots - 1;
   while (last_read_slot > first_slot && !(fs_input_slots &
          (1ull << last_vue_map->slot_to_varying[last_read_slot])))
      --last_read_slot;

   /* The URB read length is the difference of the two, counted in pairs. */
   *out_length = DIV_ROUND_UP(last_read_slot - first_slot + 1, 2);
}

#if GEN_GEN >= 6

static void
get_attr_override(
   struct GENX(SF_OUTPUT_ATTRIBUTE_DETAIL) *attr,
   const struct brw_vue_map *vue_map,
   int urb_entry_read_offset, int fs_attr,
   bool two_side_color, uint32_t *max_source_attr)
{
   /* Find the VUE slot for this attribute. */
   int slot = vue_map->varying_to_slot[fs_attr];

   /* Viewport and Layer are stored in the VUE header.  We need to override
    * them to zero if earlier stages didn't write them, as GL requires that
    * they read back as zero when not explicitly set.
    */
   if (fs_attr == VARYING_SLOT_VIEWPORT || fs_attr == VARYING_SLOT_LAYER) {
      attr->ComponentOverrideX = true;
      attr->ComponentOverrideW = true;
      attr->ConstantSource = CONST_0000;

      if (!(vue_map->slots_valid & VARYING_BIT_LAYER))
         attr->ComponentOverrideY = true;
      if (!(vue_map->slots_valid & VARYING_BIT_VIEWPORT))
         attr->ComponentOverrideZ = true;

      return;
   }

   /* If there was only a back color written but not front, use back
    * as the color instead of undefined
    */
   if (slot == -1 && fs_attr == VARYING_SLOT_COL0)
      slot = vue_map->varying_to_slot[VARYING_SLOT_BFC0];
   if (slot == -1 && fs_attr == VARYING_SLOT_COL1)
      slot = vue_map->varying_to_slot[VARYING_SLOT_BFC1];

   if (slot == -1) {
      /* This attribute does not exist in the VUE--that means that the vertex
       * shader did not write to it.  This means that either:
       *
       * (a) This attribute is a texture coordinate, and it is going to be
       * replaced with point coordinates (as a consequence of a call to
       * glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE)), so the
       * hardware will ignore whatever attribute override we supply.
       *
       * (b) This attribute is read by the fragment shader but not written by
       * the vertex shader, so its value is undefined.  Therefore the
       * attribute override we supply doesn't matter.
       *
       * (c) This attribute is gl_PrimitiveID, and it wasn't written by the
       * previous shader stage.
       *
       * Note that we don't have to worry about the cases where the attribute
       * is gl_PointCoord or is undergoing point sprite coordinate
       * replacement, because in those cases, this function isn't called.
       *
       * In case (c), we need to program the attribute overrides so that the
       * primitive ID will be stored in this slot.  In every other case, the
       * attribute override we supply doesn't matter.  So just go ahead and
       * program primitive ID in every case.
       */
      attr->ComponentOverrideW = true;
      attr->ComponentOverrideX = true;
      attr->ComponentOverrideY = true;
      attr->ComponentOverrideZ = true;
      attr->ConstantSource = PRIM_ID;
      return;
   }

   /* Compute the location of the attribute relative to urb_entry_read_offset.
    * Each increment of urb_entry_read_offset represents a 256-bit value, so
    * it counts for two 128-bit VUE slots.
    */
   int source_attr = slot - 2 * urb_entry_read_offset;
   assert(source_attr >= 0 && source_attr < 32);

   /* If we are doing two-sided color, and the VUE slot following this one
    * represents a back-facing color, then we need to instruct the SF unit to
    * do back-facing swizzling.
    */
   bool swizzling = two_side_color &&
      ((vue_map->slot_to_varying[slot] == VARYING_SLOT_COL0 &&
        vue_map->slot_to_varying[slot+1] == VARYING_SLOT_BFC0) ||
       (vue_map->slot_to_varying[slot] == VARYING_SLOT_COL1 &&
        vue_map->slot_to_varying[slot+1] == VARYING_SLOT_BFC1));

   /* Update max_source_attr.  If swizzling, the SF will read this slot + 1. */
   if (*max_source_attr < source_attr + swizzling)
      *max_source_attr = source_attr + swizzling;

   attr->SourceAttribute = source_attr;
   if (swizzling)
      attr->SwizzleSelect = INPUTATTR_FACING;
}


static void
calculate_attr_overrides(
   const struct crocus_context *ice,
   struct GENX(SF_OUTPUT_ATTRIBUTE_DETAIL) *attr_overrides,
   uint32_t *point_sprite_enables,
   uint32_t *urb_entry_read_length,
   uint32_t *urb_entry_read_offset)
{
   const struct brw_wm_prog_data *wm_prog_data = (void *)
      ice->shaders.prog[MESA_SHADER_FRAGMENT]->prog_data;
   const struct brw_vue_map *vue_map = ice->shaders.last_vue_map;
   const struct crocus_rasterizer_state *cso_rast = ice->state.cso_rast;
   uint32_t max_source_attr = 0;
   const struct shader_info *fs_info =
      crocus_get_shader_info(ice, MESA_SHADER_FRAGMENT);

   int first_slot =
      brw_compute_first_urb_slot_required(fs_info->inputs_read, vue_map);

   /* Each URB offset packs two varying slots */
   assert(first_slot % 2 == 0);
   *urb_entry_read_offset = first_slot / 2;
   *point_sprite_enables = 0;

   for (int fs_attr = 0; fs_attr < VARYING_SLOT_MAX; fs_attr++) {
      const int input_index = wm_prog_data->urb_setup[fs_attr];

      if (input_index < 0)
         continue;

      // TODO: set point_sprite_enables, but ONLY if drawing points
      bool point_sprite = false;
      if (0) {
         if (fs_attr >= VARYING_SLOT_TEX0 &&
             fs_attr <= VARYING_SLOT_TEX7 &&
             cso_rast->sprite_coord_enable & (1 << (fs_attr - VARYING_SLOT_TEX0)))
            point_sprite = true;

         if (fs_attr == VARYING_SLOT_PNTC)
            point_sprite = true;

         if (point_sprite)
            *point_sprite_enables |= 1U << input_index;
      }

      struct GENX(SF_OUTPUT_ATTRIBUTE_DETAIL) attribute = { 0 };
      if (!point_sprite) {
         get_attr_override(&attribute, vue_map, *urb_entry_read_offset, fs_attr,
                           cso_rast->light_twoside, &max_source_attr);
      }

      /* The hardware can only do the overrides on 16 overrides at a
       * time, and the other up to 16 have to be lined up so that the
       * input index = the output index.  We'll need to do some
       * tweaking to make sure that's the case.
       */
      if (input_index < 16)
         attr_overrides[input_index] = attribute;
      else
         assert(attribute.SourceAttribute == input_index);
   }

   /* From the Sandy Bridge PRM, Volume 2, Part 1, documentation for
    * 3DSTATE_SF DWord 1 bits 15:11, "Vertex URB Entry Read Length":
    *
    * "This field should be set to the minimum length required to read the
    *  maximum source attribute.  The maximum source attribute is indicated
    *  by the maximum value of the enabled Attribute # Source Attribute if
    *  Attribute Swizzle Enable is set, Number of Output Attributes-1 if
    *  enable is not set.
    *  read_length = ceiling((max_source_attr + 1) / 2)
    *
    *  [errata] Corruption/Hang possible if length programmed larger than
    *  recommended"
    *
    * Similar text exists for Ivy Bridge.
    */
   *urb_entry_read_length = DIV_ROUND_UP(max_source_attr + 1, 2);
}
#endif

static unsigned
crocus_calculate_point_sprite_overrides(const struct brw_wm_prog_data *prog_data,
                                      const struct crocus_rasterizer_state *cso)
{
   unsigned overrides = 0;

   if (prog_data->urb_setup[VARYING_SLOT_PNTC] != -1)
      overrides |= 1 << prog_data->urb_setup[VARYING_SLOT_PNTC];

   for (int i = 0; i < 8; i++) {
      if ((cso->sprite_coord_enable & (1 << i)) &&
          prog_data->urb_setup[VARYING_SLOT_TEX0 + i] != -1)
         overrides |= 1 << prog_data->urb_setup[VARYING_SLOT_TEX0 + i];
   }

   return overrides;
}

#if GEN_GEN == 7
static void
crocus_emit_sbe(struct crocus_batch *batch, const struct crocus_context *ice)
{
   const struct crocus_rasterizer_state *cso_rast = ice->state.cso_rast;
   const struct brw_wm_prog_data *wm_prog_data = (void *)
      ice->shaders.prog[MESA_SHADER_FRAGMENT]->prog_data;
   const struct shader_info *fs_info =
      crocus_get_shader_info(ice, MESA_SHADER_FRAGMENT);

   // TODO Use calculate_attr_overrides instead of these.
   unsigned urb_read_offset, urb_read_length;
   crocus_compute_sbe_urb_read_interval(fs_info->inputs_read,
                                      ice->shaders.last_vue_map,
                                      cso_rast->light_twoside,
                                      &urb_read_offset, &urb_read_length);

   unsigned sprite_coord_overrides =
      crocus_calculate_point_sprite_overrides(wm_prog_data, cso_rast);

   crocus_emit_cmd(batch, GENX(3DSTATE_SBE), sbe) {
      sbe.AttributeSwizzleEnable = true;
      sbe.NumberofSFOutputAttributes = wm_prog_data->num_varying_inputs;
      sbe.PointSpriteTextureCoordinateOrigin = cso_rast->sprite_coord_mode;
      sbe.VertexURBEntryReadOffset = urb_read_offset;
      sbe.VertexURBEntryReadLength = urb_read_length;
      sbe.ConstantInterpolationEnable = wm_prog_data->flat_inputs;
      sbe.PointSpriteTextureCoordinateEnable = sprite_coord_overrides;
   }
}
#endif

/* ------------------------------------------------------------------- */

static uint8_t get_wa_flags(enum isl_format format)
{
   uint8_t wa_flags = 0;

   switch (format) {
   case ISL_FORMAT_R10G10B10A2_USCALED:
      wa_flags = BRW_ATTRIB_WA_SCALE;
      break;
   case ISL_FORMAT_R10G10B10A2_SSCALED:
      wa_flags = BRW_ATTRIB_WA_SIGN | BRW_ATTRIB_WA_SCALE;
      break;
   case ISL_FORMAT_R10G10B10A2_UNORM:
      wa_flags = BRW_ATTRIB_WA_NORMALIZE;
      break;
   case ISL_FORMAT_R10G10B10A2_SNORM:
      wa_flags = BRW_ATTRIB_WA_SIGN | BRW_ATTRIB_WA_NORMALIZE;
      break;
   case ISL_FORMAT_R10G10B10A2_SINT:
      wa_flags = BRW_ATTRIB_WA_SIGN;
      break;
   case ISL_FORMAT_B10G10R10A2_USCALED:
      wa_flags = BRW_ATTRIB_WA_SCALE | BRW_ATTRIB_WA_BGRA;
      break;
   case ISL_FORMAT_B10G10R10A2_SSCALED:
      wa_flags = BRW_ATTRIB_WA_SIGN | BRW_ATTRIB_WA_SCALE | BRW_ATTRIB_WA_BGRA;
      break;
   case ISL_FORMAT_B10G10R10A2_UNORM:
      wa_flags = BRW_ATTRIB_WA_NORMALIZE | BRW_ATTRIB_WA_BGRA;
      break;
   case ISL_FORMAT_B10G10R10A2_SNORM:
      wa_flags = BRW_ATTRIB_WA_SIGN | BRW_ATTRIB_WA_NORMALIZE | BRW_ATTRIB_WA_BGRA;
      break;
   case ISL_FORMAT_B10G10R10A2_SINT:
      wa_flags = BRW_ATTRIB_WA_SIGN | BRW_ATTRIB_WA_BGRA;
      break;
   case ISL_FORMAT_B10G10R10A2_UINT:
      wa_flags = BRW_ATTRIB_WA_BGRA;
      break;
   default:
      break;
   }
   return wa_flags;
}

/**
 * Populate VS program key fields based on the current state.
 */
static void
crocus_populate_vs_key(const struct crocus_context *ice,
                     const struct shader_info *info,
                     gl_shader_stage last_stage,
                     struct brw_vs_prog_key *key)
{
   const struct crocus_rasterizer_state *cso_rast = ice->state.cso_rast;

   if (info->clip_distance_array_size == 0 &&
       (info->outputs_written & (VARYING_BIT_POS | VARYING_BIT_CLIP_VERTEX)) &&
       last_stage == MESA_SHADER_VERTEX)
      key->nr_userclip_plane_consts = cso_rast->num_clip_plane_consts;

#if GEN_GEN <= 5
   key->copy_edgeflag = (cso_rast->state.fill_front != PIPE_POLYGON_MODE_FILL ||
                         cso_rast->state.fill_back != PIPE_POLYGON_MODE_FILL);

   key->point_coord_replace = cso_rast->sprite_coord_enable & 0xff;
#endif

   key->clamp_vertex_color = cso_rast->state.clamp_vertex_color;

#if !(GEN_VERSIONx10 == 75)
   uint64_t inputs_read = info->inputs_read;
   int ve_idx = 0;
   while (inputs_read) {
      int i = u_bit_scan64(&inputs_read);
      uint8_t wa_flags = get_wa_flags(ice->state.cso_vertex_elements->fmt[ve_idx]);
      key->gl_attrib_wa_flags[i] = wa_flags;
      ve_idx++;
   }
#endif
}

/**
 * Populate TCS program key fields based on the current state.
 */
static void
crocus_populate_tcs_key(const struct crocus_context *ice,
                      struct brw_tcs_prog_key *key)
{
}

/**
 * Populate TES program key fields based on the current state.
 */
static void
crocus_populate_tes_key(const struct crocus_context *ice,
                      const struct shader_info *info,
                      gl_shader_stage last_stage,
                      struct brw_tes_prog_key *key)
{
   const struct crocus_rasterizer_state *cso_rast = ice->state.cso_rast;

   if (info->clip_distance_array_size == 0 &&
       (info->outputs_written & (VARYING_BIT_POS | VARYING_BIT_CLIP_VERTEX)) &&
       last_stage == MESA_SHADER_TESS_EVAL)
      key->nr_userclip_plane_consts = cso_rast->num_clip_plane_consts;
}

/**
 * Populate GS program key fields based on the current state.
 */
static void
crocus_populate_gs_key(const struct crocus_context *ice,
                     const struct shader_info *info,
                     gl_shader_stage last_stage,
                     struct brw_gs_prog_key *key)
{
   const struct crocus_rasterizer_state *cso_rast = ice->state.cso_rast;

   if (info->clip_distance_array_size == 0 &&
       (info->outputs_written & (VARYING_BIT_POS | VARYING_BIT_CLIP_VERTEX)) &&
       last_stage == MESA_SHADER_GEOMETRY)
      key->nr_userclip_plane_consts = cso_rast->num_clip_plane_consts;
}

/**
 * Populate FS program key fields based on the current state.
 */
static void
crocus_populate_fs_key(const struct crocus_context *ice,
                     const struct shader_info *info,
                     struct brw_wm_prog_key *key)
{
   struct crocus_screen *screen = (void *) ice->ctx.screen;
   const struct pipe_framebuffer_state *fb = &ice->state.framebuffer;
   const struct crocus_depth_stencil_alpha_state *zsa = ice->state.cso_zsa;
   const struct crocus_rasterizer_state *rast = ice->state.cso_rast;
   const struct crocus_blend_state *blend = ice->state.cso_blend;
   uint32_t lookup = 0;

#if GEN_GEN < 6
   if (info->fs.uses_discard || zsa->cso.alpha_enabled)
      lookup |= BRW_WM_IZ_PS_KILL_ALPHATEST_BIT;

   if (info->outputs_written & BITFIELD64_BIT(FRAG_RESULT_DEPTH))
      lookup |= BRW_WM_IZ_PS_COMPUTES_DEPTH_BIT;

   if (fb->zsbuf && zsa->cso.depth_enabled) {
      lookup |= BRW_WM_IZ_DEPTH_TEST_ENABLE_BIT;

      if (zsa->cso.depth_writemask)
         lookup |= BRW_WM_IZ_DEPTH_WRITE_ENABLE_BIT;

   }
   if (zsa->cso.stencil[0].enabled || zsa->cso.stencil[1].enabled) {
      lookup |= BRW_WM_IZ_STENCIL_TEST_ENABLE_BIT;
      if (zsa->cso.stencil[0].writemask || zsa->cso.stencil[1].writemask)
         lookup |= BRW_WM_IZ_STENCIL_WRITE_ENABLE_BIT;
   }
   key->iz_lookup = lookup;
   key->stats_wm = ice->state.stats_wm;
#endif

   uint32_t line_aa = BRW_WM_AA_NEVER;
   if (rast->state.line_smooth) {
      int reduced_prim = u_reduced_prim(ice->state.prim_mode);
      if (reduced_prim == PIPE_PRIM_LINES)
         line_aa = BRW_WM_AA_ALWAYS;
      else if (reduced_prim == PIPE_PRIM_TRIANGLES) {
         if (rast->state.fill_front == PIPE_POLYGON_MODE_LINE) {
            line_aa = BRW_WM_AA_SOMETIMES;

            if (rast->state.fill_back == PIPE_POLYGON_MODE_LINE ||
                rast->state.cull_face == PIPE_FACE_BACK)
               line_aa = BRW_WM_AA_ALWAYS;
         } else if (rast->state.fill_back == PIPE_POLYGON_MODE_LINE) {
            line_aa = BRW_WM_AA_SOMETIMES;

            if (rast->state.cull_face == PIPE_FACE_FRONT)
               line_aa = BRW_WM_AA_ALWAYS;
         }
      }
   }
   key->line_aa = line_aa;

   key->nr_color_regions = fb->nr_cbufs;

   key->clamp_fragment_color = rast->clamp_fragment_color;

   key->alpha_to_coverage = blend->alpha_to_coverage;

   key->alpha_test_replicate_alpha = fb->nr_cbufs > 1 && zsa->cso.alpha_enabled;

   key->flat_shade = rast->flatshade &&
      (info->inputs_read & (VARYING_BIT_COL0 | VARYING_BIT_COL1));

   key->persample_interp = rast->force_persample_interp;
   key->multisample_fbo = rast->multisample && fb->samples > 1;

   key->coherent_fb_fetch = false; // TODO: needed?

   key->force_dual_color_blend =
      screen->driconf.dual_color_blend_by_location &&
      (blend->blend_enables & 1) && blend->dual_color_blending;

   /* TODO: Respect glHint for key->high_quality_derivatives */

#if GEN_GEN <= 5
   if (fb->nr_cbufs > 1 && zsa->cso.alpha_enabled) {
      key->alpha_test_func = zsa->cso.alpha_func;
      key->alpha_test_ref = zsa->cso.alpha_ref_value;
   }
#endif
}

static void
crocus_populate_cs_key(const struct crocus_context *ice,
                     struct brw_cs_prog_key *key)
{
}

#if GEN_GEN == 4
#define KSP(ice, shader) ro_bo((ice)->shaders.cache_bo, (shader)->offset);
#elif GEN_GEN >= 5
static uint64_t
KSP(const struct crocus_context *ice, const struct crocus_compiled_shader *shader)
{
#if GEN_GEN == 5
   return shader->offset;
#else
   return crocus_bo_offset_from_base_address(ice->shaders.cache_bo) + shader->offset;
#endif
}
#endif

/* Gen11 workaround table #2056 WABTPPrefetchDisable suggests to disable
 * prefetching of binding tables in A0 and B0 steppings.  XXX: Revisit
 * this WA on C0 stepping.
 *
 * TODO: Fill out SamplerCount for prefetching?
 */

#define INIT_THREAD_DISPATCH_FIELDS(pkt, prefix, stage)                   \
   pkt.KernelStartPointer = KSP(ice, shader);                            \
   pkt.BindingTableEntryCount = shader->bt.size_bytes / 4;                \
   pkt.FloatingPointMode = prog_data->use_alt_mode;                       \
                                                                          \
   pkt.DispatchGRFStartRegisterForURBData =                               \
      prog_data->dispatch_grf_start_reg;                                  \
   pkt.prefix##URBEntryReadLength = vue_prog_data->urb_read_length;       \
   pkt.prefix##URBEntryReadOffset = 0;                                    \
                                                                          \
   pkt.StatisticsEnable = true;                                           \
   pkt.Enable           = true;                                           \
                                                                          \
   if (prog_data->total_scratch) {                                        \
      struct crocus_bo *bo =                                                \
         crocus_get_scratch_space(ice, prog_data->total_scratch, stage);    \
      uint32_t scratch_addr = bo->gtt_offset;                             \
      pkt.PerThreadScratchSpace = ffs(prog_data->total_scratch) - 11;     \
      pkt.ScratchSpaceBasePointer = rw_bo(NULL, scratch_addr);            \
   }

/**
 * Encode most of 3DSTATE_VS based on the compiled shader.
 */
static void
crocus_store_vs_state(struct crocus_context *ice,
                    const struct gen_device_info *devinfo,
                    struct crocus_compiled_shader *shader)
{
   struct brw_stage_prog_data *prog_data = shader->prog_data;
   struct brw_vue_prog_data *vue_prog_data = (void *) prog_data;
#if GEN_GEN >= 6
   crocus_pack_command(GENX(3DSTATE_VS), shader->derived_data, vs) {
      INIT_THREAD_DISPATCH_FIELDS(vs, Vertex, MESA_SHADER_VERTEX);
      vs.MaximumNumberofThreads = devinfo->max_vs_threads - 1;
// XXX gen < 6 + gen == 5 stuff goes here
   }
#endif
}

/**
 * Encode most of 3DSTATE_HS based on the compiled shader.
 */
static void
crocus_store_tcs_state(struct crocus_context *ice,
                     const struct gen_device_info *devinfo,
                     struct crocus_compiled_shader *shader)
{
#if GEN_GEN >= 7
   struct brw_stage_prog_data *prog_data = shader->prog_data;
   struct brw_vue_prog_data *vue_prog_data = (void *) prog_data;
   struct brw_tcs_prog_data *tcs_prog_data = (void *) prog_data;

   crocus_pack_command(GENX(3DSTATE_HS), shader->derived_data, hs) {
      INIT_THREAD_DISPATCH_FIELDS(hs, Vertex, MESA_SHADER_TESS_CTRL);

      hs.InstanceCount = tcs_prog_data->instances - 1;
      hs.MaximumNumberofThreads = devinfo->max_tcs_threads - 1;
      hs.IncludeVertexHandles = true;
   }
#endif
}

/**
 * Encode 3DSTATE_TE and most of 3DSTATE_DS based on the compiled shader.
 */
static void
crocus_store_tes_state(struct crocus_context *ice,
                     const struct gen_device_info *devinfo,
                     struct crocus_compiled_shader *shader)
{
#if GEN_GEN >= 7
   struct brw_stage_prog_data *prog_data = shader->prog_data;
   struct brw_vue_prog_data *vue_prog_data = (void *) prog_data;
   struct brw_tes_prog_data *tes_prog_data = (void *) prog_data;

   uint32_t *te_state = (void *) shader->derived_data;
   uint32_t *ds_state = te_state + GENX(3DSTATE_TE_length);

   crocus_pack_command(GENX(3DSTATE_TE), te_state, te) {
      te.Partitioning = tes_prog_data->partitioning;
      te.OutputTopology = tes_prog_data->output_topology;
      te.TEDomain = tes_prog_data->domain;
      te.TEEnable = true;
      te.MaximumTessellationFactorOdd = 63.0;
      te.MaximumTessellationFactorNotOdd = 64.0;
   }

   crocus_pack_command(GENX(3DSTATE_DS), ds_state, ds) {
      INIT_THREAD_DISPATCH_FIELDS(ds, Patch, MESA_SHADER_TESS_EVAL);

//      ds.DispatchMode = DISPATCH_MODE_SIMD8_SINGLE_PATCH;
      ds.MaximumNumberofThreads = devinfo->max_tes_threads - 1;
      ds.ComputeWCoordinateEnable =
         tes_prog_data->domain == BRW_TESS_DOMAIN_TRI;

//      ds.UserClipDistanceCullTestEnableBitmask =
//         vue_prog_data->cull_distance_mask;
   }
#endif
}

/**
 * Encode most of 3DSTATE_GS based on the compiled shader.
 */
static void
crocus_store_gs_state(struct crocus_context *ice,
                    const struct gen_device_info *devinfo,
                    struct crocus_compiled_shader *shader)
{
// Actually pre-gen6 also uses GS for xfb or something
#if GEN_GEN >= 6
   struct brw_stage_prog_data *prog_data = shader->prog_data;
   struct brw_vue_prog_data *vue_prog_data = (void *) prog_data;
   struct brw_gs_prog_data *gs_prog_data = (void *) prog_data;

   crocus_pack_command(GENX(3DSTATE_GS), shader->derived_data, gs) {
      INIT_THREAD_DISPATCH_FIELDS(gs, Vertex, MESA_SHADER_GEOMETRY);

#if GEN_GEN >= 7
      gs.OutputVertexSize = gs_prog_data->output_vertex_size_hwords * 2 - 1;
      gs.OutputTopology = gs_prog_data->output_topology;
      gs.ControlDataHeaderSize =
         gs_prog_data->control_data_header_size_hwords;
      gs.InstanceControl = gs_prog_data->invocations - 1;
      gs.DispatchMode = DISPATCH_MODE_SIMD8;
      gs.IncludePrimitiveID = gs_prog_data->include_primitive_id;
      gs.ControlDataFormat = gs_prog_data->control_data_format;
#endif
      gs.ReorderMode = TRAILING;
      gs.MaximumNumberofThreads = devinfo->max_gs_threads - 1;
#if GEN_GEN < 7
      gs.SOStatisticsEnable = true;
      // if xfb, gs.SVBIPayloadEnable = active

      /* GEN6_GS_SPF_MODE and GEN6_GS_VECTOR_MASK_ENABLE are enabled as it
       * was previously done for gen6.
       *
       * TODO: test with both disabled to see if the HW is behaving
       * as expected, like in gen7.
       */
      gs.SingleProgramFlow = true;
      gs.VectorMaskEnable = true;
#endif
   }
#endif
}

/**
 * Encode most of 3DSTATE_PS and 3DSTATE_PS_EXTRA based on the shader.
 */
static void
crocus_store_fs_state(struct crocus_context *ice,
                    const struct gen_device_info *devinfo,
                    struct crocus_compiled_shader *shader)
{
#if 0
   struct brw_stage_prog_data *prog_data = shader->prog_data;
   struct brw_wm_prog_data *wm_prog_data = (void *) shader->prog_data;

   uint32_t *ps_state = (void *) shader->derived_data;

   crocus_pack_command(GENX(3DSTATE_PS), ps_state, ps) {
      ps.VectorMaskEnable = true;
      // XXX: WABTPPrefetchDisable, see above, drop at C0
      ps.BindingTableEntryCount = shader->bt.size_bytes / 4;
      ps.FloatingPointMode = prog_data->use_alt_mode;
      ps.MaximumNumberofThreads = devinfo->max_wm_threads - 1;

      ps.PushConstantEnable = prog_data->ubo_ranges[0].length > 0;

      /* From the documentation for this packet:
       * "If the PS kernel does not need the Position XY Offsets to
       *  compute a Position Value, then this field should be programmed
       *  to POSOFFSET_NONE."
       *
       * "SW Recommendation: If the PS kernel needs the Position Offsets
       *  to compute a Position XY value, this field should match Position
       *  ZW Interpolation Mode to ensure a consistent position.xyzw
       *  computation."
       *
       * We only require XY sample offsets. So, this recommendation doesn't
       * look useful at the moment.  We might need this in future.
       */
      ps.PositionXYOffsetSelect =
         wm_prog_data->uses_pos_offset ? POSOFFSET_SAMPLE : POSOFFSET_NONE;

      if (prog_data->total_scratch) {
         struct crocus_bo *bo =
            crocus_get_scratch_space(ice, prog_data->total_scratch,
                                   MESA_SHADER_FRAGMENT);
         uint32_t scratch_addr = bo->gtt_offset;
         ps.PerThreadScratchSpace = ffs(prog_data->total_scratch) - 11;
         ps.ScratchSpaceBasePointer = rw_bo(NULL, scratch_addr);
      }
   }
#endif
}

/**
 * Compute the size of the derived data (shader command packets).
 *
 * This must match the data written by the crocus_store_xs_state() functions.
 */
static void
crocus_store_cs_state(struct crocus_context *ice,
                    const struct gen_device_info *devinfo,
                    struct crocus_compiled_shader *shader)
{
#if GEN_GEN == 7
   struct brw_stage_prog_data *prog_data = shader->prog_data;
   struct brw_cs_prog_data *cs_prog_data = (void *) shader->prog_data;
   void *map = shader->derived_data;

   crocus_pack_state(GENX(INTERFACE_DESCRIPTOR_DATA), map, desc) {
      desc.KernelStartPointer = KSP(ice, shader);
      desc.ConstantURBEntryReadLength = cs_prog_data->push.per_thread.regs;
      desc.SharedLocalMemorySize =
         encode_slm_size(GEN_GEN, prog_data->total_shared);
      desc.BarrierEnable = cs_prog_data->uses_barrier;
#if GEN_VERSIONx10 == 75
      desc.CrossThreadConstantDataReadLength =
         cs_prog_data->push.cross_thread.regs;
#endif
   }
#endif
}

static unsigned
crocus_derived_program_state_size(enum crocus_program_cache_id cache_id)
{
   assert(cache_id <= CROCUS_CACHE_CLIP);

   static const unsigned dwords[] = {
#if GEN_GEN >= 6
      [CROCUS_CACHE_VS] = GENX(3DSTATE_VS_length),
      [CROCUS_CACHE_GS] = GENX(3DSTATE_GS_length),
#endif
#if GEN_GEN >= 7
      [CROCUS_CACHE_TCS] = GENX(3DSTATE_HS_length),
      [CROCUS_CACHE_TES] = GENX(3DSTATE_TE_length) + GENX(3DSTATE_DS_length),
      [CROCUS_CACHE_FS] = GENX(3DSTATE_PS_length),
      [CROCUS_CACHE_CS] = GENX(INTERFACE_DESCRIPTOR_DATA_length),
#endif
      [CROCUS_CACHE_BLORP] = 0,
      [CROCUS_CACHE_SF] = 0,
      [CROCUS_CACHE_CLIP] = 0,
   };

   return sizeof(uint32_t) * dwords[cache_id];
}

/**
 * Create any state packets corresponding to the given shader stage
 * (i.e. 3DSTATE_VS) and save them as "derived data" in the shader variant.
 * This means that we can look up a program in the in-memory cache and
 * get most of the state packet without having to reconstruct it.
 */
static void
crocus_store_derived_program_state(struct crocus_context *ice,
                                 enum crocus_program_cache_id cache_id,
                                 struct crocus_compiled_shader *shader)
{
   struct crocus_screen *screen = (void *) ice->ctx.screen;
   const struct gen_device_info *devinfo = &screen->devinfo;
#if 0
   switch (cache_id) {
   case CROCUS_CACHE_VS:
      crocus_store_vs_state(ice, devinfo, shader);
      break;
   case CROCUS_CACHE_TCS:
      crocus_store_tcs_state(ice, devinfo, shader);
      break;
   case CROCUS_CACHE_TES:
      crocus_store_tes_state(ice, devinfo, shader);
      break;
   case CROCUS_CACHE_GS:
      crocus_store_gs_state(ice, devinfo, shader);
      break;
   case CROCUS_CACHE_FS:
      crocus_store_fs_state(ice, devinfo, shader);
      break;
   case CROCUS_CACHE_CS:
      crocus_store_cs_state(ice, devinfo, shader);
   case CROCUS_CACHE_BLORP:
   case CROCUS_CACHE_SF:
   case CROCUS_CACHE_CLIP:
      break;
   default:
      break;
   }
#endif
}

/* ------------------------------------------------------------------- */

static const uint32_t push_constant_opcodes[] = {
   [MESA_SHADER_VERTEX]    = 21,
   [MESA_SHADER_TESS_CTRL] = 25, /* HS */
   [MESA_SHADER_TESS_EVAL] = 26, /* DS */
   [MESA_SHADER_GEOMETRY]  = 22,
   [MESA_SHADER_FRAGMENT]  = 23,
   [MESA_SHADER_COMPUTE]   = 0,
};

static void
emit_sized_null_surface(struct crocus_batch *batch,
                        unsigned width, unsigned height,
                        unsigned layers, unsigned levels,
                        uint32_t *out_offset)
{
   struct isl_device *isl_dev = &batch->screen->isl_dev;
   uint32_t *surf = stream_state(batch, isl_dev->ss.size,
                                 isl_dev->ss.align,
                                 out_offset);
   //TODO gen 6 multisample crash
   isl_null_fill_state(isl_dev, surf,
                       isl_extent3d(width, height, layers), levels);
}
static void
emit_null_surface(struct crocus_batch *batch,
                  uint32_t *out_offset)
{
   emit_sized_null_surface(batch, 1, 1, 1, 0, out_offset);
}

static void
emit_null_fb_surface(struct crocus_batch *batch,
                     struct crocus_context *ice,
                     uint32_t *out_offset)
{
   uint32_t width, height, layers, level;
   /* If set_framebuffer_state() was never called, fall back to 1x1x1 */
   if (ice->state.framebuffer.width == 0 && ice->state.framebuffer.height == 0) {
      emit_null_surface(batch, out_offset);
      return;
   }

   struct pipe_framebuffer_state *cso = &ice->state.framebuffer;
   width = MAX2(cso->width, 1);
   height = MAX2(cso->height, 1);
   layers = cso->layers ? cso->layers : 1;
   level = 0;

   if (cso->nr_cbufs == 0 && cso->zsbuf) {
      width = cso->zsbuf->width;
      height = cso->zsbuf->height;
      level = cso->zsbuf->u.tex.level;
   }
   emit_sized_null_surface(batch, width, height,
                           layers, level,
                           out_offset);
}

static uint32_t
surf_state_offset_for_aux(struct crocus_resource *res,
                          unsigned aux_modes,
                          enum isl_aux_usage aux_usage)
{
   return SURFACE_STATE_ALIGNMENT *
          util_bitcount(aux_modes & ((1 << aux_usage) - 1));
}

static void
update_clear_value(struct crocus_context *ice,
                   struct crocus_batch *batch,
                   struct crocus_resource *res,
                   struct crocus_surface_state *surf_state,
                   unsigned all_aux_modes,
                   struct isl_view *view)
{
   UNUSED struct isl_device *isl_dev = &batch->screen->isl_dev;
   UNUSED unsigned aux_modes = all_aux_modes;

   // TODO: check if this is needed
   /* TODO: Could update rather than re-filling */
   alloc_surface_states(surf_state, all_aux_modes);

   void *map = surf_state->cpu;

   while (aux_modes) {
      enum isl_aux_usage aux_usage = u_bit_scan(&aux_modes);
      fill_surface_state(isl_dev, map, res, &res->surf, view, aux_usage,
                         0, 0, 0);
      map += SURFACE_STATE_ALIGNMENT;
   }

   upload_surface_states(ice->state.surface_uploader, surf_state);
}

static uint32_t
emit_surface(struct crocus_context *ice,
             struct crocus_batch *batch,
             struct pipe_surface *p_surf,
             unsigned idx,
             bool writeable,
             enum isl_aux_usage aux_usage,
             struct pipe_rt_blend_state *gen4_rt_state)
{
   struct crocus_surface *surf = (void *) p_surf;
   UNUSED struct isl_device *isl_dev = &batch->screen->isl_dev;
   struct crocus_resource *res = (void *) p_surf->texture;
   uint32_t offset = 0;
   uint32_t write_disables = 0;

   write_disables |= (gen4_rt_state->colormask & PIPE_MASK_A) ? 0x0 : 0x8;
   write_disables |= (gen4_rt_state->colormask & PIPE_MASK_R) ? 0x0 : 0x4;
   write_disables |= (gen4_rt_state->colormask & PIPE_MASK_G) ? 0x0 : 0x2;
   write_disables |= (gen4_rt_state->colormask & PIPE_MASK_B) ? 0x0 : 0x1;
   struct isl_view *view = &surf->view;
   union isl_color_value clear_color = { .u32 = { 0, 0, 0, 0 } };
   uint32_t *surf_state = stream_state(batch, isl_dev->ss.size, isl_dev->ss.align, &offset);
   uint64_t clear_offset = 0;
   isl_surf_fill_state(isl_dev, surf_state, .surf = &res->surf, .view = view,
                       .address = crocus_state_reloc(batch,
						     offset + isl_dev->ss.addr_offset,
						     res->bo, 0, 0),
                       .aux_surf = NULL, .aux_usage = 0,
                       .aux_address = 0,
                       .mocs = mocs(res->bo, isl_dev),
                       .clear_color = clear_color,
                       .use_clear_address = false,
                       .clear_address = clear_offset,
                       .x_offset_sa = 0, .y_offset_sa = 0,
#if GEN_GEN <= 5
                       .write_disables = write_disables,
                       .blend_enable = gen4_rt_state->blend_enable,
#endif
                       );
   return offset;
}

static uint32_t
emit_ubo_buffer(struct crocus_context *ice,
                struct crocus_batch *batch,
                struct pipe_shader_buffer *buffer)
{
   UNUSED struct isl_device *isl_dev = &batch->screen->isl_dev;
   uint32_t offset = 0;

   uint32_t *surf_state = stream_state(batch, isl_dev->ss.size,
                                       isl_dev->ss.align, &offset);
   isl_buffer_fill_state(isl_dev, surf_state,
                         .address = crocus_state_reloc(batch, offset + isl_dev->ss.addr_offset,
                                                       crocus_resource_bo(buffer->buffer),
                                                       buffer->buffer_offset,
                                                       RELOC_32BIT),
                         .size_B = buffer->buffer_size,
                         .format = 0,
                         .swizzle = ISL_SWIZZLE_IDENTITY,
                         .stride_B = 1,
                         .mocs = mocs(crocus_resource_bo(buffer->buffer), isl_dev));

   return offset;
}

static uint32_t
emit_sampler_view(struct crocus_context *ice,
                  struct crocus_batch *batch,
                  struct crocus_sampler_view *isv)
{
   UNUSED struct isl_device *isl_dev = &batch->screen->isl_dev;
   uint32_t offset = 0;

   uint32_t *surf_state = stream_state(batch, isl_dev->ss.size,
                                       isl_dev->ss.align, &offset);
   struct isl_surf_fill_state_info f = {
      .surf = &isv->res->surf,
      .view = &isv->view,
      .address = crocus_state_reloc(batch, offset + isl_dev->ss.addr_offset,
                                    isv->res->bo, 0, RELOC_32BIT),
   };

   isl_surf_fill_state_s(isl_dev, surf_state, &f);

   return offset;
}

#define foreach_surface_used(index, group) \
   for (int index = 0; index < bt->sizes[group]; index++) \
      if (crocus_group_index_to_bti(bt, group, index) != \
          CROCUS_SURFACE_NOT_USED)


static void
crocus_populate_binding_table(struct crocus_context *ice,
                            struct crocus_batch *batch,
                              gl_shader_stage stage)
{
   struct crocus_uncompiled_shader *ish = ice->shaders.uncompiled[stage];
   struct crocus_compiled_shader *shader = ice->shaders.prog[stage];
   struct crocus_shader_state *shs = &ice->state.shaders[stage];
   if (!shader)
      return;

   struct crocus_binding_table *bt = &shader->bt;
   int s = 0;
   uint32_t *surf_offsets = shader->surf_offset;

   const struct shader_info *info = crocus_get_shader_info(ice, stage);
   if (!info) {
      return;
   }

   if (stage == MESA_SHADER_FRAGMENT) {
      struct pipe_framebuffer_state *cso_fb = &ice->state.framebuffer;
      /* Note that cso_fb->nr_cbufs == fs_key->nr_color_regions. */
      if (cso_fb->nr_cbufs) {
         for (unsigned i = 0; i < cso_fb->nr_cbufs; i++) {
            uint32_t addr;
#if GEN_GEN <= 5
            const struct pipe_rt_blend_state *rt =
               &ice->state.cso_blend->blend_state.rt[ice->state.cso_blend->blend_state.independent_blend_enable ? i : 0];
#endif
            if (cso_fb->cbufs[i]) {
               surf_offsets[s] = emit_surface(ice, batch, cso_fb->cbufs[i], i, true,
                                              ice->state.draw_aux_usage[i],
#if GEN_GEN <= 5
                                              rt
#else
                                              NULL
#endif
                                              );
            } else {
               emit_null_fb_surface(batch, ice, &surf_offsets[s]);
            }
            s++;
         }
      } else {
         emit_null_fb_surface(batch, ice, &surf_offsets[s]);
         s++;
      }
   }

   unsigned i;

   foreach_surface_used(i, CROCUS_SURFACE_GROUP_TEXTURE) {
      struct crocus_sampler_view *view = shs->textures[i];
      if (view)
         surf_offsets[s] = emit_sampler_view(ice, batch, view);
      else
         emit_null_surface(batch, &surf_offsets[s]);
      s++;
   }

   foreach_surface_used(i, CROCUS_SURFACE_GROUP_IMAGE) {

   }
   foreach_surface_used(i, CROCUS_SURFACE_GROUP_UBO) {
      surf_offsets[s++] = emit_ubo_buffer(ice, batch, &shs->constbuf[i]);
   }
   foreach_surface_used(i, CROCUS_SURFACE_GROUP_SSBO) {
   }

}
/* ------------------------------------------------------------------- */
static uint32_t
crocus_upload_binding_table(struct crocus_context *ice,
                            struct crocus_batch *batch,
                            uint32_t *table,
                            uint32_t size)

{
   if (size == 0)
      return 0;
   return emit_state(batch, table, size, 32);
}

/**
 * Possibly emit STATE_BASE_ADDRESS to update Surface State Base Address.
 */

static void
crocus_update_surface_base_address(struct crocus_batch *batch)
{
   if (batch->state_base_address_emitted)
       return;
#if GEN_GEN >= 6
   uint32_t mocs = batch->screen->isl_dev.mocs.internal;
#endif
   flush_before_state_base_change(batch);

   crocus_emit_cmd(batch, GENX(STATE_BASE_ADDRESS), sba) {

      sba.SurfaceStateBaseAddressModifyEnable = true;
      sba.SurfaceStateBaseAddress = ro_bo(batch->state.bo, 0);

#if GEN_GEN >= 5
      sba.InstructionBaseAddress = ro_bo(batch->ice->shaders.cache_bo, 0); // TODO!
#endif

      sba.GeneralStateBaseAddressModifyEnable   = true;
      sba.IndirectObjectBaseAddressModifyEnable = true;
#if GEN_GEN >= 5
      sba.InstructionBaseAddressModifyEnable    = true;
#endif

      sba.GeneralStateAccessUpperBoundModifyEnable = true;
#if GEN_GEN >= 5
      sba.IndirectObjectAccessUpperBoundModifyEnable = true;
      sba.InstructionAccessUpperBoundModifyEnable = true;
#endif
#if GEN_GEN <= 5
      sba.GeneralStateAccessUpperBound = ro_bo(NULL, 0xfffff000);
#endif
#if GEN_GEN >= 6

      
      /* The hardware appears to pay attention to the MOCS fields even
       * if you don't set the "Address Modify Enable" bit for the base.
       */
      sba.GeneralStateMOCS            = mocs;
      sba.StatelessDataPortAccessMOCS = mocs;

      sba.DynamicStateBaseAddressModifyEnable   = true;

      sba.DynamicStateBaseAddress = ro_bo(batch->state.bo, 0);

      sba.DynamicStateAccessUpperBoundModifyEnable = true;
#endif
   }

   flush_after_state_base_change(batch);

   /* According to section 3.6.1 of VOL1 of the 965 PRM,
    * STATE_BASE_ADDRESS updates require a reissue of:
    *
    * 3DSTATE_PIPELINE_POINTERS
    * 3DSTATE_BINDING_TABLE_POINTERS
    * MEDIA_STATE_POINTERS
    *
    * and this continues through Ironlake.  The Sandy Bridge PRM, vol
    * 1 part 1 says that the folowing packets must be reissued:
    *
    * 3DSTATE_CC_POINTERS
    * 3DSTATE_BINDING_TABLE_POINTERS
    * 3DSTATE_SAMPLER_STATE_POINTERS
    * 3DSTATE_VIEWPORT_STATE_POINTERS
    * MEDIA_STATE_POINTERS
    *
    * Those are always reissued following SBA updates anyway (new
    * batch time), except in the case of the program cache BO
    * changing.  Having a separate state flag makes the sequence more
    * obvious.
    */
#if GEN_GEN <= 5
   batch->ice->state.dirty |= CROCUS_DIRTY_GEN5_PIPELINED_POINTERS | CROCUS_DIRTY_GEN5_BINDING_TABLE_POINTERS;
#elif GEN_GEN == 6
   batch->ice->state.dirty |= CROCUS_DIRTY_GEN5_BINDING_TABLE_POINTERS;
#else
   //TODO
#endif
   batch->state_base_address_emitted = true;
}

static inline void
crocus_viewport_zmin_zmax(const struct pipe_viewport_state *vp, bool halfz,
                        bool window_space_position, float *zmin, float *zmax)
{
   if (window_space_position) {
      *zmin = 0.f;
      *zmax = 1.f;
      return;
   }
   util_viewport_zmin_zmax(vp, halfz, zmin, zmax);
}

struct push_bos {
   struct {
      struct crocus_address addr;
      uint32_t length;
   } buffers[4];
   int buffer_count;
   uint32_t max_length;
};

static void
setup_constant_buffers(struct crocus_context *ice,
                       struct crocus_batch *batch,
                       int stage,
                       struct push_bos *push_bos)
{
   struct crocus_shader_state *shs = &ice->state.shaders[stage];
   struct crocus_compiled_shader *shader = ice->shaders.prog[stage];
   struct brw_stage_prog_data *prog_data = (void *) shader->prog_data;

   uint32_t push_range_sum = 0;

   int n = 0;
   for (int i = 0; i < 4; i++) {
      const struct brw_ubo_range *range = &prog_data->ubo_ranges[i];

      if (range->length == 0)
         continue;

      push_range_sum += range->length;

      if (range->length > push_bos->max_length)
         push_bos->max_length = range->length;

      /* Range block is a binding table index, map back to UBO index. */
      unsigned block_index = crocus_bti_to_group_index(
         &shader->bt, CROCUS_SURFACE_GROUP_UBO, range->block);
      assert(block_index != CROCUS_SURFACE_NOT_USED);

      struct pipe_shader_buffer *cbuf = &shs->constbuf[block_index];
      struct crocus_resource *res = (void *) cbuf->buffer;

      assert(cbuf->buffer_offset % 32 == 0);

      push_bos->buffers[n].length = range->length;
      push_bos->buffers[n].addr =
         res ? ro_bo(res->bo, range->start * 32 + cbuf->buffer_offset)
         : ro_bo(batch->screen->workaround_bo, 0);
      n++;
   }

   /* From the 3DSTATE_CONSTANT_XS and 3DSTATE_CONSTANT_ALL programming notes:
    *
    *    "The sum of all four read length fields must be less than or
    *    equal to the size of 64."
    */
   assert(push_range_sum <= 64);

   push_bos->buffer_count = n;
}

#if GEN_GEN == 7
static void
gen7_depth_flush(struct crocus_batch *batch)
{
   /* From the Haswell PRM, documentation for 3DSTATE_DEPTH_BUFFER:
    *
    *    "Restriction: Prior to changing Depth/Stencil Buffer state (i.e., any
    *    combination of 3DSTATE_DEPTH_BUFFER, 3DSTATE_CLEAR_PARAMS,
    *    3DSTATE_STENCIL_BUFFER, 3DSTATE_HIER_DEPTH_BUFFER) SW must first
    *    issue a pipelined depth stall (PIPE_CONTROL with Depth Stall bit
    *    set), followed by a pipelined depth cache flush (PIPE_CONTROL with
    *    Depth Flush Bit set, followed by another pipelined depth stall
    *    (PIPE_CONTROL with Depth Stall Bit set), unless SW can otherwise
    *    guarantee that the pipeline from WM onwards is already flushed (e.g.,
    *    via a preceding MI_FLUSH)."
    */
   crocus_emit_pipe_control_flush(batch,
                                  "depth flush",
                                  PIPE_CONTROL_DEPTH_STALL);
   crocus_emit_pipe_control_flush(batch,
                                  "depth flush",
                                  PIPE_CONTROL_DEPTH_CACHE_FLUSH);
   crocus_emit_pipe_control_flush(batch,
                                  "depth flush",
                                  PIPE_CONTROL_DEPTH_STALL);
}

static void
gen7_emit_vs_workaround_flush(struct crocus_batch *batch)
{
   ASSERTED const struct gen_device_info *devinfo = &batch->screen->devinfo;

   assert(devinfo->gen == 7);
   crocus_emit_pipe_control_write(batch,
				  "vs workaround",
				  PIPE_CONTROL_WRITE_IMMEDIATE
				  | PIPE_CONTROL_DEPTH_STALL,
				  batch->screen->workaround_bo, 0, 0);
}

static void
emit_push_constant_packets(struct crocus_context *ice,
                           struct crocus_batch *batch,
                           int stage,
                           const struct push_bos *push_bos)
{
   struct crocus_compiled_shader *shader = ice->shaders.prog[stage];
   struct brw_stage_prog_data *prog_data = (void *) shader->prog_data;

   if (stage == MESA_SHADER_VERTEX) {
      if (!(GEN_VERSIONx10 == 75) && !batch->screen->devinfo.is_baytrail)
         gen7_emit_vs_workaround_flush(batch);
   }
   crocus_emit_cmd(batch, GENX(3DSTATE_CONSTANT_VS), pkt) {
      pkt._3DCommandSubOpcode = push_constant_opcodes[stage];
      if (prog_data) {
         /* The Skylake PRM contains the following restriction:
          *
          *    "The driver must ensure The following case does not occur
          *     without a flush to the 3D engine: 3DSTATE_CONSTANT_* with
          *     buffer 3 read length equal to zero committed followed by a
          *     3DSTATE_CONSTANT_* with buffer 0 read length not equal to
          *     zero committed."
          *
          * To avoid this, we program the buffers in the highest slots.
          * This way, slot 0 is only used if slot 3 is also used.
          */
         int n = push_bos->buffer_count;
         assert(n <= 4);
         const unsigned shift = 4 - n;
         for (int i = 0; i < n; i++) {
            pkt.ConstantBody.ReadLength[i + shift] =
               push_bos->buffers[i].length;
            pkt.ConstantBody.Buffer[i + shift] = push_bos->buffers[i].addr;
         }
      }
   }
}

#endif

#if GEN_GEN >= 6
typedef struct GENX(DEPTH_STENCIL_STATE)      DEPTH_STENCIL_GENXML;
#else
typedef struct GENX(COLOR_CALC_STATE)         DEPTH_STENCIL_GENXML;
#endif

static inline void
set_depth_stencil_bits(struct crocus_context *ice, DEPTH_STENCIL_GENXML *ds)
{
   struct crocus_depth_stencil_alpha_state *cso = ice->state.cso_zsa;
   ds->DepthTestEnable = cso->cso.depth_enabled;
   ds->DepthBufferWriteEnable = cso->cso.depth_writemask;
   ds->DepthTestFunction = translate_compare_func(cso->cso.depth_func);

   ds->StencilFailOp = cso->cso.stencil[0].fail_op;
   ds->StencilPassDepthFailOp = cso->cso.stencil[0].zfail_op;
   ds->StencilPassDepthPassOp = cso->cso.stencil[0].zpass_op;
   ds->StencilTestFunction = translate_compare_func(cso->cso.stencil[0].func);

   ds->StencilTestMask = cso->cso.stencil[0].valuemask;
   ds->StencilWriteMask = cso->cso.stencil[0].writemask;

   ds->BackfaceStencilFailOp = cso->cso.stencil[1].fail_op;
   ds->BackfaceStencilPassDepthFailOp = cso->cso.stencil[1].zfail_op;
   ds->BackfaceStencilPassDepthPassOp = cso->cso.stencil[1].zpass_op;
   ds->BackfaceStencilTestFunction = translate_compare_func(cso->cso.stencil[1].func);

   ds->BackfaceStencilTestMask = cso->cso.stencil[1].valuemask;
   ds->BackfaceStencilWriteMask = cso->cso.stencil[1].writemask;
   ds->DoubleSidedStencilEnable = cso->cso.stencil[1].enabled;
   ds->StencilTestEnable = cso->cso.stencil[0].enabled;
   ds->StencilBufferWriteEnable =
      cso->cso.stencil[0].writemask != 0 ||
         (cso->cso.stencil[1].enabled && cso->cso.stencil[1].writemask != 0);
}

static void
emit_vertex_buffer_state(struct crocus_batch *batch,
                         unsigned buffer_id,
                         struct crocus_bo *bo,
                         unsigned start_offset,
                         unsigned end_offset,
                         unsigned stride,
                         unsigned step_rate,
                         uint32_t **map)
{
   const unsigned vb_dwords = GENX(VERTEX_BUFFER_STATE_length);
   _crocus_pack_state(batch, GENX(VERTEX_BUFFER_STATE), *map, vb) {
      vb.BufferStartingAddress = ro_bo(bo, start_offset);
      vb.VertexBufferIndex = buffer_id;
      vb.BufferPitch = stride;
#if GEN_GEN == 7
      vb.AddressModifyEnable = true;
#endif
#if GEN_GEN >= 6
      vb.MOCS = mocs(bo, &batch->screen->isl_dev);
#endif
      vb.BufferAccessType = step_rate ? INSTANCEDATA : VERTEXDATA;
      vb.InstanceDataStepRate = step_rate;
#if GEN_GEN >= 5
      vb.EndAddress = ro_bo(bo, end_offset - 1);
#endif
   }
   *map += vb_dwords;
}

static void
crocus_upload_dirty_render_state(struct crocus_context *ice,
                               struct crocus_batch *batch,
                               const struct pipe_draw_info *draw)
{
   uint64_t dirty = ice->state.dirty;

   if (!(dirty & CROCUS_ALL_DIRTY_FOR_RENDER))
      return;

   struct crocus_genx_state *genx = ice->state.genx;
   struct brw_wm_prog_data *wm_prog_data = (void *)
      ice->shaders.prog[MESA_SHADER_FRAGMENT]->prog_data;

   if (dirty & CROCUS_DIRTY_CC_VIEWPORT) {
      const struct crocus_rasterizer_state *cso_rast = ice->state.cso_rast;
      uint32_t cc_vp_address;

      /* XXX: could avoid streaming for depth_clip [0,1] case. */
      uint32_t *cc_vp_map =
         stream_state(batch,
                      4 * ice->state.num_viewports *
                      GENX(CC_VIEWPORT_length), 32, &cc_vp_address);
      for (int i = 0; i < ice->state.num_viewports; i++) {
         float zmin, zmax;
         crocus_viewport_zmin_zmax(&ice->state.viewports[i], cso_rast->clip_halfz,
                                 ice->state.window_space_position,
                                 &zmin, &zmax);
         if (cso_rast->depth_clip_near)
            zmin = 0.0;
         if (cso_rast->depth_clip_far)
            zmax = 1.0;

         crocus_pack_state(GENX(CC_VIEWPORT), cc_vp_map, ccv) {
            ccv.MinimumDepth = zmin;
            ccv.MaximumDepth = zmax;
         }

         cc_vp_map += GENX(CC_VIEWPORT_length);
      }

#if GEN_GEN >= 7
      crocus_emit_cmd(batch, GENX(3DSTATE_VIEWPORT_STATE_POINTERS_CC), ptr) {
         ptr.CCViewportPointer = cc_vp_address;
      }
#elif GEN_GEN == 6
      crocus_emit_cmd(batch, GENX(3DSTATE_VIEWPORT_STATE_POINTERS), vp) {
         vp.CCViewportStateChange = 1;
         vp.PointertoCC_VIEWPORT = cc_vp_address;
      }
#else
      ice->state.cc_vp_address = cc_vp_address;
      dirty |= CROCUS_DIRTY_COLOR_CALC_STATE;
#endif
   }

// TODO gen6
   if (dirty & CROCUS_DIRTY_SF_CL_VIEWPORT) {
      struct pipe_framebuffer_state *cso_fb = &ice->state.framebuffer;
#if GEN_GEN == 7
      uint32_t sf_cl_vp_address;
      uint32_t *vp_map =
         stream_state(batch,
                      4 * ice->state.num_viewports *
                      GENX(SF_CLIP_VIEWPORT_length), 64, &sf_cl_vp_address);
#else
      uint32_t *vp_map =
         stream_state(batch,
                      4 * ice->state.num_viewports * GENX(SF_VIEWPORT_length),
                      32, &ice->state.sf_vp_address);
      uint32_t *clip_map =
         stream_state(batch,
                      4 * ice->state.num_viewports * GENX(CLIP_VIEWPORT_length),
                      32, &ice->state.clip_vp_address);
#endif

      for (unsigned i = 0; i < ice->state.num_viewports; i++) {
         const struct pipe_viewport_state *state = &ice->state.viewports[i];
         float gb_xmin, gb_xmax, gb_ymin, gb_ymax;

         float vp_xmin = viewport_extent(state, 0, -1.0f);
         float vp_xmax = viewport_extent(state, 0,  1.0f);
         float vp_ymin = viewport_extent(state, 1, -1.0f);
         float vp_ymax = viewport_extent(state, 1,  1.0f);

         intel_calculate_guardband_size(cso_fb->width, cso_fb->height,
                                        state->scale[0], state->scale[1],
                                        state->translate[0], state->translate[1],
                                        &gb_xmin, &gb_xmax, &gb_ymin, &gb_ymax);
#if GEN_GEN == 7
         crocus_pack_state(GENX(SF_CLIP_VIEWPORT), vp_map, vp)
#else
         crocus_pack_state(GENX(SF_VIEWPORT), vp_map, vp)
#endif
            {
            vp.ViewportMatrixElementm00 = state->scale[0];
            vp.ViewportMatrixElementm11 = state->scale[1];
            vp.ViewportMatrixElementm22 = state->scale[2];
            vp.ViewportMatrixElementm30 = state->translate[0];
            vp.ViewportMatrixElementm31 = state->translate[1];
            vp.ViewportMatrixElementm32 = state->translate[2];
#if GEN_GEN < 6
            vp.ScissorRectangle.ScissorRectangleXMin = ice->state.scissors[0].minx;
            vp.ScissorRectangle.ScissorRectangleXMax = ice->state.scissors[0].maxx;
            vp.ScissorRectangle.ScissorRectangleYMin = ice->state.scissors[0].miny;
            vp.ScissorRectangle.ScissorRectangleYMax = ice->state.scissors[0].maxy;
#endif

#if GEN_GEN == 7
            vp.XMinClipGuardband = gb_xmin;
            vp.XMaxClipGuardband = gb_xmax;
            vp.YMinClipGuardband = gb_ymin;
            vp.YMaxClipGuardband = gb_ymax;
#endif
         }
#if GEN_GEN < 7
         crocus_pack_state(GENX(CLIP_VIEWPORT), clip_map, clip) {
            clip.XMinClipGuardband = gb_xmin;
            clip.XMaxClipGuardband = gb_xmax;
            clip.YMinClipGuardband = gb_ymin;
            clip.YMaxClipGuardband = gb_ymax;
         }
#endif
#if GEN_GEN == 7
         vp_map += GENX(SF_CLIP_VIEWPORT_length);
#else
         vp_map += GENX(SF_VIEWPORT_length);
         clip_map += GENX(CLIP_VIEWPORT_length);
#endif
      }
#if GEN_GEN == 7
      crocus_emit_cmd(batch, GENX(3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP), ptr) {
         ptr.SFClipViewportPointer = sf_cl_vp_address;
      }
#elif GEN_GEN == 6
      crocus_emit_cmd(batch, GENX(3DSTATE_VIEWPORT_STATE_POINTERS), vp) {
         vp.SFViewportStateChange = 1;
         vp.CLIPViewportStateChange = 1;
         vp.PointertoCLIP_VIEWPORT = ice->state.clip_vp_address;
         vp.PointertoSF_VIEWPORT = ice->state.sf_vp_address;
      }
#endif
   }

#if GEN_GEN == 7
   emit_l3_state(batch);
#endif
   if (dirty & CROCUS_DIRTY_URB) {
      unsigned size[4];

      for (int i = MESA_SHADER_VERTEX; i <= MESA_SHADER_GEOMETRY; i++) {
         if (!ice->shaders.prog[i]) {
            size[i] = 1;
         } else {
            struct brw_vue_prog_data *vue_prog_data =
               (void *) ice->shaders.prog[i]->prog_data;
            size[i] = vue_prog_data->urb_entry_size;
         }
         assert(size[i] != 0);
      }

      genX(emit_urb_setup)(ice, batch, size,
                           ice->shaders.prog[MESA_SHADER_TESS_EVAL] != NULL,
                           ice->shaders.prog[MESA_SHADER_GEOMETRY] != NULL);
   }

   if (dirty & CROCUS_DIRTY_BLEND_STATE) {
#if GEN_GEN >= 6
      struct crocus_blend_state *cso_blend = ice->state.cso_blend;
      struct pipe_framebuffer_state *cso_fb = &ice->state.framebuffer;
      struct crocus_depth_stencil_alpha_state *cso_zsa = ice->state.cso_zsa;

      STATIC_ASSERT(GENX(BLEND_STATE_ENTRY_length) == 2);

      const int rt_dwords =
         MAX2(cso_fb->nr_cbufs, 1) * GENX(BLEND_STATE_ENTRY_length);

      uint32_t blend_offset;
      uint32_t *blend_map =
         stream_state(batch,
                      4 * rt_dwords, 64, &blend_offset);

      uint32_t blend_state_entry[2];
      crocus_pack_state(GENX(BLEND_STATE_ENTRY), blend_state_entry, bs) {
         bs.AlphaTestEnable = cso_zsa->cso.alpha_enabled;
         bs.AlphaTestFunction = translate_compare_func(cso_zsa->cso.alpha_func);
      }

      memcpy(blend_map, cso_blend->blend_state, 4 * rt_dwords);
      for (int i = 0; i < cso_fb->nr_cbufs; i++)
         blend_map[i * 2 + 1] |= blend_state_entry[1];

#if GEN_GEN < 7
      crocus_emit_cmd(batch, GENX(3DSTATE_CC_STATE_POINTERS), ptr) {
         ptr.PointertoBLEND_STATE = blend_offset;
         ptr.BLEND_STATEChange = true;
      }
#else
      crocus_emit_cmd(batch, GENX(3DSTATE_BLEND_STATE_POINTERS), ptr) {
         ptr.BlendStatePointer = blend_offset;
      }
#endif
#endif
   }

   if (dirty & CROCUS_DIRTY_COLOR_CALC_STATE) {
      struct crocus_depth_stencil_alpha_state *cso = ice->state.cso_zsa;
      UNUSED struct crocus_blend_state *cso_blend = ice->state.cso_blend;
      struct pipe_stencil_ref *p_stencil_refs = &ice->state.stencil_ref;
      uint32_t cc_offset;
      void *cc_map =
         stream_state(batch,
                      sizeof(uint32_t) * GENX(COLOR_CALC_STATE_length),
                      64, &cc_offset);
#if GEN_GEN <= 5
      dirty |= CROCUS_DIRTY_GEN5_PIPELINED_POINTERS;
      int blend_idx = 0;

      if (cso_blend->blend_state.independent_blend_enable) {
         for (unsigned i = 0; i < PIPE_MAX_COLOR_BUFS; i++) {
            if (cso_blend->blend_state.rt[i].blend_enable) {
               blend_idx = i;
               break;
            }
         }
      }
      const struct pipe_rt_blend_state *rt = &cso_blend->blend_state.rt[blend_idx];
#endif
      _crocus_pack_state(batch, GENX(COLOR_CALC_STATE), cc_map, cc) {
         cc.AlphaTestFormat = ALPHATEST_FLOAT32;
         cc.AlphaReferenceValueAsFLOAT32 = cso->cso.alpha_ref_value;

#if GEN_GEN <= 5

         set_depth_stencil_bits(ice, &cc);

         cc.ColorBufferBlendEnable = rt->blend_enable;

         cc.LogicOpEnable = cso_blend->blend_state.logicop_enable;
         cc.LogicOpFunction = cso_blend->blend_state.logicop_func;
         cc.ColorDitherEnable = cso_blend->blend_state.dither;
         cc.ColorBlendFunction = rt->rgb_func;
         cc.AlphaBlendFunction = rt->alpha_func;
         cc.SourceBlendFactor = rt->rgb_src_factor;
         cc.SourceAlphaBlendFactor = rt->alpha_src_factor;
         cc.DestinationBlendFactor = rt->rgb_dst_factor;
         cc.DestinationAlphaBlendFactor = rt->alpha_dst_factor;

         if (rt->rgb_func != rt->alpha_func ||
             rt->rgb_src_factor != rt->alpha_src_factor ||
             rt->rgb_dst_factor != rt->alpha_dst_factor)
            cc.IndependentAlphaBlendEnable = true;

         if (cso->cso.alpha_enabled && ice->state.framebuffer.nr_cbufs <= 1) {
            cc.AlphaTestEnable = cso->cso.alpha_enabled;
            cc.AlphaTestFunction = translate_compare_func(cso->cso.alpha_func);
         }
	 cc.StatisticsEnable = ice->state.stats_wm ? 1 : 0;
	 cc.CCViewportStatePointer = ro_bo(batch->state.bo, ice->state.cc_vp_address);
#else
         cc.AlphaTestFormat = ALPHATEST_FLOAT32;
         cc.AlphaReferenceValueAsFLOAT32 = cso->cso.alpha_ref_value;

         cc.BlendConstantColorRed   = ice->state.blend_color.color[0];
         cc.BlendConstantColorGreen = ice->state.blend_color.color[1];
         cc.BlendConstantColorBlue  = ice->state.blend_color.color[2];
         cc.BlendConstantColorAlpha = ice->state.blend_color.color[3];
#endif
         cc.StencilReferenceValue = p_stencil_refs->ref_value[0];
         cc.BackfaceStencilReferenceValue = p_stencil_refs->ref_value[1];
      }
      ice->shaders.cc_offset = cc_offset;
#if GEN_GEN >= 6
      crocus_emit_cmd(batch, GENX(3DSTATE_CC_STATE_POINTERS), ptr) {
         ptr.ColorCalcStatePointer = cc_offset;
#if GEN_GEN != 7
         ptr.ColorCalcStatePointerValid = true;
#endif
      }
#endif
   }
#if GEN_GEN <= 5
   if (dirty & CROCUS_DIRTY_GEN4_CONSTANT_COLOR) {
      crocus_emit_cmd(batch, GENX(3DSTATE_CONSTANT_COLOR), blend_cc) {
         blend_cc.BlendConstantColorRed = ice->state.blend_color.color[0];
         blend_cc.BlendConstantColorGreen = ice->state.blend_color.color[1];
         blend_cc.BlendConstantColorBlue = ice->state.blend_color.color[2];
         blend_cc.BlendConstantColorAlpha = ice->state.blend_color.color[3];
      }
   }
#endif
   for (int stage = 0; stage <= MESA_SHADER_FRAGMENT; stage++) {
      if (!(dirty & (CROCUS_DIRTY_CONSTANTS_VS << stage)))
         continue;

      struct crocus_shader_state *shs = &ice->state.shaders[stage];
      struct crocus_compiled_shader *shader = ice->shaders.prog[stage];

      if (!shader)
         continue;

      if (shs->sysvals_need_upload)
         upload_sysvals(ice, stage);

      struct push_bos push_bos = {};
      setup_constant_buffers(ice, batch, stage, &push_bos);

#if GEN_GEN >= 7
      emit_push_constant_packets(ice, batch, stage, &push_bos);
#endif
   }

   for (int stage = 0; stage <= MESA_SHADER_FRAGMENT; stage++) {
      if (dirty & (CROCUS_DIRTY_BINDINGS_VS << stage)) {
         if (ice->shaders.prog[stage]) {
#if GEN_GEN <= 6
            dirty |= CROCUS_DIRTY_GEN5_BINDING_TABLE_POINTERS;
#endif
            crocus_populate_binding_table(ice, batch, stage);
	    ice->shaders.prog[stage]->bind_bo_offset = crocus_upload_binding_table(ice, batch,
                                                                                   ice->shaders.prog[stage]->surf_offset,
                                                                                   ice->shaders.prog[stage]->bt.size_bytes);

#if GEN_GEN == 7
	    crocus_emit_cmd(batch, GENX(3DSTATE_BINDING_TABLE_POINTERS_VS), ptr) {
	       ptr._3DCommandSubOpcode = 38 + stage;
	       ptr.PointertoVSBindingTable = ice->shaders.prog[stage]->bind_bo_offset;
	    }
#endif
         }
      }
   }
#if GEN_GEN <= 6
   if (dirty & CROCUS_DIRTY_GEN5_BINDING_TABLE_POINTERS) {
      crocus_emit_cmd(batch, GENX(3DSTATE_BINDING_TABLE_POINTERS), ptr) {
         ptr.PointertoVSBindingTable = ice->shaders.prog[MESA_SHADER_VERTEX]->bind_bo_offset;
         ptr.PointertoPSBindingTable = ice->shaders.prog[MESA_SHADER_FRAGMENT]->bind_bo_offset;
#if GEN_GEN == 6
         ptr.VSBindingTableChange = true;
         ptr.PSBindingTableChange = true;
#endif
      }
   }
#endif

   for (int stage = 0; stage <= MESA_SHADER_FRAGMENT; stage++) {
      if (!(dirty & (CROCUS_DIRTY_SAMPLER_STATES_VS << stage)) ||
          !ice->shaders.prog[stage])
         continue;

      crocus_upload_sampler_states(ice, batch, stage);

#if GEN_GEN >= 7
      struct crocus_shader_state *shs = &ice->state.shaders[stage];
      struct pipe_resource *res = shs->sampler_table.res;
      //      if (res)
      //         crocus_use_pinned_bo(batch, crocus_resource_bo(res), false);

      crocus_emit_cmd(batch, GENX(3DSTATE_SAMPLER_STATE_POINTERS_VS), ptr) {
         ptr._3DCommandSubOpcode = 43 + stage;
         ptr.PointertoVSSamplerState = shs->sampler_table.offset;
      }
#endif
   }

   if (dirty & CROCUS_DIRTY_MULTISAMPLE) {
#if GEN_GEN >= 6
      crocus_emit_cmd(batch, GENX(3DSTATE_MULTISAMPLE), ms) {
         ms.PixelLocation =
            ice->state.cso_rast->half_pixel_center ? CENTER : UL_CORNER;
         if (ice->state.framebuffer.samples > 0)
            ms.NumberofMultisamples = ffs(ice->state.framebuffer.samples) - 1;
#if GEN_GEN == 6
	 INTEL_SAMPLE_POS_4X(ms.Sample);
#elif GEN_GEN == 7
	 switch (ice->state.framebuffer.samples) {
	 case 1:
	    INTEL_SAMPLE_POS_1X(ms.Sample);
	    break;
	 case 2:
	    INTEL_SAMPLE_POS_2X(ms.Sample);
	    break;
	 case 4:
	    INTEL_SAMPLE_POS_4X(ms.Sample);
	    break;
	 case 8:
	    INTEL_SAMPLE_POS_8X(ms.Sample);
	    break;
	 default:
	    break;
	 }
#endif
      }
#endif
   }

   if (dirty & CROCUS_DIRTY_SAMPLE_MASK) {
#if GEN_GEN >= 6
      crocus_emit_cmd(batch, GENX(3DSTATE_SAMPLE_MASK), ms) {
         ms.SampleMask = ice->state.sample_mask & (GEN_GEN == 6 ? 0xf : 0xff);
      }
#endif
   }

   for (int stage = 0; stage <= MESA_SHADER_FRAGMENT; stage++) {
      if (!(dirty & (CROCUS_DIRTY_VS << stage)))
         continue;

      struct crocus_compiled_shader *shader = ice->shaders.prog[stage];

      if (shader) {
         struct brw_stage_prog_data *prog_data = shader->prog_data;

         if (prog_data->total_scratch > 0) {
            struct crocus_bo *bo =
               crocus_get_scratch_space(ice, prog_data->total_scratch, stage);
	    //            crocus_use_pinned_bo(batch, bo, true);
         }

         if (stage == MESA_SHADER_FRAGMENT) {
            UNUSED struct crocus_rasterizer_state *cso = ice->state.cso_rast;
            struct pipe_framebuffer_state *cso_fb = &ice->state.framebuffer;
	    struct brw_wm_prog_data *wm_prog_data = (void *) shader->prog_data;
#if GEN_GEN >= 7
            crocus_emit_cmd(batch, GENX(3DSTATE_PS), ps) {
               ps._8PixelDispatchEnable = wm_prog_data->dispatch_8;
               ps._16PixelDispatchEnable = wm_prog_data->dispatch_16;
               ps._32PixelDispatchEnable = wm_prog_data->dispatch_32;

               ps.DispatchGRFStartRegisterForConstantSetupData0 =
                  brw_wm_prog_data_dispatch_grf_start_reg(wm_prog_data, ps, 0);
               ps.DispatchGRFStartRegisterForConstantSetupData1 =
                  brw_wm_prog_data_dispatch_grf_start_reg(wm_prog_data, ps, 1);
               ps.DispatchGRFStartRegisterForConstantSetupData2 =
                  brw_wm_prog_data_dispatch_grf_start_reg(wm_prog_data, ps, 2);

               ps.KernelStartPointer0 = KSP(ice, shader) +
                  brw_wm_prog_data_prog_offset(wm_prog_data, ps, 0);
               ps.KernelStartPointer1 = KSP(ice, shader) +
                  brw_wm_prog_data_prog_offset(wm_prog_data, ps, 1);
               ps.KernelStartPointer2 = KSP(ice, shader) +
                  brw_wm_prog_data_prog_offset(wm_prog_data, ps, 2);

	       // XXX: WABTPPrefetchDisable, see above, drop at C0
	       ps.BindingTableEntryCount = shader->bt.size_bytes / 4;
	       ps.FloatingPointMode = prog_data->use_alt_mode;
	       ps.MaximumNumberofThreads = batch->screen->devinfo.max_wm_threads - 1;

	       ps.PushConstantEnable = prog_data->ubo_ranges[0].length > 0;

	       ps.oMaskPresenttoRenderTarget = wm_prog_data->uses_omask;
	       ps.AttributeEnable = (wm_prog_data->num_varying_inputs != 0);
	       /* From the documentation for this packet:
		* "If the PS kernel does not need the Position XY Offsets to
		*  compute a Position Value, then this field should be programmed
		*  to POSOFFSET_NONE."
		*
		* "SW Recommendation: If the PS kernel needs the Position Offsets
		*  to compute a Position XY value, this field should match Position
		*  ZW Interpolation Mode to ensure a consistent position.xyzw
		*  computation."
		*
		* We only require XY sample offsets. So, this recommendation doesn't
		* look useful at the moment.  We might need this in future.
		*/
	       ps.PositionXYOffsetSelect =
		  wm_prog_data->uses_pos_offset ? POSOFFSET_SAMPLE : POSOFFSET_NONE;
            }

	    //            uint32_t *shader_ps = (uint32_t *) shader->derived_data;
	    //            crocus_emit_merge(batch, shader_ps, ps_state,
	    //                            GENX(3DSTATE_PS_length));
#endif
         } else {
            crocus_batch_emit(batch, shader->derived_data,
                            crocus_derived_program_state_size(stage));
// TODO: VS constants for gen6
         }
      } else {
         if (stage == MESA_SHADER_TESS_EVAL) {
#if GEN_GEN >= 7
            crocus_emit_cmd(batch, GENX(3DSTATE_HS), hs);
            crocus_emit_cmd(batch, GENX(3DSTATE_TE), te);
            crocus_emit_cmd(batch, GENX(3DSTATE_DS), ds);
#endif
         } else if (stage == MESA_SHADER_GEOMETRY) {
#if GEN_GEN >= 6
            crocus_emit_cmd(batch, GENX(3DSTATE_GS), gs);
#endif
         }
      }
   }

#if GEN_GEN >= 7
   // XXX what about SO for earlier gens?
   if (ice->state.streamout_active) {
      if (dirty & CROCUS_DIRTY_SO_BUFFERS) {
         crocus_batch_emit(batch, genx->so_buffers,
                         4 * 4 * GENX(3DSTATE_SO_BUFFER_length));
         for (int i = 0; i < 4; i++) {
            struct crocus_stream_output_target *tgt =
               (void *) ice->state.so_target[i];
            if (tgt) {
               tgt->zeroed = true;
	       //               crocus_use_pinned_bo(batch, crocus_resource_bo(tgt->base.buffer),
	       //                                  true);
	       //               crocus_use_pinned_bo(batch, crocus_resource_bo(tgt->offset.res),
	       //                                  true);
            }
         }
      }

      if ((dirty & CROCUS_DIRTY_SO_DECL_LIST) && ice->state.streamout) {
         uint32_t *decl_list =
            ice->state.streamout + GENX(3DSTATE_STREAMOUT_length);
         crocus_batch_emit(batch, decl_list, 4 * ((decl_list[0] & 0xff) + 2));
      }

      if (dirty & CROCUS_DIRTY_STREAMOUT) {
         const struct crocus_rasterizer_state *cso_rast = ice->state.cso_rast;

         uint32_t dynamic_sol[GENX(3DSTATE_STREAMOUT_length)];
         crocus_pack_command(GENX(3DSTATE_STREAMOUT), dynamic_sol, sol) {
            sol.SOFunctionEnable = true;
            sol.SOStatisticsEnable = true;

            sol.RenderingDisable = cso_rast->rasterizer_discard &&
                                   !ice->state.prims_generated_query_active;
            sol.ReorderMode = cso_rast->flatshade_first ? LEADING : TRAILING;
         }

         assert(ice->state.streamout);

         crocus_emit_merge(batch, ice->state.streamout, dynamic_sol,
                         GENX(3DSTATE_STREAMOUT_length));
      }
   } else {
      if (dirty & CROCUS_DIRTY_STREAMOUT) {
         crocus_emit_cmd(batch, GENX(3DSTATE_STREAMOUT), sol);
      }
   }
#endif

#if GEN_GEN < 6
   recalculate_urb_fence(batch);
#endif

   if (dirty & CROCUS_DIRTY_CLIP) {
      struct crocus_rasterizer_state *cso_rast = ice->state.cso_rast;
      struct pipe_framebuffer_state *cso_fb = &ice->state.framebuffer;

      bool gs_or_tes = ice->shaders.prog[MESA_SHADER_GEOMETRY] ||
                       ice->shaders.prog[MESA_SHADER_TESS_EVAL];
      bool points_or_lines = cso_rast->fill_mode_point_or_line ||
         (gs_or_tes ? ice->shaders.output_topology_is_points_or_lines
                    : ice->state.prim_is_points_or_lines);

#if GEN_GEN < 6
      const struct brw_clip_prog_data *clip_prog_data = (struct brw_clip_prog_data *)ice->shaders.clip_prog->prog_data;
      struct pipe_rasterizer_state *cso_state = &ice->state.cso_rast->state;

      uint32_t *clip_ptr = stream_state(batch, GENX(CLIP_STATE_length) * 4, 32, &ice->shaders.clip_offset);
      dirty |= CROCUS_DIRTY_GEN5_PIPELINED_POINTERS;
      _crocus_pack_state(batch, GENX(CLIP_STATE), clip_ptr, clip) {
         clip.KernelStartPointer = KSP(ice, ice->shaders.clip_prog);
         clip.FloatingPointMode = FLOATING_POINT_MODE_Alternate;
         clip.SingleProgramFlow = true;
         clip.GRFRegisterCount = DIV_ROUND_UP(clip_prog_data->total_grf, 16) - 1;

         clip.VertexURBEntryReadLength = clip_prog_data->urb_read_length;
         clip.ConstantURBEntryReadLength = clip_prog_data->curb_read_length;

         clip.DispatchGRFStartRegisterForURBData = 1;
         clip.VertexURBEntryReadOffset = 0;

         clip.NumberofURBEntries = batch->ice->urb.nr_clip_entries;
         clip.URBEntryAllocationSize = batch->ice->urb.vsize - 1;

         if (batch->ice->urb.nr_clip_entries >= 10) {
            /* Half of the URB entries go to each thread, and it has to be an
             * even number.
             */
            assert(batch->ice->urb.nr_clip_entries % 2 == 0);

            /* Although up to 16 concurrent Clip threads are allowed on Ironlake,
             * only 2 threads can output VUEs at a time.
             */
            clip.MaximumNumberofThreads = (GEN_GEN == 5 ? 16 : 2) - 1;
         } else {
            assert(batch->ice->urb.nr_clip_entries >= 5);
            clip.MaximumNumberofThreads = 1 - 1;
         }
         clip.VertexPositionSpace = VPOS_NDCSPACE;
         clip.UserClipFlagsMustClipEnable = true;
         clip.GuardbandClipTestEnable = true;

         clip.ClipperViewportStatePointer = ro_bo(batch->state.bo, ice->state.clip_vp_address);
         clip.ScreenSpaceViewportXMin = -1.0;
         clip.ScreenSpaceViewportXMax = 1.0;
         clip.ScreenSpaceViewportYMin = -1.0;
         clip.ScreenSpaceViewportYMax = 1.0;
         clip.ViewportXYClipTestEnable = true;
         clip.ViewportZClipTestEnable = (cso_state->depth_clip_near || cso_state->depth_clip_far);

         clip.UserClipDistanceClipTestEnableBitmask = cso_state->clip_plane_enable;

         clip.APIMode = cso_state->clip_halfz ? APIMODE_D3D : APIMODE_OGL;
         clip.GuardbandClipTestEnable = true;

         clip.ClipMode = clip_prog_data->clip_mode;
#if GEN_IS_G4X
         clip.NegativeWClipTestEnable = true;
#endif
      }

#else //if GEN_GEN >= 6
      uint32_t dynamic_clip[GENX(3DSTATE_CLIP_length)];
      crocus_pack_command(GENX(3DSTATE_CLIP), &dynamic_clip, cl) {
         cl.StatisticsEnable = ice->state.statistics_counters_enabled;
         if (cso_rast->rasterizer_discard)
            cl.ClipMode = CLIPMODE_REJECT_ALL;
         else if (ice->state.window_space_position)
            cl.ClipMode = CLIPMODE_ACCEPT_ALL;
         else
            cl.ClipMode = CLIPMODE_NORMAL;

         cl.PerspectiveDivideDisable = ice->state.window_space_position;
         cl.ViewportXYClipTestEnable = !points_or_lines;

         if (wm_prog_data->barycentric_interp_modes &
             BRW_BARYCENTRIC_NONPERSPECTIVE_BITS)
            cl.NonPerspectiveBarycentricEnable = true;

         cl.ForceZeroRTAIndexEnable = cso_fb->layers <= 1;
         cl.MaximumVPIndex = ice->state.num_viewports - 1;
      }
      crocus_emit_merge(batch, cso_rast->clip, dynamic_clip,
                      ARRAY_SIZE(cso_rast->clip));
#endif
   }

   if (dirty) {
      const struct brw_vue_prog_data *vue_prog_data = brw_vue_prog_data(ice->shaders.prog[MESA_SHADER_VERTEX]->prog_data);

#if GEN_GEN == 7
      if (batch->screen->devinfo.is_ivybridge)
	 gen7_emit_vs_workaround_flush(batch);
#endif

#if GEN_GEN >= 6
      crocus_emit_cmd(batch, GENX(3DSTATE_VS), vs) {
#else
      uint32_t *vs_ptr = stream_state(batch,
                                      GENX(VS_STATE_length) * 4, 32, &ice->shaders.vs_offset);
      dirty |= CROCUS_DIRTY_GEN5_PIPELINED_POINTERS;
      _crocus_pack_state(batch, GENX(VS_STATE), vs_ptr, vs) {

#endif
         vs.KernelStartPointer = KSP(ice, ice->shaders.prog[MESA_SHADER_VERTEX]);
         vs.Enable = true;

         vs.MaximumNumberofThreads = (batch->screen->devinfo.max_vs_threads / 2)- 1;
         vs.FloatingPointMode  = vue_prog_data->base.use_alt_mode;
         vs.DispatchGRFStartRegisterForURBData = vue_prog_data->base.dispatch_grf_start_reg;

         vs.VertexURBEntryReadLength = vue_prog_data->urb_read_length;

         vs.BindingTableEntryCount = ice->shaders.prog[MESA_SHADER_VERTEX]->bt.size_bytes / 4;

#if GEN_GEN < 6
         vs.GRFRegisterCount = DIV_ROUND_UP(vue_prog_data->total_grf, 16) - 1;
	 vs.ConstantURBEntryReadLength = vue_prog_data->base.curb_read_length;

         vs.NumberofURBEntries = batch->ice->urb.nr_vs_entries >> (GEN_GEN == 5 ? 2 : 0);
         vs.URBEntryAllocationSize = batch->ice->urb.vsize - 1;

         vs.MaximumNumberofThreads =
            CLAMP(batch->ice->urb.nr_vs_entries / 2, 1, batch->screen->devinfo.max_vs_threads) - 1;
         vs.StatisticsEnable = false;
         vs.SamplerStatePointer = ro_bo(batch->state.bo, ice->state.shaders[MESA_SHADER_VERTEX].sampler_offset);
#endif
#if GEN_GEN == 5
         /* Force single program flow on Ironlake.  We cannot reliably get
          * all applications working without it.  See:
          * https://bugs.freedesktop.org/show_bug.cgi?id=29172
          *
          * The most notable and reliably failing application is the Humus
          * demo "CelShading"
          */
         vs.SingleProgramFlow = true;
         vs.SamplerCount = 0; /* hardware requirement */

#endif
      }
   }
   if (dirty & CROCUS_DIRTY_RASTER) {
      struct crocus_rasterizer_state *cso = ice->state.cso_rast;

#if GEN_GEN <= 5
      if (ice->state.global_depth_offset_clamp != cso->state.offset_clamp) {
         crocus_emit_cmd(batch, GENX(3DSTATE_GLOBAL_DEPTH_OFFSET_CLAMP), clamp) {
            clamp.GlobalDepthOffsetClamp = cso->state.offset_clamp;
         }
         ice->state.global_depth_offset_clamp = cso->state.offset_clamp;
      }
#endif

#if GEN_GEN < 6
      const struct brw_sf_prog_data *sf_prog_data = (struct brw_sf_prog_data *)ice->shaders.sf_prog->prog_data;
      struct pipe_rasterizer_state *cso_state = &ice->state.cso_rast->state;
      uint32_t *sf_ptr = stream_state(batch,
                                      GENX(SF_STATE_length) * 4, 32, &ice->shaders.sf_offset);
      dirty |= CROCUS_DIRTY_GEN5_PIPELINED_POINTERS;
      _crocus_pack_state(batch, GENX(SF_STATE), sf_ptr, sf) {
         sf.KernelStartPointer = KSP(ice, ice->shaders.sf_prog);
         sf.FloatingPointMode = FLOATING_POINT_MODE_Alternate;
         sf.GRFRegisterCount = DIV_ROUND_UP(sf_prog_data->total_grf, 16) - 1;
         sf.DispatchGRFStartRegisterForURBData = 3;
         sf.VertexURBEntryReadOffset = BRW_SF_URB_ENTRY_READ_OFFSET;
         sf.VertexURBEntryReadLength = sf_prog_data->urb_read_length;
         sf.URBEntryAllocationSize = batch->ice->urb.sfsize - 1;
         sf.NumberofURBEntries = batch->ice->urb.nr_sf_entries;
         sf.PointRasterizationRule = RASTRULE_UPPER_RIGHT;

         sf.SetupViewportStateOffset = ro_bo(batch->state.bo, ice->state.sf_vp_address);

         sf.MaximumNumberofThreads =
            MIN2(GEN_GEN == 5 ? 48 : 24, batch->ice->urb.nr_sf_entries) - 1;

         sf.SpritePointEnable = cso_state->point_quad_rasterization;
         sf.DestinationOriginHorizontalBias = 0.5;
         sf.DestinationOriginVerticalBias = 0.5;

         sf.LastPixelEnable = cso_state->line_last_pixel;
         sf.LineWidth = get_line_width(cso_state);
         sf.PointWidth = cso_state->point_size;
         sf.PointWidthSource = cso_state->point_size_per_vertex ? Vertex : State;
#if GEN_IS_G4X || GEN_GEN >= 5
         sf.AALineDistanceMode = AALINEDISTANCE_TRUE;
#endif
         sf.ViewportTransformEnable = true;
         sf.FrontWinding = cso_state->front_ccw ? 1 : 0;
         sf.ScissorRectangleEnable = cso_state->scissor;
         sf.CullMode = translate_cull_mode(cso_state->cull_face);

         if (cso_state->flatshade_first) {
            sf.TriangleFanProvokingVertexSelect = 1;
         } else {
            sf.TriangleStripListProvokingVertexSelect = 2;
            sf.TriangleFanProvokingVertexSelect = 2;
            sf.LineStripListProvokingVertexSelect = 1;
         }
      }
#else
      uint32_t dynamic_sf[GENX(3DSTATE_SF_length)];
      crocus_pack_command(GENX(3DSTATE_SF), &dynamic_sf, sf) {
         sf.ViewportTransformEnable = !ice->state.window_space_position;

#if GEN_GEN == 6
         const struct brw_wm_prog_data *wm_prog_data = brw_wm_prog_data(ice->shaders.prog[MESA_SHADER_FRAGMENT]->prog_data);
         uint32_t urb_entry_read_length;
         uint32_t urb_entry_read_offset;
         uint32_t point_sprite_enables;
         calculate_attr_overrides(ice, sf.Attribute, &point_sprite_enables,
                                  &urb_entry_read_length,
                                  &urb_entry_read_offset);
         sf.VertexURBEntryReadLength = urb_entry_read_length;
         sf.VertexURBEntryReadOffset = urb_entry_read_offset;
         sf.PointSpriteTextureCoordinateEnable = point_sprite_enables;
         sf.ConstantInterpolationEnable = wm_prog_data->flat_inputs;
         sf.NumberofSFOutputAttributes = wm_prog_data->num_varying_inputs;
#endif
      }
      crocus_emit_merge(batch, cso->sf, dynamic_sf,
                      ARRAY_SIZE(dynamic_sf));
#endif
   }

   if (dirty & CROCUS_DIRTY_WM) {
      struct crocus_rasterizer_state *cso = ice->state.cso_rast;
      const struct brw_wm_prog_data *wm_prog_data = brw_wm_prog_data(ice->shaders.prog[MESA_SHADER_FRAGMENT]->prog_data);
      UNUSED bool writes_depth = wm_prog_data->computed_depth_mode != BRW_PSCDEPTH_OFF;
      UNUSED const struct shader_info *fs_info =
         crocus_get_shader_info(ice, MESA_SHADER_FRAGMENT);
#if GEN_GEN >= 6
      crocus_emit_cmd(batch, GENX(3DSTATE_WM), wm) {
#else
      uint32_t *wm_ptr = stream_state(batch,
                                      GENX(WM_STATE_length) * 4, 32, &ice->shaders.wm_offset);

      dirty |= CROCUS_DIRTY_GEN5_PIPELINED_POINTERS;

      _crocus_pack_state(batch, GENX(WM_STATE), wm_ptr, wm) {
#endif
#if GEN_GEN <= 6
	
         wm._8PixelDispatchEnable = wm_prog_data->dispatch_8;
         wm._16PixelDispatchEnable = wm_prog_data->dispatch_16;
         wm._32PixelDispatchEnable = wm_prog_data->dispatch_32;
#endif
#if GEN_GEN == 4
      /* On gen4, we only have one shader kernel */
         if (brw_wm_state_has_ksp(wm, 0)) {
            wm.KernelStartPointer0 = KSP(ice, ice->shaders.prog[MESA_SHADER_FRAGMENT]) +
            brw_wm_prog_data_prog_offset(wm_prog_data, wm, 0);
               wm.GRFRegisterCount0 = brw_wm_prog_data_reg_blocks(wm_prog_data, wm, 0);
            wm.DispatchGRFStartRegisterForConstantSetupData0 =
               wm_prog_data->base.dispatch_grf_start_reg;
         }
#elif GEN_GEN == 5
         wm.KernelStartPointer0 = KSP(ice, ice->shaders.prog[MESA_SHADER_FRAGMENT]) +
            brw_wm_prog_data_prog_offset(wm_prog_data, wm, 0);
         wm.KernelStartPointer1 = KSP(ice, ice->shaders.prog[MESA_SHADER_FRAGMENT]) +
            brw_wm_prog_data_prog_offset(wm_prog_data, wm, 1);
         wm.KernelStartPointer2 = KSP(ice, ice->shaders.prog[MESA_SHADER_FRAGMENT]) +
            brw_wm_prog_data_prog_offset(wm_prog_data, wm, 2);

         wm.GRFRegisterCount0 = brw_wm_prog_data_reg_blocks(wm_prog_data, wm, 0);
         wm.GRFRegisterCount1 = brw_wm_prog_data_reg_blocks(wm_prog_data, wm, 1);
         wm.GRFRegisterCount2 = brw_wm_prog_data_reg_blocks(wm_prog_data, wm, 2);

         wm.DispatchGRFStartRegisterForConstantSetupData0 =
            wm_prog_data->base.dispatch_grf_start_reg;
#elif GEN_GEN == 6
	 wm.KernelStartPointer0 = KSP(ice, ice->shaders.prog[MESA_SHADER_FRAGMENT]) +
            brw_wm_prog_data_prog_offset(wm_prog_data, wm, 0);
         wm.KernelStartPointer1 = KSP(ice, ice->shaders.prog[MESA_SHADER_FRAGMENT]) +
            brw_wm_prog_data_prog_offset(wm_prog_data, wm, 1);
         wm.KernelStartPointer2 = KSP(ice, ice->shaders.prog[MESA_SHADER_FRAGMENT]) +
            brw_wm_prog_data_prog_offset(wm_prog_data, wm, 2);

         wm.DispatchGRFStartRegisterForConstantSetupData0 =
	   brw_wm_prog_data_dispatch_grf_start_reg(wm_prog_data, wm, 0);
	 wm.DispatchGRFStartRegisterForConstantSetupData1 =
	   brw_wm_prog_data_dispatch_grf_start_reg(wm_prog_data, wm, 1);
	 wm.DispatchGRFStartRegisterForConstantSetupData2 =
	   brw_wm_prog_data_dispatch_grf_start_reg(wm_prog_data, wm, 2);
#endif
#if GEN_GEN <= 5
         wm.SetupURBEntryReadLength = wm_prog_data->num_varying_inputs * 2;
         wm.SetupURBEntryReadOffset = 0;
         wm.EarlyDepthTestEnable = true;
         wm.LineAntialiasingRegionWidth = _05pixels;
         wm.LineEndCapAntialiasingRegionWidth = _10pixels;
         wm.DepthCoefficientURBReadOffset = 1;

         if (cso->state.offset_tri) {
            wm.GlobalDepthOffsetEnable = true;

         /* Something weird going on with legacy_global_depth_bias,
          * offset_constant, scaling and MRD.  This value passes glean
          * but gives some odd results elsewere (eg. the
          * quad-offset-units test).
          */
            wm.GlobalDepthOffsetConstant = cso->state.offset_units * 2;
            wm.GlobalDepthOffsetScale = cso->state.offset_scale;
         }
         wm.SamplerStatePointer = ro_bo(batch->state.bo, ice->state.shaders[MESA_SHADER_FRAGMENT].sampler_offset);
#endif

	 wm.StatisticsEnable = (GEN_GEN >= 6 || ice->state.stats_wm) ? 1 : 0;

#if GEN_GEN >= 6
	 wm.LineAntialiasingRegionWidth = _10pixels;
	 wm.LineEndCapAntialiasingRegionWidth = _05pixels;

	 wm.PointRasterizationRule = RASTRULE_UPPER_RIGHT;
	 wm.BarycentricInterpolationMode = wm_prog_data->barycentric_interp_modes;
#endif
#if GEN_GEN == 6
      /* TODO
      wm.DualSourceBlendEnable =
         wm_prog_data->dual_src_blend && (ctx->Color.BlendEnabled & 1) &&
         ctx->Color.Blend[0]._UsesDualSrc;
      */
      wm.oMaskPresenttoRenderTarget = wm_prog_data->uses_omask;
      wm.NumberofSFOutputAttributes = wm_prog_data->num_varying_inputs;

      /* From the SNB PRM, volume 2 part 1, page 281:
       * "If the PS kernel does not need the Position XY Offsets
       * to compute a Position XY value, then this field should be
       * programmed to POSOFFSET_NONE."
       *
       * "SW Recommendation: If the PS kernel needs the Position Offsets
       * to compute a Position XY value, this field should match Position
       * ZW Interpolation Mode to ensure a consistent position.xyzw
       * computation."
       * We only require XY sample offsets. So, this recommendation doesn't
       * look useful at the moment. We might need this in future.
       */
      if (wm_prog_data->uses_pos_offset)
         wm.PositionXYOffsetSelect = POSOFFSET_SAMPLE;
      else
         wm.PositionXYOffsetSelect = POSOFFSET_NONE;
#endif
         wm.LineStippleEnable = cso->line_stipple_enable;
         wm.PolygonStippleEnable = cso->poly_stipple_enable;

#if GEN_GEN < 7
         if (wm_prog_data->base.use_alt_mode)
            wm.FloatingPointMode = FLOATING_POINT_MODE_Alternate;
         wm.BindingTableEntryCount = ice->shaders.prog[MESA_SHADER_FRAGMENT]->bt.size_bytes / 4;
         wm.MaximumNumberofThreads = batch->screen->devinfo.max_wm_threads - 1;
#endif

#if GEN_GEN >= 6
	 wm.PixelShaderUsesSourceW = wm_prog_data->uses_src_w;

	 wm.MultisampleRasterizationMode = MSRASTMODE_OFF_PIXEL;
         wm.MultisampleDispatchMode = MSDISPMODE_PERSAMPLE;
#endif

	 wm.PixelShaderUsesSourceDepth = wm_prog_data->uses_src_depth;

         if (wm_prog_data->uses_kill ||
             ice->state.cso_zsa->cso.alpha_enabled ||
             ice->state.cso_blend->alpha_to_coverage ||
             GEN_GEN >= 6 && wm_prog_data->uses_omask)
            wm.PixelShaderKillsPixel = true;

         if (has_writeable_rt(ice->state.cso_blend, fs_info) || writes_depth || wm.PixelShaderKillsPixel ||
             (GEN_GEN >= 6 && wm_prog_data->has_side_effects))
            wm.ThreadDispatchEnable = true;

#if GEN_GEN >= 7
	 wm.PixelShaderComputedDepthMode = wm_prog_data->computed_depth_mode;
	 wm.PixelShaderUsesInputCoverageMask = wm_prog_data->uses_sample_mask;
#else
	 wm.PixelShaderComputedDepth = writes_depth;

#endif
	 /* The "UAV access enable" bits are unnecessary on HSW because they only
	  * seem to have an effect on the HW-assisted coherency mechanism which we
	  * don't need, and the rasterization-related UAV_ONLY flag and the
	  * DISPATCH_ENABLE bit can be set independently from it.
	  * C.f. gen8_upload_ps_extra().
	  *
	  * BRW_NEW_FRAGMENT_PROGRAM | BRW_NEW_FS_PROG_DATA | _NEW_BUFFERS |
	  * _NEW_COLOR
	  */
#if GEN_VERSIONx10 == 75
	 // TODO HSW
	 //	 if (!(brw_color_buffer_write_enabled(brw) || writes_depth) &&
	 //          wm_prog_data->has_side_effects)
	 //         wm.PSUAVonly = ON;
#endif

#if GEN_GEN >= 7
      /* BRW_NEW_FS_PROG_DATA */
	 if (wm_prog_data->early_fragment_tests)
	   wm.EarlyDepthStencilControl = EDSC_PREPS;
	 else if (wm_prog_data->has_side_effects)
	   wm.EarlyDepthStencilControl = EDSC_PSEXEC;
#endif
      };
   }

#if GEN_GEN >= 7
   if (dirty & CROCUS_DIRTY_SBE) {
      crocus_emit_sbe(batch, ice);
   }
#endif

   if (dirty & CROCUS_DIRTY_WM_DEPTH_STENCIL) {
#if GEN_GEN >= 6
      uint32_t ds_offset;
      void *ds_map = stream_state(batch,
                                  sizeof(uint32_t) * GENX(DEPTH_STENCIL_STATE_length),
                                  64, &ds_offset);
      struct crocus_depth_stencil_alpha_state *cso = ice->state.cso_zsa;

      _crocus_pack_state(batch, GENX(DEPTH_STENCIL_STATE), ds_map, ds) {
         set_depth_stencil_bits(ice, &ds);
      }

#if GEN_GEN == 6
      crocus_emit_cmd(batch, GENX(3DSTATE_CC_STATE_POINTERS), ptr) {
         ptr.PointertoDEPTH_STENCIL_STATE = ds_offset;
         ptr.DEPTH_STENCIL_STATEChange = true;
      }
#else
      crocus_emit_cmd(batch, GENX(3DSTATE_DEPTH_STENCIL_STATE_POINTERS), ptr) {
         ptr.PointertoDEPTH_STENCIL_STATE = ds_offset;
      }
#endif
#endif
   }

   if (dirty & CROCUS_DIRTY_SCISSOR_RECT) {
#if GEN_GEN >= 6
      uint32_t scissor_offset =
         emit_state(batch,
                    ice->state.scissors,
                    sizeof(struct pipe_scissor_state) *
                    ice->state.num_viewports, 32);

      crocus_emit_cmd(batch, GENX(3DSTATE_SCISSOR_STATE_POINTERS), ptr) {
         ptr.ScissorRectPointer = scissor_offset;
      }
#endif
   }

   if (dirty & CROCUS_DIRTY_DEPTH_BUFFER) {
      struct crocus_depth_buffer_state *cso_z = &ice->state.genx->depth_buffer;
      struct isl_device *isl_dev = &batch->screen->isl_dev;
#if GEN_GEN == 7
      gen7_depth_flush(batch);
#endif
      void *batch_ptr;
      struct crocus_resource *zres, *sres;
      struct pipe_framebuffer_state *cso = &ice->state.framebuffer;
      batch_ptr = crocus_get_command_space(batch, isl_dev->ds.size);

      struct isl_view view = {
                              .base_level = 0,
                              .levels = 1,
                              .base_array_layer = 0,
                              .array_len = 1,
                              .swizzle = ISL_SWIZZLE_IDENTITY,
      };
      struct isl_depth_stencil_hiz_emit_info info = { .view = &view };

      if (cso->zsbuf) {
         crocus_get_depth_stencil_resources(&batch->screen->devinfo, cso->zsbuf->texture, &zres, &sres);

         info.depth_address = crocus_command_reloc(batch,
                                                   (batch_ptr - batch->command.map) + isl_dev->ds.depth_offset,
                                                   zres->bo, 0, RELOC_32BIT);
         info.depth_surf = &zres->surf;
         info.mocs = mocs(zres->bo, isl_dev);

         view.base_level = cso->zsbuf->u.tex.level;
         view.base_array_layer = cso->zsbuf->u.tex.first_layer;
         view.usage |= ISL_SURF_USAGE_DEPTH_BIT;
         view.format = zres->surf.format;
      }
      isl_emit_depth_stencil_hiz_s(isl_dev, batch_ptr, &info);
   }
   if (dirty & (CROCUS_DIRTY_DEPTH_BUFFER | CROCUS_DIRTY_WM_DEPTH_STENCIL)) {
      /* Listen for buffer changes, and also write enable changes. */
      struct pipe_framebuffer_state *cso_fb = &ice->state.framebuffer;
      // TODO: emit things?
   }

   if (dirty & CROCUS_DIRTY_POLYGON_STIPPLE) {
#if 0
      crocus_emit_cmd(batch, GENX(3DSTATE_POLY_STIPPLE_PATTERN), poly) {
         for (int i = 0; i < 32; i++) {
            poly.PatternRow[i] = ice->state.poly_stipple.stipple[i];
         }
      }
#endif
   }

   if (dirty & CROCUS_DIRTY_LINE_STIPPLE) {
#if 0
      struct crocus_rasterizer_state *cso = ice->state.cso_rast;
      crocus_batch_emit(batch, cso->line_stipple, sizeof(cso->line_stipple));
#endif
   }

#if GEN_GEN <= 5
   if (dirty & CROCUS_DIRTY_GEN5_PIPELINED_POINTERS)
      upload_pipelined_state_pointers(batch, ice->shaders.vs_offset, ice->shaders.sf_offset, ice->shaders.clip_offset, ice->shaders.wm_offset, ice->shaders.cc_offset);
   crocus_upload_urb_fence(batch);

   crocus_emit_cmd(batch, GENX(CS_URB_STATE), cs);
   crocus_emit_cmd(batch, GENX(CONSTANT_BUFFER), cb);
#endif
   if (dirty & CROCUS_DIRTY_DRAWING_RECTANGLE) {
      struct pipe_framebuffer_state *fb = &ice->state.framebuffer;
      crocus_emit_cmd(batch, GENX(3DSTATE_DRAWING_RECTANGLE), rect) {
         rect.ClippedDrawingRectangleXMax = fb->width - 1;
         rect.ClippedDrawingRectangleYMax = fb->height - 1;
      }
   }

   if (dirty & CROCUS_DIRTY_VERTEX_BUFFERS) {
      const uint32_t user_count = util_bitcount(ice->state.bound_vertex_buffers);
      const uint32_t count = user_count +
         ice->state.vs_uses_draw_params + ice->state.vs_uses_derived_draw_params;
      uint32_t dynamic_bound = ice->state.bound_vertex_buffers;

      if (count) {
         const unsigned vb_dwords = GENX(VERTEX_BUFFER_STATE_length);

         uint32_t *map =
            crocus_get_command_space(batch, 4 * (1 + vb_dwords * count));
         _crocus_pack_command(batch, GENX(3DSTATE_VERTEX_BUFFERS), map, vb) {
            vb.DWordLength = (vb_dwords * count + 1) - 2;
         }
         map += 1;

         uint32_t bound = dynamic_bound;
         int i;
         while (bound) {
            i = u_bit_scan(&bound);
            struct pipe_vertex_buffer *buf = &ice->state.vertex_buffers[i];
            struct crocus_resource *res = (struct crocus_resource *)buf->buffer.resource;
            uint32_t step_rate = 0;
            const unsigned padding =
               (!(GEN_VERSIONx10 == 75) && !batch->screen->devinfo.is_baytrail) * 2;
            const unsigned end = res->base.width0 + padding;
            for (unsigned ve = 0; ve < ice->state.cso_vertex_elements->count; ve++)
               if (ice->state.cso_vertex_elements->vbo_index[ve] == i)
                  step_rate = ice->state.cso_vertex_elements->instance_divisor[ve];

            emit_vertex_buffer_state(batch, i, res->bo,
                                     buf->buffer_offset,
                                     end,
                                     buf->stride,
                                     step_rate,
                                     &map);
         }
         i = user_count;
         if (ice->state.vs_uses_draw_params) {
            struct crocus_resource *res = (struct crocus_resource *)ice->draw.draw_params.res;
            emit_vertex_buffer_state(batch, i++,
                                     res->bo,
                                     ice->draw.draw_params.offset,
                                     ice->draw.draw_params.res->width0,
                                     0, 0, &map);
         }
         if (ice->state.vs_uses_derived_draw_params) {
            struct crocus_resource *res = (struct crocus_resource *)ice->draw.derived_draw_params.res;
            emit_vertex_buffer_state(batch, i++,
                                     res->bo,
                                     ice->draw.derived_draw_params.offset,
                                     ice->draw.derived_draw_params.res->width0,
                                     0, 0, &map);
         }
      }
   }

   if (dirty & CROCUS_DIRTY_VERTEX_ELEMENTS) {
      struct crocus_vertex_element_state *cso = ice->state.cso_vertex_elements;
      const unsigned entries = MAX2(cso->count, 1);
      if (!(ice->state.vs_needs_sgvs_element ||
            ice->state.vs_uses_derived_draw_params ||
            ice->state.vs_needs_edge_flag)) {
         crocus_batch_emit(batch, cso->vertex_elements, sizeof(uint32_t) *
                         (1 + entries * GENX(VERTEX_ELEMENT_STATE_length)));
      } else {
         uint32_t dynamic_ves[1 + 33 * GENX(VERTEX_ELEMENT_STATE_length)];
         const unsigned dyn_count = cso->count +
            ice->state.vs_needs_sgvs_element +
            ice->state.vs_uses_derived_draw_params;

         crocus_pack_command(GENX(3DSTATE_VERTEX_ELEMENTS),
                           &dynamic_ves, ve) {
            ve.DWordLength =
               1 + GENX(VERTEX_ELEMENT_STATE_length) * dyn_count - 2;
         }
         memcpy(&dynamic_ves[1], &cso->vertex_elements[1],
                (cso->count - ice->state.vs_needs_edge_flag) *
                GENX(VERTEX_ELEMENT_STATE_length) * sizeof(uint32_t));
         uint32_t *ve_pack_dest =
            &dynamic_ves[1 + (cso->count - ice->state.vs_needs_edge_flag) *
                         GENX(VERTEX_ELEMENT_STATE_length)];

         if (ice->state.vs_needs_sgvs_element) {
            uint32_t base_ctrl = ice->state.vs_uses_draw_params ?
                                 VFCOMP_STORE_SRC : VFCOMP_STORE_0;
            crocus_pack_state(GENX(VERTEX_ELEMENT_STATE), ve_pack_dest, ve) {
               ve.Valid = true;
               ve.VertexBufferIndex =
                  util_bitcount64(ice->state.bound_vertex_buffers);
               ve.SourceElementFormat = ISL_FORMAT_R32G32_UINT;
               ve.Component0Control = base_ctrl;
               ve.Component1Control = base_ctrl;
               ve.Component2Control = ice->state.vs_uses_vertexid ? VFCOMP_STORE_VID : VFCOMP_STORE_0;
               ve.Component3Control = ice->state.vs_uses_instanceid ? VFCOMP_STORE_IID : VFCOMP_STORE_0;
#if GEN_GEN < 5
               ve.DestinationElementOffset = cso->count * 4;
#endif
            }
            ve_pack_dest += GENX(VERTEX_ELEMENT_STATE_length);
         }
         if (ice->state.vs_uses_derived_draw_params) {
            crocus_pack_state(GENX(VERTEX_ELEMENT_STATE), ve_pack_dest, ve) {
               ve.Valid = true;
               ve.VertexBufferIndex =
                  util_bitcount64(ice->state.bound_vertex_buffers) +
                  ice->state.vs_uses_draw_params;
               ve.SourceElementFormat = ISL_FORMAT_R32G32_UINT;
               ve.Component0Control = VFCOMP_STORE_SRC;
               ve.Component1Control = VFCOMP_STORE_SRC;
               ve.Component2Control = VFCOMP_STORE_0;
               ve.Component3Control = VFCOMP_STORE_0;
#if GEN_GEN < 5
               ve.DestinationElementOffset = (cso->count + ice->state.vs_needs_sgvs_element) * 4;
#endif
            }
            ve_pack_dest += GENX(VERTEX_ELEMENT_STATE_length);
         }
         if (ice->state.vs_needs_edge_flag) {
            for (int i = 0; i < GENX(VERTEX_ELEMENT_STATE_length);  i++)
               ve_pack_dest[i] = cso->edgeflag_ve[i];
         }

         crocus_batch_emit(batch, &dynamic_ves, sizeof(uint32_t) *
                         (1 + dyn_count * GENX(VERTEX_ELEMENT_STATE_length)));
      }
   }

#if GEN_VERSIONx10 == 75
   if (dirty & CROCUS_DIRTY_VF) {
      crocus_emit_cmd(batch, GENX(3DSTATE_VF), vf) {
         if (draw->primitive_restart) {
            vf.IndexedDrawCutIndexEnable = true;
            vf.CutIndex = draw->restart_index;
         }
      }
   }
#endif

   if (dirty & CROCUS_DIRTY_VF_STATISTICS) {
      crocus_emit_cmd(batch, GENX(3DSTATE_VF_STATISTICS), vf) {
         vf.StatisticsEnable = true;
      }
   }
}

static void
crocus_upload_render_state(struct crocus_context *ice,
                           struct crocus_batch *batch,
                           const struct pipe_draw_info *draw,
                           const struct pipe_draw_indirect_info *indirect,
                           const struct pipe_draw_start_count *sc)
{
   bool use_predicate = ice->state.predicate == CROCUS_PREDICATE_STATE_USE_BIT;

   batch->no_wrap = true;
   crocus_upload_dirty_render_state(ice, batch, draw);

   batch->no_wrap = false;
   if (draw->index_size > 0) {
      unsigned offset;
      unsigned size;

      if (draw->has_user_indices) {
         unsigned start_offset = draw->index_size * sc->start;
         u_upload_data(ice->ctx.stream_uploader, 0,
                       sc->count * draw->index_size, 4,
                       (char *)draw->index.user + start_offset,
                       &offset, &ice->state.last_res.index_buffer);
         offset -= start_offset;
         size = sc->count * draw->index_size;
      } else {
         struct crocus_resource *res = (void *) draw->index.resource;
         res->bind_history |= PIPE_BIND_INDEX_BUFFER;

         pipe_resource_reference(&ice->state.last_res.index_buffer,
                                 draw->index.resource);
         offset = 0;
         size = draw->index.resource->width0;
      }

      struct crocus_genx_state *genx = ice->state.genx;
      struct crocus_bo *bo = crocus_resource_bo(ice->state.last_res.index_buffer);

      crocus_emit_cmd(batch, GENX(3DSTATE_INDEX_BUFFER), ib) {
#if !(GEN_VERSIONx10 == 75)
         ib.CutIndexEnable = draw->primitive_restart;
#endif
         ib.IndexFormat = draw->index_size >> 1;
         ib.BufferStartingAddress = ro_bo(bo, offset);
         // XXX rework this to use proper bo
         ib.BufferEndingAddress = ro_bo(bo, offset + size - 1);
      }
   }

#define _3DPRIM_END_OFFSET          0x2420
#define _3DPRIM_START_VERTEX        0x2430
#define _3DPRIM_VERTEX_COUNT        0x2434
#define _3DPRIM_INSTANCE_COUNT      0x2438
#define _3DPRIM_START_INSTANCE      0x243C
#define _3DPRIM_BASE_VERTEX         0x2440

#if GEN_GEN == 7
   if (indirect && !indirect->count_from_stream_output) {
      if (indirect->indirect_draw_count) {
         use_predicate = true;

         struct crocus_bo *draw_count_bo =
            crocus_resource_bo(indirect->indirect_draw_count);
         unsigned draw_count_offset =
            indirect->indirect_draw_count_offset;

         crocus_emit_pipe_control_flush(batch,
                                      "ensure indirect draw buffer is flushed",
                                      PIPE_CONTROL_FLUSH_ENABLE);
#if 0
         if (ice->state.predicate == CROCUS_PREDICATE_STATE_USE_BIT) {
            struct gen_mi_builder b;
            gen_mi_builder_init(&b, batch);

            /* comparison = draw id < draw count */
            struct gen_mi_value comparison =
               gen_mi_ult(&b, gen_mi_imm(draw->drawid),
                              gen_mi_mem32(ro_bo(draw_count_bo,
                                                 draw_count_offset)));

            /* predicate = comparison & conditional rendering predicate */
            gen_mi_store(&b, gen_mi_reg32(MI_PREDICATE_RESULT),
                             gen_mi_iand(&b, comparison,
                                             gen_mi_reg32(CS_GPR(15))));
         } else {
            uint32_t mi_predicate;

            /* Upload the id of the current primitive to MI_PREDICATE_SRC1. */
            crocus_load_register_imm64(batch, MI_PREDICATE_SRC1, draw->drawid);
            /* Upload the current draw count from the draw parameters buffer
             * to MI_PREDICATE_SRC0.
             */
            crocus_load_register_mem32(batch, MI_PREDICATE_SRC0,
                                     draw_count_bo, draw_count_offset);
            /* Zero the top 32-bits of MI_PREDICATE_SRC0 */
            crocus_load_register_imm32(batch, MI_PREDICATE_SRC0 + 4, 0);

            if (draw->drawid == 0) {
               mi_predicate = MI_PREDICATE | MI_PREDICATE_LOADOP_LOADINV |
                              MI_PREDICATE_COMBINEOP_SET |
                              MI_PREDICATE_COMPAREOP_SRCS_EQUAL;
            } else {
               /* While draw_index < draw_count the predicate's result will be
                *  (draw_index == draw_count) ^ TRUE = TRUE
                * When draw_index == draw_count the result is
                *  (TRUE) ^ TRUE = FALSE
                * After this all results will be:
                *  (FALSE) ^ FALSE = FALSE
                */
               mi_predicate = MI_PREDICATE | MI_PREDICATE_LOADOP_LOAD |
                              MI_PREDICATE_COMBINEOP_XOR |
                              MI_PREDICATE_COMPAREOP_SRCS_EQUAL;
            }
            crocus_batch_emit(batch, &mi_predicate, sizeof(uint32_t));
         }
#endif
      }

      struct crocus_bo *bo = crocus_resource_bo(indirect->buffer);
      assert(bo);

      crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_MEM), lrm) {
         lrm.RegisterAddress = _3DPRIM_VERTEX_COUNT;
         lrm.MemoryAddress = ro_bo(bo, indirect->offset + 0);
      }
      crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_MEM), lrm) {
         lrm.RegisterAddress = _3DPRIM_INSTANCE_COUNT;
         lrm.MemoryAddress = ro_bo(bo, indirect->offset + 4);
      }
      crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_MEM), lrm) {
         lrm.RegisterAddress = _3DPRIM_START_VERTEX;
         lrm.MemoryAddress = ro_bo(bo, indirect->offset + 8);
      }
      if (draw->index_size) {
         crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_MEM), lrm) {
            lrm.RegisterAddress = _3DPRIM_BASE_VERTEX;
            lrm.MemoryAddress = ro_bo(bo, indirect->offset + 12);
         }
         crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_MEM), lrm) {
            lrm.RegisterAddress = _3DPRIM_START_INSTANCE;
            lrm.MemoryAddress = ro_bo(bo, indirect->offset + 16);
         }
      } else {
         crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_MEM), lrm) {
            lrm.RegisterAddress = _3DPRIM_START_INSTANCE;
            lrm.MemoryAddress = ro_bo(bo, indirect->offset + 12);
         }
         crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_IMM), lri) {
            lri.RegisterOffset = _3DPRIM_BASE_VERTEX;
            lri.DataDWord = 0;
         }
      }
   } else if (indirect && indirect->count_from_stream_output) {
      struct crocus_stream_output_target *so =
         (void *) indirect->count_from_stream_output;

      /* XXX: Replace with actual cache tracking */
      crocus_emit_pipe_control_flush(batch,
                                   "draw count from stream output stall",
                                   PIPE_CONTROL_CS_STALL);
#if 0
      struct gen_mi_builder b;
      gen_mi_builder_init(&b, batch);

      struct crocus_address addr =
         ro_bo(crocus_resource_bo(so->offset.res), so->offset.offset);
      struct gen_mi_value offset =
         gen_mi_iadd_imm(&b, gen_mi_mem32(addr), -so->base.buffer_offset);

      gen_mi_store(&b, gen_mi_reg32(_3DPRIM_VERTEX_COUNT),
                       gen_mi_udiv32_imm(&b, offset, so->stride));

      _crocus_emit_lri(batch, _3DPRIM_START_VERTEX, 0);
      _crocus_emit_lri(batch, _3DPRIM_BASE_VERTEX, 0);
      _crocus_emit_lri(batch, _3DPRIM_START_INSTANCE, 0);
      _crocus_emit_lri(batch, _3DPRIM_INSTANCE_COUNT, draw->instance_count);
#endif
   }
#else
   assert(!indirect);
#endif

   crocus_emit_cmd(batch, GENX(3DPRIMITIVE), prim) {
      prim.VertexAccessType = draw->index_size > 0 ? RANDOM : SEQUENTIAL;
#if GEN_GEN == 7
      prim.PredicateEnable = use_predicate;
#endif

      prim.PrimitiveTopologyType = translate_prim_type(draw->mode, 0);
      if (indirect) {
         // XXX Probably have to do something for gen6 here?
#if GEN_GEN == 7
         prim.IndirectParameterEnable = true;
#endif
      } else {
#if GEN_GEN >= 5
         prim.StartInstanceLocation = draw->start_instance;
#endif
         prim.InstanceCount = draw->instance_count;
         prim.VertexCountPerInstance = sc->count;

         prim.StartVertexLocation = sc->start;

         if (draw->index_size) {
            prim.BaseVertexLocation += draw->index_bias;
         } else {
            prim.StartVertexLocation += draw->index_bias;
         }
      }
   }
}

#if GEN_GEN == 7

static void
crocus_upload_compute_state(struct crocus_context *ice,
                          struct crocus_batch *batch,
                          const struct pipe_grid_info *grid)
{
   const uint64_t dirty = ice->state.dirty;
   struct crocus_screen *screen = batch->screen;
   const struct gen_device_info *devinfo = &screen->devinfo;
   struct crocus_shader_state *shs = &ice->state.shaders[MESA_SHADER_COMPUTE];
   struct crocus_compiled_shader *shader =
      ice->shaders.prog[MESA_SHADER_COMPUTE];
   struct brw_stage_prog_data *prog_data = shader->prog_data;
   struct brw_cs_prog_data *cs_prog_data = (void *) prog_data;
   const uint32_t group_size = grid->block[0] * grid->block[1] * grid->block[2];
   const unsigned simd_size =
      brw_cs_simd_size_for_group_size(devinfo, cs_prog_data, group_size);
   const unsigned threads = DIV_ROUND_UP(group_size, simd_size);

   if ((dirty & CROCUS_DIRTY_CONSTANTS_CS) && shs->sysvals_need_upload)
      upload_sysvals(ice, MESA_SHADER_COMPUTE);

//   if (dirty & CROCUS_DIRTY_BINDINGS_CS)
//      crocus_populate_binding_table(ice, batch, MESA_SHADER_COMPUTE, false);

   if (dirty & CROCUS_DIRTY_SAMPLER_STATES_CS)
      crocus_upload_sampler_states(ice, batch, MESA_SHADER_COMPUTE);

//   crocus_use_optional_res(batch, shs->sampler_table.res, false);

//   if (ice->state.need_border_colors)
//      crocus_use_pinned_bo(batch, ice->state.border_color_pool.bo, false);

   if (dirty & CROCUS_DIRTY_CS) {
      /* The MEDIA_VFE_STATE documentation for Gen8+ says:
       *
       *   "A stalling PIPE_CONTROL is required before MEDIA_VFE_STATE unless
       *    the only bits that are changed are scoreboard related: Scoreboard
       *    Enable, Scoreboard Type, Scoreboard Mask, Scoreboard Delta.  For
       *    these scoreboard related states, a MEDIA_STATE_FLUSH is
       *    sufficient."
       */
      crocus_emit_pipe_control_flush(batch,
                                   "workaround: stall before MEDIA_VFE_STATE",
                                   PIPE_CONTROL_CS_STALL);

      crocus_emit_cmd(batch, GENX(MEDIA_VFE_STATE), vfe) {
         if (prog_data->total_scratch) {
            struct crocus_bo *bo =
               crocus_get_scratch_space(ice, prog_data->total_scratch,
                                      MESA_SHADER_COMPUTE);
            vfe.PerThreadScratchSpace = ffs(prog_data->total_scratch) - 11;
            vfe.ScratchSpaceBasePointer = rw_bo(bo, 0);
         }

         vfe.MaximumNumberofThreads =
            devinfo->max_cs_threads * screen->subslice_total - 1;
         vfe.ResetGatewayTimer =
            Resettingrelativetimerandlatchingtheglobaltimestamp;
         vfe.BypassGatewayControl = true;
         vfe.GPGPUMode = 1;
         vfe.NumberofURBEntries = 2;
         vfe.URBEntryAllocationSize = 2;

         vfe.CURBEAllocationSize =
            ALIGN(cs_prog_data->push.per_thread.regs * threads +
                  cs_prog_data->push.cross_thread.regs, 2);
      }
   }

   /* TODO: Combine subgroup-id with cbuf0 so we can push regular uniforms */
   if (dirty & CROCUS_DIRTY_CS) {
      uint32_t curbe_data_offset = 0;
      assert(cs_prog_data->push.cross_thread.dwords == 0 &&
             cs_prog_data->push.per_thread.dwords == 1 &&
             cs_prog_data->base.param[0] == BRW_PARAM_BUILTIN_SUBGROUP_ID);
      const unsigned push_const_size =
         brw_cs_push_const_total_size(cs_prog_data, threads);
      uint32_t *curbe_data_map =
         stream_state(batch,
                      ALIGN(push_const_size, 64), 64,
                      &curbe_data_offset);
      assert(curbe_data_map);
      memset(curbe_data_map, 0x5a, ALIGN(push_const_size, 64));
      crocus_fill_cs_push_const_buffer(cs_prog_data, threads, curbe_data_map);

      crocus_emit_cmd(batch, GENX(MEDIA_CURBE_LOAD), curbe) {
         curbe.CURBETotalDataLength = ALIGN(push_const_size, 64);
         curbe.CURBEDataStartAddress = curbe_data_offset;
      }
   }

   if (dirty & (CROCUS_DIRTY_SAMPLER_STATES_CS |
                CROCUS_DIRTY_BINDINGS_CS |
                CROCUS_DIRTY_CONSTANTS_CS |
                CROCUS_DIRTY_CS)) {
      uint32_t desc[GENX(INTERFACE_DESCRIPTOR_DATA_length)];

      crocus_pack_state(GENX(INTERFACE_DESCRIPTOR_DATA), desc, idd) {
         idd.SamplerStatePointer = shs->sampler_table.offset;
         // TODO idd.BindingTablePointer = binder->bt_offset[MESA_SHADER_COMPUTE];
         idd.NumberofThreadsinGPGPUThreadGroup = threads;
      }

      for (int i = 0; i < GENX(INTERFACE_DESCRIPTOR_DATA_length); i++)
         desc[i] |= ((uint32_t *) shader->derived_data)[i];

      crocus_emit_cmd(batch, GENX(MEDIA_INTERFACE_DESCRIPTOR_LOAD), load) {
         load.InterfaceDescriptorTotalLength =
            GENX(INTERFACE_DESCRIPTOR_DATA_length) * sizeof(uint32_t);
         load.InterfaceDescriptorDataStartAddress =
            emit_state(batch, desc, sizeof(desc), 64);
      }
   }

   uint32_t remainder = group_size & (simd_size - 1);
   uint32_t right_mask;

   if (remainder > 0)
      right_mask = ~0u >> (32 - remainder);
   else
      right_mask = ~0u >> (32 - simd_size);

#define GPGPU_DISPATCHDIMX 0x2500
#define GPGPU_DISPATCHDIMY 0x2504
#define GPGPU_DISPATCHDIMZ 0x2508

   if (grid->indirect) {
      struct crocus_state_ref *grid_size = &ice->state.grid_size;
      struct crocus_bo *bo = crocus_resource_bo(grid_size->res);
      crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_MEM), lrm) {
         lrm.RegisterAddress = GPGPU_DISPATCHDIMX;
         lrm.MemoryAddress = ro_bo(bo, grid_size->offset + 0);
      }
      crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_MEM), lrm) {
         lrm.RegisterAddress = GPGPU_DISPATCHDIMY;
         lrm.MemoryAddress = ro_bo(bo, grid_size->offset + 4);
      }
      crocus_emit_cmd(batch, GENX(MI_LOAD_REGISTER_MEM), lrm) {
         lrm.RegisterAddress = GPGPU_DISPATCHDIMZ;
         lrm.MemoryAddress = ro_bo(bo, grid_size->offset + 8);
      }
   }

   crocus_emit_cmd(batch, GENX(GPGPU_WALKER), ggw) {
      ggw.IndirectParameterEnable    = grid->indirect != NULL;
      ggw.SIMDSize                   = simd_size / 16;
      ggw.ThreadDepthCounterMaximum  = 0;
      ggw.ThreadHeightCounterMaximum = 0;
      ggw.ThreadWidthCounterMaximum  = threads - 1;
      ggw.ThreadGroupIDXDimension    = grid->grid[0];
      ggw.ThreadGroupIDYDimension    = grid->grid[1];
      ggw.ThreadGroupIDZDimension    = grid->grid[2];
      ggw.RightExecutionMask         = right_mask;
      ggw.BottomExecutionMask        = 0xffffffff;
   }

   crocus_emit_cmd(batch, GENX(MEDIA_STATE_FLUSH), msf);

   if (!batch->contains_draw) {
//      crocus_restore_compute_saved_bos(ice, batch, grid);
      batch->contains_draw = true;
   }
}

#endif /* GEN_GEN == 7 */

/**
 * State module teardown.
 */
static void
crocus_destroy_state(struct crocus_context *ice)
{
   struct crocus_genx_state *genx = ice->state.genx;

   pipe_resource_reference(&ice->draw.draw_params.res, NULL);
   pipe_resource_reference(&ice->draw.derived_draw_params.res, NULL);

   /* Loop over all VBOs, including ones for draw parameters */
//   for (unsigned i = 0; i < ARRAY_SIZE(genx->vertex_buffers); i++) {
//      pipe_resource_reference(&genx->vertex_buffers[i].resource, NULL);
//   }

   free(ice->state.genx);

   for (int i = 0; i < 4; i++) {
      pipe_so_target_reference(&ice->state.so_target[i], NULL);
   }

   for (unsigned i = 0; i < ice->state.framebuffer.nr_cbufs; i++) {
      pipe_surface_reference(&ice->state.framebuffer.cbufs[i], NULL);
   }
   pipe_surface_reference(&ice->state.framebuffer.zsbuf, NULL);

   for (int stage = 0; stage < MESA_SHADER_STAGES; stage++) {
      struct crocus_shader_state *shs = &ice->state.shaders[stage];
      pipe_resource_reference(&shs->sampler_table.res, NULL);
      for (int i = 0; i < PIPE_MAX_CONSTANT_BUFFERS; i++) {
         pipe_resource_reference(&shs->constbuf[i].buffer, NULL);
         pipe_resource_reference(&shs->constbuf_surf_state[i].res, NULL);
      }
      for (int i = 0; i < PIPE_MAX_SHADER_IMAGES; i++) {
         pipe_resource_reference(&shs->image[i].base.resource, NULL);
         pipe_resource_reference(&shs->image[i].surface_state.ref.res, NULL);
         free(shs->image[i].surface_state.cpu);
      }
      for (int i = 0; i < PIPE_MAX_SHADER_BUFFERS; i++) {
         pipe_resource_reference(&shs->ssbo[i].buffer, NULL);
         pipe_resource_reference(&shs->ssbo_surf_state[i].res, NULL);
      }
      for (int i = 0; i < CROCUS_MAX_TEXTURE_SAMPLERS; i++) {
         pipe_sampler_view_reference((struct pipe_sampler_view **)
                                     &shs->textures[i], NULL);
      }
   }

   pipe_resource_reference(&ice->state.grid_size.res, NULL);
   pipe_resource_reference(&ice->state.grid_surf_state.res, NULL);

   pipe_resource_reference(&ice->state.null_fb.res, NULL);
   pipe_resource_reference(&ice->state.unbound_tex.res, NULL);

   pipe_resource_reference(&ice->state.last_res.cc_vp, NULL);
   pipe_resource_reference(&ice->state.last_res.sf_cl_vp, NULL);
   pipe_resource_reference(&ice->state.last_res.color_calc, NULL);
   pipe_resource_reference(&ice->state.last_res.scissor, NULL);
   pipe_resource_reference(&ice->state.last_res.blend, NULL);
   pipe_resource_reference(&ice->state.last_res.index_buffer, NULL);
   pipe_resource_reference(&ice->state.last_res.cs_thread_ids, NULL);
   pipe_resource_reference(&ice->state.last_res.cs_desc, NULL);
}

/* ------------------------------------------------------------------- */

static void
crocus_rebind_buffer(struct crocus_context *ice,
                   struct crocus_resource *res)
{
   struct pipe_context *ctx = &ice->ctx;
   struct crocus_genx_state *genx = ice->state.genx;

   assert(res->base.target == PIPE_BUFFER);

   /* Buffers can't be framebuffer attachments, nor display related,
    * and we don't have upstream Clover support.
    */
   assert(!(res->bind_history & (PIPE_BIND_DEPTH_STENCIL |
                                 PIPE_BIND_RENDER_TARGET |
                                 PIPE_BIND_BLENDABLE |
                                 PIPE_BIND_DISPLAY_TARGET |
                                 PIPE_BIND_CURSOR |
                                 PIPE_BIND_COMPUTE_RESOURCE |
                                 PIPE_BIND_GLOBAL)));

#if 0//TODO
   if (res->bind_history & PIPE_BIND_VERTEX_BUFFER) {
      uint64_t bound_vbs = ice->state.bound_vertex_buffers;
      while (bound_vbs) {
         const int i = u_bit_scan64(&bound_vbs);
         struct crocus_vertex_buffer_state *state = &genx->vertex_buffers[i];

         /* Update the CPU struct */
         STATIC_ASSERT(GENX(VERTEX_BUFFER_STATE_BufferStartingAddress_start) == 32);
         STATIC_ASSERT(GENX(VERTEX_BUFFER_STATE_BufferStartingAddress_bits) == 32);
         uint32_t *addr = (uint32_t *) &state->state[1];
         struct crocus_bo *bo = crocus_resource_bo(state->resource);

         // XXX relocs!
         if (*addr != bo->gtt_offset + state->offset) {
            *addr = bo->gtt_offset + state->offset;
            ice->state.dirty |= CROCUS_DIRTY_VERTEX_BUFFERS;
         }
      }
   }
#endif

   /* We don't need to handle PIPE_BIND_INDEX_BUFFER here: we re-emit
    * the 3DSTATE_INDEX_BUFFER packet whenever the address changes.
    *
    * There is also no need to handle these:
    * - PIPE_BIND_COMMAND_ARGS_BUFFER (emitted for every indirect draw)
    * - PIPE_BIND_QUERY_BUFFER (no persistent state references)
    */

   if (res->bind_history & PIPE_BIND_STREAM_OUTPUT) {
      /* XXX: be careful about resetting vs appending... */
      assert(false);
   }

   for (int s = MESA_SHADER_VERTEX; s < MESA_SHADER_STAGES; s++) {
      struct crocus_shader_state *shs = &ice->state.shaders[s];
      enum pipe_shader_type p_stage = stage_to_pipe(s);

      if (!(res->bind_stages & (1 << s)))
         continue;

      if (res->bind_history & PIPE_BIND_CONSTANT_BUFFER) {
         /* Skip constant buffer 0, it's for regular uniforms, not UBOs */
         uint32_t bound_cbufs = shs->bound_cbufs & ~1u;
         while (bound_cbufs) {
            const int i = u_bit_scan(&bound_cbufs);
            struct pipe_shader_buffer *cbuf = &shs->constbuf[i];
            struct crocus_state_ref *surf_state = &shs->constbuf_surf_state[i];

            if (res->bo == crocus_resource_bo(cbuf->buffer)) {
               pipe_resource_reference(&surf_state->res, NULL);
               ice->state.dirty |= CROCUS_DIRTY_CONSTANTS_VS << s;
            }
         }
      }

      if (res->bind_history & PIPE_BIND_SHADER_BUFFER) {
         uint32_t bound_ssbos = shs->bound_ssbos;
         while (bound_ssbos) {
            const int i = u_bit_scan(&bound_ssbos);
            struct pipe_shader_buffer *ssbo = &shs->ssbo[i];

            if (res->bo == crocus_resource_bo(ssbo->buffer)) {
               struct pipe_shader_buffer buf = {
                  .buffer = &res->base,
                  .buffer_offset = ssbo->buffer_offset,
                  .buffer_size = ssbo->buffer_size,
               };
               crocus_set_shader_buffers(ctx, p_stage, i, 1, &buf,
                                       (shs->writable_ssbos >> i) & 1);
            }
         }
      }

      if (res->bind_history & PIPE_BIND_SAMPLER_VIEW) {
         uint32_t bound_sampler_views = shs->bound_sampler_views;
         while (bound_sampler_views) {
            const int i = u_bit_scan(&bound_sampler_views);
            struct crocus_sampler_view *isv = shs->textures[i];
            struct crocus_bo *bo = isv->res->bo;

            if (update_surface_state_addrs(ice->state.surface_uploader,
                                           &isv->surface_state, bo)) {
               ice->state.dirty |= CROCUS_DIRTY_BINDINGS_VS << s;
            }
         }
      }

      if (res->bind_history & PIPE_BIND_SHADER_IMAGE) {
         uint32_t bound_image_views = shs->bound_image_views;
         while (bound_image_views) {
            const int i = u_bit_scan(&bound_image_views);
            struct crocus_image_view *iv = &shs->image[i];
            struct crocus_bo *bo = crocus_resource_bo(iv->base.resource);

            if (update_surface_state_addrs(ice->state.surface_uploader,
                                           &iv->surface_state, bo)) {
               ice->state.dirty |= CROCUS_DIRTY_BINDINGS_VS << s;
            }
         }
      }
   }
}

/* ------------------------------------------------------------------- */

static unsigned
flags_to_post_sync_op(uint32_t flags)
{
   if (flags & PIPE_CONTROL_WRITE_IMMEDIATE)
      return WriteImmediateData;

   if (flags & PIPE_CONTROL_WRITE_DEPTH_COUNT)
      return WritePSDepthCount;

   if (flags & PIPE_CONTROL_WRITE_TIMESTAMP)
      return WriteTimestamp;

   return 0;
}

/**
 * Do the given flags have a Post Sync or LRI Post Sync operation?
 */
static enum pipe_control_flags
get_post_sync_flags(enum pipe_control_flags flags)
{
   flags &= PIPE_CONTROL_WRITE_IMMEDIATE |
            PIPE_CONTROL_WRITE_DEPTH_COUNT |
            PIPE_CONTROL_WRITE_TIMESTAMP |
            PIPE_CONTROL_LRI_POST_SYNC_OP;

   /* Only one "Post Sync Op" is allowed, and it's mutually exclusive with
    * "LRI Post Sync Operation".  So more than one bit set would be illegal.
    */
   assert(util_bitcount(flags) <= 1);

   return flags;
}

#define IS_COMPUTE_PIPELINE(batch) (batch->name == CROCUS_BATCH_COMPUTE)

/**
 * Emit a series of PIPE_CONTROL commands, taking into account any
 * workarounds necessary to actually accomplish the caller's request.
 *
 * Unless otherwise noted, spec quotations in this function come from:
 *
 * Synchronization of the 3D Pipeline > PIPE_CONTROL Command > Programming
 * Restrictions for PIPE_CONTROL.
 *
 * You should not use this function directly.  Use the helpers in
 * crocus_pipe_control.c instead, which may split the pipe control further.
 */
static void
crocus_emit_raw_pipe_control(struct crocus_batch *batch,
                           const char *reason,
                           uint32_t flags,
                           struct crocus_bo *bo,
                           uint32_t offset,
                           uint64_t imm)
{
   UNUSED const struct gen_device_info *devinfo = &batch->screen->devinfo;
   enum pipe_control_flags post_sync_flags = get_post_sync_flags(flags);
   enum pipe_control_flags non_lri_post_sync_flags =
      post_sync_flags & ~PIPE_CONTROL_LRI_POST_SYNC_OP;

   /* Recursive PIPE_CONTROL workarounds --------------------------------
    * (http://knowyourmeme.com/memes/xzibit-yo-dawg)
    *
    * We do these first because we want to look at the original operation,
    * rather than any workarounds we set.
    */

   /* "Flush Types" workarounds ---------------------------------------------
    * We do these now because they may add post-sync operations or CS stalls.
    */

   // TODO: check if this applies
   if (flags & PIPE_CONTROL_VF_CACHE_INVALIDATE) {
      /* Project: BDW, SKL+ (stopping at CNL) / Argument: VF Invalidate
       *
       * "'Post Sync Operation' must be enabled to 'Write Immediate Data' or
       *  'Write PS Depth Count' or 'Write Timestamp'."
       */
      if (!bo) {
         flags |= PIPE_CONTROL_WRITE_IMMEDIATE;
         post_sync_flags |= PIPE_CONTROL_WRITE_IMMEDIATE;
         non_lri_post_sync_flags |= PIPE_CONTROL_WRITE_IMMEDIATE;
         bo = batch->screen->workaround_bo;
      }
   }

   if (flags & PIPE_CONTROL_DEPTH_STALL) {
      /* From the PIPE_CONTROL instruction table, bit 13 (Depth Stall Enable):
       *
       *    "This bit must be DISABLED for operations other than writing
       *     PS_DEPTH_COUNT."
       *
       * This seems like nonsense.  An Ivybridge workaround requires us to
       * emit a PIPE_CONTROL with a depth stall and write immediate post-sync
       * operation.  Gen8+ requires us to emit depth stalls and depth cache
       * flushes together.  So, it's hard to imagine this means anything other
       * than "we originally intended this to be used for PS_DEPTH_COUNT".
       *
       * We ignore the supposed restriction and do nothing.
       */
   }

   if (flags & (PIPE_CONTROL_RENDER_TARGET_FLUSH |
                PIPE_CONTROL_STALL_AT_SCOREBOARD)) {
      /* From the PIPE_CONTROL instruction table, bit 12 and bit 1:
       *
       *    "This bit must be DISABLED for End-of-pipe (Read) fences,
       *     PS_DEPTH_COUNT or TIMESTAMP queries."
       *
       * TODO: Implement end-of-pipe checking.
       */
      assert(!(post_sync_flags & (PIPE_CONTROL_WRITE_DEPTH_COUNT |
                                  PIPE_CONTROL_WRITE_TIMESTAMP)));
   }

   if (flags & PIPE_CONTROL_STALL_AT_SCOREBOARD) {
      /* From the PIPE_CONTROL instruction table, bit 1:
       *
       *    "This bit is ignored if Depth Stall Enable is set.
       *     Further, the render cache is not flushed even if Write Cache
       *     Flush Enable bit is set."
       *
       * We assert that the caller doesn't do this combination, to try and
       * prevent mistakes.  It shouldn't hurt the GPU, though.
       *
       * We skip this check on Gen11+ as the "Stall at Pixel Scoreboard"
       * and "Render Target Flush" combo is explicitly required for BTI
       * update workarounds.
       */
      assert(!(flags & (PIPE_CONTROL_DEPTH_STALL |
                        PIPE_CONTROL_RENDER_TARGET_FLUSH)));
   }

   /* PIPE_CONTROL page workarounds ------------------------------------- */

   if (GEN_GEN == 7 && (flags & PIPE_CONTROL_STATE_CACHE_INVALIDATE)) {
      /* From the PIPE_CONTROL page itself:
       *
       *    "IVB, HSW, BDW
       *     Restriction: Pipe_control with CS-stall bit set must be issued
       *     before a pipe-control command that has the State Cache
       *     Invalidate bit set."
       */
      flags |= PIPE_CONTROL_CS_STALL;
   }

   if (flags & PIPE_CONTROL_FLUSH_LLC) {
      /* From the PIPE_CONTROL instruction table, bit 26 (Flush LLC):
       *
       *    "Project: ALL
       *     SW must always program Post-Sync Operation to "Write Immediate
       *     Data" when Flush LLC is set."
       *
       * For now, we just require the caller to do it.
       */
      assert(flags & PIPE_CONTROL_WRITE_IMMEDIATE);
   }

   /* "Post-Sync Operation" workarounds -------------------------------- */

   /* Project: All / Argument: Global Snapshot Count Reset [19]
    *
    * "This bit must not be exercised on any product.
    *  Requires stall bit ([20] of DW1) set."
    *
    * We don't use this, so we just assert that it isn't used.  The
    * PIPE_CONTROL instruction page indicates that they intended this
    * as a debug feature and don't think it is useful in production,
    * but it may actually be usable, should we ever want to.
    */
   assert((flags & PIPE_CONTROL_GLOBAL_SNAPSHOT_COUNT_RESET) == 0);

   if (flags & (PIPE_CONTROL_MEDIA_STATE_CLEAR |
                PIPE_CONTROL_INDIRECT_STATE_POINTERS_DISABLE)) {
      /* Project: All / Arguments:
       *
       * - Generic Media State Clear [16]
       * - Indirect State Pointers Disable [16]
       *
       *    "Requires stall bit ([20] of DW1) set."
       *
       * Also, the PIPE_CONTROL instruction table, bit 16 (Generic Media
       * State Clear) says:
       *
       *    "PIPECONTROL command with “Command Streamer Stall Enable” must be
       *     programmed prior to programming a PIPECONTROL command with "Media
       *     State Clear" set in GPGPU mode of operation"
       *
       * This is a subset of the earlier rule, so there's nothing to do.
       */
      flags |= PIPE_CONTROL_CS_STALL;
   }

   if (flags & PIPE_CONTROL_STORE_DATA_INDEX) {
      /* Project: All / Argument: Store Data Index
       *
       * "Post-Sync Operation ([15:14] of DW1) must be set to something other
       *  than '0'."
       *
       * For now, we just assert that the caller does this.  We might want to
       * automatically add a write to the workaround BO...
       */
      assert(non_lri_post_sync_flags != 0);
   }

   if (flags & PIPE_CONTROL_SYNC_GFDT) {
      /* Project: All / Argument: Sync GFDT
       *
       * "Post-Sync Operation ([15:14] of DW1) must be set to something other
       *  than '0' or 0x2520[13] must be set."
       *
       * For now, we just assert that the caller does this.
       */
      assert(non_lri_post_sync_flags != 0);
   }

   if (flags & PIPE_CONTROL_TLB_INVALIDATE) {
      /* Project: IVB+ / Argument: TLB inv
       *
       *    "Requires stall bit ([20] of DW1) set."
       *
       * Also, from the PIPE_CONTROL instruction table:
       *
       *    "Project: SKL+
       *     Post Sync Operation or CS stall must be set to ensure a TLB
       *     invalidation occurs.  Otherwise no cycle will occur to the TLB
       *     cache to invalidate."
       *
       * This is not a subset of the earlier rule, so there's nothing to do.
       */
      flags |= PIPE_CONTROL_CS_STALL;
   }

   /* "GPGPU specific workarounds" (both post-sync and flush) ------------ */

   if (IS_COMPUTE_PIPELINE(batch)) {
      // TODO: check if this is needed on gen7
      if (GEN_GEN == 7 && (post_sync_flags ||
                           (flags & (PIPE_CONTROL_NOTIFY_ENABLE |
                                     PIPE_CONTROL_DEPTH_STALL |
                                     PIPE_CONTROL_RENDER_TARGET_FLUSH |
                                     PIPE_CONTROL_DEPTH_CACHE_FLUSH |
                                     PIPE_CONTROL_DATA_CACHE_FLUSH)))) {
         /* Project: BDW / Arguments:
          *
          * - LRI Post Sync Operation   [23]
          * - Post Sync Op              [15:14]
          * - Notify En                 [8]
          * - Depth Stall               [13]
          * - Render Target Cache Flush [12]
          * - Depth Cache Flush         [0]
          * - DC Flush Enable           [5]
          *
          *    "Requires stall bit ([20] of DW) set for all GPGPU and Media
          *     Workloads."
          */
         flags |= PIPE_CONTROL_CS_STALL;

         /* Also, from the PIPE_CONTROL instruction table, bit 20:
          *
          *    "Project: BDW
          *     This bit must be always set when PIPE_CONTROL command is
          *     programmed by GPGPU and MEDIA workloads, except for the cases
          *     when only Read Only Cache Invalidation bits are set (State
          *     Cache Invalidation Enable, Instruction cache Invalidation
          *     Enable, Texture Cache Invalidation Enable, Constant Cache
          *     Invalidation Enable). This is to WA FFDOP CG issue, this WA
          *     need not implemented when FF_DOP_CG is disable via "Fixed
          *     Function DOP Clock Gate Disable" bit in RC_PSMI_CTRL register."
          *
          * It sounds like we could avoid CS stalls in some cases, but we
          * don't currently bother.  This list isn't exactly the list above,
          * either...
          */
      }
   }

   /* "Stall" workarounds ----------------------------------------------
    * These have to come after the earlier ones because we may have added
    * some additional CS stalls above.
    */

   if (flags & PIPE_CONTROL_CS_STALL) {
      /* Project: PRE-SKL, VLV, CHV
       *
       * "[All Stepping][All SKUs]:
       *
       *  One of the following must also be set:
       *
       *  - Render Target Cache Flush Enable ([12] of DW1)
       *  - Depth Cache Flush Enable ([0] of DW1)
       *  - Stall at Pixel Scoreboard ([1] of DW1)
       *  - Depth Stall ([13] of DW1)
       *  - Post-Sync Operation ([13] of DW1)
       *  - DC Flush Enable ([5] of DW1)"
       *
       * If we don't already have one of those bits set, we choose to add
       * "Stall at Pixel Scoreboard".  Some of the other bits require a
       * CS stall as a workaround (see above), which would send us into
       * an infinite recursion of PIPE_CONTROLs.  "Stall at Pixel Scoreboard"
       * appears to be safe, so we choose that.
       */
      const uint32_t wa_bits = PIPE_CONTROL_RENDER_TARGET_FLUSH |
                               PIPE_CONTROL_DEPTH_CACHE_FLUSH |
                               PIPE_CONTROL_WRITE_IMMEDIATE |
                               PIPE_CONTROL_WRITE_DEPTH_COUNT |
                               PIPE_CONTROL_WRITE_TIMESTAMP |
                               PIPE_CONTROL_STALL_AT_SCOREBOARD |
                               PIPE_CONTROL_DEPTH_STALL |
                               PIPE_CONTROL_DATA_CACHE_FLUSH;
      if (!(flags & wa_bits))
         flags |= PIPE_CONTROL_STALL_AT_SCOREBOARD;
   }

   /* Emit --------------------------------------------------------------- */

   if (INTEL_DEBUG & DEBUG_PIPE_CONTROL) {
      fprintf(stderr,
              "  PC [%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%"PRIx64"]: %s\n",
              (flags & PIPE_CONTROL_FLUSH_ENABLE) ? "PipeCon " : "",
              (flags & PIPE_CONTROL_CS_STALL) ? "CS " : "",
              (flags & PIPE_CONTROL_STALL_AT_SCOREBOARD) ? "Scoreboard " : "",
              (flags & PIPE_CONTROL_VF_CACHE_INVALIDATE) ? "VF " : "",
              (flags & PIPE_CONTROL_RENDER_TARGET_FLUSH) ? "RT " : "",
              (flags & PIPE_CONTROL_CONST_CACHE_INVALIDATE) ? "Const " : "",
              (flags & PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE) ? "TC " : "",
              (flags & PIPE_CONTROL_DATA_CACHE_FLUSH) ? "DC " : "",
              (flags & PIPE_CONTROL_DEPTH_CACHE_FLUSH) ? "ZFlush " : "",
              (flags & PIPE_CONTROL_DEPTH_STALL) ? "ZStall " : "",
              (flags & PIPE_CONTROL_STATE_CACHE_INVALIDATE) ? "State " : "",
              (flags & PIPE_CONTROL_TLB_INVALIDATE) ? "TLB " : "",
              (flags & PIPE_CONTROL_INSTRUCTION_INVALIDATE) ? "Inst " : "",
              (flags & PIPE_CONTROL_MEDIA_STATE_CLEAR) ? "MediaClear " : "",
              (flags & PIPE_CONTROL_NOTIFY_ENABLE) ? "Notify " : "",
              (flags & PIPE_CONTROL_GLOBAL_SNAPSHOT_COUNT_RESET) ?
                 "SnapRes" : "",
              (flags & PIPE_CONTROL_INDIRECT_STATE_POINTERS_DISABLE) ?
                  "ISPDis" : "",
              (flags & PIPE_CONTROL_WRITE_IMMEDIATE) ? "WriteImm " : "",
              (flags & PIPE_CONTROL_WRITE_DEPTH_COUNT) ? "WriteZCount " : "",
              (flags & PIPE_CONTROL_WRITE_TIMESTAMP) ? "WriteTimestamp " : "",
              imm, reason);
   }

   crocus_emit_cmd(batch, GENX(PIPE_CONTROL), pc) {
#if GEN_GEN >= 7
      pc.LRIPostSyncOperation = NoLRIOperation;
      pc.PipeControlFlushEnable = flags & PIPE_CONTROL_FLUSH_ENABLE;
      pc.DCFlushEnable = flags & PIPE_CONTROL_DATA_CACHE_FLUSH;
#endif
#if GEN_GEN >= 6
      pc.StoreDataIndex = 0;
      pc.CommandStreamerStallEnable = flags & PIPE_CONTROL_CS_STALL;
      pc.GlobalSnapshotCountReset =
         flags & PIPE_CONTROL_GLOBAL_SNAPSHOT_COUNT_RESET;
      pc.TLBInvalidate = flags & PIPE_CONTROL_TLB_INVALIDATE;
      pc.GenericMediaStateClear = flags & PIPE_CONTROL_MEDIA_STATE_CLEAR;
      pc.StallAtPixelScoreboard = flags & PIPE_CONTROL_STALL_AT_SCOREBOARD;
      pc.RenderTargetCacheFlushEnable =
         flags & PIPE_CONTROL_RENDER_TARGET_FLUSH;
      pc.DepthCacheFlushEnable = flags & PIPE_CONTROL_DEPTH_CACHE_FLUSH;
      pc.StateCacheInvalidationEnable =
         flags & PIPE_CONTROL_STATE_CACHE_INVALIDATE;
      pc.VFCacheInvalidationEnable = flags & PIPE_CONTROL_VF_CACHE_INVALIDATE;
      pc.ConstantCacheInvalidationEnable =
         flags & PIPE_CONTROL_CONST_CACHE_INVALIDATE;
#else
      pc.WriteCacheFlush = flags & PIPE_CONTROL_RENDER_TARGET_FLUSH;
#endif
      pc.PostSyncOperation = flags_to_post_sync_op(flags);
      pc.DepthStallEnable = flags & PIPE_CONTROL_DEPTH_STALL;
      pc.InstructionCacheInvalidateEnable =
         flags & PIPE_CONTROL_INSTRUCTION_INVALIDATE;
      pc.NotifyEnable = flags & PIPE_CONTROL_NOTIFY_ENABLE;
#if GEN_GEN >= 5 || GEN_IS_G4X
      pc.IndirectStatePointersDisable =
         flags & PIPE_CONTROL_INDIRECT_STATE_POINTERS_DISABLE;
#endif
#if GEN_GEN >= 6
      pc.TextureCacheInvalidationEnable =
         flags & PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE;
#elif GEN_GEN == 5 || GEN_IS_G4X
      pc.TextureCacheFlushEnable =
         flags & PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE;
#endif
      pc.Address = rw_bo(bo, offset);
      if (GEN_GEN < 7 && bo)
         pc.DestinationAddressType = DAT_GGTT;
      pc.ImmediateData = imm;
   }
}

#if GEN_GEN == 7
void
genX(emit_urb_setup)(struct crocus_context *ice,
                     struct crocus_batch *batch,
                     const unsigned size[4],
                     bool tess_present, bool gs_present)
{
   const struct gen_device_info *devinfo = &batch->screen->devinfo;
   unsigned entries[4];
   unsigned start[4];

   ice->shaders.last_vs_entry_size = size[MESA_SHADER_VERTEX];

   intel_get_urb_config(devinfo, batch->screen->l3_config_3d,
                        tess_present, gs_present,
                        size, entries, start, NULL, NULL);

   if (!(GEN_VERSIONx10 == 75) && !devinfo->is_baytrail)
      gen7_emit_vs_workaround_flush(batch);
   for (int i = MESA_SHADER_VERTEX; i <= MESA_SHADER_GEOMETRY; i++) {
      crocus_emit_cmd(batch, GENX(3DSTATE_URB_VS), urb) {
         urb._3DCommandSubOpcode += i;
         urb.VSURBStartingAddress     = start[i];
         urb.VSURBEntryAllocationSize = size[i] - 1;
         urb.VSNumberofURBEntries     = entries[i];
      }
   }
}
#elif GEN_GEN <= 6
void
genX(emit_urb_setup)(struct crocus_context *ice,
                     struct crocus_batch *batch,
                     const unsigned size[4],
                     bool tess_present, bool gs_present)
{
#if GEN_GEN <= 5
   crocus_calculate_urb_fence(batch, 0,
                              size[0], size[1]);
#endif
   // XXX TODO
}
#endif

static void
crocus_lost_genx_state(struct crocus_context *ice, struct crocus_batch *batch)
{
}

static void
crocus_emit_mi_report_perf_count(struct crocus_batch *batch,
                               struct crocus_bo *bo,
                               uint32_t offset_in_bytes,
                               uint32_t report_id)
{
#if GEN_GEN >= 7
   crocus_emit_cmd(batch, GENX(MI_REPORT_PERF_COUNT), mi_rpc) {
      mi_rpc.MemoryAddress = rw_bo(bo, offset_in_bytes);
      mi_rpc.ReportID = report_id;
   }
#endif
}

#if GEN_VERSIONx10 == 75
static void
crocus_state_finish_batch(struct crocus_batch *batch)
{
   crocus_emit_mi_flush(batch);
   crocus_emit_cmd(batch, GENX(3DSTATE_CC_STATE_POINTERS), ptr) {
      ptr.ColorCalcStatePointer = batch->ice->shaders.cc_offset;
   }

   crocus_emit_pipe_control_flush(batch, "hsw wa", PIPE_CONTROL_RENDER_TARGET_FLUSH |
				  PIPE_CONTROL_CS_STALL);
}
#endif

#if GEN_VERSIONx10 == 75
struct pipe_rasterizer_state *crocus_get_rast_state(struct crocus_context *ice)
{
  return &ice->state.cso_rast->state;
}
#endif

static void crocus_fill_clamp_mask(const struct crocus_sampler_state *samp,
                                   int s,
                                   uint32_t *clamp_mask)
{
   if (samp->pstate.min_img_filter != PIPE_TEX_FILTER_NEAREST &&
       samp->pstate.mag_img_filter != PIPE_TEX_FILTER_NEAREST) {
      if (samp->pstate.wrap_s == PIPE_TEX_WRAP_CLAMP)
         clamp_mask[0] |= (1 << s);
      if (samp->pstate.wrap_t == PIPE_TEX_WRAP_CLAMP)
         clamp_mask[1] |= (1 << s);
      if (samp->pstate.wrap_r == PIPE_TEX_WRAP_CLAMP)
         clamp_mask[2] |= (1 << s);
   }
}

void
genX(init_state)(struct crocus_context *ice)
{
   struct pipe_context *ctx = &ice->ctx;
   struct crocus_screen *screen = (struct crocus_screen *)ctx->screen;

   ctx->create_blend_state = crocus_create_blend_state;
   ctx->create_depth_stencil_alpha_state = crocus_create_zsa_state;
   ctx->create_rasterizer_state = crocus_create_rasterizer_state;
   ctx->create_sampler_state = crocus_create_sampler_state;
   ctx->create_sampler_view = crocus_create_sampler_view;
   ctx->create_surface = crocus_create_surface;
   ctx->create_vertex_elements_state = crocus_create_vertex_elements;
   ctx->bind_blend_state = crocus_bind_blend_state;
   ctx->bind_depth_stencil_alpha_state = crocus_bind_zsa_state;
   ctx->bind_sampler_states = crocus_bind_sampler_states;
   ctx->bind_rasterizer_state = crocus_bind_rasterizer_state;
   ctx->bind_vertex_elements_state = crocus_bind_vertex_elements_state;
   ctx->delete_blend_state = crocus_delete_state;
   ctx->delete_depth_stencil_alpha_state = crocus_delete_state;
   ctx->delete_rasterizer_state = crocus_delete_state;
   ctx->delete_sampler_state = crocus_delete_state;
   ctx->delete_vertex_elements_state = crocus_delete_state;
   ctx->set_blend_color = crocus_set_blend_color;
   ctx->set_clip_state = crocus_set_clip_state;
   ctx->set_constant_buffer = crocus_set_constant_buffer;
   ctx->set_shader_buffers = crocus_set_shader_buffers;
   ctx->set_shader_images = crocus_set_shader_images;
   ctx->set_sampler_views = crocus_set_sampler_views;
   ctx->set_tess_state = crocus_set_tess_state;
   ctx->set_framebuffer_state = crocus_set_framebuffer_state;
   ctx->set_polygon_stipple = crocus_set_polygon_stipple;
   ctx->set_sample_mask = crocus_set_sample_mask;
   ctx->set_scissor_states = crocus_set_scissor_states;
   ctx->set_stencil_ref = crocus_set_stencil_ref;
   ctx->set_vertex_buffers = crocus_set_vertex_buffers;
   ctx->set_viewport_states = crocus_set_viewport_states;
   ctx->sampler_view_destroy = crocus_sampler_view_destroy;
   ctx->surface_destroy = crocus_surface_destroy;
   ctx->draw_vbo = crocus_draw_vbo;
   ctx->launch_grid = crocus_launch_grid;
#if GEN_GEN == 7
   ctx->create_stream_output_target = crocus_create_stream_output_target;
   ctx->stream_output_target_destroy = crocus_stream_output_target_destroy;
   ctx->set_stream_output_targets = crocus_set_stream_output_targets;
#endif

   ice->vtbl.destroy_state = crocus_destroy_state;
   ice->vtbl.init_render_context = crocus_init_render_context;
   ice->vtbl.upload_render_state = crocus_upload_render_state;
#if GEN_GEN == 7
   ice->vtbl.init_compute_context = crocus_init_compute_context;
   ice->vtbl.upload_compute_state = crocus_upload_compute_state;
#endif
   ice->vtbl.emit_raw_pipe_control = crocus_emit_raw_pipe_control;
   ice->vtbl.emit_mi_report_perf_count = crocus_emit_mi_report_perf_count;
   ice->vtbl.rebind_buffer = crocus_rebind_buffer;
   ice->vtbl.load_register_reg32 = crocus_load_register_reg32;
   ice->vtbl.load_register_reg64 = crocus_load_register_reg64;
   ice->vtbl.load_register_imm32 = crocus_load_register_imm32;
   ice->vtbl.load_register_imm64 = crocus_load_register_imm64;
   ice->vtbl.load_register_mem32 = crocus_load_register_mem32;
   ice->vtbl.load_register_mem64 = crocus_load_register_mem64;
   ice->vtbl.store_register_mem32 = crocus_store_register_mem32;
   ice->vtbl.store_register_mem64 = crocus_store_register_mem64;
   ice->vtbl.store_data_imm32 = crocus_store_data_imm32;
   ice->vtbl.store_data_imm64 = crocus_store_data_imm64;
   ice->vtbl.copy_mem_mem = crocus_copy_mem_mem;
   ice->vtbl.derived_program_state_size = crocus_derived_program_state_size;
   ice->vtbl.update_surface_base_address = crocus_update_surface_base_address;
   ice->vtbl.store_derived_program_state = crocus_store_derived_program_state;
#if GEN_GEN == 7
   ice->vtbl.create_so_decl_list = crocus_create_so_decl_list;
#endif
   ice->vtbl.populate_vs_key = crocus_populate_vs_key;
   ice->vtbl.populate_tcs_key = crocus_populate_tcs_key;
   ice->vtbl.populate_tes_key = crocus_populate_tes_key;
   ice->vtbl.populate_gs_key = crocus_populate_gs_key;
   ice->vtbl.populate_fs_key = crocus_populate_fs_key;
   ice->vtbl.populate_cs_key = crocus_populate_cs_key;
   ice->vtbl.mocs = mocs;
   ice->vtbl.lost_genx_state = crocus_lost_genx_state;
#if GEN_VERSIONx10 == 75
   ice->vtbl.finish_batch = crocus_state_finish_batch;
#endif
#if GEN_GEN <= 5
   ice->vtbl.upload_urb_fence = crocus_upload_urb_fence;
   ice->vtbl.calculate_urb_fence = crocus_calculate_urb_fence;
#endif
   ice->vtbl.fill_clamp_mask = crocus_fill_clamp_mask;
   ice->state.dirty = ~0ull;

   ice->state.statistics_counters_enabled = true;

   genX(init_blt)(ice);
   ice->state.sample_mask = 0xff;
   ice->state.num_viewports = 1;
   ice->state.prim_mode = PIPE_PRIM_MAX;
   ice->state.genx = calloc(1, sizeof(struct crocus_genx_state));
   ice->draw.derived_params.drawid = -1;

#if 0
   /* Make a 1x1x1 null surface for unbound textures */
   void *null_surf_map =
      upload_state(ice->state.surface_uploader, &ice->state.unbound_tex,
                   4 * GENX(RENDER_SURFACE_STATE_length), 64);
   isl_null_fill_state(&screen->isl_dev, null_surf_map, isl_extent3d(1, 1, 1));
   ice->state.unbound_tex.offset +=
      crocus_bo_offset_from_base_address(crocus_resource_bo(ice->state.unbound_tex.res));
#endif
   /* Default all scissor rectangles to be empty regions. */
   for (int i = 0; i < CROCUS_MAX_VIEWPORTS; i++) {
      ice->state.scissors[i] = (struct pipe_scissor_state) {
         .minx = 1, .maxx = 0, .miny = 1, .maxy = 0,
      };
   }
}
