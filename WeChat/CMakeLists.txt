cmake_minimum_required(VERSION 3.6)

project(WCDB)

# NOTE: must set this variable when execute
# ./build.sh linux
#
set(WCONAN_TARGET_PLATFORM "linux-x86_64-gcc-5.4.0")
include(base.cmake)

set(TARGET_INSTALL_DIR ".")
set(LIB_FRAMEWORK_IDENTIFIER "com.Tencent.WCDB")

if (NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif ()
if (NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS OFF)
endif ()

if (WIN32)
    if (BUILD_SHARED_LIBS)
        add_definitions(-DWCDB_EXPORTS)
    endif ()
    # disable warning LNK4099: PDB 'xxx.pdb' was not found
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099")
elseif (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -Wno-atomic-memory-ordering -Wno-deprecated-declarations")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-dead_strip")

    # suppress generation of ZERO_CHECK in xcodeproj
    set(CMAKE_SUPPRESS_REGENERATION true)

    # for symbols hidden
    set(CMAKE_XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING "YES")
    set(CMAKE_XCODE_ATTRIBUTE_EXPORTED_SYMBOLS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/export_symbol.list")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
    set(CMAKE_XCODE_ATTRIBUTE_STRIP_STYLE "non-global")
elseif (LINUX)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas -Wno-pragmas -Wno-deprecated -Wno-unused-variable")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-pragmas -Wno-deprecated -Wno-unused-variable")
    endif()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--exclude-libs,ALL -Wl,--gc-sections")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,$ORIGIN")
endif ()

wconan_import_libraries(
        INCLUDE_SRC APPLE_LINK_FLAGS LINK_LIBRARIES
        wconan_libs
        sqlcipher
)

set(WCDB_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../apple)

