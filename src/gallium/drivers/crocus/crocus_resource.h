/*
 * Copyright 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef CROCUS_RESOURCE_H
#define CROCUS_RESOURCE_H

#include "pipe/p_state.h"
#include "util/u_inlines.h"
#include "util/u_range.h"
#include "intel/isl/isl.h"

struct crocus_batch;
struct crocus_context;

#define CROCUS_MAX_MIPLEVELS 15

struct crocus_format_info {
   enum isl_format fmt;
   struct isl_swizzle swizzle;
};

/**
 * Resources represent a GPU buffer object or image (mipmap tree).
 *
 * They contain the storage (BO) and layout information (ISL surface).
 */
struct crocus_resource {
   struct pipe_resource base;
   enum pipe_format internal_format;

   /**
    * The ISL surface layout information for this resource.
    *
    * This is not filled out for PIPE_BUFFER resources, but is guaranteed
    * to be zeroed.  Note that this also guarantees that res->surf.tiling
    * will be ISL_TILING_LINEAR, so it's safe to check that.
    */
   struct isl_surf surf;

   /** Backing storage for the resource */
   struct crocus_bo *bo;

   /** offset at which data starts in the BO */
   uint64_t offset;

   /**
    * A bitfield of PIPE_BIND_* indicating how this resource was bound
    * in the past.  Only meaningful for PIPE_BUFFER; used for flushing.
    */
   unsigned bind_history;

   /**
    * A bitfield of MESA_SHADER_* stages indicating where this resource
    * was bound.
    */
   unsigned bind_stages;

   /**
    * For PIPE_BUFFER resources, a range which may contain valid data.
    *
    * This is a conservative estimate of what part of the buffer contains
    * valid data that we have to preserve.  The rest of the buffer is
    * considered invalid, and we can promote writes to that region to
    * be unsynchronized writes, avoiding blit copies.
    */
   struct util_range valid_buffer_range;

   /**
    * Auxiliary buffer information (CCS, MCS, or HiZ).
    */
   struct {
      /** The surface layout for the auxiliary buffer. */
      struct isl_surf surf;

      /** The buffer object containing the auxiliary data. */
      struct crocus_bo *bo;

      /** Offset into 'bo' where the auxiliary surface starts. */
      uint32_t offset;

      struct {
         struct isl_surf surf;

         /** Offset into 'bo' where the auxiliary surface starts. */
         uint32_t offset;
      } extra_aux;

      /**
       * Fast clear color for this surface.  For depth surfaces, the clear
       * value is stored as a float32 in the red component.
       */
      union isl_color_value clear_color;

      /** Buffer object containing the indirect clear color.  */
      struct crocus_bo *clear_color_bo;

      /** Offset into bo where the clear color can be found.  */
      uint64_t clear_color_offset;

      /**
       * \brief The type of auxiliary compression used by this resource.
       *
       * This describes the type of auxiliary compression that is intended to
       * be used by this resource.  An aux usage of ISL_AUX_USAGE_NONE means
       * that auxiliary compression is permanently disabled.  An aux usage
       * other than ISL_AUX_USAGE_NONE does not imply that auxiliary
       * compression will always be enabled for this surface.
       */
      enum isl_aux_usage usage;

      /**
       * A bitfield of ISL_AUX_* modes that might this resource might use.
       *
       * For example, a surface might use both CCS_E and CCS_D at times.
       */
      unsigned possible_usages;

      /**
       * Same as possible_usages, but only with modes supported for sampling.
       */
      unsigned sampler_usages;

      /**
       * \brief Maps miptree slices to their current aux state.
       *
       * This two-dimensional array is indexed as [level][layer] and stores an
       * aux state for each slice.
       */
      enum isl_aux_state **state;

      /**
       * If (1 << level) is set, HiZ is enabled for that miplevel.
       */
      uint16_t has_hiz;
   } aux;

   /**
    * For external surfaces, this is DRM format modifier that was used to
    * create or import the surface.  For internal surfaces, this will always
    * be DRM_FORMAT_MOD_INVALID.
    */
   const struct isl_drm_modifier_info *mod_info;
};

