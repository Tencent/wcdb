// Created by chenqiuwen on 2023/6/11.
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

#define WCDBRustStatementCreateViewFuncName(funcName) WCDBRust(StatementCreateView, funcName)
#define WCDBRustStatementCreateViewObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementCreateView, funcName, __VA_ARGS__)
#define WCDBRustStatementCreateViewObjectMethodWithNoArg(funcName) \
    WCDBRustObjectMethodWithNoArg(StatementCreateView, funcName)
#define WCDBRustStatementCreateViewClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementCreateView, funcName)
#define WCDBRustStatementCreateViewClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementCreateView, funcName, __VA_ARGS__)

void* WCDBRustStatementCreateViewClassMethodWithNoArg(createCppObj);

void WCDBRustStatementCreateViewClassMethod(configView, void* self, const char* name);
void WCDBRustStatementCreateViewClassMethod(configSchema,
                                            void* self,
                                            WCDBRustObjectOrStringParameter(schema));
void WCDBRustStatementCreateViewClassMethod(configTemp, void* self);
void WCDBRustStatementCreateViewClassMethod(configIfNotExist, void* self);
void WCDBRustStatementCreateViewClassMethod(configAs, void* self, void* select);
void WCDBRustStatementCreateViewClassMethod(configColumns,
                                            void* self,
                                            WCDBRustObjectOrStringArrayParameter(columns));
