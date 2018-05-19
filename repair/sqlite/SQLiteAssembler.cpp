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
SQLiteAssembler::SQLiteAssembler(const std::string &path_) : path(path_)
{
}

SQLiteAssembler::~SQLiteAssembler()
{
    close();
}

#pragma mark - Assemble
void SQLiteAssembler::markAsAssembled()
{
    Assembler::markAsAssembled();
    finalize();
}

bool SQLiteAssembler::assembleTable(const std::string &sql)
{
    if (!Assembler::assembleTable(sql)) {
        return false;
    }
    if (!open()) {
        return false;
    }
    int rc = sqlite3_exec((sqlite3 *) m_handle, sql.c_str(), nullptr, nullptr,
                          nullptr);
    if (rc != SQLITE_OK) {
        setupError(rc, sql);
        return false;
    }
    return true;
}

bool SQLiteAssembler::assembleTableAssociated(
    const std::list<std::string> &sqls)
{
    if (!Assembler::assembleTableAssociated(sqls)) {
        return false;
    }
    WCTInnerAssert(isOpened());
    int rc = SQLITE_OK;
    for (const auto &sql : sqls) {
        rc = sqlite3_exec((sqlite3 *) m_handle, sql.c_str(), nullptr, nullptr,
                          nullptr);
        if (rc != SQLITE_OK) {
            setupError(rc, sql);
            return false;
        }
    }
    return true;
}

bool SQLiteAssembler::assembleCell(const Cell &cell)
{
    if (!Assembler::assembleCell(cell)) {
        return false;
    }
    int rc = SQLITE_OK;
    if (!isPrepared()) {
        auto pair = getAssembleSQL();
        if (!pair.first) {
            return false;
        }
        rc = sqlite3_prepare_v2((sqlite3 *) m_handle, pair.second.c_str(), -1,
                                (sqlite3_stmt **) &m_stmt, nullptr);
        if (rc != SQLITE_OK) {
            setupError(rc, pair.second);
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
    rc = sqlite3_step((sqlite3_stmt *) m_stmt);
    if (rc != SQLITE_ROW) {
        const char *sql = sqlite3_sql((sqlite3_stmt *) m_stmt);
        if (sql) {
            setupError(rc, sql);
        } else {
            setupError(rc);
        }
        return false;
    }
    sqlite3_reset((sqlite3_stmt *) m_stmt);
    return true;
}

#pragma mark - Error
void SQLiteAssembler::setupError(int rc)
{
    m_error.setupSQLiteCode(rc, sqlite3_extended_errcode((sqlite3 *) m_handle));
}

void SQLiteAssembler::setupError(int rc, const std::string &sql)
{
    m_error.setupSQLiteCode(rc, sqlite3_extended_errcode((sqlite3 *) m_handle));
    m_error.message = sql;
}

#pragma mark - SQLite Handle
bool SQLiteAssembler::open()
{
    if (!m_handle) {
        int rc = sqlite3_open(path.c_str(), (sqlite3 **) &m_handle);
        if (rc != SQLITE_OK) {
            setupError(rc);
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

#pragma mark - SQLite STMT
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
        setupError(rc, tableInfoSQL);
        return {false, ""};
    }
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        firstHalfStream << ", " << sqlite3_column_text(stmt, 1);
        lastHalfStream << ", ?";
    }
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        setupError(rc, tableInfoSQL);
        return {false, ""};
    }

    lastHalfStream << ")";

    return {true, firstHalfStream.str() + lastHalfStream.str()};
}

} //namespace Repair

} //namespace WCDB

#endif // WCDB_OMIT_SQLITE_ASSEMBLER
