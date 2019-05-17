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
ObservationQueue::Parameter::Parameter(Source source_)
: source(source_), identifier(0), numberOfFileDescriptors(0)
{
}

ObservationQueue::ObservationQueue(const String& name, ObservationQueueEvent* event)
: AsyncQueue(name)
, m_event(event)
, m_observerForMemoryWarning(registerNotificationWhenMemoryWarning())
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

bool ObservationQueue::onTimed(const String& key, const Parameter& parameter)
{
    WCTInnerAssert(m_event != nullptr);

    if (parameter.source == Parameter::Source::MemoryWarning
        || parameter.source == Parameter::Source::FileDescriptorsWarning
        || parameter.source == Parameter::Source::OutOfMaxAllowedFileDescriptors) {
        WCTInnerAssert(key == s_purgeKey);

        if (parameter.source == Parameter::Source::MemoryWarning) {
            Error error(Error::Code::Warning, Error::Level::Warning, "Purge due to memory warning.");
            Notifier::shared()->notify(error);
        } else if (parameter.source == Parameter::Source::FileDescriptorsWarning) {
            Error error(Error::Code::Warning, Error::Level::Warning, "Purge due to file descriptors warning.");
            error.infos.set("MaxAllowedFileDescriptors",
                            maxAllowedNumberOfFileDescriptors());
            error.infos.set("FileDescriptor", parameter.numberOfFileDescriptors);
            Notifier::shared()->notify(error);
        } else if (parameter.source == Parameter::Source::OutOfMaxAllowedFileDescriptors) {
            Error error(Error::Code::Warning,
                        Error::Level::Warning,
                        "Purge due to out of max allowed file descriptors.");
            error.infos.set("MaxAllowedFileDescriptors",
                            maxAllowedNumberOfFileDescriptors());
            Notifier::shared()->notify(error);
        }

        // do purge
        m_event->observatedThatNeedPurge();

        LockGuard lockGuard(m_lock);
        m_lastPurgeTime = SteadyClock::now();
    } else if (parameter.source == Parameter::Source::Integrity) {
        WCTInnerAssert(key != s_purgeKey && key != s_notifyKey);
        m_event->observatedThatMayBeCorrupted(key);
    } else if (parameter.source == Parameter::Source::Notify) {
        WCTInnerAssert(key == s_notifyKey);
        std::map<uint32_t, std::pair<String, Notification>> notifys;
        {
            SharedLockGuard lockGuard(m_lock);
            notifys = m_notifys;
        }
        std::set<uint32_t> resolveds;
        for (const auto& iter : notifys) {
            if (iter.second.second != nullptr) {
                if (iter.second.second(iter.second.first, iter.first)) {
                    resolveds.emplace(iter.first);
                }
            }
        }
        {
            LockGuard lockGuard(m_lock);
            for (const auto& resolved : resolveds) {
                m_notifys.erase(resolved);
            }
        }
        if (resolveds.size() != notifys.size()) {
            // some are not resolved
            m_pendings.queue(s_notifyKey,
                             ObservationQueueTimeIntervalForReinvokingCorruptedEvent,
                             Parameter(Parameter::Source::Notify));
            return false;
        }
    }
    return true;
}

#pragma mark - Purge
int ObservationQueue::maxAllowedNumberOfFileDescriptors()
{
    const int s_maxAllowedNumberOfFileDescriptors = std::max(getdtablesize(), 1024);
    return s_maxAllowedNumberOfFileDescriptors;
}

void ObservationQueue::observatedThatNeedPurge(const Parameter& parameter)
{
    if (m_event != nullptr) {
        bool queue = false;
        {
            SharedLockGuard lockGuard(m_lock);
            if (SteadyClock::now().timeIntervalSinceSteadyClock(m_lastPurgeTime)
                > ObservationQueueTimeIntervalForPurgingAgain) {
                queue = true;
            }
        }
        if (queue) {
            m_pendings.queue(s_purgeKey, 0, parameter, false);
            lazyRun();
        }
    }
}

void ObservationQueue::observatedThatFileOpened(int fd)
{
    if (fd >= 0) {
        int possibleNumberOfActiveFileDescriptors = fd + 1;
        if (possibleNumberOfActiveFileDescriptors
            > maxAllowedNumberOfFileDescriptors() * ObservationQueueRateForTooManyFileDescriptors) {
            Parameter parameter(Parameter::Source::FileDescriptorsWarning);
            parameter.numberOfFileDescriptors = possibleNumberOfActiveFileDescriptors;
            observatedThatNeedPurge(parameter);
        }
    } else if (errno == EMFILE) {
        Parameter parameter(Parameter::Source::OutOfMaxAllowedFileDescriptors);
        observatedThatNeedPurge(parameter);
    }
}

#pragma mark - Corrupt
void ObservationQueue::handleError(const Error& error)
{
    if (!error.isCorruption() || isExiting()) {
        return;
    }

    const auto& infos = error.infos.getStrings();

    auto iter = infos.find(ErrorStringKeyPath);
    if (iter == infos.end() || iter->second.empty()) {
        // make sure no empty path will be added into queue
        return;
    }
    const String& path = iter->second;

    bool fromIntegrity = false;
    {
        const auto& stringInfos = error.infos.getStrings();
        auto actionIter = stringInfos.find(ErrorStringKeyAction);
        if (actionIter != stringInfos.end() && actionIter->second == ErrorActionIntegrity) {
            fromIntegrity = true;
        }
    }

    if (fromIntegrity) {
        // it's already corrupted
        bool succeed;
        uint32_t identifier;
        std::tie(succeed, identifier) = FileManager::getFileIdentifier(path);
        if (!succeed) {
            return;
        }

        {
            SharedLockGuard lockGuard(m_lock);
            if (m_corrupteds.find(identifier) != m_corrupteds.end()) {
                // already received
                return;
            }
        }
        {
            LockGuard lockGuard(m_lock);
            if (m_corrupteds.emplace(identifier).second) {
                Notification notification = nullptr;
                auto notificationIter = m_notifications.find(path);
                if (notificationIter != m_notifications.end()) {
                    notification = notificationIter->second;
                }
                m_notifys[identifier] = { path, notification };
            }
        }
        m_pendings.queue(s_notifyKey, 0, Parameter(Parameter::Source::Notify));
    } else {
        // dispatch to check integrity
        Parameter parameter(Parameter::Source::Integrity);
        m_pendings.queue(path, 0, parameter, false);
        lazyRun();
    }
}

bool ObservationQueue::isFileObservedCorrupted(const String& path) const
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
