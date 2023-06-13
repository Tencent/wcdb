// Created by chenqiuwen on 2023/4/13.
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

#define WCDBJNIStatementUpdateFuncName(funcName)                               \
    WCDBJNI(StatementUpdate, funcName)
#define WCDBJNIStatementUpdateObjectMethod(funcName, ...)                      \
    WCDBJNIObjectMethod(StatementUpdate, funcName, __VA_ARGS__)
#define WCDBJNIStatementUpdateObjectMethodWithNoArg(funcName)                  \
    WCDBJNIObjectMethodWithNoArg(StatementUpdate, funcName)
#define WCDBJNIStatementUpdateClassMethodWithNoArg(funcName)                   \
    WCDBJNIClassMethodWithNoArg(StatementUpdate, funcName)

jlong WCDBJNIStatementUpdateObjectMethodWithNoArg(create);

void WCDBJNIStatementUpdateObjectMethod(configWith, jlong self, jlongArray expressions);
void WCDBJNIStatementUpdateObjectMethod(configRecursive, jlong self);

void WCDBJNIStatementUpdateObjectMethod(configTable,
                                        jlong self,
                                        WCDBJNIObjectOrStringParameter(table));
void WCDBJNIStatementUpdateObjectMethod(configConfliction, jlong self, jint action);
void WCDBJNIStatementUpdateObjectMethod(configColumns,
                                        jlong self,
                                        WCDBJNIObjectOrStringArrayParameter(columns));
void WCDBJNIStatementUpdateObjectMethod(configValue,
                                        jlong self,
                                        WCDBJNICommonValueParameter(value));
void WCDBJNIStatementUpdateObjectMethod(configColumnsToValues,
                                        jlong self,
                                        WCDBJNIObjectOrStringArrayParameter(columns),
                                        WCDBJNIMultiTypeArrayParameter(values));
void WCDBJNIStatementUpdateObjectMethod(configColumnsWithBindParameter,
                                        jlong self,
                                        WCDBJNIObjectOrStringArrayParameter(columns));
void WCDBJNIStatementUpdateObjectMethod(configCondition, jlong self, jlong condition);
void WCDBJNIStatementUpdateObjectMethod(configOrders, jlong self, jlongArray orders);
void WCDBJNIStatementUpdateObjectMethod(
configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to);
void WCDBJNIStatementUpdateObjectMethod(configLimitCount, jlong self, jint type, jlong limit);
void WCDBJNIStatementUpdateObjectMethod(configOffset, jlong self, jint type, jlong offset);
