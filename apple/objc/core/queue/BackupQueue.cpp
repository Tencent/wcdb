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

BackupQueueEvent::~BackupQueueEvent()
{
}

BackupQueue::BackupQueue(const String& name, BackupQueueEvent* event)
: AsyncQueue(name), m_event(event)
{
    WCTInnerAssert(m_event != nullptr);
}

void BackupQueue::loop()
{
    m_timedQueue.loop(std::bind(
    &BackupQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

void BackupQueue::put(const String& path, int frames)
{
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_records.find(path);
    if (iter != m_records.end()) {
        WCTInnerAssert(iter->second.registers > 0);
        int recordedFrames = iter->second.frames;
        if (frames >= recordedFrames + BackupConfigFramesIntervalForCritical // expired too much
            || frames < recordedFrames // restarted
        ) {
            m_timedQueue.queue(path, BackupQueueTimeIntervalForCritical, frames);
            lazyRun();
        } else if (frames >= recordedFrames + BackupConfigFramesIntervalForNonCritical) {
            m_timedQueue.queue(path, BackupQueueTimeIntervalForNonCritical, frames);
            lazyRun();
        }
    }
}

void BackupQueue::register_(const String& path)
{
    LockGuard lockGuard(m_lock);
    ++m_records[path].registers;
}

void BackupQueue::unregister(const String& path)
{
    LockGuard lockGuard(m_lock);
    WCTInnerAssert(m_records[path].registers >= 0);
    if (--m_records[path].registers == 0) {
        m_records.erase(path);
        m_timedQueue.remove(path);
    }
}

void BackupQueue::onTimed(const String& path, const int& frames)
{
    bool result = m_event->databaseShouldBackup(path);
    LockGuard lockGuard(m_lock);
    auto iter = m_records.find(path);
    if (iter != m_records.end()) {
        // already unregistered

        WCTInnerAssert(iter->second.registers > 0);
        if (!result) {
            // retry if failed
            m_timedQueue.queue(path, BackupQueueTimeIntervalForRetryingAfterFailure, frames);
        } else {
            iter->second.frames = frames;
        }
    }
}

BackupQueue::Record::Record() : frames(0), registers(0)
{
}

} // namespace WCDB
