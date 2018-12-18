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

#ifndef _WCDB_STATEMENTANALYZE_HPP
#define _WCDB_STATEMENTANALYZE_HPP

#include <WCDB/Statement.hpp>

namespace WCDB {

class StatementAnalyze final : public TypedSyntax<Syntax::AnalyzeSTMT, Statement> {
public:
    using TypedSyntax<Syntax::AnalyzeSTMT, Statement>::TypedSyntax;

    StatementAnalyze& analyze();
    StatementAnalyze& schema(const Schema& schema);
    StatementAnalyze& table(const String& table);
    StatementAnalyze& index(const String& index);
};

} // namespace WCDB

#endif /* _WCDB_STATEMENTANALYZE_HPP */
