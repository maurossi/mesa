/*
 * Copyright © 2019 Red Hat.
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

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "util/macros.h"
#include "util/list.h"

#include "compiler/shader_enums.h"
#include "pipe/p_screen.h"
#include "pipe/p_state.h"
#include "cso_cache/cso_context.h"
#include "nir.h"

/* Pre-declarations needed for WSI entrypoints */
struct wl_surface;
struct wl_display;
typedef struct xcb_connection_t xcb_connection_t;
typedef uint32_t xcb_visualid_t;
typedef uint32_t xcb_window_t;

#define VK_PROTOTYPES
#include <vulkan/vulkan.h>
#include <vulkan/vk_icd.h>

#include "lvp_entrypoints.h"
#include "vk_device.h"
#include "vk_instance.h"
#include "vk_physical_device.h"
#include "vk_shader_module.h"
#include "vk_util.h"
#include "vk_format.h"

#include "wsi_common.h"

#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SETS         8
#define MAX_PUSH_CONSTANTS_SIZE 128
#define MAX_PUSH_DESCRIPTORS 32

#ifdef _WIN32
#define lvp_printflike(a, b)
#else
#define lvp_printflike(a, b) __attribute__((__format__(__printf__, a, b)))
#endif

int lvp_get_instance_entrypoint_index(const char *name);
int lvp_get_device_entrypoint_index(const char *name);
int lvp_get_physical_device_entrypoint_index(const char *name);

const char *lvp_get_instance_entry_name(int index);
const char *lvp_get_physical_device_entry_name(int index);
const char *lvp_get_device_entry_name(int index);

bool lvp_instance_entrypoint_is_enabled(int index, uint32_t core_version,
                                         const struct vk_instance_extension_table *instance);
bool lvp_physical_device_entrypoint_is_enabled(int index, uint32_t core_version,
                                                const struct vk_instance_extension_table *instance);
bool lvp_device_entrypoint_is_enabled(int index, uint32_t core_version,
                                       const struct vk_instance_extension_table *instance,
                                       const struct vk_device_extension_table *device);


/* Whenever we generate an error, pass it through this function. Useful for
 * debugging, where we can break on it. Only call at error site, not when
 * propagating errors. Might be useful to plug in a stack trace here.
 */

struct lvp_instance;
VkResult __vk_errorf(struct lvp_instance *instance, VkResult error, const char *file, int line, const char *format, ...);

#define LVP_DEBUG_ALL_ENTRYPOINTS (1 << 0)

#define vk_error(instance, error) __vk_errorf(instance, error, __FILE__, __LINE__, NULL);
#define vk_errorf(instance, error, format, ...) __vk_errorf(instance, error, __FILE__, __LINE__, format, ## __VA_ARGS__);

void __lvp_finishme(const char *file, int line, const char *format, ...)
   lvp_printflike(3, 4);

