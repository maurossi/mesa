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
 * @file crocus_resolve.c
 *
 * This file handles resolve tracking for main and auxiliary surfaces.
 *
 * It also handles our cache tracking.  We have sets for the render cache,
 * depth cache, and so on.  If a BO is in a cache's set, then it may have
 * data in that cache.  The helpers take care of emitting flushes for
 * render-to-texture, format reinterpretation issues, and other situations.
 */

#include "util/hash_table.h"
#include "util/set.h"
#include "crocus_context.h"
#include "compiler/nir/nir.h"

/**
 * Disable auxiliary buffers if a renderbuffer is also bound as a texture
 * or shader image.  This causes a self-dependency, where both rendering
 * and sampling may concurrently read or write the CCS buffer, causing
 * incorrect pixels.
 */
static bool
disable_rb_aux_buffer(struct crocus_context *ice,
                      bool *draw_aux_buffer_disabled,
                      struct crocus_resource *tex_res,
                      unsigned min_level, unsigned num_levels,
                      const char *usage)
{
   struct pipe_framebuffer_state *cso_fb = &ice->state.framebuffer;
   bool found = false;

   /* We only need to worry about color compression and fast clears. */
   if (tex_res->aux.usage != ISL_AUX_USAGE_CCS_D &&
       tex_res->aux.usage != ISL_AUX_USAGE_CCS_E)
      return false;

   for (unsigned i = 0; i < cso_fb->nr_cbufs; i++) {
      struct crocus_surface *surf = (void *) cso_fb->cbufs[i];
      if (!surf)
         continue;

      struct crocus_resource *rb_res = (void *) surf->base.texture;

      if (rb_res->bo == tex_res->bo &&
          surf->base.u.tex.level >= min_level &&
          surf->base.u.tex.level < min_level + num_levels) {
         found = draw_aux_buffer_disabled[i] = true;
      }
   }

   if (found) {
      perf_debug(&ice->dbg,
                 "Disabling CCS because a renderbuffer is also bound %s.\n",
                 usage);
   }

   return found;
}

static void
resolve_sampler_views(struct crocus_context *ice,
                      struct crocus_batch *batch,
                      struct crocus_shader_state *shs,
                      const struct shader_info *info,
                      bool *draw_aux_buffer_disabled,
                      bool consider_framebuffer)
{
   uint32_t views = info ? (shs->bound_sampler_views & info->textures_used[0]) : 0;

   while (views) {
      const int i = u_bit_scan(&views);
      struct crocus_sampler_view *isv = shs->textures[i];
      struct crocus_resource *res = (void *) isv->base.texture;

      if (res->base.target != PIPE_BUFFER) {
         if (consider_framebuffer) {
            disable_rb_aux_buffer(ice, draw_aux_buffer_disabled,
                                  res, isv->view.base_level, isv->view.levels,
                                  "for sampling");
         }

         crocus_resource_prepare_texture(ice, batch, res, isv->view.format,
                                       isv->view.base_level, isv->view.levels,
                                       isv->view.base_array_layer,
                                       isv->view.array_len);
      }

      crocus_cache_flush_for_read(batch, res->bo);
   }
}

static void
resolve_image_views(struct crocus_context *ice,
                    struct crocus_batch *batch,
                    struct crocus_shader_state *shs,
                    bool *draw_aux_buffer_disabled,
                    bool consider_framebuffer)
{
   /* TODO: Consider images used by program */
   uint32_t views = shs->bound_image_views;

   while (views) {
      const int i = u_bit_scan(&views);
      struct pipe_image_view *pview = &shs->image[i].base;
      struct crocus_resource *res = (void *) pview->resource;

      if (res->base.target != PIPE_BUFFER) {
         if (consider_framebuffer) {
            disable_rb_aux_buffer(ice, draw_aux_buffer_disabled,
                                  res, pview->u.tex.level, 1,
                                  "as a shader image");
         }

         unsigned num_layers =
            pview->u.tex.last_layer - pview->u.tex.first_layer + 1;

         /* The data port doesn't understand any compression */
         crocus_resource_prepare_access(ice, batch, res,
                                      pview->u.tex.level, 1,
                                      pview->u.tex.first_layer, num_layers,
                                      ISL_AUX_USAGE_NONE, false);
      }

      crocus_cache_flush_for_read(batch, res->bo);
   }
}


/**
 * \brief Resolve buffers before drawing.
 *
 * Resolve the depth buffer's HiZ buffer, resolve the depth buffer of each
 * enabled depth texture, and flush the render cache for any dirty textures.
 */
void
crocus_predraw_resolve_inputs(struct crocus_context *ice,
                            struct crocus_batch *batch,
                            bool *draw_aux_buffer_disabled,
                            gl_shader_stage stage,
                            bool consider_framebuffer)
{
   struct crocus_shader_state *shs = &ice->state.shaders[stage];
   const struct shader_info *info = crocus_get_shader_info(ice, stage);

   uint64_t dirty = (CROCUS_DIRTY_BINDINGS_VS << stage) |
                    (consider_framebuffer ? CROCUS_DIRTY_BINDINGS_FS : 0);

   if (ice->state.dirty & dirty) {
      resolve_sampler_views(ice, batch, shs, info, draw_aux_buffer_disabled,
                            consider_framebuffer);
      resolve_image_views(ice, batch, shs, draw_aux_buffer_disabled,
                          consider_framebuffer);
   }
}

void
crocus_predraw_resolve_framebuffer(struct crocus_context *ice,
                                 struct crocus_batch *batch,
                                 bool *draw_aux_buffer_disabled)
{
   struct pipe_framebuffer_state *cso_fb = &ice->state.framebuffer;
   struct crocus_screen *screen = (void *) ice->ctx.screen;
   struct gen_device_info *devinfo = &screen->devinfo;

   if (ice->state.dirty & CROCUS_DIRTY_DEPTH_BUFFER) {
      struct pipe_surface *zs_surf = cso_fb->zsbuf;

      if (zs_surf) {
         struct crocus_resource *z_res, *s_res;
         crocus_get_depth_stencil_resources(devinfo, zs_surf->texture, &z_res, &s_res);
         unsigned num_layers =
            zs_surf->u.tex.last_layer - zs_surf->u.tex.first_layer + 1;

         if (z_res) {
            crocus_resource_prepare_depth(ice, batch, z_res,
                                        zs_surf->u.tex.level,
                                        zs_surf->u.tex.first_layer,
                                        num_layers);
            crocus_cache_flush_for_depth(batch, z_res->bo);
         }

         if (s_res) {
            crocus_cache_flush_for_depth(batch, s_res->bo);
         }
      }
   }

   if (ice->state.dirty & (CROCUS_DIRTY_BINDINGS_FS | CROCUS_DIRTY_BLEND_STATE)) {
      for (unsigned i = 0; i < cso_fb->nr_cbufs; i++) {
         struct crocus_surface *surf = (void *) cso_fb->cbufs[i];
         if (!surf)
            continue;

         struct crocus_resource *res = (void *) surf->base.texture;

         enum isl_aux_usage aux_usage =
            crocus_resource_render_aux_usage(ice, res, surf->view.format,
                                           ice->state.blend_enables & (1u << i),
                                           draw_aux_buffer_disabled[i]);

         if (ice->state.draw_aux_usage[i] != aux_usage) {
            ice->state.draw_aux_usage[i] = aux_usage;
            /* XXX: Need to track which bindings to make dirty */
            ice->state.dirty |= CROCUS_ALL_DIRTY_BINDINGS;
         }

         crocus_resource_prepare_render(ice, batch, res, surf->view.base_level,
                                      surf->view.base_array_layer,
                                      surf->view.array_len,
                                      aux_usage);

         crocus_cache_flush_for_render(batch, res->bo, surf->view.format,
                                     aux_usage);
      }
   }
}

