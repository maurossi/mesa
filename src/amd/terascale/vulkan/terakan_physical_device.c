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

#include "terakan_descriptor.h"
#include "terakan_entrypoints.h"
#include "terakan_hw_state.h"
#include "terakan_image.h"
#include "terakan_instance.h"
#include "terakan_limits.h"
#include "terakan_wsi.h"

#include "compiler/shader_enums.h"
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

enum radeon_family
terakan_physical_device_get_chip_family(uint32_t const pci_device_id)
{
   /* CHIP_UNKNOWN is not an error, as this function is used in filtering out unsupported physical
    * devices.
    */
   enum radeon_family chip_family = CHIP_UNKNOWN;
   switch (pci_device_id) {
#define CHIPSET(chipset_pci_id, chipset_name, chipset_family)                                      \
   case chipset_pci_id:                                                                            \
      chip_family = CHIP_##chipset_family;                                                         \
      break;
#include "pci_ids/r600_pci_ids.h"
#undef CHIPSET
   }
   return chip_family;
}

char const *
terakan_physical_device_chip_family_name(enum radeon_family const chip_family)
{
   switch (chip_family) {
   case CHIP_CEDAR:
      return "Cedar";
   case CHIP_REDWOOD:
      return "Redwood";
   case CHIP_JUNIPER:
      return "Juniper";
   case CHIP_CYPRESS:
      return "Cypress";
   case CHIP_HEMLOCK:
      return "Hemlock";
   case CHIP_PALM:
      return "Palm";
   case CHIP_SUMO:
      return "Sumo";
   case CHIP_SUMO2:
      return "SuperSumo";
   case CHIP_BARTS:
      return "Barts";
   case CHIP_TURKS:
      return "Turks";
   case CHIP_CAICOS:
      return "Caicos";
   case CHIP_CAYMAN:
      return "Cayman";
   case CHIP_ARUBA:
      return "Aruba";
   default:
      assert(!"Unsupported GFX chip family");
      return "Evergreen/Northern Islands";
   }
}

void
terakan_physical_device_chip_family_info_init(
   enum radeon_family const chip_family,
   struct terakan_physical_device_chip_family_info * const chip_family_info_out)
{
   assert(terakan_physical_device_is_chip_family_supported(chip_family));

   chip_family_info_out->chip_family = chip_family;
   bool const is_r9xx = chip_family >= CHIP_CAYMAN;
   chip_family_info_out->is_r9xx = is_r9xx;

   switch (chip_family) {
   case CHIP_PALM:
   case CHIP_SUMO:
   case CHIP_SUMO2:
   case CHIP_ARUBA:
      chip_family_info_out->has_dedicated_vram = false;
      break;
   default:
      chip_family_info_out->has_dedicated_vram = true;
   }

   switch (chip_family) {
   case CHIP_CEDAR:
   case CHIP_PALM:
   case CHIP_SUMO:
   case CHIP_SUMO2:
   case CHIP_CAICOS:
      chip_family_info_out->has_vertex_cache = false;
      break;
   default:
      /* R9xx vertex fetch always goes through the texture cache, but Linux Radeon 2.50.0 and the
       * Gallium R600 driver set SQ_CONFIG.VC_ENABLE to 1 on it.
       */
      chip_family_info_out->has_vertex_cache = true;
   }

   if (is_r9xx) {
      chip_family_info_out->sq_max_threads = 256;
      chip_family_info_out->sq_ps_threads_r8xx = 0;
   } else {
      switch (chip_family) {
      case CHIP_CEDAR:
      case CHIP_PALM:
      case CHIP_CAICOS:
         chip_family_info_out->sq_max_threads = 192;
         break;
      default:
         chip_family_info_out->sq_max_threads = 248;
      }

      switch (chip_family) {
      case CHIP_CEDAR:
      case CHIP_PALM:
      case CHIP_SUMO:
      case CHIP_SUMO2:
         chip_family_info_out->sq_ps_threads_r8xx = 96;
         break;
      default:
         chip_family_info_out->sq_ps_threads_r8xx = 128;
      }
   }

   switch (chip_family) {
   case CHIP_CEDAR:
   case CHIP_REDWOOD:
   case CHIP_PALM:
   case CHIP_SUMO:
   case CHIP_TURKS:
   case CHIP_CAICOS:
      chip_family_info_out->sq_max_stack_entries = 256;
      break;
   default:
      chip_family_info_out->sq_max_stack_entries = 512;
   }
}

