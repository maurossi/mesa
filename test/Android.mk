DEBUG_BUILD := true

LOCAL_PATH := $(call my-dir)
LLVM_ROOT_PATH := external/llvm

mesa_SRC_FILES :=	\
    egl.cpp \
	cmain.c \
	m_matrix.c \
	main.cpp
	
mesa_C_INCLUDES :=	\
	$(LOCAL_PATH)   \
	$(LOCAL_PATH)/../include
	
# Executable for host
# ========================================================
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

ifeq ($(DEBUG_BUILD),true)
LOCAL_CFLAGS += -DDEBUG -UNDEBUG -O0 -g
endif

LOCAL_MODULE := mesa
LOCAL_SRC_FILES := $(mesa_SRC_FILES)
LOCAL_STATIC_LIBRARIES := libMesa
LOCAL_SHARED_LIBRARIES := libbcc
LOCAL_C_INCLUDES := $(mesa_C_INCLUDES)
LOCAL_LDLIBS := -lpthread -ldl

include $(LLVM_ROOT_PATH)/llvm-host-build.mk
include $(BUILD_HOST_EXECUTABLE)

# Executable for target
# ========================================================
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

ifeq ($(DEBUG_BUILD),true)
LOCAL_CFLAGS += -DDEBUG -UNDEBUG -O0 -g
endif

LOCAL_MODULE := mesa
LOCAL_SRC_FILES := $(mesa_SRC_FILES)
LOCAL_STATIC_LIBRARIES := libMesa
LOCAL_SHARED_LIBRARIES := libstlport libcutils libutils libhardware libgui libbcc libdl
LOCAL_C_INCLUDES := $(mesa_C_INCLUDES)

include $(LLVM_ROOT_PATH)/llvm-device-build.mk
include $(BUILD_EXECUTABLE)
