set(ZSTD_ROOT ${CMAKE_CURRENT_LIST_DIR}/../zstd)
set(ZSTD_INSTALL_HEADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
message(STATUS ${ZSTD_ROOT})

enable_language(ASM)
set(CMAKE_ASM_FLAGS "${CFLAGS} -x assembler-with-cpp")

set(ZSTD_SRC
    ${ZSTD_ROOT}/lib/common/entropy_common.c
    ${ZSTD_ROOT}/lib/common/fse_decompress.c
    ${ZSTD_ROOT}/lib/common/debug.c
    ${ZSTD_ROOT}/lib/common/xxhash.c
    ${ZSTD_ROOT}/lib/common/pool.c
    ${ZSTD_ROOT}/lib/common/threading.c
    ${ZSTD_ROOT}/lib/common/zstd_common.c
    ${ZSTD_ROOT}/lib/common/error_private.c
    ${ZSTD_ROOT}/lib/compress/zstd_compress_superblock.c
    ${ZSTD_ROOT}/lib/compress/zstdmt_compress.c
    ${ZSTD_ROOT}/lib/compress/zstd_double_fast.c
    ${ZSTD_ROOT}/lib/compress/zstd_fast.c
    ${ZSTD_ROOT}/lib/compress/zstd_compress_sequences.c
    ${ZSTD_ROOT}/lib/compress/zstd_ldm.c
    ${ZSTD_ROOT}/lib/compress/hist.c
    ${ZSTD_ROOT}/lib/compress/zstd_compress.c
    ${ZSTD_ROOT}/lib/compress/zstd_lazy.c
    ${ZSTD_ROOT}/lib/compress/zstd_compress_literals.c
    ${ZSTD_ROOT}/lib/compress/huf_compress.c
    ${ZSTD_ROOT}/lib/compress/zstd_opt.c
    ${ZSTD_ROOT}/lib/compress/fse_compress.c
    ${ZSTD_ROOT}/lib/decompress/zstd_ddict.c
    ${ZSTD_ROOT}/lib/decompress/huf_decompress.c
    ${ZSTD_ROOT}/lib/decompress/zstd_decompress.c
    ${ZSTD_ROOT}/lib/decompress/huf_decompress_amd64.S
    ${ZSTD_ROOT}/lib/decompress/zstd_decompress_block.c
    ${ZSTD_ROOT}/lib/dictBuilder/cover.c
    ${ZSTD_ROOT}/lib/dictBuilder/divsufsort.c
    ${ZSTD_ROOT}/lib/dictBuilder/fastcover.c
    ${ZSTD_ROOT}/lib/dictBuilder/zdict.c
)

# Copy header files to export directory
set(ZSTD_INSTALL_HEADERS 
    ${ZSTD_ROOT}/lib/zstd.h
    ${ZSTD_ROOT}/lib/zdict.h
    ${ZSTD_ROOT}/lib/zstd_errors.h
)
file(COPY ${ZSTD_INSTALL_HEADERS} DESTINATION ${ZSTD_INSTALL_HEADER_DIR}/zstd)

add_library(zstd STATIC
    ${ZSTD_SRC}
)
target_include_directories(zstd INTERFACE ${ZSTD_INSTALL_HEADER_DIR})
target_include_directories(zstd PUBLIC
    ${ZSTD_ROOT}/lib/common
    ${ZSTD_ROOT}/lib/compress
    ${ZSTD_ROOT}/lib/decompress
    ${ZSTD_ROOT}/lib/dictBuilder
)

if(WIN32)
    target_compile_options(zstd PRIVATE /W4)
else()
    target_compile_options(zstd PRIVATE -Wall -Wno-unused -fvisibility=hidden)
endif()