/* Winsys-specific extensions are not handled, they should be configured by the
 * get_winsys_extensions winsys callback.
 * deviceUUID is not set as well, expected to be configured by the winsys.
 */
static void
terakan_physical_device_get_capabilities(
   struct terakan_instance const * const instance, uint32_t const pci_device_id,
   struct terakan_physical_device_chip_family_info const * const chip_family_info,
   unsigned const tile_pipe_interleave_bytes_log2, VkDeviceSize const min_memory_map_alignment,
   uint32_t const clock_crystal_frequency, VkDeviceSize const max_memory_allocation_size,
   struct vk_device_extension_table * const extensions_out, struct vk_features * const features_out,
   struct vk_properties * const properties_out)
{
   memset(extensions_out, 0, sizeof(*extensions_out));
   memset(features_out, 0, sizeof(*features_out));
   memset(properties_out, 0, sizeof(*properties_out));

   /* Vulkan 1.0. */

   features_out->robustBufferAccess = true;
   features_out->fullDrawIndexUint32 = true;
   features_out->imageCubeArray = true;
   /* TODO(Triang3l): independentBlend. */
   /* TODO(Triang3l): geometryShader. */
   /* TODO(Triang3l): tessellationShader. */
   /* TODO(Triang3l): sampleRateShading. */
   /* TODO(Triang3l): dualSrcBlend. */
   /* TODO(Triang3l): logicOp. */
   /* TODO(Triang3l): multiDrawIndirect. */
   /* TODO(Triang3l): drawIndirectFirstInstance. */
   features_out->depthClamp = true;
   /* TODO(Triang3l): depthBiasClamp. */
   features_out->fillModeNonSolid = true;
   /* TODO(Triang3l): wideLines. */
   /* TODO(Triang3l): largePoints. */
   /* TODO(Triang3l): alphaToOne. */
   features_out->multiViewport = true;
   features_out->samplerAnisotropy = true;
   features_out->textureCompressionBC = true;
   /* TODO(Triang3l): occlusionQueryPrecise. */
   /* TODO(Triang3l): pipelineStatisticsQuery. */
   /* TODO(Triang3l): fragmentStoresAndAtomics. */
   /* TODO(Triang3l): shaderTessellationAndGeometryPointSize. */
   /* TODO(Triang3l): Possibly shaderImageGatherExtended. */
   /* TODO(Triang3l): Shader storage image format features. */
   /* TODO(Triang3l): Shader binding array dynamic indexing. */
   /* TODO(Triang3l): shaderClipDistance. */
   /* TODO(Triang3l): shaderCullDistance. */
   /* TODO(Triang3l): shaderFloat64. */
   /* TODO(Triang3l): shaderResourceMinLod. */
   /* TODO(Triang3l): variableMultisampleRate. */
   /* TODO(Triang3l): inheritedQueries. */

   properties_out->apiVersion = TERAKAN_API_VERSION;
   /* TODO(Triang3l): Change to vk_get_driver_version() when Vulkan 1.0 compatibility is achieved.
    */
   properties_out->driverVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
   properties_out->vendorID = TERAKAN_PHYSICAL_DEVICE_VENDOR_ID_ATI;
   properties_out->deviceID = pci_device_id;
   properties_out->deviceType = chip_family_info->has_dedicated_vram
                                   ? VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
                                   : VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
   snprintf(properties_out->deviceName, sizeof(properties_out->deviceName),
            "AMD R%cxx %s (Terakan)", chip_family_info->is_r9xx ? '9' : '8',
            terakan_physical_device_chip_family_name(chip_family_info->chip_family));
   /* TODO(Triang3l): pipelineCacheUUID when pipeline cache is implemented. */

   properties_out->maxImageDimension1D = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   properties_out->maxImageDimension2D = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   properties_out->maxImageDimension3D = TERAKAN_IMAGE_MAX_TARGET_SLICES;
   properties_out->maxImageDimensionCube = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   properties_out->maxImageArrayLayers = TERAKAN_IMAGE_MAX_TARGET_SLICES;

