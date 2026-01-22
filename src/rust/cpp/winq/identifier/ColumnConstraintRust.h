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

#define WCDBRustColumnConstraintFuncName(funcName) WCDBRust(ColumnConstraint, funcName)
#define WCDBRustColumnConstraintObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(ColumnConstraint, funcName, __VA_ARGS__)
#define WCDBRustColumnConstraintClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(ColumnConstraint, funcName)
#define WCDBRustColumnConstraintClassMethod(funcName, ...) \
    WCDBRustClassMethod(ColumnConstraint, funcName, __VA_ARGS__)

void* WCDBRustColumnConstraintClassMethod(create, const char* name);

void WCDBRustColumnConstraintClassMethod(configPrimaryKey, void* constraint);

// void WCDBRustColumnConstraintClassMethod(configOrder, jlong constraint, jint order);

void WCDBRustColumnConstraintClassMethod(configConflictAction,
                                         void* constraint,
                                         int conflictAction);

void WCDBRustColumnConstraintClassMethod(configAutoIncrement, void* constraint);

void WCDBRustColumnConstraintClassMethod(configNotNull, void* constraint);

void WCDBRustColumnConstraintClassMethod(configUnique, void* constraint);
//
// void WCDBRustColumnConstraintClassMethod(configCheck, jlong constraint, jlong expression);
//
void WCDBRustColumnConstraintClassMethod(configDefaultValue,
                                         void* constraint,
                                         WCDBRustCommonValueParameter(value));

void WCDBRustColumnConstraintClassMethod(configCollation, void* constraint, const char* collation);
//
// void WCDBRustColumnConstraintClassMethod(configForeignKey, jlong constraint, jlong foreignKey);
//
void WCDBRustColumnConstraintClassMethod(configUnindexed, void* constraint);
