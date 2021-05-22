# SPDX-License-Identifier: Apache-2.0
#
# Copyright (C) 2021 Roman Stratiienko (r.stratiienko@gmail.com)

MY_PATH := $(call my-dir)

AOSP_ABSOLUTE_PATH := $(realpath .)

m_dummy_$(LOCAL_MULTILIB) := $(TARGET_OUT_INTERMEDIATES)/MESON_DUMMY_$(LOCAL_MULTILIB)/dummy.c

$(m_dummy_$(LOCAL_MULTILIB)):
	mkdir -p $(dir $@)
	touch $@

LOCAL_PATH := .

LOCAL_SRC_FILES := $(m_dummy_$(LOCAL_MULTILIB))
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := meson.dummy.$(LOCAL_MULTILIB)

# Prepare intermediate variables by AOSP make/core internals
include $(BUILD_SHARED_LIBRARY)

LOCAL_PATH := $(MY_PATH)

link_deps := \
	$(built_static_libraries) \
	$(built_shared_libraries) \
	$(built_whole_libraries) \
	$(strip $(all_objects)) \
	$(my_target_libatomic) \
	$(my_target_libcrt_builtins) \
	$(my_target_crtbegin_so_o) \
	$(my_target_crtend_so_o)

# Build mesa3d using intermediate variables provided by AOSP make/core internals
M_TARGET_PREFIX := $(my_2nd_arch_prefix)

MESA3D_LIB_DIR := lib$(subst 32,,$(LOCAL_MULTILIB))

MESON_OUT_DIR                            := $($(M_TARGET_PREFIX)TARGET_OUT_INTERMEDIATES)/MESON_MESA3D
MESON_GEN_DIR                            := $(MESON_OUT_DIR)_GEN
MESON_GEN_FILES_TARGET                   := $(MESON_GEN_DIR)/.timestamp

MESA3D_GALLIUM_DRI_DIR                   := $(MESON_OUT_DIR)/install/usr/local/lib/dri
$(M_TARGET_PREFIX)MESA3D_GALLIUM_DRI_BIN := $(MESON_OUT_DIR)/install/usr/local/lib/libgallium_dri.so
$(M_TARGET_PREFIX)MESA3D_LIBEGL_BIN      := $(MESON_OUT_DIR)/install/usr/local/lib/libEGL.so.1.0.0
$(M_TARGET_PREFIX)MESA3D_LIBGLESV1_BIN   := $(MESON_OUT_DIR)/install/usr/local/lib/libGLESv1_CM.so.1.1.0
$(M_TARGET_PREFIX)MESA3D_LIBGLESV2_BIN   := $(MESON_OUT_DIR)/install/usr/local/lib/libGLESv2.so.2.0.0
$(M_TARGET_PREFIX)MESA3D_LIBGLAPI_BIN    := $(MESON_OUT_DIR)/install/usr/local/lib/libglapi.so.0.0.0
$(M_TARGET_PREFIX)MESA3D_LIBGBM_BIN      := $(MESON_OUT_DIR)/install/usr/local/lib/libgbm.so.1.0.0


MESA3D_GLES_BINS := \
    $($(M_TARGET_PREFIX)MESA3D_LIBEGL_BIN)    \
    $($(M_TARGET_PREFIX)MESA3D_LIBGLESV1_BIN) \
    $($(M_TARGET_PREFIX)MESA3D_LIBGLESV2_BIN) \
    $($(M_TARGET_PREFIX)MESA3D_LIBGLAPI_BIN)  \

MESON_GEN_NINJA := \
	cd $(MESON_OUT_DIR) && PATH=/usr/bin:/usr/local/bin:$$PATH meson ./build     \
	--cross-file $(AOSP_ABSOLUTE_PATH)/$(MESON_GEN_DIR)/aosp_cross               \
	--buildtype=release                                                          \
	-Ddri-search-path=/vendor/$(MESA3D_LIB_DIR)/dri                              \
	-Dplatforms=android                                                          \
	-Dplatform-sdk-version=$(PLATFORM_SDK_VERSION)                               \
	-Ddri-drivers=                                                               \
	-Dgallium-drivers=$(subst $(space),$(comma),$(BOARD_MESA3D_GALLIUM_DRIVERS)) \
	-Dvulkan-drivers=$(subst $(space),$(comma),$(BOARD_MESA3D_VULKAN_DRIVERS))   \
	-Dgbm=enabled                                                                \
	-Degl=enabled                                                                \
	-Dfreedreno-kgsl=true

