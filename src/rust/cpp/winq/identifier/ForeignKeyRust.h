// Created by qiuwenchen on 2023/6/8.
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

#define WCDBRustForeignKeyFuncName(funcName) WCDBRust(ForeignKey, funcName)
#define WCDBRustForeignKeyObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(ForeignKey, funcName, __VA_ARGS__)
#define WCDBRustForeignKeyClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(ForeignKey, funcName)
#define WCDBRustForeignKeyClassMethod(funcName, ...) \
    WCDBRustClassMethod(ForeignKey, funcName, __VA_ARGS__)

void* WCDBRustForeignKeyClassMethodWithNoArg(createCppObject);

void WCDBRustForeignKeyClassMethod(configReferencesTable, void* self, const char* table);
void WCDBRustForeignKeyClassMethod(configColumns,
                                   void* self,
                                   WCDBRustObjectOrStringArrayParameter(column));

void WCDBRustForeignKeyClassMethod(configOnDeleteAction, void* self, int action);

void WCDBRustForeignKeyClassMethod(configOnUpdateAction, void* self, int action);

void WCDBRustForeignKeyClassMethod(configMatch, void* self, int match);

void WCDBRustForeignKeyClassMethod(configDeferrable, void* self, int type);
void WCDBRustForeignKeyClassMethod(configNotDeferrable, void* self, int type);
