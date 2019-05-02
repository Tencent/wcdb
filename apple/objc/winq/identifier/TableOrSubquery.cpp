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

TableOrSubquery::TableOrSubquery(const UnsafeString& table)
{
    syntax().switcher = SyntaxType::Switch::Table;
    syntax().tableOrFunction = table;
}

TableOrSubquery& TableOrSubquery::schema(const Schema& schema)
{
    syntax().schema = schema;
    return *this;
}

TableOrSubquery& TableOrSubquery::as(const String& alias)
{
    syntax().alias = alias;
    return *this;
}

TableOrSubquery& TableOrSubquery::notIndexed()
{
    syntax().indexType = SyntaxType::IndexType::NotIndexed;
    return *this;
}

TableOrSubquery& TableOrSubquery::indexed(const String& index)
{
    syntax().indexType = SyntaxType::IndexType::Indexed;
    syntax().index = index;
    return *this;
}

TableOrSubquery TableOrSubquery::function(const String& function)
{
    TableOrSubquery tableOrSubquery;
    tableOrSubquery.syntax().switcher = SyntaxType::Switch::Function;
    tableOrSubquery.syntax().tableOrFunction = function;
    return tableOrSubquery;
}

TableOrSubquery& TableOrSubquery::invoke()
{
    return *this;
}

TableOrSubquery& TableOrSubquery::argument(const Expression& expression)
{
    syntax().expressions.push_back(expression);
    return *this;
}

TableOrSubquery& TableOrSubquery::arguments(const Expressions& expressions)
{
    syntax().expressions = expressions;
    return *this;
}

TableOrSubquery::TableOrSubquery(const TablesOrSubqueries& tableOrSubqueries)
{
    syntax().switcher = SyntaxType::Switch::TableOrSubqueries;
    syntax().tableOrSubqueries = tableOrSubqueries;
}

TableOrSubquery::TableOrSubquery(const Join& join)
{
    syntax().switcher = SyntaxType::Switch::JoinClause;
    syntax().joinClause = join;
}

TableOrSubquery::TableOrSubquery(const StatementSelect& select)
{
    syntax().switcher = SyntaxType::Switch::Select;
    syntax().select = select;
}

TableOrSubquery TableOrSubquery::master()
{
    return WCDB::Syntax::masterTable;
}

} // namespace WCDB
