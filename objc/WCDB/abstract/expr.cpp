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
#include <WCDB/expr.hpp>
#include <WCDB/statement_select.hpp>

namespace WCDB {

const Expr Expr::BindParameter = Expr(Column("?"));

Expr::Expr(const LiteralValue &value) : Describable(value)
{
}

Expr::Expr(const char *value) : Describable(LiteralValue(value))
{
}

Expr::Expr(const std::string &value) : Describable(LiteralValue(value))
{
}

Expr::Expr(const std::nullptr_t &value) : Describable(LiteralValue(value))
{
}

Expr::Expr(const typename ColumnTypeInfo<ColumnType::BLOB>::CType &value,
           int size)
    : WCDB::Describable(LiteralValue(value, size))
{
}

Expr::Expr() : Describable("")
{
}

Expr::Expr(const Column &column) : Describable(column.getName())
{
}

Expr::operator ExprList() const
{
    return {*this};
}

Expr Expr::operator!() const
{
    Expr expr;
    expr.m_description.append("NOT (" + m_description + ")");
    return expr;
}

Expr Expr::operator+() const
{
    Expr expr;
    expr.m_description.append("(" + m_description + ")");
    return expr;
}

Expr Expr::operator-() const
{
    Expr expr;
    expr.m_description.append("-(" + m_description + ")");
    return expr;
}

Expr Expr::operator~() const
{
    Expr expr;
    expr.m_description.append("~(" + m_description + ")");
    return expr;
}

Expr Expr::operator||(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " OR " +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator&&(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " AND " +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator*(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "*" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator/(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "/" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator%(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "%" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator+(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "+" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator-(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "-" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator<<(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "<<" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator>>(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + ">>" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator&(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "&" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator|(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "|" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator<(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "<" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator<=(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description +
                              "<=" + operand.m_description + ")");
    return expr;
}

Expr Expr::operator>(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + ">" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator>=(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description +
                              ">=" + operand.m_description + ")");
    return expr;
}

Expr Expr::operator==(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "=" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::operator!=(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description +
                              "!=" + operand.m_description + ")");
    return expr;
}

Expr Expr::concat(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + "||" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::substr(const Expr &start, const Expr &length) const
{
    Expr expr;
    expr.m_description.append("SUBSTR(" + m_description + "," +
                              start.m_description + "," + length.m_description +
                              ")");
    return expr;
}

Expr Expr::like(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " LIKE " +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::glob(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " GLOB " +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::match(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " MATCH " +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::regexp(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " REGEXP" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::notLike(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOT LIKE " +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::notGlob(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOT GLOB " +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::notMatch(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOT MATCH " +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::notRegexp(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOT REGEXP" +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::like(const Expr &operand, const Expr &escape) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " LIKE" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expr Expr::glob(const Expr &operand, const Expr &escape) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " GLOB" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expr Expr::match(const Expr &operand, const Expr &escape) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " MATCH" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expr Expr::regexp(const Expr &operand, const Expr &escape) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " REGEXP" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expr Expr::notLike(const Expr &operand, const Expr &escape) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOT LIKE" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expr Expr::notGlob(const Expr &operand, const Expr &escape) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOT GLOB" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expr Expr::notMatch(const Expr &operand, const Expr &escape) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOT MATCH" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expr Expr::notRegexp(const Expr &operand, const Expr &escape) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOT REGEXP" +
                              operand.m_description + " ESCAPE " +
                              escape.getDescription() + ")");
    return expr;
}

Expr Expr::isNull() const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " ISNULL" + ")");
    return expr;
}

Expr Expr::isNotNull() const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOTNULL" + ")");
    return expr;
}

Expr Expr::is(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " IS " +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::isNot(const Expr &operand) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " IS NOT " +
                              operand.m_description + ")");
    return expr;
}

Expr Expr::between(const Expr &left, const Expr &right) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " BETWEEN " +
                              left.m_description + " AND " +
                              right.m_description + ")");
    return expr;
}

Expr Expr::notBetween(const Expr &left, const Expr &right) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOT BETWEEN " +
                              left.m_description + " AND " +
                              right.m_description + ")");
    return expr;
}

Expr::Expr(const StatementSelect &selectStatement)
    : Describable(selectStatement)
{
}

Expr Expr::Exists(const StatementSelect &selectStatement)
{
    Expr expr;
    expr.m_description.append("EXISTS(" + selectStatement.getDescription() +
                              ")");
    return expr;
}

Expr Expr::NotExists(const StatementSelect &selectStatement)
{
    Expr expr;
    expr.m_description.append("NOT EXISTS(" + selectStatement.getDescription() +
                              ")");
    return expr;
}

Expr Expr::in(const std::string &table) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " IN " + table + ")");
    return expr;
}

Expr Expr::notIn(const std::string &table) const
{
    Expr expr;
    expr.m_description.append("(" + m_description + " NOT IN " + table + ")");
    return expr;
}

Expr Expr::avg(bool distinct) const
{
    return Expr::Function("AVG", {*this}, distinct);
}

Expr Expr::count(bool distinct) const
{
    return Expr::Function("COUNT", {*this}, distinct);
}

Expr Expr::groupConcat(bool distinct) const
{
    return Expr::Function("GROUP_CONCAT", {*this}, distinct);
}

Expr Expr::groupConcat(const std::string &seperator, bool distinct) const
{
    return Expr::Function("GROUP_CONCAT", ExprList({*this, seperator}),
                          distinct);
}

Expr Expr::max(bool distinct) const
{
    return Expr::Function("MAX", {*this}, distinct);
}

Expr Expr::min(bool distinct) const
{
    return Expr::Function("MIN", {*this}, distinct);
}

Expr Expr::sum(bool distinct) const
{
    return Expr::Function("SUM", {*this}, distinct);
}

Expr Expr::total(bool distinct) const
{
    return Expr::Function("TOTAL", {*this}, distinct);
}

Expr Expr::abs(bool distinct) const
{
    return Expr::Function("ABS", {*this}, distinct);
}

Expr Expr::hex(bool distinct) const
{
    return Expr::Function("HEX", {*this}, distinct);
}

Expr Expr::length(bool distinct) const
{
    return Expr::Function("LENGTH", {*this}, distinct);
}

Expr Expr::lower(bool distinct) const
{
    return Expr::Function("LOWER", {*this}, distinct);
}

Expr Expr::upper(bool distinct) const
{
    return Expr::Function("UPPER", {*this}, distinct);
}

Expr Expr::round(bool distinct) const
{
    return Expr::Function("ROUND", {*this}, distinct);
}

Expr Expr::matchinfo() const
{
    return Expr::Function("MATCHINFO", {*this});
}

Expr Expr::offsets() const
{
    return Expr::Function("OFFSETS", {*this});
}

Expr Expr::snippet() const
{
    return Expr::Function("SNIPPET", {*this});
}

} //namespace WCDB
