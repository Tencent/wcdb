cmake_minimum_required(VERSION 3.13)
project(WCDB)

include(utility.cmake)
include(../sqlcipher/sqlcipher.cmake)

if (NOT DEFINED WCDB_ZSTD)
    set(WCDB_ZSTD ON CACHE BOOL "Build WCDB with zstd" FORCE)
endif ()

set(WCONAN_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../WeChat/wconan.cmake)
if (NOT SKIP_WCONAN AND EXISTS ${WCONAN_CMAKE_PATH})
    message(STATUS "${WCONAN_CMAKE_PATH} found.")
    set(WCONAN_MODE ON CACHE BOOL "Build with wconan" FORCE)
    include(${WCONAN_CMAKE_PATH})
endif ()

if (NOT ANDROID OR WCONAN_MODE OR NOT DEFINED WCDB_CPP)
    set(WCDB_CPP ON CACHE BOOL "Build WCDB with cpp interface" FORCE)
endif ()

# Hide symbols by default
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)

# Add debug info
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

set(WCDB_SRC_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE WCDB_COMMON_SRC
    ${WCDB_SRC_DIR}/common/*.c
    ${WCDB_SRC_DIR}/common/*.cpp
    ${WCDB_SRC_DIR}/common/*.h
    ${WCDB_SRC_DIR}/common/*.hpp
)
recursive_subdirs(WCDB_COMMON_INCLUDES ${WCDB_SRC_DIR}/common)

file(GLOB_RECURSE WCDB_CPP_SRC
    ${WCDB_SRC_DIR}/cpp/*.c
    ${WCDB_SRC_DIR}/cpp/*.cpp
    ${WCDB_SRC_DIR}/cpp/*.h
    ${WCDB_SRC_DIR}/cpp/*.hpp
)
list(FILTER WCDB_CPP_SRC EXCLUDE REGEX "(.*/tests/.*)")
recursive_subdirs(WCDB_CPP_INCLUDES ${WCDB_SRC_DIR}/cpp)

