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

#include "WCDBRust.h"

#define WCDBRustJoinFuncName(funcName) WCDBRust(Join, funcName)
#define WCDBRustJoinObjectMethod(funcName, ...) WCDBRustObjectMethod(Join, funcName, __VA_ARGS__)
#define WCDBRustJoinClassMethodWithNoArg(funcName) WCDBRustClassMethodWithNoArg(Join, funcName)
#define WCDBRustJoinClassMethod(funcName, ...) WCDBRustClassMethod(Join, funcName, __VA_ARGS__)

void* WCDBRustJoinClassMethod(createCppObj, WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWith, void* join, WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWithJoin, void* join, WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWithLeftOuterJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWithLeftJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWithInnerJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWithCrossJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWithNaturalJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWithNaturalLeftOuterJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWithNaturalLeftJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWithNaturalInnerJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configWithNaturalCrossJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query));
void WCDBRustJoinClassMethod(configOn, void* join, void* expression);
void WCDBRustJoinClassMethod(configUsingColumn,
                             void* join,
                             WCDBRustObjectOrStringArrayParameter(columns));