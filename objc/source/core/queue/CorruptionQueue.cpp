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
#include <WCDB/CoreConst.h>
#include <WCDB/CorruptionQueue.hpp>
#include <WCDB/Exiting.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Notifier.hpp>

namespace WCDB {

CorruptionEvent::~CorruptionEvent()
{
}

CorruptionQueue::CorruptionQueue(const String& name, CorruptionEvent* event)
: AsyncQueue(name, event)
{
    Notifier::shared()->setNotification(
    0, name, std::bind(&CorruptionQueue::handleError, this, std::placeholders::_1));
}

CorruptionQueue::~CorruptionQueue()
{
    Notifier::shared()->unsetNotification(name);
}

void CorruptionQueue::handleError(const Error& error)
{
    if (!error.isCorruption() || exiting()) {
        return;
    }
    const auto& infos = error.infos.getStrings();
    auto iter = infos.find("Path");
    if (iter == infos.end()) {
        return;
    }
    const String& path = iter->second;
    bool succeed;
    uint32_t identifier;
    std::tie(succeed, identifier) = FileManager::getFileIdentifier(iter->second);
    if (!succeed) {
        return;
    }
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_refractories.find(identifier);
        if (iter == m_refractories.end()
            || SteadyClock::now() > iter->second
                                    + std::chrono::microseconds((long long) (CorruptionQueueTimeIntervalForInvokingEvent
                                                                             * 1000000))) {
            m_timedQueue.reQueue(path, 0, identifier);
        }
    }
    lazyRun();
}

void CorruptionQueue::loop()
{
    m_timedQueue.loop(std::bind(
    &CorruptionQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

bool CorruptionQueue::onTimed(const String& path, const uint32_t& identifier)
{
    static_cast<CorruptionEvent*>(m_event)->databaseDidBecomeCorrupted(path, identifier);
    LockGuard lockGuard(m_lock);
    m_refractories.emplace(identifier, SteadyClock::now());
    return true;
}

} //namespace WCDB
