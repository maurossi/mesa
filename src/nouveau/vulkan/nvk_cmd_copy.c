#include "nvk_cmd_buffer.h"

#include "vulkan/util/vk_format.h"

#include "nvk_buffer.h"
#include "nvk_device.h"
#include "nvk_device_memory.h"
#include "nvk_format.h"
#include "nvk_image.h"
#include "nvk_physical_device.h"

#include "nouveau_bo.h"
#include "nouveau_context.h"

#include "nvtypes.h"
#include "nvk_cl902d.h"
#include "nvk_cl90b5.h"
#include "nvk_clc1b5.h"

struct nouveau_copy_buffer {
   uint64_t base_addr;
   VkImageType image_type;
   struct nil_offset4d offset_el;
   struct nil_extent4d extent_el;
   uint32_t bpp;
   uint32_t row_stride;
   uint32_t array_stride;
   struct nil_tiling tiling;
};

struct nouveau_copy {
   struct nouveau_copy_buffer src;
   struct nouveau_copy_buffer dst;
   struct {
      uint8_t comp_size;
      uint8_t dst[4];
   } remap;
   struct nil_extent4d extent_el;
};

static struct nouveau_copy_buffer
nouveau_copy_rect_buffer(
   struct nvk_buffer *buf,
   VkDeviceSize offset,
   struct vk_image_buffer_layout buffer_layout)
{
   return (struct nouveau_copy_buffer) {
      .base_addr = nvk_buffer_address(buf, offset),
      .image_type = VK_IMAGE_TYPE_2D,
      .bpp = buffer_layout.element_size_B,
      .row_stride = buffer_layout.row_stride_B,
      .array_stride = buffer_layout.image_stride_B,
   };
}

static struct nil_offset4d
vk_to_nil_offset(VkOffset3D offset, uint32_t base_array_layer)
{
   return nil_offset4d(offset.x, offset.y, offset.z, base_array_layer);
}

static struct nil_extent4d
vk_to_nil_extent(VkExtent3D extent, uint32_t array_layers)
{
   return nil_extent4d(extent.width, extent.height, extent.depth, array_layers);
}

static struct nouveau_copy_buffer
nouveau_copy_rect_image(
   struct nvk_image *img,
   VkOffset3D offset_px,
   const VkImageSubresourceLayers *sub_res)
{
   const VkExtent3D lvl_extent_px =
      vk_image_mip_level_extent(&img->vk, sub_res->mipLevel);
   const struct nil_extent4d lvl_extent4d_px =
      vk_to_nil_extent(lvl_extent_px, img->vk.array_layers);

   offset_px = vk_image_sanitize_offset(&img->vk, offset_px);
   const struct nil_offset4d offset4d_px =
      vk_to_nil_offset(offset_px, sub_res->baseArrayLayer);

   struct nouveau_copy_buffer buf = {
      .base_addr = nvk_image_base_address(img) +
                   img->nil.levels[sub_res->mipLevel].offset_B,
      .image_type = img->vk.image_type,
      .offset_el = nil_offset4d_px_to_el(offset4d_px, img->nil.format,
                                         img->nil.sample_layout),
      .extent_el = nil_extent4d_px_to_el(lvl_extent4d_px, img->nil.format,
                                         img->nil.sample_layout),
      .bpp = vk_format_get_blocksize(img->vk.format),
      .row_stride = img->nil.levels[sub_res->mipLevel].row_stride_B,
      .array_stride = img->nil.array_stride_B,
      .tiling = img->nil.levels[sub_res->mipLevel].tiling,
   };

   return buf;
}

static uint32_t
to_90b5_remap_comp_size(uint8_t comp_size)
{
   static const uint8_t to_90b5[] = {
      [1] = NV90B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_ONE,
      [2] = NV90B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_TWO,
      [3] = NV90B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_THREE,
      [4] = NV90B5_SET_REMAP_COMPONENTS_COMPONENT_SIZE_FOUR,
   };
   assert(comp_size > 0 && comp_size < ARRAY_SIZE(to_90b5));

   uint32_t size_90b5 = comp_size - 1;
   assert(size_90b5 == to_90b5[comp_size]);
   return size_90b5;
}

