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

#include "JoinJNI.h"
#include "JoinBridge.h"

jlong WCDBJNIJoinClassMethod(createCppObj, WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNICreateObjectOrStringCommonValue(query);
    jlong ret = (jlong) WCDBJoinCreateWithTableOrSubquery2(query_common).innerValue;
    WCDBJNITryReleaseStringInCommonValue(query);
    return ret;
}

void WCDBJNIJoinObjectMethod(configWith, jlong join, WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWith2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configWithJoin, jlong join, WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWithJoin2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configWithLeftOuterJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWithLeftOuterJoin2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configWithLeftJoin, jlong join, WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWithLeftJoin2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configWithInnerJoin, jlong join, WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWithInnerJoin2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configWithCrossJoin, jlong join, WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWithCrossJoin2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configWithNaturalJoin, jlong join, WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWithNaturalJoin2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configWithNaturalLeftOuterJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWithNaturalLeftOuterJoin2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configWithNaturalLeftJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWithNaturalLeftJoin2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configWithNaturalInnerJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWithNaturalInnerJoin2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configWithNaturalCrossJoin,
                             jlong join,
                             WCDBJNIObjectOrStringParameter(query))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringCommonValue(query);
    WCDBJoinWithNaturalCrossJoin2(joinStruct, query_common);
    WCDBJNITryReleaseStringInCommonValue(query);
}

void WCDBJNIJoinObjectMethod(configOn, jlong join, jlong expression)
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJoinConfigOn(joinStruct, expressionStruct);
}

void WCDBJNIJoinObjectMethod(configUsingColumn,
                             jlong join,
                             WCDBJNIObjectOrStringArrayParameter(columns))
{
    WCDBJNIBridgeStruct(CPPJoin, join);
    WCDBJNICreateObjectOrStringArrayWithAction(
    columns, WCDBJoinConfigUsingColumn2(joinStruct, columns_commonArray));
}