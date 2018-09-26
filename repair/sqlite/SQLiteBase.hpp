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

#ifndef SQLiteBase_hpp
#define SQLiteBase_hpp

#include <WCDB/ErrorProne.hpp>
#include <string>

// repair/sqlite are optional. Ones can implement their own tools.

namespace WCDB {

namespace Repair {

class SQLiteBase : public ErrorProne {
public:
    SQLiteBase();

    void setPath(const std::string &path);
    const std::string &getPath() const;

protected:
    std::string m_path;

#pragma mark - Error
protected:
    void setError(int rc, const char *sql = nullptr);

#pragma mark - SQLite Handle
protected:
    bool open();
    bool isOpened() const;
    void close();

    // if errorToBeIgnored < 0, all errors will be ignored.
    bool execute(const char *sql, int errorToBeIgnored = 0);

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

#endif /* SQLiteBase_hpp */
