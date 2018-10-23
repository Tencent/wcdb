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

StatementInsert& StatementInsert::with(const With& with)
{
    syntax.useWithClause = true;
    syntax.withClause = with;
    return *this;
}

StatementInsert& StatementInsert::insertIntoTable(const SyntaxString& table)
{
    syntax.schema = Schema();
    syntax.table = table;
    return *this;
}

StatementInsert&
StatementInsert::insertIntoTable(const Schema& schema, const SyntaxString& table)
{
    syntax.schema = schema;
    syntax.table = table;
    return *this;
}

StatementInsert& StatementInsert::orReplace()
{
    syntax.switcher = SyntaxType::Switch::InsertOrReplace;
    return *this;
}

StatementInsert& StatementInsert::orRollback()
{
    syntax.switcher = SyntaxType::Switch::InsertOrRollback;
    return *this;
}

StatementInsert& StatementInsert::orAbort()
{
    syntax.switcher = SyntaxType::Switch::InsertOrAbort;
    return *this;
}

StatementInsert& StatementInsert::orFail()
{
    syntax.switcher = SyntaxType::Switch::InsertOrFail;
    return *this;
}

StatementInsert& StatementInsert::orIgnore()
{
    syntax.switcher = SyntaxType::Switch::InsertOrIgnore;
    return *this;
}

StatementInsert& StatementInsert::as(const SyntaxString& alias)
{
    syntax.alias = alias;
    return *this;
}

StatementInsert& StatementInsert::columns(const Columns& columns)
{
    syntax.columns = columns;
    return *this;
}

StatementInsert& StatementInsert::values(const Expressions& expressions)
{
    syntax.valueSwitcher = SyntaxType::SwitchValue::Values;
    syntax.expressionsValues.push_back(expressions);
    return *this;
}

StatementInsert& StatementInsert::values(const StatementSelect& select)
{
    syntax.valueSwitcher = SyntaxType::SwitchValue::Select;
    syntax.select = select;
    return *this;
}

StatementInsert& StatementInsert::defaultValues()
{
    syntax.valueSwitcher = SyntaxType::SwitchValue::Default;
    return *this;
}

StatementInsert& StatementInsert::upsert(const Upsert& upsert)
{
    syntax.useUpsertClause = true;
    syntax.upsertClause = upsert;
    return *this;
}

} // namespace WCDB
