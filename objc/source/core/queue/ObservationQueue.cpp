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
#include <unistd.h>

namespace WCDB {

#pragma mark - Event
ObservationQueueEvent::~ObservationQueueEvent()
{
}

#pragma mark - Delegate
ObservationDelegate::~ObservationDelegate()
{
}

void ObservationDelegate::observatedThatFileOpened(int fd)
{
    WCTInnerAssert(observationQueue() != nullptr);
    observationQueue()->observatedThatFileOpened(fd);
}

#pragma mark - Queue
ObservationQueue::ObservationQueue(const String& name, ObservationQueueEvent* event)
: AsyncQueue(name)
, m_event(event)
, m_observerForMemoryWarning(registerNotificationWhenMemoryWarning())
, m_pendingToPurge(false)
{
    Notifier::shared()->setNotification(
    0, name, std::bind(&ObservationQueue::handleError, this, std::placeholders::_1));
}

ObservationQueue::~ObservationQueue()
{
    Notifier::shared()->unsetNotification(name);
    unregisterNotificationWhenMemoryWarning(m_observerForMemoryWarning);
}

void ObservationQueue::loop()
{
    m_pendings.loop(std::bind(
    &ObservationQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

bool ObservationQueue::onTimed(const String& parameter1, const uint32_t& parameter2)
{
    if (parameter1.empty()) {
        WCTInnerAssert(m_event != nullptr);

        // do purge
        m_event->observatedThatNeedPurged();

        Error error;
        error.level = Error::Level::Warning;
        error.setCode(Error::Code::Warning);
        if (parameter2 > 0) {
            error.message = String::formatted(
            "Purge due to too many file descriptors with %u.", parameter2);
        } else {
            error.message = "Purge due to memory warning.";
        }
        Notifier::shared()->notify(error);

        LockGuard lockGuard(m_lock);
        m_lastPurgeTime = SteadyClock::now();
        m_pendingToPurge = false;
        return true;
    } else {
        return doNotifyCorruptedEvent(parameter1, parameter2);
    }
}

#pragma mark - Purge
void ObservationQueue::observatedThatNeedPurged(uint32_t parameter)
{
    if (m_event != nullptr) {
        bool pending = false;
        {
            SharedLockGuard lockGuard(m_lock);
            if (!m_pendingToPurge
                && SteadyClock::now().timeIntervalSinceSteadyClock(m_lastPurgeTime)
                   > ObservationQueueTimeIntervalForPurgingAgain) {
                pending = true;
            }
        }
        {
            if (pending) {
                LockGuard lockGuard(m_lock);
                if (m_pendingToPurge) {
                    pending = false;
                } else {
                    m_pendingToPurge = true;
                }
            }
        }
        if (pending) {
            m_pendings.reQueue(nullptr, 0, parameter); // reQueue nullptr means a purge event
            lazyRun();
        }
    }
}

void ObservationQueue::observatedThatFileOpened(int fd)
{
    static int s_maxAllowedNumberOfFileDescriptors = std::max(getdtablesize(), 1024);
    if (fd >= 0) {
        int possibleNumberOfActiveFileDescriptors = fd + 1;
        if (possibleNumberOfActiveFileDescriptors
            > s_maxAllowedNumberOfFileDescriptors * ObservationQueueRateForTooManyFileDescriptors) {
            observatedThatNeedPurged(possibleNumberOfActiveFileDescriptors);
        }
    } else if (errno == EMFILE) {
        observatedThatNeedPurged(s_maxAllowedNumberOfFileDescriptors);
    }
}

#pragma mark - Corrupt
bool ObservationQueue::doNotifyCorruptedEvent(const String& path, uint32_t identifier)
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
        path, ObservationQueueTimeIntervalForReinvokingCorruptedEvent, identifier);
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
    if (iter == infos.end() || iter->second.empty()) {
        // make sure no empty path will be added into queue
        return;
    }
    const String& path = iter->second;
    bool succeed;
    uint32_t identifier;
    std::tie(succeed, identifier) = FileManager::getFileIdentifier(path);
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
