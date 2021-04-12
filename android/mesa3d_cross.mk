# SPDX-License-Identifier: Apache-2.0
#
# Copyright (C) 2021 Roman Stratiienko (r.stratiienko@gmail.com)

LOCAL_PATH := $(call my-dir)

AOSP_ABSOLUTE_PATH := $(realpath .)

M_TARGET_PREFIX := $(LOCAL_2ND_ARCH_VAR_PREFIX)

MESON_CONFIGS_SRC_DIR                    := $(LOCAL_PATH)/configs_raw
MESON_OUT_DIR                            := $($(M_TARGET_PREFIX)TARGET_OUT_INTERMEDIATES)/MESON_MESA3D
MESON_GEN_DIR                            := $(MESON_OUT_DIR)_GEN

MESA3D_GALLIUM_DRI_DIR                   := $(MESON_OUT_DIR)/install/usr/local/lib/dri
$(M_TARGET_PREFIX)MESA3D_GALLIUM_DRI_BIN := $(MESON_OUT_DIR)/install/usr/local/lib/libgallium_dri.so
$(M_TARGET_PREFIX)MESA3D_LIBEGL_BIN      := $(MESON_OUT_DIR)/install/usr/local/lib/libEGL.so.1.0.0
$(M_TARGET_PREFIX)MESA3D_LIBGLESV1_BIN   := $(MESON_OUT_DIR)/install/usr/local/lib/libGLESv1_CM.so.1.1.0
$(M_TARGET_PREFIX)MESA3D_LIBGLESV2_BIN   := $(MESON_OUT_DIR)/install/usr/local/lib/libGLESv2.so.2.0.0
$(M_TARGET_PREFIX)MESA3D_LIBGLAPI_BIN    := $(MESON_OUT_DIR)/install/usr/local/lib/libglapi.so.0.0.0
$(M_TARGET_PREFIX)MESA3D_LIBGBM_BIN      := $(MESON_OUT_DIR)/install/usr/local/lib/libgbm.so.1.0.0

$(M_TARGET_PREFIX)MESA3D_VULKAN_BIN      := $(MESON_OUT_DIR)/install/usr/local/lib/libvulkan_$(BOARD_MESA3D_VULKAN_DRIVER).so

MESA3D_GLES_BINS := \
    $($(M_TARGET_PREFIX)MESA3D_LIBEGL_BIN)    \
    $($(M_TARGET_PREFIX)MESA3D_LIBGLESV1_BIN) \
    $($(M_TARGET_PREFIX)MESA3D_LIBGLESV2_BIN) \
    $($(M_TARGET_PREFIX)MESA3D_LIBGLAPI_BIN)  \

MESON_GEN_NINJA := \
	cd $(MESON_OUT_DIR) && PATH=/usr/bin:/bin:/sbin:$$PATH meson ./build    \
	--cross-file $(AOSP_ABSOLUTE_PATH)/$(MESON_GEN_DIR)/aosp_cross          \
	--buildtype=release                                                     \
	-Ddri-search-path=/vendor/$(MESA3D_LIB_DIR)/dri                         \
	-Dplatforms=android                                                     \
	-Dplatform-sdk-version=$(PLATFORM_SDK_VERSION)                          \
	-Dgallium-drivers=$(BOARD_MESA3D_GALLIUM_DRIVERS)                       \
	-Dvulkan-drivers=$(BOARD_MESA3D_VULKAN_DRIVER)                          \
	-Dgbm=enabled                                                           \
	-Degl=enabled                                                           \
	-Dfreedreno-kgsl=true

MESON_BUILD := PATH=/usr/bin:/bin:/sbin:$$PATH ninja -C $(MESON_OUT_DIR)/build

# meson arch file doesn't recognize arm64 or x86_64
MESON_CPU_FAMILY := $(TARGET_$(M_TARGET_PREFIX)ARCH)
ifneq ($(filter arm64, $(MESON_CPU_FAMILY)),)
MESON_CPU_FAMILY := arm
endif
ifneq ($(filter x86_64, $(MESON_CPU_FAMILY)),)
MESON_CPU_FAMILY := x86
endif

## DEPENDENCIES: build/make/core/binary.mk was used for reference
my_shared_libraries := $(LOCAL_SHARED_LIBRARIES)

my_shared_libraries := $(foreach l,$(my_shared_libraries),\
      $(if $(SPLIT_VENDOR.SHARED_LIBRARIES.$(l)),$(l).vendor,$(l)))

my_shared_libraries := $(foreach l,$(my_shared_libraries),\
      $(if $(filter $(l),$(VENDOR_PUBLIC_LIBRARIES)),$(l).vendorpublic,$(l)))

