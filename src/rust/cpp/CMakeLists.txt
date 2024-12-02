cmake_minimum_required(VERSION 3.21)
project(WCDBRust)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_ARCHITECTURES "arm64")

include(../../utility.cmake)

set(TARGET_NAME "WCDB")
set(WCDB_BRIDGE ON)
set(SKIP_WCONAN ON)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/wcdb)

set(WCDB_RUST_SRC_DIR
        ${CMAKE_CURRENT_LIST_DIR}/base
        ${CMAKE_CURRENT_LIST_DIR}/core
        ${CMAKE_CURRENT_LIST_DIR}/winq)

set(WCDB_RUST_SRC)
set(WCDB_RUST_INCLUDE)

foreach(DIR ${WCDB_RUST_SRC_DIR})
    file(GLOB_RECURSE DIR_SRC
            ${DIR}/*.cpp
            ${DIR}/*.c
            ${DIR}/*.h)
    list(APPEND WCDB_RUST_SRC ${DIR_SRC})

    recursive_subdirs(DIR_INCLUDE ${DIR})
    list(APPEND WCDB_RUST_INCLUDE ${DIR_INCLUDE})
endforeach()

target_sources(${TARGET_NAME} PUBLIC ${WCDB_RUST_SRC})
target_include_directories(${TARGET_NAME} PUBLIC ${WCDB_RUST_INCLUDE})
