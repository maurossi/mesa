#include "nvk_buffer.h"
#include "nvk_cmd_buffer.h"
#include "nvk_descriptor_set.h"
#include "nvk_device.h"
#include "nvk_mme.h"
#include "nvk_physical_device.h"
#include "nvk_pipeline.h"

#include "nouveau_context.h"

#include "classes/cla0b5.h"

#include "nvk_cl9097.h"
#include "nvk_cla0c0.h"
#include "cla1c0.h"
#include "clc0c0.h"
#include "nvk_clc3c0.h"
#include "nvk_clc597.h"

#include "drf.h"
#include "cla0c0qmd.h"
#include "clc0c0qmd.h"
#include "clc3c0qmd.h"

#define NVA0C0_QMDV00_06_VAL_SET(p,a...) NVVAL_MW_SET((p), NVA0C0, QMDV00_06, ##a)
#define NVA0C0_QMDV00_06_DEF_SET(p,a...) NVDEF_MW_SET((p), NVA0C0, QMDV00_06, ##a)
#define NVC0C0_QMDV02_01_VAL_SET(p,a...) NVVAL_MW_SET((p), NVC0C0, QMDV02_01, ##a)
#define NVC0C0_QMDV02_01_DEF_SET(p,a...) NVDEF_MW_SET((p), NVC0C0, QMDV02_01, ##a)
#define NVC3C0_QMDV02_02_VAL_SET(p,a...) NVVAL_MW_SET((p), NVC3C0, QMDV02_02, ##a)
#define NVC3C0_QMDV02_02_DEF_SET(p,a...) NVDEF_MW_SET((p), NVC3C0, QMDV02_02, ##a)

void
nvk_cmd_buffer_begin_compute(struct nvk_cmd_buffer *cmd,
                             const VkCommandBufferBeginInfo *pBeginInfo)
{ }

static void
nva0c0_qmd_set_dispatch_size(UNUSED struct nvk_device *dev, uint32_t *qmd,
                             uint32_t x, uint32_t y, uint32_t z)
{
   NVA0C0_QMDV00_06_VAL_SET(qmd, CTA_RASTER_WIDTH, x);
   NVA0C0_QMDV00_06_VAL_SET(qmd, CTA_RASTER_HEIGHT, y);
   NVA0C0_QMDV00_06_VAL_SET(qmd, CTA_RASTER_DEPTH, z);
}

static void
nvc0c0_qmd_set_dispatch_size(UNUSED struct nvk_device *dev, uint32_t *qmd,
                             uint32_t x, uint32_t y, uint32_t z)
{
   NVC0C0_QMDV02_01_VAL_SET(qmd, CTA_RASTER_WIDTH, x);
   NVC0C0_QMDV02_01_VAL_SET(qmd, CTA_RASTER_HEIGHT, y);
   /* this field is different from older QMD versions */
   NVC0C0_QMDV02_01_VAL_SET(qmd, CTA_RASTER_DEPTH, z);
}

static uint32_t
qmd_dispatch_size_offset(struct nvk_device *dev)
{
   assert(dev->ctx->compute.cls >= VOLTA_COMPUTE_A);
   uint32_t bit = DRF_LO(DRF_MW(NVC3C0_QMDV02_02_CTA_RASTER_WIDTH));
   assert(bit % 32 == 0);
   assert(DRF_LO(DRF_MW(NVC3C0_QMDV02_02_CTA_RASTER_HEIGHT)) == bit + 32);
   assert(DRF_LO(DRF_MW(NVC3C0_QMDV02_02_CTA_RASTER_DEPTH)) == bit + 64);
   return bit / 8;
}

static inline void
nva0c0_cp_launch_desc_set_cb(uint32_t *qmd, unsigned index,
                             uint32_t size, uint64_t address)
{
   NVA0C0_QMDV00_06_VAL_SET(qmd, CONSTANT_BUFFER_ADDR_LOWER, index, address);
   NVA0C0_QMDV00_06_VAL_SET(qmd, CONSTANT_BUFFER_ADDR_UPPER, index, address >> 32);
   NVA0C0_QMDV00_06_VAL_SET(qmd, CONSTANT_BUFFER_SIZE, index, size);
   NVA0C0_QMDV00_06_DEF_SET(qmd, CONSTANT_BUFFER_VALID, index, TRUE);
}

