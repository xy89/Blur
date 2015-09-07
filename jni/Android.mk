LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := StackBlur

LOCAL_SRC_FILES := com_yunos_assistant_ui_card_ImageUtilEngine.c

LOCAL_LDLIBS    :=  -llog

include $(BUILD_SHARED_LIBRARY)

