//
// Created by sanhuazhang on 2019/05/02
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

#include "WINQ.h"

namespace WCDB {

Expression::Expression() = default;

Expression::~Expression() = default;

Expression::Expression(const LiteralValue& literalValue)
{
    syntax().switcher = SyntaxType::Switch::LiteralValue;
    syntax().literalValue() = literalValue;
}

Expression::Expression(const BindParameter& bindParameter)
{
    syntax().switcher = SyntaxType::Switch::BindParameter;
    syntax().bindParameter() = bindParameter;
}

Expression::Expression(const Column& column)
{
    syntax().switcher = SyntaxType::Switch::Column;
    syntax().column() = column;
}

Expression::Expression(const StatementSelect& select)
{
    syntax().switcher = SyntaxType::Switch::Select;
    syntax().select() = select;
}

Expression& Expression::schema(const Schema& schema)
{
    syntax().schema() = schema;
    return *this;
}

Expression::Expression(const Expressions& expressions)
{
    syntax().switcher = SyntaxType::Switch::Expressions;
    syntax().expressions = expressions;
}

Expression::Expression(const RaiseFunction& raiseFunction)
{
    syntax().switcher = SyntaxType::Switch::RaiseFunction;
    syntax().raiseFunction() = raiseFunction;
}

Expression Expression::function(const UnsafeStringView& function)
{
    Expression expression;
    expression.syntax().switcher = SyntaxType::Switch::Function;
    expression.syntax().function() = function;
    expression.syntax().useWildcard = false;
    return expression;
}

Expression& Expression::invoke()
{
    syntax().useWildcard = false;
    return *this;
}

Expression& Expression::argument(const Expression& parameter)
{
    syntax().expressions.push_back(parameter);
    syntax().useWildcard = false;
    return *this;
}

Expression& Expression::arguments(const Expressions& parameters)
{
    syntax().expressions.insert(
    syntax().expressions.end(), parameters.begin(), parameters.end());
    syntax().useWildcard = false;
    return *this;
}

Expression& Expression::invokeAll()
{
    syntax().useWildcard = true;
    return *this;
}

Expression& Expression::distinct()
{
    syntax().distinct = true;
    return *this;
}

Expression Expression::cast(const Expression& expression)
{
    Expression result;
    result.syntax().switcher = SyntaxType::Switch::Cast;
    result.syntax().expressions.push_back(expression);
    return result;
}

Expression& Expression::as(const ColumnType& columnType)
{
    syntax().castType = columnType;
    return *this;
}

ResultColumn Expression::as(const UnsafeStringView& alias)
{
    return ResultColumn(*this).as(alias);
}

Expression Expression::exists(const StatementSelect& select)
{
    Expression expression;
    expression.syntax().switcher = SyntaxType::Switch::Exists;
    expression.syntax().isNot = false;
    expression.syntax().select() = select;
    return expression;
}

Expression Expression::notExists(const StatementSelect& select)
{
    Expression expression;
    expression.syntax().switcher = SyntaxType::Switch::Exists;
    expression.syntax().isNot = true;
    expression.syntax().select() = select;
    return expression;
}

Expression Expression::case_(const Expression& expression)
{
    Expression result;
    result.syntax().switcher = SyntaxType::Switch::Case;
    result.syntax().hasCase = true;
    result.syntax().expressions.push_back(expression);
    return result;
}

Expression Expression::case_()
{
    Expression result;
    result.syntax().hasCase = false;
    result.syntax().switcher = SyntaxType::Switch::Case;
    return result;
}

Expression& Expression::when(const Expression& expression)
{
    syntax().expressions.push_back(expression);
    return *this;
}

Expression& Expression::then(const Expression& expression)
{
    syntax().expressions.push_back(expression);
    return *this;
}

Expression& Expression::else_(const Expression& expression)
{
    syntax().hasElse = true;
    syntax().expressions.push_back(expression);
    return *this;
}

Expression& Expression::escape(const Expression& operand)
{
    syntax().escape = true;
    syntax().expressions.push_back(operand);
    return *this;
}

Expression Expression::windowFunction(const UnsafeStringView& function)
{
    Expression expression;
    expression.syntax().switcher = SyntaxType::Switch::Window;
    expression.syntax().function() = function;
    return expression;
}

Expression& Expression::filter(const Expression& condition)
{
    syntax().filter() = Filter().where(condition);
    return *this;
}

Expression& Expression::over(const WindowDef& windowDef)
{
    syntax().windowDef() = windowDef;
    return *this;
}

Expression& Expression::over(const UnsafeStringView& window)
{
    syntax().windowName() = window;
    return *this;
}

Expression Expression::asExpressionOperand() const
{
    return *this;
}

} // namespace WCDB
