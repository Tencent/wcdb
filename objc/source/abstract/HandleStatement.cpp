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

#include <WCDB/HandleStatement.hpp>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

HandleStatement::HandleStatement() : m_stmt(nullptr)
{
}

void HandleStatement::setup(const Statement &statement, void *stmt)
{
    assert(stmt != nullptr);
    m_stmt = stmt;
    m_statement = statement;
}

void HandleStatement::reset()
{
    sqlite3_reset((sqlite3_stmt *) m_stmt);
}

bool HandleStatement::step(bool &done)
{
    int rc = sqlite3_step((sqlite3_stmt *) m_stmt);
    assert(rc != SQLITE_MISUSE);
    done = rc == SQLITE_DONE;
    return rc == SQLITE_OK || rc == SQLITE_ROW || rc == SQLITE_DONE;
}

bool HandleStatement::step()
{
    bool unused;
    return step(unused);
}

void HandleStatement::finalize()
{
    if (m_stmt) {
        sqlite3_finalize((sqlite3_stmt *) m_stmt);
        m_stmt = nullptr;
    }
}

int HandleStatement::getColumnCount()
{
    return sqlite3_column_count((sqlite3_stmt *) m_stmt);
}

const char *HandleStatement::getColumnName(int index)
{
    return sqlite3_column_name((sqlite3_stmt *) m_stmt, index);
}

const char *HandleStatement::getColumnTableName(int index)
{
    return sqlite3_column_table_name((sqlite3_stmt *) m_stmt, index);
}

ColumnType HandleStatement::getType(int index)
{
    switch (sqlite3_column_type((sqlite3_stmt *) m_stmt, index)) {
        case SQLITE_INTEGER:
            return ColumnType::Integer64;
        case SQLITE_FLOAT:
            return ColumnType::Float;
        case SQLITE_BLOB:
            return ColumnType::BLOB;
        case SQLITE_TEXT:
            return ColumnType::Text;
        default:
            return ColumnType::Null;
    }
}

void HandleStatement::bindInteger32(const Integer32 &value, int index)
{
    sqlite3_bind_int((sqlite3_stmt *) m_stmt, index, value);
}

void HandleStatement::bindInteger64(const Integer64 &value, int index)
{
    sqlite3_bind_int64((sqlite3_stmt *) m_stmt, index, value);
}

void HandleStatement::bindDouble(const Float &value, int index)
{
    sqlite3_bind_double((sqlite3_stmt *) m_stmt, index, value);
}

void HandleStatement::bindText(const Text &value, int index)
{
    bindText(value, -1, index);
}

void HandleStatement::bindText(const Text &value, int length, int index)
{
    sqlite3_bind_text((sqlite3_stmt *) m_stmt, index, value, length,
                      SQLITE_TRANSIENT);
}

void HandleStatement::bindBLOB(const BLOB &value, int index)
{
    sqlite3_bind_blob((sqlite3_stmt *) m_stmt, index, value.data,
                      (int) value.size, SQLITE_TRANSIENT);
}

void HandleStatement::bindNull(int index)
{
    sqlite3_bind_null((sqlite3_stmt *) m_stmt, index);
}

HandleStatement::Integer32 HandleStatement::getInteger32(int index)
{
    return static_cast<Integer32>(
        sqlite3_column_int((sqlite3_stmt *) m_stmt, index));
}

HandleStatement::Integer64 HandleStatement::getInteger64(int index)
{
    return static_cast<Integer64>(
        sqlite3_column_int64((sqlite3_stmt *) m_stmt, index));
}

HandleStatement::Float HandleStatement::getDouble(int index)
{
    return static_cast<Float>(
        sqlite3_column_double((sqlite3_stmt *) m_stmt, index));
}

HandleStatement::Text HandleStatement::getText(int index)
{
    return reinterpret_cast<Text>(
        sqlite3_column_text((sqlite3_stmt *) m_stmt, index));
}

HandleStatement::BLOB HandleStatement::getBLOB(int index)
{
    int size = sqlite3_column_bytes((sqlite3_stmt *) m_stmt, index);
    const unsigned char *data = (const unsigned char *) sqlite3_column_blob(
        (sqlite3_stmt *) m_stmt, index);
    return BLOB(data, size);
}

bool HandleStatement::isReadonly()
{
    return sqlite3_stmt_readonly((sqlite3_stmt *) m_stmt);
}

bool HandleStatement::isPrepared()
{
    return m_stmt != nullptr;
}

const char *HandleStatement::getSQL()
{
    return sqlite3_sql((sqlite3_stmt *) m_stmt);
}

const Statement &HandleStatement::getStatement()
{
    return m_statement;
}

} //namespace WCDB
