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

#define WCDBJNIFrameSpecFuncName(funcName) WCDBJNI(FrameSpec, funcName)
#define WCDBJNIFrameSpecObjectMethod(funcName, ...)                            \
    WCDBJNIObjectMethod(FrameSpec, funcName, __VA_ARGS__)
#define WCDBJNIFrameSpecClassMethodWithNoArg(funcName)                         \
    WCDBJNIClassMethodWithNoArg(FrameSpec, funcName)
#define WCDBJNIFrameSpecClassMethod(funcName, ...)                             \
    WCDBJNIClassMethod(FrameSpec, funcName, __VA_ARGS__)

jlong WCDBJNIFrameSpecClassMethodWithNoArg(createCppObj);
void WCDBJNIFrameSpecObjectMethod(configRange, jlong self);
void WCDBJNIFrameSpecObjectMethod(configRows, jlong self);
void WCDBJNIFrameSpecObjectMethod(configUnboundedPreceding, jlong self);
void WCDBJNIFrameSpecObjectMethod(configPreceding,
                                  jlong self,
                                  WCDBJNIObjectOrIntegerParameter(expression));
void WCDBJNIFrameSpecObjectMethod(configCurrentRow, jlong self);
void WCDBJNIFrameSpecObjectMethod(configBetweenUnboundedPreceding, jlong self);
void WCDBJNIFrameSpecObjectMethod(configBetweenPreceding,
                                  jlong self,
                                  WCDBJNIObjectOrIntegerParameter(expression));
void WCDBJNIFrameSpecObjectMethod(configBetweenCurrentRow, jlong self);
void WCDBJNIFrameSpecObjectMethod(configBetweenFollowing,
                                  jlong self,
                                  WCDBJNIObjectOrIntegerParameter(expression));
void WCDBJNIFrameSpecObjectMethod(configAndPreceding,
                                  jlong self,
                                  WCDBJNIObjectOrIntegerParameter(expression));
void WCDBJNIFrameSpecObjectMethod(configAndCurrentRow, jlong self);
void WCDBJNIFrameSpecObjectMethod(configAndFollowing,
                                  jlong self,
                                  WCDBJNIObjectOrIntegerParameter(expression));
void WCDBJNIFrameSpecObjectMethod(configAndUnboundedFollowing, jlong self);
