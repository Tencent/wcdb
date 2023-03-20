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

StatementInsert::StatementInsert() = default;

StatementInsert::~StatementInsert() = default;

StatementInsert& StatementInsert::with(const CommonTableExpressions& commonTableExpressions)
{
    syntax().commonTableExpressions = commonTableExpressions;
    return *this;
}

StatementInsert& StatementInsert::recursive()
{
    syntax().recursive = true;
    return *this;
}

StatementInsert& StatementInsert::insertIntoTable(const UnsafeStringView& table)
{
    syntax().table = table;
    return *this;
}

StatementInsert& StatementInsert::schema(const Schema& schema)
{
    syntax().schema = schema;
    return *this;
}

StatementInsert& StatementInsert::orReplace()
{
    syntax().conflictAction = Syntax::ConflictAction::Replace;
    return *this;
}

StatementInsert& StatementInsert::orRollback()
{
    syntax().conflictAction = Syntax::ConflictAction::Rollback;
    return *this;
}

StatementInsert& StatementInsert::orAbort()
{
    syntax().conflictAction = Syntax::ConflictAction::Abort;
    return *this;
}

StatementInsert& StatementInsert::orFail()
{
    syntax().conflictAction = Syntax::ConflictAction::Fail;
    return *this;
}

StatementInsert& StatementInsert::orIgnore()
{
    syntax().conflictAction = Syntax::ConflictAction::Ignore;
    return *this;
}

StatementInsert& StatementInsert::as(const UnsafeStringView& alias)
{
    syntax().alias = alias;
    return *this;
}

StatementInsert& StatementInsert::column(const Column& column)
{
    syntax().columns.push_back(column);
    return *this;
}

StatementInsert& StatementInsert::columns(const Columns& columns)
{
    syntax().columns = columns;
    return *this;
}

StatementInsert& StatementInsert::value(const Expression& expression)
{
    syntax().switcher = SyntaxType::Switch::Values;
    if (syntax().expressionsValues.empty()) {
        syntax().expressionsValues.push_back({});
    }
    syntax().expressionsValues.begin()->push_back(expression);
    return *this;
}

StatementInsert& StatementInsert::values(const Expressions& expressions)
{
    syntax().switcher = SyntaxType::Switch::Values;
    syntax().expressionsValues.push_back(expressions);
    return *this;
}

StatementInsert& StatementInsert::values(const StatementSelect& select)
{
    syntax().switcher = SyntaxType::Switch::Select;
    syntax().select = select;
    return *this;
}

StatementInsert& StatementInsert::defaultValues()
{
    syntax().switcher = SyntaxType::Switch::Default;
    return *this;
}

StatementInsert& StatementInsert::upsert(const Upsert& upsert)
{
    syntax().upsertClause = upsert;
    return *this;
}

} // namespace WCDB
