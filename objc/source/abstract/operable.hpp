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
 * WITHOUExpression WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef operable_hpp
#define operable_hpp

#include <WCDB/convertible.hpp>
#include <WCDB/declare.hpp>

namespace WCDB {

class Operator {
protected:
    friend class Operable;
    friend class Expression;

    static Expression operateWithRaw(const std::string &raw);
    static Expression operateWithPrefix(const std::string &prefix,
                                        const Expression &operand);
    template <typename T>
    static typename std::enable_if<ExpressionConvertible<T>::value,
                                   Expression>::type
    operateWithTitle(const std::string &title,
                     const std::string &infix,
                     const std::list<const T> &list)
    {
        return Expression(title + "(" + (infix.empty() ? "" : infix + " ") +
                              stringByJoiningList(list) + ")",
                          nullptr);
    }
    static Expression operateWithTitle(const std::string &title,
                                       const std::string &infix,
                                       const std::list<const Expression> &list);
    static Expression operateWithPostfix(const Expression &operand,
                                         const std::string &postfix);
    static Expression operateWithOperator(const Expression &left,
                                          const std::string &op,
                                          const Expression &right);
    static Expression operateWithComplexOperator(const Expression &one,
                                                 const std::string &op1,
                                                 const Expression &two,
                                                 const std::string &op2,
                                                 const Expression &three);
};

class Operable {
public:
    Expression operator!() const;
    Expression operator+() const;
    Expression operator-() const;
    Expression operator~() const;

    Expression operator||(const Expression &operand) const;
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
    Expression between(const Expression &begin, const Expression &end) const;
    Expression notBetween(const Expression &begin, const Expression &end) const;

    Expression in(const StatementSelect &statementSelect) const;
    Expression notIn(const StatementSelect &statementSelect) const;
    template <typename T>
    typename std::enable_if<ExpressionConvertible<T>::value, Expression>::type
    in(const std::list<const T> &list) const;
    template <typename T>
    typename std::enable_if<ExpressionConvertible<T>::value, Expression>::type
    notIn(const std::list<const T> &list) const;
    Expression in(const std::list<const Expression> &list) const;
    Expression notIn(const std::list<const Expression> &list) const;
    Expression in(const Expression &expression) const;
    Expression notIn(const Expression &expression) const;

    //aggregate functions
    Expression avg(bool isDistinct = false) const;
    Expression count(bool isDistinct = false) const;
    Expression groupConcat(bool isDistinct = false) const;
    Expression groupConcat(bool isDistinct, const std::string &seperator) const;
    Expression max(bool isDistinct = false) const;
    Expression min(bool isDistinct = false) const;
    Expression sum(bool isDistinct = false) const;
    Expression total(bool isDistinct = false) const;

    //core functions
    Expression abs(bool isDistinct = false) const;
    Expression hex(bool isDistinct = false) const;
    Expression length(bool isDistinct = false) const;
    Expression lower(bool isDistinct = false) const;
    Expression upper(bool isDistinct = false) const;
    Expression round(bool isDistinct = false) const;

    //FTS3
    Expression matchinfo() const;
    Expression offsets() const;
    Expression snippet() const;

protected:
    virtual Expression asExpression() const = 0;
};

} //namespace WCDB

#endif /* operable_hpp */