MESON_BUILD := PATH=/usr/bin:/bin:/sbin:$$PATH ninja -C $(MESON_OUT_DIR)/build

$(MESON_GEN_FILES_TARGET): MESON_CPU_FAMILY := $(subst arm64,aarch64,$(TARGET_$(M_TARGET_PREFIX)ARCH))

define create-pkgconfig
echo -e "Name: $2" \
	"\nDescription: $2" \
	"\nVersion: $3" > $1/$2.pc

endef

# Taken from build/make/core/binary.mk. We need this
# to use definitions from build/make/core/definitions.mk
$(MESON_GEN_FILES_TARGET): PRIVATE_GLOBAL_C_INCLUDES := $(my_target_global_c_includes)
$(MESON_GEN_FILES_TARGET): PRIVATE_GLOBAL_C_SYSTEM_INCLUDES := $(my_target_global_c_system_includes)

$(MESON_GEN_FILES_TARGET): PRIVATE_2ND_ARCH_VAR_PREFIX := $(M_TARGET_PREFIX)
$(MESON_GEN_FILES_TARGET): PRIVATE_CC := $(my_cc)
$(MESON_GEN_FILES_TARGET): PRIVATE_LINKER := $(my_linker)
$(MESON_GEN_FILES_TARGET): PRIVATE_CXX := $(my_cxx)
$(MESON_GEN_FILES_TARGET): PRIVATE_CXX_LINK := $(my_cxx_link)
$(MESON_GEN_FILES_TARGET): PRIVATE_YACCFLAGS := $(LOCAL_YACCFLAGS)
$(MESON_GEN_FILES_TARGET): PRIVATE_ASFLAGS := $(my_asflags)
$(MESON_GEN_FILES_TARGET): PRIVATE_CONLYFLAGS := $(my_conlyflags)
$(MESON_GEN_FILES_TARGET): PRIVATE_CFLAGS := $(my_cflags)
$(MESON_GEN_FILES_TARGET): PRIVATE_CPPFLAGS := $(my_cppflags)
$(MESON_GEN_FILES_TARGET): PRIVATE_CFLAGS_NO_OVERRIDE := $(my_cflags_no_override)
$(MESON_GEN_FILES_TARGET): PRIVATE_CPPFLAGS_NO_OVERRIDE := $(my_cppflags_no_override)
$(MESON_GEN_FILES_TARGET): PRIVATE_RTTI_FLAG := $(LOCAL_RTTI_FLAG)
$(MESON_GEN_FILES_TARGET): PRIVATE_DEBUG_CFLAGS := $(debug_cflags)
$(MESON_GEN_FILES_TARGET): PRIVATE_C_INCLUDES := $(my_c_includes)
$(MESON_GEN_FILES_TARGET): PRIVATE_IMPORTED_INCLUDES := $(imported_includes)
$(MESON_GEN_FILES_TARGET): PRIVATE_LDFLAGS := $(my_ldflags)
$(MESON_GEN_FILES_TARGET): PRIVATE_LDLIBS := $(my_ldlibs)
$(MESON_GEN_FILES_TARGET): PRIVATE_TIDY_CHECKS := $(my_tidy_checks)
$(MESON_GEN_FILES_TARGET): PRIVATE_TIDY_FLAGS := $(my_tidy_flags)
$(MESON_GEN_FILES_TARGET): PRIVATE_ARFLAGS := $(my_arflags)
$(MESON_GEN_FILES_TARGET): PRIVATE_ALL_SHARED_LIBRARIES := $(built_shared_libraries)
$(MESON_GEN_FILES_TARGET): PRIVATE_ALL_STATIC_LIBRARIES := $(built_static_libraries)
$(MESON_GEN_FILES_TARGET): PRIVATE_ALL_WHOLE_STATIC_LIBRARIES := $(built_whole_libraries)
$(MESON_GEN_FILES_TARGET): PRIVATE_ALL_OBJECTS := $(strip $(all_objects))

$(MESON_GEN_FILES_TARGET): PRIVATE_ARM_CFLAGS := $(normal_objects_cflags)

$(MESON_GEN_FILES_TARGET): PRIVATE_TARGET_LIBCRT_BUILTINS := $(my_target_libcrt_builtins)
$(MESON_GEN_FILES_TARGET): PRIVATE_TARGET_LIBATOMIC := $(my_target_libatomic)
$(MESON_GEN_FILES_TARGET): PRIVATE_TARGET_CRTBEGIN_SO_O := $(my_target_crtbegin_so_o)
$(MESON_GEN_FILES_TARGET): PRIVATE_TARGET_CRTEND_SO_O := $(my_target_crtend_so_o)
##