static inline void
nvc0c0_cp_launch_desc_set_cb(uint32_t *qmd, unsigned index,
                             uint32_t size, uint64_t address)
{
   NVC0C0_QMDV02_01_VAL_SET(qmd, CONSTANT_BUFFER_ADDR_LOWER, index, address);
   NVC0C0_QMDV02_01_VAL_SET(qmd, CONSTANT_BUFFER_ADDR_UPPER, index, address >> 32);
   NVC0C0_QMDV02_01_VAL_SET(qmd, CONSTANT_BUFFER_SIZE_SHIFTED4, index,
                                 DIV_ROUND_UP(size, 16));
   NVC0C0_QMDV02_01_DEF_SET(qmd, CONSTANT_BUFFER_VALID, index, TRUE);
}

void
nvk_cmd_bind_compute_pipeline(struct nvk_cmd_buffer *cmd,
                              struct nvk_compute_pipeline *pipeline)
{
   cmd->state.cs.pipeline = pipeline;
}

static uint32_t
nvk_compute_local_size(struct nvk_cmd_buffer *cmd)
{
   const struct nvk_compute_pipeline *pipeline = cmd->state.cs.pipeline;
   const struct nvk_shader *shader =
      &pipeline->base.shaders[MESA_SHADER_COMPUTE];

   return shader->cp.block_size[0] *
          shader->cp.block_size[1] *
          shader->cp.block_size[2];
}

static uint64_t
nvk_flush_compute_state(struct nvk_cmd_buffer *cmd,
                        uint64_t *root_desc_addr_out)
{
   const struct nvk_compute_pipeline *pipeline = cmd->state.cs.pipeline;
   const struct nvk_device *dev = nvk_cmd_buffer_device(cmd);
   struct nvk_descriptor_state *desc = &cmd->state.cs.descriptors;
   VkResult result;

   nvk_cmd_buffer_flush_push_descriptors(cmd, desc);

   /* pre Pascal the constant buffer sizes need to be 0x100 aligned. As we simply allocated a
    * buffer and upload data to it, make sure its size is 0x100 aligned.
    */
   STATIC_ASSERT((sizeof(desc->root) & 0xff) == 0);

   uint64_t root_desc_addr;
   result = nvk_cmd_buffer_upload_data(cmd, &desc->root, sizeof(desc->root),
                                       NVK_MIN_UBO_ALIGNMENT,
                                       &root_desc_addr);
   if (unlikely(result != VK_SUCCESS)) {
      vk_command_buffer_set_error(&cmd->vk, result);
      return 0;
   }

   uint32_t qmd[128];
   memset(qmd, 0, sizeof(qmd));
   memcpy(qmd, pipeline->qmd_template, sizeof(pipeline->qmd_template));

   if (dev->ctx->compute.cls >= PASCAL_COMPUTE_A) {
      nvc0c0_qmd_set_dispatch_size(nvk_cmd_buffer_device(cmd), qmd,
                                   desc->root.cs.group_count[0],
                                   desc->root.cs.group_count[1],
                                   desc->root.cs.group_count[2]);

      nvc0c0_cp_launch_desc_set_cb(qmd, 0, sizeof(desc->root), root_desc_addr);
      nvc0c0_cp_launch_desc_set_cb(qmd, 1, sizeof(desc->root), root_desc_addr);
   } else {
      assert(dev->ctx->compute.cls >= KEPLER_COMPUTE_A);
      nva0c0_qmd_set_dispatch_size(nvk_cmd_buffer_device(cmd), qmd,
                                   desc->root.cs.group_count[0],
                                   desc->root.cs.group_count[1],
                                   desc->root.cs.group_count[2]);

      nva0c0_cp_launch_desc_set_cb(qmd, 0, sizeof(desc->root), root_desc_addr);
      nva0c0_cp_launch_desc_set_cb(qmd, 1, sizeof(desc->root), root_desc_addr);
   }

   uint64_t qmd_addr;
   result = nvk_cmd_buffer_upload_data(cmd, qmd, sizeof(qmd), 256, &qmd_addr);
   if (unlikely(result != VK_SUCCESS)) {
      vk_command_buffer_set_error(&cmd->vk, result);
      return 0;
   }

   if (root_desc_addr_out != NULL)
      *root_desc_addr_out = root_desc_addr;

   return qmd_addr;
}

