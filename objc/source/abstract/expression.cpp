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

#include <WCDB/column.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/statement_select.hpp>

namespace WCDB {

const Expression Expression::BindParameter = Expression(Column("?"));

Expression::Expression(const LiteralValue &value) : Describable(value)
{
}

Expression::Expression(const char *value) : Describable(LiteralValue(value))
{
}

Expression::Expression(const std::string &value)
    : Describable(LiteralValue(value))
{
}

Expression::Expression(const std::nullptr_t &value)
    : Describable(LiteralValue(value))
{
}

Expression::Expression(
    const typename ColumnTypeInfo<ColumnType::BLOB>::CType &value, int size)
    : WCDB::Describable(LiteralValue(value, size))
{
}

Expression::Expression() : Describable("")
{
}

Expression::Expression(const Column &column) : Describable(column.getName())
{
}

Expression::operator ExprList() const
{
    return {*this};
}

Expression Expression::operator!() const
{
    Expression expr;
    expr.m_description.append("NOT (" + m_description + ")");
    return expr;
}

Expression Expression::operator+() const
{
    Expression expr;
    expr.m_description.append("(" + m_description + ")");
    return expr;
}

Expression Expression::operator-() const
{
    Expression expr;
    expr.m_description.append("-(" + m_description + ")");
    return expr;
}

Expression Expression::operator~() const
{
    Expression expr;
    expr.m_description.append("~(" + m_description + ")");
    return expr;
}

Expression Expression::operator||(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " OR " +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator&&(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " AND " +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator*(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "*" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator/(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "/" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator%(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "%" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator+(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "+" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator-(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "-" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator<<(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "<<" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator>>(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + ">>" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator&(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "&" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator|(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "|" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator<(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "<" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator<=(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description +
                              "<=" + operand.m_description + ")");
    return expr;
}

Expression Expression::operator>(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + ">" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator>=(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description +
                              ">=" + operand.m_description + ")");
    return expr;
}

Expression Expression::operator==(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "=" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::operator!=(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description +
                              "!=" + operand.m_description + ")");
    return expr;
}

Expression Expression::concat(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + "||" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::substr(const Expression &start,
                              const Expression &length) const
{
    Expression expr;
    expr.m_description.append("SUBSTR(" + m_description + "," +
                              start.m_description + "," + length.m_description +
                              ")");
    return expr;
}

Expression Expression::like(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " LIKE " +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::glob(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " GLOB " +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::match(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " MATCH " +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::regexp(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " REGEXP" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::notLike(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOT LIKE " +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::notGlob(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOT GLOB " +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::notMatch(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOT MATCH " +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::notRegexp(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOT REGEXP" +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::like(const Expression &operand,
                            const Expression &escape) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " LIKE" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expression Expression::glob(const Expression &operand,
                            const Expression &escape) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " GLOB" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expression Expression::match(const Expression &operand,
                             const Expression &escape) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " MATCH" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expression Expression::regexp(const Expression &operand,
                              const Expression &escape) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " REGEXP" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expression Expression::notLike(const Expression &operand,
                               const Expression &escape) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOT LIKE" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expression Expression::notGlob(const Expression &operand,
                               const Expression &escape) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOT GLOB" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expression Expression::notMatch(const Expression &operand,
                                const Expression &escape) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOT MATCH" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expression Expression::notRegexp(const Expression &operand,
                                 const Expression &escape) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOT REGEXP" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expression Expression::isNull() const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " ISNULL" + ")");
    return expr;
}

Expression Expression::isNotNull() const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOTNULL" + ")");
    return expr;
}

Expression Expression::is(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " IS " +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::isNot(const Expression &operand) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " IS NOT " +
                              operand.m_description + ")");
    return expr;
}

Expression Expression::between(const Expression &left,
                               const Expression &right) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " BETWEEN " +
                              left.m_description + " AND " +
                              right.m_description + ")");
    return expr;
}

Expression Expression::notBetween(const Expression &left,
                                  const Expression &right) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOT BETWEEN " +
                              left.m_description + " AND " +
                              right.m_description + ")");
    return expr;
}

Expression::Expression(const StatementSelect &selectStatement)
    : Describable(selectStatement)
{
}

Expression Expression::Exists(const StatementSelect &selectStatement)
{
    Expression expr;
    expr.m_description.append("EXISTS(" + selectStatement.getDescription() +
                              ")");
    return expr;
}

Expression Expression::NotExists(const StatementSelect &selectStatement)
{
    Expression expr;
    expr.m_description.append("NOT EXISTS(" + selectStatement.getDescription() +
                              ")");
    return expr;
}

Expression Expression::in(const std::string &table) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " IN " + table + ")");
    return expr;
}

Expression Expression::notIn(const std::string &table) const
{
    Expression expr;
    expr.m_description.append("(" + m_description + " NOT IN " + table + ")");
    return expr;
}

Expression Expression::avg(bool distinct) const
{
    return Expression::Function("AVG", {*this}, distinct);
}

Expression Expression::count(bool distinct) const
{
    return Expression::Function("COUNT", {*this}, distinct);
}

Expression Expression::groupConcat(bool distinct) const
{
    return Expression::Function("GROUP_CONCAT", {*this}, distinct);
}

Expression Expression::groupConcat(const std::string &seperator,
                                   bool distinct) const
{
    return Expression::Function("GROUP_CONCAT", ExprList({*this, seperator}),
                                distinct);
}

Expression Expression::max(bool distinct) const
{
    return Expression::Function("MAX", {*this}, distinct);
}

Expression Expression::min(bool distinct) const
{
    return Expression::Function("MIN", {*this}, distinct);
}

Expression Expression::sum(bool distinct) const
{
    return Expression::Function("SUM", {*this}, distinct);
}

Expression Expression::total(bool distinct) const
{
    return Expression::Function("TOTAL", {*this}, distinct);
}

Expression Expression::abs(bool distinct) const
{
    return Expression::Function("ABS", {*this}, distinct);
}

Expression Expression::hex(bool distinct) const
{
    return Expression::Function("HEX", {*this}, distinct);
}

Expression Expression::length(bool distinct) const
{
    return Expression::Function("LENGTH", {*this}, distinct);
}

Expression Expression::lower(bool distinct) const
{
    return Expression::Function("LOWER", {*this}, distinct);
}

Expression Expression::upper(bool distinct) const
{
    return Expression::Function("UPPER", {*this}, distinct);
}

Expression Expression::round(bool distinct) const
{
    return Expression::Function("ROUND", {*this}, distinct);
}

Expression Expression::matchinfo() const
{
    return Expression::Function("MATCHINFO", {*this});
}

Expression Expression::offsets() const
{
    return Expression::Function("OFFSETS", {*this});
}

Expression Expression::snippet() const
{
    return Expression::Function("SNIPPET", {*this});
}

} //namespace WCDB
