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

#ifndef FTS3Function_hpp
#define FTS3Function_hpp

#include <WCDB/ExpressionOperable.hpp>
#include <WCDB/SQL.hpp>

namespace WCDB {

class FTS3Function {
public:
    FTS3Function() = delete;
    FTS3Function(const FTS3Function &) = delete;
    FTS3Function &operator=(const FTS3Function &) = delete;

    static Expression offsets(const Expression &expression);
    static Expression snippet(const Expression &expression,
                              const std::string &startMatchText = "<b>",
                              const std::string &endMatchText = "</b>",
                              const std::string &ellipses = "<b>...</b>",
                              int columnNumber = -1,
                              int approximateNumberOfTokens = -15);
    static Expression matchinfo(const Expression &expression);
};

class FTS3FunctionOperable : virtual public ExpressionOperable {
public:
    virtual ~FTS3FunctionOperable();
    Expression offsets() const;
    Expression snippet(const std::string &startMatchText = "<b>",
                       const std::string &endMatchText = "</b>",
                       const std::string &ellipses = "<b>...</b>",
                       int columnNumber = -1,
                       int approximateNumberOfTokens = -15) const;
    Expression matchinfo() const;
};

} // namespace WCDB

#endif /* FTS3Function_hpp */
