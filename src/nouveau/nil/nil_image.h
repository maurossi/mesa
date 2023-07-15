#ifndef NIL_H
#define NIL_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "util/macros.h"
#include "util/format/u_format.h"

struct nouveau_ws_device;

enum PACKED nil_image_dim {
   NIL_IMAGE_DIM_1D = 1,
   NIL_IMAGE_DIM_2D = 2,
   NIL_IMAGE_DIM_3D = 3,
};

enum nil_image_usage_flags {
   NIL_IMAGE_USAGE_RENDER_TARGET_BIT   = BITFIELD_BIT(0),
   NIL_IMAGE_USAGE_DEPTH_BIT           = BITFIELD_BIT(1),
   NIL_IMAGE_USAGE_STENCIL_BIT         = BITFIELD_BIT(2),
   NIL_IMAGE_USAGE_TEXTURE_BIT         = BITFIELD_BIT(3),
   NIL_IMAGE_USAGE_STORAGE_BIT         = BITFIELD_BIT(4),
   NIL_IMAGE_USAGE_CUBE_BIT            = BITFIELD_BIT(5),
   NIL_IMAGE_USAGE_2D_VIEW_BIT         = BITFIELD_BIT(6),
};

struct nil_extent4d {
   union { uint32_t w, width; };
   union { uint32_t h, height; };
   union { uint32_t d, depth; };
   union { uint32_t a, array_len; };
};

static inline struct nil_extent4d
nil_extent4d(uint32_t w, uint32_t h, uint32_t d, uint32_t a)
{
   struct nil_extent4d e;
   e.w = w;
   e.h = h;
   e.d = d;
   e.a = a;
   return e;
}

#define NIL_GOB_WIDTH_B 64
#define NIL_GOB_HEIGHT(gob_height_8) ((gob_height_8) ? 8 : 4)
#define NIL_GOB_DEPTH 1
#define NIL_MAX_LEVELS 16

struct nil_tiling {
   bool is_tiled:1;
   bool gob_height_8:1; /**< GOB height is 4 or 8 */
   uint8_t y_log2:3; /**< log2 of the Y tile dimension in GOBs */
   uint8_t z_log2:3; /**< log2 of the Z tile dimension in GOBs */
};

struct nil_image_init_info {
   enum nil_image_dim dim;
   enum pipe_format format;

   struct nil_extent4d extent_px;
   uint32_t levels;
   uint32_t samples;

   enum nil_image_usage_flags usage;
};

/** Represents the data layout of a single slice (level+lod) of an image */
struct nil_image_level {
   /** Offset into the image of this level in bytes */
   uint64_t offset_B;

   /** Tiling for this level */
   struct nil_tiling tiling;

   /** Stride between rows in bytes */
   uint32_t row_stride_B;
};

struct nil_image {
   enum nil_image_dim dim;
   enum pipe_format format;

   struct nil_extent4d extent_px;
   uint8_t num_levels;
   uint8_t mip_tail_start;
   uint8_t num_samples;

   struct nil_image_level levels[NIL_MAX_LEVELS];

   uint32_t array_stride_B;

   uint64_t size_B;
};

struct nil_view {
   enum pipe_texture_target target;

   /**
    * The format to use in the view
    *
    * This may differ from the format of the actual isl_surf but must have
    * the same block size.
    */
   enum pipe_format format;

   uint32_t base_level;
   uint32_t num_levels;

   /**
    * Base array layer
    *
    * For cube maps, both base_array_layer and array_len should be
    * specified in terms of 2-D layers and must be a multiple of 6.
    */
   uint32_t base_array_layer;

   /**
    * Array Length
    *
    * Indicates the number of array elements starting at  Base Array Layer.
    */
   uint32_t array_len;

   enum pipe_swizzle swizzle[4];
};

bool nil_image_init(struct nouveau_ws_device *dev,
                    struct nil_image *image,
                    const struct nil_image_init_info *restrict info);

static inline uint64_t
nil_image_level_layer_offset_B(const struct nil_image *image,
                               uint32_t level, uint32_t layer)
{
   assert(level < image->num_levels);
   assert(layer < image->extent_px.array_len);
   return image->levels[level].offset_B + (layer * image->array_stride_B);
}


void nil_image_fill_tic(struct nouveau_ws_device *dev,
                        const struct nil_image *image,
                        const struct nil_view *view,
                        uint64_t base_address,
                        void *desc_out);

#endif /* NIL_H */
