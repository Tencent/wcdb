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

Expression Operable::operator !() const
{
    return Operator::operateWithPrefix("NOT ", asExpression());
}
Expression Operable::operator +() const
{
    return Operator::operateWithPrefix("", asExpression());
}
Expression Operable::operator -() const
{
    return Operator::operateWithPrefix("-", asExpression());
}
Expression Operable::operator ~() const
{
    return Operator::operateWithPrefix("~", asExpression());
}
Expression Operable::operator ||(const Expression& operand) const
{
    return Operator::operateWithOperator(asExpression(), "OR", operand);
}
Expression Operable::operator &&(const Expression& operand) const
{
    return Operator::operateWithOperator(asExpression(), "AND", operand);
}
Expression Operable::operator *(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "*", operand);
}
Expression Operable::operator /(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "/", operand);
}
Expression Operable::operator %(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "%", operand);
}
Expression Operable::operator +(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "+", operand);
}
Expression Operable::operator -(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "-", operand);
}
Expression Operable::operator <<(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "<<", operand);
}
Expression Operable::operator >>(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), ">>", operand);
}
Expression Operable::operator &(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "&", operand);
}
Expression Operable::operator |(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "|", operand);
}
Expression Operable::operator <(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "<", operand);
}
Expression Operable::operator <=(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "<=", operand);
}
Expression Operable::operator >(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), ">", operand);
}
Expression Operable::operator >=(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), ">=", operand);
}
Expression Operable::operator ==(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "=", operand);
}
Expression Operable::operator !=(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "!=", operand);
}
Expression Operable::concat(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "||", operand);
}
Expression Operable::substr(const Expression& start, const Expression& length) const {
    return Operator::operateWithRaw("SUBSTR(" + Expression(asExpression()).getDescription() + ", " + start.getDescription() + ", " + length.getDescription() + ")");
}
Expression Operable::like(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "LIKE", operand);
}
Expression Operable::glob(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "GLOB", operand);
}
Expression Operable::match(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "MATCH", operand);
}
Expression Operable::regexp(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "REGEXP", operand);
}
Expression Operable::notLike(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "NOT LIKE", operand);
}
Expression Operable::notGlob(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "NOT GLOB", operand);
}
Expression Operable::notMatch(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "NOT MATCH", operand);
}
Expression Operable::notRegexp(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "NOT REGEXP", operand);
}
Expression Operable::like(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "LIKE", operand, "ESCAPE", escape);
}
Expression Operable::glob(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "GLOB", operand, "ESCAPE", escape);
}
Expression Operable::match(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "MATCH", operand, "ESCAPE", escape);
}
Expression Operable::regexp(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "REGEXP", operand, "ESCAPE", escape);
}
Expression Operable::notLike(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "NOT LIKE", operand, "ESCAPE", escape);
}
Expression Operable::notGlob(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "NOT GLOB", operand, "ESCAPE", escape);
}
Expression Operable::notMatch(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "NOT MATCH", operand, "ESCAPE", escape);
}
Expression Operable::notRegexp(const Expression& operand, const Expression& escape) const {
    return Operator::operateWithComplexOperator(asExpression(), "NOT REGEXP", operand, "ESCAPE", escape);
}
Expression Operable::isNull() const {
    return Operator::operateWithPostfix(asExpression(), "ISNULL");
}
Expression Operable::isNotNull() const {
    return Operator::operateWithPostfix(asExpression(), "NOTNULL");
}
Expression Operable::is(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "IS", operand);
}
Expression Operable::isNot(const Expression& operand) const {
    return Operator::operateWithOperator(asExpression(), "IS NOT", operand);
}
Expression Operable::between(const Expression& begin, const Expression& end) const {
    return Operator::operateWithComplexOperator(asExpression(), "BETWEEN", begin, "AND", end);
}
Expression Operable::notBetween(const Expression& begin, const Expression& end) const {
    return Operator::operateWithComplexOperator(asExpression(), "NOT BETWEEN", begin, "AND", end);
}
Expression Operable::in(const StatementSelect& statementSelect) const {
    return Operator::operateWithPrefix("IN ", statementSelect);
}
Expression Operable::notIn(const StatementSelect&  statementSelect) const {
    return Operator::operateWithPrefix("NOT IN ", statementSelect);
}
    
Expression Operable::in(const std::list<const Expression> &list) const
{
    return Operator::operateWithPostfix(asExpression(), "IN(" + stringByJoiningList(list) + ")");
}
    
Expression Operable::notIn(const std::list<const Expression> &list) const
{
    return Operator::operateWithPostfix(asExpression(), "NOT IN(" + stringByJoiningList(list) + ")");
}
    
Expression Operable::in(const Expression &expression) const
{
    return Operator::operateWithPostfix(asExpression(), "IN(" + expression.getDescription() + ")");
}
    
Expression Operable::notIn(const Expression &expression) const
{
    return Operator::operateWithPostfix(asExpression(), "NOT IN(" + expression.getDescription() + ")");
}

Expression Operable::avg(bool isDistinct) const {
    return Expression::Function("AVG", asExpression(), isDistinct);;
}
Expression Operable::count(bool isDistinct) const {
    return Expression::Function("COUNT", asExpression(), isDistinct);
}
Expression Operable::groupConcat(bool isDistinct) const {
    return Expression::Function("GROUP_CONCAT", asExpression(), isDistinct);
}
Expression Operable::groupConcat(bool isDistinct, const std::string& seperator) const {
    return Expression::Function("GROUP_CONCAT", {asExpression(), seperator}, isDistinct);
}
Expression Operable::max(bool isDistinct) const {
    return Expression::Function("MAX", asExpression(), isDistinct);
}
Expression Operable::min(bool isDistinct) const {
    return Expression::Function("MIN", asExpression(), isDistinct);
}
Expression Operable::sum(bool isDistinct) const {
    return Expression::Function("SUM", asExpression(), isDistinct);
}
Expression Operable::total(bool isDistinct) const {
    return Expression::Function("TOTAL", asExpression(), isDistinct);
}
Expression Operable::abs(bool isDistinct) const {
    return Expression::Function("ABS", asExpression(), isDistinct);
}
Expression Operable::hex(bool isDistinct) const {
    return Expression::Function("HEX", asExpression(), isDistinct);
}
Expression Operable::length(bool isDistinct) const {
    return Expression::Function("LENGTH", asExpression(), isDistinct);
}
Expression Operable::lower(bool isDistinct) const {
    return Expression::Function("LOWER", asExpression(), isDistinct);
}
Expression Operable::upper(bool isDistinct) const {
    return Expression::Function("UPPER", asExpression(), isDistinct);
}
Expression Operable::round(bool isDistinct) const {
    return Expression::Function("ROUND", asExpression(), isDistinct);
}
Expression Operable::matchinfo() const {
    return Expression::Function("MATCHINFO", asExpression());
}
Expression Operable::offsets() const {
    return Expression::Function("OFFSETS", asExpression());
}
Expression Operable::snippet() const {
    return Expression::Function("SNIPPET", asExpression());
}
    
} //namespace WCDB
