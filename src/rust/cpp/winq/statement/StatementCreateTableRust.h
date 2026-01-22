// Created by qiuwenchen on 2023/4/11.
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

#include "WCDBRust.h"

#define WCDBRustStatementCreateTableFuncName(funcName) WCDB(StatementCreateTable, funcName)
#define WCDBRustStatementCreateTableObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementCreateTable, funcName, __VA_ARGS__)
#define WCDBRustStatementCreateTableObjectMethodWithNoArg(funcName) \
    WCDBRustObjectMethodWithNoArg(StatementCreateTable, funcName)
#define WCDBRustStatementCreateTableClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementCreateTable, funcName)
#define WCDBRustStatementCreateTableClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementCreateTable, funcName, __VA_ARGS__)

void* WCDBRustStatementCreateTableClassMethodWithNoArg(create);
void WCDBRustStatementCreateTableClassMethod(configTableName, void* self, const char* tableName);
// void WCDBRustStatementCreateTableClassMethod(configSchema,
//                                             jlong self,
//                                             WCDBRustObjectOrStringParameter(schema));
// void WCDBRustStatementCreateTableClassMethod(configTemp, jlong self);
// void WCDBRustStatementCreateTableClassMethod(configIfNotExist, jlong self);
// void WCDBRustStatementCreateTableClassMethod(configAs, jlong self, jlong select);
// void WCDBRustStatementCreateTableClassMethod(configColumn, jlong self, jlong column);
void WCDBRustStatementCreateTableClassMethod(configColumns,
                                             void* self,
                                             const void** columns,
                                             int len);
// void WCDBRustStatementCreateTableClassMethod(configConstraints, jlong self, jlongArray
// constraints); void WCDBRustStatementCreateTableClassMethod(configWithoutRowid, jlong self);