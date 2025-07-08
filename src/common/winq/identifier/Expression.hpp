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

#include "AggregateFunction.hpp"
#include "CoreFunction.hpp"
#include "FTSFunction.hpp"
#include "SQL.hpp"

namespace WCDB {

class WCDB_API Expression final : public SpecifiedSyntax<Syntax::Expression, SQL>,
                                  public ExpressionUnaryOperable,
                                  public ExpressionBinaryOperable,
                                  public ExpressionInOperable,
                                  public ExpressionBetweenOperable,
                                  public ExpressionCollateOperable,
                                  public CoreFunctionOperable,
                                  public AggregateFunctionOperable,
                                  public FTSFunctionOperable {
public:
    using SpecifiedSyntax<Syntax::Expression, SQL>::SpecifiedSyntax;
    ~Expression() override;

    template<typename T, typename Enable = typename std::enable_if<ExpressionConvertible<T>::value>::type>
    Expression(const T& t)
    {
        *this = ExpressionConvertible<T>::asExpression(t);
    }

    Expression();
    Expression(const LiteralValue& literalValue);
    Expression(const BindParameter& bindParameter);
    Expression(const Column& column);
    Expression(const StatementSelect& select);

    explicit Expression(const Expressions& expressions);
    Expression(const RaiseFunction& raiseFunction);

    static Expression function(const UnsafeStringView& function);
    Expression& schema(const Schema& schema);
    Expression& invoke();
    Expression& argument(const Expression& parameter);
    Expression& arguments(const Expressions& parameters);
    Expression& invokeAll();

    Expression& distinct();

    static Expression cast(const Expression& expression);
    Expression& as(const ColumnType& columnType);

    ResultColumn as(const UnsafeStringView& alias);

    OrderingTerm asAscOrder() const;
    OrderingTerm asDescOrder() const;

    static Expression exists(const StatementSelect& select);
    static Expression notExists(const StatementSelect& select);

    static Expression case_(const Expression& expression);
    static Expression case_();
    Expression& when(const Expression& expression);
    Expression& then(const Expression& expression);
    Expression& else_(const Expression& expression);

    Expression& escape(const Expression& operand);

    static Expression windowFunction(const UnsafeStringView& function);
    Expression& filter(const Expression& condition);
    Expression& over(const WindowDef& windowDef);
    Expression& over(const UnsafeStringView& window);

protected:
    Expression asExpressionOperand() const override final;
};

} // namespace WCDB
