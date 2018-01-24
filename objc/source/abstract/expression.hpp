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

#ifndef expression_hpp
#define expression_hpp

#include <WCDB/convertible.hpp>
#include <WCDB/declare.hpp>
#include <WCDB/operable.hpp>
#include <WCDB/redirectable.hpp>

namespace WCDB {

template <>
class Redirectable<Expression> {
public:
    template <typename T>
    T as(const T &t) const;

private:
    virtual const std::string &getRedirectableDescription() const = 0;
};

class Expression : public Describable,
                   public Operable,
                   public Redirectable<Expression> {
public:
    operator std::list<const Expression>() const;

    static const Expression BindParameter;

    template <typename T>
    Expression(const T &t,
               typename std::enable_if<ColumnConvertible<T>::value, void>::type
                   * = nullptr)
        : Describable(ColumnConvertible<T>::asColumn(t).getDescription())
    {
    }

    template <typename T>
    Expression(const T &t,
               typename std::enable_if<LiteralValueConvertible<T>::value,
                                       void>::type * = nullptr)
        : Describable(
              LiteralValueConvertible<T>::asLiteralValue(t).getDescription())
    {
    }

    Expression(const StatementSelect &statementSelect);

    static Expression Exists(const StatementSelect &statementSelect);
    static Expression NotExists(const StatementSelect &statementSelect);

    template <typename T>
    static Expression Combine(
        const std::list<const T> &list,
        typename std::enable_if<ExpressionConvertible<T>::value, void>::type * =
            nullptr)
    {
        return Expression("(" + stringByJoiningList(list) + ")", nullptr);
    }

    static Expression Combine(const std::list<const Expression> &list);

    template <typename T>
    static Expression Function(
        const std::string &name,
        const std::list<const T> &list,
        bool isDistinct = false,
        typename std::enable_if<ExpressionConvertible<T>::value, void>::type * =
            nullptr)
    {
        return Operator::operateWithTitle(name, isDistinct ? "DISTINCT" : "",
                                          list);
    }

    static Expression Function(const std::string &name,
                               const std::list<const Expression> &list,
                               bool isDistinct = false);

    static Expression Function(const std::string &name,
                               const Expression &expression,
                               bool isDistinct = false);

    class CaseExpression : public Describable {
    public:
        CaseExpression(const Expression &expression);
        CaseExpression &when(const Expression &expression);
        CaseExpression &then(const Expression &expression);
        CaseExpression &else_(const Expression &expression);
    };

    static CaseExpression Case(const Expression &expression);

protected:
    friend class Operator;
    Expression(const std::string &raw, const std::nullptr_t &dummy);
    virtual Expression asExpression() const override;
    virtual const std::string &getRedirectableDescription() const override;
};

} //namespace WCDB

#endif /* expression_hpp */
