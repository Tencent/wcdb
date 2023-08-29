
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.13)

# Declares and names the project.

project(WCDB_Android)

include(../../../../../utility.cmake)

set(WCDB_JAVA_SRC_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SRC_WCDB_JAVA
    ${WCDB_JAVA_SRC_DIR}/*.c
    ${WCDB_JAVA_SRC_DIR}/*.h)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(wcdb-android SHARED
    ${SRC_WCDB_JAVA}
)

recursive_subdirs(INCLUDES_WCDB_JAVA ${WCDB_JAVA_SRC_DIR})
target_include_directories(wcdb-android PUBLIC ${INCLUDES_WCDB_JAVA})
target_compile_options(wcdb-android PRIVATE -fvisibility=hidden -fno-exceptions -fno-rtti)

# find_library( # Sets the name of the path variable.
#               log-lib

#               # Specifies the name of the NDK library that
#               # you want CMake to locate.
#               log )

target_link_libraries(wcdb-android
    wcdb-bridge
    wcdb-cpp
    wcdb-common
)

add_subdirectory(../../../../.. ${CMAKE_CURRENT_BINARY_DIR}/wcdb)
