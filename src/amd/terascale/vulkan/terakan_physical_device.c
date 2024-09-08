/*
 * Copyright © 2023 Vitaliy Triang3l Kuzmin
 *
 * Based in part on radv_physical_device.c which is:
 * Copyright © 2016 Red Hat.
 * Copyright © 2016 Bas Nieuwenhuizen
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

#include "terakan_physical_device.h"

#include "winsys/drm_radeon/terakan_winsys_drm_radeon.h"
#include "terakan_descriptor.h"
#include "terakan_entrypoints.h"
#include "terakan_image.h"
#include "terakan_instance.h"
#include "terakan_limits.h"
#include "terakan_wsi.h"

#include "util/macros.h"
#include "util/u_math.h"
#include "vk_alloc.h"
#include "vk_device.h"
#include "vk_extensions.h"
#include "vk_log.h"
#include "vk_util.h"
#include "wsi_common.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if !defined(_WIN32)
#include <fcntl.h>
#include <xf86drm.h>
#include <sys/stat.h>

#ifdef MAJOR_IN_MKDEV
#include <sys/mkdev.h>
#endif
#ifdef MAJOR_IN_SYSMACROS
#include <sys/sysmacros.h>
#endif
#endif

static void
terakan_physical_device_get_supported_extensions(
   struct vk_device_extension_table * const extensions_out)
{
   memset(extensions_out, 0, sizeof(*extensions_out));

   extensions_out->EXT_4444_formats = true;
   extensions_out->EXT_depth_clip_control = true;
   extensions_out->EXT_depth_clip_enable = true;
   extensions_out->EXT_extended_dynamic_state = true;
   extensions_out->EXT_extended_dynamic_state2 = true;
   extensions_out->EXT_extended_dynamic_state3 = true;
   extensions_out->EXT_provoking_vertex = true;
   extensions_out->KHR_bind_memory2 = true;
   extensions_out->KHR_dedicated_allocation = true;
   extensions_out->KHR_dynamic_rendering = true;
   extensions_out->KHR_external_memory = true;
   extensions_out->KHR_format_feature_flags2 = true;
   extensions_out->KHR_map_memory2 = true;
   extensions_out->KHR_timeline_semaphore = true;

#if !defined(_WIN32)
   extensions_out->EXT_external_memory_dma_buf = true;
   extensions_out->EXT_pci_bus_info = true;
   extensions_out->EXT_physical_device_drm = true;
   extensions_out->KHR_external_memory_fd = true;
#endif

#ifdef TERAKAN_USE_WSI_PLATFORM
   extensions_out->KHR_swapchain = true;
   extensions_out->KHR_swapchain_mutable_format = true;
#endif
}

static void
terakan_physical_device_get_supported_features(struct vk_features * const features_out)
{
   memset(features_out, 0, sizeof(*features_out));

   /* Vulkan 1.0 */
   features_out->robustBufferAccess = true;
   features_out->fullDrawIndexUint32 = true;
   features_out->depthClamp = true;
   features_out->fillModeNonSolid = true;
   features_out->textureCompressionBC = true;

   /* VK_EXT_4444_formats */
   features_out->formatA4R4G4B4 = true;
   features_out->formatA4B4G4R4 = true;

   /* VK_EXT_depth_clip_control */
   features_out->depthClipControl = true;

   /* VK_EXT_depth_clip_enable */
   features_out->depthClipEnable = true;

   /* VK_EXT_extended_dynamic_state3 */
   features_out->extendedDynamicState3DepthClampEnable = true;
   features_out->extendedDynamicState3PolygonMode = true;
   features_out->extendedDynamicState3SampleMask = true;
   features_out->extendedDynamicState3ProvokingVertexMode = true;
   features_out->extendedDynamicState3DepthClipEnable = true;
   features_out->extendedDynamicState3DepthClipNegativeOneToOne = true;

   /* VK_EXT_provoking_vertex */
   features_out->provokingVertexLast = true;
   features_out->transformFeedbackPreservesProvokingVertex = true;

   /* VK_KHR_dynamic_rendering */
   features_out->dynamicRendering = true;

   /* VK_KHR_timeline_semaphore */
   features_out->timelineSemaphore = true;
}

