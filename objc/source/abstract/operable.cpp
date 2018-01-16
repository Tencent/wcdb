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

#include <WCDB/operable.hpp>
#include <WCDB/literal_value.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/convertible_impl.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {
    
Expression Operator::operateWithRaw(const std::string &raw)
{
    return Expression(raw, nullptr);
}

Expression Operator::operateWithPrefix(const std::string &prefix, const Expression& operand)
{
    return Expression("(" + prefix + operand.getDescription() + ")", nullptr);
}
    
Expression Operator::operateWithTitle(const std::string &title, const std::string& infix, const std::list<const Expression> &list) {
    return Expression(title + "(" + (infix.empty()?"":infix+" ") + stringByJoiningList(list) + ")", nullptr);
}

Expression Operator::operateWithPostfix(const Expression& operand, const std::string &postfix)
{
    return Expression("(" + operand.getDescription() + " " + postfix + ")", nullptr);
}

Expression Operator::operateWithOperator(const Expression& left, const std::string &op, const Expression& right)
{
    return Expression("(" + left.getDescription() + " " + op + " " + right.getDescription() + ")", nullptr);
}

Expression Operator::operateWithComplexOperator(const Expression& one, const std::string &op1, const Expression& two, const std::string& op2, const Expression &three)
{
    return Expression("(" + one.getDescription() + " " + op1 + " " + two.getDescription() + " " + op2 + " " + three.getDescription() + ")", nullptr);
}

template <>
Expression Operable<Expression>::operator !() const
{
    return Operator::operateWithPrefix("NOT ", asExpression());
}
template <>
Expression Operable<Expression>::operator +() const
{
    return Operator::operateWithPrefix("", asExpression());
}
template <>
Expression Operable<Expression>::operator -() const
{
    return Operator::operateWithPrefix("-", asExpression());
}
template <>
Expression Operable<Expression>::operator ~() const
{
    return Operator::operateWithPrefix("~", asExpression());
}
template <>
Expression Operable<Expression>::operator ||(const Expression& operand) const
{
    return Operator::operateWithOperator(asExpression(), "OR", operand);
}
template <>
Expression Operable<Expression>::operator &&(const Expression& operand) const
{
    return Operator::operateWithOperator(asExpression(), "AND", operand);
}
template <>
Expression Operable<Expression>::operator *(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "*", operand);
}
template <>
Expression Operable<Expression>::operator /(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "/", operand);
}
template <>
Expression Operable<Expression>::operator %(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "%", operand);
}
template <>
Expression Operable<Expression>::operator +(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "+", operand);
}
template <>
Expression Operable<Expression>::operator -(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "-", operand);
}
template <>
Expression Operable<Expression>::operator <<(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "<<", operand);
}
template <>
Expression Operable<Expression>::operator >>(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), ">>", operand);
}
template <>
Expression Operable<Expression>::operator &(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "&", operand);
}
template <>
Expression Operable<Expression>::operator |(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "|", operand);
}
template <>
Expression Operable<Expression>::operator <(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "<", operand);
}
template <>
Expression Operable<Expression>::operator <=(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "<=", operand);
}
template <>
Expression Operable<Expression>::operator >(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), ">", operand);
}
template <>
Expression Operable<Expression>::operator >=(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), ">=", operand);
}
template <>
Expression Operable<Expression>::operator ==(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "=", operand);
}
template <>
Expression Operable<Expression>::operator !=(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "!=", operand);
}
template <>
Expression Operable<Expression>::concat(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "||", operand);
}
template <>
Expression Operable<Expression>::substr(const Expression& start, const Expression& length) const {
    return Operator::operateWithRaw("SUBSTR(" + Expression(asExpression()).getDescription() + ", " + start.getDescription() + ", " + length.getDescription() + ")");
}
template <>
Expression Operable<Expression>::like(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "LIKE", operand);
}
template <>
Expression Operable<Expression>::glob(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "GLOB", operand);
}
template <>
Expression Operable<Expression>::match(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "MATCH", operand);
}
template <>
Expression Operable<Expression>::regexp(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "REGEXP", operand);
}
template <>
Expression Operable<Expression>::notLike(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "NOT LIKE", operand);
}
template <>
Expression Operable<Expression>::notGlob(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "NOT GLOB", operand);
}
template <>
Expression Operable<Expression>::notMatch(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "NOT MATCH", operand);
}
template <>
Expression Operable<Expression>::notRegexp(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "NOT REGEXP", operand);
}
template <>
Expression Operable<Expression>::like(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "LIKE", operand, "ESCAPE", escape);
}
template <>
Expression Operable<Expression>::glob(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "GLOB", operand, "ESCAPE", escape);
}
template <>
Expression Operable<Expression>::match(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "MATCH", operand, "ESCAPE", escape);
}
template <>
Expression Operable<Expression>::regexp(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "REGEXP", operand, "ESCAPE", escape);
}
template <>
Expression Operable<Expression>::notLike(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "NOT LIKE", operand, "ESCAPE", escape);
}
template <>
Expression Operable<Expression>::notGlob(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "NOT GLOB", operand, "ESCAPE", escape);
}
template <>
Expression Operable<Expression>::notMatch(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "NOT MATCH", operand, "ESCAPE", escape);
}
template <>
Expression Operable<Expression>::notRegexp(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "NOT REGEXP", operand, "ESCAPE", escape);
}
template <>
Expression Operable<Expression>::isNull() const {
    return Operator::operateWithPostfix(asExpression(), "ISNULL");
}
template <>
Expression Operable<Expression>::isNotNull() const {
    return Operator::operateWithPostfix(asExpression(), "NOTNULL");
}
template <>
Expression Operable<Expression>::is(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "IS", operand);
}
template <>
Expression Operable<Expression>::isNot(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "IS NOT", operand);
}
template <>
Expression Operable<Expression>::between(const Expression& begin, const Expression& end) const {
    return Operator::operateWithComplexOperator(asExpression(), "BETWEEN", begin, "AND", end);
}
template <>
Expression Operable<Expression>::notBetween(const Expression& begin, const Expression& end) const {
    return Operator::operateWithComplexOperator(asExpression(), "NOT BETWEEN", begin, "AND", end);
}
template <>
Expression Operable<Expression>::in(const StatementSelect& statementSelect) const {
    return Operator::operateWithPrefix("IN ", statementSelect);
}
template <>
Expression Operable<Expression>::notIn(const StatementSelect&  statementSelect) const {
    return Operator::operateWithPrefix("NOT IN ", statementSelect);
}
    
