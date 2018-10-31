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

#ifndef Column_hpp
#define Column_hpp

#include <WCDB/AggregateFunction.hpp>
#include <WCDB/CoreFunction.hpp>
#include <WCDB/FTS3Function.hpp>
#include <WCDB/SQL.hpp>

namespace WCDB {

class Column : public TypedSyntax<Syntax::Column, SQL>,
               public ExpressionUnaryOperable,
               public ExpressionBinaryOperable,
               public ExpressionInOperable,
               public ExpressionBetweenOperable,
               public CoreFunctionOperable,
               public AggregateFunctionOperable,
               public FTS3FunctionOperable {
public:
    Column();
    explicit Column(const String& name);

    static Column rowid();

protected:
    Expression asExpressionOperand() const override final;
};

} // namespace WCDB

#endif /* Column_hpp */
