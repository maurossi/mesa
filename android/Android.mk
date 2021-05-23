# SPDX-License-Identifier: Apache-2.0
#
# Copyright (C) 2021 Roman Stratiienko (r.stratiienko@gmail.com)

ifneq ($(filter true, $(BOARD_MESA3D_USES_MESON_BUILD)),)

LOCAL_PATH := $(call my-dir)
MESA3D_TOP := $(dir $(LOCAL_PATH))

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libc libdl libdrm libm liblog libcutils libz libc++ libnativewindow libsync libhardware
LOCAL_STATIC_LIBRARIES := libexpat libarect
LOCAL_HEADER_LIBRARIES := libnativebase_headers hwvulkan_headers libbacktrace_headers
MESON_GEN_PKGCONFIGS := backtrace cutils expat hardware libdrm:2.4.105 nativewindow sync zlib:1.2.11

ifneq ($(filter iris,$(BOARD_MESA3D_GALLIUM_DRIVERS)),)
LOCAL_SHARED_LIBRARIES += libdrm_intel
MESON_GEN_PKGCONFIGS += libdrm_intel:2.4.105
endif

ifneq ($(filter radeonsi,$(BOARD_MESA3D_GALLIUM_DRIVERS)),)
LOCAL_SHARED_LIBRARIES += libdrm_amdgpu
MESON_GEN_PKGCONFIGS += libdrm_amdgpu:2.4.105
LOCAL_CFLAGS += -DFORCE_BUILD_AMDGPU   # instructs LLVM to declare LLVMInitializeAMDGPU* functions
LOCAL_SHARED_LIBRARIES += libLLVM11
MESON_GEN_PKGCONFIGS += libLLVM11
endif
ifneq ($(filter amd,$(BOARD_MESA3D_VULKAN_DRIVERS)),)
LOCAL_SHARED_LIBRARIES += libdrm_amdgpu
MESON_GEN_PKGCONFIGS += libdrm_amdgpu:2.4.105
endif

ifneq ($(filter radeonsi r300 r600,$(BOARD_MESA3D_GALLIUM_DRIVERS)),)
LOCAL_SHARED_LIBRARIES += libdrm_radeon
MESON_GEN_PKGCONFIGS += libdrm_radeon:2.4.105
LOCAL_WHOLE_STATIC_LIBRARIES += libelf
MESON_GEN_PKGCONFIGS += libelf
endif

ifneq ($(filter nouveau,$(BOARD_MESA3D_GALLIUM_DRIVERS)),)
LOCAL_SHARED_LIBRARIES += libdrm_nouveau
MESON_GEN_PKGCONFIGS += libdrm_nouveau:2.4.105
endif

ifeq ($(TARGET_IS_64_BIT),true)
LOCAL_MULTILIB := 64
else
LOCAL_MULTILIB := 32
endif
include $(LOCAL_PATH)/mesa3d_cross.mk

ifdef TARGET_2ND_ARCH
LOCAL_MULTILIB := 32
include $(LOCAL_PATH)/mesa3d_cross.mk
endif

#-------------------------------------------------------------------------------

define mesa3d-lib
LOCAL_PATH := .
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE := $1
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := $3
ifdef TARGET_2ND_ARCH
LOCAL_SRC_FILES_$(TARGET_ARCH) := $($4)
LOCAL_SRC_FILES_$(TARGET_2ND_ARCH) := $(2ND_$4)
LOCAL_MULTILIB := both
else
LOCAL_SRC_FILES := $($4)
endif
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_SYMLINKS := $1$2
include $(BUILD_PREBUILT)
include $(CLEAR_VARS)
endef

include $(CLEAR_VARS)

# Module 'libgallium_dri', produces '/vendor/lib{64}/dri/libgallium_dri.so'
# This module also trigger DRI symlinks creation process
$(eval $(call mesa3d-lib,libgallium_dri,.so.0,dri,MESA3D_GALLIUM_DRI_BIN))
# Module 'libglapi', produces '/vendor/lib{64}/libglapi.so'
$(eval $(call mesa3d-lib,libglapi,.so.0,,MESA3D_LIBGLAPI_BIN))

# Module 'libEGL_mesa', produces '/vendor/lib{64}/egl/libEGL_mesa.so'
$(eval $(call mesa3d-lib,libEGL_mesa,.so.1,egl,MESA3D_LIBEGL_BIN))
# Module 'libGLESv1_CM_mesa', produces '/vendor/lib{64}/egl/libGLESv1_CM_mesa.so'
$(eval $(call mesa3d-lib,libGLESv1_CM_mesa,.so.1,egl,MESA3D_LIBGLESV1_BIN))
# Module 'libGLESv2_mesa', produces '/vendor/lib{64}/egl/libGLESv2_mesa.so'
$(eval $(call mesa3d-lib,libGLESv2_mesa,.so.2,egl,MESA3D_LIBGLESV2_BIN))

# Modules 'vulkan.{driver_name}', produces '/vendor/lib{64}/hw/vulkan.{driver_name}.so' HAL
$(foreach driver,$(BOARD_MESA3D_VULKAN_DRIVERS), \
    $(eval $(call mesa3d-lib,vulkan.$(driver),.so.0,hw,MESA3D_VULKAN_$(driver)_BIN)))

LOCAL_EXPORT_C_INCLUDE_DIRS := $(MESA3D_TOP)/src/gbm/main

# Modules 'libgbm', produces '/vendor/lib{64}/libgbm.so'
$(eval $(call mesa3d-lib,libgbm,.so.1,,MESA3D_LIBGBM_BIN))

#-------------------------------------------------------------------------------

endif
