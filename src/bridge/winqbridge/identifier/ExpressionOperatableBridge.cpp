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

#include "ExpressionOperatableBridge.h"
#include "Assertion.hpp"
#include "BindParameter.hpp"
#include "Column.hpp"
#include "Expression.hpp"
#include "LiteralValue.hpp"
#include "ObjectBridge.hpp"
#include "StatementSelect.hpp"
#include "WinqBridge.hpp"

#define WCDBExpressionPushOperand(operand)                                     \
    newExpression->syntax().expressions.push_back(                             \
    WCDB::WinqBridge::createExpression(operand));

CPPExpression WCDBExpressionUnaryOperate(CPPExpression expression,
                                         enum WCDBExpressionUnaryOperateType operatorType)
{
    WCDBGetObjectOrReturnValue(expression, WCDB::Expression, cppExpression, CPPExpression());
    WCDB::Expression::SyntaxType::UnaryOperator cppOperatorType;
    switch (operatorType) {
    case WCDBExpressionUnaryOperateType_Not:
        cppOperatorType = WCDB::Expression::SyntaxType::UnaryOperator::Not;
        break;
    case WCDBExpressionUnaryOperateType_Negative:
        cppOperatorType = WCDB::Expression::SyntaxType::UnaryOperator::Negative;
        break;
    case WCDBExpressionUnaryOperateType_Positive:
        cppOperatorType = WCDB::Expression::SyntaxType::UnaryOperator::Positive;
        break;
    case WCDBExpressionUnaryOperateType_Tilde:
        cppOperatorType = WCDB::Expression::SyntaxType::UnaryOperator::Tilde;
        break;
    default:
        WCTAssert(false);
    }

    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression());

    newExpression->syntax().switcher = WCDB::Expression::SyntaxType::Switch::UnaryOperation;
    newExpression->syntax().unaryOperator = cppOperatorType;
    newExpression->syntax().expressions.push_back(*cppExpression);
    return ret;
}

CPPExpression WCDBExpressionNullOperate(CPPExpression expression, bool isNot)
{
    WCDBGetObjectOrReturnValue(expression, WCDB::Expression, cppExpression, CPPExpression());

    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::UnaryOperation;
    newExpression->syntax().unaryOperator
    = WCDB::Expression::SyntaxType::UnaryOperator::Null;
    newExpression->syntax().expressions.push_back(*cppExpression);
    newExpression->syntax().isNot = isNot;
    return ret;
}

CPPExpression WCDBExpressionNullOperate2(CPPCommonValue expression, bool isNot)
{
    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::UnaryOperation;
    newExpression->syntax().unaryOperator
    = WCDB::Expression::SyntaxType::UnaryOperator::Null;
    newExpression->syntax().isNot = isNot;
    WCDBExpressionPushOperand(expression);
    return ret;
}

WCDB::Expression::SyntaxType::BinaryOperator
WCDBCastOperatorType(enum WCDBExpressionBinaryOperatorType operatorType)
{
    WCDB::Expression::SyntaxType::BinaryOperator cppOperatorType;
    switch (operatorType) {
    case WCDBExpressionBinaryOperatorType_Concatenate:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Concatenate;
        break;
    case WCDBExpressionBinaryOperatorType_Multiply:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Multiply;
        break;
    case WCDBExpressionBinaryOperatorType_Divide:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Divide;
        break;
    case WCDBExpressionBinaryOperatorType_Modulo:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Modulo;
        break;
    case WCDBExpressionBinaryOperatorType_Plus:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Plus;
        break;
    case WCDBExpressionBinaryOperatorType_Minus:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Minus;
        break;
    case WCDBExpressionBinaryOperatorType_LeftShift:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::LeftShift;
        break;
    case WCDBExpressionBinaryOperatorType_RightShift:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::RightShift;
        break;
    case WCDBExpressionBinaryOperatorType_BitwiseAnd:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::BitwiseAnd;
        break;
    case WCDBExpressionBinaryOperatorType_BitwiseOr:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::BitwiseOr;
        break;
    case WCDBExpressionBinaryOperatorType_Less:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Less;
        break;
    case WCDBExpressionBinaryOperatorType_LessOrEqual:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::LessOrEqual;
        break;
    case WCDBExpressionBinaryOperatorType_Greater:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Greater;
        break;
    case WCDBExpressionBinaryOperatorType_GreaterOrEqual:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::GreaterOrEqual;
        break;
    case WCDBExpressionBinaryOperatorType_Equal:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Equal;
        break;
    case WCDBExpressionBinaryOperatorType_NotEqual:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::NotEqual;
        break;
    case WCDBExpressionBinaryOperatorType_Is:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Is;
        break;
    case WCDBExpressionBinaryOperatorType_And:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::And;
        break;
    case WCDBExpressionBinaryOperatorType_Or:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Or;
        break;
    case WCDBExpressionBinaryOperatorType_Like:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Like;
        break;
    case WCDBExpressionBinaryOperatorType_GLOB:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::GLOB;
        break;
    case WCDBExpressionBinaryOperatorType_RegExp:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::RegExp;
        break;
    case WCDBExpressionBinaryOperatorType_Match:
        cppOperatorType = WCDB::Expression::SyntaxType::BinaryOperator::Match;
        break;
    }
    return cppOperatorType;
}

