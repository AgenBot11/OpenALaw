#!/bin/bash

# OpenALaw ARM Cross-Compilation Setup Script
# This script sets up the cross-compilation environment for ARM64

set -e  # Exit on any error

echo "Setting up OpenALaw ARM Cross-Compilation Environment..."

# Check if Android NDK is installed
if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "ERROR: ANDROID_NDK_HOME environment variable not set"
    echo "Please download and install Android NDK, then set ANDROID_NDK_HOME"
    exit 1
fi

echo "ANDROID_NDK_HOME: $ANDROID_NDK_HOME"

# Create build directory
mkdir -p build-arm64
cd build-arm64

# Set ARM64 cross-compilation variables
export TOOLCHAIN="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64"
export ARM_CC="$TOOLCHAIN/bin/aarch64-linux-android21-clang"
export ARM_CXX="$TOOLCHAIN/bin/aarch64-linux-android21-clang++"
export ARM_STRIP="$TOOLCHAIN/bin/aarch64-linux-android-strip"

echo "Configuring CMake for ARM64..."
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-21 \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=21 \
    -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a \
    -DCMAKE_ANDROID_NDK=$ANDROID_NDK_HOME

echo "Building OpenALaw for ARM64..."
make -j$(nproc)

echo "Build completed successfully!"
echo "ARM64 binaries are located in the build-arm64 directory."

# Verify the binary architecture
echo "Verifying binary architecture..."
file libopenalaw-core.so || echo "libopenalaw-core.so not found"
file libandroid-bridge.so || echo "libandroid-bridge.so not found"

echo "ARM Cross-Compilation Setup Complete!"