/**
 * A simple <resource, offset> tuple for storing a reference to a
 * piece of state stored in a GPU buffer object.
 */
struct crocus_state_ref {
   struct pipe_resource *res;
   uint32_t offset;
};

/**
 * The SURFACE_STATE descriptors for a resource.
 */
struct crocus_surface_state {
   /**
    * CPU-side copy of the packed SURFACE_STATE structures, already
    * aligned so they can be uploaded as a contiguous pile of bytes.
    *
    * This can be updated and re-uploaded if (e.g.) addresses need to change.
    */
   uint32_t *cpu;

   /**
    * How many states are there?  (Each aux mode has its own state.)
    */
   unsigned num_states;

   /**
    * Address of the resource (res->bo->gtt_offset).  Note that "Surface
    * Base Address" may be offset from this value.
    */
   uint64_t bo_address;

   /** A reference to the GPU buffer holding our uploaded SURFACE_STATE */
   struct crocus_state_ref ref;
};

/**
 * Gallium CSO for sampler views (texture views).
 *
 * In addition to the normal pipe_resource, this adds an ISL view
 * which may reinterpret the format or restrict levels/layers.
 *
 * These can also be linear texture buffers.
 */
struct crocus_sampler_view {
   struct pipe_sampler_view base;
   struct isl_view view;

   union isl_color_value clear_color;

   /* A short-cut (not a reference) to the actual resource being viewed.
    * Multi-planar (or depth+stencil) images may have multiple resources
    * chained together; this skips having to traverse base->texture->*.
    */
   struct crocus_resource *res;

   /** The resource (BO) holding our SURFACE_STATE. */
   struct crocus_surface_state surface_state;
};

/**
 * Image view representation.
 */
struct crocus_image_view {
   struct pipe_image_view base;

   /** The resource (BO) holding our SURFACE_STATE. */
   struct crocus_surface_state surface_state;
};

/**
 * Gallium CSO for surfaces (framebuffer attachments).
 *
 * A view of a surface that can be bound to a color render target or
 * depth/stencil attachment.
 */
struct crocus_surface {
   struct pipe_surface base;
   struct isl_view view;
   struct isl_view read_view;
   union isl_color_value clear_color;

   /** The resource (BO) holding our SURFACE_STATE. */
   struct crocus_surface_state surface_state;
   /** The resource (BO) holding our SURFACE_STATE for read. */
   struct crocus_surface_state surface_state_read;
};

/**
 * Transfer object - information about a buffer mapping.
 */
struct crocus_transfer {
   struct pipe_transfer base;
   struct pipe_debug_callback *dbg;
   void *buffer;
   void *ptr;

   /** A linear staging resource for GPU-based copy_region transfers. */
   struct pipe_resource *staging;
   struct blorp_context *blorp;
   struct crocus_batch *batch;

   bool dest_had_defined_contents;

   void (*unmap)(struct crocus_transfer *);
};

/**
 * Unwrap a pipe_resource to get the underlying crocus_bo (for convenience).
 */
static inline struct crocus_bo *
crocus_resource_bo(struct pipe_resource *p_res)
{
   struct crocus_resource *res = (void *) p_res;
   return res->bo;
}

struct crocus_format_info crocus_format_for_usage(const struct gen_device_info *,
                                              enum pipe_format pf,
                                              isl_surf_usage_flags_t usage);

struct pipe_resource *crocus_resource_get_separate_stencil(struct pipe_resource *);

void crocus_get_depth_stencil_resources(struct pipe_resource *res,
                                      struct crocus_resource **out_z,
                                      struct crocus_resource **out_s);
bool crocus_resource_set_clear_color(struct crocus_context *ice,
                                   struct crocus_resource *res,
                                   union isl_color_value color);
union isl_color_value
crocus_resource_get_clear_color(const struct crocus_resource *res,
                              struct crocus_bo **clear_color_bo,
                              uint64_t *clear_color_offset);

void crocus_init_screen_resource_functions(struct pipe_screen *pscreen);

void crocus_dirty_for_history(struct crocus_context *ice,
                            struct crocus_resource *res);