static void
nvk_build_mme_add_cs_invocations(struct mme_builder *b,
                                 struct mme_value64 count)
{
   struct mme_value accum_hi = mme_state(b,
      NVC597_SET_MME_SHADOW_SCRATCH(NVK_MME_SCRATCH_CS_INVOCATIONS_HI));
   struct mme_value accum_lo = mme_state(b,
      NVC597_SET_MME_SHADOW_SCRATCH(NVK_MME_SCRATCH_CS_INVOCATIONS_LO));
   struct mme_value64 accum = mme_value64(accum_lo, accum_hi);

   accum = mme_add64(b, accum, count);

   STATIC_ASSERT(NVK_MME_SCRATCH_CS_INVOCATIONS_HI + 1 ==
                 NVK_MME_SCRATCH_CS_INVOCATIONS_LO);

   mme_mthd(b, NVC597_SET_MME_SHADOW_SCRATCH(NVK_MME_SCRATCH_CS_INVOCATIONS_HI));
   mme_emit(b, accum.hi);
   mme_emit(b, accum.lo);
}

void
nvk_mme_add_cs_invocations(struct nvk_device *dev, struct mme_builder *b)
{
   struct mme_value count_hi = mme_load(b);
   struct mme_value count_lo = mme_load(b);
   struct mme_value64 count = mme_value64(count_lo, count_hi);

   nvk_build_mme_add_cs_invocations(b, count);
}

VKAPI_ATTR void VKAPI_CALL
nvk_CmdDispatch(VkCommandBuffer commandBuffer,
                uint32_t groupCountX,
                uint32_t groupCountY,
                uint32_t groupCountZ)
{
   VK_FROM_HANDLE(nvk_cmd_buffer, cmd, commandBuffer);
   struct nvk_descriptor_state *desc = &cmd->state.cs.descriptors;

   desc->root.cs.group_count[0] = groupCountX;
   desc->root.cs.group_count[1] = groupCountY;
   desc->root.cs.group_count[2] = groupCountZ;

   uint64_t qmd_addr = nvk_flush_compute_state(cmd, NULL);
   if (unlikely(qmd_addr == 0))
      return;

   const uint32_t local_size = nvk_compute_local_size(cmd);
   const uint64_t cs_invocations =
      (uint64_t)local_size * (uint64_t)groupCountX *
      (uint64_t)groupCountY * (uint64_t)groupCountZ;

   struct nv_push *p = nvk_cmd_buffer_push(cmd, 9);

   P_1INC(p, NV9097, CALL_MME_MACRO(NVK_MME_ADD_CS_INVOCATIONS));
   P_INLINE_DATA(p, cs_invocations >> 32);
   P_INLINE_DATA(p, cs_invocations);

   P_MTHD(p, NVA0C0, INVALIDATE_SHADER_CACHES_NO_WFI);
   P_NVA0C0_INVALIDATE_SHADER_CACHES_NO_WFI(p, {
      .constant = CONSTANT_TRUE
   });

   P_MTHD(p, NVA0C0, SEND_PCAS_A);
   P_NVA0C0_SEND_PCAS_A(p, qmd_addr >> 8);
   P_IMMD(p, NVA0C0, SEND_SIGNALING_PCAS_B, {
      .invalidate = INVALIDATE_TRUE,
      .schedule = SCHEDULE_TRUE
   });
}

static void
mme_store_global(struct mme_builder *b,
                 struct mme_value64 addr,
                 uint64_t offset,
                 struct mme_value v)
{
   if (offset > 0)
      addr = mme_add64(b, addr, mme_imm64(offset));