built_shared_libraries := \
$(foreach lib,$(my_shared_libraries), \
    $(call intermediates-dir-for, \
        SHARED_LIBRARIES,$(lib),$(my_kind),,$(LOCAL_2ND_ARCH_VAR_PREFIX),$(my_host_cross))/$(lib)$(so_suffix))

built_shared_library_deps := $(addsuffix .toc, $(built_shared_libraries))
####
my_static_libraries := $(LOCAL_STATIC_LIBRARIES)

my_static_libraries := $(foreach l,$(my_static_libraries),\
    $(if $(SPLIT_VENDOR.STATIC_LIBRARIES.$(l)),$(l).vendor,$(l)))

built_static_libraries := \
    $(foreach lib,$(my_static_libraries), \
      $(call intermediates-dir-for, \
        STATIC_LIBRARIES,$(lib),$(my_kind),,$(LOCAL_2ND_ARCH_VAR_PREFIX),$(my_host_cross))/$(lib)$(a_suffix))
####
my_target_crtbegin_dynamic_o := $(SOONG_$(LOCAL_2ND_ARCH_VAR_PREFIX)TARGET_OBJECT_crtbegin_dynamic.vendor)
my_target_crtbegin_static_o := $(SOONG_$(LOCAL_2ND_ARCH_VAR_PREFIX)TARGET_OBJECT_crtbegin_static.vendor)
my_target_crtbegin_so_o := $(SOONG_$(LOCAL_2ND_ARCH_VAR_PREFIX)TARGET_OBJECT_crtbegin_so.vendor)
my_target_crtend_o := $(SOONG_$(LOCAL_2ND_ARCH_VAR_PREFIX)TARGET_OBJECT_crtend_android.vendor)
####
$(warning STATIC: $(built_static_libraries))

link_deps := \
	$(built_static_libraries) \
	$(built_shared_libraries) \
	$(my_target_crtend_o) \
	$(my_target_crtbegin_so_o) \

abs_link_deps := $(addprefix -Wl$(comma)$(AOSP_ABSOLUTE_PATH)/,$(link_deps))
####
my_header_libraries := $(LOCAL_HEADER_LIBRARIES)

imported_includes := $(strip \
    $(if $(LOCAL_USE_VNDK),\
      $(call intermediates-dir-for,HEADER_LIBRARIES,device_kernel_headers,$(my_kind),,$(LOCAL_2ND_ARCH_VAR_PREFIX),$(my_host_cross))) \
    $(foreach l, $(my_shared_libraries), \
      $(call intermediates-dir-for,SHARED_LIBRARIES,$(l),$(my_kind),,$(LOCAL_2ND_ARCH_VAR_PREFIX),$(my_host_cross))) \
    $(foreach l, $(my_static_libraries) $(my_whole_static_libraries), \
      $(call intermediates-dir-for,STATIC_LIBRARIES,$(l),$(my_kind),,$(LOCAL_2ND_ARCH_VAR_PREFIX),$(my_host_cross))) \
    $(foreach l, $(my_header_libraries), \
      $(call intermediates-dir-for,HEADER_LIBRARIES,$(l),$(my_kind),,$(LOCAL_2ND_ARCH_VAR_PREFIX),$(my_host_cross))))
#### DEPENDENCIES

