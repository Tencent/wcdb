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

#include "WCDBRust.h"

#pragma once
#define WCDBRustStatementAlterTableFuncName(funcName) WCDBRust(StatementAlterTable, funcName)
#define WCDBRustStatementAlterTableObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementAlterTable, funcName, __VA_ARGS__)
#define WCDBRustStatementAlterTableClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementAlterTable, funcName)
#define WCDBRustStatementAlterTableClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementAlterTable, funcName, __VA_ARGS__)

void* WCDBRustStatementAlterTableClassMethodWithNoArg(createCppObj);
void WCDBRustStatementAlterTableClassMethod(configTable, void* self, const char* table);
void WCDBRustStatementAlterTableClassMethod(configSchema,
                                            void* self,
                                            WCDBRustObjectOrStringParameter(schema));
void WCDBRustStatementAlterTableClassMethod(configRenameToTable, void* self, const char* table);
void WCDBRustStatementAlterTableClassMethod(configRenameColumn,
                                            void* self,
                                            WCDBRustObjectOrStringParameter(column));
void WCDBRustStatementAlterTableClassMethod(configRenameToColumn,
                                            void* self,
                                            WCDBRustObjectOrStringParameter(column));
void WCDBRustStatementAlterTableClassMethod(configAddColumn, void* self, void* columnDef);
