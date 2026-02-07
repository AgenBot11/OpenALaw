# OpenALaw Android Build Configuration
# ARM Architecture Optimized Build

LOCAL_PATH := $(call my-dir)

# Build the memory alignment checker library
include $(CLEAR_VARS)

LOCAL_MODULE := libopenalaw-memory
LOCAL_SRC_FILES := memory-alignment-check.cpp
LOCAL_CFLAGS := -std=c++11 -Wall -Wextra -O2
LOCAL_CPPFLAGS := -std=c++11 -fexceptions -frtti
LOCAL_LDLIBS := -llog

# Target ARM architectures
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true

include $(BUILD_SHARED_LIBRARY)

# Build the main OpenALaw core
include $(CLEAR_VARS)

LOCAL_MODULE := libopenalaw-core
LOCAL_SRC_FILES := openalaw-core.cpp
LOCAL_CFLAGS := -std=c++11 -Wall -Wextra -O2 -DARM_ARCH
LOCAL_CPPFLAGS := -std=c++11 -fexceptions -frtti -DARM_OPTIMIZED
LOCAL_LDLIBS := -llog -landroid

# ARM-specific optimizations
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true

include $(BUILD_SHARED_LIBRARY)

# Build Android Core Bridge native components
include $(CLEAR_VARS)

LOCAL_MODULE := libandroid-bridge
LOCAL_SRC_FILES := android-bridge-native.cpp
LOCAL_CFLAGS := -std=c++11 -Wall -Wextra -O2 -DANDROID_NDK
LOCAL_CPPFLAGS := -std=c++11 -fexceptions -frtti -DANDROID_BRIDGE
LOCAL_LDLIBS := -llog -landroid -ljnigraphics

# ARM-specific optimizations
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true

include $(BUILD_SHARED_LIBRARY)

# Application build configuration
APP_ABI := arm64-v8a armeabi-v7a
APP_PLATFORM := android-21
APP_STL := c++_shared