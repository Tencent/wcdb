# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.6)

# Declares and names the project.

project(WCDB)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
include(wconan.cmake)

set(TARGET_NAME ${WCONAN_LIB_NAME})
set(TARGET_INSTALL_DIR ".")

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS OFF)
endif ()

# Platform dependent linker settings
if (ANDROID)
    # No linker flag to add
elseif (APPLE)
    set(LIB_FRAMEWORK_IDENTIFIER "com.Tencent.WCDB")
    #add_definitions(-DFORCE_POSIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-atomic-memory-ordering -Wno-deprecated-declarations")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-dead_strip")
    # suppress generation of ZERO_CHECK in xcodeproj
    set(CMAKE_SUPPRESS_REGENERATION true)
elseif (LINUX)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas -Wno-pragmas -Wno-deprecated -Wno-unused-variable")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-pragmas -Wno-deprecated -Wno-unused-variable -fpermissive")
    endif ()
    #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,'$ORIGIN'")
elseif (WIN32)
    # TODO
endif ()

set(WCDB_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../apple)

# Copy all headers to include folder
file(GLOB_RECURSE WCDB_PUBLIC_HEADERS
        ${WCDB_SRC_DIR}/common/*AggregateFunction.hpp
        ${WCDB_SRC_DIR}/common/*AuxiliaryFunctionModule.hpp
        ${WCDB_SRC_DIR}/common/*BaseBinding.hpp
        ${WCDB_SRC_DIR}/common/*BaseTokenizerUtil.hpp
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
        ${WCDB_SRC_DIR}/common/*FTSConst.h
        ${WCDB_SRC_DIR}/common/*FTSError.hpp
        ${WCDB_SRC_DIR}/common/*IndexedColumn.hpp
        ${WCDB_SRC_DIR}/common/*Join.hpp
        ${WCDB_SRC_DIR}/common/*JoinConstraint.hpp
        ${WCDB_SRC_DIR}/common/*LiteralValue.hpp
        ${WCDB_SRC_DIR}/common/*Macro.h
        ${WCDB_SRC_DIR}/common/*OneOrBinaryTokenizer.hpp
        ${WCDB_SRC_DIR}/common/*OrderingTerm.hpp
        ${WCDB_SRC_DIR}/common/*Pragma.hpp
        ${WCDB_SRC_DIR}/common/*QualifiedTable.hpp
        ${WCDB_SRC_DIR}/common/*RaiseFunction.hpp
        ${WCDB_SRC_DIR}/common/*Recyclable.hpp
        ${WCDB_SRC_DIR}/common/*RecyclableHandle.hpp
        ${WCDB_SRC_DIR}/common/*ResultColumn.hpp
        ${WCDB_SRC_DIR}/common/*Schema.hpp
        ${WCDB_SRC_DIR}/common/*Sequence.hpp
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
        ${WCDB_SRC_DIR}/common/*WCTBinding.h
        ${WCDB_SRC_DIR}/common/*WindowDef.hpp
        ${WCDB_SRC_DIR}/common/*WINQ.h
        ${WCDB_SRC_DIR}/cpp/*Accessor.hpp
        ${WCDB_SRC_DIR}/cpp/*BaseOperation.hpp
        ${WCDB_SRC_DIR}/cpp/*Binding.hpp
        ${WCDB_SRC_DIR}/cpp/*ChainCall.hpp
        ${WCDB_SRC_DIR}/cpp/*CPPBindingMacro.h
        ${WCDB_SRC_DIR}/cpp/*CPPColumnConstraintMacro.h
        ${WCDB_SRC_DIR}/cpp/*CPPDeclaration.h
        ${WCDB_SRC_DIR}/cpp/*CPPFieldMacro.h
        ${WCDB_SRC_DIR}/cpp/*CPPIndexMacro.h
        ${WCDB_SRC_DIR}/cpp/*CppInterface.h
        ${WCDB_SRC_DIR}/cpp/*CPPORM.h
        ${WCDB_SRC_DIR}/cpp/*CPPORMMacro.h
        ${WCDB_SRC_DIR}/cpp/*CPPTableConstraintMacro.h
        ${WCDB_SRC_DIR}/cpp/*CPPVirtualTableMacro.h
        ${WCDB_SRC_DIR}/cpp/*Database.hpp
        ${WCDB_SRC_DIR}/cpp/*Delete.hpp
        ${WCDB_SRC_DIR}/cpp/*Field.hpp
        ${WCDB_SRC_DIR}/cpp/*FTSTokenizerUtil.hpp
        ${WCDB_SRC_DIR}/cpp/*Handle.hpp
        ${WCDB_SRC_DIR}/cpp/*HandleOperation.hpp
        ${WCDB_SRC_DIR}/cpp/*HandleORMOperation.hpp
        ${WCDB_SRC_DIR}/cpp/*HandleStatement.hpp
        ${WCDB_SRC_DIR}/cpp/*Insert.hpp
        ${WCDB_SRC_DIR}/cpp/*Master.hpp
        ${WCDB_SRC_DIR}/cpp/*MemberPointer.hpp
        ${WCDB_SRC_DIR}/cpp/*MultiObject.hpp
        ${WCDB_SRC_DIR}/cpp/*MultiSelect.hpp
        ${WCDB_SRC_DIR}/cpp/*ResultField.hpp
        ${WCDB_SRC_DIR}/cpp/*RunTimeAccessor.hpp
        ${WCDB_SRC_DIR}/cpp/*Select.hpp
        ${WCDB_SRC_DIR}/cpp/*StatementOperation.hpp
        ${WCDB_SRC_DIR}/cpp/*Table.hpp
        ${WCDB_SRC_DIR}/cpp/*TableOperation.hpp
        ${WCDB_SRC_DIR}/cpp/*TableORMOperation.hpp
        ${WCDB_SRC_DIR}/cpp/*Update.hpp
        ${WCDB_SRC_DIR}/cpp/*WCDBCpp.h
        )
# export public headers
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.wconan2/include/WCDB)
file(COPY ${WCDB_PUBLIC_HEADERS} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/.wconan2/include/WCDB)

# copy all headers to include
file(GLOB_RECURSE WCDB_ALL_HEADERS
        ${WCDB_SRC_DIR}/*.hpp
        ${WCDB_SRC_DIR}/*.h
        )
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/WCDB)
file(COPY ${WCDB_ALL_HEADERS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/WCDB)

file(GLOB_RECURSE SRC_WCDB_COMMON
        ${WCDB_SRC_DIR}/common/*.cpp
        ${WCDB_SRC_DIR}/common/*.c
        )
file(GLOB_RECURSE SRC_WCDB_CPP
        ${WCDB_SRC_DIR}/cpp/*.cpp
        ${WCDB_SRC_DIR}/cpp/*.c
        )
list(FILTER SRC_WCDB_CPP EXCLUDE REGEX
        "(.*/tests/.*)"
        )

