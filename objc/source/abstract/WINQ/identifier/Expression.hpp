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

#include <WCDB/AggregateFunction.hpp>
#include <WCDB/CoreFunction.hpp>
#include <WCDB/FTS3Function.hpp>
#include <WCDB/SQL.hpp>

namespace WCDB {

class Expression : public SQLSyntax<Syntax::Expression>,
                   public ExpressionUnaryOperable,
                   public ExpressionBinaryOperable,
                   public ExpressionInOperable,
                   public ExpressionBetweenOperable,
                   public ExpressionCollateOperable,
                   public CoreFunctionOperable,
                   public AggregateFunctionOperable,
                   public FTS3FunctionOperable {
public:
    using SQLSyntax<Syntax::Expression>::SQLSyntax;

    template<typename T, typename Enable = typename std::enable_if<ExpressionConvertible<T>::value>::type>
    Expression(const T& t)
    : Expression(ExpressionConvertible<T>::asExpression(t))
    {
    }

    Expression(const LiteralValue& literalValue);
    Expression(const BindParameter& bindParameter);
    Expression(const Column& column);

    Expression& table(const String& table);
    Expression& schema(const Schema& schema);

    explicit Expression(const Expressions& expressions);
    Expression(const RaiseFunction& raiseFunction);

    static Expression function(const String& function);
    Expression& invoke();
    Expression& invoke(const Expressions& parameters);
    Expression& invokeAll();

    Expression& distinct();

    static Expression cast(const Expression& expression);
    Expression& as(const ColumnType& columnType);

    static Expression exists(const StatementSelect& select);
    static Expression notExists(const StatementSelect& select);

    static Expression case_(const Expression& expression);
    static Expression case_();
    Expression& when(const Expression& expression);
    Expression& then(const Expression& expression);
    Expression& else_(const Expression& expression);

    Expression& escape(const Expression& operand);

    static Expression windowFunction(const String& function);
    static Expression windowFunction(const String& function, const Expressions& parameter);
    static Expression windowFunctionAll(const String& function);
    Expression& filter(const Filter& filter);
    Expression& over(const WindowDef& windowDef);
    Expression& over(const String& window);

protected:
    Expression asExpressionOperand() const override final;
};

} // namespace WCDB

#endif /* Expression_hpp */
