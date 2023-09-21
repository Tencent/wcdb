//
// Created by 陈秋文 on 2023/9/8.
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

#include "BackupRelatedHandle.hpp"
#include "SQLite.h"

namespace WCDB {

namespace Repair {

BackupRelatedHandle::BackupRelatedHandle() : InnerHandle()
{
}

BackupRelatedHandle::~BackupRelatedHandle() = default;

#pragma mark - Backup
void BackupRelatedHandle::setBackupPath(const UnsafeStringView &path)
{
    InnerHandle::setPath(path);
    open();
}

const StringView &BackupRelatedHandle::getBackupPath() const
{
    return InnerHandle::getPath();
}

const Error &BackupRelatedHandle::getBackupError() const
{
    return InnerHandle::getError();
}

bool BackupRelatedHandle::acquireBackupSharedLock()
{
    WCTAssert(isOpened());
    int maxRryTimes = 5;
    do {
        if (setCheckPointLock(true)) {
            return true;
        }
        if (m_error.code() != Error::Code::Busy) {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        maxRryTimes--;
    } while (maxRryTimes > 0);
    return false;
}

bool BackupRelatedHandle::releaseBackupSharedLock()
{
    return setCheckPointLock(false);
}

bool BackupRelatedHandle::acquireBackupExclusiveLock()
{
    WCTAssert(isOpened());
    return beginTransaction();
}

bool BackupRelatedHandle::releaseBackupExclusiveLock()
{
    rollbackTransaction();
    return true;
}

void BackupRelatedHandle::finishBackup()
{
    InnerHandle::close();
}

} // namespace Repair

} // namespace WCDB
