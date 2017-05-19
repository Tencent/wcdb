LOCAL_PATH:= $(call my-dir)

NDK_PROJECT_PATH := $(LOCAL_PATH)/build
APP_BUILD_SCRIPT := $(LOCAL_PATH)/Android.mk
NDK_TOOLCHAIN_VERSION := 4.9
APP_ABI := armeabi armeabi-v7a arm64-v8a x86
APP_PLATFORM := android-12
APP_STL := gnustl_static
APP_PIE := true