/**
 * \brief Call this after drawing to mark which buffers need resolving
 *
 * If the depth buffer was written to and if it has an accompanying HiZ
 * buffer, then mark that it needs a depth resolve.
 *
 * If the color buffer is a multisample window system buffer, then
 * mark that it needs a downsample.
 *
 * Also mark any render targets which will be textured as needing a render
 * cache flush.
 */
void
crocus_postdraw_update_resolve_tracking(struct crocus_context *ice,
                                      struct crocus_batch *batch)
{
   struct pipe_framebuffer_state *cso_fb = &ice->state.framebuffer;
   struct crocus_screen *screen = (void *) ice->ctx.screen;
   struct gen_device_info *devinfo = &screen->devinfo;
   // XXX: front buffer drawing?

   bool may_have_resolved_depth =
      ice->state.dirty & (CROCUS_DIRTY_DEPTH_BUFFER |
                          CROCUS_DIRTY_WM_DEPTH_STENCIL);

   struct pipe_surface *zs_surf = cso_fb->zsbuf;
   if (zs_surf) {
      struct crocus_resource *z_res, *s_res;
      crocus_get_depth_stencil_resources(devinfo, zs_surf->texture, &z_res, &s_res);
      unsigned num_layers =
         zs_surf->u.tex.last_layer - zs_surf->u.tex.first_layer + 1;

      if (z_res) {
         if (may_have_resolved_depth) {
            crocus_resource_finish_depth(ice, z_res, zs_surf->u.tex.level,
                                       zs_surf->u.tex.first_layer, num_layers,
                                       ice->state.depth_writes_enabled);
         }

         if (ice->state.depth_writes_enabled)
            crocus_depth_cache_add_bo(batch, z_res->bo);
      }

      if (s_res) {
         if (may_have_resolved_depth && ice->state.stencil_writes_enabled) {
            crocus_resource_finish_write(ice, s_res, zs_surf->u.tex.level,
                                       zs_surf->u.tex.first_layer, num_layers,
                                       s_res->aux.usage);
         }

         if (ice->state.stencil_writes_enabled)
            crocus_depth_cache_add_bo(batch, s_res->bo);
      }
   }

   bool may_have_resolved_color =
      ice->state.dirty & (CROCUS_DIRTY_BINDINGS_FS | CROCUS_DIRTY_BLEND_STATE);

   for (unsigned i = 0; i < cso_fb->nr_cbufs; i++) {
      struct crocus_surface *surf = (void *) cso_fb->cbufs[i];
      if (!surf)
         continue;

      struct crocus_resource *res = (void *) surf->base.texture;
      enum isl_aux_usage aux_usage = ice->state.draw_aux_usage[i];

      crocus_render_cache_add_bo(batch, res->bo, surf->view.format,
                               aux_usage);

      if (may_have_resolved_color) {
         union pipe_surface_desc *desc = &surf->base.u;
         unsigned num_layers =
            desc->tex.last_layer - desc->tex.first_layer + 1;
         crocus_resource_finish_render(ice, res, desc->tex.level,
                                     desc->tex.first_layer, num_layers,
                                     aux_usage);
      }
   }
}

/**
 * Clear the cache-tracking sets.
 */
void
crocus_cache_sets_clear(struct crocus_batch *batch)
{
   hash_table_foreach(batch->cache.render, render_entry)
      _mesa_hash_table_remove(batch->cache.render, render_entry);

   set_foreach(batch->cache.depth, depth_entry)
      _mesa_set_remove(batch->cache.depth, depth_entry);
}

/**
 * Emits an appropriate flush for a BO if it has been rendered to within the
 * same batchbuffer as a read that's about to be emitted.
 *
 * The GPU has separate, incoherent caches for the render cache and the
 * sampler cache, along with other caches.  Usually data in the different
 * caches don't interact (e.g. we don't render to our driver-generated
 * immediate constant data), but for render-to-texture in FBOs we definitely
 * do.  When a batchbuffer is flushed, the kernel will ensure that everything
 * necessary is flushed before another use of that BO, but for reuse from
 * different caches within a batchbuffer, it's all our responsibility.
 */
void
crocus_flush_depth_and_render_caches(struct crocus_batch *batch)
{
   const struct gen_device_info *devinfo = &batch->screen->devinfo;
   if (devinfo->gen >= 6) {
      crocus_emit_pipe_control_flush(batch,
                                     "cache tracker: render-to-texture",
                                     PIPE_CONTROL_DEPTH_CACHE_FLUSH |
                                     PIPE_CONTROL_RENDER_TARGET_FLUSH |
                                     PIPE_CONTROL_CS_STALL);

      crocus_emit_pipe_control_flush(batch,
                                     "cache tracker: render-to-texture",
                                     PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE |
                                     PIPE_CONTROL_CONST_CACHE_INVALIDATE);
   } else {
      crocus_emit_mi_flush(batch);
   }

   crocus_cache_sets_clear(batch);
}

void
crocus_cache_flush_for_read(struct crocus_batch *batch,
                          struct crocus_bo *bo)
{
   if (_mesa_hash_table_search_pre_hashed(batch->cache.render, bo->hash, bo) ||
       _mesa_set_search_pre_hashed(batch->cache.depth, bo->hash, bo))
      crocus_flush_depth_and_render_caches(batch);
}

static void *
format_aux_tuple(enum isl_format format, enum isl_aux_usage aux_usage)
{
   return (void *)(uintptr_t)((uint32_t)format << 8 | aux_usage);
}

void
crocus_cache_flush_for_render(struct crocus_batch *batch,
                            struct crocus_bo *bo,
                            enum isl_format format,
                            enum isl_aux_usage aux_usage)
{
   if (_mesa_set_search_pre_hashed(batch->cache.depth, bo->hash, bo))
      crocus_flush_depth_and_render_caches(batch);

