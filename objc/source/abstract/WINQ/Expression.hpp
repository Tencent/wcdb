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

#ifndef Expression_hpp
#define Expression_hpp

#include <WINQ/Describable.hpp>

namespace WCDB {

class Expression : public DescribableWithLang<lang::Expr> {
public:
    Expression(const LiteralValue &literalValue);

    Expression(const BindParameter &bindParameter);

    class Column : public DescribableWithLang<lang::ExprColumn> {
    public:
        Column(const std::string &columnName);
        Column &withTable(const std::string &tableName);
        Column &withSchema(const std::string &schemaName);
    };
    static Expression::Column ColumnNamed(const std::string &columnName);
    Expression(const Expression::Column &expressionColumn);

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

    static Expression Function(const std::string &functionName);
    static Expression Function(const std::string &functionName,
                               const Expression &parameter,
                               bool distinct = false);
    static Expression Function(const std::string &functionName,
                               const std::list<Expression> &parameters,
                               bool distinct = false);
    static Expression FunctionAll(const std::string &functionName);

    Expression(const std::list<Expression> &expressions);

    Expression as(const ColumnType &columnType);

    Expression &withCollate(const std::string &collationName);

    Expression like(const Expression &expr) const;
    Expression glob(const Expression &expr) const;
    Expression regexp(const Expression &expr) const;
    Expression match(const Expression &expr) const;
    Expression notLike(const Expression &expr) const;
    Expression notGlob(const Expression &expr) const;
    Expression notRegexp(const Expression &expr) const;
    Expression notMatch(const Expression &expr) const;
    Expression &withEscape(const Expression &expr);

    Expression isNull() const;
    Expression notNull() const;

    Expression between(const Expression &left, const Expression &right) const;
    Expression notBetween(const Expression &left,
                          const Expression &right) const;

    Expression in(const StatementSelect &selectSTMT);
    Expression notIn(const StatementSelect &selectSTMT);
    Expression in(const Expression &expression);
    Expression notIn(const Expression &expression);
    Expression in(const std::list<Expression> &expressions);
    Expression notIn(const std::list<Expression> &expressions);

    Expression inTable(const std::string &tableName);
    Expression notInTable(const std::string &tableName);
    Expression inTable(const std::string &schemaName,
                       const std::string &tableName);
    Expression notInTable(const std::string &schemaName,
                          const std::string &tableName);

    Expression inFunction(const std::string &functionName);
    Expression notInFunction(const std::string &functionName);
    Expression inFunction(const std::string &schemaName,
                          const std::string &functionName);
    Expression notInFunction(const std::string &schemaName,
                             const std::string &functionName);

    Expression inFunction(const std::string &functionName,
                          const Expression &expression);
    Expression notInFunction(const std::string &functionName,
                             const Expression &expression);
    Expression inFunction(const std::string &schemaName,
                          const std::string &functionName,
                          const Expression &expression);
    Expression notInFunction(const std::string &schemaName,
                             const std::string &functionName,
                             const Expression &expression);

    Expression inFunction(const std::string &functionName,
                          const std::list<Expression> &expressions);
    Expression notInFunction(const std::string &functionName,
                             const std::list<Expression> &expressions);
    Expression inFunction(const std::string &schemaName,
                          const std::string &functionName,
                          const std::list<Expression> &expressions);
    Expression notInFunction(const std::string &schemaName,
                             const std::string &functionName,
                             const std::list<Expression> &expressions);

    static Expression Exists(const StatementSelect &selectSTMT);
    static Expression NotExists(const StatementSelect &selectSTMT);
    Expression(const StatementSelect &selectSTMT,
               bool isNot = false,
               bool exists = false);

    class CaseInternal : public DescribableWithLang<lang::ExprCase> {
    public:
        CaseInternal();
        CaseInternal(const Expression &expression);
        CaseInternal &whenAndThen(const Expression &when,
                                  const Expression &then);
        CaseInternal &else_(const Expression &expression);
    };
    static Expression::CaseInternal Case(const Expression &expression);
    static Expression::CaseInternal Case();
    Expression(const CaseInternal &expressionCase);

    Expression(const RaiseFunction &raiseFunction);

protected:
    Expression(const lang::copy_on_write_lazy_lang<lang::ExprUnaryOperation>
                   &exprUnaryOperation);
    Expression(const lang::copy_on_write_lazy_lang<lang::ExprBinaryOperation>
                   &exprBinaryOperation);
    Expression(
        const lang::copy_on_write_lazy_lang<lang::ExprFunction> &exprFunction);
    Expression(const lang::copy_on_write_lazy_lang<lang::ExprCast> &exprCast);
    Expression(
        const lang::copy_on_write_lazy_lang<lang::ExprPattern> &exprPattern);
    Expression(const lang::copy_on_write_lazy_lang<lang::ExprNull> &exprNull);
    Expression(
        const lang::copy_on_write_lazy_lang<lang::ExprBetween> &exprBetween);
    Expression(const lang::copy_on_write_lazy_lang<lang::ExprIn> &exprIn);
    Expression(
        const lang::copy_on_write_lazy_lang<lang::ExprExists> &exprExists);

    lang::copy_on_write_lazy_lang<lang::ExprUnaryOperation> unaryOperation(
        const lang::ExprUnaryOperation::Operator &unaryOperator) const;
    lang::copy_on_write_lazy_lang<lang::ExprBinaryOperation>
    binaryOperation(const lang::ExprBinaryOperation::Operator &binaryOperator,
                    const Expression &operand) const;
    lang::copy_on_write_lazy_lang<lang::ExprPattern>
    pattern(const lang::ExprPattern::Type &type,
            const Expression &operand,
            bool isNot) const;
    lang::copy_on_write_lazy_lang<lang::ExprNull> null(bool isNull) const;
    lang::copy_on_write_lazy_lang<lang::ExprBetween>
    between(const Expression &left, const Expression &right, bool isNot) const;

    lang::copy_on_write_lazy_lang<lang::ExprIn>
    in(const StatementSelect &selectSTMT, bool isNot);
    lang::copy_on_write_lazy_lang<lang::ExprIn> in(const Expression &expression,
                                                   bool isNot);
    lang::copy_on_write_lazy_lang<lang::ExprIn>
    in(const std::list<Expression> &expressions, bool isNot);

    lang::copy_on_write_lazy_lang<lang::ExprIn>
    inTable(const std::string &tableName, bool isNot);
    lang::copy_on_write_lazy_lang<lang::ExprIn>
    inTable(const std::string &schemaName,
            const std::string &tableName,
            bool isNot);

    lang::copy_on_write_lazy_lang<lang::ExprIn>
    inFunction(const std::string &functionName, bool isNot);
    lang::copy_on_write_lazy_lang<lang::ExprIn>
    inFunction(const std::string &schemaName,
               const std::string &functionName,
               bool isNot);
    lang::copy_on_write_lazy_lang<lang::ExprIn>
    inFunction(const std::string &functionName,
               const Expression &parameter,
               bool isNot);
    lang::copy_on_write_lazy_lang<lang::ExprIn>
    inFunction(const std::string &schemaName,
               const std::string &functionName,
               const Expression &parameter,
               bool isNot);
    lang::copy_on_write_lazy_lang<lang::ExprIn>
    inFunction(const std::string &functionName,
               const std::list<Expression> &parameters,
               bool isNot);
    lang::copy_on_write_lazy_lang<lang::ExprIn>
    inFunction(const std::string &schemaName,
               const std::string &functionName,
               const std::list<Expression> &parameters,
               bool isNot);
};

} // namespace WCDB

#endif /* Expression_hpp */