static uint32_t
to_90b5_remap_num_comps(uint8_t num_comps)
{
   static const uint8_t to_90b5[] = {
      [1] = NV90B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_ONE,
      [2] = NV90B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_TWO,
      [3] = NV90B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_THREE,
      [4] = NV90B5_SET_REMAP_COMPONENTS_NUM_SRC_COMPONENTS_FOUR,
   };
   assert(num_comps > 0 && num_comps < ARRAY_SIZE(to_90b5));

   uint32_t num_comps_90b5 = num_comps - 1;
   assert(num_comps_90b5 == to_90b5[num_comps]);
   return num_comps_90b5;
}

static void
nouveau_copy_rect(struct nvk_cmd_buffer *cmd, struct nouveau_copy *copy)
{
   uint32_t src_bw, dst_bw;
   if (copy->remap.comp_size > 0) {
      struct nv_push *p = nvk_cmd_buffer_push(cmd, 2);

      assert(copy->src.bpp % copy->remap.comp_size == 0);
      assert(copy->dst.bpp % copy->remap.comp_size == 0);
      uint32_t num_src_comps = copy->src.bpp / copy->remap.comp_size;
      uint32_t num_dst_comps = copy->dst.bpp / copy->remap.comp_size;

      /* When running with component remapping enabled, most X/Y dimensions
       * are in units of blocks.
       */
      src_bw = dst_bw = 1;

      P_IMMD(p, NV90B5, SET_REMAP_COMPONENTS, {
         .dst_x = copy->remap.dst[0],
         .dst_y = copy->remap.dst[1],
         .dst_z = copy->remap.dst[2],
         .dst_w = copy->remap.dst[3],
         .component_size = to_90b5_remap_comp_size(copy->remap.comp_size),
         .num_src_components = to_90b5_remap_comp_size(num_src_comps),
         .num_dst_components = to_90b5_remap_comp_size(num_dst_comps),
      });
   } else {
      /* When component remapping is disabled, dimensions are in units of
       * bytes (an implicit block widht of 1B).
       */
      assert(copy->src.bpp == copy->dst.bpp);
      src_bw = copy->src.bpp;
      dst_bw = copy->dst.bpp;
   }

   assert(copy->extent_el.depth == 1 || copy->extent_el.array_len == 1);
   for (unsigned z = 0; z < MAX2(copy->extent_el.d, copy->extent_el.a); z++) {
      VkDeviceSize src_addr = copy->src.base_addr;
      VkDeviceSize dst_addr = copy->dst.base_addr;

      if (copy->src.image_type != VK_IMAGE_TYPE_3D)
         src_addr += (z + copy->src.offset_el.a) * copy->src.array_stride;

      if (copy->dst.image_type != VK_IMAGE_TYPE_3D)
         dst_addr += (z + copy->dst.offset_el.a) * copy->dst.array_stride;

      if (!copy->src.tiling.is_tiled) {
         src_addr += copy->src.offset_el.x * copy->src.bpp +
                     copy->src.offset_el.y * copy->src.row_stride;
      }

      if (!copy->dst.tiling.is_tiled) {
         dst_addr += copy->dst.offset_el.x * copy->dst.bpp +
                     copy->dst.offset_el.y * copy->dst.row_stride;
      }

      struct nv_push *p = nvk_cmd_buffer_push(cmd, 31);

      P_MTHD(p, NV90B5, OFFSET_IN_UPPER);
      P_NV90B5_OFFSET_IN_UPPER(p, src_addr >> 32);
      P_NV90B5_OFFSET_IN_LOWER(p, src_addr & 0xffffffff);
      P_NV90B5_OFFSET_OUT_UPPER(p, dst_addr >> 32);
      P_NV90B5_OFFSET_OUT_LOWER(p, dst_addr & 0xffffffff);
      P_NV90B5_PITCH_IN(p, copy->src.row_stride);
      P_NV90B5_PITCH_OUT(p, copy->dst.row_stride);
      P_NV90B5_LINE_LENGTH_IN(p, copy->extent_el.width * src_bw);
      P_NV90B5_LINE_COUNT(p, copy->extent_el.height);

      uint32_t src_layout = 0, dst_layout = 0;
      if (copy->src.tiling.is_tiled) {
         P_MTHD(p, NV90B5, SET_SRC_BLOCK_SIZE);
         P_NV90B5_SET_SRC_BLOCK_SIZE(p, {
            .width = 0, /* Tiles are always 1 GOB wide */
            .height = copy->src.tiling.y_log2,
            .depth = copy->src.tiling.z_log2,
            .gob_height = copy->src.tiling.gob_height_8 ?
                          GOB_HEIGHT_GOB_HEIGHT_FERMI_8 :
                          GOB_HEIGHT_GOB_HEIGHT_TESLA_4,
         });
         P_NV90B5_SET_SRC_WIDTH(p, copy->src.extent_el.width * src_bw);
         P_NV90B5_SET_SRC_HEIGHT(p, copy->src.extent_el.height);
         P_NV90B5_SET_SRC_DEPTH(p, copy->src.extent_el.depth);
         if (copy->src.image_type == VK_IMAGE_TYPE_3D)
            P_NV90B5_SET_SRC_LAYER(p, z + copy->src.offset_el.z);
         else
            P_NV90B5_SET_SRC_LAYER(p, 0);

         if (nvk_cmd_buffer_device(cmd)->ctx->copy.cls >= 0xc1b5) {
            P_MTHD(p, NVC1B5, SRC_ORIGIN_X);
            P_NVC1B5_SRC_ORIGIN_X(p, copy->src.offset_el.x * src_bw);
            P_NVC1B5_SRC_ORIGIN_Y(p, copy->src.offset_el.y);
         } else {
            P_MTHD(p, NV90B5, SET_SRC_ORIGIN);
            P_NV90B5_SET_SRC_ORIGIN(p, {
               .x = copy->src.offset_el.x * src_bw,
               .y = copy->src.offset_el.y
            });
         }

         src_layout = NV90B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT_BLOCKLINEAR;
      } else {
         src_addr += copy->src.array_stride;
         src_layout = NV90B5_LAUNCH_DMA_SRC_MEMORY_LAYOUT_PITCH;
      }

      if (copy->dst.tiling.is_tiled) {
         P_MTHD(p, NV90B5, SET_DST_BLOCK_SIZE);
         P_NV90B5_SET_DST_BLOCK_SIZE(p, {
            .width = 0, /* Tiles are always 1 GOB wide */
            .height = copy->dst.tiling.y_log2,
            .depth = copy->dst.tiling.z_log2,
            .gob_height = copy->dst.tiling.gob_height_8 ?
                          GOB_HEIGHT_GOB_HEIGHT_FERMI_8 :
                          GOB_HEIGHT_GOB_HEIGHT_TESLA_4,
         });
         P_NV90B5_SET_DST_WIDTH(p, copy->dst.extent_el.width * dst_bw);
         P_NV90B5_SET_DST_HEIGHT(p, copy->dst.extent_el.height);
         P_NV90B5_SET_DST_DEPTH(p, copy->dst.extent_el.depth);
         if (copy->dst.image_type == VK_IMAGE_TYPE_3D)
            P_NV90B5_SET_DST_LAYER(p, z + copy->dst.offset_el.z);
         else
            P_NV90B5_SET_DST_LAYER(p, 0);

         if (nvk_cmd_buffer_device(cmd)->ctx->copy.cls >= 0xc1b5) {
            P_MTHD(p, NVC1B5, DST_ORIGIN_X);
            P_NVC1B5_DST_ORIGIN_X(p, copy->dst.offset_el.x * dst_bw);
            P_NVC1B5_DST_ORIGIN_Y(p, copy->dst.offset_el.y);
         } else {
            P_MTHD(p, NV90B5, SET_DST_ORIGIN);
            P_NV90B5_SET_DST_ORIGIN(p, {
               .x = copy->dst.offset_el.x * dst_bw,
               .y = copy->dst.offset_el.y
            });
         }

         dst_layout = NV90B5_LAUNCH_DMA_DST_MEMORY_LAYOUT_BLOCKLINEAR;
      } else {
         dst_addr += copy->dst.array_stride;
         dst_layout = NV90B5_LAUNCH_DMA_DST_MEMORY_LAYOUT_PITCH;
      }

      P_IMMD(p, NV90B5, LAUNCH_DMA, {
         .data_transfer_type = DATA_TRANSFER_TYPE_NON_PIPELINED,
         .multi_line_enable = MULTI_LINE_ENABLE_TRUE,
         .flush_enable = FLUSH_ENABLE_TRUE,
         .src_memory_layout = src_layout,
         .dst_memory_layout = dst_layout,
         .remap_enable = copy->remap.comp_size > 0,
      });
   }
}

