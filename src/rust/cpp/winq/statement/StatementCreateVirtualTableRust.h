// Created by chenqiuwen on 2023/11/4.
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

#define WCDBRustStatementCreateVirtualTableFuncName(funcName) \
    WCDBRust(StatementCreateVirtualTable, funcName)
#define WCDBRustStatementCreateVirtualTableObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementCreateVirtualTable, funcName, __VA_ARGS__)
#define WCDBRustStatementCreateVirtualTableObjectMethodWithNoArg(funcName) \
    WCDBRustObjectMethodWithNoArg(StatementCreateVirtualTable, funcName)
#define WCDBRustStatementCreateVirtualTableClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementCreateVirtualTable, funcName)
#define WCDBRustStatementCreateVirtualTableClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementCreateVirtualTable, funcName, __VA_ARGS__)

void* WCDBRustStatementCreateVirtualTableClassMethodWithNoArg(createCppObj);

void WCDBRustStatementCreateVirtualTableClassMethod(configTable, void* self, const char* name);
void WCDBRustStatementCreateVirtualTableClassMethod(configSchema,
                                                    void* self,
                                                    WCDBRustObjectOrStringParameter(schema));
void WCDBRustStatementCreateVirtualTableClassMethod(configIfNotExist, void* self);
void WCDBRustStatementCreateVirtualTableClassMethod(configModule, void* self, const char* module);
void WCDBRustStatementCreateVirtualTableClassMethod(configArguments,
                                                    void* self,
                                                    const char** arguments,
                                                    int argumentsLength);
