set(SQLCIPHER_ROOT ${CMAKE_CURRENT_LIST_DIR}/../sqlcipher)
set(SQLCIPHER_INSTALL_HEADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
message(STATUS ${SQLCIPHER_ROOT})

set(SQLCIPHER_COMMON_DEFS
    SQLITE_HAS_CODEC=1
    SQLITE_UNTESTABLE
    SQLITE_ENABLE_MEMORY_MANAGEMENT=1
    SQLITE_HAVE_ISNAN
    SQLITE_DEFAULT_FILE_FORMAT=4
    SQLITE_THREADSAFE=2
    SQLITE_TEMP_STORE=2
    SQLITE_ENABLE_API_ARMOR
    SQLITE_ENABLE_FTS3
    SQLITE_ENABLE_FTS3_PARENTHESIS
    SQLITE_ENABLE_FTS3_TOKENIZER
    SQLITE_ENABLE_FTS4
    SQLITE_ENABLE_FTS5
    SQLITE_ENABLE_JSON1
    SQLITE_ENABLE_RTREE
    SQLITE_ENABLE_UPDATE_DELETE_LIMIT
    SQLITE_ENABLE_SESSION
    SQLITE_ENABLE_PREUPDATE_HOOK
    SQLITE_DEFAULT_WORKER_THREADS=2
    SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT=1048576
    SQLITE_ENABLE_STAT4
    SQLITE_ENABLE_EXPLAIN_COMMENTS
    SQLITE_ENABLE_DBSTAT_VTAB
    SQLITE_MALLOC_SOFT_LIMIT=0
    SQLITE_ENABLE_COLUMN_METADATA
    SQLITE_LIKE_DOESNT_MATCH_BLOBS
    SQLITE_MAX_ATTACHED=64
    SQLITE_MAX_EXPR_DEPTH=0
    SQLITE_MAX_SCHEMA_RETRY=50
    SQLITE_MAX_VARIABLE_NUMBER=99
    SQLITE_OMIT_BUILTIN_TEST
    SQLITE_OMIT_COMPILEOPTION_DIAGS
    SQLITE_OMIT_DEPRECATED
    SQLITE_PRINT_BUF_SIZE=256
    SQLITE_DEFAULT_CACHE_SIZE=-2000
    SQLITE_DEFAULT_LOCKING_MODE=0
    SQLITE_DEFAULT_MEMSTATUS=0
    SQLITE_DEFAULT_PAGE_SIZE=4096
    SQLITE_DEFAULT_SYNCHRONOUS=1
    SQLITE_DEFAULT_WAL_AUTOCHECKPOINT=0
    SQLITE_DEFAULT_WAL_SYNCHRONOUS=1
    SQLITE_WCDB=1
    SQLITE_WCDB_CHECKPOINT_HANDLER=1
    SQLITE_WCDB_IMPROVED_CHECKPOINT=1
    SQLITE_WCDB_LOCK_HOOK=1
    SQLITE_WCDB_SUSPEND=1
)
set(SQLCIPHER_COMMON_PRIVATE_DEFS
    SQLITE_CORE
    SQLCIPHER_CRYPTO_OPENSSL
    OMIT_MEMLOCK
    OMIT_MEM_SECURITY
    SQLCIPHER_PREPROCESSED=1
    SQLITE_USE_ALLOCA=1
    HAVE_USLEEP=1
    HAVE_FDATASYNC=1
)

if(ANDROID)
    list(APPEND SQLCIPHER_COMMON_PRIVATE_DEFS USE_PREAD64=1)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    list(APPEND SQLCIPHER_COMMON_DEFS SQLITE_INT64_TYPE=long)
endif()

set(SQLCIPHER_SRC
    ${SQLCIPHER_ROOT}/src/alter.c
    ${SQLCIPHER_ROOT}/src/analyze.c
    ${SQLCIPHER_ROOT}/src/attach.c
    ${SQLCIPHER_ROOT}/src/auth.c
    ${SQLCIPHER_ROOT}/src/backup.c
    ${SQLCIPHER_ROOT}/src/bitvec.c
    ${SQLCIPHER_ROOT}/src/btmutex.c
    ${SQLCIPHER_ROOT}/src/btree.c
    ${SQLCIPHER_ROOT}/src/btree.h
    ${SQLCIPHER_ROOT}/src/btreeInt.h
    ${SQLCIPHER_ROOT}/src/build.c
    ${SQLCIPHER_ROOT}/src/callback.c
    ${SQLCIPHER_ROOT}/src/complete.c
    ${SQLCIPHER_ROOT}/src/crypto.c
    ${SQLCIPHER_ROOT}/src/crypto.h
    ${SQLCIPHER_ROOT}/src/crypto_cc.c
    ${SQLCIPHER_ROOT}/src/crypto_impl.c
    ${SQLCIPHER_ROOT}/src/crypto_libtomcrypt.c
    ${SQLCIPHER_ROOT}/src/crypto_openssl.c
    ${SQLCIPHER_ROOT}/src/ctime.c
    ${SQLCIPHER_ROOT}/src/date.c
    ${SQLCIPHER_ROOT}/src/dbpage.c
    ${SQLCIPHER_ROOT}/src/dbstat.c
    ${SQLCIPHER_ROOT}/src/delete.c
    ${SQLCIPHER_ROOT}/src/expr.c
    ${SQLCIPHER_ROOT}/src/fault.c
    ${SQLCIPHER_ROOT}/src/fkey.c
    ${SQLCIPHER_ROOT}/src/func.c
    ${SQLCIPHER_ROOT}/src/global.c
    ${SQLCIPHER_ROOT}/src/hash.c
    ${SQLCIPHER_ROOT}/src/hash.h
    ${SQLCIPHER_ROOT}/src/hwtime.h
    ${SQLCIPHER_ROOT}/src/insert.c
    ${SQLCIPHER_ROOT}/src/legacy.c
    ${SQLCIPHER_ROOT}/src/loadext.c
    ${SQLCIPHER_ROOT}/src/main.c
    ${SQLCIPHER_ROOT}/src/malloc.c
    ${SQLCIPHER_ROOT}/src/mem0.c
    ${SQLCIPHER_ROOT}/src/mem1.c
    ${SQLCIPHER_ROOT}/src/mem2.c
    ${SQLCIPHER_ROOT}/src/mem3.c
    ${SQLCIPHER_ROOT}/src/mem5.c
    ${SQLCIPHER_ROOT}/src/memdb.c
    ${SQLCIPHER_ROOT}/src/memjournal.c
    ${SQLCIPHER_ROOT}/src/msvc.h
    ${SQLCIPHER_ROOT}/src/mutex.c
    ${SQLCIPHER_ROOT}/src/mutex.h
    ${SQLCIPHER_ROOT}/src/mutex_noop.c
    ${SQLCIPHER_ROOT}/src/mutex_unix.c
    ${SQLCIPHER_ROOT}/src/mutex_w32.c
    ${SQLCIPHER_ROOT}/src/notify.c
    ${SQLCIPHER_ROOT}/src/os.c
    ${SQLCIPHER_ROOT}/src/os.h
    ${SQLCIPHER_ROOT}/src/os_common.h
    ${SQLCIPHER_ROOT}/src/os_setup.h
    ${SQLCIPHER_ROOT}/src/os_unix.c
    ${SQLCIPHER_ROOT}/src/os_win.c
    ${SQLCIPHER_ROOT}/src/os_win.h
    ${SQLCIPHER_ROOT}/src/pager.c
    ${SQLCIPHER_ROOT}/src/pager.h
    ${SQLCIPHER_ROOT}/src/pcache.c
    ${SQLCIPHER_ROOT}/src/pcache.h
    ${SQLCIPHER_ROOT}/src/pcache1.c
    ${SQLCIPHER_ROOT}/src/pragma.c
    ${SQLCIPHER_ROOT}/src/pragma.h
    ${SQLCIPHER_ROOT}/src/prepare.c
    ${SQLCIPHER_ROOT}/src/printf.c
    ${SQLCIPHER_ROOT}/src/random.c
    ${SQLCIPHER_ROOT}/src/resolve.c
    ${SQLCIPHER_ROOT}/src/rowset.c
    ${SQLCIPHER_ROOT}/src/select.c
    ${SQLCIPHER_ROOT}/src/sqlcipher.h
    ${SQLCIPHER_ROOT}/src/sqlite3_wcdb.h
    ${SQLCIPHER_ROOT}/src/sqlite3ext.h
    ${SQLCIPHER_ROOT}/src/sqliteInt.h
    ${SQLCIPHER_ROOT}/src/sqliteLimit.h
    ${SQLCIPHER_ROOT}/src/status.c
    ${SQLCIPHER_ROOT}/src/table.c
    ${SQLCIPHER_ROOT}/src/threads.c
    ${SQLCIPHER_ROOT}/src/tokenize.c
    ${SQLCIPHER_ROOT}/src/treeview.c
    ${SQLCIPHER_ROOT}/src/trigger.c
    ${SQLCIPHER_ROOT}/src/update.c
    ${SQLCIPHER_ROOT}/src/upsert.c
    ${SQLCIPHER_ROOT}/src/utf.c
    ${SQLCIPHER_ROOT}/src/util.c
    ${SQLCIPHER_ROOT}/src/vacuum.c
    ${SQLCIPHER_ROOT}/src/vdbe.c
    ${SQLCIPHER_ROOT}/src/vdbe.h
    ${SQLCIPHER_ROOT}/src/vdbeInt.h
    ${SQLCIPHER_ROOT}/src/vdbeapi.c
    ${SQLCIPHER_ROOT}/src/vdbeaux.c
    ${SQLCIPHER_ROOT}/src/vdbeblob.c
    ${SQLCIPHER_ROOT}/src/vdbemem.c
    ${SQLCIPHER_ROOT}/src/vdbesort.c
    ${SQLCIPHER_ROOT}/src/vdbetrace.c
    ${SQLCIPHER_ROOT}/src/vtab.c
    ${SQLCIPHER_ROOT}/src/vxworks.h
    ${SQLCIPHER_ROOT}/src/wal.c
    ${SQLCIPHER_ROOT}/src/wal.h
    ${SQLCIPHER_ROOT}/src/walker.c
    ${SQLCIPHER_ROOT}/src/where.c
    ${SQLCIPHER_ROOT}/src/whereInt.h
    ${SQLCIPHER_ROOT}/src/wherecode.c
    ${SQLCIPHER_ROOT}/src/whereexpr.c
    ${SQLCIPHER_ROOT}/src/window.c
    # Generated source files
    ${SQLCIPHER_ROOT}/sqlite3.h
    ${SQLCIPHER_ROOT}/config.h
    ${SQLCIPHER_ROOT}/parse.c
    ${SQLCIPHER_ROOT}/opcodes.c
)

set(SQLCIPHER_FTS3_SRC
    ${SQLCIPHER_ROOT}/ext/fts3/fts3.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3.h
    ${SQLCIPHER_ROOT}/ext/fts3/fts3Int.h
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_aux.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_expr.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_hash.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_hash.h
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_icu.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_porter.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_snippet.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_term.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_tokenize_vtab.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_tokenizer.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_tokenizer.h
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_tokenizer1.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_unicode.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_unicode2.c
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_write.c
)

set(SQLCIPHER_FTS5_SRC
    # Generated source files
    ${SQLCIPHER_ROOT}/fts5.c
    ${SQLCIPHER_ROOT}/fts5.h
)

set(SQLCIPHER_RBU_SRC
    ${SQLCIPHER_ROOT}/ext/rbu/sqlite3rbu.c
    ${SQLCIPHER_ROOT}/ext/rbu/sqlite3rbu.h
)

set(SQLCIPHER_RTREE_SRC
    ${SQLCIPHER_ROOT}/ext/rtree/rtree.c
    ${SQLCIPHER_ROOT}/ext/rtree/rtree.h
    ${SQLCIPHER_ROOT}/ext/rtree/sqlite3rtree.h
    # Do not include geopoly.c as it will be #includ-ed by rtree.c
)

set(SQLCIPHER_USERAUTH_SRC
    ${SQLCIPHER_ROOT}/ext/userauth/sqlite3userauth.h
    ${SQLCIPHER_ROOT}/ext/userauth/userauth.c
)

set(SQLCIPHER_ICU_SRC
    ${SQLCIPHER_ROOT}/ext/icu/icu.c
    ${SQLCIPHER_ROOT}/ext/icu/sqliteicu.h
)

set(SQLCIPHER_SESSION_SRC
    ${SQLCIPHER_ROOT}/ext/session/sqlite3session.c
    ${SQLCIPHER_ROOT}/ext/session/sqlite3session.h
)

set(SQLCIPHER_MISC_SRC
    # Just pick sources that are instrested
    ${SQLCIPHER_ROOT}/ext/misc/json1.c
)

# Copy header files to export directory
set(SQLCIPHER_INSTALL_HEADERS 
    ${SQLCIPHER_ROOT}/sqlite3.h
    ${SQLCIPHER_ROOT}/src/sqlite3_wcdb.h
    ${SQLCIPHER_ROOT}/ext/fts3/fts3_tokenizer.h
)
file(COPY ${SQLCIPHER_INSTALL_HEADERS} DESTINATION ${SQLCIPHER_INSTALL_HEADER_DIR}/sqlcipher)

add_library(sqlcipher STATIC
    ${SQLCIPHER_SRC}
    ${SQLCIPHER_FTS3_SRC}
    ${SQLCIPHER_FTS5_SRC}
    ${SQLCIPHER_RBU_SRC}
    ${SQLCIPHER_RTREE_SRC}
    ${SQLCIPHER_USERAUTH_SRC}
    ${SQLCIPHER_ICU_SRC}
    ${SQLCIPHER_SESSION_SRC}
    ${SQLCIPHER_MISC_SRC}
)
target_include_directories(sqlcipher INTERFACE ${SQLCIPHER_INSTALL_HEADER_DIR})
target_include_directories(sqlcipher PUBLIC
    ${SQLCIPHER_ROOT}/src
)
target_include_directories(sqlcipher PRIVATE
    ${SQLCIPHER_ROOT}
    ${SQLCIPHER_ROOT}/ext/fts3
    ${SQLCIPHER_ROOT}/ext/rtree
)
target_compile_definitions(sqlcipher PUBLIC ${SQLCIPHER_COMMON_DEFS})
target_compile_definitions(sqlcipher PRIVATE ${SQLCIPHER_COMMON_PRIVATE_DEFS})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(sqlcipher PRIVATE SQLITE_DEBUG)
else()
    target_compile_definitions(sqlcipher PRIVATE NDEBUG)
endif()

# Prebuilt OpenSSL Library
if(ANDROID)
    set(PREBUILT_CRYPTO_LIB 
        ${CMAKE_CURRENT_LIST_DIR}/../prebuilt/android/${CMAKE_ANDROID_ARCH}/lib
    )
    set(PREBUILT_CRYPTO_INCLUDE 
        ${CMAKE_CURRENT_LIST_DIR}/../prebuilt/android/${CMAKE_ANDROID_ARCH}/include
    )
endif()
target_include_directories(sqlcipher PUBLIC ${PREBUILT_CRYPTO_INCLUDE})
target_link_directories(sqlcipher PUBLIC ${PREBUILT_CRYPTO_LIB})
target_link_libraries(sqlcipher PRIVATE crypto)
