// Created by chenqiuwen on 2023/4/6.
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

#define WCDBJNIOrderingTermFuncName(funcName) WCDBJNI(OrderingTerm, funcName)
#define WCDBJNIOrderingTermObjectMethod(funcName, ...)                         \
    WCDBJNIObjectMethod(OrderingTerm, funcName, __VA_ARGS__)
#define WCDBJNIOrderingTermClassMethodWithNoArg(funcName)                      \
    WCDBJNIClassMethodWithNoArg(OrderingTerm, funcName)
#define WCDBJNIOrderingTermClassMethod(funcName, ...)                          \
    WCDBJNIClassMethod(OrderingTerm, funcName, __VA_ARGS__)

jlong WCDBJNIOrderingTermObjectMethod(create, jint type, jlong expression);

void WCDBJNIOrderingTermObjectMethod(configCollation, jlong object, jstring collation);

void WCDBJNIOrderingTermObjectMethod(configOrder, jlong object, jint order);
