# Mesa 3-D graphics library
#
# Copyright (C) 2010-2011 Chia-I Wu <olvaffe@gmail.com>
# Copyright (C) 2010-2011 LunarG Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

LOCAL_PATH := $(call my-dir)

# get common_FILES, android_FILES
include $(LOCAL_PATH)/Makefile.sources

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	$(dri2_SOURCES)

LOCAL_CFLAGS := \
	-DGALLIUM_STATIC_TARGETS=1 \
	-DGALLIUM_SOFTPIPE

LOCAL_C_INCLUDES := \
	$(MESA_TOP)/src \
	$(MESA_TOP)/src/mapi \
	$(MESA_TOP)/src/mesa \
	$(MESA_TOP)/src/mesa/drivers/dri/common \
	$(DRM_TOP) \
	$(DRM_TOP)/include/drm 

# swrast only
ifeq ($(MESA_GPU_DRIVERS),swrast)
LOCAL_CFLAGS += -DANDROID_BACKEND_NO_DRM
LOCAL_SRC_FILES += $(drisw_SOURCES)
else
LOCAL_C_INCLUDES += $(DRM_GRALLOC_TOP)
endif

LOCAL_MODULE := libmesa_st_dri

include $(GALLIUM_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)
