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

#include "SQLiteLocker.hpp"
#include "Assertion.hpp"
#include "SQLite.h"

namespace WCDB {

namespace Repair {

#pragma mark - SQLiteReadLocker
SQLiteReadLocker::~SQLiteReadLocker() = default;

const Error &SQLiteReadLocker::getError() const
{
    return ErrorProne::getError();
}

void SQLiteReadLocker::setPath(const UnsafeStringView &path)
{
    WCTAssert(m_path.empty());
    SQLiteBase::setPath(path);
}

const StringView &SQLiteReadLocker::getPath() const
{
    return SQLiteBase::getPath();
}

bool SQLiteReadLocker::acquireLock()
{
    if (!open()) {
        return false;
    }
    if (!lazyBeginTransaction()) {
        return false;
    }
    //execute read sql to acquire read lock
    return execute("SELECT 1 FROM sqlite_master LIMIT 0");
}

bool SQLiteReadLocker::releaseLock()
{
    lazyCommitOrRollbackTransaction(false);
    close();
    return true;
}

#pragma mark - SQLiteWriteLocker
SQLiteWriteLocker::~SQLiteWriteLocker() = default;

const Error &SQLiteWriteLocker::getError() const
{
    return ErrorProne::getError();
}

void SQLiteWriteLocker::setPath(const UnsafeStringView &path)
{
    WCTAssert(m_path.empty());
    SQLiteBase::setPath(path);
}

const StringView &SQLiteWriteLocker::getPath() const
{
    return SQLiteBase::getPath();
}

bool SQLiteWriteLocker::acquireLock()
{
    if (!open()) {
        return false;
    }
    return lazyBeginTransactionImmediate();
}

bool SQLiteWriteLocker::releaseLock()
{
    lazyCommitOrRollbackTransaction(false);
    close();
    return true;
}

} //namespace Repair

} //namespace WCDB

#endif /* WCDB_USE_BUILTIN_SQLITE_REPAIR */