static void
terakan_physical_device_init_memory_properties(struct terakan_physical_device * const device)
{
   /* Based on radv_physical_device_init_mem_types. */

   /* TODO(Triang3l): override_vram_size DRI option. */

   VkPhysicalDeviceMemoryProperties * const memory_properties = &device->memory_properties;
   struct terakan_gpu_info const * const gpu_info = &device->winsys->gpu_info;

   uint64_t gart_size = gpu_info->gart_size;
   uint64_t vram_visible_size = MIN2(gpu_info->vram_size, gpu_info->vram_visible);
   uint64_t vram_not_visible_size = gpu_info->vram_size - vram_visible_size;

   if (!gpu_info->has_dedicated_vram) {
      uint64_t const total_size = gart_size + vram_visible_size;
      /* TODO(Triang3l): enable_unified_heap_on_apu. */
      /* On APUs, the carveout is usually too small for games that request a minimum VRAM size
       * greater than it. To workaround this, we compute the total available memory size (GART +
       * visible VRAM size) and report 2/3 as VRAM and 1/3 as GART (so games don't assume that the
       * entire system memory is VRAM and occupy it like it doesn't affect memory available to the
       * CPU).
       */
      vram_visible_size = align64((total_size * 2) / 3, (uint64_t)gpu_info->gart_page_size);
      gart_size = total_size - vram_visible_size;
      vram_not_visible_size = 0;
   }

   memory_properties->memoryHeapCount = 0;
   /* Only get a VRAM heap if it is significant, not if it is a 16 MiB remainder above visible
    * VRAM.
    */
   uint32_t heap_index_vram_not_visible = UINT32_MAX;
   if (vram_not_visible_size > 0 && vram_not_visible_size * 9 >= vram_visible_size) {
      heap_index_vram_not_visible = memory_properties->memoryHeapCount++;
      VkMemoryHeap * const heap_vram_not_visible =
         &memory_properties->memoryHeaps[heap_index_vram_not_visible];
      heap_vram_not_visible->size = vram_not_visible_size;
      heap_vram_not_visible->flags = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
   }
   uint32_t heap_index_gart = UINT32_MAX;
   if (gart_size > 0) {
      heap_index_gart = memory_properties->memoryHeapCount++;
      VkMemoryHeap * const heap_gart = &memory_properties->memoryHeaps[heap_index_gart];
      heap_gart->size = gart_size;
      heap_gart->flags = 0;
   }
   uint32_t heap_index_vram_visible = UINT32_MAX;
   if (vram_visible_size > 0) {
      heap_index_vram_visible = memory_properties->memoryHeapCount++;
      VkMemoryHeap * const heap_vram_visible =
         &memory_properties->memoryHeaps[heap_index_vram_visible];
      heap_vram_visible->size = vram_visible_size;
      heap_vram_visible->flags = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
   }

   memory_properties->memoryTypeCount = 0;
   if (heap_index_vram_not_visible != UINT32_MAX || heap_index_vram_visible != UINT32_MAX) {
      VkMemoryType * const type_vram =
         &memory_properties->memoryTypes[memory_properties->memoryTypeCount++];
      type_vram->propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      type_vram->heapIndex = heap_index_vram_not_visible != UINT32_MAX ? heap_index_vram_not_visible
                                                                       : heap_index_vram_visible;
   }
   if (heap_index_gart != UINT32_MAX) {
      VkMemoryType * const type_gart_wc =
         &memory_properties->memoryTypes[memory_properties->memoryTypeCount++];
      type_gart_wc->propertyFlags =
         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      type_gart_wc->heapIndex = heap_index_gart;
   }
   if (heap_index_vram_visible != UINT32_MAX) {
      VkMemoryType * const type_vram_visible =
         &memory_properties->memoryTypes[memory_properties->memoryTypeCount++];
      type_vram_visible->propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      type_vram_visible->heapIndex = heap_index_vram_visible;
   }
   if (heap_index_gart != UINT32_MAX) {
      VkMemoryType * const type_gart_cached =
         &memory_properties->memoryTypes[memory_properties->memoryTypeCount++];
      type_gart_cached->propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                                        VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
      type_gart_cached->heapIndex = heap_index_gart;
   }
}

VKAPI_ATTR void VKAPI_CALL
terakan_GetPhysicalDeviceMemoryProperties2(
   VkPhysicalDevice const physicalDevice,
   VkPhysicalDeviceMemoryProperties2 * const pMemoryProperties)
{
   struct terakan_physical_device const * const device =
      terakan_physical_device_from_handle(physicalDevice);

   pMemoryProperties->memoryProperties = device->memory_properties;
}

