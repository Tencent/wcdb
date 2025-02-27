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

#define WCDBRustStatementCreateIndexFuncName(funcName) WCDBRust(StatementCreateIndex, funcName)
#define WCDBRustStatementCreateIndexObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementCreateIndex, funcName, __VA_ARGS__)
#define WCDBRustStatementCreateIndexObjectMethodWithNoArg(funcName) \
    WCDBRustObjectMethodWithNoArg(StatementCreateIndex, funcName)
#define WCDBRustStatementCreateIndexClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementCreateIndex, funcName)
#define WCDBRustStatementCreateIndexClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementCreateIndex, funcName, __VA_ARGS__)

void* WCDBRustStatementCreateIndexClassMethodWithNoArg(create);
// void WCDBRustStatementCreateIndexClassMethod(configIndex, jlong self, jstring name);
// void WCDBRustStatementCreateIndexClassMethod(configSchema,
//                                             jlong self,
//                                             WCDBRustObjectOrStringParameter(schema));
// void WCDBRustStatementCreateIndexClassMethod(configUnique, jlong self);
void WCDBRustStatementCreateIndexClassMethod(configIfNotExist, void* self);
// void WCDBRustStatementCreateIndexClassMethod(configTable, jlong self, jstring tableName);
void WCDBRustStatementCreateIndexClassMethod(configIndexedColumns,
                                             void* self,
                                             WCDBRustObjectOrStringArrayParameter(indexColumns));
// void WCDBRustStatementCreateIndexClassMethod(configWhere, jlong self, jlong condition);
