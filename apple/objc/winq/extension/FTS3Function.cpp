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

#include <WCDB/WINQ.h>

namespace WCDB {

Expression FTS3Function::offsets(const Expression &expression)
{
    return Expression::function("offsets").invoke().arguments(expression);
}

Expression FTS3Function::snippet(const Expression &expression,
                                 const UnsafeStringView &startMatchText,
                                 const UnsafeStringView &endMatchText,
                                 const UnsafeStringView &ellipses,
                                 int columnNumber,
                                 int approximateNumberOfTokens)
{
    return Expression::function("snippet").invoke().arguments(
    { expression, startMatchText, endMatchText, ellipses, columnNumber, approximateNumberOfTokens });
}

Expression FTS3Function::matchinfo(const Expression &expression)
{
    return Expression::function("matchinfo").invoke().arguments(expression);
}

FTS3FunctionOperable::~FTS3FunctionOperable() = default;

Expression FTS3FunctionOperable::offsets() const
{
    return FTS3Function::offsets(asExpressionOperand());
}

Expression FTS3FunctionOperable::snippet(const UnsafeStringView &startMatchText,
                                         const UnsafeStringView &endMatchText,
                                         const UnsafeStringView &ellipses,
                                         int columnNumber,
                                         int approximateNumberOfTokens) const
{
    return FTS3Function::snippet(
    asExpressionOperand(), startMatchText, endMatchText, ellipses, columnNumber, approximateNumberOfTokens);
}

Expression FTS3FunctionOperable::matchinfo() const
{
    return FTS3Function::matchinfo(asExpressionOperand());
}

} // namespace WCDB