VKAPI_ATTR void VKAPI_CALL
nvk_CmdCopyBuffer2(VkCommandBuffer commandBuffer,
                   const VkCopyBufferInfo2 *pCopyBufferInfo)
{
   VK_FROM_HANDLE(nvk_cmd_buffer, cmd, commandBuffer);
   VK_FROM_HANDLE(nvk_buffer, src, pCopyBufferInfo->srcBuffer);
   VK_FROM_HANDLE(nvk_buffer, dst, pCopyBufferInfo->dstBuffer);

   for (unsigned r = 0; r < pCopyBufferInfo->regionCount; r++) {
      const VkBufferCopy2 *region = &pCopyBufferInfo->pRegions[r];

      uint64_t src_addr = nvk_buffer_address(src, region->srcOffset);
      uint64_t dst_addr = nvk_buffer_address(dst, region->dstOffset);
      uint64_t size = region->size;

      while (size) {
         struct nv_push *p = nvk_cmd_buffer_push(cmd, 10);

         P_MTHD(p, NV90B5, OFFSET_IN_UPPER);
         P_NV90B5_OFFSET_IN_UPPER(p, src_addr >> 32);
         P_NV90B5_OFFSET_IN_LOWER(p, src_addr & 0xffffffff);
         P_NV90B5_OFFSET_OUT_UPPER(p, dst_addr >> 32);
         P_NV90B5_OFFSET_OUT_LOWER(p, dst_addr & 0xffffffff);

         unsigned bytes = MIN2(size, 1 << 17);

         P_MTHD(p, NV90B5, LINE_LENGTH_IN);
         P_NV90B5_LINE_LENGTH_IN(p, bytes);
         P_NV90B5_LINE_COUNT(p, 1);

         P_IMMD(p, NV90B5, LAUNCH_DMA, {
                .data_transfer_type = DATA_TRANSFER_TYPE_NON_PIPELINED,
                .multi_line_enable = MULTI_LINE_ENABLE_TRUE,
                .flush_enable = FLUSH_ENABLE_TRUE,
                .src_memory_layout = SRC_MEMORY_LAYOUT_PITCH,
                .dst_memory_layout = DST_MEMORY_LAYOUT_PITCH,
         });

         src_addr += bytes;
         dst_addr += bytes;
         size -= bytes;
      }
   }
}

