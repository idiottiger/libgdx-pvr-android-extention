LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := pvrtexture
LOCAL_SRC_FILES := PVRTextureData.cpp \
				   CCTexturePVR.cpp \
				   Common.cpp

LOCAL_LDLIBS := -lGLESv2 -llog -landroid

include $(BUILD_SHARED_LIBRARY)
