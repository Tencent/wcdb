//
// Created by qiuwenchen on 2022/3/30.
//

/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdbool.h>

#ifndef WCDB_EXTERN
#if defined(__cplusplus)
#define WCDB_EXTERN extern "C"
#define WCDB_EXTERN_C_BEGIN extern "C" {
#define WCDB_EXTERN_C_END }
#else
#define WCDB_EXTERN extern
#define WCDB_EXTERN_C_BEGIN
#define WCDB_EXTERN_C_END
#endif
#endif

WCDB_EXTERN_C_BEGIN

typedef void SwiftObject;
extern void (*_Nullable WCDBReleaseSwiftObject)(SwiftObject* _Nonnull obj);

typedef struct CPPObject {
    void* _Nonnull realValue;
    void (*_Nullable deleter)(void* _Nonnull);
    bool isRecyclableObj;
} CPPObject;
void WCDBReleaseCPPObject(CPPObject* _Nonnull obj);

typedef void (*WCDBContextDestructor)(void* _Nonnull context);

#define WCDBDefineSwiftObjectBridgedType(typename)                             \
    typedef struct typename                                                    \
    {                                                                          \
        WCDB::Recyclable<SwiftObject*> innerValue;                             \
    }                                                                          \
    typename;

#define WCDBDefineCPPBridgedType(typename)                                     \
    typedef struct typename                                                    \
    {                                                                          \
        CPPObject* _Nullable innerValue;                                       \
    }                                                                          \
    typename;

#define WCDBDefineOptionalBridgedType(bridgedTypeName, originalTypeName)       \
    typedef struct bridgedTypeName {                                           \
        bool hasValue;                                                         \
        originalTypeName value;                                                \
    } bridgedTypeName;

WCDBDefineOptionalBridgedType(OptionalBool, bool)
WCDBDefineOptionalBridgedType(OptionalUInt64, unsigned long long)

enum WCDBBridgedType {
    WCDBBridgedType_Invalid = 0,
    WCDBBridgedType_Null,
    WCDBBridgedType_Bool,
    WCDBBridgedType_Int,
    WCDBBridgedType_UInt,
    WCDBBridgedType_Double,
    WCDBBridgedType_String,

    WCDBBridgedType_Column,
    WCDBBridgedType_Schema,
    WCDBBridgedType_ColumnDef,
    WCDBBridgedType_ColumnConstraint,
    WCDBBridgedType_Expression,
    WCDBBridgedType_LiteralValue,
    WCDBBridgedType_ForeignKeyClause,
    WCDBBridgedType_BindParameter,
    WCDBBridgedType_RaiseFunction,
    WCDBBridgedType_WindowDef,
    WCDBBridgedType_Filter,
    WCDBBridgedType_IndexedColumn,
    WCDBBridgedType_TableConstraint,
    WCDBBridgedType_CommonTableExpression,
    WCDBBridgedType_QualifiedTableName,
    WCDBBridgedType_OrderingTerm,
    WCDBBridgedType_UpsertClause,
    WCDBBridgedType_Pragma,
    WCDBBridgedType_JoinClause,
    WCDBBridgedType_TableOrSubquery,
    WCDBBridgedType_JoinConstraint,
    WCDBBridgedType_SelectCore,
    WCDBBridgedType_ResultColumn,
    WCDBBridgedType_FrameSpec,

    WCDBBridgedType_AlterTableSTMT,
    WCDBBridgedType_AnalyzeSTMT,
    WCDBBridgedType_AttachSTMT,
    WCDBBridgedType_BeginSTMT,
    WCDBBridgedType_CommitSTMT,
    WCDBBridgedType_RollbackSTMT,
    WCDBBridgedType_SavepointSTMT,
    WCDBBridgedType_ReleaseSTMT,
    WCDBBridgedType_CreateIndexSTMT,
    WCDBBridgedType_CreateTableSTMT,
    WCDBBridgedType_CreateTriggerSTMT,
    WCDBBridgedType_SelectSTMT,
    WCDBBridgedType_InsertSTMT,
    WCDBBridgedType_DeleteSTMT,
    WCDBBridgedType_UpdateSTMT,
    WCDBBridgedType_CreateViewSTMT,
    WCDBBridgedType_CreateVirtualTableSTMT,
    WCDBBridgedType_DetachSTMT,
    WCDBBridgedType_DropIndexSTMT,
    WCDBBridgedType_DropTableSTMT,
    WCDBBridgedType_DropTriggerSTMT,
    WCDBBridgedType_DropViewSTMT,
    WCDBBridgedType_PragmaSTMT,
    WCDBBridgedType_ReindexSTMT,
    WCDBBridgedType_VacuumSTMT,
    WCDBBridgedType_ExplainSTMT,
};

typedef struct CPPData {
    unsigned char* _Nullable buffer;
    unsigned long long size;
} CPPData;

typedef struct CPPCommonValue {
    enum WCDBBridgedType type;
    union {
        long long intValue;
        double doubleValue;
    };
} CPPCommonValue;

typedef struct CPPCommonArray {
    enum WCDBBridgedType type;
    int length;
    const void* _Nonnull* _Nullable buffer;
} CPPCommonArray;

typedef struct CPPMultiTypeArray {
    int totalLength;
    const enum WCDBBridgedType* _Nullable types;
    const long long* _Nullable intValues;
    const double* _Nullable doubleValues;
    const char* _Nonnull* _Nullable stringValues;
} CPPMultiTypeArray;

#ifdef __ANDROID__

char* _Nullable* _Nullable WCDBPreAllocStringMemorySlot(int count);
void WCDBAllocStringMemory(char* _Nullable* _Nullable slot, int size);
void WCDBClearAllocatedMemory(int count);
void WCDBClearAllPreAllocatedMemory();

#endif

WCDB_EXTERN_C_END
