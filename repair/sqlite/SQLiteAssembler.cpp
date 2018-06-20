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
#include <WCDB/Cell.hpp>
#include <WCDB/SQLiteAssembler.hpp>
#include <WCDB/String.hpp>
#include <sqlite3.h>
#include <sstream>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
SQLiteAssembler::SQLiteAssembler()
    : Assembler()
    , m_transaction(false)
    , m_onTableAssembled(nullptr)
    , m_handle(nullptr)
    , m_cellSTMT(nullptr)
    , m_sequenceSTMT(nullptr)
{
}

void SQLiteAssembler::setNotificationWhenTableAssembled(
    const TableAssembledCallback &onTableAssembled)
{
    m_onTableAssembled = onTableAssembled;
}

bool SQLiteAssembler::onTableAssembled(const std::string &tableName)
{
    //    int rc = SQLITE_OK;
    //    if (m_onTableAssembled) {
    //        rc = m_onTableAssembled(tableName, m_handle);
    //    }
    //    if (rc == SQLITE_OK) {
    m_table = tableName;
    finalize(&m_cellSTMT);
    return true;
    //    }
    //    setError(rc);
    //    return false;
}

#pragma mark - Assemble
bool SQLiteAssembler::markAsAssembling()
{
    return open();
}

bool SQLiteAssembler::markAsAssembled()
{
    m_table.clear();
    finalize(&m_sequenceSTMT);
    finalize(&m_cellSTMT);
    bool result = lazyCommitOrRollbackTransaction();
    close();
    return result;
}

bool SQLiteAssembler::markAsMilestone()
{
    return lazyCommitOrRollbackTransaction() && lazyBeginTransaction();
}

bool SQLiteAssembler::assembleTable(const std::string &tableName,
                                    const std::string &sql,
                                    int64_t sequence)
{
    return execute(sql.c_str()) && assembleSequence(tableName, sequence) &&
           onTableAssembled(tableName);
}

bool SQLiteAssembler::assembleCell(const Cell &cell)
{
    if (!lazyPrepareCell()) {
        return false;
    }
    sqlite3_bind_int64((sqlite3_stmt *) m_cellSTMT, 1, cell.getRowID());
    for (int i = 0; i < cell.getCount(); ++i) {
        int bindIndex = i + 2;
        switch (cell.getValueType(i)) {
            case Cell::Integer:
                sqlite3_bind_int64((sqlite3_stmt *) m_cellSTMT, bindIndex,
                                   cell.integerValue(i));
                break;
            case Cell::Text: {
                auto pair = cell.textValue(i);
                sqlite3_bind_text((sqlite3_stmt *) m_cellSTMT, bindIndex,
                                  pair.second, pair.first, SQLITE_STATIC);
                break;
            }
            case Cell::BLOB: {
                const Data data = cell.blobValue(i);
                sqlite3_bind_blob((sqlite3_stmt *) m_cellSTMT, bindIndex,
                                  data.buffer(), (int) data.size(),
                                  SQLITE_STATIC);
                break;
            }
            case Cell::Real:
                sqlite3_bind_double((sqlite3_stmt *) m_cellSTMT, bindIndex,
                                    cell.doubleValue(i));
                break;
            case Cell::Null:
                sqlite3_bind_null((sqlite3_stmt *) m_cellSTMT, bindIndex);
                break;
        }
    }
    if (!step(m_cellSTMT)) {
        return false;
    }
    sqlite3_reset((sqlite3_stmt *) m_cellSTMT);
    return true;
}

const Error &SQLiteAssembler::getError() const
{
    return ErrorProne::getError();
}

#pragma mark - Cell
bool SQLiteAssembler::lazyPrepareCell()
{
    WCTInnerAssert(!m_table.empty());
    if (m_cellSTMT == nullptr) {
        bool succeed;
        std::string sql;
        std::tie(succeed, sql) = getAssembleSQL(m_table);
        if (!succeed) {
            return false;
        }
        m_cellSTMT = prepare(sql.c_str());
    }
    return m_cellSTMT != nullptr;
}

std::pair<bool, std::string>
SQLiteAssembler::getAssembleSQL(const std::string &tableName)
{
    bool succeed;
    std::list<std::string> columnNames;
    std::tie(succeed, columnNames) = getColumnNames(tableName);
    if (!succeed) {
        return {false, String::empty()};
    }

    std::ostringstream firstHalfStream;
    std::ostringstream lastHalfStream;
    firstHalfStream << "INSERT INTO " << tableName << "(rowid";
    lastHalfStream << ") VALUES(?";
    for (const auto &columnName : columnNames) {
        firstHalfStream << ", " << columnName;
        lastHalfStream << ", ?";
    }
    lastHalfStream << ")";

    return {true, firstHalfStream.str() + lastHalfStream.str()};
}

std::pair<bool, std::list<std::string>>
SQLiteAssembler::getColumnNames(const std::string &tableName)
{
    std::ostringstream stream;
    stream << "PRAGMA table_info(" << tableName << ")";
    void *stmt = prepare(stream.str().c_str());
    if (stmt == nullptr) {
        return {false, {}};
    }
    bool done;
    std::list<std::string> columns;
    while (step(stmt, done) && !done) {
        const char *column = reinterpret_cast<const char *>(
            sqlite3_column_text((sqlite3_stmt *) stmt, 1));
        columns.push_back(column ? column : String::empty());
    }
    finalize(&stmt);
    if (done) {
        return {true, columns};
    }
    return {false, {}};
}