VKAPI_ATTR void VKAPI_CALL
terakan_GetPhysicalDeviceProperties(VkPhysicalDevice const physicalDevice,
                                    VkPhysicalDeviceProperties * const pProperties)
{
   struct terakan_physical_device const * const device =
      terakan_physical_device_from_handle(physicalDevice);
   struct terakan_instance const * const instance =
      container_of(device->vk.instance, struct terakan_instance const, vk);
   struct terakan_gpu_info const * const gpu_info = &device->winsys->gpu_info;

   pProperties->apiVersion = TERAKAN_API_VERSION;
   /* TODO(Triang3l): Change to vk_get_driver_version() when Vulkan 1.0 compatibility is achieved.
    */
   pProperties->driverVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
   pProperties->vendorID = TERAKAN_ATI_VENDOR_ID;
   pProperties->deviceID = gpu_info->pci_id;
   pProperties->deviceType = gpu_info->has_dedicated_vram ? VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
                                                          : VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
   snprintf(pProperties->deviceName, sizeof(pProperties->deviceName), "AMD R%cxx %s (Terakan)",
            gpu_info->gfx_level >= CAYMAN ? '9' : '8', gpu_info->chip_family_name);
   /* TODO(Triang3l): pipelineCacheUUID when pipeline cache is implemented. */
   memset(pProperties->pipelineCacheUUID, 0, sizeof(pProperties->pipelineCacheUUID));
   memset(&pProperties->sparseProperties, 0, sizeof(pProperties->sparseProperties));

   VkPhysicalDeviceLimits * const limits = &pProperties->limits;
   memset(limits, 0, sizeof(*limits));

   limits->maxImageDimension1D = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   limits->maxImageDimension2D = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   limits->maxImageDimension3D = TERAKAN_IMAGE_MAX_TARGET_SLICES;
   limits->maxImageDimensionCube = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   limits->maxImageArrayLayers = TERAKAN_IMAGE_MAX_TARGET_SLICES;

   /* Vertex fetch constants have 32-bit size minus one in bytes.
    * Random access targets have 32-bit size minus one in elements.
    * Support the maximum possible number of R32G32B32A32 elements.
    */
   limits->maxTexelBufferElements = (uint32_t)1 << (32 - 4);

   limits->maxUniformBufferRange = TERAKAN_CONSTANT_CACHE_MAX_BUFFER_SIZE_BYTES;

   /* Storage buffers are bound as R32 vertex fetch constants or random access targets. */
   limits->maxStorageBufferRange = UINT32_MAX & ~(uint32_t)(sizeof(uint32_t) - 1);

   /* TODO(Triang3l): Exclude internal constants like the draw ID, sample locations. */
   limits->maxPushConstantsSize = TERAKAN_CONSTANT_CACHE_MAX_BUFFER_SIZE_BYTES;

   limits->maxMemoryAllocationCount = UINT32_MAX;

   limits->maxSamplerAllocationCount = UINT32_MAX;

   limits->bufferImageGranularity = 1;

   /* At most 1 descriptor set per binding is useful, but descriptor sets can be empty, so no limit.
    */
   limits->maxBoundDescriptorSets = UINT32_MAX;

   limits->maxPerStageDescriptorSamplers = TERAKAN_SAMPLERS_PER_STAGE;
   limits->maxPerStageDescriptorUniformBuffers = instance->max_per_stage_uniform_buffers;
   limits->maxPerStageDescriptorStorageBuffers = instance->max_per_stage_storage_buffers;
   limits->maxPerStageDescriptorSampledImages = instance->max_per_stage_sampled_images;
   limits->maxPerStageDescriptorStorageImages =
      TERAKAN_LIMITS_HW_COLOR_RAT_COUNT - instance->max_per_stage_storage_buffers;
   limits->maxPerStageDescriptorInputAttachments = instance->max_per_stage_input_attachments;
   limits->maxColorAttachments = TERAKAN_LIMITS_HW_COLOR_MRT_COUNT;

   limits->maxPerStageResources =
      limits->maxPerStageDescriptorUniformBuffers + limits->maxPerStageDescriptorStorageBuffers +
      limits->maxPerStageDescriptorSampledImages + limits->maxPerStageDescriptorStorageImages +
      limits->maxPerStageDescriptorInputAttachments + limits->maxColorAttachments;

   uint32_t const shader_stage_count = 6;
   limits->maxDescriptorSetSamplers = limits->maxPerStageDescriptorSamplers * shader_stage_count;
   limits->maxDescriptorSetUniformBuffers =
      limits->maxPerStageDescriptorUniformBuffers * shader_stage_count;
   limits->maxDescriptorSetUniformBuffersDynamic = limits->maxDescriptorSetUniformBuffers;
   limits->maxDescriptorSetStorageBuffers =
      limits->maxPerStageDescriptorStorageBuffers * shader_stage_count;
   limits->maxDescriptorSetStorageBuffersDynamic = limits->maxDescriptorSetStorageBuffers;
   limits->maxDescriptorSetSampledImages =
      limits->maxPerStageDescriptorSampledImages * shader_stage_count;
   limits->maxDescriptorSetStorageImages =
      limits->maxPerStageDescriptorStorageImages * shader_stage_count;
   limits->maxDescriptorSetInputAttachments = limits->maxPerStageDescriptorInputAttachments;

   limits->maxVertexInputAttributes = TERAKAN_RESOURCE_HW_COUNT_FETCH;
   limits->maxVertexInputBindings = TERAKAN_RESOURCE_HW_COUNT_FETCH;
   limits->maxVertexInputAttributeOffset = UINT32_MAX;
   /* NON-CONFORMANT: R8xx has 11 bits for the stride in bytes, which can store values up to 2047.
    * Vulkan requires at least 2048. R9xx has 12 bits.
    */
   /* TODO(Triang3l): Research a workaround scaling the index (in a saturating way to maintain
    * defined overflow behavior) in the fetch shader.
    */
   limits->maxVertexInputBindingStride =
      ((uint32_t)1 << (gpu_info->gfx_level >= CAYMAN ? 12 : 11)) - 1;

   limits->maxVertexOutputComponents = 4 * TERAKAN_LIMITS_HW_PARAMETER_CACHE_VECTOR_COUNT;

   /* TODO(Triang3l): Tessellation limits. */

   /* TODO(Triang3l): Geometry shader limits. */

   limits->maxFragmentInputComponents = 4 * TERAKAN_LIMITS_HW_PARAMETER_CACHE_VECTOR_COUNT;

   limits->maxFragmentOutputAttachments = TERAKAN_LIMITS_HW_COLOR_MRT_COUNT;
   /* TODO(Triang3l): maxFragmentDualSrcAttachments when dual-source blending is enabled. */
   limits->maxFragmentCombinedOutputResources =
      MAX2(TERAKAN_LIMITS_HW_COLOR_MRT_COUNT, TERAKAN_LIMITS_HW_COLOR_RAT_COUNT);

   limits->maxComputeSharedMemorySize = sizeof(uint32_t) * TERAKAN_LIMITS_HW_LDS_SIMD_DWORD_COUNT;

   limits->maxComputeWorkGroupCount[0] = TERAKAN_LIMITS_HW_COMPUTE_GROUPS_PER_DIMENSION;
   limits->maxComputeWorkGroupCount[1] = TERAKAN_LIMITS_HW_COMPUTE_GROUPS_PER_DIMENSION;
   limits->maxComputeWorkGroupCount[2] = TERAKAN_LIMITS_HW_COMPUTE_GROUPS_PER_DIMENSION;
   limits->maxComputeWorkGroupInvocations = TERAKAN_LIMITS_HW_COMPUTE_GROUP_SIZE;
   limits->maxComputeWorkGroupSize[0] = TERAKAN_LIMITS_HW_COMPUTE_GROUP_SIZE;
   limits->maxComputeWorkGroupSize[1] = TERAKAN_LIMITS_HW_COMPUTE_GROUP_SIZE;
   limits->maxComputeWorkGroupSize[2] = TERAKAN_LIMITS_HW_COMPUTE_GROUP_SIZE;

   limits->subPixelPrecisionBits = 8;
   limits->subTexelPrecisionBits = 8;
   limits->mipmapPrecisionBits = 8;

   limits->maxDrawIndexedIndexValue = UINT32_MAX;

   /* TODO(Triang3l): maxDrawIndirectCount when indirect drawing is enabled. */

   limits->maxSamplerLodBias = 32.0f - 1.0f / 256.0f;
   /* TODO(Triang3l): maxSamplerAnisotropy when anisotropic filtering is enabled. */

   limits->maxViewports = TERAKAN_LIMITS_HW_VIEWPORTS;
   limits->maxViewportDimensions[0] = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   limits->maxViewportDimensions[1] = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   limits->viewportBoundsRange[0] = (float)INT16_MIN;
   limits->viewportBoundsRange[1] = (float)INT16_MAX;
   limits->viewportSubPixelBits = 8;

   limits->minMemoryMapAlignment = gpu_info->min_memory_map_alignment;

   /* The largest is for R32G32B32A32 random access targets, which must have element-aligned base
    * addresses.
    */
   /* TODO(Triang3l): Research what "element-aligned" means in CB_COLOR[0-11]_VIEW.SLICE_START
    * documentation, is it "buffer element" (thus texel-aligned), or "structure element" (thus
    * 4-aligned at most, like for vertex elements) as specified in "4.4.6 Element Alignment" of the
    * Direct3D 11.3 Functional Specification.
    */
   /* TODO(Triang3l): VK_EXT_texel_buffer_alignment properties:
    * storageTexelBufferOffsetAlignmentBytes = 16 (RAT element).
    * storageTexelBufferOffsetSingleTexelAlignment = VK_TRUE.
    * uniformTexelBufferOffsetAlignmentBytes = 4 (vertex element).
    * uniformTexelBufferOffsetSingleTexelAlignment = VK_TRUE.
    */
   limits->minTexelBufferOffsetAlignment = sizeof(uint32_t) * 4;

   limits->minUniformBufferOffsetAlignment = TERAKAN_CONSTANT_CACHE_LINE_BYTES;

   limits->minStorageBufferOffsetAlignment = sizeof(uint32_t);

   limits->minTexelOffset = -8;
   limits->maxTexelOffset = 8;

   /* TODO(Triang3l): Texel gather offset range when extended image gather is enabled (need to
    * research the range given that the offsets come from a GPR vector).
    */

   /* TODO(Triang3l): Interpolation offset properties when sample-rate shading is enabled. */

   limits->maxFramebufferWidth = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   limits->maxFramebufferHeight = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   limits->maxFramebufferLayers = TERAKAN_IMAGE_MAX_TARGET_SLICES;

   VkSampleCountFlags const sample_counts =
      VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_2_BIT | VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_8_BIT;

   limits->framebufferColorSampleCounts = sample_counts;
   limits->framebufferDepthSampleCounts = sample_counts;
   limits->framebufferStencilSampleCounts = sample_counts;
   limits->framebufferNoAttachmentsSampleCounts = sample_counts;
   if (gpu_info->gfx_level >= CAYMAN) {
      limits->framebufferNoAttachmentsSampleCounts |= VK_SAMPLE_COUNT_16_BIT;
   }

   limits->sampledImageColorSampleCounts = sample_counts;
   limits->sampledImageIntegerSampleCounts = sample_counts;
   limits->sampledImageDepthSampleCounts = sample_counts;
   limits->sampledImageStencilSampleCounts = sample_counts;

   limits->storageImageSampleCounts = VK_SAMPLE_COUNT_1_BIT;

   limits->maxSampleMaskWords = 1;

   if (gpu_info->clock_crystal_frequency != 0) {
      limits->timestampComputeAndGraphics = VK_TRUE;
      limits->timestampPeriod = (float)(1000000.0 / (double)gpu_info->clock_crystal_frequency);
   }

   /* TODO(Triang3l): Maximum clip and cull distances when enabled. */

   limits->discreteQueuePriorities = 2;

   /* TODO(Triang3l): Point size when enabled. */
   /* TODO(Triang3l): Line width when wide lines are enabled. */

   /* TODO(Triang3l): Research strict lines. */

   limits->standardSampleLocations = VK_TRUE;

   limits->optimalBufferCopyOffsetAlignment = 1;
   limits->optimalBufferCopyRowPitchAlignment = 1;

   /* No non-coherent host-visible memory types.
    * Otherwise sysconf(_SC_LEVEL1_DCACHE_LINESIZE) on Linux.
    */
   limits->nonCoherentAtomSize = 1;
}

