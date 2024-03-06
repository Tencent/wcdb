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

#pragma once

#if WCDB_USE_BUILTIN_SQLITE_REPAIR

#include "ErrorProne.hpp"
#include "StringView.hpp"

// repair/sqlite are optional. Ones can implement their own tools.

namespace WCDB {

namespace Repair {

class SQLiteBase : public ErrorProne {
public:
    SQLiteBase();
    virtual ~SQLiteBase() override = 0;

    void setPath(const UnsafeStringView &path);
    const StringView &getPath() const;

protected:
    StringView m_path;

#pragma mark - Error
protected:
    bool error(int rc, const char *sql = nullptr);

    // if code >= 0, then the level of error with the specified code will be marked as ignored
    // if code < 0, then the level of all errors will be marked as ignored
    void markErrorAsIgnorable(int codeToBeIgnored);
    void markErrorAsUnignorable();

private:
    int m_codeToBeIgnored;

#pragma mark - SQLite Handle
protected:
    bool open();
    bool isOpened() const;
    void close();

    bool execute(const char *sql);

    void *m_handle;

#pragma mark - SQLite STMT
protected:
    void *prepare(const char *sql);
    bool step(void *stmt);
    bool step(void *stmt, bool &done);

    void finalize(void **stmt);

#pragma mark - Transaction
protected:
    bool lazyBeginTransaction();
    bool lazyBeginTransactionImmediate();
    bool lazyCommitOrRollbackTransaction(bool commit = true);
    bool isInTransaction();
};

} //namespace Repair

} //namespace WCDB

#endif /* WCDB_USE_BUILTIN_SQLITE_REPAIR */
