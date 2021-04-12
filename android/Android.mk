# SPDX-License-Identifier: Apache-2.0
#
# Copyright (C) 2021 Roman Stratiienko (r.stratiienko@gmail.com)

ifneq ($(filter true, $(BOARD_MESA3D_USES_MESON_BUILD)),)

LOCAL_PATH := $(call my-dir)
MESA3D_TOP := $(dir $(LOCAL_PATH)/../)

LOCAL_SHARED_LIBRARIES := libc libdl libdrm libm liblog libcutils libz libc++ libnativewindow libsync libhardware
LOCAL_STATIC_LIBRARIES := libatomic libexpat libarect
LOCAL_HEADER_LIBRARIES := libnativebase_headers hwvulkan_headers
LOCAL_PKGCONFIGS := backtrace cutils expat hardware libdrm:2.4.105 nativewindow sync zlib:1.2.11

ifeq ($(TARGET_IS_64_BIT),true)
MESA3D_LIB_DIR := lib64
else
MESA3D_LIB_DIR := lib
endif
LOCAL_2ND_ARCH_VAR_PREFIX :=
include $(LOCAL_PATH)/mesa3d_cross.mk

ifdef TARGET_2ND_ARCH
MESA3D_LIB_DIR := lib
LOCAL_2ND_ARCH_VAR_PREFIX := $(TARGET_2ND_ARCH_VAR_PREFIX)
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
LOCAL_SRC_FILES := $4
endif
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_SYMLINKS := $1$2
include $(BUILD_PREBUILT)
include $(CLEAR_VARS)
endef

include $(CLEAR_VARS)

$(eval $(call mesa3d-lib,libgallium_dri,.so.0,dri,MESA3D_GALLIUM_DRI_BIN))
$(eval $(call mesa3d-lib,libglapi,.so.0,,MESA3D_LIBGLAPI_BIN))

$(eval $(call mesa3d-lib,libEGL_mesa,.so.1,egl,MESA3D_LIBEGL_BIN))
$(eval $(call mesa3d-lib,libGLESv1_CM_mesa,.so.1,egl,MESA3D_LIBGLESV1_BIN))
$(eval $(call mesa3d-lib,libGLESv2_mesa,.so.2,egl,MESA3D_LIBGLESV2_BIN))

ifneq (,$(BOARD_MESA3D_VULKAN_DRIVER))
$(eval $(call mesa3d-lib,vulkan.mesa,.so.0,hw,MESA3D_VULKAN_BIN))
endif

LOCAL_EXPORT_C_INCLUDE_DIRS := $(MESA3D_TOP)/src/gbm/main
$(eval $(call mesa3d-lib,libgbm,.so.1,,MESA3D_LIBGBM_BIN))

#-------------------------------------------------------------------------------

endif
