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

#include <WCDB/Core.h>

namespace WCDB {

KeyValueTable::KeyValueTable(Handle *handle) : m_handle(handle)
{
    WCTInnerAssert(m_handle != nullptr);
}

std::pair<bool, bool> KeyValueTable::isTableExists()
{
    return m_handle->Handle::isTableExists(KeyValueTable::getTable());
}

bool KeyValueTable::createTable()
{
    //CREATE TABLE IF NOT EXISTS WCDBReserved(key INTEGER PRIMARY KEY, value TEXT)
    static const StatementCreateTable s_statement =
        StatementCreateTable()
            .createTable(KeyValueTable::getTable())
            .ifNotExists()
            .define(
                {ColumnDef(KeyValueTable::getKey())
                     .withType(ColumnType::Text)
                     .byAddingConstraint(ColumnConstraint().withPrimaryKey()),
                 ColumnDef(KeyValueTable::getValue())});
    return m_handle->execute(s_statement);
}

std::pair<bool, std::string> KeyValueTable::getMigratingValue()
{
    return getTextValue("WCDBMigrating");
}

bool KeyValueTable::setMigratingValue(const std::string &value)
{
    return setTextValue("WCDBMigrating", value);
}

std::pair<bool, std::string> KeyValueTable::getTextValue(const std::string &key)
{
    static const StatementSelect s_statement =
        StatementSelect()
            .select(KeyValueTable::getValue())
            .from(KeyValueTable::getTable())
            .where(KeyValueTable::getKey() == BindParameter(1));
    std::pair<bool, Handle::Text> result = {false, {}};
    if (!m_handle->prepare(s_statement)) {
        return result;
    }
    bool done;
    m_handle->bindText(key.c_str(), (int) key.length(), 1);
    if (m_handle->step(done)) {
        result.first = true;
        if (!done) {
            result.second = m_handle->Handle::getText(0);
        }
    }
    m_handle->Handle::finalize();
    return result;
}

bool KeyValueTable::setTextValue(const std::string &key,
                                 const std::string &value)
{
    static const StatementInsert s_statement =
        StatementInsert()
            .insertOrReplaceInto(KeyValueTable::getTable())
            .values(BindParameter::bindParameters(2));
    if (!m_handle->prepare(s_statement)) {
        return false;
    }
    m_handle->Handle::bindText(key.c_str(), (int) key.length(), 1);
    m_handle->Handle::bindText(value.c_str(), (int) value.length(), 2);
    bool result = m_handle->step();
    m_handle->Handle::finalize();
    return result;
}

const Column KeyValueTable::getKey()
{
    static const Column s_key("key");
    return s_key;
}

const Column KeyValueTable::getValue()
{
    static const Column s_value("value");
    return s_value;
}

const std::string KeyValueTable::getTable()
{
    static const std::string s_table("WCDBKV");
    return s_table;
}

} //namespace WCDB
