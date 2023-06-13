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

#define WCDBJNIJoinFuncName(funcName) WCDBJNI(Join, funcName)
#define WCDBJNIJoinObjectMethod(funcName, ...)                                 \
    WCDBJNIObjectMethod(Join, funcName, __VA_ARGS__)
#define WCDBJNIJoinClassMethodWithNoArg(funcName)                              \
    WCDBJNIClassMethodWithNoArg(Join, funcName)
#define WCDBJNIJoinClassMethod(funcName, ...)                                  \
    WCDBJNIClassMethod(Join, funcName, __VA_ARGS__)

jlong WCDBJNIJoinClassMethod(createCppObj, WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWith, jlong join, WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWithJoin, jlong join, WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWithLeftOuterJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWithLeftJoin, jlong join, WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWithInnerJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWithCrossJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWithNaturalJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWithNaturalLeftOuterJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWithNaturalLeftJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWithNaturalInnerJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configWithNaturalCrossJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query));
void WCDBJNIJoinObjectMethod(configOn, jlong join, jlong expression);
void WCDBJNIJoinObjectMethod(configUsingColumn,
                             jlong join,
                             WCDBJNIObjectOrStringArrayParameter(columns));