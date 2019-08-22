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

#ifndef expr_hpp
#define expr_hpp

#include <WCDB/column_type.hpp>
#include <WCDB/declare.hpp>
#include <WCDB/describable.hpp>
#include <WCDB/literal_value.hpp>

namespace WCDB {

class Expr : public Describable {
public:
    static const Expr BindParameter;

    Expr();
    Expr(const Column &column);

    template <typename T>
    Expr(const T &value,
         typename std::enable_if<std::is_arithmetic<T>::value ||
                                 std::is_enum<T>::value>::type * = nullptr)
        : Describable(LiteralValue(value))
    {
    }

    Expr(const char *value);

    Expr(const std::string &value);

    Expr(const std::nullptr_t &value);

    Expr(const typename ColumnTypeInfo<ColumnType::BLOB>::CType &value,
         int size);

    operator ExprList() const;

    //unary
    Expr operator!() const;
    Expr operator+() const;
    Expr operator-() const;
    Expr operator~() const;

    //binary
    Expr operator||(const Expr &operand) const; //or, not concat
    Expr operator&&(const Expr &operand) const;
    Expr operator*(const Expr &operand) const;
    Expr operator/(const Expr &operand) const;
    Expr operator%(const Expr &operand) const;
    Expr operator+(const Expr &operand) const;
    Expr operator-(const Expr &operand) const;
    Expr operator<<(const Expr &operand) const;
    Expr operator>>(const Expr &operand) const;
    Expr operator&(const Expr &operand) const;
    Expr operator|(const Expr &operand) const;
    Expr operator<(const Expr &operand) const;
    Expr operator<=(const Expr &operand) const;
    Expr operator>(const Expr &operand) const;
    Expr operator>=(const Expr &operand) const;
    Expr operator==(const Expr &operand) const;
    Expr operator!=(const Expr &operand) const;

    Expr concat(const Expr &operand) const;
    Expr substr(const Expr &start, const Expr &length) const;

    template <typename T = Expr>
    typename std::enable_if<std::is_base_of<Expr, T>::value, Expr>::type
    in(const std::list<const T> &exprList) const
    {
        Expr expr;
        expr.m_description.append(m_description + " IN(");
        expr.joinDescribableList(exprList);
        expr.m_description.append(")");
        return expr;
    }

    template <typename T = Expr>
    typename std::enable_if<std::is_base_of<Expr, T>::value, Expr>::type
    notIn(const std::list<const T> &exprList) const
    {
        Expr expr;
        expr.m_description.append(m_description + " NOT IN(");
        expr.joinDescribableList(exprList);
        expr.m_description.append(")");
        return expr;
    }

    template <typename T = StatementSelect>
    typename std::enable_if<std::is_base_of<StatementSelect, T>::value,
                            Expr>::type
    in(const std::list<const T> &statementSelectList) const
    {
        Expr expr;
        expr.m_description.append(m_description + " IN(");
        expr.joinDescribableList(statementSelectList);
        expr.m_description.append(")");
        return expr;
    }

    template <typename T = StatementSelect>
    typename std::enable_if<std::is_base_of<StatementSelect, T>::value,
                            Expr>::type
    notIn(const std::list<const T> &statementSelectList) const
    {
        Expr expr;
        expr.m_description.append(m_description + " NOT IN(");
        expr.joinDescribableList(statementSelectList);
        expr.m_description.append(")");
        return expr;
    }
    Expr in(const std::string &table) const;
    Expr notIn(const std::string &table) const;
    Expr between(const Expr &left, const Expr &right) const;
    Expr notBetween(const Expr &left, const Expr &right) const;

    Expr like(const Expr &operand) const;
    Expr glob(const Expr &operand) const;
    Expr match(const Expr &operand) const;
    Expr regexp(const Expr &operand) const;
    Expr notLike(const Expr &operand) const;
    Expr notGlob(const Expr &operand) const;
    Expr notMatch(const Expr &operand) const;
    Expr notRegexp(const Expr &operand) const;

    Expr like(const Expr &operand, const Expr &escape) const;
    Expr glob(const Expr &operand, const Expr &escape) const;
    Expr match(const Expr &operand, const Expr &escape) const;
    Expr regexp(const Expr &operand, const Expr &escape) const;
    Expr notLike(const Expr &operand, const Expr &escape) const;
    Expr notGlob(const Expr &operand, const Expr &escape) const;
    Expr notMatch(const Expr &operand, const Expr &escape) const;
    Expr notRegexp(const Expr &operand, const Expr &escape) const;

    Expr isNull() const;
    Expr isNotNull() const;
    Expr is(const Expr &operand) const;
    Expr isNot(const Expr &operand) const;

    Expr(const StatementSelect &statementSelect);
    static Expr Exists(const StatementSelect &statementSelect);
    static Expr NotExists(const StatementSelect &statementSelect);

    template <typename T = Expr>
    static typename std::enable_if<std::is_base_of<Expr, T>::value, Expr>::type
    Combine(const std::list<const T> &exprList)
    {
        Expr expr;
        expr.m_description.append("(");
        expr.joinDescribableList(exprList);
        expr.m_description.append(")");
        return expr;
    }

    //aggregate functions
    Expr avg(bool distinct = false) const;
    Expr count(bool distinct = false) const;
    Expr groupConcat(bool distinct = false) const;
    Expr groupConcat(const std::string &seperator, bool distinct = false) const;
    Expr max(bool distinct = false) const;
    Expr min(bool distinct = false) const;
    Expr sum(bool distinct = false) const;
    Expr total(bool distinct = false) const;

    //core functions
    Expr abs(bool distinct = false) const;
    Expr hex(bool distinct = false) const;
    Expr length(bool distinct = false) const;
    Expr lower(bool distinct = false) const;
    Expr upper(bool distinct = false) const;
    Expr round(bool distinct = false) const;

    template <typename T = Expr>
    static typename std::enable_if<std::is_base_of<Expr, T>::value, Expr>::type
    Function(const std::string &function,
             const std::list<const T> &exprList,
             bool distinct = false)
    {
        Expr expr;
        expr.m_description.append(function + "(");
        if (distinct) {
            expr.m_description.append("DISTINCT ");
        }
        expr.joinDescribableList(exprList);
        expr.m_description.append(")");
        return expr;
    }

    template <typename T = Expr>
    static typename std::enable_if<std::is_base_of<Expr, T>::value, Expr>::type
    Case(const Expr &case_,
         const std::list<std::pair<T, T>> &when,
         const std::list<T> &else_)
    {
        Expr expr;
        expr.m_description.append("CASE " + case_.m_description + " ");
        for (const auto &p : when) {
            expr.m_description.append("WHEN ");
            expr.m_description.append(p.first.m_description);
            expr.m_description.append("THEN ");
            expr.m_description.append(p.second.m_description);
            expr.m_description.append(" ");
        }
        for (const auto &e : else_) {
            expr.m_description.append("ELSE ");
            expr.m_description.append(e.m_description);
            expr.m_description.append(" ");
        }
        expr.m_description.append("END");
        return expr;
    }

    //FTS3
    Expr matchinfo() const;
    Expr offsets() const;
    Expr snippet() const;

protected:
    Expr(const LiteralValue &value);
};

} //namespace WCDB

#endif /* expr_hpp */