uint32_t crocus_flush_bits_for_history(struct crocus_resource *res);

void crocus_flush_and_dirty_for_history(struct crocus_context *ice,
                                      struct crocus_batch *batch,
                                      struct crocus_resource *res,
                                      uint32_t extra_flags,
                                      const char *reason);

unsigned crocus_get_num_logical_layers(const struct crocus_resource *res,
                                     unsigned level);

void crocus_resource_disable_aux(struct crocus_resource *res);

#define INTEL_REMAINING_LAYERS UINT32_MAX
#define INTEL_REMAINING_LEVELS UINT32_MAX

void
crocus_hiz_exec(struct crocus_context *ice,
              struct crocus_batch *batch,
              struct crocus_resource *res,
              unsigned int level, unsigned int start_layer,
              unsigned int num_layers, enum isl_aux_op op,
              bool update_clear_depth);

/**
 * Prepare a miptree for access
 *
 * This function should be called prior to any access to miptree in order to
 * perform any needed resolves.
 *
 * \param[in]  start_level    The first mip level to be accessed
 *
 * \param[in]  num_levels     The number of miplevels to be accessed or
 *                            INTEL_REMAINING_LEVELS to indicate every level
 *                            above start_level will be accessed
 *
 * \param[in]  start_layer    The first array slice or 3D layer to be accessed
 *
 * \param[in]  num_layers     The number of array slices or 3D layers be
 *                            accessed or INTEL_REMAINING_LAYERS to indicate
 *                            every layer above start_layer will be accessed
 *
 * \param[in]  aux_supported  Whether or not the access will support the
 *                            miptree's auxiliary compression format;  this
 *                            must be false for uncompressed miptrees
 *
 * \param[in]  fast_clear_supported Whether or not the access will support
 *                                  fast clears in the miptree's auxiliary
 *                                  compression format
 */
void
crocus_resource_prepare_access(struct crocus_context *ice,
                             struct crocus_batch *batch,
                             struct crocus_resource *res,
                             uint32_t start_level, uint32_t num_levels,
                             uint32_t start_layer, uint32_t num_layers,
                             enum isl_aux_usage aux_usage,
                             bool fast_clear_supported);

/**
 * Complete a write operation
 *
 * This function should be called after any operation writes to a miptree.
 * This will update the miptree's compression state so that future resolves
 * happen correctly.  Technically, this function can be called before the
 * write occurs but the caller must ensure that they don't interlace
 * crocus_resource_prepare_access and crocus_resource_finish_write calls to
 * overlapping layer/level ranges.
 *
 * \param[in]  level             The mip level that was written
 *
 * \param[in]  start_layer       The first array slice or 3D layer written
 *
 * \param[in]  num_layers        The number of array slices or 3D layers
 *                               written or INTEL_REMAINING_LAYERS to indicate
 *                               every layer above start_layer was written
 *
 * \param[in]  written_with_aux  Whether or not the write was done with
 *                               auxiliary compression enabled
 */
void
crocus_resource_finish_write(struct crocus_context *ice,
                           struct crocus_resource *res, uint32_t level,
                           uint32_t start_layer, uint32_t num_layers,
                           enum isl_aux_usage aux_usage);

/** Get the auxiliary compression state of a miptree slice */
enum isl_aux_state
crocus_resource_get_aux_state(const struct crocus_resource *res,
                            uint32_t level, uint32_t layer);

/**
 * Set the auxiliary compression state of a miptree slice range
 *
 * This function directly sets the auxiliary compression state of a slice
 * range of a miptree.  It only modifies data structures and does not do any
 * resolves.  This should only be called by code which directly performs
 * compression operations such as fast clears and resolves.  Most code should
 * use crocus_resource_prepare_access or crocus_resource_finish_write.
 */
void
crocus_resource_set_aux_state(struct crocus_context *ice,
                            struct crocus_resource *res, uint32_t level,
                            uint32_t start_layer, uint32_t num_layers,
                            enum isl_aux_state aux_state);

