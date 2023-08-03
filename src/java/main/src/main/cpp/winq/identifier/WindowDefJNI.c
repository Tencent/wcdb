// Created by chenqiuwen on 2023/6/10.
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

#include "WindowDefJNI.h"
#include "WindowDefBridge.h"

jlong WCDBJNIWindowDefClassMethodWithNoArg(createCppObj)
{
    return (jlong) WCDBWindowDefCreate().innerValue;
}

void WCDBJNIWindowDefObjectMethod(configPartitions,
                                  jlong self,
                                  WCDBJNIMultiTypeArrayParameter(partitions))
{
    WCDBJNIBridgeStruct(CPPWindowDef, self);
    WCDBJNICreateMultiTypeArray(partitions);
    WCDBWindowDefConfigPartition2(selfStruct, partitionsArray);
    WCDBJNIReleaseMultiTypeArray(partitions);
}

void WCDBJNIWindowDefObjectMethod(configOrders, jlong self, jlongArray orders)
{
    WCDBJNIBridgeStruct(CPPWindowDef, self);
    WCDBJNIGetCppPointerArrayCritical(orders);
    WCDBWindowDefConfigOrder(selfStruct, (const CPPOrderingTerm*) ordersArray, ordersLength);
    WCDBJNIReleaseCppPointerArrayCritical(orders);
}

void WCDBJNIWindowDefObjectMethod(configFrameSpec, jlong self, jlong frameSpec)
{
    WCDBJNIBridgeStruct(CPPWindowDef, self);
    WCDBJNIBridgeStruct(CPPFrameSpec, frameSpec);
    WCDBWindowDefConfigFrameSpec(selfStruct, frameSpecStruct);
}