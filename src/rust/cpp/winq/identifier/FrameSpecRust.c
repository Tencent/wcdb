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

#include "FrameSpecRust.h"

#include "FrameSpecBridge.h"

void* WCDBRustFrameSpecClassMethodWithNoArg(createCppObj) {
    return (void*)WCDBFrameSpecCreate().innerValue;
}

void WCDBRustFrameSpecClassMethod(configRange, void* self) {
    WCDBRustBridgeStruct(CPPFrameSpec, self);
    WCDBFrameSpecConfigRange(selfStruct);
}
//
// void WCDBRustFrameSpecClassMethod(configRows, jlong self)
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBFrameSpecConfigRows(selfStruct);
// }

void WCDBRustFrameSpecClassMethod(configUnboundedPreceding, void* self) {
    WCDBRustBridgeStruct(CPPFrameSpec, self);
    WCDBFrameSpecConfigUnboundedPreceding(selfStruct);
}

// void WCDBRustFrameSpecClassMethod(configPreceding,
//                                  jlong self,
//                                  WCDBRustObjectOrIntegerParameter(expression))
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBRustCreateObjectOrIntegerCommonValue(expression);
//     WCDBFrameSpecConfigPreceding2(selfStruct, expression_common);
// }
//
// void WCDBRustFrameSpecClassMethod(configCurrentRow, jlong self)
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBFrameSpecConfigCurrentRow(selfStruct);
// }
//
// void WCDBRustFrameSpecClassMethod(configBetweenUnboundedPreceding, jlong self)
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBFrameSpecConfigBetweenUnboundedPreceding(selfStruct);
// }
//
// void WCDBRustFrameSpecClassMethod(configBetweenPreceding,
//                                  jlong self,
//                                  WCDBRustObjectOrIntegerParameter(expression))
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBRustCreateObjectOrIntegerCommonValue(expression);
//     WCDBFrameSpecConfigBetweenPreceding2(selfStruct, expression_common);
// }
//
// void WCDBRustFrameSpecClassMethod(configBetweenCurrentRow, jlong self)
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBFrameSpecConfigBetweenCurrentRow(selfStruct);
// }
//
// void WCDBRustFrameSpecClassMethod(configBetweenFollowing,
//                                  jlong self,
//                                  WCDBRustObjectOrIntegerParameter(expression))
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBRustCreateObjectOrIntegerCommonValue(expression);
//     WCDBFrameSpecConfigBetweenFollowing2(selfStruct, expression_common);
// }
//
// void WCDBRustFrameSpecClassMethod(configAndPreceding,
//                                  jlong self,
//                                  WCDBRustObjectOrIntegerParameter(expression))
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBRustCreateObjectOrIntegerCommonValue(expression);
//     WCDBFrameSpecConfigAndPreceding2(selfStruct, expression_common);
// }
//
// void WCDBRustFrameSpecClassMethod(configAndCurrentRow, jlong self)
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBFrameSpecConfigAndCurrentRow(selfStruct);
// }
//
// void WCDBRustFrameSpecClassMethod(configAndFollowing,
//                                  jlong self,
//                                  WCDBRustObjectOrIntegerParameter(expression))
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBRustCreateObjectOrIntegerCommonValue(expression);
//     WCDBFrameSpecConfigAndFollowing2(selfStruct, expression_common);
// }
//
// void WCDBRustFrameSpecClassMethod(configAndUnboundedFollowing, jlong self)
// {
//     WCDBRustBridgeStruct(CPPFrameSpec, self);
//     WCDBFrameSpecConfigAndUnboundedFollowing(selfStruct);
// }
