LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := native_emu
LOCAL_SRC_FILES := memory.c cpu.c interrupts.c timer.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/./
LOCAL_LDLIBS := -landroid
include $(BUILD_SHARED_LIBRARY)