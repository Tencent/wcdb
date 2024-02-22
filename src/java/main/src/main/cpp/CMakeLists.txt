
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.13)

# Declares and names the project.

project(WCDB_Android)

include(../../../../../utility.cmake)

set(TARGET_NAME "WCDB")
set(WCDB_BRIDGE ON)
set(SKIP_WCONAN ON)
set(BUILD_SHARED_LIBS ON)

add_subdirectory(../../../../.. ${CMAKE_CURRENT_BINARY_DIR}/wcdb)

set(WCDB_JNI_SRC_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE WCDB_JNI_SRC
    ${WCDB_JNI_SRC_DIR}/*.cpp
    ${WCDB_JNI_SRC_DIR}/*.c
    ${WCDB_JNI_SRC_DIR}/*.h)

recursive_subdirs(WCDB_JNI_INCLUDES ${WCDB_JNI_SRC_DIR})

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

target_sources(${TARGET_NAME} PUBLIC ${WCDB_JNI_SRC})
target_include_directories(${TARGET_NAME} PUBLIC ${WCDB_JNI_INCLUDES})
