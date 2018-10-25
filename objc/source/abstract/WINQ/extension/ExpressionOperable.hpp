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

#ifndef Operable_hpp
#define Operable_hpp

#include <WCDB/SQL.hpp>

namespace WCDB {

#warning TODO make return type configurable
class ExpressionOperable {
public:
    virtual ~ExpressionOperable();

protected:
    virtual Expression asExpressionOperand() const = 0;
};

class ExpressionUnaryOperable : virtual public ExpressionOperable {
public:
    virtual ~ExpressionUnaryOperable();
    Expression operator-() const;
    Expression operator+() const;
    Expression operator!() const;
    Expression operator~() const;
    Expression isNull() const;
    Expression notNull() const;

private:
    Expression unaryOperate(const Syntax::Expression::UnaryOperator &op) const;
};

class ExpressionBinaryOperable : virtual public ExpressionOperable {
public:
    virtual ~ExpressionBinaryOperable();

    Expression concat(const Expression &operand) const;
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
    Expression is(const Expression &operand) const;
    Expression isNot(const Expression &operand) const;
    Expression operator&&(const Expression &operand) const;
    Expression operator||(const Expression &operand) const;

    Expression like(const Expression &operand) const;
    Expression glob(const Expression &operand) const;
    Expression regexp(const Expression &operand) const;
    Expression match(const Expression &operand) const;
    Expression notLike(const Expression &operand) const;
    Expression notGlob(const Expression &operand) const;
    Expression notRegexp(const Expression &operand) const;
    Expression notMatch(const Expression &operand) const;

private:
    Expression binaryOperate(const Expression &operand,
                             const Syntax::Expression::BinaryOperator &op) const;
};

class ExpressionBetweenOperable : virtual public ExpressionOperable {
public:
    virtual ~ExpressionBetweenOperable();
    Expression between(const Expression &left, const Expression &right) const;
    Expression notBetween(const Expression &left, const Expression &right) const;

private:
    Expression betweenOperate(const Expression &left, const Expression &right) const;
};

class ExpressionInOperable : virtual public ExpressionOperable {
public:
    virtual ~ExpressionInOperable();

    Expression in();
    Expression notIn();
    Expression inTable(const String &table);
    Expression notInTable(const String &table);
    Expression in(const StatementSelect &select);
    Expression notIn(const StatementSelect &select);
    Expression in(const Expressions &expressions);
    Expression notIn(const Expressions &expressions);
    Expression inFunction(const String &tableFunction);
    Expression notInFunction(const String &tableFunction);
};

class ExpressionCollateOperable : virtual public ExpressionOperable {
public:
    virtual ~ExpressionCollateOperable();
    Expression collate(const String &collation);
};

} // namespace WCDB

#endif /* Operable_hpp */