#define lvp_finishme(format, ...) \
   __lvp_finishme(__FILE__, __LINE__, format, ##__VA_ARGS__);

#define stub_return(v) \
   do { \
      lvp_finishme("stub %s", __func__); \
      return (v); \
   } while (0)

#define stub() \
   do { \
      lvp_finishme("stub %s", __func__); \
      return; \
   } while (0)

#define LVP_STAGE_MASK ((1 << MESA_SHADER_STAGES) - 1)

#define lvp_foreach_stage(stage, stage_bits)                         \
   for (gl_shader_stage stage,                                       \
        __tmp = (gl_shader_stage)((stage_bits) & LVP_STAGE_MASK);    \
        stage = ffs(__tmp) - 1, __tmp;                     \
        __tmp &= ~(1 << (stage)))

struct lvp_physical_device {
   struct vk_physical_device vk;

   struct pipe_loader_device *pld;
   struct pipe_screen *pscreen;
   uint32_t max_images;

   struct wsi_device                       wsi_device;
};

struct lvp_instance {
   struct vk_instance vk;

   uint32_t apiVersion;
   int physicalDeviceCount;
   struct lvp_physical_device physicalDevice;

   uint64_t debug_flags;

   struct pipe_loader_device *devs;
   int num_devices;
};

VkResult lvp_init_wsi(struct lvp_physical_device *physical_device);
void lvp_finish_wsi(struct lvp_physical_device *physical_device);

bool lvp_physical_device_extension_supported(struct lvp_physical_device *dev,
                                              const char *name);

struct lvp_queue {
   struct vk_object_base base;
   VkDeviceQueueCreateFlags flags;
   struct lvp_device *                         device;
   struct pipe_context *ctx;
   struct cso_context *cso;
   bool shutdown;
   thrd_t exec_thread;
   mtx_t m;
   cnd_t new_work;
   struct list_head workqueue;
   volatile int count;
};

struct lvp_queue_work {
   struct list_head list;
   uint32_t cmd_buffer_count;
   struct lvp_cmd_buffer **cmd_buffers;
   struct lvp_fence *fence;
};

struct lvp_pipeline_cache {
   struct vk_object_base                        base;
   struct lvp_device *                          device;
   VkAllocationCallbacks                        alloc;
};

struct lvp_device {
   struct vk_device vk;

   struct lvp_queue queue;
   struct lvp_instance *                       instance;
   struct lvp_physical_device *physical_device;
   struct pipe_screen *pscreen;

   mtx_t fence_lock;
};

void lvp_device_get_cache_uuid(void *uuid);

struct lvp_device_memory {
   struct vk_object_base base;
   struct pipe_memory_allocation *pmem;
   uint32_t                                     type_index;
   VkDeviceSize                                 map_size;
   void *                                       map;
};

struct lvp_image {
   struct vk_object_base base;
   VkImageType type;
   VkDeviceSize size;
   uint32_t alignment;
   struct pipe_memory_allocation *pmem;
   unsigned memory_offset;
   struct pipe_resource *bo;
};

static inline uint32_t
lvp_get_layerCount(const struct lvp_image *image,
                   const VkImageSubresourceRange *range)
{
   return range->layerCount == VK_REMAINING_ARRAY_LAYERS ?
      image->bo->array_size - range->baseArrayLayer : range->layerCount;
}

static inline uint32_t
lvp_get_levelCount(const struct lvp_image *image,
                   const VkImageSubresourceRange *range)
{
   return range->levelCount == VK_REMAINING_MIP_LEVELS ?
      (image->bo->last_level + 1) - range->baseMipLevel : range->levelCount;
}

struct lvp_image_create_info {
   const VkImageCreateInfo *vk_info;
   uint32_t bind_flags;
   uint32_t stride;
};

VkResult
lvp_image_create(VkDevice _device,
                 const struct lvp_image_create_info *create_info,
                 const VkAllocationCallbacks* alloc,
                 VkImage *pImage);

struct lvp_image_view {
   struct vk_object_base base;
   const struct lvp_image *image; /**< VkImageViewCreateInfo::image */

   VkImageViewType view_type;
   VkFormat format;
   enum pipe_format pformat;
   VkComponentMapping components;
   VkImageSubresourceRange subresourceRange;

   struct pipe_surface *surface; /* have we created a pipe surface for this? */
};

struct lvp_subpass_attachment {
   uint32_t         attachment;
   VkImageLayout    layout;
   bool             in_render_loop;
};

struct lvp_subpass {
   uint32_t                                     attachment_count;
   struct lvp_subpass_attachment *             attachments;

   uint32_t                                     input_count;
   uint32_t                                     color_count;
   struct lvp_subpass_attachment *              input_attachments;
   struct lvp_subpass_attachment *              color_attachments;
   struct lvp_subpass_attachment *              resolve_attachments;
   struct lvp_subpass_attachment *              depth_stencil_attachment;
   struct lvp_subpass_attachment *              ds_resolve_attachment;

   /** Subpass has at least one color resolve attachment */
   bool                                         has_color_resolve;

   /** Subpass has at least one color attachment */
   bool                                         has_color_att;

   VkSampleCountFlagBits                        max_sample_count;

   uint32_t                                     view_mask;
};

struct lvp_render_pass_attachment {
   VkFormat                                     format;
   uint32_t                                     samples;
   VkAttachmentLoadOp                           load_op;
   VkAttachmentLoadOp                           stencil_load_op;
   VkImageLayout                                initial_layout;
   VkImageLayout                                final_layout;

   /* The subpass id in which the attachment will be used first/last. */
   uint32_t                                     first_subpass_idx;
   uint32_t                                     last_subpass_idx;
};

struct lvp_render_pass {
   struct vk_object_base                        base;
   uint32_t                                     attachment_count;
   uint32_t                                     subpass_count;
   struct lvp_subpass_attachment *              subpass_attachments;
   struct lvp_render_pass_attachment *          attachments;
   bool has_color_attachment;
   bool has_zs_attachment;
   struct lvp_subpass                           subpasses[0];
};

struct lvp_sampler {
   struct vk_object_base base;
   VkSamplerCreateInfo create_info;
   union pipe_color_union border_color;
   VkSamplerReductionMode reduction_mode;
   uint32_t state[4];
};

struct lvp_framebuffer {
   struct vk_object_base                        base;
   uint32_t                                     width;
   uint32_t                                     height;
   uint32_t                                     layers;

   bool                                         imageless;
   uint32_t                                     attachment_count;
   struct lvp_image_view *                      attachments[0];
};

struct lvp_descriptor_set_binding_layout {
   uint16_t descriptor_index;
   /* Number of array elements in this binding */
   VkDescriptorType type;
   uint16_t array_size;
   bool valid;

   int16_t dynamic_index;
   struct {
      int16_t const_buffer_index;
      int16_t shader_buffer_index;
      int16_t sampler_index;
      int16_t sampler_view_index;
      int16_t image_index;
   } stage[MESA_SHADER_STAGES];

   /* Immutable samplers (or NULL if no immutable samplers) */
   struct lvp_sampler **immutable_samplers;
};

struct lvp_descriptor_set_layout {
   struct vk_object_base base;

   const VkAllocationCallbacks *alloc;
   /* Descriptor set layouts can be destroyed at almost any time */
   uint32_t ref_cnt;

   /* Number of bindings in this descriptor set */
   uint16_t binding_count;

   /* Total size of the descriptor set with room for all array entries */
   uint16_t size;

   /* Shader stages affected by this descriptor set */
   uint16_t shader_stages;

   struct {
      uint16_t const_buffer_count;
      uint16_t shader_buffer_count;
      uint16_t sampler_count;
      uint16_t sampler_view_count;
      uint16_t image_count;
   } stage[MESA_SHADER_STAGES];

   /* Number of dynamic offsets used by this descriptor set */
   uint16_t dynamic_offset_count;

   /* Bindings in this descriptor set */
   struct lvp_descriptor_set_binding_layout binding[0];
};

void lvp_descriptor_set_layout_destroy(struct lvp_device *device,
                                       struct lvp_descriptor_set_layout *layout);

static inline void
lvp_descriptor_set_layout_ref(struct lvp_descriptor_set_layout *layout)
{
   assert(layout && layout->ref_cnt >= 1);
   p_atomic_inc(&layout->ref_cnt);
}

static inline void
lvp_descriptor_set_layout_unref(struct lvp_device *device,
                                struct lvp_descriptor_set_layout *layout)
{
   assert(layout && layout->ref_cnt >= 1);
   if (p_atomic_dec_zero(&layout->ref_cnt))
      lvp_descriptor_set_layout_destroy(device, layout);
}

union lvp_descriptor_info {
   struct {
      struct lvp_sampler *sampler;
      struct lvp_image_view *iview;
      VkImageLayout image_layout;
   };
   struct {
      struct lvp_buffer *buffer;
      VkDeviceSize offset;
      VkDeviceSize range;
   };
   struct lvp_buffer_view *buffer_view;
};

struct lvp_descriptor {
   VkDescriptorType type;

   union lvp_descriptor_info info;
};

struct lvp_descriptor_set {
   struct vk_object_base base;
   struct lvp_descriptor_set_layout *layout;
   struct list_head link;
   struct lvp_descriptor descriptors[0];
};

struct lvp_descriptor_pool {
   struct vk_object_base base;
   VkDescriptorPoolCreateFlags flags;
   uint32_t max_sets;

   struct list_head sets;
};

struct lvp_descriptor_update_template {
   struct vk_object_base base;
   uint32_t entry_count;
   uint32_t set;
   VkDescriptorUpdateTemplateType type;
   VkPipelineBindPoint bind_point;
   struct lvp_pipeline_layout *pipeline_layout;
   VkDescriptorUpdateTemplateEntry entry[0];
};

VkResult
lvp_descriptor_set_create(struct lvp_device *device,
                          struct lvp_descriptor_set_layout *layout,
                          struct lvp_descriptor_set **out_set);

void
lvp_descriptor_set_destroy(struct lvp_device *device,
                           struct lvp_descriptor_set *set);

struct lvp_pipeline_layout {
   struct vk_object_base base;
   struct {
      struct lvp_descriptor_set_layout *layout;
      uint32_t dynamic_offset_start;
   } set[MAX_SETS];

   uint32_t num_sets;
   uint32_t push_constant_size;
   struct {
      bool has_dynamic_offsets;
   } stage[MESA_SHADER_STAGES];
};

struct lvp_pipeline {
   struct vk_object_base base;
   struct lvp_device *                          device;
   struct lvp_pipeline_layout *                 layout;

   void *mem_ctx;
   bool is_compute_pipeline;
   bool force_min_sample;
   nir_shader *pipeline_nir[MESA_SHADER_STAGES];
   void *shader_cso[PIPE_SHADER_TYPES];
   VkGraphicsPipelineCreateInfo graphics_create_info;
   VkComputePipelineCreateInfo compute_create_info;
   uint32_t line_stipple_factor;
   uint16_t line_stipple_pattern;
   bool line_stipple_enable;
   bool line_smooth;
   bool disable_multisample;
   bool line_rectangular;
   bool gs_output_lines;
   bool provoking_vertex_last;
};

struct lvp_event {
   struct vk_object_base base;
   volatile uint64_t event_storage;
};

struct lvp_fence {
   struct vk_object_base base;
   bool signaled;
   struct pipe_fence_handle *handle;
};

struct lvp_semaphore {
   struct vk_object_base base;
   bool dummy;
};

struct lvp_buffer {
   struct vk_object_base base;

   VkDeviceSize                                 size;

   VkBufferUsageFlags                           usage;
   VkDeviceSize                                 offset;

   struct pipe_memory_allocation *pmem;
   struct pipe_resource *bo;
   uint64_t total_size;
};

struct lvp_buffer_view {
   struct vk_object_base base;
   VkFormat format;
   enum pipe_format pformat;
   struct lvp_buffer *buffer;
   uint32_t offset;
   uint64_t range;
};

struct lvp_query_pool {
   struct vk_object_base base;
   VkQueryType type;
   uint32_t count;
   VkQueryPipelineStatisticFlags pipeline_stats;
   enum pipe_query_type base_type;
   struct pipe_query *queries[0];
};

struct lvp_cmd_pool {
   struct vk_object_base                        base;
   VkAllocationCallbacks                        alloc;
   struct list_head                             cmd_buffers;
   struct list_head                             free_cmd_buffers;
};


enum lvp_cmd_buffer_status {
   LVP_CMD_BUFFER_STATUS_INVALID,
   LVP_CMD_BUFFER_STATUS_INITIAL,
   LVP_CMD_BUFFER_STATUS_RECORDING,
   LVP_CMD_BUFFER_STATUS_EXECUTABLE,
   LVP_CMD_BUFFER_STATUS_PENDING,
};

struct lvp_cmd_buffer {
   struct vk_object_base base;

   struct lvp_device *                          device;

   VkCommandBufferLevel                         level;
   enum lvp_cmd_buffer_status status;
   struct lvp_cmd_pool *                        pool;
   struct list_head                             pool_link;

   struct list_head                             cmds;
   struct list_head                            *last_emit;

   uint8_t push_constants[MAX_PUSH_CONSTANTS_SIZE];
};


#define LVP_FROM_HANDLE(__lvp_type, __name, __handle) \
   struct __lvp_type *__name = __lvp_type ## _from_handle(__handle)

VK_DEFINE_HANDLE_CASTS(lvp_cmd_buffer, base, VkCommandBuffer,
                       VK_OBJECT_TYPE_COMMAND_BUFFER)
VK_DEFINE_HANDLE_CASTS(lvp_device, vk.base, VkDevice, VK_OBJECT_TYPE_DEVICE)
VK_DEFINE_HANDLE_CASTS(lvp_instance, vk.base, VkInstance, VK_OBJECT_TYPE_INSTANCE)
VK_DEFINE_HANDLE_CASTS(lvp_physical_device, vk.base, VkPhysicalDevice,
                       VK_OBJECT_TYPE_PHYSICAL_DEVICE)
VK_DEFINE_HANDLE_CASTS(lvp_queue, base, VkQueue, VK_OBJECT_TYPE_QUEUE)

   VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_cmd_pool, base,VkCommandPool,
                                  VK_OBJECT_TYPE_COMMAND_POOL)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_buffer, base, VkBuffer,
                               VK_OBJECT_TYPE_BUFFER)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_buffer_view, base, VkBufferView,
                               VK_OBJECT_TYPE_BUFFER_VIEW)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_descriptor_pool, base, VkDescriptorPool,
                               VK_OBJECT_TYPE_DESCRIPTOR_POOL)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_descriptor_set, base, VkDescriptorSet,
                               VK_OBJECT_TYPE_DESCRIPTOR_SET)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_descriptor_set_layout, base, VkDescriptorSetLayout,
                               VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_descriptor_update_template, base, VkDescriptorUpdateTemplate,
                               VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_device_memory, base, VkDeviceMemory,
                               VK_OBJECT_TYPE_DEVICE_MEMORY)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_event, base, VkEvent, VK_OBJECT_TYPE_EVENT)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_framebuffer, base, VkFramebuffer,
                               VK_OBJECT_TYPE_FRAMEBUFFER)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_image, base, VkImage, VK_OBJECT_TYPE_IMAGE)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_image_view, base, VkImageView,
                               VK_OBJECT_TYPE_IMAGE_VIEW);
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_pipeline_cache, base, VkPipelineCache,
                               VK_OBJECT_TYPE_PIPELINE_CACHE)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_pipeline, base, VkPipeline,
                               VK_OBJECT_TYPE_PIPELINE)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_pipeline_layout, base, VkPipelineLayout,
                               VK_OBJECT_TYPE_PIPELINE_LAYOUT)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_query_pool, base, VkQueryPool,
                               VK_OBJECT_TYPE_QUERY_POOL)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_render_pass, base, VkRenderPass,
                               VK_OBJECT_TYPE_RENDER_PASS)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_sampler, base, VkSampler,
                               VK_OBJECT_TYPE_SAMPLER)
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_fence, base, VkFence, VK_OBJECT_TYPE_FENCE);
VK_DEFINE_NONDISP_HANDLE_CASTS(lvp_semaphore, base, VkSemaphore,
                               VK_OBJECT_TYPE_SEMAPHORE);

