# version 1.0.0
include(CMakeParseArguments)
# read WCONAN_PACKAGE_NAME from wconan_package.yaml
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/wconan_package.yaml")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/wconan_package.yaml" TEMP_YAML)
    string(REGEX MATCH "[ \t]*name[ \t]*:[ \t]*([0-9a-zA-Z\\.\\_\\-]*)" _ ${TEMP_YAML})
    set(WCONAN_PACKAGE_NAME ${CMAKE_MATCH_1})
    message(STATUS "WCONAN_PACKAGE_NAME: ${WCONAN_PACKAGE_NAME}")
else ()
    message(FATAL "wconan_package.yaml NOT FOUND!")
endif ()

if (NOT WCONAN_LIB_NAME)
    string(REPLACE "-" "_" WCONAN_LIB_NAME ${WCONAN_PACKAGE_NAME})
endif ()
if (NOT WCONAN_LIB_VERSION)
    set(WCONAN_LIB_VERSION "0.0.0")
endif ()

message(STATUS "wconan.cmake - WCONAN_PACKAGE_NAME: [${WCONAN_PACKAGE_NAME}], WCONAN_LIB_NAME: [${WCONAN_LIB_NAME}], WCONAN_LIB_VERSION: [${WCONAN_LIB_VERSION}]")

# auto generate lib_version.cpp
# when build with cmake, lib_version.cpp should contain a function like this:
#     extern "C" const char* get_owl_version();
set(SRC_LIB_VERSION ${CMAKE_CURRENT_SOURCE_DIR}/wconan_build/cmake_build/lib_version.c)
execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "GIT_BRANCH ${GIT_BRANCH}, GIT_COMMIT_ID: ${GIT_COMMIT_ID}")
string(TOLOWER ${WCONAN_LIB_NAME} WCONAN_LIB_NAME_LOWER)
file(WRITE "${SRC_LIB_VERSION}" "const char* get_${WCONAN_LIB_NAME_LOWER}_version() { return \"${WCONAN_LIB_NAME} ${WCONAN_LIB_VERSION} ${GIT_BRANCH} ${GIT_COMMIT_ID}(\" __DATE__ \" \" __TIME__ \")\"; }")
set(DEFAULT_GIT_IGNORE ${CMAKE_CURRENT_SOURCE_DIR}/wconan_build/cmake_build/.gitignore)
if (NOT EXISTS "${DEFAULT_GIT_IGNORE}")
    file(WRITE "${DEFAULT_GIT_IGNORE}" "*")
endif ()
# platform define, available platforms:
# ANDROID, APPLE(MACOS/IOS/...), WIN32, LINUX
# for LINUX: use CMAKE_CROSSCOMPILING to distinguish normal Linux(Ubuntu, Centos, etc) and embedding Linux
# for ANDROID: use ANDROID_ABI to distinguish different arch("arm64-v8a", "armeabi-v7a", etc)
# for WIN32: use CMAKE_SIZEOF_VOID_P to distinguish different arch(X86_64, x86)
if (APPLE)
    message(STATUS "WCONAN_PLATFORM: Apple ${CMAKE_SYSTEM_NAME}")
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(MACOS TRUE)
        set(WCONAN_PLATFORM "macos")
        set(WCONAN_LOCAL_PLATFORM "macos")
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "watchOS")
        set(WATCHOS TRUE)
        set(WCONAN_PLATFORM "watchos")
        set(WCONAN_LOCAL_PLATFORM "watchos")
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "tvOS")
        set(TVOS TRUE)
        set(WCONAN_PLATFORM "tvos")
        set(WCONAN_LOCAL_PLATFORM "tvos")
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
        set(IOS TRUE)
        set(WCONAN_PLATFORM "ios")
        set(WCONAN_LOCAL_PLATFORM "ios")
    else ()
        message(FATAL_ERROR "CMAKE_SYSTEM_NAME ${CMAKE_SYSTEM_NAME} is not supported")
    endif ()
elseif (ANDROID)
    message(STATUS "WCONAN_PLATFORM: Android ${ANDROID_ABI}")
    set(WCONAN_PLATFORM "android")
    set(ANDROID_ARCH_LIST armeabi-v7a arm64-v8a x86 x86_64)
    if(${ANDROID_ABI} IN_LIST ANDROID_ARCH_LIST)
        set(WCONAN_LOCAL_PLATFORM "android-${ANDROID_ABI}")
    else()
        message(FATAL_ERROR "unsupported ANDROID_ABI ${ANDROID_ABI}")
    endif()
elseif (WIN32)
    set(WCONAN_PLATFORM "windows")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(X86_ARCH "x86_64")
        set(WCONAN_LOCAL_PLATFORM "win64")
    else ()
        set(X86_ARCH "x86")
        set(WCONAN_LOCAL_PLATFORM "win32")
    endif ()
    message(STATUS "WCONAN_PLATFORM: Windows ${X86_ARCH}")
elseif (UNIX)
    set(WCONAN_PLATFORM "linux")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(X86_ARCH "x86_64")
    else ()
        set(X86_ARCH "x86")
    endif ()
    message(STATUS "WCONAN_PLATFORM: Linux ${X86_ARCH}")
    set(LINUX TRUE)
else ()
    message(FATAL_ERROR "unsupported platform")
