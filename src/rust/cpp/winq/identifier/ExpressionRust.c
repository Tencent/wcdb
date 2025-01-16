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

void* WCDBRustExpressionClassMethod(create, int type, long long object) {
    CPPCommonValue commonValue;
    commonValue.type = type;
    commonValue.intValue = object;
    void* ret = (void*)WCDBExpressionCreate(commonValue).innerValue;
    return ret;
}

// jlong WCDBRustExpressionClassMethod(createWithFunction, jstring funcName)
//{
//     WCDBRustGetStringCritical(funcName);
//     jlong ret = (jlong) WCDBExpressionCreateWithFunction(funcNameString).innerValue;
//     WCDBRustReleaseStringCritical(funcName);
//     return ret;
// }
//
// jlong WCDBRustExpressionClassMethod(createWithExistStatement, jlong select)
//{
//     WCDBRustBridgeStruct(CPPStatementSelect, select);
//     return (jlong) WCDBExpressionCreateWithExistStatement(selectStruct).innerValue;
// }
//
// jlong WCDBRustExpressionClassMethod(createWithNotExistStatement, jlong select)
//{
//     WCDBRustBridgeStruct(CPPStatementSelect, select);
//     return (jlong) WCDBExpressionCreateWithNotExistStatement(selectStruct).innerValue;
// }
//
// void WCDBRustExpressionClassMethod(setWithSchema,
//                                   jlong expression,
//                                   WCDBRustObjectOrStringParameter(schema))
//{
//     WCDBRustBridgeStruct(CPPExpression, expression);
//     WCDBRustCreateObjectOrStringCommonValue(schema, true);
//     WCDBExpressionSetWithSchema2(expressionStruct, schema_common);
//     WCDBRustTryReleaseStringInCommonValue(schema);
// }
//
void WCDBRustExpressionClassMethod(argument,
                                   void* expression,
                                   WCDBRustCommonValueParameter(argument)) {
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBRustCreateCommonValue(argument);
    WCDBExpressionSetArgument(expressionStruct, argument_common);
    //    WCDBRustTryReleaseStringInCommonValue(argument); // todo qixinbing : 需要释放？
}
//
// void WCDBRustExpressionClassMethod(invoke, jlong expression)
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBExpressionInvoke(expressionStruct);
//}
//
// void WCDBRustExpressionClassMethod(invokeAll, jlong expression)
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBExpressionInvokeAll(expressionStruct);
//}
//
// void WCDBRustExpressionClassMethod(distinct, jlong expression)
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBExpressionDistinct(expressionStruct);
//}
//
// jlong WCDBRustExpressionClassMethod(cast, WCDBRustObjectOrStringParameter(expression))
//{
//    WCDBRustCreateObjectOrStringCommonValue(expression, true);
//    jlong ret = (jlong) WCDBExpressionCast2(expression_common).innerValue;
//    WCDBRustTryReleaseStringInCommonValue(expression);
//    return ret;
//}
//
// void WCDBRustExpressionClassMethod(as, jlong expression, jint type)
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBExpressionAs(expressionStruct, type);
//}
//
// jlong WCDBRustExpressionClassMethod(configAlias, jlong expression, jstring alias)
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBRustGetString(alias);
//    jlong ret = (jlong) WCDBExpressionConfigAlias(expressionStruct, aliasString).innerValue;
//    WCDBRustReleaseString(alias);
//    return ret;
//}
//
// jlong WCDBRustExpressionClassMethod(caseWithExp, WCDBRustObjectOrStringParameter(expression))
//{
//    if (expression_type == 0) {
//        return (jlong) WCDBExpressionCase().innerValue;
//    }
//    WCDBRustCreateObjectOrStringCommonValue(expression, true);
//    jlong ret = (jlong) WCDBExpressionCaseWithExp2(expression_common).innerValue;
//    WCDBRustTryReleaseStringInCommonValue(expression);
//    return ret;
//}
//
// jlong WCDBRustExpressionClassMethodWithNoArg(case_)
//{
//    return (jlong) WCDBExpressionCase().innerValue;
//}
//
// void WCDBRustExpressionClassMethod(setWithWhenExp, jlong expression,
// WCDBRustCommonValueParameter(when))
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBRustCreateCommonValue(when, true);
//    WCDBExpressionSetWithWhenExp2(expressionStruct, when_common);
//    WCDBRustTryReleaseStringInCommonValue(when);
//}
//
// void WCDBRustExpressionClassMethod(setWithThenExp, jlong expression,
// WCDBRustCommonValueParameter(then))
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBRustCreateCommonValue(then, true);
//    WCDBExpressionSetWithThenExp2(expressionStruct, then_common);
//    WCDBRustTryReleaseStringInCommonValue(then);
//}
//
// void WCDBRustExpressionClassMethod(setWithElseExp, jlong expression,
// WCDBRustCommonValueParameter(else_))
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBRustCreateCommonValue(else_, true);
//    WCDBExpressionSetWithElseExp2(expressionStruct, else__common);
//    WCDBRustTryReleaseStringInCommonValue(else_);
//}
//
// void WCDBRustExpressionClassMethod(escapeWith, jlong expression, jstring content)
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBRustGetStringCritical(content);
//    WCDBExpressionEscapeWith2(expressionStruct, contentString);
//    WCDBRustReleaseStringCritical(content);
//}
//
// jlong WCDBRustExpressionClassMethod(createWithWindowFunction, jstring funcName)
//{
//    WCDBRustGetStringCritical(funcName);
//    jlong ret = (jlong) WCDBExpressionCreateWithWindowFunction(funcNameString).innerValue;
//    WCDBRustReleaseStringCritical(funcName);
//    return ret;
//}
//
// void WCDBRustExpressionClassMethod(filter, jlong expression, jlong condition)
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBRustBridgeStruct(CPPExpression, condition);
//    WCDBExpressionFilter(expressionStruct, conditionStruct);
//}
//
// void WCDBRustExpressionClassMethod(overWindowDef, jlong expression, jlong def)
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBRustBridgeStruct(CPPWindowDef, def);
//    WCDBExpressionOverWindowDef(expressionStruct, defStruct);
//}
//
// void WCDBRustExpressionClassMethod(overWindow, jlong expression, jstring window)
//{
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBRustGetStringCritical(window);
//    WCDBExpressionOverWindow(expressionStruct, windowString);
//    WCDBRustReleaseStringCritical(window);
//}