file(GLOB_RECURSE WCDB_BRIDGE_SRC
    ${WCDB_SRC_DIR}/bridge/base/*.[ch]
    ${WCDB_SRC_DIR}/bridge/base/*.[ch]pp
    ${WCDB_SRC_DIR}/bridge/cppbridge/*.[ch]
    ${WCDB_SRC_DIR}/bridge/cppbridge/*.[ch]pp
    ${WCDB_SRC_DIR}/bridge/winqbridge/*.[ch]
    ${WCDB_SRC_DIR}/bridge/winqbridge/*.[ch]pp
)
recursive_subdirs(WCDB_BRIDGE_INCLUDES ${WCDB_SRC_DIR}/bridge)

# Copy all headers to include folder
file(GLOB_RECURSE WCDB_PUBLIC_HEADERS
    ${WCDB_SRC_DIR}/common/*/AggregateFunction.hpp
    ${WCDB_SRC_DIR}/common/*/AuxiliaryFunctionModule.hpp
    ${WCDB_SRC_DIR}/common/*/BaseBinding.hpp
    ${WCDB_SRC_DIR}/common/*/BaseTokenizerUtil.hpp
    ${WCDB_SRC_DIR}/common/*/BindParameter.hpp
    ${WCDB_SRC_DIR}/common/*/CaseInsensitiveList.hpp
    ${WCDB_SRC_DIR}/common/*/Column.hpp
    ${WCDB_SRC_DIR}/common/*/ColumnConstraint.hpp
    ${WCDB_SRC_DIR}/common/*/ColumnDef.hpp
    ${WCDB_SRC_DIR}/common/*/ColumnType.hpp
    ${WCDB_SRC_DIR}/common/*/CommonTableExpression.hpp
    ${WCDB_SRC_DIR}/common/*/Convertible.hpp
    ${WCDB_SRC_DIR}/common/*/ConvertibleImplementation.hpp
    ${WCDB_SRC_DIR}/common/*/CoreFunction.hpp
    ${WCDB_SRC_DIR}/common/*/Data.hpp
    ${WCDB_SRC_DIR}/common/*/WCDBError.hpp
    ${WCDB_SRC_DIR}/common/*/Expression.hpp
    ${WCDB_SRC_DIR}/common/*/ExpressionOperable.hpp
    ${WCDB_SRC_DIR}/common/*/Filter.hpp
    ${WCDB_SRC_DIR}/common/*/ForeignKey.hpp
    ${WCDB_SRC_DIR}/common/*/FrameSpec.hpp
    ${WCDB_SRC_DIR}/common/*/FTSFunction.hpp
    ${WCDB_SRC_DIR}/common/*/FTS5AuxiliaryFunctionTemplate.hpp
    ${WCDB_SRC_DIR}/common/*/FTSConst.h
    ${WCDB_SRC_DIR}/common/*/FTSError.hpp
    ${WCDB_SRC_DIR}/common/*/IndexedColumn.hpp
    ${WCDB_SRC_DIR}/common/*/Join.hpp
    ${WCDB_SRC_DIR}/common/*/JoinConstraint.hpp
    ${WCDB_SRC_DIR}/common/*/LiteralValue.hpp
    ${WCDB_SRC_DIR}/common/*/Macro.h
    ${WCDB_SRC_DIR}/common/*/MemberPointer.hpp
    ${WCDB_SRC_DIR}/common/*/OneOrBinaryTokenizer.hpp
    ${WCDB_SRC_DIR}/common/*/OrderingTerm.hpp
    ${WCDB_SRC_DIR}/common/*/Pragma.hpp
    ${WCDB_SRC_DIR}/common/*/QualifiedTable.hpp
    ${WCDB_SRC_DIR}/common/*/RaiseFunction.hpp
    ${WCDB_SRC_DIR}/common/*/Recyclable.hpp
    ${WCDB_SRC_DIR}/common/*/RecyclableHandle.hpp
    ${WCDB_SRC_DIR}/common/*/ResultColumn.hpp
    ${WCDB_SRC_DIR}/common/*/ScalarFunctionModule.hpp
    ${WCDB_SRC_DIR}/common/*/Schema.hpp
    ${WCDB_SRC_DIR}/common/*/Shadow.hpp
    ${WCDB_SRC_DIR}/common/*/SharedThreadedErrorProne.hpp
    ${WCDB_SRC_DIR}/common/*/SQL.hpp
    ${WCDB_SRC_DIR}/common/*/Statement.hpp
    ${WCDB_SRC_DIR}/common/*/StatementAlterTable.hpp
    ${WCDB_SRC_DIR}/common/*/StatementAnalyze.hpp
    ${WCDB_SRC_DIR}/common/*/StatementAttach.hpp
    ${WCDB_SRC_DIR}/common/*/StatementBegin.hpp
    ${WCDB_SRC_DIR}/common/*/StatementCommit.hpp
    ${WCDB_SRC_DIR}/common/*/StatementCreateIndex.hpp
    ${WCDB_SRC_DIR}/common/*/StatementCreateTable.hpp
    ${WCDB_SRC_DIR}/common/*/StatementCreateTrigger.hpp
    ${WCDB_SRC_DIR}/common/*/StatementCreateView.hpp
    ${WCDB_SRC_DIR}/common/*/StatementCreateVirtualTable.hpp
    ${WCDB_SRC_DIR}/common/*/StatementDelete.hpp
    ${WCDB_SRC_DIR}/common/*/StatementDetach.hpp
    ${WCDB_SRC_DIR}/common/*/StatementDropIndex.hpp
    ${WCDB_SRC_DIR}/common/*/StatementDropTable.hpp
    ${WCDB_SRC_DIR}/common/*/StatementDropTrigger.hpp
    ${WCDB_SRC_DIR}/common/*/StatementDropView.hpp
    ${WCDB_SRC_DIR}/common/*/StatementExplain.hpp
    ${WCDB_SRC_DIR}/common/*/StatementInsert.hpp
    ${WCDB_SRC_DIR}/common/*/StatementPragma.hpp
    ${WCDB_SRC_DIR}/common/*/StatementReindex.hpp
    ${WCDB_SRC_DIR}/common/*/StatementRelease.hpp
    ${WCDB_SRC_DIR}/common/*/StatementRollback.hpp
    ${WCDB_SRC_DIR}/common/*/StatementSavepoint.hpp
    ${WCDB_SRC_DIR}/common/*/StatementSelect.hpp
    ${WCDB_SRC_DIR}/common/*/StatementUpdate.hpp
    ${WCDB_SRC_DIR}/common/*/StatementVacuum.hpp
    ${WCDB_SRC_DIR}/common/*/StringView.hpp
    ${WCDB_SRC_DIR}/common/*/SubstringMatchInfo.hpp
    ${WCDB_SRC_DIR}/common/*/Syntax.h
    ${WCDB_SRC_DIR}/common/*/SyntaxAlterTableSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxAnalyzeSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxAttachSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxBeginSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxBindParameter.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxColumn.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxColumnConstraint.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxColumnDef.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxCommitSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxCommonConst.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxCommonTableExpression.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxCreateIndexSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxCreateTableSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxCreateTriggerSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxCreateViewSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxCreateVirtualTableSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxDeleteSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxDetachSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxDropIndexSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxDropTableSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxDropTriggerSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxDropViewSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxExplainSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxExpression.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxFilter.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxForeignKeyClause.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxForwardDeclaration.h
    ${WCDB_SRC_DIR}/common/*/SyntaxFrameSpec.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxIdentifier.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxIndexedColumn.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxInsertSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxJoinClause.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxJoinConstraint.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxList.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxLiteralValue.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxOrderingTerm.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxPragma.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxPragmaSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxQualifiedTableName.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxRaiseFunction.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxReindexSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxReleaseSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxResultColumn.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxRollbackSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxSavepointSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxSchema.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxSelectCore.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxSelectSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxTableConstraint.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxTableOrSubquery.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxUpdateSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxUpsertClause.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxVacuumSTMT.hpp
    ${WCDB_SRC_DIR}/common/*/SyntaxWindowDef.hpp
    ${WCDB_SRC_DIR}/common/*/TableConstraint.hpp
    ${WCDB_SRC_DIR}/common/*/TableOrSubquery.hpp
    ${WCDB_SRC_DIR}/common/*/Tag.hpp
    ${WCDB_SRC_DIR}/common/*/TokenizerModule.hpp
    ${WCDB_SRC_DIR}/common/*/TokenizerModuleTemplate.hpp
    ${WCDB_SRC_DIR}/common/*/SysTypes.h
    ${WCDB_SRC_DIR}/common/*/UnsafeData.hpp
    ${WCDB_SRC_DIR}/common/*/Upsert.hpp
    ${WCDB_SRC_DIR}/common/*/Value.hpp
    ${WCDB_SRC_DIR}/common/*/ValueArray.hpp
    ${WCDB_SRC_DIR}/common/*/WindowDef.hpp
    ${WCDB_SRC_DIR}/common/*/WINQ.h
    ${WCDB_SRC_DIR}/common/*/WCDBOptional.hpp
    ${WCDB_SRC_DIR}/cpp/*/Accessor.hpp
    ${WCDB_SRC_DIR}/cpp/*/BaseOperation.hpp
    ${WCDB_SRC_DIR}/cpp/*/Binding.hpp
    ${WCDB_SRC_DIR}/cpp/*/Builtin.h
    ${WCDB_SRC_DIR}/cpp/*/ChainCall.hpp
    ${WCDB_SRC_DIR}/cpp/*/CPPBindingMacro.h
    ${WCDB_SRC_DIR}/cpp/*/CPPColumnConstraintMacro.h
    ${WCDB_SRC_DIR}/cpp/*/CPPDeclaration.h
    ${WCDB_SRC_DIR}/cpp/*/CPPFieldMacro.h
    ${WCDB_SRC_DIR}/cpp/*/CPPIndexMacro.h
    ${WCDB_SRC_DIR}/cpp/*/CppInterface.h
    ${WCDB_SRC_DIR}/cpp/*/CPPORM.h
    ${WCDB_SRC_DIR}/cpp/*/CPPORMMacro.h
    ${WCDB_SRC_DIR}/cpp/*/CPPTableConstraintMacro.h
    ${WCDB_SRC_DIR}/cpp/*/CPPVirtualTableMacro.h
    ${WCDB_SRC_DIR}/cpp/*/Database.hpp
    ${WCDB_SRC_DIR}/cpp/*/Delete.hpp
    ${WCDB_SRC_DIR}/cpp/*/Field.hpp
    ${WCDB_SRC_DIR}/cpp/*/FTSTokenizerUtil.hpp
    ${WCDB_SRC_DIR}/cpp/*/Handle.hpp
    ${WCDB_SRC_DIR}/cpp/*/HandleOperation.hpp
    ${WCDB_SRC_DIR}/cpp/*/HandleORMOperation.hpp
    ${WCDB_SRC_DIR}/cpp/*/Insert.hpp
    ${WCDB_SRC_DIR}/cpp/*/Master.hpp
    ${WCDB_SRC_DIR}/cpp/*/MultiObject.hpp
    ${WCDB_SRC_DIR}/cpp/*/MultiSelect.hpp
    ${WCDB_SRC_DIR}/cpp/*/PreparedStatement.hpp
    ${WCDB_SRC_DIR}/cpp/*/ResultField.hpp
    ${WCDB_SRC_DIR}/cpp/*/RunTimeAccessor.hpp
    ${WCDB_SRC_DIR}/cpp/*/STDOptionalAccessor.hpp
    ${WCDB_SRC_DIR}/cpp/*/WCDBOptionalAccessor.hpp
    ${WCDB_SRC_DIR}/cpp/*/SharedPtrAccessor.hpp
    ${WCDB_SRC_DIR}/cpp/*/Select.hpp
    ${WCDB_SRC_DIR}/cpp/*/Sequence.hpp
    ${WCDB_SRC_DIR}/cpp/*/StatementOperation.hpp
    ${WCDB_SRC_DIR}/cpp/*/Table.hpp
    ${WCDB_SRC_DIR}/cpp/*/TableOperation.hpp
    ${WCDB_SRC_DIR}/cpp/*/TableORMOperation.hpp
    ${WCDB_SRC_DIR}/cpp/*/Update.hpp
    ${WCDB_SRC_DIR}/cpp/WCDBCpp.h
)

