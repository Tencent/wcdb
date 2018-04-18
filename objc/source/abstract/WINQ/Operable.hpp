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

#include <WCDB/Describable.hpp>

namespace WCDB {

class FunctionOperable {
public:
    virtual Expression function(const std::string &functionName,
                                bool distinct = false) const = 0;

    //Core Functions
    Expression abs(bool distinct = false) const;
    Expression hex(bool distinct = false) const;
    Expression length(bool distinct = false) const;
    Expression lower(bool distinct = false) const;
    Expression upper(bool distinct = false) const;
    Expression total_change(bool distinct = false) const;

    //Aggregate Functions
    Expression avg(bool distinct = false) const;
    Expression count(bool distinct = false) const;
    Expression group_concat(bool distinct = false) const;
    Expression group_concat(const Expression &separator,
                            bool distinct = false) const;
    Expression max(bool distinct = false) const;
    Expression min(bool distinct = false) const;
    Expression total(bool distinct = false) const;
    Expression sum(bool distinct = false) const;

    //FTS Functions
    Expression offsets() const;
    Expression snippet() const;
    Expression matchinfo() const;
};

class Operable : public FunctionOperable {
public:
    Expression operator-() const;
    Expression operator+() const;
    Expression operator!() const;
    Expression operator~() const;

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

    Expression like(const Expression &expr) const;
    Expression glob(const Expression &expr) const;
    Expression regexp(const Expression &expr) const;
    Expression match(const Expression &expr) const;
    Expression notLike(const Expression &expr) const;
    Expression notGlob(const Expression &expr) const;
    Expression notRegexp(const Expression &expr) const;
    Expression notMatch(const Expression &expr) const;
    Expression withEscape(const Expression &expr) const;

    Expression isNull() const;
    Expression notNull() const;

    Expression between(const Expression &left, const Expression &right) const;
    Expression notBetween(const Expression &left,
                          const Expression &right) const;

    Expression in() const;
    Expression notIn() const;
    Expression in(const StatementSelect &selectSTMT) const;
    Expression notIn(const StatementSelect &selectSTMT) const;
    Expression in(const Expression &expression) const;
    Expression notIn(const Expression &expression) const;
    Expression in(const std::list<Expression> &expressions) const;
    Expression notIn(const std::list<Expression> &expressions) const;

    Expression in(const std::string &tableName) const;
    Expression notIn(const std::string &tableName) const;
    Expression in(const std::string &schemaName,
                  const std::string &tableName) const;
    Expression notIn(const std::string &schemaName,
                     const std::string &tableName) const;

    Expression inFunction(const std::string &functionName) const;
    Expression notInFunction(const std::string &functionName) const;
    Expression inFunction(const std::string &schemaName,
                          const std::string &functionName) const;
    Expression notInFunction(const std::string &schemaName,
                             const std::string &functionName) const;

    Expression inFunction(const std::string &functionName,
                          const Expression &expression) const;
    Expression notInFunction(const std::string &functionName,
                             const Expression &expression) const;
    Expression inFunction(const std::string &schemaName,
                          const std::string &functionName,
                          const Expression &expression) const;
    Expression notInFunction(const std::string &schemaName,
                             const std::string &functionName,
                             const Expression &expression) const;

    Expression inFunction(const std::string &functionName,
                          const std::list<Expression> &expressions) const;
    Expression notInFunction(const std::string &functionName,
                             const std::list<Expression> &expressions) const;
    Expression inFunction(const std::string &schemaName,
                          const std::string &functionName,
                          const std::list<Expression> &expressions) const;
    Expression notInFunction(const std::string &schemaName,
                             const std::string &functionName,
                             const std::list<Expression> &expressions) const;

    Expression cast(const ColumnType &columnType) const;

    Expression collate(const std::string &collationName) const;

    Expression function(const std::string &functionName,
                        bool distinct = false) const override;

protected:
    virtual Lang::CopyOnWriteLazyLang<Lang::Expr> getExpressionLang() const = 0;

private:
    Lang::CopyOnWriteLazyLang<Lang::ExprUnaryOperation> unaryOperation(
        const Lang::ExprUnaryOperation::Operator &unaryOperator) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprBinaryOperation>
    binaryOperation(const Lang::ExprBinaryOperation::Operator &binaryOperator,
                    const Expression &operand) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprPattern>
    pattern(const Lang::ExprPattern::Type &type,
            const Expression &operand,
            bool isNot) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprNull> null(bool isNull) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprBetween>
    between(const Expression &left, const Expression &right, bool isNot) const;

    Lang::CopyOnWriteLazyLang<Lang::ExprIn> in(bool isNot) const;

    Lang::CopyOnWriteLazyLang<Lang::ExprIn>
    in(const StatementSelect &selectSTMT, bool isNot) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> in(const Expression &expression,
                                               bool isNot) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprIn>
    in(const std::list<Expression> &expressions, bool isNot) const;

    Lang::CopyOnWriteLazyLang<Lang::ExprIn> in(const std::string &tableName,
                                               bool isNot) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprIn> in(const std::string &schemaName,
                                               const std::string &tableName,
                                               bool isNot) const;

    Lang::CopyOnWriteLazyLang<Lang::ExprIn>
    inFunction(const std::string &functionName, bool isNot) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprIn>
    inFunction(const std::string &schemaName,
               const std::string &functionName,
               bool isNot) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprIn>
    inFunction(const std::string &functionName,
               const Expression &parameter,
               bool isNot) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprIn>
    inFunction(const std::string &schemaName,
               const std::string &functionName,
               const Expression &parameter,
               bool isNot) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprIn>
    inFunction(const std::string &functionName,
               const std::list<Expression> &parameters,
               bool isNot) const;
    Lang::CopyOnWriteLazyLang<Lang::ExprIn>
    inFunction(const std::string &schemaName,
               const std::string &functionName,
               const std::list<Expression> &parameters,
               bool isNot) const;
};

} // namespace WCDB

#endif /* Operable_hpp */
