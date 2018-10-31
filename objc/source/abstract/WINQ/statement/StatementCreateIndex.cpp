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

StatementCreateIndex& StatementCreateIndex::createIndex(const String& index)
{
    syntax().index = index;
    return *this;
}

StatementCreateIndex& StatementCreateIndex::schema(const Schema& schema)
{
    syntax().schema = schema;
    return *this;
}

StatementCreateIndex& StatementCreateIndex::unique()
{
    syntax().unique = true;
    return *this;
}

StatementCreateIndex& StatementCreateIndex::ifNotExists()
{
    syntax().ifNotExists = true;
    return *this;
}

StatementCreateIndex& StatementCreateIndex::onTable(const String& table)
{
    syntax().table = table;
    return *this;
}

StatementCreateIndex& StatementCreateIndex::indexed(const IndexedColumns& indexedColumns)
{
    syntax().indexedColumns = indexedColumns;
    return *this;
}

StatementCreateIndex& StatementCreateIndex::where(const Expression& condition)
{
    syntax().useCondition = true;
    syntax().condition = condition;
    return *this;
}

} // namespace WCDB
