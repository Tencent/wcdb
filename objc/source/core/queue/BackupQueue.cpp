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
#include <WCDB/BackupQueue.hpp>
#include <WCDB/CoreConst.h>

namespace WCDB {

BackupEvent::~BackupEvent()
{
}

BackupQueue::BackupQueue(const String& name, BackupEvent* event)
: AsyncQueue(name, event)
{
}

BackupQueue::~BackupQueue()
{
    m_timedQueue.stop();
    m_timedQueue.waitUntilDone();
}

void BackupQueue::loop()
{
    m_timedQueue.loop(std::bind(
    &BackupQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

void BackupQueue::put(const String& path, double delay, int frames)
{
    if (!exit()) {
        m_timedQueue.reQueue(path, delay, frames);
        lazyRun();
    }
}

int BackupQueue::getBackedUpFrames(const String& path)
{
    SharedLockGuard lockGuard(m_lock);
    return m_backedUp[path];
}

bool BackupQueue::onTimed(const String& path, const int& frames)
{
    if (exit()) {
        m_timedQueue.stop();
        return true;
    }

    bool result = static_cast<BackupEvent*>(m_event)->databaseShouldBackup(path);
    if (!result) {
        // retry if failed
        m_timedQueue.reQueue(path, BackupQueueDelayForRetryingAfterFailure, frames);
    }
    LockGuard lockGuard(m_lock);
    m_backedUp[path] = frames;
    return result;
}

} // namespace WCDB
