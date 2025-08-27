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

#include "ExpressionRust.h"

#include "ExpressionBridge.h"

void* WCDBRustExpressionClassMethod(create, int type, void* object) {
    CPPCommonValue commonValue;
    commonValue.type = type;
    commonValue.intValue = (long long)object;
    return WCDBExpressionCreate(commonValue).innerValue;
}

void* WCDBRustExpressionClassMethod(createWithFunction, const char* func) {
    return WCDBExpressionCreateWithFunction(func).innerValue;
}

//
// jlong WCDBRustExpressionClassMethod(createWithExistStatement, jlong select)
//{
//     WCDBRustBridgeStruct(CPPStatementSelect, select);
//     return (jlong) WCDBExpressionCreateWithExistStatement(selectStruct).innerValue;
// }

void* WCDBRustExpressionClassMethod(createWithExistStatement, void* select) {
    WCDBRustBridgeStruct(CPPStatementSelect, select);
    return WCDBExpressionCreateWithExistStatement(selectStruct).innerValue;
}

void* WCDBRustExpressionClassMethod(createWithNotExistStatement, void* select) {
    WCDBRustBridgeStruct(CPPStatementSelect, select);
    return WCDBExpressionCreateWithNotExistStatement(selectStruct).innerValue;
}

 void WCDBRustExpressionClassMethod(setWithSchema,
                                   void* expression,
                                   WCDBRustObjectOrStringParameter(schema))
{
     WCDBRustBridgeStruct(CPPExpression, expression);
     WCDBRustCreateObjectOrStringCommonValue(schema, true);
     WCDBExpressionSetWithSchema2(expressionStruct, schema_common);
     // WCDBRustTryReleaseStringInCommonValue(schema);
 }

void WCDBRustExpressionClassMethod(argument,
                                   void* expression,
                                   WCDBRustCommonValueParameter(argument)) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBRustCreateCommonValue(argument);
    WCDBExpressionSetArgument(expressionStruct, argument_common);
    //    WCDBRustTryReleaseStringInCommonValue(argument); // todo qixinbing : 需要释放？
}

void WCDBRustExpressionClassMethod(invoke, void* expression) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBExpressionInvoke(expressionStruct);
}

void WCDBRustExpressionClassMethod(invokeAll, void* expression) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBExpressionInvokeAll(expressionStruct);
}
void WCDBRustExpressionClassMethod(distinct, void* expression) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBExpressionDistinct(expressionStruct);
}

void* WCDBRustExpressionClassMethod(cast, WCDBRustObjectOrStringParameter(expression)) {
    WCDBRustCreateObjectOrStringCommonValue(expression, true);
    void* ret = WCDBExpressionCast2(expression_common).innerValue;
    // WCDBRustTryReleaseStringInCommonValue(expression);
    return ret;
}

void WCDBRustExpressionClassMethod(as, void* expression, int type) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBExpressionAs(expressionStruct, type);
}

void* WCDBRustExpressionClassMethod(configAlias, void* expression, const char* alias) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    void* ret = WCDBExpressionConfigAlias(expressionStruct, alias).innerValue;
    return ret;
}

void* WCDBRustExpressionClassMethod(caseWithExp, WCDBRustObjectOrStringParameter(expression)) {
    if (expression_type == 0) {
        return WCDBExpressionCase().innerValue;
    }
    WCDBRustCreateObjectOrStringCommonValue(expression, true);
    void* ret = WCDBExpressionCaseWithExp2(expression_common).innerValue;
    // WCDBRustTryReleaseStringInCommonValue(expression);
    return ret;
}

void* WCDBRustExpressionClassMethodWithNoArg(case_) {
    return (void*)WCDBExpressionCase().innerValue;
}

void WCDBRustExpressionClassMethod(setWithWhenExp,
                                   void* expression,
                                   WCDBRustCommonValueParameter(when)) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBRustCreateCommonValue(when);
    WCDBExpressionSetWithWhenExp2(expressionStruct, when_common);
    // WCDBRustTryReleaseStringInCommonValue(when);
}

void WCDBRustExpressionClassMethod(setWithThenExp,
                                   void* expression,
                                   WCDBRustCommonValueParameter(then)) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBRustCreateCommonValue(then);
    WCDBExpressionSetWithThenExp2(expressionStruct, then_common);
    // WCDBRustTryReleaseStringInCommonValue(then);
}

void WCDBRustExpressionClassMethod(setWithElseExp,
                                   void* expression,
                                   WCDBRustCommonValueParameter(else_)) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBRustCreateCommonValue(else_);
    WCDBExpressionSetWithElseExp2(expressionStruct, else__common);
    // WCDBRustTryReleaseStringInCommonValue(else_);
}

void WCDBRustExpressionClassMethod(escapeWith, void* expression, const char* content) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBExpressionEscapeWith2(expressionStruct, content);
}

// void WCDBRustExpressionClassMethod(escapeWith, jlong expression, jstring content)
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBRustGetStringCritical(content);
//    WCDBExpressionEscapeWith2(expressionStruct, contentString);
//    WCDBRustReleaseStringCritical(content);
//}
//
void* WCDBRustExpressionClassMethod(createWithWindowFunction, const char* funcName) {
    // WCDBRustGetStringCritical(funcName);
    void* ret = (void*)WCDBExpressionCreateWithWindowFunction(funcName).innerValue;
    // WCDBRustReleaseStringCritical(funcName);
    return ret;
}

void WCDBRustExpressionClassMethod(filter, void* expression, void* condition) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBRustBridgeStruct(CPPExpression, condition);
    WCDBExpressionFilter(expressionStruct, conditionStruct);
}

void WCDBRustExpressionClassMethod(overWindowDef, void* expression, void* def) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBRustBridgeStruct(CPPWindowDef, def);
    WCDBExpressionOverWindowDef(expressionStruct, defStruct);
}

void WCDBRustExpressionClassMethod(overWindow, void* expression, const char* windowString) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    // WCDBRustGetStringCritical(window);
    WCDBExpressionOverWindow(expressionStruct, windowString);
    // WCDBRustReleaseStringCritical(window);
}
