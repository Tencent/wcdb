cmake_minimum_required(VERSION 3.6)

project(WCDB)

set(WCONAN_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/wconan.cmake)
if (EXISTS ${WCONAN_CMAKE_PATH})
    set(WCONAN_MODE ON CACHE BOOL "Build with wconan" FORCE)
    include(${WCONAN_CMAKE_PATH})
endif ()

if (NOT DEFINED WCONAN_LIB_NAME)
    set(WCONAN_LIB_NAME "WCDB")
endif ()

set(TARGET_NAME ${WCONAN_LIB_NAME})

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
    # APPLE should be built from xcodeproj!
elseif (LINUX)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas -Wno-pragmas -Wno-deprecated -Wno-unused-variable")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-pragmas -Wno-deprecated -Wno-unused-variable -fpermissive")
    endif ()
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
file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/wconan_build/export_headers)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/wconan_build/export_headers/WCDB)
file(COPY ${WCDB_PUBLIC_HEADERS} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/wconan_build/export_headers/WCDB)

# copy all headers to include
file(GLOB_RECURSE WCDB_ALL_HEADERS
        ${WCDB_SRC_DIR}/*.hpp
        ${WCDB_SRC_DIR}/*.h
        )
file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/include)
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

set(SQLCIPHER_COMPILE_DEFS)
set(SQLCIPHER_SRC_ROOT)
set(LIB_SQLCIPHER sqlcipher)
include(sqlcipher.cmake)

if (WCONAN_MODE)
    wconan_get_libraries_dir(
            WCONAN_LIB_DIR
            wconan_libs
    )
    target_include_directories(${LIB_SQLCIPHER} PUBLIC ${WCONAN_LIB_DIR}/include)
    target_link_directories(${LIB_SQLCIPHER} PUBLIC ${WCONAN_LIB_DIR}/libs)
    target_link_libraries(${LIB_SQLCIPHER} PUBLIC crypto)

    set(SQLCIPHER_SYSTEM_DEFS)
    set(SQLCIPHER_SYSTEM_SRC)

    if (ANDROID OR LINUX)
        list(APPEND SQLCIPHER_SYSTEM_DEFS
                SQLCIPHER_CRYPTO_OPENSSL=1
                SQLITE_OS_UNIX=1)
        list(APPEND SQLCIPHER_SYSTEM_SRC
                ${SQLCIPHER_SRC_ROOT}/src/crypto.c
                ${SQLCIPHER_SRC_ROOT}/src/crypto_impl.c
                ${SQLCIPHER_SRC_ROOT}/src/crypto_openssl.c
                ${SQLCIPHER_SRC_ROOT}/src/mutex_unix.c
                ${SQLCIPHER_SRC_ROOT}/src/os_unix.c)
    elseif (WIN32)
        list(APPEND SQLCIPHER_SYSTEM_DEFS
                SQLCIPHER_CRYPTO_OPENSSL=1
                SQLITE_OS_WIN=1)
        list(APPEND SQLCIPHER_SYSTEM_SRC
                ${SQLCIPHER_SRC_ROOT}/src/crypto.c
                ${SQLCIPHER_SRC_ROOT}/src/crypto_impl.c
                ${SQLCIPHER_SRC_ROOT}/src/crypto_openssl.c
                ${SQLCIPHER_SRC_ROOT}/src/mutex_w32.c
                ${SQLCIPHER_SRC_ROOT}/src/os_win.c)
    endif ()

    target_compile_definitions(${LIB_SQLCIPHER} PRIVATE
            ${SQLCIPHER_SYSTEM_DEFS})
    target_sources(${LIB_SQLCIPHER} PRIVATE
            ${SQLCIPHER_SYSTEM_SRC})
endif ()

add_library(${TARGET_NAME} ${ALL_SRCS})

target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/include)

target_link_libraries(${TARGET_NAME} PRIVATE
        ${LIB_SQLCIPHER})

if (ANDROID)
    message(STATUS "---- BUILD FOR ANDROID ----")
    find_library(z-lib z)
    target_link_libraries(${TARGET_NAME} PRIVATE
            ${LINK_LIBRARIES}
            ${z-lib})
elseif (LINUX)
    message(STATUS "---- BUILD FOR LINUX ----")
    target_link_libraries(${TARGET_NAME} PRIVATE
            ${LINK_LIBRARIES}
            pthread
            z)
elseif (WIN32)
    message(STATUS "---- BUILD FOR WINDOWS ----")
    target_link_libraries(${TARGET_NAME} PRIVATE
            ${LINK_LIBRARIES}
            z)
else ()
    message(FATAL_ERROR "Unsupported platform!")
endif ()