   mme_mthd(b, NV9097_SET_REPORT_SEMAPHORE_A);
   mme_emit_addr64(b, addr);
   mme_emit(b, v);
   mme_emit(b, mme_imm(0x10000000));

   if (offset > 0) {
      mme_free_reg(b, addr.lo);
      mme_free_reg(b, addr.hi);
   }
}

static void
mme_store_global_vec3(struct mme_builder *b,
                      struct mme_value64 addr,
                      uint32_t offset,
                      struct mme_value x,
                      struct mme_value y,
                      struct mme_value z)
{
   mme_store_global(b, addr, offset + 0, x);
   mme_store_global(b, addr, offset + 4, y);
   mme_store_global(b, addr, offset + 8, z);
}

void
nvk_mme_dispatch_indirect(struct nvk_device *dev, struct mme_builder *b)
{
   struct mme_value local_size = mme_load(b);
   struct mme_value64 dispatch_addr = mme_load_addr64(b);
   struct mme_value64 root_desc_addr = mme_load_addr64(b);
   struct mme_value64 qmd_addr = mme_load_addr64(b);

   mme_tu104_read_fifoed(b, dispatch_addr, mme_imm(3));

   uint32_t qmd_size_offset = qmd_dispatch_size_offset(dev);
   uint32_t root_desc_size_offset =
      offsetof(struct nvk_root_descriptor_table, cs.group_count);

   struct mme_value group_count_x = mme_load(b);
   struct mme_value group_count_y = mme_load(b);
   struct mme_value group_count_z = mme_load(b);

   struct mme_value64 cs1 = mme_umul_32x32_64(b, local_size, group_count_x);
   struct mme_value64 cs2 = mme_umul_32x32_64(b, group_count_y, group_count_z);
   nvk_build_mme_add_cs_invocations(b, mme_mul64(b, cs1, cs2));

   mme_store_global_vec3(b, qmd_addr, qmd_size_offset,
                         group_count_x, group_count_y, group_count_z);
   mme_store_global_vec3(b, root_desc_addr, root_desc_size_offset,
                         group_count_x, group_count_y, group_count_z);
}

VKAPI_ATTR void VKAPI_CALL
nvk_CmdDispatchIndirect(VkCommandBuffer commandBuffer,
                        VkBuffer _buffer,
                        VkDeviceSize offset)
{
   VK_FROM_HANDLE(nvk_cmd_buffer, cmd, commandBuffer);
   VK_FROM_HANDLE(nvk_buffer, buffer, _buffer);

   uint64_t dispatch_addr = nvk_buffer_address(buffer, offset);

   uint64_t root_desc_addr;
   uint64_t qmd_addr = nvk_flush_compute_state(cmd, &root_desc_addr);
   if (unlikely(qmd_addr == 0))
      return;

   struct nv_push *p = nvk_cmd_buffer_push(cmd, 16);

   P_IMMD(p, NVC597, SET_MME_DATA_FIFO_CONFIG, FIFO_SIZE_SIZE_4KB);
   P_1INC(p, NV9097, CALL_MME_MACRO(NVK_MME_DISPATCH_INDIRECT));
   P_INLINE_DATA(p, nvk_compute_local_size(cmd));
   P_INLINE_DATA(p, dispatch_addr >> 32);
   P_INLINE_DATA(p, dispatch_addr);
   P_INLINE_DATA(p, root_desc_addr >> 32);
   P_INLINE_DATA(p, root_desc_addr);
   P_INLINE_DATA(p, qmd_addr >> 32);
   P_INLINE_DATA(p, qmd_addr);

   P_MTHD(p, NVA0C0, INVALIDATE_SHADER_CACHES_NO_WFI);
   P_NVA0C0_INVALIDATE_SHADER_CACHES_NO_WFI(p, {
      .constant = CONSTANT_TRUE
   });

   P_MTHD(p, NVA0C0, SEND_PCAS_A);
   P_NVA0C0_SEND_PCAS_A(p, qmd_addr >> 8);
   P_IMMD(p, NVA0C0, SEND_SIGNALING_PCAS_B, {
      .invalidate = INVALIDATE_TRUE,
      .schedule = SCHEDULE_TRUE
   });
}