   /* Check to see if this bo has been used by a previous rendering operation
    * but with a different format or aux usage.  If it has, flush the render
    * cache so we ensure that it's only in there with one format or aux usage
    * at a time.
    *
    * Even though it's not obvious, this can easily happen in practice.
    * Suppose a client is blending on a surface with sRGB encode enabled on
    * gen9.  This implies that you get AUX_USAGE_CCS_D at best.  If the client
    * then disables sRGB decode and continues blending we will flip on
    * AUX_USAGE_CCS_E without doing any sort of resolve in-between (this is
    * perfectly valid since CCS_E is a subset of CCS_D).  However, this means
    * that we have fragments in-flight which are rendering with UNORM+CCS_E
    * and other fragments in-flight with SRGB+CCS_D on the same surface at the
    * same time and the pixel scoreboard and color blender are trying to sort
    * it all out.  This ends badly (i.e. GPU hangs).
    *
    * To date, we have never observed GPU hangs or even corruption to be
    * associated with switching the format, only the aux usage.  However,
    * there are comments in various docs which indicate that the render cache
    * isn't 100% resilient to format changes.  We may as well be conservative
    * and flush on format changes too.  We can always relax this later if we
    * find it to be a performance problem.
    */
   struct hash_entry *entry =
      _mesa_hash_table_search_pre_hashed(batch->cache.render, bo->hash, bo);
   if (entry && entry->data != format_aux_tuple(format, aux_usage))
      crocus_flush_depth_and_render_caches(batch);
}

void
crocus_render_cache_add_bo(struct crocus_batch *batch,
                         struct crocus_bo *bo,
                         enum isl_format format,
                         enum isl_aux_usage aux_usage)
{
#ifndef NDEBUG
   struct hash_entry *entry =
      _mesa_hash_table_search_pre_hashed(batch->cache.render, bo->hash, bo);
   if (entry) {
      /* Otherwise, someone didn't do a flush_for_render and that would be
       * very bad indeed.
       */
      assert(entry->data == format_aux_tuple(format, aux_usage));
   }
#endif

   _mesa_hash_table_insert_pre_hashed(batch->cache.render, bo->hash, bo,
                                      format_aux_tuple(format, aux_usage));
}

void
crocus_cache_flush_for_depth(struct crocus_batch *batch,
                           struct crocus_bo *bo)
{
   if (_mesa_hash_table_search_pre_hashed(batch->cache.render, bo->hash, bo))
      crocus_flush_depth_and_render_caches(batch);
}

void
crocus_depth_cache_add_bo(struct crocus_batch *batch, struct crocus_bo *bo)
{
   _mesa_set_add_pre_hashed(batch->cache.depth, bo->hash, bo);
}

static void
crocus_resolve_color(struct crocus_context *ice,
                   struct crocus_batch *batch,
                   struct crocus_resource *res,
                   unsigned level, unsigned layer,
                   enum isl_aux_op resolve_op)
{
   //DBG("%s to mt %p level %u layer %u\n", __FUNCTION__, mt, level, layer);

   struct blorp_surf surf;
   crocus_blorp_surf_for_resource(&ice->vtbl, &batch->screen->isl_dev, &surf,
                                &res->base, res->aux.usage, level, true);

   crocus_batch_maybe_flush(batch, 1500);

   /* Ivybridge PRM Vol 2, Part 1, "11.7 MCS Buffer for Render Target(s)":
    *
    *    "Any transition from any value in {Clear, Render, Resolve} to a
    *     different value in {Clear, Render, Resolve} requires end of pipe
    *     synchronization."
    *
    * In other words, fast clear ops are not properly synchronized with
    * other drawing.  We need to use a PIPE_CONTROL to ensure that the
    * contents of the previous draw hit the render target before we resolve
    * and again afterwards to ensure that the resolve is complete before we
    * do any more regular drawing.
    */
   crocus_emit_end_of_pipe_sync(batch, "color resolve: pre-flush",
                              PIPE_CONTROL_RENDER_TARGET_FLUSH);

   struct blorp_batch blorp_batch;
   blorp_batch_init(&ice->blorp, &blorp_batch, batch, 0);
   blorp_ccs_resolve(&blorp_batch, &surf, level, layer, 1,
		     isl_format_srgb_to_linear(res->surf.format),
		     resolve_op);
   blorp_batch_finish(&blorp_batch);

   /* See comment above */
   crocus_emit_end_of_pipe_sync(batch, "color resolve: post-flush",
                              PIPE_CONTROL_RENDER_TARGET_FLUSH);
}

static void
crocus_mcs_partial_resolve(struct crocus_context *ice,
                         struct crocus_batch *batch,
                         struct crocus_resource *res,
                         uint32_t start_layer,
                         uint32_t num_layers)
{
   //DBG("%s to mt %p layers %u-%u\n", __FUNCTION__, mt,
       //start_layer, start_layer + num_layers - 1);

   assert(isl_aux_usage_has_mcs(res->aux.usage));

   struct blorp_surf surf;
   crocus_blorp_surf_for_resource(&ice->vtbl, &batch->screen->isl_dev, &surf,
                                &res->base, res->aux.usage, 0, true);

   struct blorp_batch blorp_batch;
   blorp_batch_init(&ice->blorp, &blorp_batch, batch, 0);
   blorp_mcs_partial_resolve(&blorp_batch, &surf,
                             isl_format_srgb_to_linear(res->surf.format),
                             start_layer, num_layers);
   blorp_batch_finish(&blorp_batch);
}


/**
 * Return true if the format that will be used to access the resource is
 * CCS_E-compatible with the resource's linear/non-sRGB format.
 *
 * Why use the linear format?  Well, although the resourcemay be specified
 * with an sRGB format, the usage of that color space/format can be toggled.
 * Since our HW tends to support more linear formats than sRGB ones, we use
 * this format variant for check for CCS_E compatibility.
 */
static bool
format_ccs_e_compat_with_resource(const struct gen_device_info *devinfo,
                                  const struct crocus_resource *res,
                                  enum isl_format access_format)
{
   assert(res->aux.usage == ISL_AUX_USAGE_CCS_E);

   enum isl_format isl_format = isl_format_srgb_to_linear(res->surf.format);
   return isl_formats_are_ccs_e_compatible(devinfo, isl_format, access_format);
}

