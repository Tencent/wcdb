//
// Created by sanhuazhang on 2019/05/19
//

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

HandleStatement::HandleStatement(HandleStatement &&other)
: HandleRelated(other.getHandle()), m_stmt(other.m_stmt), m_done(other.m_done)
{
    other.m_done = false;
    other.m_stmt = nullptr;
}

HandleStatement::HandleStatement(AbstractHandle *handle)
: HandleRelated(handle), m_stmt(nullptr), m_done(false)
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

bool HandleStatement::prepare(const UnsafeStringView &sql)
{
    WCTRemedialAssert(!isPrepared(), "Last statement is not finalized.", finalize(););

    bool result = APIExit(
    sqlite3_prepare_v2(getRawHandle(), sql.data(), -1, &m_stmt, nullptr), sql);
    m_done = false;
    if (!result) {
        m_stmt = nullptr;
    }
    return result;
}

void HandleStatement::reset()
{
    WCTAssert(isPrepared());
    APIExit(sqlite3_reset(m_stmt));
}

bool HandleStatement::done()
{
    return m_done;
}

bool HandleStatement::step()
{
    WCTAssert(isPrepared());

    int rc = sqlite3_step(m_stmt);
    m_done = rc == SQLITE_DONE;
    const char *sql = nullptr;
    if (isPrepared()) {
        // There will be privacy issues if use sqlite3_expanded_sql
        sql = sqlite3_sql(m_stmt);
    }
    return APIExit(rc, sql);
}

void HandleStatement::finalize()
{
    if (m_stmt != nullptr) {
        // no need to call APIExit since it returns old code only.
        sqlite3_finalize(m_stmt);
        m_stmt = nullptr;
    }
}

int HandleStatement::getNumberOfColumns()
{
    WCTAssert(isPrepared());
    return sqlite3_column_count(m_stmt);
}

const UnsafeStringView HandleStatement::getOriginColumnName(int index)
{
    WCTAssert(isPrepared());
    return sqlite3_column_origin_name(m_stmt, index);
}

const UnsafeStringView HandleStatement::getColumnName(int index)
{
    WCTAssert(isPrepared());
    return sqlite3_column_name(m_stmt, index);
}

const UnsafeStringView HandleStatement::getColumnTableName(int index)
{
    WCTAssert(isPrepared());
    return sqlite3_column_table_name(m_stmt, index);
}

ColumnType HandleStatement::getType(int index)
{
    WCTAssert(isPrepared());
    switch (sqlite3_column_type(m_stmt, index)) {
    case SQLITE_INTEGER:
        return ColumnType::Integer;
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

void HandleStatement::bindInteger(const Integer &value, int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    bool succeed = APIExit(sqlite3_bind_int64(m_stmt, index, value));
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindDouble(const Float &value, int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    bool succeed = APIExit(sqlite3_bind_double(m_stmt, index, value));
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindText(const Text &value, int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    // use SQLITE_STATIC if auto_commit?
    bool succeed = APIExit(sqlite3_bind_text(
    m_stmt, index, value.data(), (int) value.length(), SQLITE_TRANSIENT));
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindBLOB(const BLOB &value, int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    // TODO: use SQLITE_STATIC to get better performance?
    bool succeed = APIExit(sqlite3_bind_blob(
    m_stmt, index, value.buffer(), (int) value.size(), SQLITE_TRANSIENT));
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindNull(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    bool succeed = APIExit(sqlite3_bind_null(m_stmt, index));
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

void HandleStatement::bindPointer(void *ptr, int index, const Text &type, void(*destructor)(void*))
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    bool succeed = APIExit(sqlite3_bind_pointer(m_stmt, index, ptr, type.data(), destructor));
    WCTAssert(succeed);
    WCDB_UNUSED(succeed);
}

int HandleStatement::bindParameterIndex(const Text &parameterName)
{
    WCTAssert(isPrepared());
    WCTAssert(!isBusy());
    int index = sqlite3_bind_parameter_index(m_stmt, parameterName.data());
    return index;
}

HandleStatement::Integer HandleStatement::getInteger(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return static_cast<Integer>(sqlite3_column_int64(m_stmt, index));
}

HandleStatement::Float HandleStatement::getDouble(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return static_cast<Float>(sqlite3_column_double(m_stmt, index));
}

HandleStatement::Text HandleStatement::getText(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return UnsafeStringView(
    reinterpret_cast<const char *>(sqlite3_column_text(m_stmt, index)),
    sqlite3_column_bytes(m_stmt, index));
}

const HandleStatement::BLOB HandleStatement::getBLOB(int index)
{
    WCTAssert(isPrepared());
    WCTAssert(isBusy());
    return BLOB::immutable(
    reinterpret_cast<const unsigned char *>(sqlite3_column_blob(m_stmt, index)),
    sqlite3_column_bytes(m_stmt, index));
}

bool HandleStatement::isBusy()
{
    WCTAssert(isPrepared());
    return sqlite3_stmt_busy(m_stmt) != 0;
}

bool HandleStatement::isReadonly()
{
    WCTAssert(isPrepared());
    return sqlite3_stmt_readonly(m_stmt) != 0;
}

bool HandleStatement::isPrepared()
{
    return m_stmt != nullptr;
}

} //namespace WCDB