/* in same order and buffer building commands in spec. */
enum lvp_cmds {
   LVP_CMD_BIND_PIPELINE,
   LVP_CMD_SET_VIEWPORT,
   LVP_CMD_SET_SCISSOR,
   LVP_CMD_SET_LINE_WIDTH,
   LVP_CMD_SET_DEPTH_BIAS,
   LVP_CMD_SET_BLEND_CONSTANTS,
   LVP_CMD_SET_DEPTH_BOUNDS,
   LVP_CMD_SET_STENCIL_COMPARE_MASK,
   LVP_CMD_SET_STENCIL_WRITE_MASK,
   LVP_CMD_SET_STENCIL_REFERENCE,
   LVP_CMD_BIND_DESCRIPTOR_SETS,
   LVP_CMD_BIND_INDEX_BUFFER,
   LVP_CMD_BIND_VERTEX_BUFFERS,
   LVP_CMD_DRAW,
   LVP_CMD_DRAW_INDEXED,
   LVP_CMD_DRAW_INDIRECT,
   LVP_CMD_DRAW_INDEXED_INDIRECT,
   LVP_CMD_DISPATCH,
   LVP_CMD_DISPATCH_INDIRECT,
   LVP_CMD_COPY_BUFFER,
   LVP_CMD_COPY_IMAGE,
   LVP_CMD_BLIT_IMAGE,
   LVP_CMD_COPY_BUFFER_TO_IMAGE,
   LVP_CMD_COPY_IMAGE_TO_BUFFER,
   LVP_CMD_UPDATE_BUFFER,
   LVP_CMD_FILL_BUFFER,
   LVP_CMD_CLEAR_COLOR_IMAGE,
   LVP_CMD_CLEAR_DEPTH_STENCIL_IMAGE,
   LVP_CMD_CLEAR_ATTACHMENTS,
   LVP_CMD_RESOLVE_IMAGE,
   LVP_CMD_SET_EVENT,
   LVP_CMD_RESET_EVENT,
   LVP_CMD_WAIT_EVENTS,
   LVP_CMD_PIPELINE_BARRIER,
   LVP_CMD_BEGIN_QUERY,
   LVP_CMD_END_QUERY,
   LVP_CMD_RESET_QUERY_POOL,
   LVP_CMD_WRITE_TIMESTAMP,
   LVP_CMD_COPY_QUERY_POOL_RESULTS,
   LVP_CMD_PUSH_CONSTANTS,
   LVP_CMD_BEGIN_RENDER_PASS,
   LVP_CMD_NEXT_SUBPASS,
   LVP_CMD_END_RENDER_PASS,
   LVP_CMD_EXECUTE_COMMANDS,
   LVP_CMD_DRAW_INDIRECT_COUNT,
   LVP_CMD_DRAW_INDEXED_INDIRECT_COUNT,
   LVP_CMD_PUSH_DESCRIPTOR_SET,
   LVP_CMD_BIND_TRANSFORM_FEEDBACK_BUFFERS,
   LVP_CMD_BEGIN_TRANSFORM_FEEDBACK,
   LVP_CMD_END_TRANSFORM_FEEDBACK,
   LVP_CMD_DRAW_INDIRECT_BYTE_COUNT,
   LVP_CMD_BEGIN_CONDITIONAL_RENDERING,
   LVP_CMD_END_CONDITIONAL_RENDERING,
   LVP_CMD_SET_VERTEX_INPUT,
   LVP_CMD_SET_CULL_MODE,
   LVP_CMD_SET_FRONT_FACE,
   LVP_CMD_SET_PRIMITIVE_TOPOLOGY,
   LVP_CMD_SET_DEPTH_TEST_ENABLE,
   LVP_CMD_SET_DEPTH_WRITE_ENABLE,
   LVP_CMD_SET_DEPTH_COMPARE_OP,
   LVP_CMD_SET_DEPTH_BOUNDS_TEST_ENABLE,
   LVP_CMD_SET_STENCIL_TEST_ENABLE,
   LVP_CMD_SET_STENCIL_OP,
   LVP_CMD_SET_LINE_STIPPLE,
   LVP_CMD_SET_DEPTH_BIAS_ENABLE,
   LVP_CMD_SET_LOGIC_OP,
   LVP_CMD_SET_PATCH_CONTROL_POINTS,
   LVP_CMD_SET_PRIMITIVE_RESTART_ENABLE,
   LVP_CMD_SET_RASTERIZER_DISCARD_ENABLE,
};