if (WCONAN_MODE)
    set(EXPORT_PUBLIC_HEADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../WeChat/export_headers)
else ()
    set(EXPORT_PUBLIC_HEADERS_PATH ${CMAKE_CURRENT_BINARY_DIR}/export_headers)
endif()

# export public headers
if (WCDB_CPP) 
    file(REMOVE_RECURSE ${EXPORT_PUBLIC_HEADERS_PATH})
    file(MAKE_DIRECTORY ${EXPORT_PUBLIC_HEADERS_PATH}/WCDB)
    file(COPY ${WCDB_PUBLIC_HEADERS} DESTINATION ${EXPORT_PUBLIC_HEADERS_PATH}/WCDB)
endif()

# platform define, available platforms:
# ANDROID, APPLE(MACOS/IOS/...), WIN32, LINUX
# for LINUX: use CMAKE_CROSSCOMPILING to distinguish normal Linux(Ubuntu, Centos, etc) and embedding Linux
# for ANDROID: use ANDROID_ABI to distinguish different arch("arm64-v8a", "armeabi-v7a", etc)
# for WIN32: use CMAKE_SIZEOF_VOID_P to distinguish different arch(X86_64, x86)
if (APPLE)
    message(STATUS "PLATFORM: Apple ${CMAKE_SYSTEM_NAME}")
    if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(MACOS TRUE)
    elseif (CMAKE_SYSTEM_NAME MATCHES "watchOS")
        set(WATCHOS TRUE)
    elseif (CMAKE_SYSTEM_NAME MATCHES "tvOS")
        set(TVOS TRUE)
    elseif (CMAKE_SYSTEM_NAME MATCHES "iOS")
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
elseif (OHOS)
    message(STATUS "PLATFORM: OpenHarmony")
