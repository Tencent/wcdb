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

#include <WINQ/WINQ.h>

namespace WCDB {

Statement::Type StatementAlterTable::getType() const
{
    return Statement::Type::AlterTable;
}

StatementAlterTable &
StatementAlterTable::alterTable(const std::string &schemaName,
                                const std::string &tableName)
{
    lang::AlterTableSTMT &lang = getMutableLang();
    lang.schemaName.assign(schemaName);
    lang.tableName.assign(tableName);
    return *this;
}

StatementAlterTable &
StatementAlterTable::alterTable(const std::string &tableName)
{
    lang::AlterTableSTMT &lang = getMutableLang();
    lang.schemaName.clear();
    lang.tableName.assign(tableName);
    return *this;
}

StatementAlterTable &
StatementAlterTable::renameTo(const std::string &newTableName)
{
    lang::AlterTableSTMT &lang = getMutableLang();
    lang.switcher = lang::AlterTableSTMT::Switch::Rename;
    lang.newTableName.assign(newTableName);
    return *this;
}

StatementAlterTable &StatementAlterTable::addColumn(const ColumnDef &columnDef)
{
    lang::AlterTableSTMT &lang = getMutableLang();
    lang.switcher = lang::AlterTableSTMT::Switch::AddColumn;
    lang.columnDef.assign(columnDef.getLang());
    return *this;
}

} // namespace WCDB