static void
terakan_physical_device_get_properties_1_1(struct terakan_physical_device const * const device,
                                           uint32_t const max_per_set_descriptors,
                                           VkPhysicalDeviceVulkan11Properties * const properties)
{
   assert(properties->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES);

   memcpy(properties->deviceUUID, device->device_uuid, sizeof(properties->deviceUUID));

   char const driver_uuid[] = "AMD-MESA-DRV";
   static_assert(sizeof(driver_uuid) <= sizeof(properties->driverUUID),
                 "Driver UUID must fit into the Vulkan UUID field.");
   memcpy(properties->driverUUID, driver_uuid, sizeof(driver_uuid));
   memset(properties->driverUUID + sizeof(driver_uuid), 0,
          sizeof(properties->driverUUID) - sizeof(driver_uuid));

   memset(properties->deviceLUID, 0, sizeof(properties->deviceLUID));
   properties->deviceNodeMask = 0;
   properties->deviceLUIDValid = VK_FALSE;

   properties->subgroupSize = 64;
   properties->subgroupSupportedStages = 0;
   properties->subgroupSupportedOperations = 0;
   properties->subgroupQuadOperationsInAllStages = VK_FALSE;

   properties->pointClippingBehavior = VK_POINT_CLIPPING_BEHAVIOR_ALL_CLIP_PLANES;

   properties->maxMultiviewViewCount = 1;
   properties->maxMultiviewInstanceIndex = UINT32_MAX;

   properties->protectedNoFault = VK_FALSE;

   properties->maxPerSetDescriptors = max_per_set_descriptors;

   properties->maxMemoryAllocationSize = device->winsys->gpu_info.max_bo_size;
}