/**
 * Prepare a miptree for raw access
 *
 * This helper prepares the miptree for access that knows nothing about any
 * sort of compression whatsoever.  This is useful when mapping the surface or
 * using it with the blitter.
 */
static inline void
crocus_resource_access_raw(struct crocus_context *ice,
                         struct crocus_batch *batch,
                         struct crocus_resource *res,
                         uint32_t level, uint32_t layer,
                         uint32_t num_layers,
                         bool write)
{
   crocus_resource_prepare_access(ice, batch, res, level, 1, layer, num_layers,
                                ISL_AUX_USAGE_NONE, false);
   if (write) {
      crocus_resource_finish_write(ice, res, level, layer, num_layers,
                                 ISL_AUX_USAGE_NONE);
   }
}

enum isl_dim_layout crocus_get_isl_dim_layout(const struct gen_device_info *devinfo,
                                            enum isl_tiling tiling,
                                            enum pipe_texture_target target);
enum isl_surf_dim target_to_isl_surf_dim(enum pipe_texture_target target);
void
crocus_resource_get_tile_dims(enum isl_tiling tiling, uint32_t cpp,
			      uint32_t *tile_w, uint32_t *tile_h);
void
crocus_resource_get_image_offset(struct crocus_resource *res,
				 uint32_t level, uint32_t z,
				 uint32_t *x, uint32_t *y);
uint32_t crocus_resource_get_tile_offsets(const struct crocus_resource *res,
                                        uint32_t level, uint32_t z,
                                        uint32_t *tile_x, uint32_t *tile_y);
enum isl_aux_usage crocus_resource_texture_aux_usage(struct crocus_context *ice,
                                                   const struct crocus_resource *res,
                                                   enum isl_format view_fmt);
void crocus_resource_prepare_texture(struct crocus_context *ice,
                                   struct crocus_batch *batch,
                                   struct crocus_resource *res,
                                   enum isl_format view_format,
                                   uint32_t start_level, uint32_t num_levels,
                                   uint32_t start_layer, uint32_t num_layers);

static inline bool
crocus_resource_unfinished_aux_import(struct crocus_resource *res)
{
   return res->base.next != NULL && res->mod_info &&
      res->mod_info->aux_usage != ISL_AUX_USAGE_NONE;
}

void crocus_resource_finish_aux_import(struct pipe_screen *pscreen,
                                     struct crocus_resource *res);

bool crocus_has_color_unresolved(const struct crocus_resource *res,
                               unsigned start_level, unsigned num_levels,
                               unsigned start_layer, unsigned num_layers);

void crocus_resource_check_level_layer(const struct crocus_resource *res,
                                     uint32_t level, uint32_t layer);

bool crocus_resource_level_has_hiz(const struct crocus_resource *res,
                                 uint32_t level);
bool crocus_has_color_unresolved(const struct crocus_resource *res,
                               unsigned start_level, unsigned num_levels,
                               unsigned start_layer, unsigned num_layers);

enum isl_aux_usage crocus_resource_render_aux_usage(struct crocus_context *ice,
                                                  struct crocus_resource *res,
                                                  enum isl_format render_fmt,
                                                  bool blend_enabled,
                                                  bool draw_aux_disabled);
void crocus_resource_prepare_render(struct crocus_context *ice,
                                  struct crocus_batch *batch,
                                  struct crocus_resource *res, uint32_t level,
                                  uint32_t start_layer, uint32_t layer_count,
                                  enum isl_aux_usage aux_usage);
void crocus_resource_finish_render(struct crocus_context *ice,
                                 struct crocus_resource *res, uint32_t level,
                                 uint32_t start_layer, uint32_t layer_count,
                                 enum isl_aux_usage aux_usage);
void crocus_resource_prepare_depth(struct crocus_context *ice,
                                 struct crocus_batch *batch,
                                 struct crocus_resource *res, uint32_t level,
                                 uint32_t start_layer, uint32_t layer_count);
void crocus_resource_finish_depth(struct crocus_context *ice,
                                struct crocus_resource *res, uint32_t level,
                                uint32_t start_layer, uint32_t layer_count,
                                bool depth_written);
#endif