struct lvp_cmd_bind_pipeline {
   VkPipelineBindPoint bind_point;
   struct lvp_pipeline *pipeline;
};

struct lvp_cmd_set_viewport {
   uint32_t first_viewport;
   uint32_t viewport_count;
   VkViewport viewports[16];
};

struct lvp_cmd_set_scissor {
   uint32_t first_scissor;
   uint32_t scissor_count;
   VkRect2D scissors[16];
};

struct lvp_cmd_set_line_width {
   float line_width;
};

struct lvp_cmd_set_depth_bias {
   float constant_factor;
   float clamp;
   float slope_factor;
};

struct lvp_cmd_set_blend_constants {
   float blend_constants[4];
};

struct lvp_cmd_set_depth_bounds {
   float min_depth;
   float max_depth;
};

struct lvp_cmd_set_stencil_vals {
   VkStencilFaceFlags face_mask;
   uint32_t value;
};

struct lvp_cmd_bind_descriptor_sets {
   VkPipelineBindPoint bind_point;
   struct lvp_descriptor_set_layout *set_layout[MAX_SETS];
   uint32_t first;
   uint32_t count;
   struct lvp_descriptor_set **sets;
   uint32_t dynamic_offset_count;
   const uint32_t *dynamic_offsets;
};

struct lvp_cmd_bind_index_buffer {
   const struct lvp_buffer *buffer;
   VkDeviceSize offset;
   VkIndexType index_type;
};

