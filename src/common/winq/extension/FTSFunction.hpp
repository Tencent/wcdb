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

class WCDB_API FTSFunction final {
public:
    FTSFunction() = delete;
    FTSFunction(const FTSFunction &) = delete;
    FTSFunction &operator=(const FTSFunction &) = delete;

    // For FTS3/4
    static Expression offsets(const Expression &expression);
    static Expression fts3Snippet(const Expression &expression,
                                  const UnsafeStringView &startMatchText = "<b>",
                                  const UnsafeStringView &endMatchText = "</b>",
                                  const UnsafeStringView &ellipses = "<b>...</b>",
                                  int columnNumber = -1,
                                  int approximateNumberOfTokens = -15);
    static Expression matchinfo(const Expression &expression);

    // For FTS5
    static Expression bm25(const Expression &expression);
    static Expression highlight(const Expression &expression,
                                int columnNumber = 0,
                                const UnsafeStringView &startMatchText = "<b>",
                                const UnsafeStringView &endMatchText = "</b>");
    static Expression fts5Snippet(const Expression &expression,
                                  int columnNumber = 0,
                                  const UnsafeStringView &startMatchText = "<b>",
                                  const UnsafeStringView &endMatchText = "</b>",
                                  const UnsafeStringView &ellipses = "<b>...</b>",
                                  int approximateNumberOfTokens = 64);
    static Expression substringMatchInfo(const Expression &expression,
                                         int columnNumber,
                                         const UnsafeStringView &separators);
};

class WCDB_API FTSFunctionOperable : virtual public ExpressionOperable {
public:
    virtual ~FTSFunctionOperable() override = 0;

    // For FTS3/4
    Expression offsets() const;
    Expression fts3Snippet(const UnsafeStringView &startMatchText = "<b>",
                           const UnsafeStringView &endMatchText = "</b>",
                           const UnsafeStringView &ellipses = "<b>...</b>",
                           int columnNumber = -1,
                           int approximateNumberOfTokens = -15) const;
    Expression matchinfo() const;

    // For FTS5
    Expression bm25() const;
    Expression highlight(int columnNumber = 0,
                         const UnsafeStringView &startMatchText = "<b>",
                         const UnsafeStringView &endMatchText = "</b>") const;
    Expression fts5Snippet(int columnNumber = 0,
                           const UnsafeStringView &startMatchText = "<b>",
                           const UnsafeStringView &endMatchText = "</b>",
                           const UnsafeStringView &ellipses = "<b>...</b>",
                           int approximateNumberOfTokens = 64) const;
    Expression substringMatchInfo(int columnNumber, const UnsafeStringView &separators) const;
};

} // namespace WCDB
