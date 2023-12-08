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

#include "FTSConst.h"
#include "WINQ.h"

namespace WCDB {

Expression FTSFunction::offsets(const Expression &expression)
{
    static const char *funcName = "offsets";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression FTSFunction::fts3Snippet(const Expression &expression,
                                    const UnsafeStringView &startMatchText,
                                    const UnsafeStringView &endMatchText,
                                    const UnsafeStringView &ellipses,
                                    int columnNumber,
                                    int approximateNumberOfTokens)
{
    static const char *funcName = "snippet";
    return Expression::function(StringView::makeConstant(funcName))
    .invoke()
    .arguments({ expression, startMatchText, endMatchText, ellipses, columnNumber, approximateNumberOfTokens });
}

Expression FTSFunction::matchinfo(const Expression &expression)
{
    static const char *funcName = "matchinfo";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression FTSFunction::bm25(const Expression &expression)
{
    static const char *funcName = "bm25";
    return Expression::function(StringView::makeConstant(funcName)).invoke().arguments(expression);
}

Expression FTSFunction::highlight(const Expression &expression,
                                  int columnNumber,
                                  const UnsafeStringView &startMatchText,
                                  const UnsafeStringView &endMatchText)
{
    static const char *funcName = "highlight";
    return Expression::function(StringView::makeConstant(funcName))
    .invoke()
    .arguments({ expression, columnNumber, startMatchText, endMatchText });
}

Expression FTSFunction::fts5Snippet(const Expression &expression,
                                    int columnNumber,
                                    const UnsafeStringView &startMatchText,
                                    const UnsafeStringView &endMatchText,
                                    const UnsafeStringView &ellipses,
                                    int approximateNumberOfTokens)
{
    static const char *funcName = "snippet";
    return Expression::function(StringView::makeConstant(funcName))
    .invoke()
    .arguments({ expression, columnNumber, startMatchText, endMatchText, ellipses, approximateNumberOfTokens });
}

Expression FTSFunction::substringMatchInfo(const Expression &expression,
                                           int columnNumber,
                                           const UnsafeStringView &separators)
{
    return Expression::function(StringView::makeConstant(BuiltinAuxiliaryFunction::SubstringMatchInfo))
    .invoke()
    .arguments({ expression, columnNumber, separators });
}

FTSFunctionOperable::~FTSFunctionOperable() = default;

Expression FTSFunctionOperable::offsets() const
{
    return FTSFunction::offsets(asExpressionOperand());
}

Expression FTSFunctionOperable::fts3Snippet(const UnsafeStringView &startMatchText,
                                            const UnsafeStringView &endMatchText,
                                            const UnsafeStringView &ellipses,
                                            int columnNumber,
                                            int approximateNumberOfTokens) const
{
    return FTSFunction::fts3Snippet(
    asExpressionOperand(), startMatchText, endMatchText, ellipses, columnNumber, approximateNumberOfTokens);
}

Expression FTSFunctionOperable::matchinfo() const
{
    return FTSFunction::matchinfo(asExpressionOperand());
}

Expression FTSFunctionOperable::bm25() const
{
    return FTSFunction::bm25(asExpressionOperand());
}

Expression FTSFunctionOperable::highlight(int columnNumber,
                                          const UnsafeStringView &startMatchText,
                                          const UnsafeStringView &endMatchText) const
{
    return FTSFunction::highlight(
    asExpressionOperand(), columnNumber, startMatchText, endMatchText);
}

Expression FTSFunctionOperable::fts5Snippet(int columnNumber,
                                            const UnsafeStringView &startMatchText,
                                            const UnsafeStringView &endMatchText,
                                            const UnsafeStringView &ellipses,
                                            int approximateNumberOfTokens) const
{
    return FTSFunction::fts5Snippet(
    asExpressionOperand(), columnNumber, startMatchText, endMatchText, ellipses, approximateNumberOfTokens);
}

Expression FTSFunctionOperable::substringMatchInfo(int columnNumber,
                                                   const UnsafeStringView &separators) const
{
    return FTSFunction::substringMatchInfo(asExpressionOperand(), columnNumber, separators);
}

} // namespace WCDB
