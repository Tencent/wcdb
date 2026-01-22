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

#define WCDBJNIIndexedColumnFuncName(funcName) WCDBRust(IndexedColumn, funcName)
#define WCDBRustIndexedColumnObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(IndexedColumn, funcName, __VA_ARGS__)
#define WCDBRustIndexedColumnClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(IndexedColumn, funcName)
#define WCDBRustIndexedColumnClassMethod(funcName, ...) \
    WCDBRustClassMethod(IndexedColumn, funcName, __VA_ARGS__)

void* WCDBRustIndexedColumnClassMethod(create, WCDBRustObjectOrStringParameter(column));

void WCDBRustIndexedColumnClassMethod(configCollation, void* indexedColumn, const char* collation);

void WCDBRustIndexedColumnClassMethod(configOrder, void* indexedColumn, int order);
