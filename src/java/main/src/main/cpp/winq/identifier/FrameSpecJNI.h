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
void WCDBJNIFrameSpecClassMethod(configRange, jlong self);
void WCDBJNIFrameSpecClassMethod(configRows, jlong self);
void WCDBJNIFrameSpecClassMethod(configUnboundedPreceding, jlong self);
void WCDBJNIFrameSpecClassMethod(configPreceding,
                                 jlong self,
                                 WCDBJNIObjectOrIntegerParameter(expression));
void WCDBJNIFrameSpecClassMethod(configCurrentRow, jlong self);
void WCDBJNIFrameSpecClassMethod(configBetweenUnboundedPreceding, jlong self);
void WCDBJNIFrameSpecClassMethod(configBetweenPreceding,
                                 jlong self,
                                 WCDBJNIObjectOrIntegerParameter(expression));
void WCDBJNIFrameSpecClassMethod(configBetweenCurrentRow, jlong self);
void WCDBJNIFrameSpecClassMethod(configBetweenFollowing,
                                 jlong self,
                                 WCDBJNIObjectOrIntegerParameter(expression));
void WCDBJNIFrameSpecClassMethod(configAndPreceding,
                                 jlong self,
                                 WCDBJNIObjectOrIntegerParameter(expression));
void WCDBJNIFrameSpecClassMethod(configAndCurrentRow, jlong self);
void WCDBJNIFrameSpecClassMethod(configAndFollowing,
                                 jlong self,
                                 WCDBJNIObjectOrIntegerParameter(expression));
void WCDBJNIFrameSpecClassMethod(configAndUnboundedFollowing, jlong self);
