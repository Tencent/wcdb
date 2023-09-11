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
#define WCDBJNIStatementUpdateClassMethod(funcName, ...)                       \
    WCDBJNIClassMethod(StatementUpdate, funcName, __VA_ARGS__)

jlong WCDBJNIStatementUpdateClassMethodWithNoArg(create);

void WCDBJNIStatementUpdateClassMethod(configWith, jlong self, jlongArray expressions);
void WCDBJNIStatementUpdateClassMethod(configRecursive, jlong self);

void WCDBJNIStatementUpdateClassMethod(configTable,
                                       jlong self,
                                       WCDBJNIObjectOrStringParameter(table));
void WCDBJNIStatementUpdateClassMethod(configConfliction, jlong self, jint action);
void WCDBJNIStatementUpdateClassMethod(configColumns,
                                       jlong self,
                                       WCDBJNIObjectOrStringArrayParameter(columns));
void WCDBJNIStatementUpdateClassMethod(configValue, jlong self, WCDBJNICommonValueParameter(value));
void WCDBJNIStatementUpdateClassMethod(configColumnsToValues,
                                       jlong self,
                                       WCDBJNIObjectOrStringArrayParameter(columns),
                                       WCDBJNIMultiTypeArrayParameter(values));
void WCDBJNIStatementUpdateClassMethod(configColumnsWithBindParameter,
                                       jlong self,
                                       WCDBJNIObjectOrStringArrayParameter(columns));
void WCDBJNIStatementUpdateClassMethod(configCondition, jlong self, jlong condition);
void WCDBJNIStatementUpdateClassMethod(configOrders, jlong self, jlongArray orders);
void WCDBJNIStatementUpdateClassMethod(
configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to);
void WCDBJNIStatementUpdateClassMethod(configLimitCount, jlong self, jint type, jlong limit);
void WCDBJNIStatementUpdateClassMethod(configOffset, jlong self, jint type, jlong offset);
