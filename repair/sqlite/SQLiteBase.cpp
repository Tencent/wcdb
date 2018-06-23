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
#include <WCDB/SQLiteBase.hpp>
#include <WCDB/String.hpp>
#include <sqlite3.h>
#include <sstream>

namespace WCDB {

namespace Repair {

SQLiteBase::SQLiteBase() : m_handle(nullptr)
{
}

void SQLiteBase::setPath(const std::string &path)
{
    m_path = path;
}

const std::string &SQLiteBase::getPath() const
{
    return m_path;
}

#pragma mark - Transaction
bool SQLiteBase::lazyBeginTransaction()
{
    if (isInTransaction()) {
        return true;
    }
    return execute("BEGIN IMMEDIATE");
    ;
}

bool SQLiteBase::lazyCommitOrRollbackTransaction(bool commit)
{
    if (isInTransaction()) {
        if (!commit || !execute("COMMIT")) {
            execute("ROLLBACK", true); //ignore error
            return false;
        }
    }
    return true;
}

bool SQLiteBase::isInTransaction()
{
    return sqlite3_get_autocommit((sqlite3 *) m_handle) == 0;
}

#pragma mark - Error
void SQLiteBase::setError(int rc, const char *sql)
{
    Error error;
    error.setSQLiteCode(rc, sqlite3_extended_errcode((sqlite3 *) m_handle));
    const char *message = sqlite3_errmsg((sqlite3 *) m_handle);
    if (message) {
        error.message = message;
    }
    if (sql) {
        error.infos.set("SQL", sql);
    }
    error.infos.set("Path", m_path);
    Notifier::shared()->notify(error);
    ErrorProne::setError(std::move(error));
}

#pragma mark - SQLite Handle
bool SQLiteBase::open()
{
    WCTInnerAssert(!m_path.empty());
    if (!m_handle) {
        int rc = sqlite3_open(m_path.c_str(), (sqlite3 **) &m_handle);
        if (rc != SQLITE_OK) {
            setError(rc);
            return false;
        }
    }
    return true;
}

bool SQLiteBase::isOpened() const
{
    return m_handle != nullptr;
}

void SQLiteBase::close()
{
    if (m_handle) {
        sqlite3_close((sqlite3 *) m_handle);
        m_handle = nullptr;
    }
}

bool SQLiteBase::execute(const char *sql, bool ignoreError)
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
void *SQLiteBase::prepare(const char *sql)
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

bool SQLiteBase::step(void *stmt)
{
    bool unused;
    return step(stmt, unused);
}

bool SQLiteBase::step(void *stmt, bool &done)
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

void SQLiteBase::finalize(void **stmt)
{
    if (*stmt != nullptr) {
        sqlite3_finalize((sqlite3_stmt *) *stmt);
        *stmt = nullptr;
    }
}

} //namespace Repair

} //namespace WCDB