VKAPI_ATTR void VKAPI_CALL
terakan_GetPhysicalDeviceProperties2(VkPhysicalDevice const physicalDevice,
                                     VkPhysicalDeviceProperties2 * const pProperties)
{
   struct terakan_physical_device const * const device =
      terakan_physical_device_from_handle(physicalDevice);

   terakan_GetPhysicalDeviceProperties(physicalDevice, &pProperties->properties);

   VkPhysicalDeviceVulkan11Properties core_1_1;
   core_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
   core_1_1.pNext = NULL;
   terakan_physical_device_get_properties_1_1(
      device, pProperties->properties.limits.maxBoundDescriptorSets, &core_1_1);

   vk_foreach_struct(ext, pProperties->pNext)
   {
      if (vk_get_physical_device_core_1_1_property_ext(ext, &core_1_1)) {
         continue;
      }

      switch (ext->sType) {
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_PROPERTIES_EXT: {
         VkPhysicalDeviceExtendedDynamicState3PropertiesEXT * const properties =
            (VkPhysicalDeviceExtendedDynamicState3PropertiesEXT *)ext;
         properties->dynamicPrimitiveTopologyUnrestricted = VK_TRUE;
      } break;

#if !defined(_WIN32)
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRM_PROPERTIES_EXT: {
         VkPhysicalDeviceDrmPropertiesEXT * const properties =
            (VkPhysicalDeviceDrmPropertiesEXT *)ext;
         if (device->drm_available_nodes & (1 << DRM_NODE_PRIMARY)) {
            properties->hasPrimary = VK_TRUE;
            properties->primaryMajor = (int64_t)major(device->drm_primary_devid);
            properties->primaryMinor = (int64_t)minor(device->drm_primary_devid);
         } else {
            properties->hasPrimary = VK_FALSE;
         }
         if (device->drm_available_nodes & (1 << DRM_NODE_RENDER)) {
            properties->hasRender = VK_TRUE;
            properties->renderMajor = (int64_t)major(device->drm_render_devid);
            properties->renderMinor = (int64_t)minor(device->drm_render_devid);
         } else {
            properties->hasRender = VK_FALSE;
         }
      } break;
#endif

#if !defined(_WIN32)
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT: {
         VkPhysicalDevicePCIBusInfoPropertiesEXT * const properties =
            (VkPhysicalDevicePCIBusInfoPropertiesEXT *)ext;
         properties->pciDomain = device->drm_bus_info.domain;
         properties->pciBus = device->drm_bus_info.bus;
         properties->pciDevice = device->drm_bus_info.dev;
         properties->pciFunction = device->drm_bus_info.func;
      } break;
#endif

      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROVOKING_VERTEX_PROPERTIES_EXT: {
         VkPhysicalDeviceProvokingVertexPropertiesEXT * const properties =
            (VkPhysicalDeviceProvokingVertexPropertiesEXT *)ext;
         properties->provokingVertexModePerPipeline = VK_TRUE;
         properties->transformFeedbackPreservesTriangleFanProvokingVertex = VK_TRUE;
      } break;

      /* TODO(Triang3l): Move to VkPhysicalDeviceVulkan12Properties. */
      case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES: {
         VkPhysicalDeviceTimelineSemaphoreProperties * const properties =
            (VkPhysicalDeviceTimelineSemaphoreProperties *)ext;
         properties->maxTimelineSemaphoreValueDifference = UINT64_MAX;
      } break;

      default:
         break;
      }
   }
}

