LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := native_emu
LOCAL_SRC_FILES := memory.c cpu.c interrupts.c timer.c tape.c video.c SID.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/./
LOCAL_LDLIBS := -landroid -llog
include $(BUILD_SHARED_LIBRARY)
