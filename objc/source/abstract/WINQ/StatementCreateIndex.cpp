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

#include <WCDB/Assertion.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

StatementCreateIndex::StatementCreateIndex()
{
    getMutableLang().ifNotExists = true;
}

StatementCreateIndex &StatementCreateIndex::createIndex(const std::string &indexName)
{
    Lang::CreateIndexSTMT &lang = getMutableLang();
    lang.indexName.assign(indexName);
    return *this;
}

StatementCreateIndex &StatementCreateIndex::ifNotExists(bool ifNotExists)
{
    Lang::CreateIndexSTMT &lang = getMutableLang();
    lang.ifNotExists = ifNotExists;
    return *this;
}

StatementCreateIndex &StatementCreateIndex::unique(bool unique)
{
    Lang::CreateIndexSTMT &lang = getMutableLang();
    lang.unique = unique;
    return *this;
}

StatementCreateIndex &StatementCreateIndex::withSchema(const std::string &schemaName)
{
    Lang::CreateIndexSTMT &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementCreateIndex &StatementCreateIndex::on(const std::string &tableName)
{
    Lang::CreateIndexSTMT &lang = getMutableLang();
    lang.tableName.assign(tableName);
    return *this;
}

StatementCreateIndex &StatementCreateIndex::indexedBy(const IndexedColumn &indexedColumn)
{
    Lang::CreateIndexSTMT &lang = getMutableLang();
    lang.indexedColumns.append(indexedColumn.getCOWLang());
    return *this;
}

StatementCreateIndex &
StatementCreateIndex::indexedBy(const std::list<IndexedColumn> &indexedColumns)
{
    Lang::CreateIndexSTMT &lang = getMutableLang();
    for (const IndexedColumn &indexedColumn : indexedColumns) {
        lang.indexedColumns.append(indexedColumn.getCOWLang());
    }
    return *this;
}

StatementCreateIndex &StatementCreateIndex::where(const Expression &condition)
{
    Lang::CreateIndexSTMT &lang = getMutableLang();
    lang.expr.assign(condition.getCOWLang());
    return *this;
}

} // namespace WCDB
