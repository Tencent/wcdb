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

#define WCDBRustTableConstraintFuncName(funcName) WCDBRust(TableConstraint, funcName)
#define WCDBRustTableConstraintObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(TableConstraint, funcName, __VA_ARGS__)
#define WCDBRustTableConstraintClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(TableConstraint, funcName)
#define WCDBRustTableConstraintClassMethod(funcName, ...) \
    WCDBRustClassMethod(TableConstraint, funcName, __VA_ARGS__)

void* WCDBRustTableConstraintClassMethod(create, const char* name);
void WCDBRustTableConstraintClassMethod(configPrimaryKey, void* constraint);
void WCDBRustTableConstraintClassMethod(configUnique, void* constraint);
void WCDBRustTableConstraintClassMethod(configIndexedColumn,
                                        void* constraint,
                                        WCDBRustObjectOrStringArrayParameter(indexedColumns));
// void WCDBRustTableConstraintClassMethod(configConfliction, jlong constraint, int conflictAction);
// void WCDBRustTableConstraintClassMethod(configCheckCondition, jlong constraint, jlong
// expression); void WCDBRustTableConstraintClassMethod(configForeignKey,
//                                        jlong constraint,
//                                        WCDBRustObjectOrStringArrayParameter(columns),
//                                        jlong foreignKey);
