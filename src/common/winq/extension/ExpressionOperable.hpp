//
// Created by sanhuazhang on 2019/05/02
//

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

#pragma once

#include "SQL.hpp"

namespace WCDB {

class WCDB_API ExpressionOperable {
public:
    virtual ~ExpressionOperable() = 0;

protected:
    virtual Expression asExpressionOperand() const = 0;
};

class WCDB_API ExpressionUnaryOperable : virtual public ExpressionOperable {
public:
    virtual ~ExpressionUnaryOperable() override = 0;
    Expression operator!() const;
    Expression operator~() const;
    Expression isNull() const;
    Expression notNull() const;

private:
    Expression unaryOperate(const Syntax::Expression::UnaryOperator &op) const;
};

class WCDB_API ExpressionBinaryOperable : virtual public ExpressionOperable {
public:
    virtual ~ExpressionBinaryOperable() override = 0;

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

class WCDB_API ExpressionBetweenOperable : virtual public ExpressionOperable {
public:
    virtual ~ExpressionBetweenOperable() override = 0;
    Expression between(const Expression &left, const Expression &right) const;
    Expression notBetween(const Expression &left, const Expression &right) const;

private:
    Expression betweenOperate(const Expression &left, const Expression &right) const;
};

class WCDB_API ExpressionInOperable : virtual public ExpressionOperable {
public:
    virtual ~ExpressionInOperable() override = 0;

    Expression in() const;
    Expression notIn() const;
    Expression inTable(const UnsafeStringView &table) const;
    Expression notInTable(const UnsafeStringView &table) const;
    Expression in(const StatementSelect &select) const;
    Expression notIn(const StatementSelect &select) const;
    // in(arguments) usually accept multiple arguments, so the seperated method is not provided.
    Expression in(const Expressions &expressions) const;
    Expression notIn(const Expressions &expressions) const;
    Expression inFunction(const UnsafeStringView &tableFunction) const;
    Expression notInFunction(const UnsafeStringView &tableFunction) const;
};

class WCDB_API ExpressionCollateOperable : virtual public ExpressionOperable {
public:
    virtual ~ExpressionCollateOperable() override = 0;
    Expression collate(const UnsafeStringView &collation) const;
};

} // namespace WCDB
