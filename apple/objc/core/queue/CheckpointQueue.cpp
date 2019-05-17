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
#include <WCDB/CheckpointQueue.hpp>
#include <WCDB/CoreConst.h>

namespace WCDB {

CheckpointQueueEvent::~CheckpointQueueEvent()
{
}

CheckpointQueue::CheckpointQueue(const String& name, CheckpointQueueEvent* event)
: AsyncQueue(name), m_event(event)
{
    WCTInnerAssert(m_event != nullptr);
}

void CheckpointQueue::loop()
{
    m_timedQueue.loop(std::bind(
    &CheckpointQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

bool CheckpointQueue::onTimed(const String& path, const int& frames)
{
    bool result = m_event->databaseShouldCheckpoint(path, frames);
    if (!result) {
        // delay retry if failed
        put(path, CheckpointQueueTimeIntervalForRetryingAfterFailure, frames);
    }
    return result;
}

void CheckpointQueue::put(const String& path, int frames)
{
    put(path,
        frames >= CheckpointQueueFramesThresholdForCritical ?
        CheckpointQueueTimeIntervalForCritical :
        CheckpointQueueTimeIntervalForNonCritical,
        frames);
}

void CheckpointQueue::put(const String& path, double delay, int frames)
{
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_records.find(path);
        if (iter == m_records.end()) {
            return;
        }
        WCTInnerAssert(iter->second > 0);
    }
    m_timedQueue.queue(path, delay, frames);
    lazyRun();
}

void CheckpointQueue::remove(const String& path)
{
    m_timedQueue.remove(path);
}

void CheckpointQueue::register_(const String& path)
{
    LockGuard lockGuard(m_lock);
    ++m_records[path];
}

void CheckpointQueue::unregister(const String& path)
{
    bool removed = false;
    {
        LockGuard lockGuard(m_lock);
        removed = --m_records[path] == 0;
        WCTInnerAssert(m_records[path] >= 0);
        if (removed) {
            m_records.erase(path);
        }
    }
    m_timedQueue.remove(path);
}

} // namespace WCDB
