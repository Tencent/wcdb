//
// Created by sanhuazhang on 2018/06/22
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

#include "SQLiteBase.hpp"
#include "Assertion.hpp"
#include "Notifier.hpp"
#include "SQLite.h"
#include "StringView.hpp"
#include <sstream>

namespace WCDB {

namespace Repair {

SQLiteBase::SQLiteBase() : m_handle(nullptr), m_codeToBeIgnored(SQLITE_OK)
{
}

void SQLiteBase::setPath(const UnsafeStringView &path)
{
    m_path = path;
}

const StringView &SQLiteBase::getPath() const
{
    return m_path;
}

#pragma mark - Transaction
bool SQLiteBase::lazyBeginTransaction()
{
    if (isInTransaction()) {
        return true;
    }
    return execute("BEGIN");
}

bool SQLiteBase::lazyBeginTransactionImmediate()
{
    if (isInTransaction()) {
        return true;
    }
    return execute("BEGIN IMMEDIATE");
}

bool SQLiteBase::lazyCommitOrRollbackTransaction(bool commit)
{
    if (isInTransaction()) {
        if (!commit || !execute("COMMIT")) {
            markErrorAsIgnorable(-1);
            execute("ROLLBACK"); //ignore all errors
            markErrorAsUnignorable();
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
void SQLiteBase::markErrorAsIgnorable(int codeToBeIgnored)
{
    m_codeToBeIgnored = codeToBeIgnored;
}

void SQLiteBase::markErrorAsUnignorable()
{
    m_codeToBeIgnored = SQLITE_OK;
}

bool SQLiteBase::error(int rc, const char *sql)
{
    bool result;
    Error error;
    if (m_codeToBeIgnored >= 0 && rc != m_codeToBeIgnored) {
        error.level = Error::Level::Error;
        result = false;
    } else {
        error.level = Error::Level::Notice;
        result = true;
    }
    error.setSQLiteCode(rc, sqlite3_extended_errcode((sqlite3 *) m_handle));
    const char *message = sqlite3_errmsg((sqlite3 *) m_handle);
    if (message != nullptr) {
        error.message = message;
    }
    if (sql != nullptr) {
        error.infos.insert_or_assign(ErrorStringKeySQL, sql);
    }
    error.infos.insert_or_assign(ErrorStringKeyPath, m_path);
    Notifier::shared().notify(error);
    ErrorProne::setError(std::move(error));
    return result;
}

#pragma mark - SQLite Handle
bool SQLiteBase::open()
{
    WCTAssert(!m_path.empty());
    if (m_handle == nullptr) {
        int rc = sqlite3_open(m_path.c_str(), (sqlite3 **) &m_handle);
        if (rc != SQLITE_OK) {
            return error(rc);
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
    if (m_handle != nullptr) {
        sqlite3_close_v2((sqlite3 *) m_handle);
        m_handle = nullptr;
    }
}

bool SQLiteBase::execute(const char *sql)
{
    WCTAssert(isOpened());
    WCTAssert(sql != nullptr);
    int rc = sqlite3_exec((sqlite3 *) m_handle, sql, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        return error(rc, sql);
    }
    return true;
}

#pragma mark - SQLite STMT
void *SQLiteBase::prepare(const char *sql)
{
    WCTAssert(isOpened());
    WCTAssert(sql != nullptr);
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2((sqlite3 *) m_handle, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        return stmt;
    }
    error(rc, sql);
    return nullptr;
}

bool SQLiteBase::step(void *stmt)
{
    bool unused;
    return step(stmt, unused);
}

bool SQLiteBase::step(void *stmt, bool &done)
{
    WCTAssert(isOpened());
    WCTAssert(stmt != nullptr);
    int rc = sqlite3_step((sqlite3_stmt *) stmt);
    done = rc == SQLITE_DONE;
    if (rc == SQLITE_OK || rc == SQLITE_DONE || rc == SQLITE_ROW) {
        return true;
    };
    const char *sql = sqlite3_sql((sqlite3_stmt *) stmt);
    return error(rc, sql ? sql : nullptr);
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

#endif /* WCDB_USE_BUILTIN_SQLITE_REPAIR */