struct lvp_cmd_bind_vertex_buffers {
   uint32_t first;
   uint32_t binding_count;
   struct lvp_buffer **buffers;
   const VkDeviceSize *offsets;
   const VkDeviceSize *sizes;
   const VkDeviceSize *strides;
};

struct lvp_cmd_draw {
   uint32_t instance_count;
   uint32_t first_instance;
   uint32_t draw_count;
   struct pipe_draw_start_count_bias draws[0];
};

struct lvp_cmd_draw_indexed {
   uint32_t instance_count;
   uint32_t first_instance;
   bool calc_start;
   uint32_t draw_count;
   bool vertex_offset_changes;
   struct pipe_draw_start_count_bias draws[0];
};

struct lvp_cmd_draw_indirect {
   VkDeviceSize offset;
   struct lvp_buffer *buffer;
   uint32_t draw_count;
   uint32_t stride;
};

struct lvp_cmd_dispatch {
   uint32_t x;
   uint32_t y;
   uint32_t z;
   uint32_t base_x;
   uint32_t base_y;
   uint32_t base_z;
};

struct lvp_cmd_dispatch_indirect {
   const struct lvp_buffer *buffer;
   VkDeviceSize offset;
};

struct lvp_cmd_copy_buffer {
   struct lvp_buffer *src;
   struct lvp_buffer *dst;
   uint32_t region_count;
   const VkBufferCopy *regions;
};

