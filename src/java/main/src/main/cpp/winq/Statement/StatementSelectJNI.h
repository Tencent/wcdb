// Created by qiuwenchen on 2023/4/13.
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

#include "WCDBJNI.h"

#define WCDBJNIStatementSelectFuncName(funcName)                               \
    WCDBJNI(StatementSelect, funcName)
#define WCDBJNIStatementSelectObjectMethod(funcName, ...)                      \
    WCDBJNIObjectMethod(StatementSelect, funcName, __VA_ARGS__)
#define WCDBJNIStatementSelectObjectMethodWithNoArg(funcName)                  \
    WCDBJNIObjectMethodWithNoArg(StatementSelect, funcName)
#define WCDBJNIStatementSelectClassMethodWithNoArg(funcName)                   \
    WCDBJNIClassMethodWithNoArg(StatementSelect, funcName)
#define WCDBJNIStatementSelectClassMethod(funcName, ...)                       \
    WCDBJNIClassMethod(StatementSelect, funcName, __VA_ARGS__)

jlong WCDBJNIStatementSelectClassMethodWithNoArg(create);

void WCDBJNIStatementSelectClassMethod(configWith, jlong self, jlongArray expressions);
void WCDBJNIStatementSelectClassMethod(configRecursive, jlong self);

void WCDBJNIStatementSelectClassMethod(configResultColumns,
                                       jlong self,
                                       WCDBJNIMultiTypeArrayParameter(resultColumns));
void WCDBJNIStatementSelectClassMethod(configDistiction, jlong self);
void WCDBJNIStatementSelectClassMethod(configTableOrSubqueries,
                                       jlong self,
                                       WCDBJNIMultiTypeArrayParameter(tableOrSubqueries));
void WCDBJNIStatementSelectClassMethod(configCondition, jlong self, jlong condition);
void WCDBJNIStatementSelectClassMethod(configGroups,
                                       jlong self,
                                       WCDBJNIMultiTypeArrayParameter(groups));
void WCDBJNIStatementSelectClassMethod(configHaving, jlong self, jlong expression);
void WCDBJNIStatementSelectClassMethod(configUnion, jlong self);
void WCDBJNIStatementSelectClassMethod(configUnionAll, jlong self);
void WCDBJNIStatementSelectClassMethod(configIntersect, jlong self);
void WCDBJNIStatementSelectClassMethod(configExcept, jlong self);
void WCDBJNIStatementSelectClassMethod(configOrders, jlong self, jlongArray orders);
void WCDBJNIStatementSelectClassMethod(
configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to);
void WCDBJNIStatementSelectClassMethod(configLimitCount, jlong self, jint type, jlong limit);
void WCDBJNIStatementSelectClassMethod(configOffset, jlong self, jint type, jlong offset);
