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

#include <WCDB/statement_create_virtual_table.hpp>

namespace WCDB {

StatementCreateVirtualTable &
StatementCreateVirtualTable::create(const std::string &table, bool ifNotExists)
{
    m_description.append("CREATE VIRTUAL TABLE ");
    if (ifNotExists) {
        m_description.append("IF NOT EXISTS ");
    }
    m_description.append(table);
    return *this;
}

StatementCreateVirtualTable &
StatementCreateVirtualTable::usingModule(const std::string &moduleName)
{
    m_description.append(" USING " + moduleName);
    return *this;
}

Statement::Type StatementCreateVirtualTable::getStatementType() const
{
    return Statement::Type::CreateVirtualTable;
}

} //namespace WCDB
