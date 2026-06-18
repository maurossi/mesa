/*
 * Mesa 3-D graphics library
 *
 * Copyright © 2017, Google Inc.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef NVK_ANDROID_H
#define NVK_ANDROID_H

#include "util/detect_os.h"
#include "vulkan/vulkan.h"

struct nvk_device;
struct nvk_image;

#if DETECT_OS_ANDROID && ANDROID_API_LEVEL >= 26
VkResult nvk_android_ahb_image_init(struct nvk_device *dev,
                                    struct nvk_image *img,
                                    struct AHardwareBuffer *ahb);
#endif

#endif /* NVK_ANDROID_H */
