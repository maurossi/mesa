#include "mme_runner.h"

#include <fcntl.h>
#include <string.h>
#include <xf86drm.h>

#include "mme_fermi_sim.h"
#include "mme_tu104_sim.h"

#include "nvk_clc597.h"

#include "nouveau_bo.h"
#include "nouveau_context.h"

/* nouveau_drm.h isn't C++-friendly */
#define class cls
#include <nouveau_drm.h>
#undef class

mme_runner::mme_runner() :
  devinfo(NULL), data_addr(0), data(NULL)
{ }

mme_runner::~mme_runner()
{ }

mme_hw_runner::mme_hw_runner() :
  mme_runner(), p(NULL), dev(NULL), ctx(NULL),
  data_bo(NULL), push_bo(NULL), push_map(NULL)
{
   memset(&push, 0, sizeof(push));
}

void
mme_runner::mme_store_data(mme_builder *b, uint32_t dw_idx,
                           mme_value data, bool free_reg)
{
   mme_store_imm_addr(b, data_addr + dw_idx * 4, data, free_reg);
}

mme_hw_runner::~mme_hw_runner()
{
   if (push_bo) {
      nouveau_ws_bo_unmap(push_bo, push_map);
      nouveau_ws_bo_destroy(push_bo);
   }
   if (ctx)
      nouveau_ws_context_destroy(ctx);
   if (dev)
      nouveau_ws_device_destroy(dev);
}

#define PUSH_SIZE 64 * 4096

bool
mme_hw_runner::set_up_hw(uint16_t min_cls, uint16_t max_cls)
{
   drmDevicePtr devices[8];
   int max_devices = drmGetDevices2(0, devices, 8);

   int i;
   for (i = 0; i < max_devices; i++) {
      if (devices[i]->available_nodes & 1 << DRM_NODE_RENDER &&
          devices[i]->bustype == DRM_BUS_PCI &&
          devices[i]->deviceinfo.pci->vendor_id == 0x10de) {
         dev = nouveau_ws_device_new(devices[i]);
         if (dev == NULL)
            continue;

         if (dev->info.cls_eng3d < min_cls || dev->info.cls_eng3d > max_cls) {
            nouveau_ws_device_destroy(dev);
            dev = NULL;
            continue;
         }

         /* Found a Turning+ device */
         break;
      }
   }

   if (dev == NULL)
      return false;

   devinfo = &dev->info;

   int ret = nouveau_ws_context_create(dev, &ctx);
   if (ret)
      return false;

   uint32_t data_bo_flags = NOUVEAU_WS_BO_GART | NOUVEAU_WS_BO_MAP;
   data_bo = nouveau_ws_bo_new_mapped(dev, DATA_BO_SIZE, 0,
                                      (nouveau_ws_bo_flags)data_bo_flags,
                                      NOUVEAU_WS_BO_RDWR, (void **)&data);
   if (data_bo == NULL)
      return false;

   memset(data, 139, DATA_BO_SIZE);
   data_addr = data_bo->offset;

   uint32_t push_bo_flags = NOUVEAU_WS_BO_GART | NOUVEAU_WS_BO_MAP;
   push_bo = nouveau_ws_bo_new_mapped(dev, PUSH_SIZE, 0,
                                      (nouveau_ws_bo_flags)push_bo_flags,
                                      NOUVEAU_WS_BO_WR, &push_map);
   if (push_bo == NULL)
      return false;

   reset_push();

   return true;
}

void
mme_hw_runner::reset_push()
{
   nv_push_init(&push, (uint32_t *)push_map, PUSH_SIZE / 4);
   p = &push;

   P_MTHD(p, NV9097, SET_OBJECT);
   P_NV9097_SET_OBJECT(p, {
      .class_id = dev->info.cls_eng3d,
      .engine_id = 0,
   });
}

