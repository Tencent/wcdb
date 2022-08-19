# auto generate lib version
# when build with cmake, lib_version.cpp should contain a function like this:
#     const char* get_owl_version();
set(SRC_LIB_VERSION ${CMAKE_CURRENT_SOURCE_DIR}/build/lib_version.cpp)

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

message(STATUS "base.cmake - WCONAN_PACKAGE_NAME: [${WCONAN_PACKAGE_NAME}], WCONAN_LIB_NAME: [${WCONAN_LIB_NAME}], WCONAN_LIB_VERSION: [${WCONAN_LIB_VERSION}]")

if (WIN32)
    execute_process(
            COMMAND cmd /C lib_version.bat ${WCONAN_PACKAGE_NAME} ${WCONAN_LIB_NAME} ${WCONAN_LIB_VERSION}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else ()
    execute_process(
            COMMAND bash lib_version.sh ${WCONAN_PACKAGE_NAME} ${WCONAN_LIB_NAME} ${WCONAN_LIB_VERSION}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif ()


# platform define, available platforms:
# ANDROID, APPLE(MACOS/IOS), WIN32, LINUX
# for LINUX: use CMAKE_CROSSCOMPILING to distinguish normal Linux(Ubuntu, Centos, etc) and embedding Linux
# for ANDROID: use ANDROID_ABI to distinguish different arch("arm64-v8a", "armeabi-v7a", etc)
# for WIN32: use CMAKE_SIZEOF_VOID_P to distinguish different arch(X86_64, x86)
if (UNIX AND NOT APPLE AND NOT ANDROID)
    set(LINUX TRUE)
endif ()

if (APPLE)
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
endif ()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(X86_ARCH "x86_64")
    set(WINDOWS_ARCH "win64")
else ()
    set(X86_ARCH "x86")
    set(WINDOWS_ARCH "win32")
endif ()

# print platform string
if (ANDROID)
    message(STATUS "PLATFORM: ANDROID ${ANDROID_ABI}")
endif ()
if (APPLE)
    message(STATUS "PLATFORM: ${CMAKE_SYSTEM_NAME}")
endif ()
if (WIN32)
    message(STATUS "PLATFORM: ${WINDOWS_ARCH}")
endif ()
if (LINUX)
    message(STATUS "PLATFORM: LINUX ${X86_ARCH}")
endif ()
message(STATUS "CMAKE_CROSSCOMPILING: ${CMAKE_CROSSCOMPILING}")
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

if (DEFINED CONAN_COMPILER)
    # TARGETPLATFORM is defined by wconan, eg
    #     wconan publish -p linux-x86_64-gcc-5.4.0_adj
    # the TARGETPLATFORM is linux-x86_64-gcc-5.4.0_adj
    set(WCONAN_TARGET_PLATFORM ${TARGETPLATFORM})
else ()
    if (LINUX AND (NOT DEFINED WCONAN_TARGET_PLATFORM))
        message(FATAL_ERROR "build linux but WCONAN_TARGET_PLATFORM is NOT DEFINED, please set(WCONAN_TARGET_PLATFORM \"<linux_platform>\") before include(base.cmake)")
    endif ()
endif()

#
# find wconan library with name ${lib_name} in ${src} directory.
# return:
# include_dir - library include directory
#    link_dir - library link directory
#
# wconan_find_libraray(include_dir link_dir src ilink-tdi)
#
function(wconan_find_libraray out_include_dir out_link_dir lib_dir lib_name)
    if (WIN32)
        set(SUB_LINK_DIR windows/${WINDOWS_ARCH})
    elseif (MACOS)
        set(SUB_LINK_DIR macos)
    elseif (IOS)
        set(SUB_LINK_DIR ios/all)
    elseif (ANDROID)
        set(SUB_LINK_DIR android/${ANDROID_ABI})
    elseif (LINUX)
        #TODO: replace linux with ${X86_ARCH}
        set(SUB_LINK_DIR ${WCONAN_TARGET_PLATFORM})
    endif ()

    set(${out_include_dir} ${CMAKE_CURRENT_SOURCE_DIR}/${lib_dir}/${lib_name}/${CMAKE_BUILD_TYPE}/include PARENT_SCOPE)
    set(${out_link_dir} ${CMAKE_CURRENT_SOURCE_DIR}/${lib_dir}/${lib_name}/${CMAKE_BUILD_TYPE}/${SUB_LINK_DIR} PARENT_SCOPE)
endfunction()

#
# import wconan libraries into this project.
# return:
# include_src      - all sources in library's include_dir
# apple_link_flags - all link flags need to link APPLE framework
# link_libraries   - all libraries need to link
#
# wconan_import_libraries(include_src apple_link_flags link_libraries src ilink-tdi ilink-network ...)
#
if (DEFINED CONAN_COMPILER)
    function(wconan_import_libraries out_include_src out_apple_link_flags out_link_libraries lib_dir)
        #message(STATUS "ARGC ${ARGC}")
        #message(STATUS "ARGV ${ARGV}")
        #message(STATUS "ARGN ${ARGN}")
        set(link_libraries)
        if (NOT DEFINED WCONAN_BUILD_PATH)
            message(FATAL_ERROR "WCONAN_BUILD_PATH is not defined, please upgrade wconan")
        endif ()
        message(STATUS "WCONAN_BUILD_PATH: ${WCONAN_BUILD_PATH}")
        set(inc_dir ${WCONAN_BUILD_PATH}/third_include)
        set(link_dir ${WCONAN_BUILD_PATH}/third_libs)
        aux_source_directory(${inc_dir} include_src)
        set(apple_link_flags -Wl,-F'${link_dir}')
        include_directories(${inc_dir})
        link_directories(${link_dir})
        foreach (lib_name ${ARGN})
            string(REPLACE "-" "_" real_lib_name ${lib_name})
            if (${lib_name} MATCHES "mars-ilink")
                set(real_lib_name mars)
            endif ()
            if (${lib_name} MATCHES "ilink-id")
                if (APPLE)
                    set(real_lib_name ilinkid)
                else ()
                    set(real_lib_name linkid)
                endif ()
            endif ()

            message(STATUS "lib_name ${lib_name}, real_lib_name ${real_lib_name}")
            message(STATUS "    inc_dir  ${inc_dir}")
            message(STATUS "    link_dir ${link_dir}")
            if (EXISTS "${link_dir}/lib${real_lib_name}.a" OR
                    EXISTS "${link_dir}/${real_lib_name}.lib" OR
                    EXISTS "${link_dir}/lib${real_lib_name}.so" OR
                    EXISTS "${link_dir}/${real_lib_name}.framework")
                if (APPLE)
                    list(APPEND link_libraries "-framework ${real_lib_name}")
                else ()
                    list(APPEND link_libraries ${real_lib_name})
                endif ()
            else ()
                message(STATUS "    NOT EXISTS")
            endif ()
            if (EXISTS "${link_dir}/${real_lib_name}.framework/Headers")
                include_directories(${link_dir}/${real_lib_name}.framework/Headers)
            endif()
        endforeach ()
        set(${out_include_src} ${include_src} PARENT_SCOPE)
        set(${out_apple_link_flags} ${apple_link_flags} PARENT_SCOPE)
        set(${out_link_libraries} ${link_libraries} PARENT_SCOPE)
        message(STATUS "out_include_src ${include_src}")
        message(STATUS "out_apple_link_flags ${apple_link_flags}")
        message(STATUS "out_link_libraries ${link_libraries}")
    endfunction()
else ()
    function(wconan_import_libraries out_include_src out_apple_link_flags out_link_libraries lib_dir)
        #message(STATUS "ARGC ${ARGC}")
        #message(STATUS "ARGV ${ARGV}")
        #message(STATUS "ARGN ${ARGN}")
        set(include_src)
        set(apple_link_flags)
        set(link_libraries)
        foreach (lib_name ${ARGN})
            string(REPLACE "-" "_" real_lib_name ${lib_name})
            if (${lib_name} MATCHES "mars-ilink")
                set(real_lib_name mars)
            endif ()
            if (${lib_name} MATCHES "ilink-id")
                if (APPLE)
                    set(real_lib_name ilinkid)
                else ()
                    set(real_lib_name linkid)
                endif ()
            endif ()
            wconan_find_libraray(inc_dir link_dir ${lib_dir} ${lib_name})
            set(framework_inc_dir ${link_dir}/${real_lib_name}.framework/Headers)
            if (EXISTS "${framework_inc_dir}")
                set(inc_dir ${framework_inc_dir})
            endif ()
            message(STATUS "lib_name ${lib_name}, real_lib_name ${real_lib_name}")
            message(STATUS "    inc_dir  ${inc_dir}")
            message(STATUS "    link_dir ${link_dir}")
            if (EXISTS "${link_dir}/lib${real_lib_name}.a" OR
                    EXISTS "${link_dir}/${real_lib_name}.lib" OR
                    EXISTS "${link_dir}/lib${real_lib_name}.so" OR
                    EXISTS "${link_dir}/${real_lib_name}.framework")
                set(inc_src_list)
                aux_source_directory(${inc_dir} inc_src_list)
                list(APPEND include_src ${inc_src_list})
                set(apple_link_flags "${apple_link_flags} -Wl,-F'${link_dir}'")
                if (APPLE)
                    list(APPEND link_libraries "-framework ${real_lib_name}")
                else ()
                    list(APPEND link_libraries ${real_lib_name})
                endif ()
            else ()
                message(STATUS "    NOT EXISTS")
            endif ()
            include_directories(${inc_dir})
            link_directories(${link_dir})
        endforeach ()
        set(${out_include_src} ${include_src} PARENT_SCOPE)
        set(${out_apple_link_flags} ${apple_link_flags} PARENT_SCOPE)
        set(${out_link_libraries} ${link_libraries} PARENT_SCOPE)
        message(STATUS "out_include_src ${include_src}")
        message(STATUS "out_apple_link_flags ${apple_link_flags}")
        message(STATUS "out_link_libraries ${link_libraries}")
    endfunction()
endif ()

function(wconan_import_ios_libraries out_include_src out_apple_link_flags out_link_libraries lib_dir)
    #message(STATUS "ARGC ${ARGC}")
    #message(STATUS "ARGV ${ARGV}")
    #message(STATUS "ARGN ${ARGN}")
    set(include_src)
    set(apple_link_flags)
    set(link_libraries)
    set(link_dir ${lib_dir})
    link_directories(${link_dir})
    set(apple_link_flags "-Wl,-F'${link_dir}'")
    foreach (lib_name ${ARGN})
        string(REPLACE "-" "_" real_lib_name ${lib_name})
        if (${lib_name} MATCHES "mars-ilink")
            set(real_lib_name mars)
        endif ()
        if (${lib_name} MATCHES "ilink-id")
            if (APPLE)
                set(real_lib_name ilinkid)
            else ()
                set(real_lib_name linkid)
            endif ()
        endif ()
        set(inc_dir ${lib_dir}/${real_lib_name}.framework/Headers)
        message(STATUS "lib_name ${lib_name}, real_lib_name ${real_lib_name}")
        message(STATUS "    inc_dir  ${inc_dir}")
        message(STATUS "    link_dir ${link_dir}")
        if (EXISTS "${link_dir}/lib${real_lib_name}.a" OR
                EXISTS "${link_dir}/lib${real_lib_name}.so" OR
                EXISTS "${link_dir}/${real_lib_name}.framework")
            set(inc_src_list)
            aux_source_directory(${inc_dir} inc_src_list)
            list(APPEND include_src ${inc_src_list})
            if (APPLE)
                list(APPEND link_libraries "-framework ${real_lib_name}")
            else ()
                list(APPEND link_libraries ${real_lib_name})
            endif ()
        else ()
            message(STATUS "    NOT EXISTS")
        endif ()
        include_directories(${inc_dir})
    endforeach ()
    set(${out_include_src} ${include_src} PARENT_SCOPE)
    set(${out_apple_link_flags} ${apple_link_flags} PARENT_SCOPE)
    set(${out_link_libraries} ${link_libraries} PARENT_SCOPE)
    message(STATUS "out_include_src ${include_src}")
    message(STATUS "out_apple_link_flags ${apple_link_flags}")
    message(STATUS "out_link_libraries ${link_libraries}")
endfunction()