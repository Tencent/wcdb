//
// Created by sanhuazhang on 2018/05/19
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

#if WCDB_USE_BUILTIN_SQLITE_REPAIR

#include "SQLiteAssembler.hpp"
#include "Assertion.hpp"
#include "Cell.hpp"
#include "SQLite.h"
#include "StringView.hpp"
#include <sstream>

namespace WCDB {

namespace Repair {

#pragma mark - Initialize
SQLiteAssembler::SQLiteAssembler() : m_cellSTMT(nullptr), m_primary(-1)
{
}

SQLiteAssembler::~SQLiteAssembler() = default;

#pragma mark - Assemble
void SQLiteAssembler::setPath(const UnsafeStringView &path)
{
    SQLiteBase::setPath(path);
}

const StringView &SQLiteAssembler::getPath() const
{
    return SQLiteBase::getPath();
}

bool SQLiteAssembler::markAsAssembling()
{
    if (!open()) {
        return false;
    }
    if (!markSequenceAsAssembling()) {
        close();
        return false;
    }
    return true;
}

bool SQLiteAssembler::markAsAssembled()
{
    m_table.clear();
    finalize(&m_cellSTMT);
    bool result = markSequenceAsAssembled();
    result = lazyCommitOrRollbackTransaction() && result;
    close();
    return result;
}

bool SQLiteAssembler::markAsMilestone()
{
    return lazyCommitOrRollbackTransaction() && lazyBeginTransactionImmediate();
}

bool SQLiteAssembler::assembleTable(const UnsafeStringView &tableName,
                                    const UnsafeStringView &sql)
{
    finalize(&m_cellSTMT);
    m_table.clear();
    markErrorAsIgnorable(SQLITE_ERROR);
    bool succeed = execute(sql.c_str());
    markErrorAsUnignorable();
    if (succeed) {
        m_table = tableName;
        return true;
    }
    return false;
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
            sqlite3_bind_int64(
            (sqlite3_stmt *) m_cellSTMT, bindIndex, cell.integerValue(i));
            break;
        case Cell::Text: {
            sqlite3_bind_text(
            (sqlite3_stmt *) m_cellSTMT, bindIndex, cell.textValue(i).cstring(), -1, SQLITE_TRANSIENT);
            break;
        }
        case Cell::BLOB: {
            const UnsafeData data = cell.blobValue(i);
            sqlite3_bind_blob(
            (sqlite3_stmt *) m_cellSTMT, bindIndex, data.buffer(), (int) data.size(), SQLITE_TRANSIENT);
            break;
        }
        case Cell::Real:
            sqlite3_bind_double(
            (sqlite3_stmt *) m_cellSTMT, bindIndex, cell.doubleValue(i));
            break;
        case Cell::Null:
            if (i == m_primary) {
                sqlite3_bind_int64((sqlite3_stmt *) m_cellSTMT, bindIndex, cell.getRowID());
            } else {
                sqlite3_bind_null((sqlite3_stmt *) m_cellSTMT, bindIndex);
            }
            break;
        }
    }
    bool result = step(m_cellSTMT);
    sqlite3_reset((sqlite3_stmt *) m_cellSTMT);
    return result;
}

bool SQLiteAssembler::assembleSQL(const UnsafeStringView &sql)
{
    markErrorAsIgnorable(SQLITE_ERROR);
    bool succeed = execute(sql.c_str());
    markErrorAsUnignorable();
    return succeed;
}

const Error &SQLiteAssembler::getError() const
{
    return ErrorProne::getError();
}

#pragma mark - Cell
bool SQLiteAssembler::lazyPrepareCell()
{
    WCTAssert(!m_table.empty());
    if (m_cellSTMT == nullptr) {
        bool succeed;
        StringView sql;
        std::tie(succeed, sql) = getAssembleSQL(m_table);
        if (!succeed) {
            return false;
        }
        m_cellSTMT = prepare(sql.c_str());
    }
    return m_cellSTMT != nullptr;
}

