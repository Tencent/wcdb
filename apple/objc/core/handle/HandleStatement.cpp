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

#include <WCDB/AbstractHandle.hpp>
#include <WCDB/Assertion.hpp>
#include <WCDB/HandleStatement.hpp>
#include <WCDB/SQLite.h>

namespace WCDB {

HandleStatement::HandleStatement(AbstractHandle *handle, HandleStatementEvent *event)
: HandleRelated(handle), m_stmt(nullptr), m_event(event), m_done(false)
{
}

HandleStatement::~HandleStatement()
{
    finalize();
}

bool HandleStatement::prepare(const Statement &statement)
{
    return prepare(statement.getDescription());
}

bool HandleStatement::prepare(const String &sql)
{
    WCTRemedialAssert(!isPrepared(), "Last statement is not finalized.", finalize(););

    bool result = exitAPI(
    sqlite3_prepare_v2(getRawHandle(), sql.c_str(), -1, &m_stmt, nullptr), sql);
    m_done = false;
    if (!result) {
        m_stmt = nullptr;
    }
    return result;
}

void HandleStatement::reset()
{
    WCTInnerAssert(isPrepared());
    exitAPI(sqlite3_reset(m_stmt));
}

bool HandleStatement::done()
{
    return m_done;
}

bool HandleStatement::step()
{
    WCTInnerAssert(isPrepared());

    if (m_event != nullptr) {
        m_event->statementWillStep(this);
    }
    int rc = sqlite3_step(m_stmt);
    m_done = rc == SQLITE_DONE;
    const char *sql = nullptr;
    if (isPrepared()) {
        // There will be privacy issues if use sqlite3_expanded_sql
        sql = sqlite3_sql(m_stmt);
    }
    bool succeed = exitAPI(rc, sql);
    if (m_event != nullptr) {
        m_event->statementDidStep(this, succeed);
    }
    return succeed;
}

void HandleStatement::finalize()
{
    if (m_stmt != nullptr) {
        // no need to call exitAPI since it will return old code only.
        sqlite3_finalize(m_stmt);
        m_stmt = nullptr;
    }
}

int HandleStatement::getNumberOfColumns()
{
    WCTInnerAssert(isPrepared());
    return sqlite3_column_count(m_stmt);
}

const UnsafeString HandleStatement::getOriginColumnName(int index)
{
    WCTInnerAssert(isPrepared());
    return sqlite3_column_origin_name(m_stmt, index);
}

const UnsafeString HandleStatement::getColumnName(int index)
{
    WCTInnerAssert(isPrepared());
    return sqlite3_column_name(m_stmt, index);
}

const UnsafeString HandleStatement::getColumnTableName(int index)
{
    WCTInnerAssert(isPrepared());
    return sqlite3_column_table_name(m_stmt, index);
}

ColumnType HandleStatement::getType(int index)
{
    WCTInnerAssert(isPrepared());
    switch (sqlite3_column_type(m_stmt, index)) {
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
    exitAPI(sqlite3_bind_int(m_stmt, index, value));
}

void HandleStatement::bindInteger64(const Integer64 &value, int index)
{
    WCTInnerAssert(isPrepared());
    exitAPI(sqlite3_bind_int64(m_stmt, index, value));
}

void HandleStatement::bindDouble(const Float &value, int index)
{
    WCTInnerAssert(isPrepared());
    exitAPI(sqlite3_bind_double(m_stmt, index, value));
}

void HandleStatement::bindText(const Text &value, int index)
{
    WCTInnerAssert(isPrepared());
    // use SQLITE_STATIC if auto_commit?
    exitAPI(sqlite3_bind_text(
    m_stmt, index, value.cstring(), (int) value.length(), SQLITE_TRANSIENT));
}

void HandleStatement::bindBLOB(const BLOB &value, int index)
{
    WCTInnerAssert(isPrepared());
    // use SQLITE_STATIC if auto_commit?
    exitAPI(sqlite3_bind_blob(m_stmt, index, value.buffer(), (int) value.size(), SQLITE_TRANSIENT));
}

void HandleStatement::bindNull(int index)
{
    WCTInnerAssert(isPrepared());
    exitAPI(sqlite3_bind_null(m_stmt, index));
}

HandleStatement::Integer32 HandleStatement::getInteger32(int index)
{
    WCTInnerAssert(isPrepared());
    return static_cast<Integer32>(sqlite3_column_int(m_stmt, index));
}

HandleStatement::Integer64 HandleStatement::getInteger64(int index)
{
    WCTInnerAssert(isPrepared());
    return static_cast<Integer64>(sqlite3_column_int64(m_stmt, index));
}

HandleStatement::Float HandleStatement::getDouble(int index)
{
    WCTInnerAssert(isPrepared());
    return static_cast<Float>(sqlite3_column_double(m_stmt, index));
}

HandleStatement::Text HandleStatement::getText(int index)
{
    WCTInnerAssert(isPrepared());
    return UnsafeString(reinterpret_cast<const char *>(sqlite3_column_text(m_stmt, index)),
                        sqlite3_column_bytes(m_stmt, index));
}

const HandleStatement::BLOB HandleStatement::getBLOB(int index)
{
    WCTInnerAssert(isPrepared());
    return BLOB::immutable(
    reinterpret_cast<const unsigned char *>(sqlite3_column_blob(m_stmt, index)),
    sqlite3_column_bytes(m_stmt, index));
}

bool HandleStatement::isReadonly()
{
    WCTInnerAssert(isPrepared());
    return sqlite3_stmt_readonly(m_stmt) != 0;
}

bool HandleStatement::isPrepared()
{
    return m_stmt != nullptr;
}

} //namespace WCDB