static bool
sample_with_depth_aux(const struct gen_device_info *devinfo,
                      const struct crocus_resource *res)
{
   switch (res->aux.usage) {
   case ISL_AUX_USAGE_HIZ:
      return false;
   case ISL_AUX_USAGE_HIZ_CCS:
      return false;
   case ISL_AUX_USAGE_HIZ_CCS_WT:
      break;
   default:
      return false;
   }

   /* It seems the hardware won't fallback to the depth buffer if some of the
    * mipmap levels aren't available in the HiZ buffer. So we need all levels
    * of the texture to be HiZ enabled.
    */
   for (unsigned level = 0; level < res->surf.levels; ++level) {
      if (!crocus_resource_level_has_hiz(res, level))
         return false;
   }

   /* If compressed multisampling is enabled, then we use it for the auxiliary
    * buffer instead.
    *
    * From the BDW PRM (Volume 2d: Command Reference: Structures
    *                   RENDER_SURFACE_STATE.AuxiliarySurfaceMode):
    *
    *  "If this field is set to AUX_HIZ, Number of Multisamples must be
    *   MULTISAMPLECOUNT_1, and Surface Type cannot be SURFTYPE_3D.
    *
    * There is no such blurb for 1D textures, but there is sufficient evidence
    * that this is broken on SKL+.
    */
   // XXX: i965 disables this for arrays too, is that reasonable?
   return res->surf.samples == 1 && res->surf.dim == ISL_SURF_DIM_2D;
}

/**
 * Perform a HiZ or depth resolve operation.
 *
 * For an overview of HiZ ops, see the following sections of the Sandy Bridge
 * PRM, Volume 1, Part 2:
 *   - 7.5.3.1 Depth Buffer Clear
 *   - 7.5.3.2 Depth Buffer Resolve
 *   - 7.5.3.3 Hierarchical Depth Buffer Resolve
 */
void
crocus_hiz_exec(struct crocus_context *ice,
              struct crocus_batch *batch,
              struct crocus_resource *res,
              unsigned int level, unsigned int start_layer,
              unsigned int num_layers, enum isl_aux_op op,
              bool update_clear_depth)
{
   assert(crocus_resource_level_has_hiz(res, level));
   assert(op != ISL_AUX_OP_NONE);
   UNUSED const char *name = NULL;

   switch (op) {
   case ISL_AUX_OP_FULL_RESOLVE:
      name = "depth resolve";
      break;
   case ISL_AUX_OP_AMBIGUATE:
      name = "hiz ambiguate";
      break;
   case ISL_AUX_OP_FAST_CLEAR:
      name = "depth clear";
      break;
   case ISL_AUX_OP_PARTIAL_RESOLVE:
   case ISL_AUX_OP_NONE:
      unreachable("Invalid HiZ op");
   }

   //DBG("%s %s to mt %p level %d layers %d-%d\n",
       //__func__, name, mt, level, start_layer, start_layer + num_layers - 1);

   /* The following stalls and flushes are only documented to be required
    * for HiZ clear operations.  However, they also seem to be required for
    * resolve operations.
    *
    * From the Ivybridge PRM, volume 2, "Depth Buffer Clear":
    *
    *   "If other rendering operations have preceded this clear, a
    *    PIPE_CONTROL with depth cache flush enabled, Depth Stall bit
    *    enabled must be issued before the rectangle primitive used for
    *    the depth buffer clear operation."
    *
    * Same applies for Gen8 and Gen9.
    *
    * In addition, from the Ivybridge PRM, volume 2, 1.10.4.1
    * PIPE_CONTROL, Depth Cache Flush Enable:
    *
    *   "This bit must not be set when Depth Stall Enable bit is set in
    *    this packet."
    *
    * This is confirmed to hold for real, Haswell gets immediate gpu hangs.
    *
    * Therefore issue two pipe control flushes, one for cache flush and
    * another for depth stall.
    */
   crocus_emit_pipe_control_flush(batch,
                                "hiz op: pre-flushes (1/2)",
                                PIPE_CONTROL_DEPTH_CACHE_FLUSH |
                                PIPE_CONTROL_CS_STALL);

   crocus_emit_pipe_control_flush(batch, "hiz op: pre-flushes (2/2)",
                                PIPE_CONTROL_DEPTH_STALL);

   assert(isl_aux_usage_has_hiz(res->aux.usage) && res->aux.bo);

   crocus_batch_maybe_flush(batch, 1500);

   struct blorp_surf surf;
   crocus_blorp_surf_for_resource(&ice->vtbl, &batch->screen->isl_dev, &surf,
                                &res->base, res->aux.usage, level, true);

   struct blorp_batch blorp_batch;
   enum blorp_batch_flags flags = 0;
   flags |= update_clear_depth ? 0 : BLORP_BATCH_NO_UPDATE_CLEAR_COLOR;
   blorp_batch_init(&ice->blorp, &blorp_batch, batch, flags);
   blorp_hiz_op(&blorp_batch, &surf, level, start_layer, num_layers, op);
   blorp_batch_finish(&blorp_batch);

   /* The following stalls and flushes are only documented to be required
    * for HiZ clear operations.  However, they also seem to be required for
    * resolve operations.
    *
    * From the Broadwell PRM, volume 7, "Depth Buffer Clear":
    *
    *    "Depth buffer clear pass using any of the methods (WM_STATE,
    *     3DSTATE_WM or 3DSTATE_WM_HZ_OP) must be followed by a
    *     PIPE_CONTROL command with DEPTH_STALL bit and Depth FLUSH bits
    *     "set" before starting to render.  DepthStall and DepthFlush are
    *     not needed between consecutive depth clear passes nor is it
    *     required if the depth clear pass was done with
    *     'full_surf_clear' bit set in the 3DSTATE_WM_HZ_OP."
    *
    * TODO: Such as the spec says, this could be conditional.
    */
   crocus_emit_pipe_control_flush(batch,
                                "hiz op: post flush",
                                PIPE_CONTROL_DEPTH_CACHE_FLUSH |
                                PIPE_CONTROL_DEPTH_STALL);
}

/**
 * Does the resource's slice have hiz enabled?
 */
bool
crocus_resource_level_has_hiz(const struct crocus_resource *res, uint32_t level)
{
   crocus_resource_check_level_layer(res, level, 0);
   return res->aux.has_hiz & 1 << level;
}

/** \brief Assert that the level and layer are valid for the resource. */
void
crocus_resource_check_level_layer(UNUSED const struct crocus_resource *res,
                                UNUSED uint32_t level, UNUSED uint32_t layer)
{
   assert(level < res->surf.levels);
   assert(layer < util_num_layers(&res->base, level));
}

static inline uint32_t
miptree_level_range_length(const struct crocus_resource *res,
                           uint32_t start_level, uint32_t num_levels)
{
   assert(start_level < res->surf.levels);

   if (num_levels == INTEL_REMAINING_LAYERS)
      num_levels = res->surf.levels;

   /* Check for overflow */
   assert(start_level + num_levels >= start_level);
   assert(start_level + num_levels <= res->surf.levels);

   return num_levels;
}

static inline uint32_t
miptree_layer_range_length(const struct crocus_resource *res, uint32_t level,
                           uint32_t start_layer, uint32_t num_layers)
{
   assert(level <= res->base.last_level);

   const uint32_t total_num_layers = crocus_get_num_logical_layers(res, level);
   assert(start_layer < total_num_layers);
   if (num_layers == INTEL_REMAINING_LAYERS)
      num_layers = total_num_layers - start_layer;
   /* Check for overflow */
   assert(start_layer + num_layers >= start_layer);
   assert(start_layer + num_layers <= total_num_layers);

   return num_layers;
}