   /* Vertex fetch constants have 32-bit size minus one in bytes.
    * Random access targets have 32-bit size minus one in elements.
    * Support the maximum possible number of R32G32B32A32 elements.
    */
   properties_out->maxTexelBufferElements = (uint32_t)1 << (32 - 4);

   properties_out->maxUniformBufferRange = TERAKAN_CONSTANT_CACHE_MAX_BUFFER_SIZE_BYTES;

   /* Storage buffers are bound as R32 vertex fetch constants or random access targets.
    * However, buffer RATs have LINEAR_ALIGNED array more, and thus alignment equal to the tile
    * interleave, with the offset (in element units) applied in shaders. Adding the offset may
    * result in out-of-bounds index values near UINT32_MAX wrapping and becoming valid indices
    * near 0. Instead of comparing the index to the buffer size in shaders to implement robustness
    * with the offset, the index value can be clamped to this maximum range as unsigned so that
    * adding any alignment offset after the clamping won't cause wraparound.
    */
   properties_out->maxStorageBufferRange = ~(((uint32_t)1 << tile_pipe_interleave_bytes_log2) - 1);

   /* TODO(Triang3l): Exclude internal constants like the draw ID, ring layout, sample locations,
    * RAT alignment offsets.
    */
   properties_out->maxPushConstantsSize = TERAKAN_CONSTANT_CACHE_MAX_BUFFER_SIZE_BYTES;

   properties_out->maxMemoryAllocationCount = UINT32_MAX;

   properties_out->maxSamplerAllocationCount = UINT32_MAX;

   properties_out->bufferImageGranularity = 1;

   properties_out->maxPerStageDescriptorSamplers = TERAKAN_SAMPLERS_PER_STAGE;
   properties_out->maxPerStageDescriptorUniformBuffers = instance->max_per_stage_uniform_buffers;
   properties_out->maxPerStageDescriptorStorageBuffers = instance->max_per_stage_storage_buffers;
   properties_out->maxPerStageDescriptorSampledImages = instance->max_per_stage_sampled_images;
   properties_out->maxPerStageDescriptorStorageImages =
      TERAKAN_LIMITS_HW_COLOR_RAT_COUNT - instance->max_per_stage_storage_buffers;
   properties_out->maxPerStageDescriptorInputAttachments =
      instance->max_per_stage_input_attachments;
   properties_out->maxColorAttachments = TERAKAN_LIMITS_HW_COLOR_MRT_COUNT;

   properties_out->maxPerStageResources = properties_out->maxPerStageDescriptorUniformBuffers +
                                          properties_out->maxPerStageDescriptorStorageBuffers +
                                          properties_out->maxPerStageDescriptorSampledImages +
                                          properties_out->maxPerStageDescriptorStorageImages +
                                          properties_out->maxPerStageDescriptorInputAttachments +
                                          properties_out->maxColorAttachments;

   properties_out->maxDescriptorSetSamplers =
      properties_out->maxPerStageDescriptorSamplers * MESA_SHADER_STAGES;
   properties_out->maxDescriptorSetUniformBuffers =
      properties_out->maxPerStageDescriptorUniformBuffers * MESA_SHADER_STAGES;
   properties_out->maxDescriptorSetUniformBuffersDynamic =
      properties_out->maxDescriptorSetUniformBuffers;
   properties_out->maxDescriptorSetStorageBuffers =
      properties_out->maxPerStageDescriptorStorageBuffers * MESA_SHADER_STAGES;
   properties_out->maxDescriptorSetStorageBuffersDynamic =
      properties_out->maxDescriptorSetStorageBuffers;
   properties_out->maxDescriptorSetSampledImages =
      properties_out->maxPerStageDescriptorSampledImages * MESA_SHADER_STAGES;
   properties_out->maxDescriptorSetStorageImages =
      properties_out->maxPerStageDescriptorStorageImages * MESA_SHADER_STAGES;
   properties_out->maxDescriptorSetInputAttachments =
      properties_out->maxPerStageDescriptorInputAttachments;

   /* There are no descriptor sets in the hardware, and technically there's no limit because sets
    * can be empty, but other drivers generally provide small numbers of bound sets, so expose a
    * finite amount assuming that every binding can be in its own set.
    */
   uint32_t const max_per_set_descriptors = properties_out->maxDescriptorSetSamplers +
                                            properties_out->maxDescriptorSetUniformBuffers +
                                            properties_out->maxDescriptorSetStorageBuffers +
                                            properties_out->maxDescriptorSetSampledImages +
                                            properties_out->maxDescriptorSetStorageImages +
                                            properties_out->maxDescriptorSetInputAttachments;
   properties_out->maxBoundDescriptorSets = max_per_set_descriptors;