set(ALL_SRCS
        ${SRC_WCDB_COMMON}
        ${SRC_WCDB_CPP}
        )

add_library(${TARGET_NAME} ${ALL_SRCS})

wconan_target_import_libraries(
        TARGET ${TARGET_NAME}
        DIR wconan_libs
        LIBRARIES sqlcipher
)

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/include
        )

if (ANDROID)
    target_compile_definitions(${TARGET_NAME} PUBLIC
            HAVE_USLEEP=1
            OMIT_MEM_SECURITY=1
            OMIT_MEMLOCK=1
            SQLCIPHER_PREPROCESSED=1
            SQLITE_CORE=1
            SQLITE_DEFAULT_CACHE_SIZE=-2000
            SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT=4194304
            SQLITE_DEFAULT_LOCKING_MODE=0
            SQLITE_DEFAULT_MEMSTATUS=0
            SQLITE_DEFAULT_PAGE_SIZE=4096
            SQLITE_DEFAULT_SYNCHRONOUS=1
            SQLITE_DEFAULT_WAL_AUTOCHECKPOINT=100
            SQLITE_DEFAULT_WAL_SYNCHRONOUS=1
            SQLITE_ENABLE_COLUMN_METADATA=1
            SQLITE_ENABLE_DBSTAT_VTAB=1
            SQLITE_ENABLE_FTS3=1
            SQLITE_ENABLE_FTS3_PARENTHESIS=1
            SQLITE_ENABLE_FTS3_TOKENIZER=1
            SQLITE_ENABLE_FTS5=1
            SQLITE_ENABLE_JSON1=1
            SQLITE_ENABLE_MEMORY_MANAGEMENT=1
            SQLITE_ENABLE_PREUPDATE_HOOK=1
            SQLITE_ENABLE_RTREE=1
            SQLITE_ENABLE_SESSION=1
            SQLITE_ENABLE_STAT4=1
            SQLITE_ENABLE_UPDATE_DELETE_LIMIT=1
            SQLITE_HAS_CODEC=1
            SQLITE_HAVE_ISNAN=1
            SQLITE_LIKE_DOESNT_MATCH_BLOBS=1
            SQLITE_MALLOC_SOFT_LIMIT=0
            SQLITE_MAX_ATTACHED=64
            SQLITE_MAX_EXPR_DEPTH=0
            SQLITE_MAX_SCHEMA_RETRY=50
            SQLITE_MAX_VARIABLE_NUMBER=99
            SQLITE_OMIT_BUILTIN_TEST=1
            SQLITE_OMIT_COMPILEOPTION_DIAGS=1
            SQLITE_OMIT_DEPRECATED=1
            SQLITE_PRINT_BUF_SIZE=256
            SQLITE_SYSTEM_MALLOC=1
            SQLITE_TEMP_STORE=2
            SQLITE_THREADSAFE=2
            SQLITE_UNTESTABLE=1
            SQLITE_WCDB=1
            SQLITE_WCDB_CHECKPOINT_HANDLER=1
            SQLITE_WCDB_IMPROVED_CHECKPOINT=1
            SQLITE_WCDB_LOCK_HOOK=1
            SQLITE_WCDB_SUSPEND=1
            USE_PREAD64=1
            )
