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

StatementCreateTable &StatementCreateTable::createTable(
    const std::string &tableName, bool ifNotExists, bool temp)
{
    lang::CreateTableSTMT &lang = getMutableLang();
    lang.tableName.assign(tableName);
    lang.ifNotExists = ifNotExists;
    lang.temp = temp;
    return *this;
}

StatementCreateTable &
StatementCreateTable::withSchema(const std::string &schemaName)
{
    lang::CreateTableSTMT &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementCreateTable &
StatementCreateTable::as(const StatementSelect &selectSTMT)
{
    lang::CreateTableSTMT &lang = getMutableLang();
    lang.selectSTMT.assign(selectSTMT.getLang());
    return *this;
}

StatementCreateTable &StatementCreateTable::define(const ColumnDef &columnDef)
{
    lang::CreateTableSTMT &lang = getMutableLang();
    lang.columnDefs.append(columnDef.getLang());
    return *this;
}

StatementCreateTable &
StatementCreateTable::define(const std::list<ColumnDef> &columnDefs)
{
    lang::CreateTableSTMT &lang = getMutableLang();
    for (const ColumnDef &columnDef : columnDefs) {
        lang.columnDefs.append(columnDef.getLang());
    }
    return *this;
}

StatementCreateTable &
StatementCreateTable::addTableConstraint(const TableConstraint &tableConstraint)
{
    lang::CreateTableSTMT &lang = getMutableLang();
    lang.tableConstraints.append(tableConstraint.getLang());
    return *this;
}

StatementCreateTable &StatementCreateTable::addTableConstraints(
    const std::list<TableConstraint> &tableConstraints)
{
    lang::CreateTableSTMT &lang = getMutableLang();
    for (const TableConstraint &tableConstraint : tableConstraints) {
        lang.tableConstraints.append(tableConstraint.getLang());
    }
    return *this;
}

StatementCreateTable &StatementCreateTable::withoutRowID()
{
    lang::CreateTableSTMT &lang = getMutableLang();
    lang.withoutRowID = true;
    return *this;
}

Statement::Type StatementCreateTable::getType() const
{
    return Statement::Type::CreateTable;
}

} // namespace WCDB
