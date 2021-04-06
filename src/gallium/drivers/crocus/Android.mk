# Mesa 3-D graphics library
#
# Copyright (C) 2018 Intel Corporation
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

# get C_SOURCES
include $(LOCAL_PATH)/Makefile.sources

include $(CLEAR_VARS)

LIBCROCUS_SRC_FILES := \
	crocus_blorp.c \
	crocus_blt.c \
	crocus_query.c \
	crocus_state.c

LIBCROCUS_STATIC_LIBS := \
	libmesa_nir

CROCUS_COMMON_INCLUDES := \
	$(MESA_TOP)/src/mapi \
	$(MESA_TOP)/src/mesa \
	$(MESA_TOP)/src/gallium/include \
	$(MESA_TOP)/src/gallium/auxiliary

#
# libcrocus for gen4
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_crocus_gen4
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(LIBCROCUS_SRC_FILES)
LOCAL_CFLAGS := -DGFX_VERx10=40

LOCAL_C_INCLUDES := $(CROCUS_COMMON_INCLUDES)

LOCAL_STATIC_LIBRARIES := $(LIBCROCUS_STATIC_LIBS)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_genxml

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libcrocus for gen45
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_crocus_gen45
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(LIBCROCUS_SRC_FILES)
LOCAL_CFLAGS := -DGFX_VERx10=45

LOCAL_C_INCLUDES := $(CROCUS_COMMON_INCLUDES)

LOCAL_STATIC_LIBRARIES := $(LIBCROCUS_STATIC_LIBS)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_genxml

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libcrocus for gen5
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_crocus_gen5
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(LIBCROCUS_SRC_FILES)
LOCAL_CFLAGS := -DGFX_VERx10=50

LOCAL_C_INCLUDES := $(CROCUS_COMMON_INCLUDES)

LOCAL_STATIC_LIBRARIES := $(LIBCROCUS_STATIC_LIBS)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_genxml

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libcrocus for gen6
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_crocus_gen6
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(LIBCROCUS_SRC_FILES)
LOCAL_CFLAGS := -DGFX_VERx10=60

LOCAL_C_INCLUDES := $(CROCUS_COMMON_INCLUDES)

LOCAL_STATIC_LIBRARIES := $(LIBCROCUS_STATIC_LIBS)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_genxml

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libcrocus for gen7
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_crocus_gen7
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(LIBCROCUS_SRC_FILES)
LOCAL_CFLAGS := -DGFX_VERx10=70

LOCAL_C_INCLUDES := $(CROCUS_COMMON_INCLUDES)

LOCAL_STATIC_LIBRARIES := $(LIBCROCUS_STATIC_LIBS)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_genxml

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libcrocus for gen75
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_crocus_gen75
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(LIBCROCUS_SRC_FILES)
LOCAL_CFLAGS := -DGFX_VERx10=75

LOCAL_C_INCLUDES := $(CROCUS_COMMON_INCLUDES)

LOCAL_STATIC_LIBRARIES := $(LIBCROCUS_STATIC_LIBS)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_genxml

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

#
# libcrocus for gen80
#

include $(CLEAR_VARS)
LOCAL_MODULE := libmesa_crocus_gen80
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := $(LIBCROCUS_SRC_FILES)
LOCAL_CFLAGS := -DGFX_VERx10=80

LOCAL_C_INCLUDES := $(CROCUS_COMMON_INCLUDES)

LOCAL_STATIC_LIBRARIES := $(LIBCROCUS_STATIC_LIBS)

LOCAL_WHOLE_STATIC_LIBRARIES := libmesa_genxml

include $(MESA_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

###########################################################
include $(CLEAR_VARS)

LOCAL_MODULE := libmesa_pipe_crocus
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_SRC_FILES := \
	$(CROCUS_C_SOURCES)

LOCAL_C_INCLUDES := \
	$(MESA_TOP)/src/mapi \
	$(MESA_TOP)/src/mesa \
	$(MESA_TOP)/include/drm-uapi \
	$(MESA_TOP)/src/gallium/include

LOCAL_SHARED_LIBRARIES := libdrm_intel

LOCAL_STATIC_LIBRARIES := \
	libmesa_blorp \
	libmesa_genxml \
	libmesa_intel_common \
	libmesa_intel_compiler \
	libmesa_intel_perf \
	libmesa_nir

LOCAL_WHOLE_STATIC_LIBRARIES := \
	libmesa_crocus_gen4 \
	libmesa_crocus_gen45 \
	libmesa_crocus_gen5 \
	libmesa_crocus_gen6 \
	libmesa_crocus_gen7 \
	libmesa_crocus_gen75 \
	libmesa_crocus_gen80

include $(GALLIUM_COMMON_MK)
include $(BUILD_STATIC_LIBRARY)

ifneq ($(HAVE_GALLIUM_CROCUS),)
GALLIUM_TARGET_DRIVERS += crocus
$(eval GALLIUM_LIBS += $(LOCAL_MODULE) libmesa_winsys_crocus $(filter-out libmesa_nir, $(LOCAL_STATIC_LIBRARIES)))
$(eval GALLIUM_SHARED_LIBS += $(LOCAL_SHARED_LIBRARIES))
endif