CPPExpression WCDBExpressionBinaryOperate(CPPExpression left,
                                          CPPExpression right,
                                          enum WCDBExpressionBinaryOperatorType operatorType,
                                          bool isNot)
{
    WCDBGetObjectOrReturnValue(left, WCDB::Expression, cppLeft, CPPExpression());
    WCDBGetObjectOrReturnValue(right, WCDB::Expression, cppRight, CPPExpression());

    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::BinaryOperation;
    newExpression->syntax().binaryOperator = WCDBCastOperatorType(operatorType);
    newExpression->syntax().expressions.push_back(*cppLeft);
    newExpression->syntax().expressions.push_back(*cppRight);
    newExpression->syntax().isNot = isNot;
    return ret;
}

CPPExpression WCDBExpressionBinaryOperate2(CPPCommonValue left,
                                           CPPCommonValue right,
                                           enum WCDBExpressionBinaryOperatorType operatorType,
                                           bool isNot)
{
    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::BinaryOperation;
    newExpression->syntax().binaryOperator = WCDBCastOperatorType(operatorType);
    newExpression->syntax().isNot = isNot;
    WCDBExpressionPushOperand(left);
    WCDBExpressionPushOperand(right);
    return ret;
}

CPPExpression
WCDBExpressionBetweenOperate(CPPExpression operand, CPPExpression left, CPPExpression right, bool isNot)
{
    WCDBGetObjectOrReturnValue(operand, WCDB::Expression, cppOperand, CPPExpression());
    WCDBGetObjectOrReturnValue(left, WCDB::Expression, cppLeft, CPPExpression());
    WCDBGetObjectOrReturnValue(right, WCDB::Expression, cppRight, CPPExpression());

    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::Between;
    auto& expressions = newExpression->syntax().expressions;
    expressions.push_back(*cppOperand);
    expressions.push_back(*cppLeft);
    expressions.push_back(*cppRight);
    newExpression->syntax().isNot = isNot;

    return ret;
}

CPPExpression WCDBExpressionBetweenOperate2(CPPCommonValue operand,
                                            CPPCommonValue left,
                                            CPPCommonValue right,
                                            bool isNot)
{
    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())
    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::Between;
    newExpression->syntax().isNot = isNot;
    WCDBExpressionPushOperand(operand);
    WCDBExpressionPushOperand(left);
    WCDBExpressionPushOperand(right);
    return ret;
}

CPPExpression
WCDBExpressionInTableOperate(CPPExpression operand, const char* _Nullable table, bool isNot)
{
    WCDBGetObjectOrReturnValue(operand, WCDB::Expression, cppOperand, CPPExpression());

    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::In;
    newExpression->syntax().expressions.push_back(*cppOperand);
    newExpression->syntax().inSwitcher = WCDB::Expression::SyntaxType::SwitchIn::Table;
    newExpression->syntax().table() = table;
    newExpression->syntax().isNot = isNot;
    return ret;
}

CPPExpression
WCDBExpressionInTableOperate2(CPPCommonValue operand, const char* _Nullable table, bool isNot)
{
    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::In;
    newExpression->syntax().inSwitcher = WCDB::Expression::SyntaxType::SwitchIn::Table;
    newExpression->syntax().table() = table;
    newExpression->syntax().isNot = isNot;
    WCDBExpressionPushOperand(operand);
    return ret;
}

CPPExpression
WCDBExpressionInSelectionOperate(CPPExpression operand, CPPStatementSelect select, bool isNot)
{
    WCDBGetObjectOrReturnValue(operand, WCDB::Expression, cppOperand, CPPExpression());
    WCDBGetObjectOrReturnValue(select, WCDB::StatementSelect, cppSelect, CPPExpression());

    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::In;
    newExpression->syntax().expressions.push_back(*cppOperand);
    newExpression->syntax().inSwitcher = WCDB::Expression::SyntaxType::SwitchIn::Select;
    newExpression->syntax().select() = *cppSelect;
    newExpression->syntax().isNot = isNot;
    return ret;
}

CPPExpression
WCDBExpressionInSelectionOperate2(CPPCommonValue operand, CPPStatementSelect select, bool isNot)
{
    WCDBGetObjectOrReturnValue(select, WCDB::StatementSelect, cppSelect, CPPExpression());
    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::In;
    newExpression->syntax().inSwitcher = WCDB::Expression::SyntaxType::SwitchIn::Select;
    newExpression->syntax().select() = *cppSelect;
    newExpression->syntax().isNot = isNot;
    WCDBExpressionPushOperand(operand);
    return ret;
}