elseif (UNIX)
    message(STATUS "PLATFORM: Linux ${CMAKE_SYSTEM_PROCESSOR}")
    set(LINUX TRUE)
else ()
    message(FATAL_ERROR "unsupported platform")
endif ()

if (NOT DEFINED WCONAN_LIB_NAME)
    set(WCONAN_LIB_NAME "WCDB")
endif ()

if (NOT DEFINED TARGET_NAME)
    set(TARGET_NAME ${WCONAN_LIB_NAME})
endif ()

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

    if (BUILD_SHARED_LIBS) 
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
        #add_definitions(-DWCDB_DLL -DWCDB_BUILD_DLL)
    endif ()
    
    if (BUILD_SHARED_LIBS)
        target_compile_options(sqlcipher PRIVATE 
            $<$<CONFIG:>:/MD> #---------|
            $<$<CONFIG:Debug>:/MDd> #---|-- Dynamic link the runtime libraries
            $<$<CONFIG:Release>:/MD> #--|
            $<$<CONFIG:RelWithDebInfo>:/MD>
        )
    else ()
        target_compile_options(sqlcipher PRIVATE 
            $<$<CONFIG:>:/MT> #---------|
            $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
            $<$<CONFIG:Release>:/MT> #--|
            $<$<CONFIG:RelWithDebInfo>:/MT>
        )
    endif()
    
    add_compile_options(/MP)
    add_compile_options(/wd4068)  # Disable unknown pragma warnings
    add_compile_options(/wd4996)
    add_compile_options(/wd4715)
    add_compile_options(/wd4251)
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
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-pragmas -Wno-deprecated -Wno-unused-variable")
        endif ()
    elseif (ANDROID)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
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