Optional<StringView> SQLiteAssembler::getAssembleSQL(const UnsafeStringView &tableName)
{
    bool succeed;
    std::list<StringView> columnNames;
    std::tie(succeed, columnNames) = getColumnNames(tableName);
    if (!succeed) {
        return { false, StringView() };
    }

    std::ostringstream firstHalfStream;
    std::ostringstream lastHalfStream;
    if (isDuplicatedIgnorable()) {
        firstHalfStream << "INSERT OR IGNORE INTO ";
    } else {
        firstHalfStream << "INSERT INTO ";
    }
    firstHalfStream << tableName << "(rowid";
    lastHalfStream << ") VALUES(?";
    for (const auto &columnName : columnNames) {
        firstHalfStream << ", " << columnName;
        lastHalfStream << ", ?";
    }
    lastHalfStream << ")";

    return { true, firstHalfStream.str() + lastHalfStream.str() };
}

Optional<std::list<StringView>>
SQLiteAssembler::getColumnNames(const UnsafeStringView &tableName)
{
    std::ostringstream stream;
    stream << "PRAGMA table_info(" << tableName << ")";
    void *stmt = prepare(stream.str().c_str());
    if (stmt == nullptr) {
        return { false, {} };
    }
    bool done;
    std::list<StringView> columns;
    int primary = -1;
    int maxpk = 0;
    while (step(stmt, done) && !done) {
        const char *column
        = reinterpret_cast<const char *>(sqlite3_column_text((sqlite3_stmt *) stmt, 1));
        columns.push_back(column ? column : StringView());

        //check if and only if single column is primary key
        int pk = sqlite3_column_int((sqlite3_stmt *) stmt, 5);
        maxpk = std::max(pk, maxpk);
        if (pk == 1) {
            primary = (int) columns.size() - 1;
        }
    }
    finalize(&stmt);
    if (done) {
        m_primary = maxpk == 1 ? primary : -1;
        return { true, columns };
    }
    return { false, {} };
}

#pragma mark - Sequence
bool SQLiteAssembler::markSequenceAsAssembling()
{
    return execute(
    "CREATE TABLE IF NOT EXISTS wcdb_dummy_sqlite_sequence(i INTEGER "
    "PRIMARY KEY AUTOINCREMENT)");
}

bool SQLiteAssembler::markSequenceAsAssembled()
{
    return execute("DROP TABLE IF EXISTS wcdb_dummy_sqlite_sequence");
}

Optional<bool>
SQLiteAssembler::updateSequence(const UnsafeStringView &tableName, int64_t sequence)
{
    void *stmt = prepare("UPDATE sqlite_sequence SET seq = ?1 WHERE name = ?2");
    sqlite3_bind_int64((sqlite3_stmt *) stmt, 1, sequence);
    sqlite3_bind_text((sqlite3_stmt *) stmt, 2, tableName.c_str(), -1, SQLITE_TRANSIENT);
    bool result = step(stmt);
    finalize(&stmt);
    if (!result) {
        return { false, false };
    }
    int changes = sqlite3_changes((sqlite3 *) m_handle);
    WCTAssert(changes <= 1);
    return { true, changes > 0 };
}

bool SQLiteAssembler::insertSequence(const UnsafeStringView &tableName, int64_t sequence)
{
    void *stmt = prepare("INSERT INTO sqlite_sequence(name, seq) VALUES(?1, ?2)");
    sqlite3_bind_text((sqlite3_stmt *) stmt, 1, tableName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64((sqlite3_stmt *) stmt, 2, sequence);
    bool result = step(stmt);
    finalize(&stmt);
    return result;
}

bool SQLiteAssembler::assembleSequence(const UnsafeStringView &tableName, int64_t sequence)
{
    if (sequence <= 0) {
        return true;
    }
    bool succeed, updated;
    std::tie(succeed, updated) = updateSequence(tableName, sequence);
    if (!succeed) {
        return false;
    }
    if (updated) {
        return true;
    }
    return insertSequence(tableName, sequence);
}

#pragma mark - SQLite Base
bool SQLiteAssembler::open()
{
    if (!SQLiteBase::open()) {
        return false;
    }
    if (!execute("PRAGMA journal_mode=OFF") || !execute("PRAGMA mmap_size=2147418112")) {
        close();
        return false;
    }
    return true;
}

void SQLiteAssembler::close()
{
    WCTAssert(m_cellSTMT == nullptr);
    SQLiteBase::close();
}

} //namespace Repair

} //namespace WCDB

#endif /* WCDB_USE_BUILTIN_SQLITE_REPAIR */