else()
    target_compile_definitions(${TARGET_NAME} PUBLIC
            _HAVE_SQLITE_CONFIG_H
            OMIT_MEM_SECURITY=1
            OMIT_MEMLOCK=1
            SQLCIPHER_PREPROCESSED=1
            SQLITE_CORE=1
            SQLITE_DEFAULT_CACHE_SIZE=-2000
            SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT=4194304
            SQLITE_DEFAULT_LOCKING_MODE=0
            SQLITE_DEFAULT_MEMSTATUS=0
            SQLITE_DEFAULT_PAGE_SIZE=4096
            SQLITE_DEFAULT_SYNCHRONOUS=1
            SQLITE_DEFAULT_WAL_AUTOCHECKPOINT=0
            SQLITE_DEFAULT_WAL_SYNCHRONOUS=1
            SQLITE_ENABLE_API_ARMOR=1
            SQLITE_ENABLE_BATCH_ATOMIC_WRITE=1
            SQLITE_ENABLE_COLUMN_METADATA=1
            SQLITE_ENABLE_DBSTAT_VTAB=1
            SQLITE_ENABLE_FTS3=1
            SQLITE_ENABLE_FTS3_PARENTHESIS=1
            SQLITE_ENABLE_FTS3_TOKENIZER=1
            SQLITE_ENABLE_FTS5=1
            SQLITE_ENABLE_RTREE=1
            SQLITE_ENABLE_UPDATE_DELETE_LIMIT=1
            SQLITE_HAS_CODEC=1
            SQLITE_LIKE_DOESNT_MATCH_BLOBS=1
            SQLITE_MALLOC_SOFT_LIMIT=0
            SQLITE_MAX_ATTACHED=64
            SQLITE_MAX_EXPR_DEPTH=0
            SQLITE_MAX_SCHEMA_RETRY=50
            SQLITE_MAX_VARIABLE_NUMBER=99
            SQLITE_OMIT_BUILTIN_TEST=1
            SQLITE_OMIT_COMPILEOPTION_DIAGS=1
            SQLITE_OMIT_DEPRECATED=1
            SQLITE_OMIT_LOAD_EXTENSION=1
            SQLITE_OMIT_PROGRESS_CALLBACK=1
            SQLITE_OMIT_SHARED_CACHE=1
            SQLITE_PRINT_BUF_SIZE=256
            SQLITE_SYSTEM_MALLOC=1
            SQLITE_TEMP_STORE=2
            SQLITE_THREADSAFE=2
            SQLITE_UNTESTABLE=1
            SQLITE_USE_ALLOCA=1
            SQLITE_WCDB=1
            SQLITE_WCDB_CHECKPOINT_HANDLER=1
            SQLITE_WCDB_LOCK_HOOK=1
            SQLITE_WCDB_SUSPEND=1
            USE_PREAD=1
            SQLITE_WCDB_RECOVER_NBACKFILL=1
            )
