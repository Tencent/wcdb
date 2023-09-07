//
// Created by qiuwenchen on 2022/5/31.
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

#include "ExpressionBridge.h"

WCDB_EXTERN_C_BEGIN

enum WCDBExpressionUnaryOperateType {
    WCDBExpressionUnaryOperateType_Negative = 1,
    WCDBExpressionUnaryOperateType_Positive,
    WCDBExpressionUnaryOperateType_Tilde,
    WCDBExpressionUnaryOperateType_Not,
};
CPPExpression WCDBExpressionUnaryOperate(CPPExpression expression,
                                         enum WCDBExpressionUnaryOperateType operatorType);
CPPExpression WCDBExpressionNullOperate(CPPExpression expression, bool isNot);

CPPExpression WCDBExpressionNullOperate2(CPPCommonValue expression, bool isNot);

enum WCDBExpressionBinaryOperatorType {
    WCDBExpressionBinaryOperatorType_Concatenate = 1,
    WCDBExpressionBinaryOperatorType_Multiply,
    WCDBExpressionBinaryOperatorType_Divide,
    WCDBExpressionBinaryOperatorType_Modulo,
    WCDBExpressionBinaryOperatorType_Plus,
    WCDBExpressionBinaryOperatorType_Minus,
    WCDBExpressionBinaryOperatorType_LeftShift,
    WCDBExpressionBinaryOperatorType_RightShift,
    WCDBExpressionBinaryOperatorType_BitwiseAnd,
    WCDBExpressionBinaryOperatorType_BitwiseOr,
    WCDBExpressionBinaryOperatorType_Less,
    WCDBExpressionBinaryOperatorType_LessOrEqual,
    WCDBExpressionBinaryOperatorType_Greater,
    WCDBExpressionBinaryOperatorType_GreaterOrEqual,
    WCDBExpressionBinaryOperatorType_Equal,
    WCDBExpressionBinaryOperatorType_NotEqual,
    WCDBExpressionBinaryOperatorType_Is,
    WCDBExpressionBinaryOperatorType_And,
    WCDBExpressionBinaryOperatorType_Or,
    WCDBExpressionBinaryOperatorType_Like,
    WCDBExpressionBinaryOperatorType_GLOB,
    WCDBExpressionBinaryOperatorType_RegExp,
    WCDBExpressionBinaryOperatorType_Match,
};
CPPExpression WCDBExpressionBinaryOperate(CPPExpression left,
                                          CPPExpression right,
                                          enum WCDBExpressionBinaryOperatorType operatorType,
                                          bool isNot);

CPPExpression WCDBExpressionBinaryOperate2(CPPCommonValue left,
                                           CPPCommonValue right,
                                           enum WCDBExpressionBinaryOperatorType operatorType,
                                           bool isNot);

CPPExpression WCDBExpressionBetweenOperate(CPPExpression operand,
                                           CPPExpression left,
                                           CPPExpression right,
                                           bool isNot);

CPPExpression WCDBExpressionBetweenOperate2(CPPCommonValue operand,
                                            CPPCommonValue left,
                                            CPPCommonValue right,
                                            bool isNot);

CPPExpression
WCDBExpressionInOperate(CPPCommonValue operand, CPPCommonArray valueArray, bool isNot);

CPPExpression
WCDBExpressionInTableOperate(CPPExpression operand, const char* _Nullable table, bool isNot);
CPPExpression
WCDBExpressionInTableOperate2(CPPCommonValue operand, const char* _Nullable table, bool isNot);

CPPExpression
WCDBExpressionInSelectionOperate(CPPExpression operand, CPPStatementSelect select, bool isNot);

CPPExpression WCDBExpressionInSelectionOperate2(CPPCommonValue operand,
                                                CPPStatementSelect select,
                                                bool isNot);

CPPExpression WCDBExpressionInExpressionOperate(CPPExpression operand,
                                                const CPPExpression* _Nullable expressions,
                                                int num,
                                                bool isNot);

CPPExpression
WCDBExpressionInFunctionOperate(CPPExpression operand, const char* _Nullable func, bool isNot);

CPPExpression WCDBExpressionInFunctionOperate2(CPPCommonValue operand,
                                               const char* _Nullable func,
                                               bool isNot);

CPPExpression
WCDBExpressionCollateOperate(CPPExpression operand, const char* _Nullable collation);

CPPExpression
WCDBExpressionCollateOperate2(CPPCommonValue operand, const char* _Nullable collation);

WCDB_EXTERN_C_END
