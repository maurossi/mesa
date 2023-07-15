
#include "nvk_private.h"
#include "nvk_device.h"
#include "nvk_pipeline.h"

#include "vk_pipeline_cache.h"

struct nvk_pipeline *
nvk_pipeline_zalloc(struct nvk_device *device,
                    enum nvk_pipeline_type type, size_t size,
                    const VkAllocationCallbacks *pAllocator)
{
   struct nvk_pipeline *pipeline;

   assert(size >= sizeof(*pipeline));
   pipeline = vk_object_zalloc(&device->vk, pAllocator, size,
                               VK_OBJECT_TYPE_PIPELINE);
   if (pipeline == NULL)
      return NULL;

   pipeline->type = type;

   return pipeline;
}

void
nvk_pipeline_free(struct nvk_device *device,
                  struct nvk_pipeline *pipeline,
                  const VkAllocationCallbacks *pAllocator)
{
   for (uint32_t s = 0; s < ARRAY_SIZE(pipeline->shaders); s++) {
      if (pipeline->shaders[s].upload_size > 0) {
         nvk_heap_free(device, &device->shader_heap,
                       pipeline->shaders[s].upload_addr,
                       pipeline->shaders[s].upload_size);
      }
      free(pipeline->shaders[s].xfb);
   }

   vk_object_free(&device->vk, pAllocator, pipeline);
}

VKAPI_ATTR VkResult VKAPI_CALL
nvk_CreateGraphicsPipelines(VkDevice _device,
                            VkPipelineCache pipelineCache,
                            uint32_t createInfoCount,
                            const VkGraphicsPipelineCreateInfo *pCreateInfos,
                            const VkAllocationCallbacks *pAllocator,
                            VkPipeline *pPipelines)
{
   VK_FROM_HANDLE(nvk_device, device, _device);
   VK_FROM_HANDLE(vk_pipeline_cache, cache, pipelineCache);
   VkResult result = VK_SUCCESS;

   unsigned i = 0;
   for (; i < createInfoCount; i++) {
      VkResult r = nvk_graphics_pipeline_create(device, cache, &pCreateInfos[i],
                                                pAllocator, &pPipelines[i]);
      if (r == VK_SUCCESS)
         continue;

      result = r;
      pPipelines[i] = VK_NULL_HANDLE;
      if (pCreateInfos[i].flags & VK_PIPELINE_CREATE_EARLY_RETURN_ON_FAILURE_BIT)
         break;
   }

   for (; i < createInfoCount; i++)
      pPipelines[i] = VK_NULL_HANDLE;

   return result;
}

VKAPI_ATTR VkResult VKAPI_CALL
nvk_CreateComputePipelines(VkDevice _device,
                           VkPipelineCache pipelineCache,
                           uint32_t createInfoCount,
                           const VkComputePipelineCreateInfo *pCreateInfos,
                           const VkAllocationCallbacks *pAllocator,
                           VkPipeline *pPipelines)
{
   VK_FROM_HANDLE(nvk_device, device, _device);
   VK_FROM_HANDLE(vk_pipeline_cache, cache, pipelineCache);
   VkResult result = VK_SUCCESS;

   unsigned i = 0;
   for (; i < createInfoCount; i++) {
      VkResult r = nvk_compute_pipeline_create(device, cache, &pCreateInfos[i],
                                               pAllocator, &pPipelines[i]);
      if (r == VK_SUCCESS)
         continue;

      result = r;
      pPipelines[i] = VK_NULL_HANDLE;
      if (pCreateInfos[i].flags & VK_PIPELINE_CREATE_EARLY_RETURN_ON_FAILURE_BIT)
         break;
   }

   for (; i < createInfoCount; i++)
      pPipelines[i] = VK_NULL_HANDLE;

   return result;
}

VKAPI_ATTR void VKAPI_CALL
nvk_DestroyPipeline(VkDevice _device, VkPipeline _pipeline,
                    const VkAllocationCallbacks *pAllocator)
{
   VK_FROM_HANDLE(nvk_device, device, _device);
   VK_FROM_HANDLE(nvk_pipeline, pipeline, _pipeline);

   if (!pipeline)
      return;

   nvk_pipeline_free(device, pipeline, pAllocator);
}
