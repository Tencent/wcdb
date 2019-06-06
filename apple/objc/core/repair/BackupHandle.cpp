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

#include <WCDB/BackupHandle.hpp>
#include <WCDB/CoreConst.h>

namespace WCDB {

BackupHandle::BackupHandle()
: m_statementForAcquireReadLock(
StatementSelect().select(1).from(Syntax::masterTable).limit(0))
, m_statementForReadTransaction(StatementBegin().beginDeferred())
{
    m_error.infos.set(ErrorStringKeyAction, ErrorActionBackup);
}

void BackupHandle::setPath(const String &path)
{
    Handle::setPath(path);
}

void BackupHandle::clearPath()
{
    Handle::clearPath();
}

const String &BackupHandle::getPath() const
{
    return Handle::getPath();
}

const Error &BackupHandle::getError() const
{
    return Handle::getError();
}

bool BackupHandle::acquireReadLock()
{
    return open() && execute(m_statementForReadTransaction)
           && execute(m_statementForAcquireReadLock);
}

bool BackupHandle::releaseReadLock()
{
    rollbackTransaction();
    return true;
}

bool BackupHandle::acquireWriteLock()
{
    return open() && beginTransaction();
}

bool BackupHandle::releaseWriteLock()
{
    rollbackTransaction();
    return true;
}

} // namespace WCDB
