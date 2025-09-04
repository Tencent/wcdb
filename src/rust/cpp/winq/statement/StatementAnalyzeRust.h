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

#include "WCDBRust.h"

#pragma once
#define WCDBRustStatementAnalyzeFuncName(funcName) WCDBRust(StatementAnalyze, funcName)
#define WCDBRustStatementAnalyzeObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementAnalyze, funcName, __VA_ARGS__)
#define WCDBRustStatementAnalyzeClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementAnalyze, funcName)
#define WCDBRustStatementAnalyzeClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementAnalyze, funcName, __VA_ARGS__)

void* WCDBRustStatementAnalyzeClassMethodWithNoArg(createCppObj);
void WCDBRustStatementAnalyzeClassMethod(toAnalyze, void* analyze);
void WCDBRustStatementAnalyzeClassMethod(configSchema,
                                         void* analyze,
                                         WCDBRustObjectOrStringParameter(schema));
void WCDBRustStatementAnalyzeClassMethod(configTable, void* analyze, const char* table);
void WCDBRustStatementAnalyzeClassMethod(configIndex, void* analyze, const char* index);