define m-lld-flags
  -nostdlib -Wl,--gc-sections \
  $(PRIVATE_TARGET_CRTBEGIN_SO_O) \
  $(PRIVATE_ALL_OBJECTS) \
  -Wl,--whole-archive \
  $(PRIVATE_ALL_WHOLE_STATIC_LIBRARIES) \
  -Wl,--no-whole-archive \
  $(if $(PRIVATE_GROUP_STATIC_LIBRARIES),-Wl$(comma)--start-group) \
  $(PRIVATE_ALL_STATIC_LIBRARIES) \
  $(if $(PRIVATE_GROUP_STATIC_LIBRARIES),-Wl$(comma)--end-group) \
  $(if $(filter true,$(NATIVE_COVERAGE)),$(PRIVATE_TARGET_COVERAGE_LIB)) \
  $(PRIVATE_TARGET_LIBCRT_BUILTINS) \
  $(PRIVATE_TARGET_LIBATOMIC) \
  $(PRIVATE_TARGET_GLOBAL_LDFLAGS) \
  $(PRIVATE_LDFLAGS) \
  $(PRIVATE_ALL_SHARED_LIBRARIES) \
  $(PRIVATE_TARGET_CRTEND_SO_O) \
  $(PRIVATE_LDLIBS)
endef

define m-lld-flags-cleaned
  $(subst prebuilts/,$(AOSP_ABSOLUTE_PATH)/prebuilts/, \
  $(subst out/,$(AOSP_ABSOLUTE_PATH)/out/,             \
  $(subst -Wl$(comma)--fatal-warnings,,                \
  $(subst -Wl$(comma)--no-undefined-version,,          \
  $(patsubst %dummy.o,,                               \
    $(m-lld-flags))))))
endef

define m-cpp-flags
  $(PRIVATE_TARGET_GLOBAL_CFLAGS) \
  $(PRIVATE_TARGET_GLOBAL_CPPFLAGS) \
  $(PRIVATE_ARM_CFLAGS) \
  $(PRIVATE_RTTI_FLAG) \
  $(PRIVATE_CFLAGS) \
  $(PRIVATE_CPPFLAGS) \
  $(PRIVATE_DEBUG_CFLAGS) \
  $(PRIVATE_CFLAGS_NO_OVERRIDE) \
  $(PRIVATE_CPPFLAGS_NO_OVERRIDE)
endef

define m-c-flags
  $(PRIVATE_TARGET_GLOBAL_CFLAGS) \
  $(PRIVATE_TARGET_GLOBAL_CONLYFLAGS) \
  $(PRIVATE_ARM_CFLAGS) \
  $(PRIVATE_CFLAGS) \
  $(PRIVATE_CONLYFLAGS) \
  $(PRIVATE_DEBUG_CFLAGS) \
  $(PRIVATE_CFLAGS_NO_OVERRIDE)
endef

define filter-c-flags
  $(subst -std=gnu++17,, \
  $(subst -fno-rtti,, \
  $(patsubst  -W%,, \
    $1)))
endef

define m-c-abs-includes
  $(subst  -isystem , -isystem $(AOSP_ABSOLUTE_PATH)/, \
  $(subst  -I, -I$(AOSP_ABSOLUTE_PATH)/, \
  $(subst  -I , -I, \
    $(c-includes))))
endef

