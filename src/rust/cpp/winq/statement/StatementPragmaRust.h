// Created by chenqiuwen on 2023/4/12.
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

#define WCDBRustStatementPragmaFuncName(funcName) WCDBRust(StatementPragma, funcName)
#define WCDBRustStatementPragmaObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementPragma, funcName, __VA_ARGS__)
#define WCDBRustStatementPragmaObjectMethodWithNoArg(funcName) \
    WCDBRustObjectMethodWithNoArg(StatementPragma, funcName)
#define WCDBRustStatementPragmaClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementPragma, funcName)
#define WCDBRustStatementPragmaClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementPragma, funcName, __VA_ARGS__)

void* WCDBRustStatementPragmaClassMethodWithNoArg(create);
// void WCDBJNIStatementPragmaClassMethod(configSchema,
//                                        jlong self,
//                                        WCDBJNIObjectOrStringParameter(schema));
void WCDBRustStatementPragmaClassMethod(configPragma, void* self, void* pragma);
void WCDBRustStatementPragmaClassMethod(configToValue,
                                        void* self,
                                        WCDBRustCommonValueParameter(value));

// void WCDBJNIStatementPragmaClassMethod(configWithValue,
//                                        jlong self,
//                                        WCDBJNICommonValueParameter(value));