static void
terakan_physical_device_get_queue_family_properties(
   struct terakan_physical_device const * const device, uint32_t * const count,
   VkQueueFamilyProperties * const * const properties_out)
{
   uint32_t const queue_family_count = 1;

   if (properties_out == NULL) {
      *count = queue_family_count;
      return;
   }

   if (*count == 0) {
      return;
   }

   uint32_t const timestamp_valid_bits =
      device->winsys->gpu_info.clock_crystal_frequency != 0 ? 64 : 0;

   uint32_t next_index = 0;

   if (*count > next_index) {
      assert(next_index < queue_family_count);
      VkQueueFamilyProperties * const queue_family_graphics = properties_out[next_index++];
      queue_family_graphics->queueFlags =
         VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
      queue_family_graphics->queueCount = 1;
      queue_family_graphics->timestampValidBits = timestamp_valid_bits;
      queue_family_graphics->minImageTransferGranularity.width = 1;
      queue_family_graphics->minImageTransferGranularity.height = 1;
      queue_family_graphics->minImageTransferGranularity.depth = 1;
   }

   *count = next_index;
}

VKAPI_ATTR void VKAPI_CALL
terakan_GetPhysicalDeviceQueueFamilyProperties2(
   VkPhysicalDevice const physicalDevice, uint32_t * const pQueueFamilyPropertyCount,
   VkQueueFamilyProperties2 * const pQueueFamilyProperties)
{
   struct terakan_physical_device const * const device =
      terakan_physical_device_from_handle(physicalDevice);

   if (pQueueFamilyProperties == NULL) {
      terakan_physical_device_get_queue_family_properties(device, pQueueFamilyPropertyCount, NULL);
      return;
   }

   VkQueueFamilyProperties * const properties[] = {
      &pQueueFamilyProperties[0].queueFamilyProperties,
   };
   terakan_physical_device_get_queue_family_properties(device, pQueueFamilyPropertyCount,
                                                       properties);
   assert(*pQueueFamilyPropertyCount <= ARRAY_SIZE(properties));
}

