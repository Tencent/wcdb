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

#ifndef operable_hpp
#define operable_hpp

#include <WCDB/declare.hpp>
#include <WCDB/convertible.hpp>

namespace WCDB {
    
class Operator {
protected:
    template <typename T, typename Enable>
    friend class Operable;
    friend class Expression;
    
    static Expression operateWithRaw(const std::string &raw);
    static Expression operateWithPrefix(const std::string &prefix, const Expression& operand);
    template <typename T>
    static typename std::enable_if<ExpressionConvertible<T>::value, Expression>::type operateWithTitle(const std::string &title, const std::string& infix, const std::list<const T> &list) {
        return Expression(title + "(" + (infix.empty()?"":infix+" ") + stringByJoiningList(list) + ")", nullptr);
    }
    static Expression operateWithTitle(const std::string &title, const std::string& infix, const std::list<const Expression> &list);
    static Expression operateWithPostfix(const Expression& operand, const std::string &postfix);
    static Expression operateWithOperator(const Expression& left, const std::string &op, const Expression& right);
    static Expression operateWithComplexOperator(const Expression& one, const std::string &op1, const Expression& two, const std::string& op2, const Expression &three);
};

template <typename T, typename Enable = void>
class Operable{};

template <typename T>
class Operable<T, typename std::enable_if<ExpressionConvertible<T>::value>::type>
{
public:
    T operator !() const;
    T operator +() const;
    T operator -() const;
    T operator ~() const;
    
    T operator ||(const Expression& operand) const;
    T operator &&(const Expression& operand) const;
    T operator *(const Expression& operand) const;
    T operator /(const Expression& operand) const;
    T operator %(const Expression& operand) const;
    T operator +(const Expression& operand) const;
    T operator -(const Expression& operand) const;
    T operator <<(const Expression& operand) const;
    T operator >>(const Expression& operand) const;
    T operator &(const Expression& operand) const;
    T operator |(const Expression& operand) const;
    T operator <(const Expression& operand) const;
    T operator <=(const Expression& operand) const;
    T operator >(const Expression& operand) const;
    T operator >=(const Expression& operand) const;
    T operator ==(const Expression& operand) const;
    T operator !=(const Expression& operand) const;
    
    T concat(const Expression& operand) const;
    T substr(const Expression& start, const Expression& length) const;
    
    T like(const Expression& operand) const ;
    T glob(const Expression& operand) const ;
    T match(const Expression& operand) const ;
    T regexp(const Expression& operand) const ;
    T notLike(const Expression& operand) const ;
    T notGlob(const Expression& operand) const ;
    T notMatch(const Expression& operand) const ;
    T notRegexp(const Expression& operand) const ;
    
    T like(const Expression& operand, const Expression& escape) const;
    T glob(const Expression& operand, const Expression& escape) const ;
    T match(const Expression& operand, const Expression& escape) const;
    T regexp(const Expression& operand, const Expression& escape) const;
    T notLike(const Expression& operand, const Expression& escape) const;
    T notGlob(const Expression& operand, const Expression& escape) const;
    T notMatch(const Expression& operand, const Expression& escape) const;
    T notRegexp(const Expression& operand, const Expression& escape) const;
    
    T isNull() const;
    T isNotNull() const;
    T is(const Expression& operand) const;
    T isNot(const Expression& operand) const;
    T between(const Expression& begin, const Expression& end) const;
    T notBetween(const Expression& begin, const Expression& end) const;
    
    T in(const StatementSelect& statementSelect) const;
    T notIn(const StatementSelect&  statementSelect) const;
    template <typename U>
    typename std::enable_if<ExpressionConvertible<U>::value, T>::type in(const std::list<const U> &list) const;
    template <typename U>
    typename std::enable_if<ExpressionConvertible<U>::value, T>::type notIn(const std::list<const U> &list) const;
    T in(const std::list<const Expression> &list) const;
    T notIn(const std::list<const Expression> &list) const;
    T in(const Expression &expression) const;
    T notIn(const Expression &expression) const;
    
    //aggregate functions
    T avg(bool isDistinct = false) const;
    T count(bool isDistinct = false) const;
    T groupConcat(bool isDistinct = false) const;
    T groupConcat(bool isDistinct, const std::string& seperator) const ;
    T max(bool isDistinct = false) const;
    T min(bool isDistinct = false) const;
    T sum(bool isDistinct = false) const;
    T total(bool isDistinct = false) const;
    
    //core functions
    T abs(bool isDistinct = false) const;
    T hex(bool isDistinct = false) const;
    T length(bool isDistinct = false) const;
    T lower(bool isDistinct = false) const;
    T upper(bool isDistinct = false) const;
    T round(bool isDistinct = false) const;
    