PREPROCESS_MESON_CONFIGS := sed -i \
	-e 's!$$(AOSP_TOP)!$(AOSP_ABSOLUTE_PATH)!'                               \
	-e 's!$$(MESON_CONFIGS)!$(MESON_GEN_DIR)!'                               \
	-e 's!$$(CLANG)!$(CLANG)!'                                               \
	-e 's!$$(CLANG_CXX)!$(CLANG_CXX)!'                                       \
	-e 's!$$(ARCH_TRIPLE)!$($(M_TARGET_PREFIX)CLANG_TARGET_TRIPLE)!'         \
	-e 's!$$(CPU_FAMILY)!$(MESON_CPU_FAMILY)!'                               \
	-e 's!$$(PLATFORM_SDK_VERSION)!$(PLATFORM_SDK_VERSION)!'                 \
	-e 's!$$(LLVM_AR)!$(LLVM_PREBUILTS_PATH)/llvm-ar!'                       \
	-e 's!$$(LLVM_STRIP)!$(LLVM_STRIP)!'                                     \
	-e 's!$$(TARGET_ARCH_VARIANT)!$(TARGET_$(M_TARGET_PREFIX)ARCH_VARIANT)!' \
	-e 's!$$(abs_link_deps)!$(abs_link_deps)!'                               \
	$(MESON_GEN_DIR)/*

define create-pkgconfig
echo "Name: $2" > $1/$2.pc
echo "Description: $2" >> $1/$2.pc
echo "Version: $3" >> $1/$2.pc

endef

$(MESON_GEN_DIR)/.timestamp: PRIVATE_C_INCLUDES:=
$(MESON_GEN_DIR)/.timestamp: PRIVATE_GLOBAL_C_INCLUDES:=$(SRC_HEADERS)
$(MESON_GEN_DIR)/.timestamp: PRIVATE_GLOBAL_C_SYSTEM_INCLUDES:=$(SRC_SYSTEM_HEADERS)
$(MESON_GEN_DIR)/.timestamp: PRIVATE_IMPORTED_INCLUDES:=$(imported_includes)
$(MESON_GEN_DIR)/.timestamp: PREPROCESS_MESON_CONFIGS:=$(PREPROCESS_MESON_CONFIGS)
$(MESON_GEN_DIR)/.timestamp: $(sort $(shell find -L $(MESON_CONFIGS_SRC_DIR)))
	mkdir -p $(dir $@)
	cp $(MESON_CONFIGS_SRC_DIR)/* $(dir $@)
	$(PREPROCESS_MESON_CONFIGS)
	echo $(c-includes) > $(dir $@)/c_includes
	sed -i 's! -I ! -I!g' $(dir $@)/c_includes
	sed -i 's! -I! -I$(AOSP_ABSOLUTE_PATH)/!g' $(dir $@)/c_includes
	sed -i 's!-isystem !-isystem $(AOSP_ABSOLUTE_PATH)/!g' $(dir $@)/c_includes
	sed -i -e "s!\$$(abs_include_deps)!$$(cat $(dir $@)/c_includes)!" $(dir $@)/*
	$(foreach pkg, $(LOCAL_PKGCONFIGS), $(call create-pkgconfig,$(dir $@),$(word 1, $(subst :, ,$(pkg))),$(word 2, $(subst :, ,$(pkg)))))
	touch $@


$(MESON_OUT_DIR)/.build.timestamp: MESON_GEN_NINJA:=$(MESON_GEN_NINJA)
$(MESON_OUT_DIR)/.build.timestamp: MESON_BUILD:=$(MESON_BUILD)
$(MESON_OUT_DIR)/.build.timestamp: $(MESON_GEN_DIR)/.timestamp $(sort $(shell find -L $(MESA3D_TOP) -not -path '*/\.*')) $(link_deps)
	rm -rf $(dir $@)
	mkdir -p $(dir $@)
	mkdir -p $(dir $@)/build
	# Meson will update timestamps in sources directory, continuously retriggering the build
	# even if nothing changed. Copy sources into intermediate dir to avoid this effect.
	cp -r $(MESA3D_TOP)/* $(dir $@)
	$(MESON_GEN_NINJA)
	$(MESON_BUILD)
	touch $@

MESON_COPY_LIBGALLIUM := \
	cp `ls -1 $(MESA3D_GALLIUM_DRI_DIR)/* | head -1` $($(M_TARGET_PREFIX)MESA3D_GALLIUM_DRI_BIN)

$(MESON_OUT_DIR)/install/.install.timestamp: MESON_COPY_LIBGALLIUM:=$(MESON_COPY_LIBGALLIUM)
$(MESON_OUT_DIR)/install/.install.timestamp: MESON_BUILD:=$(MESON_BUILD)
$(MESON_OUT_DIR)/install/.install.timestamp: $(MESON_OUT_DIR)/.build.timestamp
	rm -rf $(dir $@)
	mkdir -p $(dir $@)
	DESTDIR=$(AOSP_ABSOLUTE_PATH)/$(dir $@) $(MESON_BUILD) install
	$(MESON_COPY_LIBGALLIUM)
	touch $@

$($(M_TARGET_PREFIX)MESA3D_LIBGBM_BIN) $($(M_TARGET_PREFIX)MESA3D_VULKAN_BIN) $(MESA3D_GLES_BINS): $(MESON_OUT_DIR)/install/.install.timestamp
	echo "Build $@"\

$($(M_TARGET_PREFIX)TARGET_OUT_VENDOR_SHARED_LIBRARIES)/dri/.symlinks.timestamp: MESON_CREATE_SYMLINKS:=$(MESON_CREATE_SYMLINKS)
$($(M_TARGET_PREFIX)TARGET_OUT_VENDOR_SHARED_LIBRARIES)/dri/.symlinks.timestamp: $(MESON_OUT_DIR)/install/.install.timestamp
	# Create Symlinks
	mkdir -p $(dir $@)
	ls -1 $(MESA3D_GALLIUM_DRI_DIR)/ | PATH=/usr/bin:$$PATH xargs -I{} ln -s -f libgallium_dri.so $(dir $@)/{}
	touch $@

$($(M_TARGET_PREFIX)MESA3D_GALLIUM_DRI_BIN): $(TARGET_OUT_VENDOR)/$(MESA3D_LIB_DIR)/dri/.symlinks.timestamp
	echo "Build $@"