void
mme_hw_runner::submit_push()
{
   struct drm_nouveau_gem_pushbuf_bo bos[2];
   memset(bos, 0, sizeof(bos));

   bos[0].handle = push_bo->handle,
   bos[0].valid_domains = NOUVEAU_GEM_DOMAIN_GART;
   bos[0].read_domains = NOUVEAU_GEM_DOMAIN_GART;

   bos[1].handle = data_bo->handle,
   bos[1].valid_domains = NOUVEAU_GEM_DOMAIN_GART;
   bos[1].read_domains = NOUVEAU_GEM_DOMAIN_GART;
   bos[1].write_domains = NOUVEAU_GEM_DOMAIN_GART;

   struct drm_nouveau_gem_pushbuf_push push;
   memset(&push, 0, sizeof(push));

   push.bo_index = 0;
   push.offset = 0;
   push.length = nv_push_dw_count(&this->push) * 4;

   struct drm_nouveau_gem_pushbuf req;
   memset(&req, 0, sizeof(req));

   req.channel = ctx->channel;
   req.nr_buffers = 2;
   req.buffers = (uintptr_t)bos;
   req.nr_push = 1;
   req.push = (uintptr_t)&push;

   int ret = drmCommandWriteRead(dev->fd, DRM_NOUVEAU_GEM_PUSHBUF,
                                 &req, sizeof(req));
   ASSERT_EQ(ret, 0);

   bool ok = nouveau_ws_bo_wait(data_bo, NOUVEAU_WS_BO_RDWR);
   ASSERT_TRUE(ok);
}

void
mme_hw_runner::push_macro(uint32_t id, const std::vector<uint32_t> &macro)
{
   P_MTHD(p, NV9097, LOAD_MME_START_ADDRESS_RAM_POINTER);
   P_NV9097_LOAD_MME_START_ADDRESS_RAM_POINTER(p, id);
   P_NV9097_LOAD_MME_START_ADDRESS_RAM(p, 0);
   P_1INC(p, NV9097, LOAD_MME_INSTRUCTION_RAM_POINTER);
   P_NV9097_LOAD_MME_INSTRUCTION_RAM_POINTER(p, 0);
   P_INLINE_ARRAY(p, &macro[0], macro.size());
}

void
mme_hw_runner::run_macro(const std::vector<uint32_t>& macro,
                         const std::vector<uint32_t>& params)
{
   push_macro(0, macro);

   P_1INC(p, NV9097, CALL_MME_MACRO(0));
   if (params.empty()) {
      P_NV9097_CALL_MME_MACRO(p, 0, 0);
   } else {
      P_INLINE_ARRAY(p, &params[0], params.size());
   }

   submit_push();
}

mme_fermi_sim_runner::mme_fermi_sim_runner(uint64_t data_addr)
{
   memset(&info, 0, sizeof(info));
   info.cls_eng3d = FERMI_A;

   memset(data_store, 0, sizeof(data_store));

   this->devinfo = &info;
   this->data_addr = data_addr,
   this->data = data_store;
}

mme_fermi_sim_runner::~mme_fermi_sim_runner()
{ }

void
mme_fermi_sim_runner::run_macro(const std::vector<uint32_t>& macro,
                                const std::vector<uint32_t>& params)
{
   std::vector<mme_fermi_inst> insts(macro.size());
   mme_fermi_decode(&insts[0], &macro[0], macro.size());

   /* First, make a copy of the data and simulate the macro */
   mme_fermi_sim_mem sim_mem = {
      .addr = data_addr,
      .data = data,
      .size = DATA_BO_SIZE,
   };
   mme_fermi_sim(insts.size(), &insts[0],
                 params.size(), &params[0],
                 1, &sim_mem);
}

mme_tu104_sim_runner::mme_tu104_sim_runner(uint64_t data_addr)
{
   memset(&info, 0, sizeof(info));
   info.cls_eng3d = TURING_A;

   memset(data_store, 0, sizeof(data_store));

   this->devinfo = &info;
   this->data_addr = data_addr,
   this->data = data_store;
}

mme_tu104_sim_runner::~mme_tu104_sim_runner()
{ }

void
mme_tu104_sim_runner::run_macro(const std::vector<uint32_t>& macro,
                               const std::vector<uint32_t>& params)
{
   std::vector<mme_tu104_inst> insts(macro.size());
   mme_tu104_decode(&insts[0], &macro[0], macro.size());

   /* First, make a copy of the data and simulate the macro */
   mme_tu104_sim_mem sim_mem = {
      .addr = data_addr,
      .data = data,
      .size = DATA_BO_SIZE,
   };
   mme_tu104_sim(insts.size(), &insts[0],
                 params.size(), &params[0],
                 1, &sim_mem);
}