struct lvp_cmd_copy_image {
   struct lvp_image *src;
   struct lvp_image *dst;
   VkImageLayout src_layout;
   VkImageLayout dst_layout;
   uint32_t region_count;
   const VkImageCopy *regions;
};

struct lvp_cmd_blit_image {
  struct lvp_image *src;
  struct lvp_image *dst;
  VkImageLayout src_layout;
  VkImageLayout dst_layout;
  uint32_t region_count;
  const VkImageBlit *regions;
  VkFilter filter;
};

struct lvp_cmd_copy_buffer_to_image {
   struct lvp_buffer *src;
   struct lvp_image *dst;
   VkImageLayout dst_layout;
   uint32_t region_count;
   const VkBufferImageCopy *regions;
};

struct lvp_cmd_copy_image_to_buffer {
   struct lvp_image *src;
   struct lvp_buffer *dst;
   VkImageLayout src_layout;
   uint32_t region_count;
   const VkBufferImageCopy *regions;
};

struct lvp_cmd_update_buffer {
   struct lvp_buffer *buffer;
   VkDeviceSize offset;
   VkDeviceSize data_size;
   char data[0];
};

struct lvp_cmd_fill_buffer {
   struct lvp_buffer *buffer;
   VkDeviceSize offset;
   VkDeviceSize fill_size;
   uint32_t data;
};

struct lvp_cmd_clear_color_image {
   struct lvp_image *image;
   VkImageLayout layout;
   VkClearColorValue clear_val;
   uint32_t range_count;
   VkImageSubresourceRange *ranges;
};

struct lvp_cmd_clear_ds_image {
   struct lvp_image *image;
   VkImageLayout layout;
   VkClearDepthStencilValue clear_val;
   uint32_t range_count;
   VkImageSubresourceRange *ranges;
};

struct lvp_cmd_clear_attachments {
   uint32_t attachment_count;
   VkClearAttachment *attachments;
   uint32_t rect_count;
   VkClearRect *rects;
};

struct lvp_cmd_resolve_image {
   struct lvp_image *src;
   struct lvp_image *dst;
   VkImageLayout src_layout;
   VkImageLayout dst_layout;
   uint32_t region_count;
   VkImageResolve *regions;
};

struct lvp_cmd_event_set {
   struct lvp_event *event;
   bool value;
   bool flush;
};

struct lvp_cmd_wait_events {
   uint32_t event_count;
   struct lvp_event **events;
   VkPipelineStageFlags src_stage_mask;
   VkPipelineStageFlags dst_stage_mask;
   uint32_t memory_barrier_count;
   VkMemoryBarrier *memory_barriers;
   uint32_t buffer_memory_barrier_count;
   VkBufferMemoryBarrier *buffer_memory_barriers;
   uint32_t image_memory_barrier_count;
   VkImageMemoryBarrier *image_memory_barriers;
};

struct lvp_cmd_pipeline_barrier {
   VkPipelineStageFlags src_stage_mask;
   VkPipelineStageFlags dst_stage_mask;
   bool by_region;
   uint32_t memory_barrier_count;
   VkMemoryBarrier *memory_barriers;
   uint32_t buffer_memory_barrier_count;
   VkBufferMemoryBarrier *buffer_memory_barriers;
   uint32_t image_memory_barrier_count;
   VkImageMemoryBarrier *image_memory_barriers;
};

struct lvp_cmd_query_cmd {
   struct lvp_query_pool *pool;
   uint32_t query;
   uint32_t index;
   bool precise;
   bool flush;
};

struct lvp_cmd_copy_query_pool_results {
   struct lvp_query_pool *pool;
   uint32_t first_query;
   uint32_t query_count;
   struct lvp_buffer *dst;
   VkDeviceSize dst_offset;
   VkDeviceSize stride;
   VkQueryResultFlags flags;
};

struct lvp_cmd_push_constants {
   VkShaderStageFlags stage;
   uint32_t offset;
   uint32_t size;
   uint32_t val[1];
};

struct lvp_attachment_state {
   VkImageAspectFlags pending_clear_aspects;
   VkClearValue clear_value;
};

struct lvp_cmd_begin_render_pass {
   struct lvp_framebuffer *framebuffer;
   struct lvp_render_pass *render_pass;
   VkRect2D render_area;
   struct lvp_attachment_state *attachments;
   struct lvp_image_view **imageless_views;
};