    //FTS3
    T matchinfo() const;
    T offsets() const;
    T snippet() const;
protected:
    virtual T asExpression() const = 0;
};

template <>
Expression Operable<Expression>::operator !() const;
template <>
Expression Operable<Expression>::operator +() const;
template <>
Expression Operable<Expression>::operator -() const;
template <>
Expression Operable<Expression>::operator ~() const;

template <>
Expression Operable<Expression>::operator ||(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator &&(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator *(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator /(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator %(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator +(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator -(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator <<(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator >>(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator &(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator |(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator <(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator <=(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator >(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator >=(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator ==(const Expression& operand) const;
template <>
Expression Operable<Expression>::operator !=(const Expression& operand) const;

template <>
Expression Operable<Expression>::concat(const Expression& operand) const;
template <>
Expression Operable<Expression>::substr(const Expression& start, const Expression& length) const;
template <>
Expression Operable<Expression>::like(const Expression& operand) const;
template <>
Expression Operable<Expression>::glob(const Expression& operand) const;
template <>
Expression Operable<Expression>::match(const Expression& operand) const;
template <>
Expression Operable<Expression>::regexp(const Expression& operand) const;
template <>
Expression Operable<Expression>::notLike(const Expression& operand) const;
template <>
Expression Operable<Expression>::notGlob(const Expression& operand) const;
template <>
Expression Operable<Expression>::notMatch(const Expression& operand) const;
template <>
Expression Operable<Expression>::notRegexp(const Expression& operand) const;
template <>
Expression Operable<Expression>::like(const Expression& operand, const Expression& escape) const;
template <>
Expression Operable<Expression>::glob(const Expression& operand, const Expression& escape) const;
template <>
Expression Operable<Expression>::match(const Expression& operand, const Expression& escape) const;
template <>
Expression Operable<Expression>::regexp(const Expression& operand, const Expression& escape) const;
template <>
Expression Operable<Expression>::notLike(const Expression& operand, const Expression& escape) const;
template <>
Expression Operable<Expression>::notGlob(const Expression& operand, const Expression& escape) const;
template <>
Expression Operable<Expression>::notMatch(const Expression& operand, const Expression& escape) const;
template <>
Expression Operable<Expression>::notRegexp(const Expression& operand, const Expression& escape) const;
template <>
Expression Operable<Expression>::isNull() const;
template <>
Expression Operable<Expression>::isNotNull() const;
template <>
Expression Operable<Expression>::is(const Expression& operand) const;
template <>
Expression Operable<Expression>::isNot(const Expression& operand) const;
template <>
Expression Operable<Expression>::between(const Expression& begin, const Expression& end) const;
template <>
Expression Operable<Expression>::notBetween(const Expression& begin, const Expression& end) const;
template <>
Expression Operable<Expression>::in(const StatementSelect& statementSelect) const;
template <>
Expression Operable<Expression>::notIn(const StatementSelect&  statementSelect) const;
    
template <>
template <typename U>
typename std::enable_if<ExpressionConvertible<U>::value, Expression>::type Operable<Expression>::in(const std::list<const U> &list) const {
    return Operator::operateWithPostfix(*this, "IN(" + stringByJoiningList(list) + ")");
}
    
template <>
template <typename U>
typename std::enable_if<ExpressionConvertible<U>::value, Expression>::type Operable<Expression>::notIn(const std::list<const U> &list) const {
    return Operator::operateWithPostfix(*this, "NOT IN(" + stringByJoiningList(list) + ")");
}
    
template <>
Expression Operable<Expression>::in(const std::list<const Expression> &list) const;
template <>
Expression Operable<Expression>::notIn(const std::list<const Expression> &list) const;
template <>
Expression Operable<Expression>::in(const Expression &expression) const;
template <>
Expression Operable<Expression>::notIn(const Expression &expression) const;
    
template <>
Expression Operable<Expression>::avg(bool isDistinct) const;
template <>
Expression Operable<Expression>::count(bool isDistinct) const;
template <>
Expression Operable<Expression>::groupConcat(bool isDistinct) const;
template <>
Expression Operable<Expression>::groupConcat(bool isDistinct, const std::string& seperator) const;
template <>
Expression Operable<Expression>::max(bool isDistinct) const;
template <>
Expression Operable<Expression>::min(bool isDistinct) const;
template <>
Expression Operable<Expression>::sum(bool isDistinct) const;
template <>
Expression Operable<Expression>::total(bool isDistinct) const;
template <>
Expression Operable<Expression>::abs(bool isDistinct) const;
template <>
Expression Operable<Expression>::hex(bool isDistinct) const;
template <>
Expression Operable<Expression>::length(bool isDistinct) const;
template <>
Expression Operable<Expression>::lower(bool isDistinct) const;
template <>
Expression Operable<Expression>::upper(bool isDistinct) const;
template <>
Expression Operable<Expression>::round(bool isDistinct) const;
template <>
Expression Operable<Expression>::matchinfo() const;
template <>
Expression Operable<Expression>::offsets() const;
template <>
Expression Operable<Expression>::snippet() const;
    
} //namespace WCDB

#endif /* operable_hpp */
