/*
 * Mesa 3-D graphics library
 *
 * Copyright © 2017, Google Inc.
 *
 * SPDX-License-Identifier: MIT
 */

#include "nvk_private.h"

#include "nvk_android.h"
#include "nvk_entrypoints.h"
#include "vk_android.h"

#include "util/libsync.h"
#include "util/os_file.h"
#include "vk_common_entrypoints.h"
#include "vk_device.h"
#include "vk_fence.h"
#include "vk_queue.h"
#include "vk_semaphore.h"

VKAPI_ATTR VkResult VKAPI_CALL
nvk_QueueSignalReleaseImageANDROID(
      VkQueue             queue,
      uint32_t            waitSemaphoreCount,
      const VkSemaphore*  pWaitSemaphores,
      VkImage             image,
      int*                pNativeFenceFd)
{
   VkResult result;

   if (waitSemaphoreCount == 0)
      goto done;

   result = vk_common_QueueSubmit(queue, 1,
      &(VkSubmitInfo) {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = pWaitSemaphores,
            .pWaitDstStageMask = &(VkPipelineStageFlags) {
               VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
            },
      },
      (VkFence) VK_NULL_HANDLE);
   if (result != VK_SUCCESS)
      return result;

 done:
   if (pNativeFenceFd) {
      /* We can rely implicit on sync because above we submitted all
       * semaphores to the queue.
       */
      *pNativeFenceFd = -1;
   }

   return VK_SUCCESS;
}
