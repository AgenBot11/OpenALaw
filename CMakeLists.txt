# OpenALaw CMake Build Configuration
# ARM Architecture Optimized Build

cmake_minimum_required(VERSION 3.12)
project(OpenALaw LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ARM-specific compiler flags
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|ARM|aarch64|AARCH64")
    set(ARM_ARCH TRUE)
    message(STATUS "Building for ARM architecture")
    
    # ARM-specific optimizations
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|AARCH64")
        set(ARM_ARCH_TYPE "ARM64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a+crc+crypto -mtune=cortex-a53")
    else()
        set(ARM_ARCH_TYPE "ARM32")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfpu=neon -mfloat-abi=hard")
    endif()
    
    # Enable ARM-specific optimizations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon -ftree-vectorize -funroll-loops")
else()
    message(WARNING "Building for non-ARM architecture - this is not optimal for OpenALaw")
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# For Android builds
if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DANDROID -D__ANDROID_API__=${ANDROID_NATIVE_API_LEVEL}")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Define source files
set(OPENALAW_SOURCES
    core.js
    android-core-bridge.js
    memory-alignment-check.cpp
)

# Create main library
add_library(openalaw-core SHARED
    memory-alignment-check.cpp
)

# Create Android bridge library
add_library(android-bridge SHARED
    android-core-bridge.js
    android-bridge-native.cpp
)

# Link libraries
target_link_libraries(openalaw-core
    log
    android
)

target_link_libraries(android-bridge
    log
    android
    jnigraphics
)

# Set properties
set_target_properties(openalaw-core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

set_target_properties(android-bridge PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# Installation rules
install(TARGETS openalaw-core android-bridge
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Create a dummy C++ file for the Android bridge native implementation
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/android-bridge-native.cpp
"#include <jni.h>
#include <android/log.h>
#include <android/native_window_jni.h>

#define LOG_TAG \"OpenALaw-AndroidBridge\"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern \"C\" {

// Native functions for Android system interface access
JNIEXPORT jboolean JNICALL
Java_org_openalaw_AndroidBridge_nativeInitialize(JNIEnv *env, jobject thiz) {
    LOGI(\"Initializing OpenALaw Android Bridge native components\");
    return JNI_TRUE;
}

JNIEXPORT jobject JNICALL
Java_org_openalaw_AndroidBridge_nativeCaptureScreen(JNIEnv *env, jobject thiz) {
    LOGI(\"Capturing screen via native bridge\");
    // Implementation would go here
    return NULL;
}

JNIEXPORT jboolean JNICALL
Java_org_openalaw_AndroidBridge_nativeSimulateTouch(
    JNIEnv *env, jobject thiz, jint x, jint y, jint action) {
    LOGI(\"Simulating touch at (%d, %d)\", x, y);
    // Implementation would go here
    return JNI_TRUE;
}

// Additional native methods for system integration
JNIEXPORT void JNICALL
Java_org_openalaw_AndroidBridge_nativeSetAccessibilityService(
    JNIEnv *env, jobject thiz, jobject service) {
    LOGI(\"Setting up accessibility service\");
}

} // extern \"C\"
")