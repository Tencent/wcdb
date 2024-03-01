//
// Created by 陈秋文 on 2023/11/16.
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

#include "BackupHandleOperator.hpp"

namespace WCDB {

namespace Repair {

BackupHandleOperator::BackupHandleOperator(InnerHandle *handle)
: HandleOperator(handle)
{
}

BackupHandleOperator::~BackupHandleOperator() = default;

#pragma mark - Backup
void BackupHandleOperator::setBackupPath(const UnsafeStringView &path)
{
    getHandle()->setPath(path);
    getHandle()->open();
}

const StringView &BackupHandleOperator::getBackupPath() const
{
    return getHandle()->getPath();
}

const Error &BackupHandleOperator::getBackupError() const
{
    return getHandle()->getError();
}

bool BackupHandleOperator::acquireBackupSharedLock()
{
    WCTAssert(getHandle()->isOpened());
    if (!getHandle()->isOpened()) {
        getHandle()->notifyError(Error::Code::Warning, "", "Handle is not opened!");
        return false;
    }
    int maxRryTimes = 5;
    do {
        if (getHandle()->setCheckPointLock(true)) {
            return true;
        }
        if (getHandle()->getError().code() != Error::Code::Busy) {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        maxRryTimes--;
    } while (maxRryTimes > 0);
    return false;
}

bool BackupHandleOperator::releaseBackupSharedLock()
{
    return getHandle()->setCheckPointLock(false);
}

bool BackupHandleOperator::acquireBackupExclusiveLock()
{
    WCTAssert(getHandle()->isOpened());
    return getHandle()->beginTransaction();
}

bool BackupHandleOperator::releaseBackupExclusiveLock()
{
    getHandle()->rollbackTransaction();
    return true;
}

bool BackupHandleOperator::backupSuspended() const
{
    return getHandle()->isSuspended();
}

void BackupHandleOperator::finishBackup()
{
    getHandle()->close();
}

} // namespace Repair

} // namespace WCDB