endif()

# Platform dependent settings
if (ANDROID)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "Detected LP64 System, SQLite int64 set to long")
        target_compile_definitions(${TARGET_NAME} PRIVATE
                SQLITE_INT64_TYPE=long
                )
    endif ()
elseif (APPLE)
    # TODO
elseif (LINUX)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "Detected LP64 System, SQLite int64 set to long")
        target_compile_definitions(${TARGET_NAME} PRIVATE
                SQLITE_INT64_TYPE=long
                )
    endif ()
elseif (WIN32)
    # TODO
else ()
endif ()

#set(WCONAN_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wconan_libs/${CMAKE_BUILD_TYPE}/${CMAKE_BUILD_TYPE}/libs)
#message(STATUS "WCONAN_LIBS_DIR: ${WCONAN_LIBS_DIR}")
#set(WCONAN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wconan_libs/${CMAKE_BUILD_TYPE}/${CMAKE_BUILD_TYPE}/include)
#message(STATUS "WCONAN_INCLUDE_DIR: ${WCONAN_INCLUDE_DIR}")

if (ANDROID)
    message(STATUS "---- BUILD FOR ANDROID ----")
    find_library(z-lib z)
    target_link_libraries(${TARGET_NAME}
            ${LINK_LIBRARIES}
            ${z-lib}
            crypto)
elseif (APPLE)
    message(STATUS "---- BUILD FOR APPLE ----")
    set(MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${WCONAN_LIB_VERSION})
    set(MACOSX_FRAMEWORK_BUNDLE_VERSION ${WCONAN_LIB_VERSION})
    set_target_properties(${TARGET_NAME} PROPERTIES
            FRAMEWORK TRUE
            MACOSX_FRAMEWORK_IDENTIFIER ${LIB_FRAMEWORK_IDENTIFIER}
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${LIB_FRAMEWORK_IDENTIFIER}
            XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS "YES"
            XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
            XCODE_ATTRIBUTE_CLANG_DEBUG_INFORMATION_LEVEL[variant=Release] "default"
            XCODE_ATTRIBUTE_CLANG_DEBUG_INFORMATION_LEVEL[variant=Release] "line-tables-only"
            LINK_FLAGS "${APPLE_LINK_FLAGS}"
            # use wconan2 to export headers
            #PUBLIC_HEADER "${ALL_FRAMEWORK_HEADERS}"
            )
    target_link_libraries(${TARGET_NAME} ${LINK_LIBRARIES}
            "-framework Security"
            "-framework Foundation"
            z)
elseif (LINUX)
    message(STATUS "---- BUILD FOR LINUX ----")
    target_link_libraries(${TARGET_NAME} ${LINK_LIBRARIES}
            pthread
            z
            crypto)
elseif (WIN32)
    message(STATUS "---- BUILD FOR WINDOWS ----")
    target_link_libraries(${TARGET_NAME} ${LINK_LIBRARIES}
            z
            crypto)
else ()
    message(FATAL_ERROR "Unsupported platform!")
endif ()