# Copy all headers to include folder
file(GLOB_RECURSE WCDB_PUBLIC_HEADERS
        ${WCDB_SRC_DIR}/common/*AggregateFunction.hpp
        ${WCDB_SRC_DIR}/common/*AuxiliaryFunctionModule.hpp
        ${WCDB_SRC_DIR}/common/*BindParameter.hpp
        ${WCDB_SRC_DIR}/common/*CaseInsensiveList.hpp
        ${WCDB_SRC_DIR}/common/*Column.hpp
        ${WCDB_SRC_DIR}/common/*ColumnConstraint.hpp
        ${WCDB_SRC_DIR}/common/*ColumnDef.hpp
        ${WCDB_SRC_DIR}/common/*ColumnType.hpp
        ${WCDB_SRC_DIR}/common/*CommonTableExpression.hpp
        ${WCDB_SRC_DIR}/common/*Convertible.hpp
        ${WCDB_SRC_DIR}/common/*ConvertibleImplementation.hpp
        ${WCDB_SRC_DIR}/common/*CoreFunction.hpp
        ${WCDB_SRC_DIR}/common/*Data.hpp
        ${WCDB_SRC_DIR}/common/*Error.hpp
        ${WCDB_SRC_DIR}/common/*Expression.hpp
        ${WCDB_SRC_DIR}/common/*ExpressionOperable.hpp
        ${WCDB_SRC_DIR}/common/*Filter.hpp
        ${WCDB_SRC_DIR}/common/*ForeignKey.hpp
        ${WCDB_SRC_DIR}/common/*FrameSpec.hpp
        ${WCDB_SRC_DIR}/common/*FTS3Function.hpp
        ${WCDB_SRC_DIR}/common/*FTS5AuxiliaryFunctionTemplate.hpp
        ${WCDB_SRC_DIR}/common/*FTSError.hpp
        ${WCDB_SRC_DIR}/common/*IndexedColumn.hpp
        ${WCDB_SRC_DIR}/common/*Join.hpp
        ${WCDB_SRC_DIR}/common/*JoinConstraint.hpp
        ${WCDB_SRC_DIR}/common/*LiteralValue.hpp
        ${WCDB_SRC_DIR}/common/*Macro.h
        ${WCDB_SRC_DIR}/common/*OrderingTerm.hpp
        ${WCDB_SRC_DIR}/common/*Pragma.hpp
        ${WCDB_SRC_DIR}/common/*QualifiedTable.hpp
        ${WCDB_SRC_DIR}/common/*RaiseFunction.hpp
        ${WCDB_SRC_DIR}/common/*Recyclable.hpp
        ${WCDB_SRC_DIR}/common/*RecyclableHandle.hpp
        ${WCDB_SRC_DIR}/common/*ResultColumn.hpp
        ${WCDB_SRC_DIR}/common/*Schema.hpp
        ${WCDB_SRC_DIR}/common/*Shadow.hpp
        ${WCDB_SRC_DIR}/common/*SharedThreadedErrorProne.hpp
        ${WCDB_SRC_DIR}/common/*SQL.hpp
        ${WCDB_SRC_DIR}/common/*Statement.hpp
        ${WCDB_SRC_DIR}/common/*StatementAlterTable.hpp
        ${WCDB_SRC_DIR}/common/*StatementAnalyze.hpp
        ${WCDB_SRC_DIR}/common/*StatementAttach.hpp
        ${WCDB_SRC_DIR}/common/*StatementBegin.hpp
        ${WCDB_SRC_DIR}/common/*StatementCommit.hpp
        ${WCDB_SRC_DIR}/common/*StatementCreateIndex.hpp
        ${WCDB_SRC_DIR}/common/*StatementCreateTable.hpp
        ${WCDB_SRC_DIR}/common/*StatementCreateTrigger.hpp
        ${WCDB_SRC_DIR}/common/*StatementCreateView.hpp
        ${WCDB_SRC_DIR}/common/*StatementCreateVirtualTable.hpp
        ${WCDB_SRC_DIR}/common/*StatementDelete.hpp
        ${WCDB_SRC_DIR}/common/*StatementDetach.hpp
        ${WCDB_SRC_DIR}/common/*StatementDropIndex.hpp
        ${WCDB_SRC_DIR}/common/*StatementDropTable.hpp
        ${WCDB_SRC_DIR}/common/*StatementDropTrigger.hpp
        ${WCDB_SRC_DIR}/common/*StatementDropView.hpp
        ${WCDB_SRC_DIR}/common/*StatementExplain.hpp
        ${WCDB_SRC_DIR}/common/*StatementInsert.hpp
        ${WCDB_SRC_DIR}/common/*StatementPragma.hpp
        ${WCDB_SRC_DIR}/common/*StatementReindex.hpp
        ${WCDB_SRC_DIR}/common/*StatementRelease.hpp
        ${WCDB_SRC_DIR}/common/*StatementRollback.hpp
        ${WCDB_SRC_DIR}/common/*StatementSavepoint.hpp
        ${WCDB_SRC_DIR}/common/*StatementSelect.hpp
        ${WCDB_SRC_DIR}/common/*StatementUpdate.hpp
        ${WCDB_SRC_DIR}/common/*StatementVacuum.hpp
        ${WCDB_SRC_DIR}/common/*StringView.hpp
        ${WCDB_SRC_DIR}/common/*SubstringMatchInfo.hpp
        ${WCDB_SRC_DIR}/common/*Syntax.h
        ${WCDB_SRC_DIR}/common/*SyntaxAlterTableSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxAnalyzeSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxAttachSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxBeginSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxBindParameter.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxColumn.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxColumnConstraint.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxColumnDef.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxCommitSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxCommonConst.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxCommonTableExpression.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxCreateIndexSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxCreateTableSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxCreateTriggerSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxCreateViewSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxCreateVirtualTableSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxDeleteSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxDetachSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxDropIndexSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxDropTableSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxDropTriggerSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxDropViewSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxExplainSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxExpression.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxFilter.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxForeignKeyClause.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxForwardDeclaration.h
        ${WCDB_SRC_DIR}/common/*SyntaxFrameSpec.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxIdentifier.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxIndexedColumn.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxInsertSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxJoinClause.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxJoinConstraint.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxList.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxLiteralValue.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxOrderingTerm.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxPragma.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxPragmaSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxQualifiedTableName.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxRaiseFunction.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxReindexSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxReleaseSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxResultColumn.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxRollbackSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxSavepointSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxSchema.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxSelectCore.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxSelectSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxTableConstraint.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxTableOrSubquery.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxUpdateSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxUpsertClause.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxVacuumSTMT.hpp
        ${WCDB_SRC_DIR}/common/*SyntaxWindowDef.hpp
        ${WCDB_SRC_DIR}/common/*TableConstraint.hpp
        ${WCDB_SRC_DIR}/common/*TableOrSubquery.hpp
        ${WCDB_SRC_DIR}/common/*Tag.hpp
        ${WCDB_SRC_DIR}/common/*TokenizerModule.hpp
        ${WCDB_SRC_DIR}/common/*TokenizerModuleTemplate.hpp
        ${WCDB_SRC_DIR}/common/*UnsafeData.hpp
        ${WCDB_SRC_DIR}/common/*Upsert.hpp
        ${WCDB_SRC_DIR}/common/*Value.hpp
        ${WCDB_SRC_DIR}/common/*ValueArray.hpp
        ${WCDB_SRC_DIR}/common/*WindowDef.hpp
        ${WCDB_SRC_DIR}/common/*WINQ.h
        ${WCDB_SRC_DIR}/cpp/*CppInterface.h
        ${WCDB_SRC_DIR}/cpp/*Database.hpp
        ${WCDB_SRC_DIR}/cpp/*Handle.hpp
        ${WCDB_SRC_DIR}/cpp/*HandleOperation.hpp
        ${WCDB_SRC_DIR}/cpp/*HandleStatement.hpp
        ${WCDB_SRC_DIR}/cpp/*StatementOperation.hpp
        ${WCDB_SRC_DIR}/WCDB.h
        )
#if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.wconan2/output/include/WCDB)
#        file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/.wconan2/output/include/WCDB)
#endif()
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.wconan2/output/include/WCDB)
file(COPY ${WCDB_PUBLIC_HEADERS} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/.wconan2/output/include/WCDB)

file(GLOB_RECURSE WCDB_ALL_HEADERS
        ${WCDB_SRC_DIR}/*.hpp
        ${WCDB_SRC_DIR}/*.h
        )
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/WCDB)
file(COPY ${WCDB_ALL_HEADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/WCDB)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}/include
#        ${WCDB_SRC_DIR}/common/base
#        ${WCDB_SRC_DIR}/common/core
#        ${WCDB_SRC_DIR}/common/core/assemble
#        ${WCDB_SRC_DIR}/common/core/config
#        ${WCDB_SRC_DIR}/common/core/fts
#        ${WCDB_SRC_DIR}/common/core/migration
#        ${WCDB_SRC_DIR}/common/core/operate
#        ${WCDB_SRC_DIR}/common/core/sqlite
#        ${WCDB_SRC_DIR}/common/repair
#        ${WCDB_SRC_DIR}/common/repair/basic
#        ${WCDB_SRC_DIR}/common/repair/crawl
#        ${WCDB_SRC_DIR}/common/repair/deprecated
#        ${WCDB_SRC_DIR}/common/repair/factory
#        ${WCDB_SRC_DIR}/common/repair/mechanic
#        ${WCDB_SRC_DIR}/common/repair/parse
#        ${WCDB_SRC_DIR}/common/repair/sqlite
#        ${WCDB_SRC_DIR}/common/repair/utility
#        ${WCDB_SRC_DIR}/common/utility
#        ${WCDB_SRC_DIR}/common/winq
#        ${WCDB_SRC_DIR}/common/winq/extension
#        ${WCDB_SRC_DIR}/common/winq/identifier
#        ${WCDB_SRC_DIR}/common/winq/statement
#        ${WCDB_SRC_DIR}/common/winq/syntax
#        ${WCDB_SRC_DIR}/common/winq/syntax/const
#        ${WCDB_SRC_DIR}/common/winq/syntax/identifier
#        ${WCDB_SRC_DIR}/common/winq/syntax/stmt
#        ${WCDB_SRC_DIR}/cpp/interface
#        ${WCDB_SRC_DIR}/cpp/platform
)

file(GLOB_RECURSE SRC_WCDB_COMMON
        ${WCDB_SRC_DIR}/common/*.cpp
        ${WCDB_SRC_DIR}/common/*.c
        )
file(GLOB_RECURSE SRC_WCDB_CPP
        ${WCDB_SRC_DIR}/cpp/*.cpp
        ${WCDB_SRC_DIR}/cpp/*.c
        )

set(ALL_SRCS
        ${SRC_WCDB_COMMON}
        ${SRC_WCDB_CPP}
        )

add_compile_options(
        -D_HAVE_SQLITE_CONFIG_H
        -DOMIT_MEM_SECURITY=1
        -DOMIT_MEMLOCK=1
        -DSQLCIPHER_PREPROCESSED=1
        -DSQLITE_CORE=1
        -DSQLITE_DEFAULT_CACHE_SIZE=-2000
        -DSQLITE_DEFAULT_JOURNAL_SIZE_LIMIT=4194304
        -DSQLITE_DEFAULT_LOCKING_MODE=0
        -DSQLITE_DEFAULT_MEMSTATUS=0
        -DSQLITE_DEFAULT_PAGE_SIZE=4096
        -DSQLITE_DEFAULT_SYNCHRONOUS=1
        -DSQLITE_DEFAULT_WAL_AUTOCHECKPOINT=0
        -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1
        -DSQLITE_ENABLE_API_ARMOR=1
        -DSQLITE_ENABLE_BATCH_ATOMIC_WRITE=1
        -DSQLITE_ENABLE_COLUMN_METADATA=1
        -DSQLITE_ENABLE_DBSTAT_VTAB=1
        -DSQLITE_ENABLE_FTS3=1
        -DSQLITE_ENABLE_FTS3_PARENTHESIS=1
        -DSQLITE_ENABLE_FTS3_TOKENIZER=1
        -DSQLITE_ENABLE_FTS5=1
        -DSQLITE_ENABLE_RTREE=1
        -DSQLITE_ENABLE_UPDATE_DELETE_LIMIT=1
        -DSQLITE_HAS_CODEC=1
        -DSQLITE_LIKE_DOESNT_MATCH_BLOBS=1
        -DSQLITE_MALLOC_SOFT_LIMIT=0
        -DSQLITE_MAX_ATTACHED=64
        -DSQLITE_MAX_EXPR_DEPTH=0
        -DSQLITE_MAX_SCHEMA_RETRY=50
        -DSQLITE_MAX_VARIABLE_NUMBER=99
        -DSQLITE_OMIT_BUILTIN_TEST=1
        -DSQLITE_OMIT_COMPILEOPTION_DIAGS=1
        -DSQLITE_OMIT_DEPRECATED=1
        -DSQLITE_OMIT_LOAD_EXTENSION=1
        -DSQLITE_OMIT_PROGRESS_CALLBACK=1
        -DSQLITE_OMIT_SHARED_CACHE=1
        -DSQLITE_PRINT_BUF_SIZE=256
        -DSQLITE_SYSTEM_MALLOC=1
        -DSQLITE_TEMP_STORE=2
        -DSQLITE_THREADSAFE=2
        -DSQLITE_UNTESTABLE=1
        -DSQLITE_USE_ALLOCA=1
        -DSQLITE_WCDB=1
        -DSQLITE_WCDB_CHECKPOINT_HANDLER=1
        -DSQLITE_WCDB_LOCK_HOOK=1
        -DSQLITE_WCDB_SUSPEND=1
        -DUSE_PREAD=1
)

if (WIN32)
    add_compile_options(
            #-DSQLCIPHER_CRYPTO_OPENSSL=1
            -DSQLITE_OS_WIN=1
    )
elseif (APPLE)
    add_compile_options(
            -DSQLCIPHER_CRYPTO_CC=1
            -DSQLITE_OS_UNIX=1
            -DSQLITE_ENABLE_LOCKING_STYLE=1
    )
elseif (LINUX)
    add_compile_options(
            #-DSQLCIPHER_CRYPTO_OPENSSL=1
            -DSQLITE_OS_UNIX=1
    )
elseif (ANDROID)
    add_compile_options(
            #-DSQLCIPHER_CRYPTO_OPENSSL=1
            -DSQLITE_OS_UNIX=1
    )
else ()
endif ()

set(SRC_INCLUDE_FILES ${WCDB_HEADERS})
set_source_files_properties(${SRC_INCLUDE_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
add_library(${WCONAN_LIB_NAME} ${ALL_SRCS} ${SRC_INCLUDE_FILES})

if (WIN32)
    target_link_libraries(${WCONAN_LIB_NAME}
            ${LINK_LIBRARIES}
            z
            )
elseif (APPLE)
    set(MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${WCONAN_LIB_VERSION})
    set(MACOSX_FRAMEWORK_BUNDLE_VERSION ${WCONAN_LIB_VERSION})
    set_target_properties(${WCONAN_LIB_NAME} PROPERTIES
            FRAMEWORK TRUE
            MACOSX_FRAMEWORK_IDENTIFIER ${LIB_FRAMEWORK_IDENTIFIER}
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${LIB_FRAMEWORK_IDENTIFIER}
            XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES"
            XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
            XCODE_ATTRIBUTE_CLANG_DEBUG_INFORMATION_LEVEL[variant=Release] "default"
            XCODE_ATTRIBUTE_CLANG_DEBUG_INFORMATION_LEVEL[variant=Release] "line-tables-only"
            LINK_FLAGS "${APPLE_LINK_FLAGS}"
            PUBLIC_HEADER "${SRC_INCLUDE_FILES}"
            )
    if (MACOS OR IOS)
        target_link_libraries(${WCONAN_LIB_NAME}
                ${LINK_LIBRARIES}
                "-framework Security"
                "-framework Foundation"
                z)
    else ()
        message(FATAL_ERROR "Unsupported apple platform!")
    endif ()
elseif (LINUX)
    target_link_libraries(${WCONAN_LIB_NAME} ${LINK_LIBRARIES} z pthread)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "Detected LP64 System, SQLite int64 set to long")
        target_compile_definitions(${WCONAN_LIB_NAME} PRIVATE
                SQLITE_INT64_TYPE=long
                )
    endif ()
elseif (ANDROID)
    message(STATUS "---- BUILD FOR ANDROID: ANDROID_PLATFORM ${ANDROID_PLATFORM} ----")
    #add_definitions(-DMARS_STATIC_XLOG)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl")
    find_library(z-lib z)
    target_link_libraries(${WCONAN_LIB_NAME}
            ${LINK_LIBRARIES}
            ${z-lib}
            c++_shared
            )
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "Detected LP64 System, SQLite int64 set to long")
        target_compile_definitions(${WCONAN_LIB_NAME} PRIVATE
                SQLITE_INT64_TYPE=long
                )
    endif ()
else ()
    message(FATAL_ERROR "Unsupported platform!")
endif ()

if (DEFINED TARGET_INSTALL_DIR)
    install(TARGETS ${WCONAN_LIB_NAME}
            ARCHIVE DESTINATION ${TARGET_INSTALL_DIR}
            LIBRARY DESTINATION ${TARGET_INSTALL_DIR}
            RUNTIME DESTINATION ${TARGET_INSTALL_DIR}
            FRAMEWORK DESTINATION ${TARGET_INSTALL_DIR}
            )
    if (WIN32 AND BUILD_SHARED_LIBS)
        install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION ${TARGET_INSTALL_DIR} OPTIONAL)
    endif (WIN32 AND BUILD_SHARED_LIBS)
endif (DEFINED TARGET_INSTALL_DIR)