bool
crocus_has_color_unresolved(const struct crocus_resource *res,
                          unsigned start_level, unsigned num_levels,
                          unsigned start_layer, unsigned num_layers)
{
   if (!res->aux.bo)
      return false;

   /* Clamp the level range to fit the resource */
   num_levels = miptree_level_range_length(res, start_level, num_levels);

   for (uint32_t l = 0; l < num_levels; l++) {
      const uint32_t level = start_level + l;
      const uint32_t level_layers =
         miptree_layer_range_length(res, level, start_layer, num_layers);
      for (unsigned a = 0; a < level_layers; a++) {
         enum isl_aux_state aux_state =
            crocus_resource_get_aux_state(res, level, start_layer + a);
         assert(aux_state != ISL_AUX_STATE_AUX_INVALID);
         if (aux_state != ISL_AUX_STATE_PASS_THROUGH)
            return true;
      }
   }

   return false;
}

static enum isl_aux_op
get_ccs_d_resolve_op(enum isl_aux_state aux_state,
                     enum isl_aux_usage aux_usage,
                     bool fast_clear_supported)
{
   assert(aux_usage == ISL_AUX_USAGE_NONE || aux_usage == ISL_AUX_USAGE_CCS_D);

   const bool ccs_supported =
      (aux_usage == ISL_AUX_USAGE_CCS_D) && fast_clear_supported;

   switch (aux_state) {
   case ISL_AUX_STATE_CLEAR:
   case ISL_AUX_STATE_PARTIAL_CLEAR:
      if (!ccs_supported)
         return ISL_AUX_OP_FULL_RESOLVE;
      else
         return ISL_AUX_OP_NONE;

   case ISL_AUX_STATE_PASS_THROUGH:
      return ISL_AUX_OP_NONE;

   case ISL_AUX_STATE_RESOLVED:
   case ISL_AUX_STATE_AUX_INVALID:
   case ISL_AUX_STATE_COMPRESSED_CLEAR:
   case ISL_AUX_STATE_COMPRESSED_NO_CLEAR:
      break;
   }

   unreachable("Invalid aux state for CCS_D");
}

static enum isl_aux_op
get_ccs_e_resolve_op(enum isl_aux_state aux_state,
                     enum isl_aux_usage aux_usage,
                     bool fast_clear_supported)
{
   /* CCS_E surfaces can be accessed as CCS_D if we're careful. */
   assert(aux_usage == ISL_AUX_USAGE_NONE ||
          aux_usage == ISL_AUX_USAGE_CCS_D ||
          aux_usage == ISL_AUX_USAGE_CCS_E);

   switch (aux_state) {
   case ISL_AUX_STATE_CLEAR:
   case ISL_AUX_STATE_PARTIAL_CLEAR:
      if (fast_clear_supported)
         return ISL_AUX_OP_NONE;
      else if (aux_usage == ISL_AUX_USAGE_CCS_E)
         return ISL_AUX_OP_PARTIAL_RESOLVE;
      else
         return ISL_AUX_OP_FULL_RESOLVE;

   case ISL_AUX_STATE_COMPRESSED_CLEAR:
      if (aux_usage != ISL_AUX_USAGE_CCS_E)
         return ISL_AUX_OP_FULL_RESOLVE;
      else if (!fast_clear_supported)
         return ISL_AUX_OP_PARTIAL_RESOLVE;
      else
         return ISL_AUX_OP_NONE;

   case ISL_AUX_STATE_COMPRESSED_NO_CLEAR:
      if (aux_usage != ISL_AUX_USAGE_CCS_E)
         return ISL_AUX_OP_FULL_RESOLVE;
      else
         return ISL_AUX_OP_NONE;

   case ISL_AUX_STATE_PASS_THROUGH:
      return ISL_AUX_OP_NONE;

   case ISL_AUX_STATE_RESOLVED:
   case ISL_AUX_STATE_AUX_INVALID:
      break;
   }

   unreachable("Invalid aux state for CCS_E");
}

static void
crocus_resource_prepare_ccs_access(struct crocus_context *ice,
                                 struct crocus_batch *batch,
                                 struct crocus_resource *res,
                                 uint32_t level, uint32_t layer,
                                 enum isl_aux_usage aux_usage,
                                 bool fast_clear_supported)
{
   enum isl_aux_state aux_state = crocus_resource_get_aux_state(res, level, layer);

   enum isl_aux_op resolve_op;
   if (res->aux.usage == ISL_AUX_USAGE_CCS_E) {
      resolve_op = get_ccs_e_resolve_op(aux_state, aux_usage,
                                        fast_clear_supported);
   } else {
      assert(res->aux.usage == ISL_AUX_USAGE_CCS_D);
      resolve_op = get_ccs_d_resolve_op(aux_state, aux_usage,
                                        fast_clear_supported);
   }

   if (resolve_op != ISL_AUX_OP_NONE) {
      crocus_resolve_color(ice, batch, res, level, layer, resolve_op);

      switch (resolve_op) {
      case ISL_AUX_OP_FULL_RESOLVE:
         /* The CCS full resolve operation destroys the CCS and sets it to the
          * pass-through state.  (You can also think of this as being both a
          * resolve and an ambiguate in one operation.)
          */
         crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                     ISL_AUX_STATE_PASS_THROUGH);
         break;

      case ISL_AUX_OP_PARTIAL_RESOLVE:
         crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                     ISL_AUX_STATE_COMPRESSED_NO_CLEAR);
         break;

      default:
         unreachable("Invalid resolve op");
      }
   }
}

static void
crocus_resource_finish_ccs_write(struct crocus_context *ice,
                               struct crocus_resource *res,
                               uint32_t level, uint32_t layer,
                               enum isl_aux_usage aux_usage)
{
   assert(aux_usage == ISL_AUX_USAGE_NONE ||
          aux_usage == ISL_AUX_USAGE_CCS_D ||
          aux_usage == ISL_AUX_USAGE_CCS_E);

   enum isl_aux_state aux_state =
      crocus_resource_get_aux_state(res, level, layer);

   if (res->aux.usage == ISL_AUX_USAGE_CCS_E) {
      switch (aux_state) {
      case ISL_AUX_STATE_CLEAR:
      case ISL_AUX_STATE_PARTIAL_CLEAR:
         assert(aux_usage == ISL_AUX_USAGE_CCS_E ||
                aux_usage == ISL_AUX_USAGE_CCS_D);

         if (aux_usage == ISL_AUX_USAGE_CCS_E) {
            crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                        ISL_AUX_STATE_COMPRESSED_CLEAR);
         } else if (aux_state != ISL_AUX_STATE_PARTIAL_CLEAR) {
            crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                        ISL_AUX_STATE_PARTIAL_CLEAR);
         }
         break;