template <>
Expression Operable<Expression>::in(const std::list<const Expression> &list) const
{
    return Operator::operateWithPostfix(asExpression(), "IN(" + stringByJoiningList(list) + ")");
}
    
template <>
Expression Operable<Expression>::notIn(const std::list<const Expression> &list) const
{
    return Operator::operateWithPostfix(asExpression(), "NOT IN(" + stringByJoiningList(list) + ")");
}
    
template <>
Expression Operable<Expression>::in(const Expression &expression) const
{
    return Operator::operateWithPostfix(asExpression(), "IN(" + expression.getDescription() + ")");
}
    
template <>
Expression Operable<Expression>::notIn(const Expression &expression) const
{
    return Operator::operateWithPostfix(asExpression(), "NOT IN(" + expression.getDescription() + ")");
}

template <>
Expression Operable<Expression>::avg(bool isDistinct) const {
    return Expression::Function("AVG", asExpression(), isDistinct);;
}
template <>
Expression Operable<Expression>::count(bool isDistinct) const {
    return Expression::Function("COUNT", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::groupConcat(bool isDistinct) const {
    return Expression::Function("GROUP_CONCAT", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::groupConcat(bool isDistinct, const std::string& seperator) const {
    return Expression::Function("GROUP_CONCAT", {asExpression(), seperator}, isDistinct);
}
template <>
Expression Operable<Expression>::max(bool isDistinct) const {
    return Expression::Function("MAX", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::min(bool isDistinct) const {
    return Expression::Function("MIN", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::sum(bool isDistinct) const {
    return Expression::Function("SUM", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::total(bool isDistinct) const {
    return Expression::Function("TOTAL", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::abs(bool isDistinct) const {
    return Expression::Function("ABS", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::hex(bool isDistinct) const {
    return Expression::Function("HEX", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::length(bool isDistinct) const {
    return Expression::Function("LENGTH", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::lower(bool isDistinct) const {
    return Expression::Function("LOWER", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::upper(bool isDistinct) const {
    return Expression::Function("UPPER", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::round(bool isDistinct) const {
    return Expression::Function("ROUND", asExpression(), isDistinct);
}
template <>
Expression Operable<Expression>::matchinfo() const {
    return Expression::Function("MATCHINFO", asExpression());
}
template <>
Expression Operable<Expression>::offsets() const {
    return Expression::Function("OFFSETS", asExpression());
}
template <>
Expression Operable<Expression>::snippet() const {
    return Expression::Function("SNIPPET", asExpression());
}
    
} //namespace WCDB
