// Created by qiuwenchen on 2023/3/31.
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

#include "ExpressionJNI.h"
#include "ExpressionBridge.h"

jlong WCDBJNIExpressionObjectMethod(create, int type, jlong object)
{
    CPPCommonValue commonValue;
    commonValue.type = type;
    commonValue.intValue = object;
    long ret = (jlong) WCDBExpressionCreate(commonValue).innerValue;
    return ret;
}

jlong WCDBJNIExpressionClassMethod(createWithFunction, jstring funcName)
{
    WCDBJNIGetString(funcName);
    jlong ret = (jlong) WCDBExpressionCreateWithFunction(funcNameString).innerValue;
    WCDBJNIReleaseString(funcName);
    return ret;
}

jlong WCDBJNIExpressionObjectMethod(createWithExistStatement, jlong select)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, select);
    return (jlong) WCDBExpressionCreateWithExistStatement(selectStruct).innerValue;
}

jlong WCDBJNIExpressionObjectMethod(createWithNotExistStatement, jlong select)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, select);
    return (jlong) WCDBExpressionCreateWithNotExistStatement(selectStruct).innerValue;
}

void WCDBJNIExpressionObjectMethod(setWithSchema, jlong expression, jlong schema)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNIBridgeStruct(CPPSchema, schema);
    WCDBExpressionSetWithSchema(expressionStruct, schemaStruct);
}

void WCDBJNIExpressionObjectMethod(setArgument, jlong expression, WCDBJNICommonValueParameter(argument))
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNICreateCommonValue(argument);
    WCDBExpressionSetArgument(expressionStruct, argument_common);
    WCDBJNITryReleaseStringInCommonValue(argument);
}

void WCDBJNIExpressionObjectMethod(invoke, jlong expression)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBExpressionInvoke(expressionStruct);
}

void WCDBJNIExpressionObjectMethod(invokeAll, jlong expression)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBExpressionInvokeAll(expressionStruct);
}

void WCDBJNIExpressionObjectMethod(distinct, jlong expression)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBExpressionDistinct(expressionStruct);
}

void WCDBJNIExpressionObjectMethod(cast, jlong expression)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBExpressionCast(expressionStruct);
}

void WCDBJNIExpressionObjectMethod(as, jlong expression, jint type)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBExpressionAs(expressionStruct, type);
}

jlong WCDBJNIExpressionObjectMethod(caseWithExp, jlong expression)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    return (jlong) WCDBExpressionCaseWithExp(expressionStruct).innerValue;
}

jlong WCDBJNIExpressionClassMethodWithNoArg(case_)
{
    return (jlong) WCDBExpressionCase().innerValue;
}

void WCDBJNIExpressionObjectMethod(setWithWhenExp, jlong expression, jlong when)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNIBridgeStruct(CPPExpression, when);
    WCDBExpressionSetWithWhenExp(expressionStruct, whenStruct);
}

void WCDBJNIExpressionObjectMethod(setWithThenExp, jlong expression, jlong then)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNIBridgeStruct(CPPExpression, then);
    WCDBExpressionSetWithThenExp(expressionStruct, thenStruct);
}

void WCDBJNIExpressionObjectMethod(setWithElseExp, jlong expression, jlong else_)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNIBridgeStruct(CPPExpression, else_);
    WCDBExpressionSetWithElseExp(expressionStruct, else_Struct);
}

void WCDBJNIExpressionObjectMethod(escapeWith, jlong expression, jstring content)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNIGetString(content);
    WCDBExpressionEscapeWith2(expressionStruct, contentString);
    WCDBJNIReleaseString(content);
}

jlong WCDBJNIExpressionObjectMethod(createWithWindowFunction, jstring funcName)
{
    WCDBJNIGetString(funcName);
    jlong ret = (jlong) WCDBExpressionCreateWithWindowFunction(funcNameString).innerValue;
    WCDBJNIReleaseString(funcName);
    return ret;
}

void WCDBJNIExpressionObjectMethod(filter, jlong expression, jlong filter)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNIBridgeStruct(CPPFilter, filter);
    WCDBExpressionFilter(expressionStruct, filterStruct);
}

void WCDBJNIExpressionObjectMethod(overWindowDef, jlong expression, jlong def)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNIBridgeStruct(CPPWindowDef, def);
    WCDBExpressionOverWindowDef(expressionStruct, defStruct);
}

void WCDBJNIExpressionObjectMethod(overWindow, jlong expression, jstring window)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNIGetString(window);
    WCDBExpressionOverWindow(expressionStruct, windowString);
    WCDBJNIReleaseString(window);
}