VKAPI_ATTR void VKAPI_CALL
nvk_CmdCopyBufferToImage2(VkCommandBuffer commandBuffer,
                          const VkCopyBufferToImageInfo2 *pCopyBufferToImageInfo)
{
   VK_FROM_HANDLE(nvk_cmd_buffer, cmd, commandBuffer);
   VK_FROM_HANDLE(nvk_buffer, src, pCopyBufferToImageInfo->srcBuffer);
   VK_FROM_HANDLE(nvk_image, dst, pCopyBufferToImageInfo->dstImage);

   for (unsigned r = 0; r < pCopyBufferToImageInfo->regionCount; r++) {
      const VkBufferImageCopy2 *region = &pCopyBufferToImageInfo->pRegions[r];
      struct vk_image_buffer_layout buffer_layout =
         vk_image_buffer_copy_layout(&dst->vk, region);

      const VkExtent3D extent_px =
         vk_image_sanitize_extent(&dst->vk, region->imageExtent);
      const struct nil_extent4d extent4d_px =
         vk_to_nil_extent(extent_px, region->imageSubresource.layerCount);

      struct nouveau_copy copy = {
         .src = nouveau_copy_rect_buffer(src, region->bufferOffset,
                                         buffer_layout),
         .dst = nouveau_copy_rect_image(dst, region->imageOffset,
                                        &region->imageSubresource),
         .extent_el = nil_extent4d_px_to_el(extent4d_px, dst->nil.format,
                                            dst->nil.sample_layout),
      };

      const VkImageAspectFlagBits aspects = region->imageSubresource.aspectMask;
      switch (dst->vk.format) {
      case VK_FORMAT_D24_UNORM_S8_UINT:
         if (aspects == VK_IMAGE_ASPECT_DEPTH_BIT) {
            copy.remap.comp_size = 1;
            copy.remap.dst[0] = NV90B5_SET_REMAP_COMPONENTS_DST_X_SRC_X;
            copy.remap.dst[1] = NV90B5_SET_REMAP_COMPONENTS_DST_Y_SRC_Y;
            copy.remap.dst[2] = NV90B5_SET_REMAP_COMPONENTS_DST_Z_SRC_Z;
            copy.remap.dst[3] = NV90B5_SET_REMAP_COMPONENTS_DST_W_NO_WRITE;
         } else {
            assert(aspects == VK_IMAGE_ASPECT_STENCIL_BIT);
            copy.remap.comp_size = 1;
            copy.remap.dst[0] = NV90B5_SET_REMAP_COMPONENTS_DST_X_NO_WRITE;
            copy.remap.dst[1] = NV90B5_SET_REMAP_COMPONENTS_DST_Y_NO_WRITE;
            copy.remap.dst[2] = NV90B5_SET_REMAP_COMPONENTS_DST_Z_NO_WRITE;
            copy.remap.dst[3] = NV90B5_SET_REMAP_COMPONENTS_DST_W_SRC_X;
         }
         break;
      default:
         break;
      }

      nouveau_copy_rect(cmd, &copy);

      vk_foreach_struct_const(ext, region->pNext) {
         switch (ext->sType) {
         default:
            nvk_debug_ignored_stype(ext->sType);
            break;
         }
      }
   }

   vk_foreach_struct_const(ext, pCopyBufferToImageInfo->pNext) {
      switch (ext->sType) {
      default:
         nvk_debug_ignored_stype(ext->sType);
         break;
      }
   }
}