endif ()

if (NOT DEFINED WCONAN_TARGET_PLATFORM)
    # WCONAN_TARGET_PLATFORM is defined by wconan, eg
    #     wconan build -cmake . -p linux-x86_64-gcc-5.4.0_adj
    # the WCONAN_TARGET_PLATFORM is linux-x86_64-gcc-5.4.0_adj
    set(WCONAN_TARGET_PLATFORM ${WCONAN_LOCAL_PLATFORM})
endif()
if (LINUX AND (NOT DEFINED WCONAN_TARGET_PLATFORM))
    message(FATAL_ERROR "build linux but WCONAN_TARGET_PLATFORM is NOT DEFINED, please set(WCONAN_TARGET_PLATFORM <linux_platform>) before include(wconan.cmake)")
endif ()

message(STATUS "CMAKE_CROSSCOMPILING: ${CMAKE_CROSSCOMPILING}")
message(STATUS "WCONAN_TARGET_PLATFORM: ${WCONAN_TARGET_PLATFORM}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")

# set default compiler flags for each platform
if (WIN32)
    enable_language(C CXX ASM_MASM)
    set(COMM_FLAGS_ "/utf-8 /EHsc")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMM_FLAGS_}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMM_FLAGS_}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
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
    enable_language(C CXX ASM)
    set(COMM_FLAGS_ "-Wall -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMM_FLAGS_}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMM_FLAGS_}")
    #    if (NOT APPLE)
    #        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
    #    endif ()

    if (BUILD_SHARED_LIBS)
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif ()

    if (APPLE)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-dead_strip")
    else ()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
    endif ()
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(wconan_get_libraries_dir out_libraries_dir download_dir)
    set(libraries_dir "${CMAKE_CURRENT_SOURCE_DIR}/${download_dir}/${CMAKE_BUILD_TYPE}/${WCONAN_TARGET_PLATFORM}")
    set(${out_libraries_dir} ${libraries_dir} PARENT_SCOPE)
    message(STATUS "out_libraries_dir ${libraries_dir}")
endfunction()

#
# import wconan <LIBRARIES> from <DIR> for <TARGET>.
#
# wconan_target_import_libraries(<TARGET> target
#                                <DIR> dir
#                                <LIBRARIES> libs...)
#
function(wconan_target_import_libraries)
    #message(STATUS "ARGC ${ARGC}")
    #message(STATUS "ARGV ${ARGV}")
    #message(STATUS "ARGN ${ARGN}")
    set(prefix WCONAN)
    #set(options IS_A IS_B)
    set(singleValues TARGET DIR)
    set(multiValues LIBRARIES)
    cmake_parse_arguments(PARSE_ARGV 0
            "${prefix}"
            "${options}"
            "${singleValues}"
            "${multiValues}")
    message(STATUS "WCONAN_TARGET ${WCONAN_TARGET}")
    message(STATUS "WCONAN_DIR ${WCONAN_DIR}")
    message(STATUS "WCONAN_LIBRARIES ${WCONAN_LIBRARIES}")

    set(link_libraries)
    wconan_get_libraries_dir(libraries_dir ${WCONAN_DIR})
    set(include_dir ${libraries_dir}/include)
    set(libs_dir ${libraries_dir}/libs)
    set(apple_link_flags -Wl,-F'${libs_dir}')
    target_include_directories(${WCONAN_TARGET} PRIVATE ${include_dir})
    target_link_directories(${WCONAN_TARGET} PRIVATE ${libs_dir})
    target_sources(${WCONAN_TARGET} PRIVATE ${SRC_LIB_VERSION})
    foreach (lib_name ${WCONAN_LIBRARIES})
        string(REPLACE "-" "_" real_lib_name ${lib_name})
        if (EXISTS "${libs_dir}/lib${real_lib_name}.a" OR
                EXISTS "${libs_dir}/${real_lib_name}.lib" OR
                EXISTS "${libs_dir}/lib${real_lib_name}.so" OR
                EXISTS "${libs_dir}/${real_lib_name}.framework")
            if (APPLE AND EXISTS "${libs_dir}/${real_lib_name}.framework")
                list(APPEND link_libraries "-framework ${real_lib_name}")
            else ()
                list(APPEND link_libraries ${real_lib_name})
            endif ()
            message(STATUS "find ${lib_name} -> ${real_lib_name}, OK")
        else ()
            message(STATUS "find ${lib_name} -> ${real_lib_name}, NOT FOUND")
        endif ()
        if (EXISTS "${libs_dir}/${real_lib_name}.framework/Headers")
            #            file(GLOB headers_list "${libs_dir}/${real_lib_name}.framework/Headers")
            #            list(LENGTH headers_list headers_count)
            #            if (headers_count NOT EQUAL 0)
            #                # Headers dir is not empty
            #            endif ()
            target_include_directories(${WCONAN_TARGET} PRIVATE "${libs_dir}/${real_lib_name}.framework")
        endif()
    endforeach ()
    target_link_libraries(${WCONAN_TARGET} ${link_libraries})
    message(STATUS "apple_link_flags ${apple_link_flags}")
    if(APPLE)
        set_target_properties(${WCONAN_TARGET} PROPERTIES
                LINK_FLAGS "${apple_link_flags}"
                )
    endif()
endfunction()