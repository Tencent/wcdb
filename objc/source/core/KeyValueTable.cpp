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

#include <WCDB/KeyValueTable.hpp>

namespace WCDB {

const std::string KeyValueTable::s_table("WCDBKV");
const Column KeyValueTable::s_key("key");
const Column KeyValueTable::s_value("value");

KeyValueTable::KeyValueTable(Handle *handle) : m_handle(handle)
{
    assert(m_handle != nullptr);
}

std::pair<bool, bool> KeyValueTable::isTableExists()
{
    return m_handle->Handle::isTableExists(s_table);
}

bool KeyValueTable::createTable()
{
    //CREATE TABLE IF NOT EXISTS WCDBReserved(key INTEGER PRIMARY KEY, value TEXT)
    static const StatementCreateTable s_statement =
        StatementCreateTable().createTable(s_table).ifNotExists().define(
            {ColumnDef(s_key)
                 .withType(ColumnType::Integer32)
                 .byAddingConstraint(ColumnConstraint().withPrimaryKey()),
             ColumnDef(s_value).withType(ColumnType::Text)});
    return m_handle->execute(s_statement);
}

std::pair<bool, std::string> KeyValueTable::getTextValue(const Key &key)
{
    static const StatementSelect s_statement =
        StatementSelect().select(s_value).from(s_table).where(s_key ==
                                                              BindParameter(1));
    std::pair<bool, Handle::Text> result = {false, {}};
    if (!m_handle->prepare(s_statement)) {
        return result;
    }
    bool done;
    m_handle->bindInteger32((int) key, 1);
    if (m_handle->step(done)) {
        result.first = true;
        if (!done) {
            result.second = m_handle->Handle::getText(0);
        }
    }
    m_handle->Handle::finalize();
    return result;
}

bool KeyValueTable::setTextValue(const Key &key, const std::string &value)
{
    static const StatementInsert s_statement =
        StatementInsert().insertOrReplaceInto(s_table).values(
            BindParameter::bindParameters(2));
    if (!m_handle->prepare(s_statement)) {
        return false;
    }
    m_handle->Handle::bindInteger32((int) key, 1);
    m_handle->Handle::bindText(value.c_str(), (int) value.length(), 2);
    bool result = m_handle->step();
    m_handle->Handle::finalize();
    return result;
}

} //namespace WCDB
