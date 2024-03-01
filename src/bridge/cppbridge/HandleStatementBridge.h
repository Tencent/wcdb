//
// Created by qiuwenchen on 2022/4/3.
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
#include "ErrorBridge.h"
#include "ObjectBridge.h"
#include "WinqBridge.h"

WCDB_EXTERN_C_BEGIN

WCDBDefineCPPBridgedType(CPPHandleStatement)

enum WCDBColumnValueType {
    WCDBColumnValueTypeInterger = 1,
    WCDBColumnValueTypeFloat,
    WCDBColumnValueTypeString,
    WCDBColumnValueTypeBLOB,
    WCDBColumnValueTypeNull,
};

CPPError WCDBHandleStatementGetError(CPPHandleStatement handleStatement);

bool WCDBHandleStatementPrepare(CPPHandleStatement handleStatement,
                                CPPObject* _Nonnull statement);
bool WCDBHandleStatementPrepareSQL(CPPHandleStatement handleStatement,
                                   const char* _Nonnull sql);
bool WCDBHandleStatementCheckPrepared(CPPHandleStatement handleStatement);
bool WCDBHandleStatementStep(CPPHandleStatement handleStatement);
void WCDBHandleStatementReset(CPPHandleStatement handleStatement);
void WCDBHandleStatementClearBindings(CPPHandleStatement handleStatement);
void WCDBHandleStatementFinalize(CPPHandleStatement handleStatement);
bool WCDBHandleStatementIsDone(CPPHandleStatement handleStatement);

void WCDBHandleStatementBindInteger(CPPHandleStatement handleStatement,
                                    int index,
                                    signed long long intValue);
void WCDBHandleStatementBindDouble(CPPHandleStatement handleStatement, int index, double doubleValue);
void WCDBHandleStatementBindText(CPPHandleStatement handleStatement,
                                 int index,
                                 const char* _Nonnull text);
void WCDBHandleStatementBindText16(CPPHandleStatement handleStatement,
                                   int index,
                                   const short* _Nullable text,
                                   int textLength);
void WCDBHandleStatementBindBlob(CPPHandleStatement handleStatement,
                                 int index,
                                 const unsigned char* _Nonnull buffer,
                                 unsigned long long buffLength);
void WCDBHandleStatementBindNull(CPPHandleStatement handleStatement, int index);

int WCDBHandleStatementBindParameterIndex(CPPHandleStatement handleStatement,
                                          const char* _Nullable parameterName);

enum WCDBColumnValueType
WCDBHandleStatementGetColumnType(CPPHandleStatement handleStatement, int index);
signed long long
WCDBHandleStatementGetInteger(CPPHandleStatement handleStatement, int index);
double WCDBHandleStatementGetDouble(CPPHandleStatement handleStatement, int index);
const char* _Nullable WCDBHandleStatementGetText(CPPHandleStatement handleStatement, int index);
const short* _Nullable WCDBHandleStatementGetText16(CPPHandleStatement handleStatement,
                                                    int index);
int WCDBHandleStatementGetText16Length(CPPHandleStatement handleStatement, int index);
const unsigned char* _Nullable WCDBHandleStatementGetBlob(CPPHandleStatement handleStatement,
                                                          int index);
signed long long
WCDBHandleStatementGetColumnSize(CPPHandleStatement handleStatement, int index);

int WCDBHandleStatementGetColumnCount(CPPHandleStatement handleStatement);
const char* _Nullable WCDBHandleStatementGetColumnName(CPPHandleStatement handleStatement,
                                                       int index);
const char* _Nullable WCDBHandleStatementGetOriginalColumnName(CPPHandleStatement handleStatement,
                                                               int index);
const char* _Nullable WCDBHandleStatementGetColumnTableName(CPPHandleStatement handleStatement,
                                                            int index);

bool WCDBHandleStatementIsReadOnly(CPPHandleStatement handleStatement);

WCDB_EXTERN_C_END
