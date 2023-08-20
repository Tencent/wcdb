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

#include "ExpressionOperable.hpp"
#include "SQL.hpp"

namespace WCDB {

class WCDB_API AggregateFunction final {
public:
    AggregateFunction() = delete;
    AggregateFunction(const AggregateFunction&) = delete;
    AggregateFunction& operator=(const AggregateFunction&) = delete;

    static Expression avg(const Expression& expression);
    static Expression count(const Expression& expression);
    static Expression groupConcat(const Expression& expression);
    static Expression
    groupConcat(const Expression& expression, const Expression& seperator);
    static Expression max(const Expression& expression);
    static Expression min(const Expression& expression);
    static Expression sum(const Expression& expression);
    static Expression total(const Expression& expression);
};

class WCDB_API AggregateFunctionOperable : virtual public ExpressionOperable {
public:
    virtual ~AggregateFunctionOperable() override = 0;
    Expression avg() const;
    Expression count() const;
    Expression groupConcat() const;
    Expression groupConcat(const Expression& seperator) const;
    Expression max() const;
    Expression min() const;
    Expression sum() const;
    Expression total() const;
};

} // namespace WCDB
