// Created by qiuwenchen on 2023/6/9.
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

#define WCDBJNIUpsertFuncName(funcName) WCDBJNI(Upsert, funcName)
#define WCDBJNIUpsertObjectMethod(funcName, ...)                               \
    WCDBJNIObjectMethod(Upsert, funcName, __VA_ARGS__)
#define WCDBJNIUpsertClassMethodWithNoArg(funcName)                            \
    WCDBJNIClassMethodWithNoArg(Upsert, funcName)
#define WCDBJNIUpsertClassMethod(funcName, ...)                                \
    WCDBJNIClassMethod(Upsert, funcName, __VA_ARGS__)

jlong WCDBJNIUpsertClassMethodWithNoArg(createCppObj);
void WCDBJNIUpsertObjectMethod(configIndexedColumn,
                               jlong upsert,
                               WCDBJNIObjectOrStringArrayParameter(columns));
void WCDBJNIUpsertObjectMethod(configWhere, jlong upsert, jlong expression);
void WCDBJNIUpsertObjectMethod(configDoNothing, jlong upsert);
void WCDBJNIUpsertObjectMethod(configDoUpdate, jlong upsert);
void WCDBJNIUpsertObjectMethod(configSetColumns,
                               jlong upsert,
                               WCDBJNIObjectOrStringArrayParameter(columns));
void WCDBJNIUpsertObjectMethod(configToValue, jlong upsert, WCDBJNICommonValueParameter(expression));