      case ISL_AUX_STATE_COMPRESSED_CLEAR:
      case ISL_AUX_STATE_COMPRESSED_NO_CLEAR:
         assert(aux_usage == ISL_AUX_USAGE_CCS_E);
         break; /* Nothing to do */

      case ISL_AUX_STATE_PASS_THROUGH:
         if (aux_usage == ISL_AUX_USAGE_CCS_E) {
            crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                        ISL_AUX_STATE_COMPRESSED_NO_CLEAR);
         } else {
            /* Nothing to do */
         }
         break;

      case ISL_AUX_STATE_RESOLVED:
      case ISL_AUX_STATE_AUX_INVALID:
         unreachable("Invalid aux state for CCS_E");
      }
   } else {
      assert(res->aux.usage == ISL_AUX_USAGE_CCS_D);
      /* CCS_D is a bit simpler */
      switch (aux_state) {
      case ISL_AUX_STATE_CLEAR:
         assert(aux_usage == ISL_AUX_USAGE_CCS_D);
         crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                     ISL_AUX_STATE_PARTIAL_CLEAR);
         break;

      case ISL_AUX_STATE_PARTIAL_CLEAR:
         assert(aux_usage == ISL_AUX_USAGE_CCS_D);
         break; /* Nothing to do */

      case ISL_AUX_STATE_PASS_THROUGH:
         /* Nothing to do */
         break;

      case ISL_AUX_STATE_COMPRESSED_CLEAR:
      case ISL_AUX_STATE_COMPRESSED_NO_CLEAR:
      case ISL_AUX_STATE_RESOLVED:
      case ISL_AUX_STATE_AUX_INVALID:
         unreachable("Invalid aux state for CCS_D");
      }
   }
}

static void
crocus_resource_prepare_mcs_access(struct crocus_context *ice,
                                 struct crocus_batch *batch,
                                 struct crocus_resource *res,
                                 uint32_t layer,
                                 enum isl_aux_usage aux_usage,
                                 bool fast_clear_supported)
{
   assert(isl_aux_usage_has_mcs(aux_usage));

   switch (crocus_resource_get_aux_state(res, 0, layer)) {
   case ISL_AUX_STATE_CLEAR:
   case ISL_AUX_STATE_COMPRESSED_CLEAR:
      if (!fast_clear_supported) {
         crocus_mcs_partial_resolve(ice, batch, res, layer, 1);
         crocus_resource_set_aux_state(ice, res, 0, layer, 1,
                                     ISL_AUX_STATE_COMPRESSED_NO_CLEAR);
      }
      break;

   case ISL_AUX_STATE_COMPRESSED_NO_CLEAR:
      break; /* Nothing to do */

   case ISL_AUX_STATE_RESOLVED:
   case ISL_AUX_STATE_PASS_THROUGH:
   case ISL_AUX_STATE_AUX_INVALID:
   case ISL_AUX_STATE_PARTIAL_CLEAR:
      unreachable("Invalid aux state for MCS");
   }
}

static void
crocus_resource_finish_mcs_write(struct crocus_context *ice,
                               struct crocus_resource *res,
                               uint32_t layer,
                               enum isl_aux_usage aux_usage)
{
   assert(isl_aux_usage_has_mcs(aux_usage));

   switch (crocus_resource_get_aux_state(res, 0, layer)) {
   case ISL_AUX_STATE_CLEAR:
      crocus_resource_set_aux_state(ice, res, 0, layer, 1,
                                  ISL_AUX_STATE_COMPRESSED_CLEAR);
      break;

   case ISL_AUX_STATE_COMPRESSED_CLEAR:
   case ISL_AUX_STATE_COMPRESSED_NO_CLEAR:
      break; /* Nothing to do */

   case ISL_AUX_STATE_RESOLVED:
   case ISL_AUX_STATE_PASS_THROUGH:
   case ISL_AUX_STATE_AUX_INVALID:
   case ISL_AUX_STATE_PARTIAL_CLEAR:
      unreachable("Invalid aux state for MCS");
   }
}

static void
crocus_resource_prepare_hiz_access(struct crocus_context *ice,
                                 struct crocus_batch *batch,
                                 struct crocus_resource *res,
                                 uint32_t level, uint32_t layer,
                                 enum isl_aux_usage aux_usage,
                                 bool fast_clear_supported)
{
   assert(aux_usage == ISL_AUX_USAGE_NONE ||
          aux_usage == ISL_AUX_USAGE_HIZ ||
          aux_usage == ISL_AUX_USAGE_HIZ_CCS ||
          aux_usage == ISL_AUX_USAGE_CCS_E);

   enum isl_aux_op hiz_op = ISL_AUX_OP_NONE;
   switch (crocus_resource_get_aux_state(res, level, layer)) {
   case ISL_AUX_STATE_CLEAR:
   case ISL_AUX_STATE_COMPRESSED_CLEAR:
      if (aux_usage == ISL_AUX_USAGE_NONE || !fast_clear_supported)
         hiz_op = ISL_AUX_OP_FULL_RESOLVE;
      break;

   case ISL_AUX_STATE_COMPRESSED_NO_CLEAR:
      if (aux_usage == ISL_AUX_USAGE_NONE)
         hiz_op = ISL_AUX_OP_FULL_RESOLVE;
      break;

   case ISL_AUX_STATE_PASS_THROUGH:
   case ISL_AUX_STATE_RESOLVED:
      break;

   case ISL_AUX_STATE_AUX_INVALID:
      if (aux_usage != ISL_AUX_USAGE_NONE)
         hiz_op = ISL_AUX_OP_AMBIGUATE;
      break;

   case ISL_AUX_STATE_PARTIAL_CLEAR:
      unreachable("Invalid HiZ state");
   }

   if (hiz_op != ISL_AUX_OP_NONE) {
      crocus_hiz_exec(ice, batch, res, level, layer, 1, hiz_op, false);

      switch (hiz_op) {
      case ISL_AUX_OP_FULL_RESOLVE:
         crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                     ISL_AUX_STATE_RESOLVED);
         break;

      case ISL_AUX_OP_AMBIGUATE:
         /* The HiZ resolve operation is actually an ambiguate */
         crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                     ISL_AUX_STATE_PASS_THROUGH);
         break;

      default:
         unreachable("Invalid HiZ op");
      }
   }
}

static void
crocus_resource_finish_hiz_write(struct crocus_context *ice,
                               struct crocus_resource *res,
                               uint32_t level, uint32_t layer,
                               enum isl_aux_usage aux_usage)
{
   assert(aux_usage == ISL_AUX_USAGE_NONE ||
          isl_aux_usage_has_hiz(aux_usage));

   switch (crocus_resource_get_aux_state(res, level, layer)) {
   case ISL_AUX_STATE_CLEAR:
      assert(isl_aux_usage_has_hiz(aux_usage));
      crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                  ISL_AUX_STATE_COMPRESSED_CLEAR);
      break;

