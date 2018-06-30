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
    : m_cellSTMT(nullptr)
    , m_insertSequenceSTMT(nullptr)
    , m_updateSequenceSTMT(nullptr)
    , m_primary(-1)
//    , m_onTableAssembled(nullptr)
{
}

//void SQLiteAssembler::setNotificationWhenTableAssembled(
//    const TableAssembledCallback &onTableAssembled)
//{
//    m_onTableAssembled = onTableAssembled;
//}

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
void SQLiteAssembler::setPath(const std::string &path)
{
    SQLiteBase::setPath(path);
}

const std::string &SQLiteAssembler::getPath() const
{
    return SQLiteBase::getPath();
}

bool SQLiteAssembler::markAsAssembling()
{
    return open();
}

bool SQLiteAssembler::markAsAssembled()
{
    m_table.clear();
    finalize(&m_cellSTMT);
    bool result = markAsSequenceAssembled();
    result = lazyCommitOrRollbackTransaction() && result;
    close();
    return result;
}

bool SQLiteAssembler::markAsMilestone()
{
    return lazyCommitOrRollbackTransaction() && lazyBeginTransactionImmediate();
}

bool SQLiteAssembler::assembleTable(const std::string &tableName,
                                    const std::string &sql)
{
    if (!execute(sql.c_str())) {
        if (m_error.code() != Error::Code::Error) {
            return false;
        }
    }
    return onTableAssembled(tableName);
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
                                  pair.second, pair.first, SQLITE_TRANSIENT);
                break;
            }
            case Cell::BLOB: {
                const Data data = cell.blobValue(i);
                sqlite3_bind_blob((sqlite3_stmt *) m_cellSTMT, bindIndex,
                                  data.buffer(), (int) data.size(),
                                  SQLITE_TRANSIENT);
                break;
            }
            case Cell::Real:
                sqlite3_bind_double((sqlite3_stmt *) m_cellSTMT, bindIndex,
                                    cell.doubleValue(i));
                break;
            case Cell::Null:
                if (i == m_primary) {
                    sqlite3_bind_int64((sqlite3_stmt *) m_cellSTMT, bindIndex,
                                       cell.getRowID());
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
    int primary = -1;
    int maxpk = 0;
    while (step(stmt, done) && !done) {
        const char *column = reinterpret_cast<const char *>(
            sqlite3_column_text((sqlite3_stmt *) stmt, 1));
        columns.push_back(column ? column : String::empty());

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
        return {true, columns};
    }
    return {false, {}};
}

#pragma mark - Sequence
bool SQLiteAssembler::markAsSequenceAssembling()
{
    if (m_updateSequenceSTMT == nullptr && m_insertSequenceSTMT == nullptr) {
        if (!execute(
                "CREATE TABLE IF NOT EXISTS dummy_sqlite_sequence(i INTEGER "
                "PRIMARY KEY AUTOINCREMENT)")) {
            return false;
        }
    }

    if (m_updateSequenceSTMT == nullptr) {
        m_updateSequenceSTMT =
            prepare("UPDATE sqlite_sequence SET seq = ?1 WHERE name = ?2");
        if (m_updateSequenceSTMT == nullptr) {
            return false;
        }
    }

    if (m_insertSequenceSTMT == nullptr) {
        m_insertSequenceSTMT =
            prepare("INSERT INTO sqlite_sequence(name, seq) VALUES(?1, ?2)");
        if (m_insertSequenceSTMT == nullptr) {
            return false;
        }
    }

    return true;
}

bool SQLiteAssembler::markAsSequenceAssembled()
{
    bool result = execute("DROP TABLE IF EXISTS dummy_sqlite_sequence");
    finalize(&m_updateSequenceSTMT);
    finalize(&m_insertSequenceSTMT);
    return result;
}

std::pair<bool, bool>
SQLiteAssembler::updateSequence(const std::string &tableName, int64_t sequence)
{
    sqlite3_bind_int64((sqlite3_stmt *) m_updateSequenceSTMT, 1, sequence);
    sqlite3_bind_text((sqlite3_stmt *) m_updateSequenceSTMT, 2,
                      tableName.c_str(), -1, SQLITE_TRANSIENT);
    bool result = step(m_updateSequenceSTMT);
    sqlite3_reset((sqlite3_stmt *) m_updateSequenceSTMT);
    if (!result) {
        return {false, false};
    }
    int changes = sqlite3_changes((sqlite3 *) m_handle);
    WCTInnerAssert(changes <= 1);
    return {true, changes > 0};
}

bool SQLiteAssembler::insertSequence(const std::string &tableName,
                                     int64_t sequence)
{
    sqlite3_bind_text((sqlite3_stmt *) m_insertSequenceSTMT, 1,
                      tableName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64((sqlite3_stmt *) m_insertSequenceSTMT, 2, sequence);
    bool result = step(m_insertSequenceSTMT);
    sqlite3_reset((sqlite3_stmt *) m_insertSequenceSTMT);
    return result;
}

bool SQLiteAssembler::assembleSequence(const std::string &tableName,
                                       int64_t sequence)
{
    if (sequence <= 0) {
        return true;
    }
    if (!markAsSequenceAssembling()) {
        return false;
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
    int rc = execute("PRAGMA journal_mode=OFF");
    if (rc != SQLITE_OK) {
        close();
        return false;
    }
    return true;
}

void SQLiteAssembler::close()
{
    WCTInnerAssert(m_insertSequenceSTMT == nullptr);
    WCTInnerAssert(m_updateSequenceSTMT == nullptr);
    WCTInnerAssert(m_cellSTMT == nullptr);
    SQLiteBase::close();
}

} //namespace Repair

} //namespace WCDB
