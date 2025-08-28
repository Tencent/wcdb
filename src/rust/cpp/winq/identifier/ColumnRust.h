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

#define WCDBRustColumnFuncName(funcName) WCDBRust(Column, funcName)
#define WCDBRustColumnObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(Column, funcName, __VA_ARGS__)
#define WCDBRustColumnClassMethodWithNoArg(funcName) WCDBRustClassMethodWithNoArg(Column, funcName)
#define WCDBRustColumnClassMethod(funcName, ...) WCDBRustClassMethod(Column, funcName, __VA_ARGS__)

void* WCDBRustColumnClassMethodWithNoArg(createAll);
//
// jlong WCDBRustColumnClassMethodWithNoArg(createRowId);

void* WCDBRustColumnClassMethod(createWithName, const char* name, void* binding);

// jlong WCDBRustColumnClassMethod(copy, jlong column);
//
// void WCDBRustColumnClassMethod(inTable, jlong column, jstring table);
//
// void WCDBRustColumnClassMethod(ofSchema, jlong column, WCDBRustObjectOrStringParameter(schema));
//
// jlong WCDBRustColumnClassMethod(configAlias, jlong column, jstring alias);