#pragma mark - Sequence
bool SQLiteAssembler::assembleSequence(const std::string &tableName,
                                       int64_t sequence)
{
    if (sequence <= 0) {
        return true;
    }
    if (m_sequenceSTMT == nullptr) {
        m_sequenceSTMT =
            prepare("INSERT INTO sqlite_sequence(name, seq) VALUES(?1, ?2)");
        if (m_sequenceSTMT == nullptr) {
            return false;
        }
    }
    sqlite3_bind_text((sqlite3_stmt *) m_sequenceSTMT, 1, tableName.c_str(), -1,
                      SQLITE_STATIC);
    sqlite3_bind_int64((sqlite3_stmt *) m_sequenceSTMT, 2, sequence);
    bool result = step(m_sequenceSTMT);
    sqlite3_reset((sqlite3_stmt *) m_sequenceSTMT);
    return result;
}

#pragma mark - Transaction
bool SQLiteAssembler::lazyBeginTransaction()
{
    if (m_transaction) {
        return true;
    }
    if (execute("BEGIN IMMEDIATE")) {
        m_transaction = true;
        return true;
    }
    return false;
}

bool SQLiteAssembler::lazyCommitOrRollbackTransaction(bool commit)
{
    if (m_transaction) {
        m_transaction = false;
        if (!commit || !execute("COMMIT")) {
            execute("ROLLBACK", true); //ignore error
            return false;
        }
    }
    return true;
}

#pragma mark - Error
void SQLiteAssembler::setError(int rc)
{
    Error error;
    error.setSQLiteCode(rc, sqlite3_extended_errcode((sqlite3 *) m_handle));
    const char *message = sqlite3_errmsg((sqlite3 *) m_handle);
    if (message) {
        error.message = message;
    }
    error.infos.set("Path", m_path);
    Notifier::shared()->notify(error);
    ErrorProne::setError(std::move(error));
}

void SQLiteAssembler::setError(int rc, const std::string &sql)
{
    Error error;
    error.setSQLiteCode(rc, sqlite3_extended_errcode((sqlite3 *) m_handle));
    const char *message = sqlite3_errmsg((sqlite3 *) m_handle);
    if (message) {
        error.message = message;
    }
    error.infos.set("SQL", sql);
    error.infos.set("Path", m_path);
    Notifier::shared()->notify(error);
    ErrorProne::setError(std::move(error));
}

#pragma mark - SQLite Handle
bool SQLiteAssembler::open()
{
    if (!m_handle) {
        int rc = sqlite3_open(m_path.c_str(), (sqlite3 **) &m_handle);
        if (rc != SQLITE_OK) {
            setError(rc);
            return false;
        }
        rc = sqlite3_exec((sqlite3 *) m_handle, "PRAGMA journal_mode=delete",
                          nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            setError(rc);
            close();
            return false;
        }
    }
    return true;
}

bool SQLiteAssembler::isOpened() const
{
    return m_handle != nullptr;
}

void SQLiteAssembler::close()
{
    WCTInnerAssert(m_sequenceSTMT == nullptr);
    WCTInnerAssert(m_cellSTMT == nullptr);
    if (m_handle) {
        sqlite3_close((sqlite3 *) m_handle);
        m_handle = nullptr;
    }
}

bool SQLiteAssembler::execute(const char *sql, bool ignoreError)
{
    WCTInnerAssert(isOpened());
    WCTInnerAssert(sql != nullptr);
    int rc = sqlite3_exec((sqlite3 *) m_handle, sql, nullptr, nullptr, nullptr);
    if (rc == SQLITE_OK) {
        return true;
    }
    if (!ignoreError) {
        setError(rc, sql);
    }
    return false;
}

#pragma mark - SQLite STMT
void *SQLiteAssembler::prepare(const char *sql)
{
    WCTInnerAssert(isOpened());
    WCTInnerAssert(sql != nullptr);
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2((sqlite3 *) m_handle, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        return stmt;
    }
    setError(rc, sql);
    return nullptr;
}

bool SQLiteAssembler::step(void *stmt)
{
    bool unused;
    return step(stmt, unused);
}

bool SQLiteAssembler::step(void *stmt, bool &done)
{
    WCTInnerAssert(isOpened());
    WCTInnerAssert(stmt != nullptr);
    int rc = sqlite3_step((sqlite3_stmt *) stmt);
    done = rc == SQLITE_DONE;
    if (rc == SQLITE_OK || rc == SQLITE_DONE || rc == SQLITE_ROW) {
        return true;
    };
    const char *sql = sqlite3_sql((sqlite3_stmt *) stmt);
    if (sql) {
        setError(rc, sql);
    } else {
        setError(rc);
    }
    return false;
}

void SQLiteAssembler::finalize(void **stmt)
{
    if (*stmt != nullptr) {
        sqlite3_finalize((sqlite3_stmt *) *stmt);
        *stmt = nullptr;
    }
}

} //namespace Repair

} //namespace WCDB
