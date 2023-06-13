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

jlong WCDBJNIExpressionClassMethod(createWithExistStatement, jlong select)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, select);
    return (jlong) WCDBExpressionCreateWithExistStatement(selectStruct).innerValue;
}

jlong WCDBJNIExpressionClassMethod(createWithNotExistStatement, jlong select)
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

jlong WCDBJNIExpressionClassMethod(cast, WCDBJNIObjectOrStringParameter(expression))
{
    WCDBJNICreateObjectOrStringCommonValue(expression);
    jlong ret = (jlong) WCDBExpressionCast2(expression_common).innerValue;
    WCDBJNITryReleaseStringInCommonValue(expression);
    return ret;
}

void WCDBJNIExpressionObjectMethod(as, jlong expression, jint type)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBExpressionAs(expressionStruct, type);
}

jlong WCDBJNIExpressionClassMethod(caseWithExp, WCDBJNIObjectOrStringParameter(expression))
{
    if (expression_type == 0) {
        return (jlong) WCDBExpressionCase().innerValue;
    }
    WCDBJNICreateObjectOrStringCommonValue(expression);
    jlong ret = (jlong) WCDBExpressionCaseWithExp2(expression_common).innerValue;
    WCDBJNITryReleaseStringInCommonValue(expression);
    return ret;
}

jlong WCDBJNIExpressionClassMethodWithNoArg(case_)
{
    return (jlong) WCDBExpressionCase().innerValue;
}

void WCDBJNIExpressionObjectMethod(setWithWhenExp,
                                   jlong expression,
                                   WCDBJNICommonValueParameter(when))
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNICreateCommonValue(when);
    WCDBExpressionSetWithWhenExp2(expressionStruct, when_common);
    WCDBJNITryReleaseStringInCommonValue(when);
}

void WCDBJNIExpressionObjectMethod(setWithThenExp,
                                   jlong expression,
                                   WCDBJNICommonValueParameter(then))
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNICreateCommonValue(then);
    WCDBExpressionSetWithThenExp2(expressionStruct, then_common);
    WCDBJNITryReleaseStringInCommonValue(then);
}

void WCDBJNIExpressionObjectMethod(setWithElseExp,
                                   jlong expression,
                                   WCDBJNICommonValueParameter(else_))
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNICreateCommonValue(else_);
    WCDBExpressionSetWithElseExp2(expressionStruct, else__common);
    WCDBJNITryReleaseStringInCommonValue(else_);
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

void WCDBJNIExpressionObjectMethod(filter, jlong expression, jlong condition)
{
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBJNIBridgeStruct(CPPExpression, condition);
    WCDBExpressionFilter(expressionStruct, conditionStruct);
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
