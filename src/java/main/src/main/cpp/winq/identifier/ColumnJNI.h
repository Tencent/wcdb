// Created by qiuwenchen on 2023/3/30.
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

#define WCDBJNIColumnFuncName(funcName) WCDBJNI(Column, funcName)
#define WCDBJNIColumnObjectMethod(funcName, ...)                               \
    WCDBJNIObjectMethod(Column, funcName, __VA_ARGS__)
#define WCDBJNIColumnClassMethodWithNoArg(funcName)                            \
    WCDBJNIClassMethodWithNoArg(Column, funcName)
#define WCDBJNIColumnClassMethod(funcName, ...)                                \
    WCDBJNIClassMethod(Column, funcName, __VA_ARGS__)

jlong WCDBJNIColumnClassMethodWithNoArg(createAll);

jlong WCDBJNIColumnClassMethodWithNoArg(createRowId);

jlong WCDBJNIColumnClassMethod(createWithName, jstring name, jlong binding);

jlong WCDBJNIColumnClassMethod(copy, jlong column);

void WCDBJNIColumnClassMethod(inTable, jlong column, jstring table);

void WCDBJNIColumnClassMethod(ofSchema, jlong column, WCDBJNIObjectOrStringParameter(schema));

jlong WCDBJNIColumnClassMethod(configAlias, jlong column, jstring alias);
