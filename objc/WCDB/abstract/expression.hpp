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

class Expression : public Describable {
public:
    static const Expression BindParameter;

    Expression();
    Expression(const Column &column);

    template <typename T>
    Expression(
        const T &value,
        typename std::enable_if<std::is_arithmetic<T>::value ||
                                std::is_enum<T>::value>::type * = nullptr)
        : Describable(LiteralValue(value))
    {
    }

    Expression(const char *value);

    Expression(const std::string &value);

    Expression(const std::nullptr_t &value);

    Expression(const typename ColumnTypeInfo<ColumnType::BLOB>::CType &value,
               int size);

    operator ExprList() const;

    //unary
    Expression operator!() const;
    Expression operator+() const;
    Expression operator-() const;
    Expression operator~() const;

    //binary
    Expression operator||(const Expression &operand) const; //or, not concat
    Expression operator&&(const Expression &operand) const;
    Expression operator*(const Expression &operand) const;
    Expression operator/(const Expression &operand) const;
    Expression operator%(const Expression &operand) const;
    Expression operator+(const Expression &operand) const;
    Expression operator-(const Expression &operand) const;
    Expression operator<<(const Expression &operand) const;
    Expression operator>>(const Expression &operand) const;
    Expression operator&(const Expression &operand) const;
    Expression operator|(const Expression &operand) const;
    Expression operator<(const Expression &operand) const;
    Expression operator<=(const Expression &operand) const;
    Expression operator>(const Expression &operand) const;
    Expression operator>=(const Expression &operand) const;
    Expression operator==(const Expression &operand) const;
    Expression operator!=(const Expression &operand) const;

    Expression concat(const Expression &operand) const;
    Expression substr(const Expression &start, const Expression &length) const;

    template <typename T = Expression>
    typename std::enable_if<std::is_base_of<Expression, T>::value,
                            Expression>::type
    in(const std::list<const T> &exprList) const
    {
        Expression expr;
        expr.m_description.append(m_description + " IN(");
        expr.joinDescribableList(exprList);
        expr.m_description.append(")");
        return expr;
    }

    template <typename T = Expression>
    typename std::enable_if<std::is_base_of<Expression, T>::value,
                            Expression>::type
    notIn(const std::list<const T> &exprList) const
    {
        Expression expr;
        expr.m_description.append(m_description + " NOT IN(");
        expr.joinDescribableList(exprList);
        expr.m_description.append(")");
        return expr;
    }

    template <typename T = StatementSelect>
    typename std::enable_if<std::is_base_of<StatementSelect, T>::value,
                            Expression>::type
    in(const std::list<const T> &statementSelectList) const
    {
        Expression expr;
        expr.m_description.append(m_description + " IN(");
        expr.joinDescribableList(statementSelectList);
        expr.m_description.append(")");
        return expr;
    }

    template <typename T = StatementSelect>
    typename std::enable_if<std::is_base_of<StatementSelect, T>::value,
                            Expression>::type
    notIn(const std::list<const T> &statementSelectList) const
    {
        Expression expr;
        expr.m_description.append(m_description + " NOT IN(");
        expr.joinDescribableList(statementSelectList);
        expr.m_description.append(")");
        return expr;
    }
    Expression in(const std::string &table) const;
    Expression notIn(const std::string &table) const;
    Expression between(const Expression &left, const Expression &right) const;
    Expression notBetween(const Expression &left,
                          const Expression &right) const;

    Expression like(const Expression &operand) const;
    Expression glob(const Expression &operand) const;
    Expression match(const Expression &operand) const;
    Expression regexp(const Expression &operand) const;
    Expression notLike(const Expression &operand) const;
    Expression notGlob(const Expression &operand) const;
    Expression notMatch(const Expression &operand) const;
    Expression notRegexp(const Expression &operand) const;

    Expression like(const Expression &operand, const Expression &escape) const;
    Expression glob(const Expression &operand, const Expression &escape) const;
    Expression match(const Expression &operand, const Expression &escape) const;
    Expression regexp(const Expression &operand,
                      const Expression &escape) const;
    Expression notLike(const Expression &operand,
                       const Expression &escape) const;
    Expression notGlob(const Expression &operand,
                       const Expression &escape) const;
    Expression notMatch(const Expression &operand,
                        const Expression &escape) const;
    Expression notRegexp(const Expression &operand,
                         const Expression &escape) const;

    Expression isNull() const;
    Expression isNotNull() const;
    Expression is(const Expression &operand) const;
    Expression isNot(const Expression &operand) const;

    Expression(const StatementSelect &statementSelect);
    static Expression Exists(const StatementSelect &statementSelect);
    static Expression NotExists(const StatementSelect &statementSelect);

    template <typename T = Expression>
    static typename std::enable_if<std::is_base_of<Expression, T>::value,
                                   Expression>::type
    Combine(const std::list<const T> &exprList)
    {
        Expression expr;
        expr.m_description.append("(");
        expr.joinDescribableList(exprList);
        expr.m_description.append(")");
        return expr;
    }

    //aggregate functions
    Expression avg(bool distinct = false) const;
    Expression count(bool distinct = false) const;
    Expression groupConcat(bool distinct = false) const;
    Expression groupConcat(const std::string &seperator,
                           bool distinct = false) const;
    Expression max(bool distinct = false) const;
    Expression min(bool distinct = false) const;
    Expression sum(bool distinct = false) const;
    Expression total(bool distinct = false) const;

    //core functions
    Expression abs(bool distinct = false) const;
    Expression hex(bool distinct = false) const;
    Expression length(bool distinct = false) const;
    Expression lower(bool distinct = false) const;
    Expression upper(bool distinct = false) const;
    Expression round(bool distinct = false) const;

    template <typename T = Expression>
    static typename std::enable_if<std::is_base_of<Expression, T>::value,
                                   Expression>::type
    Function(const std::string &function,
             const std::list<const T> &exprList,
             bool distinct = false)
    {
        Expression expr;
        expr.m_description.append(function + "(");
        if (distinct) {
            expr.m_description.append("DISTINCT ");
        }
        expr.joinDescribableList(exprList);
        expr.m_description.append(")");
        return expr;
    }

    template <typename T = Expression>
    static typename std::enable_if<std::is_base_of<Expression, T>::value,
                                   Expression>::type
    Case(const Expression &case_,
         const std::list<std::pair<T, T>> &when,
         const std::list<T> &else_)
    {
        Expression expr;
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
    Expression matchinfo() const;
    Expression offsets() const;
    Expression snippet() const;

protected:
    Expression(const LiteralValue &value);
};

} //namespace WCDB

#endif /* expr_hpp */
