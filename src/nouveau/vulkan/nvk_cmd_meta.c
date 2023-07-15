#include "nvk_buffer.h"
#include "nvk_cmd_buffer.h"
#include "nvk_device.h"
#include "nvk_image.h"

static VkResult
nvk_cmd_bind_map_buffer(struct vk_command_buffer *vk_cmd,
                        struct vk_meta_device *meta,
                        VkBuffer _buffer, void **map_out)
{
   struct nvk_cmd_buffer *cmd =
      container_of(vk_cmd, struct nvk_cmd_buffer, vk);
   VK_FROM_HANDLE(nvk_buffer, buffer, _buffer);
   VkResult result;

   uint64_t addr;
   assert(buffer->vk.size < UINT_MAX);
   result = nvk_cmd_buffer_upload_alloc(cmd, buffer->vk.size, 16,
                                        &addr, map_out);
   if (unlikely(result != VK_SUCCESS))
      return result;

   buffer->addr = addr;

   return VK_SUCCESS;
}

VkResult
nvk_device_init_meta(struct nvk_device *dev)
{
   VkResult result = vk_meta_device_init(&dev->vk, &dev->meta);
   if (result != VK_SUCCESS)
      return result;

   dev->meta.cmd_bind_map_buffer = nvk_cmd_bind_map_buffer;
   dev->meta.max_bind_map_buffer_size_B = 64 * 1024; /* TODO */

   return VK_SUCCESS;
}

void
nvk_device_finish_meta(struct nvk_device *dev)
{
   vk_meta_device_finish(&dev->vk, &dev->meta);
}

struct nvk_meta_save {
   struct vk_vertex_input_state _dynamic_vi;
   struct vk_sample_locations_state _dynamic_sl;
   struct vk_dynamic_graphics_state dynamic;
   struct nvk_graphics_pipeline *pipeline;
   struct nvk_addr_range vb0;
   struct nvk_descriptor_set *desc0;
   bool has_push_desc0;
   struct nvk_push_descriptor_set push_desc0;
   uint8_t push[128];
};

static void
nvk_meta_begin(struct nvk_cmd_buffer *cmd,
               struct nvk_meta_save *save)
{
   save->dynamic = cmd->vk.dynamic_graphics_state;
   save->_dynamic_vi = cmd->state.gfx._dynamic_vi;
   save->_dynamic_sl = cmd->state.gfx._dynamic_sl;
   save->dynamic.vi = &save->_dynamic_vi;
   save->dynamic.ms.sample_locations = &save->_dynamic_sl;

   save->pipeline = cmd->state.gfx.pipeline;
   save->vb0 = cmd->state.gfx.vb0;

   save->desc0 = cmd->state.gfx.descriptors.sets[0];
   save->has_push_desc0 = cmd->state.gfx.descriptors.push[0];
   if (save->has_push_desc0)
      save->push_desc0 = *cmd->state.gfx.descriptors.push[0];

   STATIC_ASSERT(sizeof(save->push) ==
                 sizeof(cmd->state.gfx.descriptors.root.push));
   memcpy(save->push, cmd->state.gfx.descriptors.root.push, sizeof(save->push));
}

static void
nvk_meta_init_render(struct nvk_cmd_buffer *cmd,
                     struct vk_meta_rendering_info *info)
{
   const struct nvk_rendering_state *render = &cmd->state.gfx.render;

   *info = (struct vk_meta_rendering_info) {
      .view_mask = render->view_mask,
      .color_attachment_count = render->color_att_count,
      .depth_attachment_format = render->depth_att.vk_format,
      .stencil_attachment_format = render->stencil_att.vk_format,
   };
   for (uint32_t a = 0; a < render->color_att_count; a++)
      info->color_attachment_formats[a] = render->color_att[a].vk_format;
}

static void
nvk_meta_end(struct nvk_cmd_buffer *cmd,
             struct nvk_meta_save *save)
{
   if (save->desc0) {
      cmd->state.gfx.descriptors.sets[0] = save->desc0;
      cmd->state.gfx.descriptors.sets_dirty |= BITFIELD_BIT(0);
   } else if (save->has_push_desc0) {
      *cmd->state.gfx.descriptors.push[0] = save->push_desc0;
      cmd->state.gfx.descriptors.push_dirty |= BITFIELD_BIT(0);
   }

   vk_cmd_set_dynamic_graphics_state(&cmd->vk, &save->dynamic);
   if (save->pipeline)
      nvk_cmd_bind_graphics_pipeline(cmd, save->pipeline);

   nvk_cmd_bind_vertex_buffer(cmd, 0, save->vb0);

   memcpy(cmd->state.gfx.descriptors.root.push, save->push, sizeof(save->push));
}

VKAPI_ATTR void VKAPI_CALL
nvk_CmdBlitImage2(VkCommandBuffer commandBuffer,
                  const VkBlitImageInfo2 *pBlitImageInfo)
{
   VK_FROM_HANDLE(nvk_cmd_buffer, cmd, commandBuffer);
   struct nvk_device *dev = nvk_cmd_buffer_device(cmd);

   struct nvk_meta_save save;
   nvk_meta_begin(cmd, &save);

   vk_meta_blit_image2(&cmd->vk, &dev->meta, pBlitImageInfo);

   nvk_meta_end(cmd, &save);
}