CPPExpression
WCDBExpressionInOperate(CPPCommonValue operand, CPPCommonArray valueArray, bool isNot)
{
    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::In;
    newExpression->syntax().isNot = isNot;
    newExpression->syntax().inSwitcher = WCDB::Expression::SyntaxType::SwitchIn::Expressions;
    WCDBExpressionPushOperand(operand);
    for (int i = 0; i < valueArray.length; i++) {
        switch (valueArray.type) {
        case WCDBBridgedType_Int:
            newExpression->syntax().expressions.push_back(WCDB::Expression(
            WCDB::LiteralValue(WCDBGetCommonArrayLiteralValue(int64_t, valueArray, i))));
            break;
        case WCDBBridgedType_UInt:
            newExpression->syntax().expressions.push_back(WCDB::Expression(WCDB::LiteralValue(
            WCDBGetCommonArrayLiteralValue(uint64_t, valueArray, i))));
            break;
        case WCDBBridgedType_Double:
            newExpression->syntax().expressions.push_back(WCDB::Expression(
            WCDB::LiteralValue(WCDBGetCommonArrayLiteralValue(double, valueArray, i))));
            break;
        case WCDBBridgedType_String:
            newExpression->syntax().expressions.push_back(
            WCDB::Expression(WCDB::LiteralValue(WCDB::StringView(
            WCDBGetCommonArrayLiteralValue(const char*, valueArray, i)))));
            break;
        case WCDBBridgedType_Column:
            newExpression->syntax().expressions.push_back(
            WCDB::Expression(WCDBGetCommonArrayObject(WCDB::Column, valueArray, i)));
            break;
        case WCDBBridgedType_LiteralValue:
            newExpression->syntax().expressions.push_back(WCDB::Expression(
            WCDBGetCommonArrayObject(WCDB::LiteralValue, valueArray, i)));
            break;
        case WCDBBridgedType_BindParameter:
            newExpression->syntax().expressions.push_back(WCDB::Expression(
            WCDBGetCommonArrayObject(WCDB::BindParameter, valueArray, i)));
            break;
        case WCDBBridgedType_Expression:
            newExpression->syntax().expressions.push_back(WCDB::Expression(
            WCDBGetCommonArrayObject(WCDB::Expression, valueArray, i)));
            break;
        default:
            assert(valueArray.type == WCDBBridgedType_Null);
            break;
        }
    }
    return ret;
}

CPPExpression WCDBExpressionInExpressionOperate(CPPExpression operand,
                                                const CPPExpression* _Nullable expressions,
                                                int num,
                                                bool isNot)
{
    WCDBGetObjectOrReturnValue(operand, WCDB::Expression, cppOperand, CPPExpression());

    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::In;
    newExpression->syntax().expressions.push_back(*cppOperand);
    newExpression->syntax().inSwitcher = WCDB::Expression::SyntaxType::SwitchIn::Expressions;
    newExpression->syntax().isNot = isNot;
    WCDBGetCPPSyntaxList(WCDB::Expression, cppExpressions, expressions, num);
    newExpression->syntax().expressions.splice(
    newExpression->syntax().expressions.end(), cppExpressions);
    return ret;
}

CPPExpression
WCDBExpressionInFunctionOperate(CPPExpression operand, const char* _Nullable func, bool isNot)
{
    WCDBGetObjectOrReturnValue(operand, WCDB::Expression, cppOperand, CPPExpression());

    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::In;
    newExpression->syntax().inSwitcher = WCDB::Expression::SyntaxType::SwitchIn::Function;
    newExpression->syntax().expressions.push_back(*cppOperand);
    newExpression->syntax().function() = func;
    newExpression->syntax().isNot = isNot;
    return ret;
}

CPPExpression
WCDBExpressionInFunctionOperate2(CPPCommonValue operand, const char* _Nullable func, bool isNot)
{
    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::In;
    newExpression->syntax().inSwitcher = WCDB::Expression::SyntaxType::SwitchIn::Function;
    newExpression->syntax().function() = func;
    newExpression->syntax().isNot = isNot;
    WCDBExpressionPushOperand(operand);
    return ret;
}

CPPExpression WCDBExpressionCollateOperate(CPPExpression operand, const char* _Nullable collation)
{
    WCDBGetObjectOrReturnValue(operand, WCDB::Expression, cppOperand, CPPExpression());

    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::Collate;
    newExpression->syntax().expressions.push_back(*cppOperand);
    newExpression->syntax().collation() = collation;
    return ret;
}

CPPExpression
WCDBExpressionCollateOperate2(CPPCommonValue operand, const char* _Nullable collation)
{
    CPPExpression ret = WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression);
    WCDBGetObjectOrReturnValue(ret, WCDB::Expression, newExpression, CPPExpression())

    newExpression->syntax()
    .switcher
    = WCDB::Expression::SyntaxType::Switch::Collate;
    newExpression->syntax().collation() = collation;
    WCDBExpressionPushOperand(operand);
    return ret;
}
