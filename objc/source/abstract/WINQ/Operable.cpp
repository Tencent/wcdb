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

#include <WCDB/WINQ.h>

namespace WCDB {

Expression FunctionOperable::abs(bool distinct) const
{
    return function("abs", distinct);
}

Expression FunctionOperable::hex(bool distinct) const
{
    return function("hex", distinct);
}

Expression FunctionOperable::length(bool distinct) const
{
    return function("length", distinct);
}

Expression FunctionOperable::lower(bool distinct) const
{
    return function("lower", distinct);
}

Expression FunctionOperable::upper(bool distinct) const
{
    return function("upper", distinct);
}

Expression FunctionOperable::total_change(bool distinct) const
{
    return function("total_change", distinct);
}

Expression FunctionOperable::avg(bool distinct) const
{
    return function("avg", distinct);
}

Expression FunctionOperable::count(bool distinct) const
{
    return function("count", distinct);
}

Expression FunctionOperable::group_concat(bool distinct) const
{
    return function("group_concat", distinct);
}

Expression FunctionOperable::group_concat(const Expression &separator,
                                          bool distinct) const
{
    return function("group_concat", distinct);
}

Expression FunctionOperable::max(bool distinct) const
{
    return function("max", distinct);
}

Expression FunctionOperable::min(bool distinct) const
{
    return function("min", distinct);
}

Expression FunctionOperable::total(bool distinct) const
{
    return function("total", distinct);
}

Expression FunctionOperable::sum(bool distinct) const
{
    return function("sum", distinct);
}

Expression FunctionOperable::offsets() const
{
    return function("offsets");
}

Expression FunctionOperable::snippet() const
{
    return function("snippet");
}

Expression FunctionOperable::matchinfo() const
{
    return function("matchinfo");
}

Lang::CopyOnWriteLazyLang<Lang::ExprUnaryOperation> Operable::unaryOperation(
    const Lang::ExprUnaryOperation::Operator &unaryOperator) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprUnaryOperation> cowLang;
    Lang::ExprUnaryOperation &lang = cowLang.get_or_copy();
    lang.expr.assign(getExpressionLang());
    lang.unaryOperator = unaryOperator;
    return cowLang;
}

Expression Operable::operator-() const
{
    return unaryOperation(Lang::ExprUnaryOperation::Operator::Negative);
}

Expression Operable::operator+() const
{
    return unaryOperation(Lang::ExprUnaryOperation::Operator::Positive);
}

Expression Operable::operator!() const
{
    return unaryOperation(Lang::ExprUnaryOperation::Operator::Not);
}

Expression Operable::operator~() const
{
    return unaryOperation(Lang::ExprUnaryOperation::Operator::Tilde);
}

Lang::CopyOnWriteLazyLang<Lang::ExprBinaryOperation> Operable::binaryOperation(
    const Lang::ExprBinaryOperation::Operator &binaryOperator,
    const Expression &operand) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprBinaryOperation> cowLang;
    Lang::ExprBinaryOperation &lang = cowLang.get_or_copy();
    lang.left.assign(getExpressionLang());
    lang.right.assign(operand.getCOWLang());
    lang.binaryOperator = binaryOperator;
    return cowLang;
}

Expression Operable::concat(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Concatenate,
                           operand);
}

Expression Operable::operator*(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Multiply,
                           operand);
}

Expression Operable::operator/(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Divide,
                           operand);
}

Expression Operable::operator%(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Modulo,
                           operand);
}

Expression Operable::operator+(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Plus, operand);
}

Expression Operable::operator-(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Minus, operand);
}

Expression Operable::operator<<(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::LeftShift,
                           operand);
}

Expression Operable::operator>>(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::RightShift,
                           operand);
}

Expression Operable::operator&(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::BitwiseAnd,
                           operand);
}

Expression Operable::operator|(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::BitwiseOr,
                           operand);
}

Expression Operable::operator<(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Less, operand);
}

Expression Operable::operator<=(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::LessOrEqual,
                           operand);
}

Expression Operable::operator>(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Greater,
                           operand);
}

Expression Operable::operator>=(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::GreaterOrEqual,
                           operand);
}

Expression Operable::operator==(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Equal, operand);
}

Expression Operable::operator!=(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::NotEqual,
                           operand);
}

Expression Operable::is(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Is, operand);
}

Expression Operable::isNot(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::IsNot, operand);
}

Expression Operable::operator&&(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::And, operand);
}

Expression Operable::operator||(const Expression &operand) const
{
    return binaryOperation(Lang::ExprBinaryOperation::Operator::Or, operand);
}

Expression Operable::like(const Expression &expr) const
{
    return pattern(Lang::ExprPattern::Type::Like, expr, false);
}