   case ISL_AUX_STATE_COMPRESSED_NO_CLEAR:
   case ISL_AUX_STATE_COMPRESSED_CLEAR:
      assert(isl_aux_usage_has_hiz(aux_usage));
      break; /* Nothing to do */

   case ISL_AUX_STATE_RESOLVED:
      if (isl_aux_usage_has_hiz(aux_usage)) {
         crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                     ISL_AUX_STATE_COMPRESSED_NO_CLEAR);
      } else {
         crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                     ISL_AUX_STATE_AUX_INVALID);
      }
      break;

   case ISL_AUX_STATE_PASS_THROUGH:
      if (isl_aux_usage_has_hiz(aux_usage)) {
         crocus_resource_set_aux_state(ice, res, level, layer, 1,
                                     ISL_AUX_STATE_COMPRESSED_NO_CLEAR);
      }
      break;

   case ISL_AUX_STATE_AUX_INVALID:
      assert(!isl_aux_usage_has_hiz(aux_usage));
      break;

   case ISL_AUX_STATE_PARTIAL_CLEAR:
      unreachable("Invalid HiZ state");
   }
}

void
crocus_resource_prepare_access(struct crocus_context *ice,
                             struct crocus_batch *batch,
                             struct crocus_resource *res,
                             uint32_t start_level, uint32_t num_levels,
                             uint32_t start_layer, uint32_t num_layers,
                             enum isl_aux_usage aux_usage,
                             bool fast_clear_supported)
{
   num_levels = miptree_level_range_length(res, start_level, num_levels);

   switch (res->aux.usage) {
   case ISL_AUX_USAGE_NONE:
      /* Nothing to do */
      break;

   case ISL_AUX_USAGE_MCS:
   case ISL_AUX_USAGE_MCS_CCS:
      assert(start_level == 0 && num_levels == 1);
      const uint32_t level_layers =
         miptree_layer_range_length(res, 0, start_layer, num_layers);
      for (uint32_t a = 0; a < level_layers; a++) {
         crocus_resource_prepare_mcs_access(ice, batch, res, start_layer + a,
                                          aux_usage, fast_clear_supported);
      }
      break;

   case ISL_AUX_USAGE_CCS_D:
   case ISL_AUX_USAGE_CCS_E:
      for (uint32_t l = 0; l < num_levels; l++) {
         const uint32_t level = start_level + l;
         const uint32_t level_layers =
            miptree_layer_range_length(res, level, start_layer, num_layers);
         for (uint32_t a = 0; a < level_layers; a++) {
            crocus_resource_prepare_ccs_access(ice, batch, res, level,
                                             start_layer + a,
                                             aux_usage, fast_clear_supported);
         }
      }
      break;

   case ISL_AUX_USAGE_HIZ:
   case ISL_AUX_USAGE_HIZ_CCS:
      for (uint32_t l = 0; l < num_levels; l++) {
         const uint32_t level = start_level + l;
         if (!crocus_resource_level_has_hiz(res, level))
            continue;

         const uint32_t level_layers =
            miptree_layer_range_length(res, level, start_layer, num_layers);
         for (uint32_t a = 0; a < level_layers; a++) {
            crocus_resource_prepare_hiz_access(ice, batch, res, level,
                                             start_layer + a, aux_usage,
                                             fast_clear_supported);
         }
      }
      break;

   default:
      unreachable("Invalid aux usage");
   }
}

void
crocus_resource_finish_write(struct crocus_context *ice,
                           struct crocus_resource *res, uint32_t level,
                           uint32_t start_layer, uint32_t num_layers,
                           enum isl_aux_usage aux_usage)
{
   num_layers = miptree_layer_range_length(res, level, start_layer, num_layers);

   switch (res->aux.usage) {
   case ISL_AUX_USAGE_NONE:
      break;

   case ISL_AUX_USAGE_MCS:
   case ISL_AUX_USAGE_MCS_CCS:
      for (uint32_t a = 0; a < num_layers; a++) {
         crocus_resource_finish_mcs_write(ice, res, start_layer + a,
                                        aux_usage);
      }
      break;

   case ISL_AUX_USAGE_CCS_D:
   case ISL_AUX_USAGE_CCS_E:
      for (uint32_t a = 0; a < num_layers; a++) {
         crocus_resource_finish_ccs_write(ice, res, level, start_layer + a,
                                        aux_usage);
      }
      break;

   case ISL_AUX_USAGE_HIZ:
   case ISL_AUX_USAGE_HIZ_CCS:
      if (!crocus_resource_level_has_hiz(res, level))
         return;

      for (uint32_t a = 0; a < num_layers; a++) {
         crocus_resource_finish_hiz_write(ice, res, level, start_layer + a,
                                        aux_usage);
      }
      break;

   default:
      unreachable("Invavlid aux usage");
   }
}

enum isl_aux_state
crocus_resource_get_aux_state(const struct crocus_resource *res,
                            uint32_t level, uint32_t layer)
{
   crocus_resource_check_level_layer(res, level, layer);

   if (res->surf.usage & ISL_SURF_USAGE_DEPTH_BIT) {
      assert(crocus_resource_level_has_hiz(res, level));
   } else {
      assert(res->surf.samples == 1 ||
             res->surf.msaa_layout == ISL_MSAA_LAYOUT_ARRAY);
   }

   return res->aux.state[level][layer];
}

void
crocus_resource_set_aux_state(struct crocus_context *ice,
                            struct crocus_resource *res, uint32_t level,
                            uint32_t start_layer, uint32_t num_layers,
                            enum isl_aux_state aux_state)
{
   num_layers = miptree_layer_range_length(res, level, start_layer, num_layers);

   if (res->surf.usage & ISL_SURF_USAGE_DEPTH_BIT) {
      assert(crocus_resource_level_has_hiz(res, level));
   } else {
      assert(res->surf.samples == 1 ||
             res->surf.msaa_layout == ISL_MSAA_LAYOUT_ARRAY);
   }

   for (unsigned a = 0; a < num_layers; a++) {
      if (res->aux.state[level][start_layer + a] != aux_state) {
         res->aux.state[level][start_layer + a] = aux_state;
         /* XXX: Need to track which bindings to make dirty */
         ice->state.dirty |= CROCUS_ALL_DIRTY_BINDINGS;
      }
   }
}

/* On Gen9 color buffers may be compressed by the hardware (lossless
 * compression). There are, however, format restrictions and care needs to be
 * taken that the sampler engine is capable for re-interpreting a buffer with
 * format different the buffer was originally written with.
 *
 * For example, SRGB formats are not compressible and the sampler engine isn't
 * capable of treating RGBA_UNORM as SRGB_ALPHA. In such a case the underlying
 * color buffer needs to be resolved so that the sampling surface can be
 * sampled as non-compressed (i.e., without the auxiliary MCS buffer being
 * set).
 */
