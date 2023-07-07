//
// Created by qiuwenchen on 2022/12/2.
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

#include "HandleBridge.h"
#include "ObjectBridge.h"
#include "WinqBridge.h"

WCDB_EXTERN_C_BEGIN

WCDBDefineCPPBridgedType(CPPBinding)

CPPBinding WCDBBindingCreate();
void WCDBBindingAddColumnDef(CPPBinding binding, CPPColumnDef columnDef);
void WCDBBindingEnableAutoIncrementForExistingTable(CPPBinding binding);
void WCDBBindingAddColumnConstraint(CPPBinding binding,
                                    const char* _Nullable columnName,
                                    CPPColumnConstraint constraint);
void WCDBBindingAddIndex(CPPBinding binding,
                         const char* _Nullable indexNameOrSuffix,
                         bool isFullName,
                         CPPStatementCreateIndex createIndex);
void WCDBBindingAddTableConstraint(CPPBinding binding, CPPTableConstraint tableConstraint);
void WCDBBindingConfigVirtualModule(CPPBinding binding, const char* _Nullable moduleName);
void WCDBBindingConfigVirtualModuleArgument(CPPBinding binding, const char* _Nullable argument);
bool WCDBBindingIsPrimaryKey(CPPBinding binding, const char* _Nullable columnName);

bool WCDBBindingCreateTable(CPPBinding binding, const char* _Nullable tableName, CPPHandle handle);
bool WCDBBindingCreateVirtualTable(CPPBinding binding,
                                   const char* _Nullable tableName,
                                   CPPHandle handle);

void WCDBBindingConfigWithoutRowId(CPPBinding binding);

const void* _Nullable WCDBBindingGetBaseBinding(CPPBinding binding);

WCDB_EXTERN_C_END
