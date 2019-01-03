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
#include <vector>

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
    bool notify = false;
    {
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        auto iter = m_refractories.find(identifier);
        if (iter == m_refractories.end() || SteadyClock::now() > iter->second) {
            notify = m_corrupted.empty();
            m_corrupted[path] = identifier;
        }
    }
    lazyRun();
    if (notify) {
        m_cond.notify_all();
    }
}

void CorruptionQueue::loop()
{
    while (!exiting()) {
        String path;
        uint32_t corruptedIdentifier;
        {
            std::unique_lock<std::mutex> lockGuard(m_mutex);
            if (m_corrupted.empty()) {
                m_cond.wait(lockGuard);
                continue;
            }
            auto iter = m_corrupted.begin();
            path = iter->first;
            corruptedIdentifier = iter->second;
        }
        static_cast<CorruptionEvent*>(m_event)->databaseDidBecomeCorrupted(
        path, corruptedIdentifier);
        {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
            std::vector<uint32_t> toRemoves;
            SteadyClock now = SteadyClock::now();
            for (const auto& refractory : m_refractories) {
                if (now > refractory.second) {
                    toRemoves.push_back(refractory.first);
                }
            }
            for (const auto& toRemove : toRemoves) {
                m_refractories.erase(toRemove);
            }

            m_corrupted.erase(path);
            m_refractories.emplace(
            corruptedIdentifier,
            now
            + std::chrono::microseconds(
              (long long) (CorruptionQueueTimeIntervalForInvokingEvent * 1000000)));
        }
    }
}

} //namespace WCDB
