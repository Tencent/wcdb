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
#include <WCDB/HandleStatement.hpp>
#include <WCDB/SQLite.h>

namespace WCDB {

HandleStatement::HandleStatement(Handle *handle)
: HandleRelated(handle), m_stmt(nullptr)
{
}

bool HandleStatement::prepare(const Statement &statement)
{
    WCTInnerAssert(!isPrepared());
#warning TODO
    //    m_statement = statement;
    //    const std::string &sql = m_statement.getDescription();
    //    int rc = sqlite3_prepare_v2(
    //    (sqlite3 *) getRawHandle(), sql.c_str(), -1, (sqlite3_stmt **) &m_stmt, nullptr);
    //    if (rc == SQLITE_OK) {
    //        return true;
    //    }
    //    setError(rc, sql);
    return false;
}

bool HandleStatement::isPrepared() const
{
    return m_stmt != nullptr;
}

void HandleStatement::reset()
{
    WCTInnerAssert(isPrepared());
    sqlite3_reset((sqlite3_stmt *) m_stmt);
}

bool HandleStatement::step(bool &done)
{
    WCTInnerAssert(isPrepared());
    int rc = sqlite3_step((sqlite3_stmt *) m_stmt);
    done = rc == SQLITE_DONE;
    if (rc == SQLITE_OK || rc == SQLITE_ROW || rc == SQLITE_DONE) {
        return true;
    }
#warning TODO
    //    setError(rc, m_statement.getDescription());
    return false;
}

bool HandleStatement::step()
{
    WCTInnerAssert(isPrepared());
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
    WCTInnerAssert(isPrepared());
    return sqlite3_column_count((sqlite3_stmt *) m_stmt);
}

const char *HandleStatement::getColumnName(int index)
{
    WCTInnerAssert(isPrepared());
    return sqlite3_column_name((sqlite3_stmt *) m_stmt, index);
}

const char *HandleStatement::getColumnTableName(int index)
{
    WCTInnerAssert(isPrepared());
    return sqlite3_column_table_name((sqlite3_stmt *) m_stmt, index);
}

ColumnType HandleStatement::getType(int index)
{
    WCTInnerAssert(isPrepared());
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
    WCTInnerAssert(isPrepared());
    sqlite3_bind_int((sqlite3_stmt *) m_stmt, index, value);
}

void HandleStatement::bindInteger64(const Integer64 &value, int index)
{
    WCTInnerAssert(isPrepared());
    sqlite3_bind_int64((sqlite3_stmt *) m_stmt, index, value);
}

void HandleStatement::bindDouble(const Float &value, int index)
{
    WCTInnerAssert(isPrepared());
    sqlite3_bind_double((sqlite3_stmt *) m_stmt, index, value);
}

void HandleStatement::bindText(const Text &value, int index)
{
    bindText(value, -1, index);
}

void HandleStatement::bindText(const Text &value, int length, int index)
{
    WCTInnerAssert(isPrepared());
    sqlite3_bind_text((sqlite3_stmt *) m_stmt, index, value, length, SQLITE_TRANSIENT);
}

void HandleStatement::bindBLOB(const BLOB &value, int index)
{
    WCTInnerAssert(isPrepared());
    sqlite3_bind_blob(
    (sqlite3_stmt *) m_stmt, index, value.buffer(), (int) value.size(), SQLITE_TRANSIENT);
}

void HandleStatement::bindNull(int index)
{
    WCTInnerAssert(isPrepared());
    sqlite3_bind_null((sqlite3_stmt *) m_stmt, index);
}

HandleStatement::Integer32 HandleStatement::getInteger32(int index)
{
    WCTInnerAssert(isPrepared());
    return static_cast<Integer32>(sqlite3_column_int((sqlite3_stmt *) m_stmt, index));
}

HandleStatement::Integer64 HandleStatement::getInteger64(int index)
{
    WCTInnerAssert(isPrepared());
    return static_cast<Integer64>(sqlite3_column_int64((sqlite3_stmt *) m_stmt, index));
}

HandleStatement::Float HandleStatement::getDouble(int index)
{
    WCTInnerAssert(isPrepared());
    return static_cast<Float>(sqlite3_column_double((sqlite3_stmt *) m_stmt, index));
}

HandleStatement::Text HandleStatement::getText(int index)
{
    WCTInnerAssert(isPrepared());
    Text text
    = reinterpret_cast<Text>(sqlite3_column_text((sqlite3_stmt *) m_stmt, index));
    return text ? text : "";
}

const HandleStatement::BLOB HandleStatement::getBLOB(int index)
{
    WCTInnerAssert(isPrepared());
    int size = sqlite3_column_bytes((sqlite3_stmt *) m_stmt, index);
    const unsigned char *data
    = (const unsigned char *) sqlite3_column_blob((sqlite3_stmt *) m_stmt, index);
    return BLOB::immutable(data, size);
}

bool HandleStatement::isReadonly()
{
    WCTInnerAssert(isPrepared());
    return sqlite3_stmt_readonly((sqlite3_stmt *) m_stmt);
}

bool HandleStatement::isPrepared()
{
    return m_stmt != nullptr;
}

} //namespace WCDB
