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

#include "FrameSpecJNI.h"
#include "FrameSpecBridge.h"

jlong WCDBJNIFrameSpecClassMethodWithNoArg(createCppObj)
{
    return (jlong) WCDBFrameSpecCreate().innerValue;
}

void WCDBJNIFrameSpecObjectMethod(configRange, jlong self)
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBFrameSpecConfigRange(selfStruct);
}

void WCDBJNIFrameSpecObjectMethod(configRows, jlong self)
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBFrameSpecConfigRows(selfStruct);
}

void WCDBJNIFrameSpecObjectMethod(configUnboundedPreceding, jlong self)
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBFrameSpecConfigUnboundedPreceding(selfStruct);
}

void WCDBJNIFrameSpecObjectMethod(configPreceding,
                                  jlong self,
                                  WCDBJNIObjectOrIntegerParameter(expression))
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBJNICreateObjectOrIntegerCommonValue(expression);
    WCDBFrameSpecConfigPreceding2(selfStruct, expression_common);
}

void WCDBJNIFrameSpecObjectMethod(configCurrentRow, jlong self)
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBFrameSpecConfigCurrentRow(selfStruct);
}

void WCDBJNIFrameSpecObjectMethod(configBetweenUnboundedPreceding, jlong self)
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBFrameSpecConfigBetweenUnboundedPreceding(selfStruct);
}

void WCDBJNIFrameSpecObjectMethod(configBetweenPreceding,
                                  jlong self,
                                  WCDBJNIObjectOrIntegerParameter(expression))
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBJNICreateObjectOrIntegerCommonValue(expression);
    WCDBFrameSpecConfigBetweenPreceding2(selfStruct, expression_common);
}

void WCDBJNIFrameSpecObjectMethod(configBetweenCurrentRow, jlong self)
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBFrameSpecConfigBetweenCurrentRow(selfStruct);
}

void WCDBJNIFrameSpecObjectMethod(configBetweenFollowing,
                                  jlong self,
                                  WCDBJNIObjectOrIntegerParameter(expression))
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBJNICreateObjectOrIntegerCommonValue(expression);
    WCDBFrameSpecConfigBetweenFollowing2(selfStruct, expression_common);
}

void WCDBJNIFrameSpecObjectMethod(configAndPreceding,
                                  jlong self,
                                  WCDBJNIObjectOrIntegerParameter(expression))
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBJNICreateObjectOrIntegerCommonValue(expression);
    WCDBFrameSpecConfigAndPreceding2(selfStruct, expression_common);
}

void WCDBJNIFrameSpecObjectMethod(configAndCurrentRow, jlong self)
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBFrameSpecConfigAndCurrentRow(selfStruct);
}

void WCDBJNIFrameSpecObjectMethod(configAndFollowing,
                                  jlong self,
                                  WCDBJNIObjectOrIntegerParameter(expression))
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBJNICreateObjectOrIntegerCommonValue(expression);
    WCDBFrameSpecConfigAndFollowing2(selfStruct, expression_common);
}

void WCDBJNIFrameSpecObjectMethod(configAndUnboundedFollowing, jlong self)
{
    WCDBJNIBridgeStruct(CPPFrameSpec, self);
    WCDBFrameSpecConfigAndUnboundedFollowing(selfStruct);
}
