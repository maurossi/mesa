DEBUG_BUILD := true

ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH := $(call my-dir)

mesa_SRC_FILES :=	\
    egl.cpp \
	cmain.c \
	main.cpp
	
# Executable for host
# ========================================================
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

ifeq ($(DEBUG_BUILD),true)
LOCAL_CPPFLAGS += -DDEBUG -UNDEBUG -O0 -g
LOCAL_CFLAGS += -DDEBUG -UNDEBUG -O0 -g 
endif

LOCAL_MODULE := mesa
LOCAL_SRC_FILES := $(mesa_SRC_FILES)
LOCAL_STATIC_LIBRARIES := libMesa
LOCAL_SHARED_LIBRARIES := libbcc
LOCAL_C_INCLUDES :=	\
	$(LOCAL_PATH)   \
	frameworks/compile/mesa/include

include $(BUILD_HOST_EXECUTABLE)

# Executable for target
# ========================================================
include $(CLEAR_VARS)

#LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional

ifeq ($(DEBUG_BUILD),true)
LOCAL_CPPFLAGS += -DDEBUG -UNDEBUG -O0 -g
LOCAL_CFLAGS += -DDEBUG -UNDEBUG -O0 -g 
endif

LOCAL_MODULE := mesa
LOCAL_SRC_FILES := $(mesa_SRC_FILES)
LOCAL_STATIC_LIBRARIES := libMesa
LOCAL_SHARED_LIBRARIES := libstlport libcutils libutils libhardware libsurfaceflinger_client libbcc libdl
LOCAL_C_INCLUDES :=	\
	$(LOCAL_PATH)	\
	frameworks/compile/mesa/include

include $(BUILD_EXECUTABLE)

endif # TARGET_SIMULATOR != true
