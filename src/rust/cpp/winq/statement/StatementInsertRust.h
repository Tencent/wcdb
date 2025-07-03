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

#include "WCDBRust.h"

#define WCDBRustStatementInsertFuncName(funcName) WCDBRust(StatementInsert, funcName)
#define WCDBRustStatementInsertObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementInsert, funcName, __VA_ARGS__)
#define WCDBRustStatementInsertObjectMethodWithNoArg(funcName) \
    WCDBRustObjectMethodWithNoArg(StatementInsert, funcName)
#define WCDBRustStatementInsertClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementInsert, funcName)
#define WCDBRustStatementInsertClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementInsert, funcName, __VA_ARGS__)

void* WCDBRustStatementInsertClassMethodWithNoArg(create);

// void WCDBRustStatementInsertClassMethod(configWith, jlong self, jlongArray expressions);
// void WCDBRustStatementInsertClassMethod(configRecursive, jlong self);
void WCDBRustStatementInsertClassMethod(configTableName, void* self, const char* tableName);

// void WCDBRustStatementInsertClassMethod(configSchema,
//                                        jlong self,
//                                        WCDBRustObjectOrStringParameter(schema));
void WCDBRustStatementInsertClassMethod(configConflictAction, void* self, int action);

// void WCDBRustStatementInsertClassMethod(configAs, jlong self, jstring alias);
void WCDBRustStatementInsertClassMethod(configColumns,
                                        void* self,
                                        WCDBRustObjectOrStringArrayParameter(columns));

void WCDBRustStatementInsertClassMethod(configValues,
                                        void* self,
                                        WCDBRustMultiTypeArrayParameter(value));
void WCDBRustStatementInsertClassMethod(configValuesWithBindParameters, void* self, int count);
// void WCDBRustStatementInsertClassMethod(configSelect, jlong self, jlong select);
void WCDBRustStatementInsertClassMethod(configDefaultValues, void* self);
void WCDBRustStatementInsertClassMethod(configUpsert, void* self, void* upsert);
