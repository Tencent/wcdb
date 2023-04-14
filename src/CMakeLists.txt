cmake_minimum_required(VERSION 3.13)
project(WCDB)

include(utility.cmake)
include(sqlcipher.cmake)


set(WCDB_SRC_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SRC_WCDB_COMMON
    ${WCDB_SRC_DIR}/common/*.c
    ${WCDB_SRC_DIR}/common/*.cpp
    ${WCDB_SRC_DIR}/common/*.h
    ${WCDB_SRC_DIR}/common/*.hpp
)
recursive_subdirs(INCLUDES_WCDB_COMMON ${WCDB_SRC_DIR}/common)

file(GLOB_RECURSE SRC_WCDB_CPP
    ${WCDB_SRC_DIR}/cpp/*.c
    ${WCDB_SRC_DIR}/cpp/*.cpp
    ${WCDB_SRC_DIR}/cpp/*.h
    ${WCDB_SRC_DIR}/cpp/*.hpp
)
list(FILTER SRC_WCDB_CPP EXCLUDE REGEX "(.*/tests/.*)")
recursive_subdirs(INCLUDES_WCDB_CPP ${WCDB_SRC_DIR}/cpp)

file(GLOB_RECURSE SRC_WCDB_BRIDGE
    ${WCDB_SRC_DIR}/bridge/base/*.[ch]
    ${WCDB_SRC_DIR}/bridge/base/*.[ch]pp
    ${WCDB_SRC_DIR}/bridge/cppbridge/*.[ch]
    ${WCDB_SRC_DIR}/bridge/cppbridge/*.[ch]pp
    ${WCDB_SRC_DIR}/bridge/winqbridge/*.[ch]
    ${WCDB_SRC_DIR}/bridge/winqbridge/*.[ch]pp
)
recursive_subdirs(INCLUDES_WCDB_BRIDGE ${WCDB_SRC_DIR}/bridge)


set(WCONAN_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../WeChat/wconan.cmake)
if (NOT SKIP_WCONAN AND EXISTS ${WCONAN_CMAKE_PATH})
    message(STATUS "${WCONAN_CMAKE_PATH} found.")
    set(WCONAN_MODE ON CACHE BOOL "Build with wconan" FORCE)
    include(${WCONAN_CMAKE_PATH})
endif ()

# platform define, available platforms:
# ANDROID, APPLE(MACOS/IOS/...), WIN32, LINUX
# for LINUX: use CMAKE_CROSSCOMPILING to distinguish normal Linux(Ubuntu, Centos, etc) and embedding Linux
# for ANDROID: use ANDROID_ABI to distinguish different arch("arm64-v8a", "armeabi-v7a", etc)
# for WIN32: use CMAKE_SIZEOF_VOID_P to distinguish different arch(X86_64, x86)
if (APPLE)
    message(STATUS "PLATFORM: Apple ${CMAKE_SYSTEM_NAME}")
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(MACOS TRUE)
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "watchOS")
        set(WATCHOS TRUE)
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "tvOS")
        set(TVOS TRUE)
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
        set(IOS TRUE)
    else ()
        message(FATAL_ERROR "CMAKE_SYSTEM_NAME ${CMAKE_SYSTEM_NAME} is not supported")
    endif ()
elseif (ANDROID)
    message(STATUS "PLATFORM: ${ANDROID_PLATFORM} ${ANDROID_ABI}")
    set(ANDROID_ARCH_LIST armeabi-v7a arm64-v8a x86 x86_64)
    if(NOT ${ANDROID_ABI} IN_LIST ANDROID_ARCH_LIST)
        message(FATAL_ERROR "unsupported ANDROID_ABI ${ANDROID_ABI}")
    endif()
elseif (WIN32)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(X86_ARCH "x86_64")
    else ()
        set(X86_ARCH "x86")
    endif ()
    message(STATUS "PLATFORM: Windows ${X86_ARCH}")
elseif (UNIX)
    message(STATUS "PLATFORM: Linux ${CMAKE_SYSTEM_PROCESSOR}")
    set(LINUX TRUE)
else ()
    message(FATAL_ERROR "unsupported platform")
endif ()

if (NOT DEFINED WCONAN_LIB_NAME)
    set(WCONAN_LIB_NAME "WCDB")
endif ()

set(TARGET_NAME ${WCONAN_LIB_NAME})

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ON)
endif ()

if (ANDROID AND NOT DEFINED ANDROID_STL)
    set(ANDROID_STL "c++_shared")
endif ()

# set default compiler flags for each platform
if (WIN32)
    enable_language(C CXX ASM_MASM)
    set(COMM_FLAGS_ "/utf-8 /EHsc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMM_FLAGS_}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMM_FLAGS_}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

    if (CMAKE_CL_64)
        add_definitions(-D_WIN64 -DWIN64)
    endif ()

    add_compile_options(
            $<$<CONFIG:>:/MT> #---------|
            $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
            $<$<CONFIG:Release>:/MT> #--|
    )
else ()
    enable_language(C CXX)
    set(COMM_FLAGS_ "-Wall -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMM_FLAGS_}")
    # xcode 14.0.1 must manually set -std=c++14 since set(CMAKE_CXX_STANDARD 14) has no effect when compiler iOS-Simulator on M1
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMM_FLAGS_} -std=c++14")

    if (LINUX)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -pthread")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread")
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas -Wno-pragmas -Wno-deprecated -Wno-unused-variable")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-pragmas -Wno-deprecated -Wno-unused-variable -fpermissive")
        endif ()
    endif()

    if (APPLE)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-dead_strip")
    else ()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
    endif ()
endif ()

if (UNIX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif ()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_library(z-lib z)
add_library(wcdb-common STATIC ${SRC_WCDB_COMMON})
target_link_libraries(wcdb-common PUBLIC sqlcipher ${z-lib})
target_include_directories(wcdb-common PUBLIC ${INCLUDES_WCDB_COMMON})

add_library(wcdb-cpp STATIC ${SRC_WCDB_CPP})
target_link_libraries(wcdb-cpp PUBLIC wcdb-common)
target_include_directories(wcdb-cpp PUBLIC ${INCLUDES_WCDB_CPP})

add_library(wcdb-bridge STATIC ${SRC_WCDB_BRIDGE})
target_link_libraries(wcdb-bridge PUBLIC wcdb-common)
target_include_directories(wcdb-bridge PUBLIC ${INCLUDES_WCDB_BRIDGE})

#add_library(${TARGET_NAME} SHARED)
#target_link_libraries(${TARGET_NAME} PRIVATE wcdb-cpp wcdb-common)