void
terakan_physical_device_destroy(struct vk_physical_device * const device_base)
{
   struct terakan_physical_device * const device =
      container_of(device_base, struct terakan_physical_device, vk);

   terakan_wsi_finish(device);

   device->winsys->fn->destroy(device->winsys);

#if !defined(_WIN32)
   close(device->local_fd);
#endif

   vk_physical_device_finish(&device->vk);

   vk_free(&device->vk.instance->alloc, device);
}

VkResult
terakan_physical_device_try_create_for_drm(struct vk_instance * const instance_base,
                                           struct _drmDevice * const drm_device,
                                           struct vk_physical_device ** const device_out)
{
#if defined(_WIN32)
   return vk_errorf(instance, VK_ERROR_INCOMPATIBLE_DRIVER,
                    "Radeon Software D3DKMT winsys is not supported currently");

#else
   if (!(drm_device->available_nodes & (1 << DRM_NODE_RENDER)) ||
       drm_device->bustype != DRM_BUS_PCI ||
       drm_device->deviceinfo.pci->vendor_id != TERAKAN_ATI_VENDOR_ID) {
      return VK_ERROR_INCOMPATIBLE_DRIVER;
   }

   VkResult result;

   struct terakan_instance * const instance =
      container_of(instance_base, struct terakan_instance, vk);

   char const * const render_node_path = drm_device->nodes[DRM_NODE_RENDER];
   int const fd = open(render_node_path, O_RDWR | O_CLOEXEC);
   if (fd < 0) {
      return vk_errorf(instance,
                       errno == ENOMEM ? VK_ERROR_OUT_OF_HOST_MEMORY : VK_ERROR_INCOMPATIBLE_DRIVER,
                       "Failed to open the DRM device '%s': %m", render_node_path);
   }
   {
      drmVersionPtr const drm_version = drmGetVersion(fd);
      if (drm_version == NULL) {
         close(fd);
         return vk_errorf(instance, VK_ERROR_INCOMPATIBLE_DRIVER,
                          "Failed to get the kernel driver version for the DRM device '%s': %m",
                          render_node_path);
      }
      if (strcmp(drm_version->name, "radeon") != 0) {
         drmFreeVersion(drm_version);
         close(fd);
         return VK_ERROR_INCOMPATIBLE_DRIVER;
      }
      drmFreeVersion(drm_version);
   }
   if (instance->debug_flags & TERAKAN_DEBUG_STARTUP) {
      fprintf(stderr, "terakan: info: Found a compatible DRM device '%s'.\n", render_node_path);
   }

