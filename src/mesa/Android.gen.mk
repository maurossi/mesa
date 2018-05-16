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

# included by core mesa Android.mk for source generation

ifeq ($(LOCAL_MODULE_CLASS),)
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
endif

intermediates := $(call local-generated-sources-dir)
prebuilt_intermediates := $(MESA_TOP)/prebuilt-intermediates

# This is the list of auto-generated files: sources and headers
sources := \
	main/enums.c \
	main/api_exec.c \
	main/dispatch.h \
	main/format_fallback.c \
	main/format_pack.c \
	main/format_unpack.c \
	main/format_info.h \
	main/remap_helper.h \
	main/get_hash.h \
	main/marshal_generated.c \
	main/marshal_generated.h

LOCAL_SRC_FILES := $(filter-out $(sources), $(LOCAL_SRC_FILES))

LOCAL_C_INCLUDES += $(intermediates)/main

ifeq ($(strip $(MESA_ENABLE_ASM)),true)
ifeq ($(TARGET_ARCH),x86)
sources += x86/matypes.h
LOCAL_C_INCLUDES += $(intermediates)/x86
matypes_deps := \
	$(BUILD_OUT_EXECUTABLES)/mesa_gen_matypes$(BUILD_EXECUTABLE_SUFFIX) \
	$(LOCAL_PATH)/main/mtypes.h \
	$(LOCAL_PATH)/tnl/t_context.h
$(intermediates)/x86/matypes.h: $(matypes_deps) 
	@mkdir -p $(dir $@)
	@echo "MATYPES: $(PRIVATE_MODULE) <= $(notdir $@)"
	$(hide) $< > $@
endif
endif

sources := $(addprefix $(intermediates)/, $(sources))

LOCAL_GENERATED_SOURCES += $(sources)

$(intermediates)/main/dispatch.h: $(prebuilt_intermediates)/main/dispatch.h
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/main/remap_helper.h: $(prebuilt_intermediates)/main/remap_helper.h
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/main/enums.c: $(prebuilt_intermediates)/main/enums.c
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/main/api_exec.c: $(prebuilt_intermediates)/main/api_exec.c
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/main/marshal_generated.c: $(prebuilt_intermediates)/main/marshal_generated.c
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/main/marshal_generated.h: $(prebuilt_intermediates)/main/marshal_generated.h
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/main/get_hash.h: $(prebuilt_intermediates)/main/get_hash.h
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/main/format_fallback.c: $(prebuilt_intermediates)/main/format_fallback.c
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/main/format_info.h: $(prebuilt_intermediates)/main/format_info.h
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/main/format_pack.c: $(prebuilt_intermediates)/main/format_pack.c
	@mkdir -p $(dir $@)
	@cp -f $< $@

$(intermediates)/main/format_unpack.c: $(prebuilt_intermediates)/main/format_unpack.c
	@mkdir -p $(dir $@)
	@cp -f $< $@
