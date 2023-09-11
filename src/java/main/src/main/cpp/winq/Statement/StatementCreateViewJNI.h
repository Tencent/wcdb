// Created by chenqiuwen on 2023/6/11.
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

#define WCDBJNIStatementCreateViewFuncName(funcName)                           \
    WCDBJNI(StatementCreateView, funcName)
#define WCDBJNIStatementCreateViewObjectMethod(funcName, ...)                  \
    WCDBJNIObjectMethod(StatementCreateView, funcName, __VA_ARGS__)
#define WCDBJNIStatementCreateViewObjectMethodWithNoArg(funcName)              \
    WCDBJNIObjectMethodWithNoArg(StatementCreateView, funcName)
#define WCDBJNIStatementCreateViewClassMethodWithNoArg(funcName)               \
    WCDBJNIClassMethodWithNoArg(StatementCreateView, funcName)
#define WCDBJNIStatementCreateViewClassMethod(funcName, ...)                   \
    WCDBJNIClassMethod(StatementCreateView, funcName, __VA_ARGS__)

jlong WCDBJNIStatementCreateViewClassMethodWithNoArg(createCppObj);

void WCDBJNIStatementCreateViewClassMethod(configView, jlong self, jstring name);
void WCDBJNIStatementCreateViewClassMethod(configSchema,
                                           jlong self,
                                           WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIStatementCreateViewClassMethod(configTemp, jlong self);
void WCDBJNIStatementCreateViewClassMethod(configIfNotExist, jlong self);
void WCDBJNIStatementCreateViewClassMethod(configAs, jlong self, jlong select);
void WCDBJNIStatementCreateViewClassMethod(configColumns,
                                           jlong self,
                                           WCDBJNIObjectOrStringArrayParameter(columns));
