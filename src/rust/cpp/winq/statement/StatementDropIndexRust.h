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

#define WCDBRustStatementDropIndexFuncName(funcName) WCDBRust(StatementDropIndex, funcName)
#define WCDBRustStatementDropIndexObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementDropIndex, funcName, __VA_ARGS__)
#define WCDBRustStatementDropIndexObjectMethodWithNoArg(funcName) \
    WCDBRustObjectMethodWithNoArg(StatementDropIndex, funcName)
#define WCDBRustStatementDropIndexClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementDropIndex, funcName)
#define WCDBRustStatementDropIndexClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementDropIndex, funcName, __VA_ARGS__)

void* WCDBRustStatementDropIndexClassMethodWithNoArg(createCppObj);
void WCDBRustStatementDropIndexClassMethod(configSchema,
                                           void* self,
                                           WCDBRustObjectOrStringParameter(schema));
void WCDBRustStatementDropIndexClassMethod(configIndex, void* self, const char* indexName);
void WCDBRustStatementDropIndexClassMethod(configIfExist, void* self);