VKAPI_ATTR void VKAPI_CALL
nvk_CmdCopyImageToBuffer2(VkCommandBuffer commandBuffer,
                          const VkCopyImageToBufferInfo2 *pCopyImageToBufferInfo)
{
   VK_FROM_HANDLE(nvk_cmd_buffer, cmd, commandBuffer);
   VK_FROM_HANDLE(nvk_image, src, pCopyImageToBufferInfo->srcImage);
   VK_FROM_HANDLE(nvk_buffer, dst, pCopyImageToBufferInfo->dstBuffer);

   for (unsigned r = 0; r < pCopyImageToBufferInfo->regionCount; r++) {
      const VkBufferImageCopy2 *region = &pCopyImageToBufferInfo->pRegions[r];
      struct vk_image_buffer_layout buffer_layout =
         vk_image_buffer_copy_layout(&src->vk, region);

      const VkExtent3D extent_px =
         vk_image_sanitize_extent(&src->vk, region->imageExtent);
      const struct nil_extent4d extent4d_px =
         vk_to_nil_extent(extent_px, region->imageSubresource.layerCount);

      struct nouveau_copy copy = {
         .src = nouveau_copy_rect_image(src, region->imageOffset,
                                        &region->imageSubresource),
         .dst = nouveau_copy_rect_buffer(dst, region->bufferOffset,
                                         buffer_layout),
         .extent_el = nil_extent4d_px_to_el(extent4d_px, src->nil.format,
                                            src->nil.sample_layout),
      };

      const VkImageAspectFlagBits aspects = region->imageSubresource.aspectMask;
      switch (src->vk.format) {
      case VK_FORMAT_D24_UNORM_S8_UINT:
         if (aspects == VK_IMAGE_ASPECT_DEPTH_BIT) {
            copy.remap.comp_size = 1;
            copy.remap.dst[0] = NV90B5_SET_REMAP_COMPONENTS_DST_X_SRC_X;
            copy.remap.dst[1] = NV90B5_SET_REMAP_COMPONENTS_DST_Y_SRC_Y;
            copy.remap.dst[2] = NV90B5_SET_REMAP_COMPONENTS_DST_Z_SRC_Z;
            copy.remap.dst[3] = NV90B5_SET_REMAP_COMPONENTS_DST_W_NO_WRITE;
         } else {
            assert(aspects == VK_IMAGE_ASPECT_STENCIL_BIT);
            copy.remap.comp_size = 1;
            copy.remap.dst[0] = NV90B5_SET_REMAP_COMPONENTS_DST_X_SRC_W;
            copy.remap.dst[1] = NV90B5_SET_REMAP_COMPONENTS_DST_Y_NO_WRITE;
            copy.remap.dst[2] = NV90B5_SET_REMAP_COMPONENTS_DST_Z_NO_WRITE;
            copy.remap.dst[3] = NV90B5_SET_REMAP_COMPONENTS_DST_W_NO_WRITE;
         }
         break;
      default:
         break;
      }

      nouveau_copy_rect(cmd, &copy);

      vk_foreach_struct_const(ext, region->pNext) {
         switch (ext->sType) {
         default:
            nvk_debug_ignored_stype(ext->sType);
            break;
         }
      }
   }

   vk_foreach_struct_const(ext, pCopyImageToBufferInfo->pNext) {
      switch (ext->sType) {
      default:
         nvk_debug_ignored_stype(ext->sType);
         break;
      }
   }
}

