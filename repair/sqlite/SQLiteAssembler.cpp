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
#include <WCDB/ThreadedErrors.hpp>
#include <sqlite3.h>
#include <sstream>

namespace WCDB {

namespace Repair {

#pragma mark - Initialze
SQLiteAssembler::SQLiteAssembler()
    : Assembler(), m_transaction(false), m_onTableAssembled(nullptr)
{
}

void SQLiteAssembler::setCallbackOnTableAssembled(
    const TableAssembledCallback &onTableAssembled)
{
    m_onTableAssembled = onTableAssembled;
}

int SQLiteAssembler::onTableAssembled(const std::string &tableName)
{
    if (m_onTableAssembled) {
        return m_onTableAssembled(tableName, m_handle);
    }
    return SQLITE_OK;
}

#pragma mark - Assemble
bool SQLiteAssembler::markAsAssembling()
{
    return open();
}

void SQLiteAssembler::markAsAssembled()
{
    finalize();
    close();
}

bool SQLiteAssembler::markAsMilestone()
{
    return lazyCommitOrRollbackTransaction() && lazyBeginTransaction();
}

bool SQLiteAssembler::assembleTable(const std::string &tableName,
                                    const std::string &sql,
                                    int64_t sequence)
{
    finalize();
    if (!execute(sql.c_str()) || !assemblerSequence(tableName, sequence)) {
        return false;
    }
    int rc = onTableAssembled(tableName);
    if (rc != SQLITE_OK) {
        setThreadedError(rc);
        return false;
    }
    auto pair = getAssembleSQL(tableName);
    return pair.first && prepare(pair.second.c_str());
}

bool SQLiteAssembler::assembleCell(const Cell &cell)
{
    sqlite3_bind_int64((sqlite3_stmt *) m_stmt, 1, cell.getRowID());
    for (int i = 0; i < cell.getCount(); ++i) {
        int bindIndex = i + 2;
        switch (cell.getValueType(i)) {
            case Cell::Integer:
                sqlite3_bind_int64((sqlite3_stmt *) m_stmt, bindIndex,
                                   cell.integerValue(i));
                break;
            case Cell::Text: {
                auto pair = cell.textValue(i);
                sqlite3_bind_text((sqlite3_stmt *) m_stmt, bindIndex,
                                  pair.second, pair.first, SQLITE_TRANSIENT);
                break;
            }
            case Cell::BLOB: {
                auto pair = cell.blobValue(i);
                sqlite3_bind_blob((sqlite3_stmt *) m_stmt, bindIndex,
                                  pair.second, pair.first, SQLITE_TRANSIENT);
                break;
            }
            case Cell::Real:
                sqlite3_bind_double((sqlite3_stmt *) m_stmt, bindIndex,
                                    cell.doubleValue(i));
                break;
            case Cell::Null:
                sqlite3_bind_null((sqlite3_stmt *) m_stmt, bindIndex);
                break;
        }
    }
    if (!step()) {
        return false;
    }
    sqlite3_reset((sqlite3_stmt *) m_stmt);
    return true;
}

#pragma mark - Helper
bool SQLiteAssembler::assemblerSequence(const std::string &tableName,
                                        int64_t sequence)
{
    if (sequence <= 0) {
        return true;
    }
    std::ostringstream stringStream;
    stringStream << "INSERT INTO sqlite_sequence(name, seq) VALUES("
                 << tableName << ", " << sequence << ")";
    return execute(stringStream.str().c_str());
}

std::pair<bool, std::list<std::string>>
SQLiteAssembler::getColumnNames(const std::string &tableName)
{
    std::ostringstream stringStream;
    stringStream << "PRAGMA table_info(" << tableName << ")";
    std::string tableInfoSQL = stringStream.str();
    if (!prepare(stringStream.str().c_str())) {
        return {false, {}};
    }
    bool done;
    std::list<std::string> columnNames;
    while (step(done) && !done) {
        const char *columnName = reinterpret_cast<const char *>(
            sqlite3_column_text((sqlite3_stmt *) m_stmt, 1));
        columnNames.push_back(columnName ? columnName : String::empty());
    }
    finalize();
    if (done) {
        return {true, columnNames};
    }
    return {false, {}};
}

std::pair<bool, std::string>
SQLiteAssembler::getAssembleSQL(const std::string &tableName)
{
    WCTInnerAssert(isOpened());
    WCTInnerAssert(!isPrepared());

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

bool SQLiteAssembler::lazyBeginTransaction()
{
    if (m_transaction) {
        return true;
    }
    if (execute("BEGIN IMMEDIATED")) {
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

const Error &SQLiteAssembler::getError() const
{
    return ThreadedErrors::shared()->getThreadedError();
}

#pragma mark - Error
void SQLiteAssembler::setThreadedError(int rc)
{
    Error error;
    error.setSQLiteCode(rc, sqlite3_extended_errcode((sqlite3 *) m_handle));
    const char *message = sqlite3_errmsg((sqlite3 *) m_handle);
    if (message) {
        error.message = message;
    }
    error.infos.set("Path", m_path);
    Reporter::shared()->report(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

void SQLiteAssembler::setThreadedError(int rc, const std::string &sql)
{
    Error error;
    error.setSQLiteCode(rc, sqlite3_extended_errcode((sqlite3 *) m_handle));
    const char *message = sqlite3_errmsg((sqlite3 *) m_handle);
    if (message) {
        error.message = message;
    }
    error.infos.set("SQL", sql);
    error.infos.set("Path", m_path);
    Reporter::shared()->report(error);
    SharedThreadedErrorProne::setThreadedError(std::move(error));
}

#pragma mark - SQLite Handle
bool SQLiteAssembler::open()
{
    if (!m_handle) {
        int rc = sqlite3_open(m_path.c_str(), (sqlite3 **) &m_handle);
        if (rc != SQLITE_OK) {
            setThreadedError(rc);
            return false;
        }
        rc = sqlite3_exec((sqlite3 *) m_handle, "PRAGMA journal_mode=delete",
                          nullptr, nullptr, nullptr);
        if (rc != SQLITE_OK) {
            setThreadedError(rc);
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
    finalize();
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
        setThreadedError(rc, sql);
    }
    return false;
}

#pragma mark - SQLite STMT
bool SQLiteAssembler::prepare(const char *sql)
{
    WCTInnerAssert(isOpened());
    WCTInnerAssert(!isPrepared());
    WCTInnerAssert(sql != nullptr);
    int rc = sqlite3_prepare_v2((sqlite3 *) m_handle, sql, -1,
                                (sqlite3_stmt **) &m_stmt, nullptr);
    if (rc == SQLITE_OK) {
        return true;
    }
    setThreadedError(rc, sql);
    return false;
}

bool SQLiteAssembler::step()
{
    bool unused;
    return step(unused);
}

bool SQLiteAssembler::step(bool &done)
{
    WCTInnerAssert(isOpened());
    WCTInnerAssert(isPrepared());
    int rc = sqlite3_step((sqlite3_stmt *) m_stmt);
    done = rc == SQLITE_DONE;
    if (rc == SQLITE_OK || rc == SQLITE_DONE || rc == SQLITE_ROW) {
        return true;
    };
    const char *sql = sqlite3_sql((sqlite3_stmt *) m_stmt);
    if (sql) {
        setThreadedError(rc, sql);
    } else {
        setThreadedError(rc);
    }
    return false;
}

bool SQLiteAssembler::isPrepared() const
{
    WCTInnerAssert(isOpened());
    return m_stmt != nullptr;
}

void SQLiteAssembler::finalize()
{
    if (m_stmt) {
        sqlite3_finalize((sqlite3_stmt *) m_stmt);
        m_stmt = nullptr;
    }
}

} //namespace Repair

} //namespace WCDB