   properties_out->maxVertexInputAttributes = TERAKAN_RESOURCE_HW_COUNT_FETCH;
   properties_out->maxVertexInputBindings = TERAKAN_RESOURCE_HW_COUNT_FETCH;
   properties_out->maxVertexInputAttributeOffset = UINT32_MAX;
   /* NON-CONFORMANT: R8xx has 11 bits for the stride in bytes, which can store values up to 2047.
    * Vulkan requires at least 2048. R9xx has 12 bits.
    */
   /* TODO(Triang3l): Research a workaround scaling the index (in a saturating way to maintain
    * defined overflow behavior) in the fetch shader.
    */
   properties_out->maxVertexInputBindingStride =
      ((uint32_t)1 << (chip_family_info->is_r9xx ? 12 : 11)) - 1;

   properties_out->maxVertexOutputComponents = 4 * TERAKAN_LIMITS_HW_PARAMETER_CACHE_VECTOR_COUNT;

   /* TODO(Triang3l): Tessellation limits. */

   /* TODO(Triang3l): Geometry shader limits. */

   properties_out->maxFragmentInputComponents = 4 * TERAKAN_LIMITS_HW_PARAMETER_CACHE_VECTOR_COUNT;

   properties_out->maxFragmentOutputAttachments = TERAKAN_LIMITS_HW_COLOR_MRT_COUNT;
   /* TODO(Triang3l): maxFragmentDualSrcAttachments when dual-source blending is enabled. */
   properties_out->maxFragmentCombinedOutputResources =
      MAX2(TERAKAN_LIMITS_HW_COLOR_MRT_COUNT, TERAKAN_LIMITS_HW_COLOR_RAT_COUNT);

   properties_out->maxComputeSharedMemorySize =
      sizeof(uint32_t) * TERAKAN_LIMITS_HW_LDS_SIMD_DWORD_COUNT;

   properties_out->maxComputeWorkGroupCount[0] = TERAKAN_LIMITS_HW_COMPUTE_GROUPS_PER_DIMENSION;
   properties_out->maxComputeWorkGroupCount[1] = TERAKAN_LIMITS_HW_COMPUTE_GROUPS_PER_DIMENSION;
   properties_out->maxComputeWorkGroupCount[2] = TERAKAN_LIMITS_HW_COMPUTE_GROUPS_PER_DIMENSION;
   properties_out->maxComputeWorkGroupInvocations = TERAKAN_LIMITS_HW_COMPUTE_GROUP_SIZE;
   properties_out->maxComputeWorkGroupSize[0] = TERAKAN_LIMITS_HW_COMPUTE_GROUP_SIZE;
   properties_out->maxComputeWorkGroupSize[1] = TERAKAN_LIMITS_HW_COMPUTE_GROUP_SIZE;
   properties_out->maxComputeWorkGroupSize[2] = TERAKAN_LIMITS_HW_COMPUTE_GROUP_SIZE;

   properties_out->subPixelPrecisionBits = 8;
   properties_out->subTexelPrecisionBits = 8;
   properties_out->mipmapPrecisionBits = 8;

   properties_out->maxDrawIndexedIndexValue = UINT32_MAX;

   /* TODO(Triang3l): maxDrawIndirectCount when indirect drawing is enabled. */

   properties_out->maxSamplerLodBias = 0x1.0p5f - 0x1.0p-8f;
   properties_out->maxSamplerAnisotropy = 0x1.0p4f;

   properties_out->maxViewports = TERAKAN_HW_STATE_DRAW_MAX_VIEWPORTS;
   properties_out->maxViewportDimensions[0] = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   properties_out->maxViewportDimensions[1] = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   properties_out->viewportBoundsRange[0] = (float)INT16_MIN;
   properties_out->viewportBoundsRange[1] = (float)INT16_MAX;
   properties_out->viewportSubPixelBits = 8;

   properties_out->minMemoryMapAlignment = min_memory_map_alignment;

