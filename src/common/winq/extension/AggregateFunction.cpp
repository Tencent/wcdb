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

#include "WINQ.h"

namespace WCDB {

Expression AggregateFunction::avg(const Expression& expression)
{
    static const char* funcName = "avg";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression AggregateFunction::count(const Expression& expression)
{
    static const char* funcName = "count";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression AggregateFunction::groupConcat(const Expression& expression)
{
    static const char* funcName = "group_concat";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression
AggregateFunction::groupConcat(const Expression& expression, const Expression& seperator)
{
    static const char* funcName = "group_concat";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments({ expression, seperator });
}

Expression AggregateFunction::max(const Expression& expression)
{
    static const char* funcName = "max";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression AggregateFunction::min(const Expression& expression)
{
    static const char* funcName = "min";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression AggregateFunction::sum(const Expression& expression)
{
    static const char* funcName = "sum";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression AggregateFunction::total(const Expression& expression)
{
    static const char* funcName = "total";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

AggregateFunctionOperable::~AggregateFunctionOperable() = default;

Expression AggregateFunctionOperable::avg() const
{
    return AggregateFunction::avg(asExpressionOperand());
}

Expression AggregateFunctionOperable::count() const
{
    return AggregateFunction::count(asExpressionOperand());
}

Expression AggregateFunctionOperable::groupConcat() const
{
    return AggregateFunction::groupConcat(asExpressionOperand());
}

Expression AggregateFunctionOperable::groupConcat(const Expression& seperator) const
{
    return AggregateFunction::groupConcat(asExpressionOperand(), seperator);
}

Expression AggregateFunctionOperable::max() const
{
    return AggregateFunction::max(asExpressionOperand());
}

Expression AggregateFunctionOperable::min() const
{
    return AggregateFunction::min(asExpressionOperand());
}

Expression AggregateFunctionOperable::sum() const
{
    return AggregateFunction::sum(asExpressionOperand());
}

Expression AggregateFunctionOperable::total() const
{
    return AggregateFunction::total(asExpressionOperand());
}

} // namespace WCDB