VKAPI_ATTR void VKAPI_CALL
nvk_CmdCopyImage2(VkCommandBuffer commandBuffer,
                  const VkCopyImageInfo2 *pCopyImageInfo)
{
   VK_FROM_HANDLE(nvk_cmd_buffer, cmd, commandBuffer);
   VK_FROM_HANDLE(nvk_image, src, pCopyImageInfo->srcImage);
   VK_FROM_HANDLE(nvk_image, dst, pCopyImageInfo->dstImage);

   for (unsigned r = 0; r < pCopyImageInfo->regionCount; r++) {
      const VkImageCopy2 *region = &pCopyImageInfo->pRegions[r];

      /* From the Vulkan 1.3.217 spec:
       *
       *    "When copying between compressed and uncompressed formats the
       *    extent members represent the texel dimensions of the source image
       *    and not the destination."
       */
      const VkExtent3D extent_px =
         vk_image_sanitize_extent(&src->vk, region->extent);
      const struct nil_extent4d extent4d_px =
         vk_to_nil_extent(extent_px, region->srcSubresource.layerCount);

      struct nouveau_copy copy = {
         .src = nouveau_copy_rect_image(src, region->srcOffset,
                                        &region->srcSubresource),
         .dst = nouveau_copy_rect_image(dst, region->dstOffset,
                                        &region->dstSubresource),
         .extent_el = nil_extent4d_px_to_el(extent4d_px, src->nil.format,
                                            src->nil.sample_layout),
      };

      const VkImageAspectFlagBits aspects = region->srcSubresource.aspectMask;
      assert(aspects == region->dstSubresource.aspectMask);
      switch (src->vk.format) {
      case VK_FORMAT_D24_UNORM_S8_UINT:
         if (aspects == VK_IMAGE_ASPECT_DEPTH_BIT) {
            copy.remap.comp_size = 1;
            copy.remap.dst[0] = NV90B5_SET_REMAP_COMPONENTS_DST_W_SRC_X;
            copy.remap.dst[1] = NV90B5_SET_REMAP_COMPONENTS_DST_Y_SRC_Y;
            copy.remap.dst[2] = NV90B5_SET_REMAP_COMPONENTS_DST_Z_SRC_Z;
            copy.remap.dst[3] = NV90B5_SET_REMAP_COMPONENTS_DST_W_NO_WRITE;
         } else if (aspects == VK_IMAGE_ASPECT_STENCIL_BIT) {
            copy.remap.comp_size = 1;
            copy.remap.dst[0] = NV90B5_SET_REMAP_COMPONENTS_DST_X_NO_WRITE;
            copy.remap.dst[1] = NV90B5_SET_REMAP_COMPONENTS_DST_Y_NO_WRITE;
            copy.remap.dst[2] = NV90B5_SET_REMAP_COMPONENTS_DST_Z_NO_WRITE;
            copy.remap.dst[3] = NV90B5_SET_REMAP_COMPONENTS_DST_W_SRC_W;
         } else {
            /* If we're copying both, there's nothing special to do */
            assert(aspects == (VK_IMAGE_ASPECT_DEPTH_BIT |
                               VK_IMAGE_ASPECT_STENCIL_BIT));
         }
         break;
      default:
         break;
      }

      nouveau_copy_rect(cmd, &copy);
   }
}

