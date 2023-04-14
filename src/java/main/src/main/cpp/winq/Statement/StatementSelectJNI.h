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

jlong WCDBJNIStatementSelectObjectMethodWithNoArg(create);
void WCDBJNIStatementSelectObjectMethod(configResultColumns,
                                        jlong self,
                                        WCDBJNIMultiTypeArrayParameter(resultColumns));
void WCDBJNIStatementSelectObjectMethod(configDistiction, jlong self);
void WCDBJNIStatementSelectObjectMethod(configTableOrSubqueries,
                                        jlong self,
                                        WCDBJNIMultiTypeArrayParameter(tableOrSubqueries));
void WCDBJNIStatementSelectObjectMethod(configCondition, jlong self, jlong condition);
void WCDBJNIStatementSelectObjectMethod(configGroups,
                                        jlong self,
                                        WCDBJNIMultiTypeArrayParameter(groups));
void WCDBJNIStatementSelectObjectMethod(configHaving, jlong self, jlong expression);
void WCDBJNIStatementSelectObjectMethod(configUnion, jlong self);
void WCDBJNIStatementSelectObjectMethod(configUnionAll, jlong self);
void WCDBJNIStatementSelectObjectMethod(configIntersect, jlong self);
void WCDBJNIStatementSelectObjectMethod(configExcept, jlong self);
void WCDBJNIStatementSelectObjectMethod(configOrders, jlong self, jlongArray orders);
void WCDBJNIStatementSelectObjectMethod(
configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to);
void WCDBJNIStatementSelectObjectMethod(configLimitCount, jlong self, jint type, jlong limit);
void WCDBJNIStatementSelectObjectMethod(configOffset, jlong self, jint type, jlong offset);