   /* The largest is for R32G32B32A32 random access targets. */
   properties_out->minTexelBufferOffsetAlignment = sizeof(uint32_t) * 4;

   properties_out->minUniformBufferOffsetAlignment = TERAKAN_CONSTANT_CACHE_LINE_BYTES;

   properties_out->minStorageBufferOffsetAlignment = sizeof(uint32_t);

   properties_out->minTexelOffset = -8;
   properties_out->maxTexelOffset = 8;

   /* TODO(Triang3l): Texel gather offset range when extended image gather is enabled (need to
    * research the range given that the offsets come from a GPR vector).
    */

   /* TODO(Triang3l): Interpolation offset properties when sample-rate shading is enabled. */

   properties_out->maxFramebufferWidth = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   properties_out->maxFramebufferHeight = TERAKAN_IMAGE_MAX_WIDTH_HEIGHT_1D_SLICES;
   properties_out->maxFramebufferLayers = TERAKAN_IMAGE_MAX_TARGET_SLICES;

   VkSampleCountFlags const sample_counts =
      VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_2_BIT | VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_8_BIT;

   properties_out->framebufferColorSampleCounts = sample_counts;
   properties_out->framebufferDepthSampleCounts = sample_counts;
   properties_out->framebufferStencilSampleCounts = sample_counts;
   properties_out->framebufferNoAttachmentsSampleCounts = sample_counts;
   if (chip_family_info->is_r9xx) {
      properties_out->framebufferNoAttachmentsSampleCounts |= VK_SAMPLE_COUNT_16_BIT;
   }

   properties_out->sampledImageColorSampleCounts = sample_counts;
   properties_out->sampledImageIntegerSampleCounts = sample_counts;
   properties_out->sampledImageDepthSampleCounts = sample_counts;
   properties_out->sampledImageStencilSampleCounts = sample_counts;

   properties_out->storageImageSampleCounts = VK_SAMPLE_COUNT_1_BIT;

   properties_out->maxSampleMaskWords = 1;

   if (clock_crystal_frequency != 0) {
      properties_out->timestampComputeAndGraphics = VK_TRUE;
      properties_out->timestampPeriod = (float)(1000000.0 / (double)clock_crystal_frequency);
   }

   /* TODO(Triang3l): Maximum clip and cull distances when enabled. */

   properties_out->discreteQueuePriorities = 2;

   /* TODO(Triang3l): Point size when enabled. */
   /* TODO(Triang3l): Line width when wide lines are enabled. */

   /* TODO(Triang3l): Research strict lines. */

   properties_out->standardSampleLocations = VK_TRUE;

   properties_out->optimalBufferCopyOffsetAlignment = 1;
   properties_out->optimalBufferCopyRowPitchAlignment = 1;

   /* No non-coherent host-visible memory types.
    * Otherwise sysconf(_SC_LEVEL1_DCACHE_LINESIZE) on Linux.
    */
   properties_out->nonCoherentAtomSize = 1;

   /* VK_KHR_dynamic_rendering (#45, Vulkan 1.3). */
   extensions_out->KHR_dynamic_rendering = true;
   features_out->dynamicRendering = true;

   /* VK_KHR_external_memory_capabilities (#72, Vulkan 1.1, instance). */
   char const driver_uuid[] = "AMD-MESA-DRV";
   static_assert(sizeof(driver_uuid) <= sizeof(properties_out->driverUUID),
                 "Driver UUID must fit into the Vulkan UUID field.");
   memcpy(properties_out->driverUUID, driver_uuid, sizeof(driver_uuid));

   /* VK_KHR_external_memory (#73, Vulkan 1.1). */
   extensions_out->KHR_external_memory = true;

   /* VK_EXT_depth_clip_enable (#103). */
   extensions_out->EXT_depth_clip_enable = true;
   features_out->depthClipEnable = true;

   /* VK_KHR_dedicated_allocation (#128, Vulkan 1.1). */
   extensions_out->KHR_dedicated_allocation = true;

   /* VK_KHR_bind_memory2 (#158, Vulkan 1.1). */
   extensions_out->KHR_bind_memory2 = true;

   /* TODO(Triang3l) VK_KHR_maintenance3 (#169, Vulkan 1.1) when vkGetDescriptorSetLayoutSupport
    * is implemented.
    */

