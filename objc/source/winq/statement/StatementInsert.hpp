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

#ifndef _WCDB_STATEMENTINSERT_HPP
#define _WCDB_STATEMENTINSERT_HPP

#include <WCDB/Statement.hpp>

namespace WCDB {

class StatementInsert final : public TypedSyntax<Syntax::InsertSTMT, Statement> {
public:
    using TypedSyntax<Syntax::InsertSTMT, Statement>::TypedSyntax;

    StatementInsert& with(const With& with);
    StatementInsert& insertIntoTable(const String& table);
    StatementInsert& schema(const Schema& schema);

    StatementInsert& orReplace();
    StatementInsert& orRollback();
    StatementInsert& orAbort();
    StatementInsert& orFail();
    StatementInsert& orIgnore();

    StatementInsert& as(const String& alias);

    StatementInsert& column(const Column& column);
    StatementInsert& columns(const Columns& columns);

    StatementInsert& value(const Expression& expression);
    StatementInsert& values(const Expressions& expressions);
    StatementInsert& values(const StatementSelect& select);
    StatementInsert& defaultValues();

    StatementInsert& upsert(const Upsert& upsert);
};

} // namespace WCDB

#endif /* _WCDB_STATEMENTINSERT_HPP */
