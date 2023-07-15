#ifndef NVK_DEVICE_H
#define NVK_DEVICE_H 1

#include "nvk_private.h"

#include "nvk_descriptor_table.h"
#include "nvk_queue.h"
#include "vk_device.h"
#include "vk_meta.h"
#include "vk_queue.h"

struct novueau_ws_context;
struct nvk_physical_device;

struct nvk_slm_area {
   simple_mtx_t mutex;
   struct nouveau_ws_bo *bo;
   uint32_t bytes_per_warp;
   uint32_t bytes_per_mp;
};

struct nouveau_ws_bo *
nvk_slm_area_get_bo_ref(struct nvk_slm_area *area,
                        uint32_t *bytes_per_warp_out,
                        uint32_t *bytes_per_mp_out);

struct nvk_device {
   struct vk_device vk;
   struct nvk_physical_device *pdev;

   struct nouveau_ws_context *ctx;

   simple_mtx_t memory_objects_lock;
   struct list_head memory_objects;

   struct nvk_descriptor_table images;
   struct nvk_descriptor_table samplers;
   struct nvk_slm_area slm;

   struct nvk_queue queue;

   pthread_mutex_t mutex;
   pthread_cond_t queue_submit;

   struct vk_meta_device meta;
};

VK_DEFINE_HANDLE_CASTS(nvk_device, vk.base, VkDevice, VK_OBJECT_TYPE_DEVICE)

VkResult nvk_device_ensure_slm(struct nvk_device *dev,
                               uint32_t bytes_per_thread);

static struct nvk_physical_device *
nvk_device_physical(struct nvk_device *device)
{
   return (struct nvk_physical_device *)device->vk.physical;
}

VkResult nvk_device_init_meta(struct nvk_device *dev);
void nvk_device_finish_meta(struct nvk_device *dev);

#endif
