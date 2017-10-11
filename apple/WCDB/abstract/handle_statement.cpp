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

#include <WCDB/handle_statement.hpp>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

StatementHandle::StatementHandle(void *stmt, const Handle &handle)
    : m_stmt(stmt), m_handle(handle)
{
}

void StatementHandle::reset()
{
    int rc = sqlite3_reset((sqlite3_stmt *) m_stmt);
    if (rc == SQLITE_OK) {
        m_error.reset();
    } else {
        sqlite3 *handle = sqlite3_db_handle((sqlite3_stmt *) m_stmt);
        Error::ReportSQLite(m_handle.getTag(), m_handle.path,
                            Error::HandleOperation::Finalize, rc,
                            sqlite3_extended_errcode(handle),
                            sqlite3_errmsg(handle), &m_error);
    }
}

bool StatementHandle::step()
{
    int rc = sqlite3_step((sqlite3_stmt *) m_stmt);
    if (rc == SQLITE_ROW || rc == SQLITE_OK || rc == SQLITE_DONE) {
        m_error.reset();
        return rc == SQLITE_ROW;
    }
    sqlite3 *handle = sqlite3_db_handle((sqlite3_stmt *) m_stmt);
    Error::ReportSQLite(
        m_handle.getTag(), m_handle.path, Error::HandleOperation::Step, rc,
        sqlite3_extended_errcode(handle), sqlite3_errmsg(handle),
        sqlite3_sql((sqlite3_stmt *) m_stmt), &m_error);
    return false;
}

bool StatementHandle::isOK() const
{
    return m_error.isOK();
}

const Error &StatementHandle::getError() const
{
    return m_error;
}

int StatementHandle::getChanges()
{
    sqlite3 *handle = sqlite3_db_handle((sqlite3_stmt *) m_stmt);
    return sqlite3_changes((sqlite3 *) handle);
}

StatementHandle::~StatementHandle()
{
    finalize();
}

void StatementHandle::bindInteger32(
    const ColumnTypeInfo<ColumnType::Integer32>::CType &value, int index)
{
    sqlite3_bind_int((sqlite3_stmt *) m_stmt, index, value);
}

void StatementHandle::bindInteger64(
    const ColumnTypeInfo<ColumnType::Integer64>::CType &value, int index)
{
    sqlite3_bind_int64((sqlite3_stmt *) m_stmt, index, value);
}

void StatementHandle::bindDouble(
    const ColumnTypeInfo<ColumnType::Float>::CType &value, int index)
{
    sqlite3_bind_double((sqlite3_stmt *) m_stmt, index, value);
}

void StatementHandle::bindText(
    const ColumnTypeInfo<ColumnType::Text>::CType &value, int index)
{
    sqlite3_bind_text((sqlite3_stmt *) m_stmt, index, value, -1,
                      SQLITE_TRANSIENT);
}

void StatementHandle::bindBLOB(
    const ColumnTypeInfo<ColumnType::BLOB>::CType &value, int size, int index)
{
    sqlite3_bind_blob((sqlite3_stmt *) m_stmt, index, value, size,
                      SQLITE_TRANSIENT);
}

void StatementHandle::bindNull(int index)
{
    sqlite3_bind_null((sqlite3_stmt *) m_stmt, index);
}

ColumnTypeInfo<ColumnType::Integer32>::CType
StatementHandle::getInteger32(int index)
{
    return (typename ColumnTypeInfo<ColumnType::Integer32>::CType)
        sqlite3_column_int((sqlite3_stmt *) m_stmt, index);
}

ColumnTypeInfo<ColumnType::Integer64>::CType
StatementHandle::getInteger64(int index)
{
    return (typename ColumnTypeInfo<ColumnType::Integer64>::CType)
        sqlite3_column_int64((sqlite3_stmt *) m_stmt, index);
}

ColumnTypeInfo<ColumnType::Float>::CType StatementHandle::getDouble(int index)
{
    return (typename ColumnTypeInfo<ColumnType::Float>::CType)
        sqlite3_column_double((sqlite3_stmt *) m_stmt, index);
}

ColumnTypeInfo<ColumnType::Text>::CType StatementHandle::getText(int index)
{
    return (typename ColumnTypeInfo<ColumnType::Text>::CType)
        sqlite3_column_text((sqlite3_stmt *) m_stmt, index);
}

ColumnTypeInfo<ColumnType::BLOB>::CType StatementHandle::getBLOB(int index,
                                                                 int &size)
{
    size = sqlite3_column_bytes((sqlite3_stmt *) m_stmt, index);
    return (typename ColumnTypeInfo<ColumnType::BLOB>::CType)
        sqlite3_column_blob((sqlite3_stmt *) m_stmt, index);
}

int StatementHandle::getColumnCount()
{
    return sqlite3_column_count((sqlite3_stmt *) m_stmt);
}

const char *StatementHandle::getColumnName(int index)
{
    return sqlite3_column_name((sqlite3_stmt *) m_stmt, index);
}

const char *StatementHandle::getColumnTableName(int index)
{
    return sqlite3_column_table_name((sqlite3_stmt *) m_stmt, index);
}

ColumnType StatementHandle::getType(int index)
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
    }
    return ColumnType::Null;
}

long long StatementHandle::getLastInsertedRowID()
{
    return sqlite3_last_insert_rowid(
        sqlite3_db_handle((sqlite3_stmt *) m_stmt));
}

void StatementHandle::finalize()
{
    if (m_stmt) {
        sqlite3 *handle = sqlite3_db_handle((sqlite3_stmt *) m_stmt);
        int rc = sqlite3_finalize((sqlite3_stmt *) m_stmt);
        m_stmt = nullptr;
        if (rc == SQLITE_OK) {
            m_error.reset();
            return;
        }
        Error::ReportSQLite(m_handle.getTag(), m_handle.path,
                            Error::HandleOperation::Finalize, rc,
                            sqlite3_extended_errcode(handle),
                            sqlite3_errmsg(handle), &m_error);
    }
}

} //namespace WCDB
