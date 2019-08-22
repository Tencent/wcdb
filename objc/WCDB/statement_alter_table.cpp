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

#include <WCDB/column_def.hpp>
#include <WCDB/statement_alter_table.hpp>

namespace WCDB {

StatementAlterTable &StatementAlterTable::alter(const std::string &table)
{
    m_description.append("ALTER TABLE " + table);
    return *this;
}

StatementAlterTable &StatementAlterTable::rename(const std::string &newTable)
{
    m_description.append(" RENAME TO " + newTable);
    return *this;
}

StatementAlterTable &StatementAlterTable::addColumn(const ColumnDef &columnDef)
{
    m_description.append(" ADD COLUMN " + columnDef.getDescription());
    return *this;
}

Statement::Type StatementAlterTable::getStatementType() const
{
    return Statement::Type::AlterTable;
}

} //namespace WCDB