Expression Operable::glob(const Expression &expr) const
{
    return pattern(Lang::ExprPattern::Type::Glob, expr, false);
}

Expression Operable::regexp(const Expression &expr) const
{
    return pattern(Lang::ExprPattern::Type::Regexp, expr, false);
}

Expression Operable::match(const Expression &expr) const
{
    return pattern(Lang::ExprPattern::Type::Match, expr, false);
}

Expression Operable::notLike(const Expression &expr) const
{
    return pattern(Lang::ExprPattern::Type::Like, expr, true);
}

Expression Operable::notGlob(const Expression &expr) const
{
    return pattern(Lang::ExprPattern::Type::Glob, expr, true);
}

Expression Operable::notRegexp(const Expression &expr) const
{
    return pattern(Lang::ExprPattern::Type::Regexp, expr, true);
}

Expression Operable::notMatch(const Expression &expr) const
{
    return pattern(Lang::ExprPattern::Type::Match, expr, true);
}

Lang::CopyOnWriteLazyLang<Lang::ExprPattern>
Operable::pattern(const Lang::ExprPattern::Type &type,
                  const Expression &operand,
                  bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprPattern> cowLang;
    Lang::ExprPattern &lang = cowLang.get_or_copy();
    lang.isNot = isNot;
    lang.type = type;
    lang.left.assign(getExpressionLang());
    lang.right.assign(operand.getCOWLang());
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprNull> Operable::null(bool isNull) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprNull> cowLang;
    Lang::ExprNull &lang = cowLang.get_or_copy();
    lang.isNull = isNull;
    lang.expr.assign(getExpressionLang());
    return cowLang;
}

Expression Operable::isNull() const
{
    return null(true);
}

Expression Operable::notNull() const
{
    return null(false);
}

Lang::CopyOnWriteLazyLang<Lang::ExprBetween> Operable::between(
    const Expression &left, const Expression &right, bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprBetween> cowLang;
    Lang::ExprBetween &lang = cowLang.get_or_copy();
    lang.isNot = isNot;
    lang.expr.assign(getExpressionLang());
    lang.left.assign(left.getCOWLang());
    lang.right.assign(right.getCOWLang());
    return cowLang;
}

Expression Operable::between(const Expression &left,
                             const Expression &right) const
{
    return between(left, right, false);
}

Expression Operable::notBetween(const Expression &left,
                                const Expression &right) const
{
    return between(left, right, true);
}

Expression Operable::in() const
{
    return in(false);
}

Expression Operable::notIn() const
{
    return in(true);
}

Expression Operable::in(const StatementSelect &selectSTMT) const
{
    return in(selectSTMT, false);
}

Expression Operable::notIn(const StatementSelect &selectSTMT) const
{
    return in(selectSTMT, true);
}

Expression Operable::in(const Expression &expression) const
{
    return in(expression, false);
}

Expression Operable::notIn(const Expression &expression) const
{
    return in(expression, true);
}

Expression Operable::in(const std::list<Expression> &expressions) const
{
    return in(expressions, false);
}

Expression Operable::notIn(const std::list<Expression> &expressions) const
{
    return in(expressions, true);
}

Expression Operable::in(const std::string &tableName) const
{
    return in(tableName, false);
}

Expression Operable::notIn(const std::string &tableName) const
{
    return in(tableName, true);
}

Expression Operable::in(const std::string &schemaName,
                        const std::string &tableName) const
{
    return in(schemaName, tableName, false);
}

Expression Operable::notIn(const std::string &schemaName,
                           const std::string &tableName) const
{
    return in(schemaName, tableName, true);
}

Expression Operable::inFunction(const std::string &functionName) const
{
    return inFunction(functionName, false);
}

Expression Operable::notInFunction(const std::string &functionName) const
{
    return inFunction(functionName, true);
}

Expression Operable::inFunction(const std::string &schemaName,
                                const std::string &functionName) const
{
    return inFunction(schemaName, functionName, false);
}

Expression Operable::notInFunction(const std::string &schemaName,
                                   const std::string &functionName) const
{
    return inFunction(schemaName, functionName, true);
}

Expression Operable::inFunction(const std::string &functionName,
                                const Expression &expression) const
{
    return inFunction(functionName, expression, false);
}

Expression Operable::notInFunction(const std::string &functionName,
                                   const Expression &expression) const
{
    return inFunction(functionName, expression, true);
}

Expression Operable::inFunction(const std::string &schemaName,
                                const std::string &functionName,
                                const Expression &expression) const
{
    return inFunction(schemaName, functionName, expression, false);
}

