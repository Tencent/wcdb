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

#ifndef WCDB_OMIT_SQLITE_ASSEMBLER

#include <WCDB/Assertion.hpp>
#include <WCDB/Cell.hpp>
#include <WCDB/SQLiteAssembler.hpp>
#include <list>
#include <sqlite3.h>
#include <sstream>

namespace WCDB {

namespace Repair {

#pragma mark - Initialze
SQLiteAssembler::SQLiteAssembler(const std::string &path_)
    : Assembler(), path(path_), m_transaction(false)
{
}

SQLiteAssembler::~SQLiteAssembler()
{
    close();
}

#pragma mark - Assemble
bool SQLiteAssembler::markAsAssembling(const std::string &tableName)
{
    return SQLiteAssembler::markAsAssembling(tableName) && open();
}

bool SQLiteAssembler::markAsAssembled()
{
    finalize();
    return lazyCommitOrRollbackTransaction() && Assembler::markAsAssembled();
}

bool SQLiteAssembler::markAsMilestone()
{
    return lazyCommitOrRollbackTransaction();
}

bool SQLiteAssembler::assembleTable(const std::string &sql)
{
    WCTInnerAssert(isAssembling());
    return lazyBeginTransaction() && execute(sql.c_str());
}

bool SQLiteAssembler::assembleCell(const Cell &cell)
{
    WCTInnerAssert(isAssembling());
    if (!lazyBeginTransaction()) {
        return false;
    }
    if (!isPrepared()) {
        auto pair = getAssembleSQL();
        if (!pair.first || !prepare(pair.second.c_str())) {
            return false;
        }
    }
    sqlite3_bind_int64((sqlite3_stmt *) m_stmt, 1, cell.getRowID());
    for (int i = 0; i < cell.getCount(); ++i) {
        int bindIndex = i + 2;
        switch (cell.getValueType(i)) {
            case Cell::Integer32:
                sqlite3_bind_int((sqlite3_stmt *) m_stmt, bindIndex,
                                 cell.int32Value(i));
                break;
            case Cell::Integer64:
                sqlite3_bind_int64((sqlite3_stmt *) m_stmt, bindIndex,
                                   cell.int64Value(i));
                break;
            case Cell::Text: {
                auto pair = cell.textValue(i);
                sqlite3_bind_text((sqlite3_stmt *) m_stmt, bindIndex,
                                  pair.second, pair.first, SQLITE_STATIC);
                break;
            }
            case Cell::BLOB: {
                auto pair = cell.blobValue(i);
                sqlite3_bind_blob((sqlite3_stmt *) m_stmt, bindIndex,
                                  pair.second, pair.first, SQLITE_STATIC);
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

bool SQLiteAssembler::lazyCommitOrRollbackTransaction()
{
    if (m_transaction) {
        m_transaction = false;
        if (!execute("COMMIT")) {
            execute("ROLLBACK", true); //ignore error
            return false;
        }
    }
    return true;
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
    error.infos.set("Path", path);
    error.infos.set("Source", "SQLite");
    Reporter::shared()->report(error);
    setThreadedError(std::move(error));
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
    error.infos.set("Path", path);
    error.infos.set("Source", "SQLite");
    Reporter::shared()->report(error);
    setThreadedError(std::move(error));
}

#pragma mark - SQLite Handle
bool SQLiteAssembler::open()
{
    if (!m_handle) {
        int rc = sqlite3_open(path.c_str(), (sqlite3 **) &m_handle);
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

std::pair<bool, std::string> SQLiteAssembler::getAssembleSQL()
{
    WCTInnerAssert(isOpened());
    WCTInnerAssert(isAssembling());
    WCTInnerAssert(!isPrepared());

    std::ostringstream stringStream;
    stringStream << "PRAGMA table_info(" << m_assembling << ")";
    std::string tableInfoSQL = stringStream.str();

    std::ostringstream firstHalfStream;
    std::ostringstream lastHalfStream;
    firstHalfStream << "INSERT INTO " << m_assembling << "(rowid";
    lastHalfStream << ") VALUES(?";

    //get columns
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2((sqlite3 *) m_handle, tableInfoSQL.c_str(), -1,
                                &stmt, nullptr);
    if (rc != SQLITE_OK) {
        setThreadedError(rc, tableInfoSQL);
        return {false, ""};
    }
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        firstHalfStream << ", " << sqlite3_column_text(stmt, 1);
        lastHalfStream << ", ?";
    }
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        setThreadedError(rc, tableInfoSQL);
        return {false, ""};
    }

    lastHalfStream << ")";

    return {true, firstHalfStream.str() + lastHalfStream.str()};
}

} //namespace Repair

} //namespace WCDB

#endif // WCDB_OMIT_SQLITE_ASSEMBLER