VKAPI_ATTR void VKAPI_CALL
nvk_CmdFillBuffer(VkCommandBuffer commandBuffer,
                  VkBuffer dstBuffer,
                  VkDeviceSize dstOffset,
                  VkDeviceSize fillSize,
                  uint32_t data)
{
   VK_FROM_HANDLE(nvk_cmd_buffer, cmd, commandBuffer);
   VK_FROM_HANDLE(nvk_buffer, dst, dstBuffer);

   fillSize = vk_buffer_range(&dst->vk, dstOffset, fillSize);

   VkDeviceSize dst_addr = nvk_buffer_address(dst, 0);
   VkDeviceSize start = dstOffset / 4;
   VkDeviceSize end = start + fillSize / 4;

   /* can't go higher for whatever reason */
   uint32_t pitch = 1 << 19;
   uint32_t line = pitch / 4;

   struct nv_push *p = nvk_cmd_buffer_push(cmd, 33);

   P_IMMD(p, NV902D, SET_OPERATION, V_SRCCOPY);

   P_MTHD(p, NV902D, SET_DST_FORMAT);
   P_NV902D_SET_DST_FORMAT(p, V_A8B8G8R8);
   P_NV902D_SET_DST_MEMORY_LAYOUT(p, V_PITCH);

   P_MTHD(p, NV902D, SET_DST_PITCH);
   P_NV902D_SET_DST_PITCH(p, pitch);

   P_MTHD(p, NV902D, SET_DST_OFFSET_UPPER);
   P_NV902D_SET_DST_OFFSET_UPPER(p, dst_addr >> 32);
   P_NV902D_SET_DST_OFFSET_LOWER(p, dst_addr & 0xffffffff);

   P_MTHD(p, NV902D, RENDER_SOLID_PRIM_MODE);
   P_NV902D_RENDER_SOLID_PRIM_MODE(p, V_LINES);
   P_NV902D_SET_RENDER_SOLID_PRIM_COLOR_FORMAT(p, V_A8B8G8R8);
   P_NV902D_SET_RENDER_SOLID_PRIM_COLOR(p, data);

   /*
    * In order to support CPU efficient fills, we'll draw up to three primitives:
    *   1. rest of the first line
    *   2. a rect filling up the space between the start and end
    *   3. begining of last line
    */

   uint32_t y_0 = start / line;
   uint32_t y_1 = end / line;

   uint32_t x_0 = start % line;
   uint32_t x_1 = end % line;

   P_MTHD(p, NV902D, RENDER_SOLID_PRIM_POINT_SET_X(0));
   P_NV902D_RENDER_SOLID_PRIM_POINT_SET_X(p, 0, x_0);
   P_NV902D_RENDER_SOLID_PRIM_POINT_Y(p, 0, y_0);
   P_NV902D_RENDER_SOLID_PRIM_POINT_SET_X(p, 1, y_0 == y_1 ? x_1 : line);
   P_NV902D_RENDER_SOLID_PRIM_POINT_Y(p, 1, y_0);

   if (y_0 + 1 < y_1) {
      P_IMMD(p, NV902D, RENDER_SOLID_PRIM_MODE, V_RECTS);

      P_MTHD(p, NV902D, RENDER_SOLID_PRIM_POINT_SET_X(0));
      P_NV902D_RENDER_SOLID_PRIM_POINT_SET_X(p, 0, 0);
      P_NV902D_RENDER_SOLID_PRIM_POINT_Y(p, 0, y_0 + 1);
      P_NV902D_RENDER_SOLID_PRIM_POINT_SET_X(p, 1, line);
      P_NV902D_RENDER_SOLID_PRIM_POINT_Y(p, 1, y_1);

      P_IMMD(p, NV902D, RENDER_SOLID_PRIM_MODE, V_LINES);
   }

   if (y_0 < y_1) {
      P_MTHD(p, NV902D, RENDER_SOLID_PRIM_POINT_SET_X(0));
      P_NV902D_RENDER_SOLID_PRIM_POINT_SET_X(p, 0, 0);
      P_NV902D_RENDER_SOLID_PRIM_POINT_Y(p, 0, y_1);
      P_NV902D_RENDER_SOLID_PRIM_POINT_SET_X(p, 1, x_1);
      P_NV902D_RENDER_SOLID_PRIM_POINT_Y(p, 1, y_1);
   }
}

