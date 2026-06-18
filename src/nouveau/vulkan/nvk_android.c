/*
 * Mesa 3-D graphics library
 *
 * Copyright © 2017, Google Inc.
 *
 * SPDX-License-Identifier: MIT
 */

#include "nvk_private.h"

#include "nvk_android.h"
#include "nvk_device.h"
#include "nvk_entrypoints.h"
#include "nvk_image.h"
#include "vk_android.h"

#include "vk_device.h"
#include "vk_queue.h"

VKAPI_ATTR VkResult VKAPI_CALL
nvk_QueueSignalReleaseImageANDROID(VkQueue _queue,
                                   uint32_t waitSemaphoreCount,
                                   const VkSemaphore *pWaitSemaphores,
                                   VkImage image,
                                   int *pNativeFenceFd)
{
   VK_FROM_HANDLE(vk_queue, queue, _queue);
   struct vk_device *device = queue->base.device;

   /* This is to workaround nouveau not supporting native_fence_fd cap. On
    * Android, the platform SurfaceFlinger RenderEngine has required
    * EGL_ANDROID_native_fence_sync to import the sync file presented along
    * with the wsi image when the sync fd is not -1.
    *
    * Can drop this if EGL is provided by layering (ANGLE or Zink) on top of
    * Vulkan SYNC_FD and AHB support, or SurfaceFlinger RenderEngine backend
    * is configured to use skiavk so that NVK is used for client composition.
    */
   device->dispatch_table.QueueWaitIdle(_queue);

   *pNativeFenceFd = -1;

   return VK_SUCCESS;
}

#if DETECT_OS_ANDROID && ANDROID_API_LEVEL >= 26
VkResult
nvk_android_ahb_image_init(struct nvk_device *dev,
                           struct nvk_image *img,
                           struct AHardwareBuffer *ahb)
{
   VkResult result;
   assert(img->vk.android_deferred_create_info);

   VkImageDrmFormatModifierExplicitCreateInfoEXT eci;
   VkSubresourceLayout layouts[NVK_MAX_IMAGE_PLANES];

   /* Extract physical bounds and modifier from the Android OS */
   result = vk_android_get_ahb_layout(ahb, &eci, layouts, NVK_MAX_IMAGE_PLANES);
   if (result != VK_SUCCESS)
      return result;

   /* Append the explicit modifier info using Mesa runtime helpers */
   __vk_append_struct(img->vk.android_deferred_create_info, &eci);

   VkExternalMemoryImageCreateInfo external_info = {
      .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
      .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
   };

   __vk_append_struct(img->vk.android_deferred_create_info, &external_info);

   /* Save the received tiling (OPTIMAL/LINEAR) and spoof the state
    * to force nvk_image_init to step inside the DRM_FORMAT_MODIFIER block.
    */
   VkImageTiling original_tiling = img->vk.tiling;
   img->vk.tiling = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT;

   img->vk.drm_format_mod = eci.drmFormatModifier;

   /* Call nvk_image_init directly with the appended deferred info */
   result = nvk_image_init(dev, img, img->vk.android_deferred_create_info);

   /* Restore the original tiling to preserve Vulkan API correctness */
   img->vk.tiling = original_tiling;

   return result;
}
#endif