struct lvp_cmd_next_subpass {
   VkSubpassContents contents;
};

struct lvp_cmd_execute_commands {
   uint32_t command_buffer_count;
   struct lvp_cmd_buffer *cmd_buffers[0];
};

struct lvp_cmd_draw_indirect_count {
   VkDeviceSize offset;
   struct lvp_buffer *buffer;
   VkDeviceSize count_buffer_offset;
   struct lvp_buffer *count_buffer;
   uint32_t max_draw_count;
   uint32_t stride;
};

struct lvp_write_descriptor {
   uint32_t dst_binding;
   uint32_t dst_array_element;
   uint32_t descriptor_count;
   VkDescriptorType descriptor_type;
};

struct lvp_cmd_push_descriptor_set {
   VkPipelineBindPoint bind_point;
   struct lvp_pipeline_layout *layout;
   uint32_t set;
   uint32_t descriptor_write_count;
   struct lvp_write_descriptor *descriptors;
   union lvp_descriptor_info *infos;
};

struct lvp_cmd_bind_transform_feedback_buffers {
   uint32_t first_binding;
   uint32_t binding_count;
   struct lvp_buffer **buffers;
   VkDeviceSize *offsets;
   VkDeviceSize *sizes;
};

struct lvp_cmd_begin_transform_feedback {
   uint32_t first_counter_buffer;
   uint32_t counter_buffer_count;
   struct lvp_buffer **counter_buffers;
   VkDeviceSize *counter_buffer_offsets;
};

struct lvp_cmd_end_transform_feedback {
   uint32_t first_counter_buffer;
   uint32_t counter_buffer_count;
   struct lvp_buffer **counter_buffers;
   VkDeviceSize *counter_buffer_offsets;
};

struct lvp_cmd_draw_indirect_byte_count {
   uint32_t instance_count;
   uint32_t first_instance;
   struct lvp_buffer *counter_buffer;
   VkDeviceSize counter_buffer_offset;
   uint32_t counter_offset;
   uint32_t vertex_stride;
};

struct lvp_cmd_begin_conditional_rendering {
   struct lvp_buffer *buffer;
   VkDeviceSize offset;
   bool inverted;
};

struct lvp_cmd_set_vertex_input {
    uint32_t binding_count;
    uint32_t attr_count;
    uint8_t data[0];
    //VkVertexInputBindingDescription2EXT bindings[binding_count];
    //VkVertexInputAttributeDescription2EXT attrs[attr_count];
};

struct lvp_cmd_set_cull_mode {
   VkCullModeFlags cull_mode;
};

struct lvp_cmd_set_front_face {
   VkFrontFace front_face;
};

struct lvp_cmd_set_primitive_topology {
   VkPrimitiveTopology prim;
};

struct lvp_cmd_set_depth_test_enable {
   VkBool32 depth_test_enable;
};

struct lvp_cmd_set_depth_write_enable {
   VkBool32 depth_write_enable;
};

struct lvp_cmd_set_depth_bounds_test_enable {
   VkBool32 depth_bounds_test_enable;
};

struct lvp_cmd_set_depth_compare_op {
   VkCompareOp depth_op;
};

struct lvp_cmd_set_stencil_test_enable {
   VkBool32 stencil_test_enable;
};

struct lvp_cmd_set_stencil_op {
   VkStencilFaceFlags face_mask;
   VkStencilOp fail_op;
   VkStencilOp pass_op;
   VkStencilOp depth_fail_op;
   VkCompareOp compare_op;
};

struct lvp_cmd_set_line_stipple {
   uint32_t line_stipple_factor;
   uint16_t line_stipple_pattern;
};

struct lvp_cmd_set_depth_bias_enable {
   bool enable;
};

struct lvp_cmd_set_logic_op {
   VkLogicOp op;
};

struct lvp_cmd_set_patch_control_points {
   uint32_t vertices_per_patch;
};

struct lvp_cmd_set_primitive_restart_enable {
   bool enable;
};

struct lvp_cmd_set_rasterizer_discard_enable {
   bool enable;
};

