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

#ifndef StatementInsert_hpp
#define StatementInsert_hpp

#include <WCDB/Statement.hpp>

namespace WCDB {

class StatementInsert : public StatementSyntax<Syntax::InsertSTMT> {
public:
    StatementInsert& with(const With& with);
    StatementInsert& insertIntoTable(const SyntaxString& table);
    StatementInsert& insertIntoTable(const Schema& schema, const SyntaxString& table);

    StatementInsert& orReplace();
    StatementInsert& orRollback();
    StatementInsert& orAbort();
    StatementInsert& orFail();
    StatementInsert& orIgnore();

    StatementInsert& as(const SyntaxString& alias);

    StatementInsert& columns(const Columns& columns);

    StatementInsert& values(const Expressions& expressions);
    StatementInsert& values(const StatementSelect& select);
    StatementInsert& defaultValues();

    StatementInsert& upsert(const Upsert& upsert);
};

} // namespace WCDB

#endif /* StatementInsert_hpp */