Expression Operable::notInFunction(const std::string &schemaName,
                                   const std::string &functionName,
                                   const Expression &expression) const
{
    return inFunction(schemaName, functionName, expression, true);
}

Expression Operable::inFunction(const std::string &functionName,
                                const std::list<Expression> &expressions) const
{
    return inFunction(functionName, expressions, false);
}

Expression
Operable::notInFunction(const std::string &functionName,
                        const std::list<Expression> &expressions) const
{
    return inFunction(functionName, expressions, true);
}

Expression Operable::inFunction(const std::string &schemaName,
                                const std::string &functionName,
                                const std::list<Expression> &expressions) const
{
    return inFunction(schemaName, functionName, expressions, false);
}

Expression
Operable::notInFunction(const std::string &schemaName,
                        const std::string &functionName,
                        const std::list<Expression> &expressions) const
{
    return inFunction(schemaName, functionName, expressions, true);
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn> Operable::in(bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::NotSet;
    lang.expr.assign(getExpressionLang());
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::in(const StatementSelect &selectSTMT, bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Select;
    lang.expr.assign(getExpressionLang());
    lang.selectSTMT.assign(selectSTMT.getCOWLang());
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::in(const Expression &expression, bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Expr;
    lang.expr.assign(getExpressionLang());
    lang.exprs.append(expression.getCOWLang());
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::in(const std::list<Expression> &expressions, bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Expr;
    lang.expr.assign(getExpressionLang());
    for (const Expression &expression : expressions) {
        lang.exprs.append(expression.getCOWLang());
    }
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::in(const std::string &tableName, bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Table;
    lang.expr.assign(getExpressionLang());
    lang.tableNameOrFunction.assign(tableName);
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::in(const std::string &schemaName,
             const std::string &tableName,
             bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Table;
    lang.expr.assign(getExpressionLang());
    lang.schemaName.assign(schemaName);
    lang.tableNameOrFunction.assign(tableName);
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::inFunction(const std::string &functionName, bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Function;
    lang.expr.assign(getExpressionLang());
    lang.tableNameOrFunction.assign(functionName);
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::inFunction(const std::string &schemaName,
                     const std::string &functionName,
                     bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Function;
    lang.expr.assign(getExpressionLang());
    lang.schemaName.assign(schemaName);
    lang.tableNameOrFunction.assign(functionName);
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::inFunction(const std::string &functionName,
                     const Expression &parameter,
                     bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Function;
    lang.expr.assign(getExpressionLang());
    lang.tableNameOrFunction.assign(functionName);
    lang.exprs.append(parameter.getCOWLang());
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::inFunction(const std::string &schemaName,
                     const std::string &functionName,
                     const Expression &parameter,
                     bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Function;
    lang.expr.assign(getExpressionLang());
    lang.schemaName.assign(schemaName);
    lang.tableNameOrFunction.assign(functionName);
    lang.exprs.append(parameter.getCOWLang());
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::inFunction(const std::string &functionName,
                     const std::list<Expression> &parameters,
                     bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Function;
    lang.expr.assign(getExpressionLang());
    for (const Expression &expression : parameters) {
        lang.exprs.append(expression.getCOWLang());
    }
    lang.tableNameOrFunction.assign(functionName);
    lang.isNot = isNot;
    return cowLang;
}

Lang::CopyOnWriteLazyLang<Lang::ExprIn>
Operable::inFunction(const std::string &schemaName,
                     const std::string &functionName,
                     const std::list<Expression> &parameters,
                     bool isNot) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> cowLang;
    Lang::ExprIn &lang = cowLang.get_or_copy();
    lang.switcher = Lang::ExprIn::Switch::Function;
    lang.expr.assign(getExpressionLang());
    lang.schemaName.assign(schemaName);
    lang.tableNameOrFunction.assign(functionName);
    for (const Expression &expression : parameters) {
        lang.exprs.append(expression.getCOWLang());
    }
    lang.isNot = isNot;
    return cowLang;
}

Expression Operable::cast(const ColumnType &columnType) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprCast> cowLang;
    Lang::ExprCast &lang = cowLang.get_or_copy();
    lang.expr.assign(getExpressionLang());
    lang.type = columnType;
    return cowLang;
}

Expression Operable::collate(const std::string &collationName) const
{
    Lang::CopyOnWriteLazyLang<Lang::ExprCollate> cowLang;
    Lang::ExprCollate &lang = cowLang.get_or_copy();
    lang.collationName.assign(collationName);
    lang.expr.assign(getExpressionLang());
    return cowLang;
}

Expression Operable::function(const std::string &functionName,
                              bool distinct) const
{
    return Expression::Function(functionName, getExpressionLang(), distinct);
}

} // namespace WCDB