   struct terakan_physical_device * const device =
      vk_alloc2(&instance->vk.alloc, NULL, sizeof(struct terakan_physical_device),
                alignof(struct terakan_physical_device), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (device == NULL) {
      result = vk_error(instance, VK_ERROR_OUT_OF_HOST_MEMORY);
      goto fail_fd;
   }

   struct vk_physical_device_dispatch_table dispatch_table;
   vk_physical_device_dispatch_table_from_entrypoints(&dispatch_table,
                                                      &terakan_physical_device_entrypoints, true);
   vk_physical_device_dispatch_table_from_entrypoints(&dispatch_table,
                                                      &wsi_physical_device_entrypoints, false);

   result = vk_physical_device_init(&device->vk, &instance->vk, NULL, NULL, NULL, &dispatch_table);
   if (result != VK_SUCCESS) {
      goto fail_alloc;
   }

   device->local_fd = fd;

   device->winsys = terakan_winsys_drm_radeon_create(fd);
   if (device->winsys == NULL) {
      /* Failed to initialize the winsys, or not a compatible ATI/AMD device. */
      result = VK_ERROR_INCOMPATIBLE_DRIVER;
      goto fail_device;
   }

   /* See "NON-CONFORMANT:" comments throughout the code for more details. */
   vk_warn_non_conformant_implementation("terakan");

   device->drm_available_nodes = drm_device->available_nodes;
   device->drm_bus_info = *drm_device->businfo.pci;
   if (drm_device->available_nodes & (1 << DRM_NODE_PRIMARY)) {
      struct stat primary_stat = {0};
      if (stat(drm_device->nodes[DRM_NODE_PRIMARY], &primary_stat) != 0) {
         result = vk_errorf(instance, VK_ERROR_INITIALIZATION_FAILED,
                            "Failed to stat the DRM primary node '%s'",
                            drm_device->nodes[DRM_NODE_PRIMARY]);
         goto fail_device;
      }
      device->drm_primary_devid = primary_stat.st_rdev;
   } else {
      device->drm_primary_devid = 0;
   }
   if (drm_device->available_nodes & (1 << DRM_NODE_RENDER)) {
      struct stat render_stat = {0};
      if (stat(drm_device->nodes[DRM_NODE_RENDER], &render_stat) != 0) {
         result = vk_errorf(instance, VK_ERROR_INITIALIZATION_FAILED,
                            "Failed to stat the DRM render node '%s'",
                            drm_device->nodes[DRM_NODE_RENDER]);
         goto fail_device;
      }
      device->drm_render_devid = render_stat.st_rdev;
   } else {
      device->drm_render_devid = 0;
   }

   device->vk.supported_sync_types = device->winsys->fn->get_sync_types(device->winsys);

   /* Same as ac_compute_device_uuid as of July 2023. */
   uint32_t const device_uuid_u32[] = {
      device->drm_bus_info.domain,
      device->drm_bus_info.bus,
      device->drm_bus_info.dev,
      device->drm_bus_info.func,
   };
   static_assert(sizeof(device_uuid_u32) <= sizeof(device->device_uuid),
                 "Computed device UUID must fit into the Vulkan UUID field.");
   memcpy(device->device_uuid, device_uuid_u32, sizeof(device_uuid_u32));
   memset(device->device_uuid + sizeof(device_uuid_u32), 0,
          sizeof(device->device_uuid) - sizeof(device_uuid_u32));

   terakan_physical_device_init_memory_properties(device);

   terakan_physical_device_get_supported_extensions(&device->vk.supported_extensions);

   terakan_physical_device_get_supported_features(&device->vk.supported_features);

   /* Initialize WSI after everything else as it's a layer on top of the Vulkan physical device. */
   result = terakan_wsi_init(device);
   if (result != VK_SUCCESS) {
      vk_error(instance, result);
      goto fail_winsys;
   }

   *device_out = &device->vk;

   return VK_SUCCESS;

fail_winsys:
   device->winsys->fn->destroy(device->winsys);
fail_device:
   vk_physical_device_finish(&device->vk);
fail_alloc:
   vk_free(&instance->vk.alloc, device);
fail_fd:
   close(fd);
   return result;
#endif
}