VKAPI_ATTR void VKAPI_CALL
nvk_CmdUpdateBuffer(VkCommandBuffer commandBuffer,
                    VkBuffer dstBuffer,
                    VkDeviceSize dstOffset,
                    VkDeviceSize dataSize,
                    const void *pData)
{
   VK_FROM_HANDLE(nvk_cmd_buffer, cmd, commandBuffer);
   VK_FROM_HANDLE(nvk_buffer, dst, dstBuffer);

   uint64_t dst_addr = nvk_buffer_address(dst, dstOffset);

   uint64_t data_addr;
   nvk_cmd_buffer_upload_data(cmd, pData, dataSize, 64, &data_addr);

   struct nv_push *p = nvk_cmd_buffer_push(cmd, 10);

   P_MTHD(p, NV90B5, OFFSET_IN_UPPER);
   P_NV90B5_OFFSET_IN_UPPER(p, data_addr >> 32);
   P_NV90B5_OFFSET_IN_LOWER(p, data_addr & 0xffffffff);
   P_NV90B5_OFFSET_OUT_UPPER(p, dst_addr >> 32);
   P_NV90B5_OFFSET_OUT_LOWER(p, dst_addr & 0xffffffff);

   P_MTHD(p, NV90B5, LINE_LENGTH_IN);
   P_NV90B5_LINE_LENGTH_IN(p, dataSize);
   P_NV90B5_LINE_COUNT(p, 1);

   P_IMMD(p, NV90B5, LAUNCH_DMA, {
      .data_transfer_type = DATA_TRANSFER_TYPE_NON_PIPELINED,
      .multi_line_enable = MULTI_LINE_ENABLE_TRUE,
      .flush_enable = FLUSH_ENABLE_TRUE,
      .src_memory_layout = SRC_MEMORY_LAYOUT_PITCH,
      .dst_memory_layout = DST_MEMORY_LAYOUT_PITCH,
   });
}
