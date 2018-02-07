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

StatementCreateIndex& StatementCreateIndex::createIndex(const std::string& indexName, bool ifNotExists, bool unique)
{
    lang::CreateIndexSTMT& lang = getMutableLang();
    lang.indexName.assign(indexName);
    lang.ifNotExists = ifNotExists;
    lang.unique = unique;
    return *this;
}

StatementCreateIndex& StatementCreateIndex::withSchema(const std::string& schemaName)
{
    lang::CreateIndexSTMT& lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementCreateIndex& StatementCreateIndex::on(const std::string& tableName, const IndexedColumn& indexedColumn)
{
    lang::CreateIndexSTMT& lang = getMutableLang();
    lang.tableName.assign(tableName);
    lang.indexedColumns.append(indexedColumn.getLang());
    return *this;
}
StatementCreateIndex& StatementCreateIndex::on(const std::string& tableName, const std::list<IndexedColumn>& indexedColumns)
{
    lang::CreateIndexSTMT& lang = getMutableLang();
    lang.tableName.assign(tableName);
    for (const IndexedColumn& indexedColumn : indexedColumns) {
        lang.indexedColumns.append(indexedColumn.getLang());
    }
    return *this;
}
StatementCreateIndex& StatementCreateIndex::where(const Expression& condition)
{
    lang::CreateIndexSTMT& lang = getMutableLang();
    lang.expr.assign(condition.getLang());
    return *this;
}

Statement::Type StatementCreateIndex::getType() const
{
    return Statement::Type::CreateIndex; 
}

} // namespace WCDB