static bool
can_texture_with_ccs(const struct gen_device_info *devinfo,
                     struct pipe_debug_callback *dbg,
                     const struct crocus_resource *res,
                     enum isl_format view_format)
{
   if (res->aux.usage != ISL_AUX_USAGE_CCS_E)
      return false;

   if (!format_ccs_e_compat_with_resource(devinfo, res, view_format)) {
      const struct isl_format_layout *res_fmtl =
         isl_format_get_layout(res->surf.format);
      const struct isl_format_layout *view_fmtl =
         isl_format_get_layout(view_format);

      perf_debug(dbg, "Incompatible sampling format (%s) for CCS (%s)\n",
                 view_fmtl->name, res_fmtl->name);

      return false;
   }

   return true;
}

enum isl_aux_usage
crocus_resource_texture_aux_usage(struct crocus_context *ice,
                                const struct crocus_resource *res,
                                enum isl_format view_format)
{
   struct crocus_screen *screen = (void *) ice->ctx.screen;
   struct gen_device_info *devinfo = &screen->devinfo;

   switch (res->aux.usage) {
   case ISL_AUX_USAGE_HIZ:
      if (sample_with_depth_aux(devinfo, res))
         return ISL_AUX_USAGE_HIZ;
      break;

   case ISL_AUX_USAGE_HIZ_CCS:
      if (sample_with_depth_aux(devinfo, res))
         return ISL_AUX_USAGE_CCS_E;
      break;

   case ISL_AUX_USAGE_MCS:
   case ISL_AUX_USAGE_MCS_CCS:
      return res->aux.usage;

   case ISL_AUX_USAGE_CCS_D:
   case ISL_AUX_USAGE_CCS_E:
      /* If we don't have any unresolved color, report an aux usage of
       * ISL_AUX_USAGE_NONE.  This way, texturing won't even look at the
       * aux surface and we can save some bandwidth.
       */
      if (!crocus_has_color_unresolved(res, 0, INTEL_REMAINING_LEVELS,
                                     0, INTEL_REMAINING_LAYERS))
         return ISL_AUX_USAGE_NONE;

      if (can_texture_with_ccs(devinfo, &ice->dbg, res, view_format))
         return ISL_AUX_USAGE_CCS_E;
      break;

   default:
      break;
   }

   return ISL_AUX_USAGE_NONE;
}

static bool
isl_formats_are_fast_clear_compatible(enum isl_format a, enum isl_format b)
{
   /* On gen8 and earlier, the hardware was only capable of handling 0/1 clear
    * values so sRGB curve application was a no-op for all fast-clearable
    * formats.
    *
    * On gen9+, the hardware supports arbitrary clear values.  For sRGB clear
    * values, the hardware interprets the floats, not as what would be
    * returned from the sampler (or written by the shader), but as being
    * between format conversion and sRGB curve application.  This means that
    * we can switch between sRGB and UNORM without having to whack the clear
    * color.
    */
   return isl_format_srgb_to_linear(a) == isl_format_srgb_to_linear(b);
}

void
crocus_resource_prepare_texture(struct crocus_context *ice,
                              struct crocus_batch *batch,
                              struct crocus_resource *res,
                              enum isl_format view_format,
                              uint32_t start_level, uint32_t num_levels,
                              uint32_t start_layer, uint32_t num_layers)
{
   enum isl_aux_usage aux_usage =
      crocus_resource_texture_aux_usage(ice, res, view_format);

   bool clear_supported = aux_usage != ISL_AUX_USAGE_NONE;

   /* Clear color is specified as ints or floats and the conversion is done by
    * the sampler.  If we have a texture view, we would have to perform the
    * clear color conversion manually.  Just disable clear color.
    */
   if (!isl_formats_are_fast_clear_compatible(res->surf.format, view_format))
      clear_supported = false;

   crocus_resource_prepare_access(ice, batch, res, start_level, num_levels,
                                start_layer, num_layers,
                                aux_usage, clear_supported);
}

enum isl_aux_usage
crocus_resource_render_aux_usage(struct crocus_context *ice,
                               struct crocus_resource *res,
                               enum isl_format render_format,
                               bool blend_enabled,
                               bool draw_aux_disabled)
{
   struct crocus_screen *screen = (void *) ice->ctx.screen;
   struct gen_device_info *devinfo = &screen->devinfo;

   if (draw_aux_disabled)
      return ISL_AUX_USAGE_NONE;

   switch (res->aux.usage) {
   case ISL_AUX_USAGE_MCS:
   case ISL_AUX_USAGE_MCS_CCS:
      return res->aux.usage;

   case ISL_AUX_USAGE_CCS_D:
   case ISL_AUX_USAGE_CCS_E:
      /* Gen9+ hardware technically supports non-0/1 clear colors with sRGB
       * formats.  However, there are issues with blending where it doesn't
       * properly apply the sRGB curve to the clear color when blending.
       */
      if (res->aux.usage == ISL_AUX_USAGE_CCS_E &&
          format_ccs_e_compat_with_resource(devinfo, res, render_format))
         return ISL_AUX_USAGE_CCS_E;

      /* Otherwise, we try to fall back to CCS_D */
      if (isl_format_supports_ccs_d(devinfo, render_format))
         return ISL_AUX_USAGE_CCS_D;

      return ISL_AUX_USAGE_NONE;

   default:
      return ISL_AUX_USAGE_NONE;
   }
}

void
crocus_resource_prepare_render(struct crocus_context *ice,
                             struct crocus_batch *batch,
                             struct crocus_resource *res, uint32_t level,
                             uint32_t start_layer, uint32_t layer_count,
                             enum isl_aux_usage aux_usage)
{
   crocus_resource_prepare_access(ice, batch, res, level, 1, start_layer,
                                layer_count, aux_usage,
                                aux_usage != ISL_AUX_USAGE_NONE);
}

void
crocus_resource_finish_render(struct crocus_context *ice,
                            struct crocus_resource *res, uint32_t level,
                            uint32_t start_layer, uint32_t layer_count,
                            enum isl_aux_usage aux_usage)
{
   crocus_resource_finish_write(ice, res, level, start_layer, layer_count,
                              aux_usage);
}

void
crocus_resource_prepare_depth(struct crocus_context *ice,
                            struct crocus_batch *batch,
                            struct crocus_resource *res, uint32_t level,
                            uint32_t start_layer, uint32_t layer_count)
{
   crocus_resource_prepare_access(ice, batch, res, level, 1, start_layer,
                                layer_count, res->aux.usage, !!res->aux.bo);
}

void
crocus_resource_finish_depth(struct crocus_context *ice,
                           struct crocus_resource *res, uint32_t level,
                           uint32_t start_layer, uint32_t layer_count,
                           bool depth_written)
{
   if (depth_written) {
      crocus_resource_finish_write(ice, res, level, start_layer, layer_count,
                                 res->aux.usage);
   }
}
