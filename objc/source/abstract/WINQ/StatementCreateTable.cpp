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

StatementCreateTable::StatementCreateTable()
{
    getMutableLang().ifNotExists = true;
}

StatementCreateTable &StatementCreateTable::createTable(const std::string &tableName)
{
    Lang::CreateTableSTMT &lang = getMutableLang();
    lang.tableName.assign(tableName);
    return *this;
}

StatementCreateTable &StatementCreateTable::temp(bool temp)
{
    Lang::CreateTableSTMT &lang = getMutableLang();
    lang.temp = temp;
    return *this;
}

StatementCreateTable &StatementCreateTable::ifNotExists(bool ifNotExists)
{
    Lang::CreateTableSTMT &lang = getMutableLang();
    lang.ifNotExists = ifNotExists;
    return *this;
}

StatementCreateTable &StatementCreateTable::withSchema(const std::string &schemaName)
{
    Lang::CreateTableSTMT &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    return *this;
}

StatementCreateTable &StatementCreateTable::as(const StatementSelect &selectSTMT)
{
    Lang::CreateTableSTMT &lang = getMutableLang();
    lang.switcher = Lang::CreateTableSTMT::Switch::Select;
    lang.selectSTMT.assign(selectSTMT.getCOWLang());
    return *this;
}

StatementCreateTable &StatementCreateTable::define(const ColumnDef &columnDef)
{
    Lang::CreateTableSTMT &lang = getMutableLang();
    lang.switcher = Lang::CreateTableSTMT::Switch::ColumnDef;
    lang.columnDefs.append(columnDef.getCOWLang());
    return *this;
}

StatementCreateTable &StatementCreateTable::define(const std::list<ColumnDef> &columnDefs)
{
    Lang::CreateTableSTMT &lang = getMutableLang();
    lang.switcher = Lang::CreateTableSTMT::Switch::ColumnDef;
    for (const ColumnDef &columnDef : columnDefs) {
        lang.columnDefs.append(columnDef.getCOWLang());
    }
    return *this;
}

StatementCreateTable &
StatementCreateTable::addTableConstraint(const TableConstraint &tableConstraint)
{
    Lang::CreateTableSTMT &lang = getMutableLang();
    lang.tableConstraints.append(tableConstraint.getCOWLang());
    return *this;
}

StatementCreateTable &
StatementCreateTable::addTableConstraints(const std::list<TableConstraint> &tableConstraints)
{
    Lang::CreateTableSTMT &lang = getMutableLang();
    for (const TableConstraint &tableConstraint : tableConstraints) {
        lang.tableConstraints.append(tableConstraint.getCOWLang());
    }
    return *this;
}

StatementCreateTable &StatementCreateTable::withoutRowID()
{
    Lang::CreateTableSTMT &lang = getMutableLang();
    lang.withoutRowID = true;
    return *this;
}

} // namespace WCDB
