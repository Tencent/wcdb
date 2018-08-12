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
#include <WCDB/SQLiteLocker.hpp>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

namespace Repair {

const Error &SQLiteReadLocker::getError() const
{
    return ErrorProne::getError();
}

void SQLiteReadLocker::setPath(const std::string &path)
{
    WCTInnerAssert(m_path.empty());
    SQLiteBase::setPath(path);
}

const std::string &SQLiteReadLocker::getPath() const
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

} //namespace Repair

} //namespace WCDB
