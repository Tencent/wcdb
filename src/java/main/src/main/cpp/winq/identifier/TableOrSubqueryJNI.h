// Created by qiuwenchen on 2023/4/7.
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

#define WCDBJNITableOrSubqueryFuncName(funcName)                               \
    WCDBJNI(TableOrSubquery, funcName)
#define WCDBJNITableOrSubqueryObjectMethod(funcName, ...)                      \
    WCDBJNIObjectMethod(TableOrSubquery, funcName, __VA_ARGS__)
#define WCDBJNITableOrSubqueryClassMethodWithNoArg(funcName)                   \
    WCDBJNIClassMethodWithNoArg(TableOrSubquery, funcName)
#define WCDBJNITableOrSubqueryClassMethod(funcName, ...)                       \
    WCDBJNIClassMethod(TableOrSubquery, funcName, __VA_ARGS__)

jlong WCDBJNITableOrSubqueryObjectMethod(create, WCDBJNIObjectOrStringParameter(tableOrSubquery));

jlong WCDBJNITableOrSubqueryObjectMethod(createWithTableOrSubqueries,
                                         WCDBJNIObjectOrStringArrayParameter(subquaries));

jlong WCDBJNITableOrSubqueryClassMethod(createWithFunction, jstring funcName);

void WCDBJNITableOrSubqueryObjectMethod(configSchema,
                                        jlong tableOrSubquery,
                                        WCDBJNIObjectOrStringParameter(schema));

void WCDBJNITableOrSubqueryObjectMethod(configAlias, jlong tableOrSubquery, jstring alias);

void WCDBJNITableOrSubqueryObjectMethod(configNotIndexed, jlong tableOrSubquery);

void WCDBJNITableOrSubqueryObjectMethod(configIndexName, jlong tableOrSubquery, jstring index);

void WCDBJNITableOrSubqueryObjectMethod(argument,
                                        jlong tableOrSubquery,
                                        WCDBJNICommonValueParameter(argument));