add_library(${TARGET_NAME} ${WCDB_COMMON_SRC})
target_link_libraries(${TARGET_NAME} PRIVATE sqlcipher)
target_include_directories(${TARGET_NAME} PUBLIC ${WCDB_COMMON_INCLUDES})

if (WCDB_ZSTD)
    include(zstd.cmake)
    target_link_libraries(${TARGET_NAME} PRIVATE zstd)
    target_compile_definitions(${TARGET_NAME} PRIVATE WCDB_ZSTD=1)
endif ()

if (WIN32)
    target_compile_options(${TARGET_NAME} PRIVATE /EHsc)
else ()
    target_compile_options(${TARGET_NAME} PRIVATE -fno-exceptions)
endif()

if (WCDB_CPP)
    target_sources(${TARGET_NAME} PRIVATE ${WCDB_CPP_SRC})
    target_include_directories(${TARGET_NAME} PUBLIC ${WCDB_CPP_INCLUDES})
endif ()

if (WCDB_BRIDGE)
    target_sources(${TARGET_NAME} PRIVATE ${WCDB_BRIDGE_SRC})
    target_include_directories(${TARGET_NAME} PUBLIC ${WCDB_BRIDGE_INCLUDES})
endif ()

if (WCONAN_MODE)
    wconan_get_libraries_dir(
            WCONAN_LIB_DIR
            wconan_libs
    )
    target_include_directories(sqlcipher PUBLIC ${WCONAN_LIB_DIR}/include)
    target_link_directories(sqlcipher PUBLIC ${WCONAN_LIB_DIR}/libs)
else ()
    if (BUILD_SHARED_LIBS)
        if(ANDROID)
            target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/android_old/${CMAKE_ANDROID_ARCH}/include)
            target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/android_old/${CMAKE_ANDROID_ARCH}/lib)
            target_include_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/android_old/${CMAKE_ANDROID_ARCH}/include)
            target_link_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/android_old/${CMAKE_ANDROID_ARCH}/lib)
        elseif(LINUX)
            target_include_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/include)
            target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/include)
            target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/include)
            if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
                target_link_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/linux/x86_64)
                target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/linux/x86_64)
                target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/linux/x86_64)
            elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
                target_link_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/linux/arm64)
                target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/linux/arm64)
                target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/linux/arm64)
            endif()
        elseif(WIN32)
            target_include_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/include)
            target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/include)
            target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/include)
            if (CMAKE_SIZEOF_VOID_P EQUAL 8)
                target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/windows/win64)
                target_link_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/windows/win64)
                target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/windows/win64)
            else ()
                target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/windows/win32)
                target_link_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/windows/win32)
                target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/windows/win32)
            endif()
        elseif(OHOS)
            target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/include)
            target_include_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/include)
            if (${OHOS_ARCH} STREQUAL "arm64-v8a")
                target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/ohos/arm64-v8a)
                target_link_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/ohos/arm64-v8a)
            elseif(${OHOS_ARCH} STREQUAL "x86_64")
                target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/ohos/x86_64)
                target_link_directories(sqlcipher PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/ohos/x86_64)
            endif()
        endif()
    else ()
        if(ANDROID)
            target_include_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/android_old/${CMAKE_ANDROID_ARCH}/include)
            target_link_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/android_old/${CMAKE_ANDROID_ARCH}/lib)
        elseif(LINUX)
            target_include_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/include)
            target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/include)
            target_link_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/linux/arm64)
            target_link_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/linux/x86_64)
            target_link_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/linux/arm64)
            target_link_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/linux/x86_64)
        elseif(WIN32)
            target_include_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/include)
            target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/include)
            if (CMAKE_SIZEOF_VOID_P EQUAL 8)
                target_link_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/windows/win64)
                target_link_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/windows/win64)
            else ()
                target_link_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/zlib/windows/win32)
                target_link_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/windows/win32)
            endif()
        elseif(OHOS)
            target_include_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/include)
            if (${OHOS_ARCH} STREQUAL "arm64-v8a")
                target_link_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/ohos/arm64-v8a)
            elseif(${OHOS_ARCH} STREQUAL "x86_64")
                target_link_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../tools/prebuild/openssl/ohos/x86_64)
            endif()
        endif()
    endif()