   /* VK_KHR_timeline_semaphore (#208, Vulkan 1.2). */
   extensions_out->KHR_timeline_semaphore = true;
   features_out->timelineSemaphore = true;
   properties_out->maxTimelineSemaphoreValueDifference = UINT64_MAX;

   /* VK_EXT_provoking_vertex (#255). */
   extensions_out->EXT_provoking_vertex = true;
   features_out->provokingVertexLast = true;
   features_out->transformFeedbackPreservesProvokingVertex = true;
   properties_out->provokingVertexModePerPipeline = VK_TRUE;
   properties_out->transformFeedbackPreservesTriangleFanProvokingVertex = VK_TRUE;

   /* TODO(Triang3l): VK_EXT_extended_dynamic_state (#268, Vulkan 1.3) when all state is
    * implemented.
    */

   /* VK_KHR_map_memory2 (#272). */
   extensions_out->KHR_map_memory2 = true;

   /* VK_EXT_texel_buffer_alignment (#282, Vulkan 1.3). */
   extensions_out->EXT_texel_buffer_alignment = true;
   features_out->texelBufferAlignment = true;
   /* Random access target element of the largest format. */
   properties_out->storageTexelBufferOffsetAlignmentBytes = sizeof(uint32_t) * 4;
   properties_out->storageTexelBufferOffsetSingleTexelAlignment = VK_TRUE;
   /* Vertex fetch. */
   properties_out->uniformTexelBufferOffsetAlignmentBytes = sizeof(uint32_t);
   properties_out->uniformTexelBufferOffsetSingleTexelAlignment = VK_TRUE;

   /* TODO(Triang3l): Research border color formats with regard to VK_EXT_custom_border_color (#288)
    * and VK_EXT_border_color_swizzle (#412).
    */

   /* VK_EXT_4444_formats (#341, Vulkan 1.3). */
   extensions_out->EXT_4444_formats = true;
   features_out->formatA4R4G4B4 = true;
   features_out->formatA4B4G4R4 = true;

   /* VK_EXT_depth_clip_control (#356). */
   extensions_out->EXT_depth_clip_control = true;
   features_out->depthClipControl = true;

   /* VK_KHR_format_feature_flags2 (#361, Vulkan 1.3). */
   extensions_out->KHR_format_feature_flags2 = true;

   /* TODO(Triang3l): VK_EXT_extended_dynamic_state2 (#378, Vulkan 1.3) when all state is
    * implemented.
    */

   /* TODO(Triang3l): VK_KHR_maintenance4 (#414, Vulkan 1.3): maxBufferSize = UINT32_MAX. */
   /* Addresses within buffers are limited to 32 bits in several places:
    * - Index buffer binding via INDEX_BASE.
    * - Wraparound in copying (most importantly image copying) not handled.
    * The Linux Radeon driver, however, limits addresses within device memory to 32 bits in various
    * areas, so there's no sufficient justification for making workarounds to support more.
    */

   /* VK_EXT_non_seamless_cube_map (#423). */
   extensions_out->EXT_non_seamless_cube_map = true;
   features_out->nonSeamlessCubeMap = true;

   /* VK_EXT_extended_dynamic_state3 (#456). */
   /* TODO(Triang3l): All supported dynamic state. */
   extensions_out->EXT_extended_dynamic_state3 = true;
   features_out->extendedDynamicState3DepthClampEnable = true;
   features_out->extendedDynamicState3PolygonMode = true;
   features_out->extendedDynamicState3SampleMask = true;
   features_out->extendedDynamicState3ProvokingVertexMode = true;
   features_out->extendedDynamicState3DepthClipEnable = true;
   features_out->extendedDynamicState3DepthClipNegativeOneToOne = true;
   properties_out->dynamicPrimitiveTopologyUnrestricted = VK_TRUE;

   /* Mesa WSI. */
#ifdef TERAKAN_USE_WSI_PLATFORM
   extensions_out->KHR_swapchain = true;
   extensions_out->KHR_swapchain_mutable_format = true;
#endif
}

VkExternalMemoryHandleTypeFlags
terakan_physical_device_supported_external_memory_types(
   struct terakan_physical_device const * const device)
{
   return (device->vk.supported_extensions.KHR_external_memory_fd
              ? VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT
              : 0) |
          (device->vk.supported_extensions.EXT_external_memory_dma_buf
              ? VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT
              : 0);
}

