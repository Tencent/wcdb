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
#include <WCDB/Exiting.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/ObservationQueue.hpp>

namespace WCDB {

ObservationQueue::ObservationQueue(const String& name) : AsyncQueue(name)
{
    Notifier::shared()->setNotification(
    0, name, std::bind(&ObservationQueue::handleError, this, std::placeholders::_1));
}

ObservationQueue::~ObservationQueue()
{
    Notifier::shared()->unsetNotification(name);
}

void ObservationQueue::loop()
{
    m_pendings.loop(std::bind(
    &ObservationQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

bool ObservationQueue::onTimed(const String& path, const uint32_t& identifier)
{
    return notifyCorruptedEvent(path, identifier);
}

#pragma mark - Corrupt
bool ObservationQueue::notifyCorruptedEvent(const String& path, uint32_t identifier)
{
    Notification notification = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_notifications.find(path);
        if (iter != m_notifications.end()) {
            notification = iter->second;
        }
    }
    bool resolved = true;
    if (notification) {
        resolved = notification(path, identifier);
    }

    if (!resolved) {
        m_pendings.reQueue(
        path, ObservationQueueTimeIntervalForInvokingCorruptedEvent, identifier);
    }
    return resolved;
}

void ObservationQueue::handleError(const Error& error)
{
    if (!error.isCorruption() || error.level < Error::Level::Error || exiting()) {
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
        auto iter = m_corrupteds.find(identifier);
        if (iter != m_corrupteds.end()) {
            // already received
            return;
        }
    }
    {
        LockGuard lockGuard(m_lock);
        auto result = m_corrupteds.emplace(identifier);
        if (result.second) {
            // mark as pending if and only if it's a new corrupted one.
            m_pendings.reQueue(path, 0, identifier);
            lazyRun();
        }
    }
}

bool ObservationQueue::isFileObservedCorrupted(const String& path)
{
    bool corrupted = false;
    bool succeed;
    uint32_t identifier;
    std::tie(succeed, identifier) = FileManager::getFileIdentifier(path);
    if (succeed) {
        SharedLockGuard lockGuard(m_lock);
        corrupted = m_corrupteds.find(identifier) != m_corrupteds.end();
    }
    return corrupted;
}

#pragma mark - Notification
void ObservationQueue::setNotificationWhenCorrupted(const String& path,
                                                    const Notification& notification)
{
    LockGuard lockGuard(m_lock);
    if (notification != nullptr) {
        m_notifications[path] = notification;
    } else {
        m_notifications.erase(path);
    }
}

} //namespace WCDB