$(MESON_GEN_FILES_TARGET): PREPROCESS_MESON_CONFIGS:=$(PREPROCESS_MESON_CONFIGS)
$(MESON_GEN_FILES_TARGET): MESON_GEN_DIR:=$(MESON_GEN_DIR)
$(MESON_GEN_FILES_TARGET): $(sort $(shell find -L $(MESA3D_TOP) -not -path '*/\.*'))
	mkdir -p $(dir $@)
	echo -e -n "[properties]\n"                      \
		"needs_exe_wrapper = true\n"          \
		"c_args = [" > $(dir $@)/aosp_cross
	$(foreach flag, $(call filter-c-flags,$(m-c-flags) $(m-c-abs-includes)), echo -e -n "'$(flag)', " >> $(dir $@)/aosp_cross;)

	echo -e -n "'']\n" \
		"cpp_args = [" >> $(dir $@)/aosp_cross
	$(foreach flag, $(call filter-c-flags,$(m-cpp-flags) $(m-c-abs-includes)), echo -e -n "'$(flag)', " >> $(dir $@)/aosp_cross;)

	echo -e -n "'']\n" \
		"c_link_args = [" >> $(dir $@)/aosp_cross
	$(foreach flag, $(m-lld-flags-cleaned), echo -e -n "'$(flag)', " >> $(dir $@)/aosp_cross;)

	echo -e -n "'']\n" \
		"cpp_link_args = [" >> $(dir $@)/aosp_cross
	$(foreach flag, $(m-lld-flags-cleaned), echo -e -n "'$(flag)', " >> $(dir $@)/aosp_cross;)

	echo -e "'']\n"                                   \
		"[binaries]\n"                            \
		"ar = '$(AOSP_ABSOLUTE_PATH)/$($($(M_TARGET_PREFIX))TARGET_AR)'\n"                                                \
		"c = [$(foreach arg,$(PRIVATE_CC),'$(subst prebuilts/,$(AOSP_ABSOLUTE_PATH)/prebuilts/,$(arg))',)'']\n"           \
		"cpp = [$(foreach arg,$(PRIVATE_CXX),'$(subst prebuilts/,$(AOSP_ABSOLUTE_PATH)/prebuilts/,$(arg))',)'']\n"        \
		"c_ld = 'lld'\n"                                                                                                  \
		"cpp_ld = 'lld'\n\n"                                                                                              \
		"pkgconfig = ['env', 'PKG_CONFIG_LIBDIR=' + '$(AOSP_ABSOLUTE_PATH)/$(MESON_GEN_DIR)', '/usr/bin/pkg-config']\n\n" \
		"[host_machine]\n"                        \
		"system = 'linux'\n"                      \
		"cpu_family = '$(MESON_CPU_FAMILY)'\n"    \
		"cpu = '$(MESON_CPU_FAMILY)'\n"           \
		"endian = 'little'" >> $(dir $@)/aosp_cross

	#
	$(foreach pkg, $(MESON_GEN_PKGCONFIGS), $(call create-pkgconfig,$(dir $@),$(word 1, $(subst :, ,$(pkg))),$(word 2, $(subst :, ,$(pkg)))))
	touch $@

$(MESON_OUT_DIR)/.build.timestamp: MESON_GEN_NINJA:=$(MESON_GEN_NINJA)
$(MESON_OUT_DIR)/.build.timestamp: MESON_BUILD:=$(MESON_BUILD)
$(MESON_OUT_DIR)/.build.timestamp: $(MESON_GEN_FILES_TARGET) $(link_deps)
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

$($(M_TARGET_PREFIX)MESA3D_LIBGBM_BIN) $(MESA3D_GLES_BINS): $(MESON_OUT_DIR)/install/.install.timestamp
	echo "Build $@"\

define vulkan_target
$(M_TARGET_PREFIX)MESA3D_VULKAN_$1_BIN := $(MESON_OUT_DIR)/install/usr/local/lib/libvulkan_$1.so
$(MESON_OUT_DIR)/install/usr/local/lib/libvulkan_$1.so: $(MESON_OUT_DIR)/install/.install.timestamp
	echo $@

endef

$(foreach driver,$(BOARD_MESA3D_VULKAN_DRIVERS), $(eval $(call vulkan_target,$(driver))))

$($(M_TARGET_PREFIX)TARGET_OUT_VENDOR_SHARED_LIBRARIES)/dri/.symlinks.timestamp: MESA3D_GALLIUM_DRI_DIR:=$(MESA3D_GALLIUM_DRI_DIR)
$($(M_TARGET_PREFIX)TARGET_OUT_VENDOR_SHARED_LIBRARIES)/dri/.symlinks.timestamp: $(MESON_OUT_DIR)/install/.install.timestamp
	# Create Symlinks
	mkdir -p $(dir $@)
	ls -1 $(MESA3D_GALLIUM_DRI_DIR)/ | PATH=/usr/bin:$$PATH xargs -I{} ln -s -f libgallium_dri.so $(dir $@)/{}
	touch $@

$($(M_TARGET_PREFIX)MESA3D_GALLIUM_DRI_BIN): $(TARGET_OUT_VENDOR)/$(MESA3D_LIB_DIR)/dri/.symlinks.timestamp
	echo "Build $@"