static void
terakan_physical_device_init_memory_properties(
   bool const has_dedicated_vram, VkDeviceSize const gtt_page_size, VkDeviceSize const gtt_size,
   VkDeviceSize const vram_size, VkDeviceSize const vram_visible,
   VkPhysicalDeviceMemoryProperties * const memory_properties_out)
{
   /* Based on radv_physical_device_init_mem_types. */

   /* TODO(Triang3l): override_vram_size DRI option. */

   VkDeviceSize gtt_heap_size = gtt_size;
   VkDeviceSize vram_visible_size = MIN2(vram_size, vram_visible);
   VkDeviceSize vram_not_visible_size = vram_size - vram_visible_size;

   if (!has_dedicated_vram) {
      VkDeviceSize const total_size = gtt_size + vram_visible_size;
      /* TODO(Triang3l): enable_unified_heap_on_apu. */
      /* On APUs, the carveout is usually too small for games that request a minimum VRAM size
       * greater than it. To workaround this, we compute the total available memory size (GTT +
       * visible VRAM size) and report 2/3 as VRAM and 1/3 as GTT (so games don't assume that the
       * entire system memory is VRAM and occupy it like it doesn't affect memory available to the
       * CPU).
       */
      vram_visible_size = ALIGN_POT((total_size * 2) / 3, gtt_page_size);
      gtt_heap_size = total_size - vram_visible_size;
      vram_not_visible_size = 0;
   }

   memory_properties_out->memoryHeapCount = 0;
   /* Only get a VRAM heap if it is significant, not if it is a 16 MiB remainder above visible
    * VRAM.
    */
   uint32_t heap_index_vram_not_visible = UINT32_MAX;
   if (vram_not_visible_size > 0 && vram_not_visible_size * 9 >= vram_visible_size) {
      heap_index_vram_not_visible = memory_properties_out->memoryHeapCount++;
      VkMemoryHeap * const heap_vram_not_visible =
         &memory_properties_out->memoryHeaps[heap_index_vram_not_visible];
      heap_vram_not_visible->size = vram_not_visible_size;
      heap_vram_not_visible->flags = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
   }
   uint32_t heap_index_gtt = UINT32_MAX;
   if (gtt_heap_size > 0) {
      heap_index_gtt = memory_properties_out->memoryHeapCount++;
      VkMemoryHeap * const heap_gtt = &memory_properties_out->memoryHeaps[heap_index_gtt];
      heap_gtt->size = gtt_heap_size;
      heap_gtt->flags = 0;
   }
   uint32_t heap_index_vram_visible = UINT32_MAX;
   if (vram_visible_size > 0) {
      heap_index_vram_visible = memory_properties_out->memoryHeapCount++;
      VkMemoryHeap * const heap_vram_visible =
         &memory_properties_out->memoryHeaps[heap_index_vram_visible];
      heap_vram_visible->size = vram_visible_size;
      heap_vram_visible->flags = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
   }

   memory_properties_out->memoryTypeCount = 0;
   if (heap_index_vram_not_visible != UINT32_MAX || heap_index_vram_visible != UINT32_MAX) {
      VkMemoryType * const type_vram =
         &memory_properties_out->memoryTypes[memory_properties_out->memoryTypeCount++];
      type_vram->propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
      type_vram->heapIndex = heap_index_vram_not_visible != UINT32_MAX ? heap_index_vram_not_visible
                                                                       : heap_index_vram_visible;
   }
   if (heap_index_gtt != UINT32_MAX) {
      VkMemoryType * const type_gtt_wc =
         &memory_properties_out->memoryTypes[memory_properties_out->memoryTypeCount++];
      type_gtt_wc->propertyFlags =
         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      type_gtt_wc->heapIndex = heap_index_gtt;
   }
   if (heap_index_vram_visible != UINT32_MAX) {
      VkMemoryType * const type_vram_visible =
         &memory_properties_out->memoryTypes[memory_properties_out->memoryTypeCount++];
      type_vram_visible->propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
      type_vram_visible->heapIndex = heap_index_vram_visible;
   }
   if (heap_index_gtt != UINT32_MAX) {
      VkMemoryType * const type_gtt_cached =
         &memory_properties_out->memoryTypes[memory_properties_out->memoryTypeCount++];
      type_gtt_cached->propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                                       VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
      type_gtt_cached->heapIndex = heap_index_gtt;
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

   uint32_t const timestamp_valid_bits = device->vk.properties.timestampComputeAndGraphics ? 64 : 0;

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
terakan_physical_device_finish(struct terakan_physical_device * const device)
{
   terakan_wsi_finish(device);

   vk_physical_device_finish(&device->vk);
}

void
terakan_physical_device_destroy(struct vk_physical_device * const device_base)
{
   struct terakan_physical_device * const device =
      container_of(device_base, struct terakan_physical_device, vk);

   device->winsys_fn->destroy(device);
}

VkResult
terakan_physical_device_init(
   struct terakan_physical_device * const device, struct terakan_instance * const instance,
   struct terakan_physical_device_winsys_fn const * const winsys_fn_static,
   uint32_t const pci_device_id, VkDeviceSize const gtt_page_size, VkDeviceSize const gtt_size,
   VkDeviceSize const vram_size, VkDeviceSize const vram_visible,
   VkDeviceSize const max_memory_allocation_size, VkDeviceSize const min_memory_map_alignment,
   struct terakan_physical_device_tiling_info const * const tiling_info,
   uint32_t const clock_crystal_frequency,
   struct vk_sync_type const * const * const supported_sync_types_static)
{
   VkResult result;

   /* See NON-CONFORMANT: comments throughout the code for technical reasons. */
   vk_warn_non_conformant_implementation("terakan");

   device->winsys_fn = winsys_fn_static;

   device->pci_device_id = pci_device_id;

   terakan_physical_device_chip_family_info_init(
      terakan_physical_device_get_chip_family(device->pci_device_id), &device->chip_family_info);

   device->max_memory_allocation_size = max_memory_allocation_size;

   device->tiling_info = *tiling_info;
   /* HwlComputeMaxBaseAlignments from the R800 AddrLib for images.
    * Maximum 8x8 micro-tile size is 8-sample and 16 byte-per-pixel.
    * With the largest tile size, the bank width and height can be treated as 1.
    *
    * For buffers, the same alignment is needed as for images with the LINEAR_ALIGNED array mode
    * because it's required for RATs (equal to the pipe interleave in tiling), so it's included in
    * the image alignment. It's normally 256 bytes, but potentially can be 512 bytes, depending on
    * device. It's also not smaller than the constant cache buffer alignment (256 bytes).
    */
   device->buffer_image_bo_alignment =
      (VkDeviceSize)1 << (MIN2(device->tiling_info.row_bytes_log2, 3 + 3 + 3 + 4) +
                          device->tiling_info.banks_log2 + device->tiling_info.pipes_log2);

   struct vk_device_extension_table extensions;
   struct vk_features features;
   struct vk_properties properties;
   terakan_physical_device_get_capabilities(
      instance, device->pci_device_id, &device->chip_family_info,
      device->tiling_info.pipe_interleave_bytes_log2, min_memory_map_alignment,
      clock_crystal_frequency, max_memory_allocation_size, &extensions, &features, &properties);
   device->winsys_fn->get_winsys_extensions(device, &extensions, &features, &properties);

   struct vk_physical_device_dispatch_table dispatch_table;
   vk_physical_device_dispatch_table_from_entrypoints(&dispatch_table,
                                                      &terakan_physical_device_entrypoints, true);
   vk_physical_device_dispatch_table_from_entrypoints(&dispatch_table,
                                                      &wsi_physical_device_entrypoints, false);

   result = vk_physical_device_init(&device->vk, &instance->vk, &extensions, &features, &properties,
                                    &dispatch_table);
   if (result != VK_SUCCESS) {
      return result;
   }

   device->vk.supported_sync_types = supported_sync_types_static;

   terakan_physical_device_init_memory_properties(device->chip_family_info.has_dedicated_vram,
                                                  gtt_page_size, gtt_size, vram_size, vram_visible,
                                                  &device->memory_properties);

   /* Initialize WSI after everything else as it's a layer on top of the Vulkan physical device. */
   result = terakan_wsi_init(device);
   if (result != VK_SUCCESS) {
      vk_physical_device_finish(&device->vk);
      return result;
   }

   return VK_SUCCESS;
}
