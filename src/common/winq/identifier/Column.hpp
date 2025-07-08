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
#include "OrderingTerm.hpp"
#include "SQL.hpp"

namespace WCDB {

class WCDB_API Column : public SpecifiedSyntax<Syntax::Column, SQL>,
                        public ExpressionUnaryOperable,
                        public ExpressionBinaryOperable,
                        public ExpressionInOperable,
                        public ExpressionBetweenOperable,
                        public ExpressionCollateOperable,
                        public CoreFunctionOperable,
                        public AggregateFunctionOperable,
                        public FTSFunctionOperable {
public:
    template<typename T, typename Enable = typename std::enable_if<ColumnConvertible<T>::value>::type>
    Column(const T& t)
    {
        *this = ColumnConvertible<T>::asColumn(t);
    }
    Column();
    Column(const UnsafeStringView& name);
    Column(const UnsafeStringView& name, const BaseBinding* binding);
    virtual ~Column() override;

    Column& table(const UnsafeStringView& table);
    Column& schema(const Schema& schema);

    ResultColumn as(const UnsafeStringView& alias);

    IndexedColumn asIndex() const;
    IndexedColumn asIndex(const Order& order) const;
    IndexedColumn asAscIndex() const;
    IndexedColumn asDescIndex() const;

    OrderingTerm asOrder() const;
    OrderingTerm asOrder(const Order& order) const;
    OrderingTerm asAscOrder() const;
    OrderingTerm asDescOrder() const;

    static Column rowid();
    static Column all();

protected:
    Expression asExpressionOperand() const override final;
};

} // namespace WCDB