endif ()

if (ANDROID)
    message(STATUS "---- BUILD FOR ANDROID ----")
    find_library(z-lib z)
    target_link_libraries(${TARGET_NAME} PUBLIC
            ${LINK_LIBRARIES}
            ${z-lib})
    if (BUILD_SHARED_LIBS)
        target_link_libraries(${TARGET_NAME} PRIVATE crypto)
    else()
        target_link_libraries(${TARGET_NAME} PUBLIC crypto)
    endif()
elseif (APPLE AND NOT WCONAN_MODE)
    message(STATUS "---- BUILD FOR APPLE ----")
    find_library(SecurityFramework Security)
    find_library(FoundationFrameWork Foundation)
    find_library(z-lib z)
    target_link_libraries(${TARGET_NAME} PUBLIC
            ${LINK_LIBRARIES}
            ${SecurityFramework}
            ${FoundationFrameWork}
            ${z-lib})
    target_sources(${TARGET_NAME} PUBLIC ${WCDB_PUBLIC_HEADERS})
    file(STRINGS "../VERSION" WCDB_VERSION)
    message(STATUS "Xcode ${TARGET_NAME}.framework version ${WCDB_VERSION}")
    set_target_properties(${TARGET_NAME} PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION ${WCDB_VERSION}
        MACOSX_FRAMEWORK_IDENTIFIER com.tencent.${TARGET_NAME}
        PUBLIC_HEADER "${WCDB_PUBLIC_HEADERS}"
    )
elseif (OHOS)
    message(STATUS "---- BUILD FOR OPENHARMONY ----")
    target_link_libraries(${TARGET_NAME} PUBLIC
            ${LINK_LIBRARIES}
            pthread
            z)
    if (BUILD_SHARED_LIBS)
        target_link_libraries(${TARGET_NAME} PRIVATE crypto)
    else()
        target_link_libraries(${TARGET_NAME} PUBLIC crypto)
    endif()
elseif (LINUX)
    message(STATUS "---- BUILD FOR LINUX ----")
    target_link_libraries(${TARGET_NAME} PUBLIC
            ${LINK_LIBRARIES}
            pthread
            dl)
    if (BUILD_SHARED_LIBS)
        target_link_libraries(${TARGET_NAME} PRIVATE crypto z)
    else()
        target_link_libraries(${TARGET_NAME} PUBLIC crypto z)
    endif()
elseif (WIN32)
    message(STATUS "---- BUILD FOR WINDOWS ----")
    target_link_libraries(${TARGET_NAME} PUBLIC
            ${LINK_LIBRARIES}
            Crypt32
            Ws2_32)
    if (BUILD_SHARED_LIBS)
        target_link_libraries(${TARGET_NAME} PRIVATE libcrypto zlibstatic)
    else()
        target_link_libraries(${TARGET_NAME} PUBLIC libcrypto zlibstatic)
    endif()
else ()
    message(FATAL_ERROR "Unsupported platform!")
endif ()