struct lvp_cmd_buffer_entry {
   struct list_head cmd_link;
   uint32_t cmd_type;
   union {
      struct lvp_cmd_bind_pipeline pipeline;
      struct lvp_cmd_set_viewport set_viewport;
      struct lvp_cmd_set_scissor set_scissor;
      struct lvp_cmd_set_line_width set_line_width;
      struct lvp_cmd_set_depth_bias set_depth_bias;
      struct lvp_cmd_set_blend_constants set_blend_constants;
      struct lvp_cmd_set_depth_bounds set_depth_bounds;
      struct lvp_cmd_set_stencil_vals stencil_vals;
      struct lvp_cmd_bind_descriptor_sets descriptor_sets;
      struct lvp_cmd_bind_vertex_buffers vertex_buffers;
      struct lvp_cmd_bind_index_buffer index_buffer;
      struct lvp_cmd_draw draw;
      struct lvp_cmd_draw_indexed draw_indexed;
      struct lvp_cmd_draw_indirect draw_indirect;
      struct lvp_cmd_dispatch dispatch;
      struct lvp_cmd_dispatch_indirect dispatch_indirect;
      struct lvp_cmd_copy_buffer copy_buffer;
      struct lvp_cmd_copy_image copy_image;
      struct lvp_cmd_blit_image blit_image;
      struct lvp_cmd_copy_buffer_to_image buffer_to_img;
      struct lvp_cmd_copy_image_to_buffer img_to_buffer;
      struct lvp_cmd_update_buffer update_buffer;
      struct lvp_cmd_fill_buffer fill_buffer;
      struct lvp_cmd_clear_color_image clear_color_image;
      struct lvp_cmd_clear_ds_image clear_ds_image;
      struct lvp_cmd_clear_attachments clear_attachments;
      struct lvp_cmd_resolve_image resolve_image;
      struct lvp_cmd_event_set event_set;
      struct lvp_cmd_wait_events wait_events;
      struct lvp_cmd_pipeline_barrier pipeline_barrier;
      struct lvp_cmd_query_cmd query;
      struct lvp_cmd_copy_query_pool_results copy_query_pool_results;
      struct lvp_cmd_push_constants push_constants;
      struct lvp_cmd_begin_render_pass begin_render_pass;
      struct lvp_cmd_next_subpass next_subpass;
      struct lvp_cmd_execute_commands execute_commands;
      struct lvp_cmd_draw_indirect_count draw_indirect_count;
      struct lvp_cmd_push_descriptor_set push_descriptor_set;
      struct lvp_cmd_bind_transform_feedback_buffers bind_transform_feedback_buffers;
      struct lvp_cmd_begin_transform_feedback begin_transform_feedback;
      struct lvp_cmd_end_transform_feedback end_transform_feedback;
      struct lvp_cmd_draw_indirect_byte_count draw_indirect_byte_count;
      struct lvp_cmd_begin_conditional_rendering begin_conditional_rendering;
      struct lvp_cmd_set_vertex_input set_vertex_input;
      struct lvp_cmd_set_cull_mode set_cull_mode;
      struct lvp_cmd_set_front_face set_front_face;
      struct lvp_cmd_set_primitive_topology set_primitive_topology;
      struct lvp_cmd_set_depth_test_enable set_depth_test_enable;
      struct lvp_cmd_set_depth_write_enable set_depth_write_enable;
      struct lvp_cmd_set_depth_compare_op set_depth_compare_op;
      struct lvp_cmd_set_depth_bounds_test_enable set_depth_bounds_test_enable;
      struct lvp_cmd_set_stencil_test_enable set_stencil_test_enable;
      struct lvp_cmd_set_stencil_op set_stencil_op;
      struct lvp_cmd_set_line_stipple set_line_stipple;
      struct lvp_cmd_set_depth_bias_enable set_depth_bias_enable;
      struct lvp_cmd_set_logic_op set_logic_op;
      struct lvp_cmd_set_patch_control_points set_patch_control_points;
      struct lvp_cmd_set_primitive_restart_enable set_primitive_restart_enable;
      struct lvp_cmd_set_rasterizer_discard_enable set_rasterizer_discard_enable;
   } u;
};

VkResult lvp_execute_cmds(struct lvp_device *device,
                          struct lvp_queue *queue,
                          struct lvp_cmd_buffer *cmd_buffer);

struct lvp_image *lvp_swapchain_get_image(VkSwapchainKHR swapchain,
					  uint32_t index);

static inline enum pipe_format
lvp_vk_format_to_pipe_format(VkFormat format)
{
   /* Some formats cause problems with CTS right now.*/
   if (format == VK_FORMAT_R4G4B4A4_UNORM_PACK16 ||
       format == VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT || /* VK_EXT_4444_formats */
       format == VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT || /* VK_EXT_4444_formats */
       format == VK_FORMAT_R5G5B5A1_UNORM_PACK16 ||
       format == VK_FORMAT_R8_SRGB ||
       format == VK_FORMAT_R8G8_SRGB ||
       format == VK_FORMAT_R64G64B64A64_SFLOAT ||
       format == VK_FORMAT_R64_SFLOAT ||
       format == VK_FORMAT_R64G64_SFLOAT ||
       format == VK_FORMAT_R64G64B64_SFLOAT ||
       format == VK_FORMAT_A2R10G10B10_SINT_PACK32 ||
       format == VK_FORMAT_A2B10G10R10_SINT_PACK32 ||
       format == VK_FORMAT_G8B8G8R8_422_UNORM ||
       format == VK_FORMAT_B8G8R8G8_422_UNORM ||
       format == VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM ||
       format == VK_FORMAT_G8_B8R8_2PLANE_420_UNORM ||
       format == VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM ||
       format == VK_FORMAT_G8_B8R8_2PLANE_422_UNORM ||
       format == VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM ||
       format == VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM ||
       format == VK_FORMAT_G16_B16R16_2PLANE_420_UNORM ||
       format == VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM ||
       format == VK_FORMAT_G16_B16R16_2PLANE_422_UNORM ||
       format == VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM ||
       format == VK_FORMAT_D16_UNORM_S8_UINT)
      return PIPE_FORMAT_NONE;

   return vk_format_to_pipe_format(format);
}

#ifdef __cplusplus
}
#endif
