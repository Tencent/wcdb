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

#ifndef __WCDB_OBSERVATION_QUEUE_HPP
#define __WCDB_OBSERVATION_QUEUE_HPP

#include <WCDB/AsyncQueue.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/TimedQueue.hpp>
#include <map>
#include <set>

namespace WCDB {

class ObservationQueueEvent {
public:
    virtual ~ObservationQueueEvent() = 0;

protected:
    virtual void observatedThatNeedPurged() = 0;
    friend class ObservationQueue;
};

class ObservationQueue;
class ObservationDelegate {
public:
    virtual ~ObservationDelegate() = 0;

protected:
    void observatedThatFileOpened(int fd);
    virtual ObservationQueue* observationQueue() = 0;
};

class ObservationQueue final : public AsyncQueue {
public:
    ObservationQueue(const String& name, ObservationQueueEvent* event);
    ~ObservationQueue();

protected:
    // When the parameter1 is empty, it indicates a `purge` work. Parameters would be the reason to purge. In this case, if parameter2 is greater than 0, then it triggered by too much file descriptor, while the value would be the number of file descriptor. Or if parameter2 equals to 0, then it triggered by memory warning.
    // Otherwise it's a path -> identifier pair that indicates the corrupted database that is pending to be notified
    TimedQueue<String, uint32_t> m_pendings;
    bool onTimed(const String& parameter1, const uint32_t& parameter2);

    void loop() override final;
    mutable SharedLock m_lock;
    ObservationQueueEvent* m_event;

#pragma mark - Purge
protected:
    void observatedThatNeedPurged(uint32_t parameter);

private:
    void* registerNotificationWhenMemoryWarning();
    void unregisterNotificationWhenMemoryWarning(void* observer);
    void* m_observerForMemoryWarning;

    void observatedThatFileOpened(int fd);
    friend class ObservationDelegate;

    SteadyClock m_lastPurgeTime;
    bool m_pendingToPurge;

#pragma mark - Corruption
public:
    // return true to mark as resolved
    typedef std::function<bool(const String& path, uint32_t identifier)> Notification;
    void setNotificationWhenCorrupted(const String& path, const Notification& notification);

    bool isFileObservedCorrupted(const String& path) const;

    void markAsObservedNotCorrupted(const String& path);

protected:
    // return true to mark as resolved
    bool doNotifyCorruptedEvent(const String& path, uint32_t identifier);
    void handleError(const Error& error);
    void preprocessError(Error& error);

    // path -> user notification
    std::map<String, Notification> m_notifications;

    // identifier of the corrupted database file -> the times of ignored corruption
    // it will be kept forever in memory since the identifier will be changed after removed/recovered
    std::set<uint32_t> m_corrupteds;
    std::map<uint32_t, int> m_numberOfIgnoredCorruptions;
};

} //namespace WCDB

#endif /* __WCDB_CORRUPTION_QUEUE_HPP */
