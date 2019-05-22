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

#pragma once

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
    virtual void observatedThatMayBeCorrupted(const String& path) = 0; // check integrity
    virtual void observatedThatNeedPurge() = 0;
    friend class ObservationQueue;
};

class ObservationQueue final : public AsyncQueue {
public:
    ObservationQueue(const String& name, ObservationQueueEvent* event);
    ~ObservationQueue();

protected:
    struct Parameter {
        enum class Source {
            Integrity,
            MemoryWarning,
            FileDescriptorsWarning,
            OutOfMaxAllowedFileDescriptors,
            Notify,
        };

        Parameter(Source source);
        const Source source;

        // it's redundant.
        uint32_t identifier;
        uint32_t numberOfFileDescriptors;
    };
    typedef struct Parameter Parameter;

    static constexpr const char* s_purgeKey = "@purge";
    static constexpr const char* s_notifyKey = "@notify";

    TimedQueue<String, Parameter> m_pendings;
    bool onTimed(const String& path, const Parameter& parameter);

    void loop() override final;
    mutable SharedLock m_lock;
    ObservationQueueEvent* m_event;

#pragma mark - Purge
protected:
    void observatedThatNeedPurge(const Parameter& parameter);

private:
    static int maxAllowedNumberOfFileDescriptors();

    void* registerNotificationWhenMemoryWarning();
    void unregisterNotificationWhenMemoryWarning(void* observer);
    void* m_observerForMemoryWarning;

    void observatedThatFileOpened(int fd, const char* path, int flags, int mode);

    SteadyClock m_lastPurgeTime;

#pragma mark - Corruption
public:
    // return true to mark as resolved
    typedef std::function<bool(const String& path, uint32_t identifier)> Notification;
    void setNotificationWhenCorrupted(const String& path, const Notification& notification);

    bool isFileObservedCorrupted(const String& path) const;

protected:
    void handleError(const Error& error);

    // path -> user notification
    std::map<String, Notification> m_notifications;
    std::map<uint32_t, std::pair<String, Notification>> m_notifys;

    // identifier of the corrupted database file -> the times of ignored corruption
    // it will be kept forever in memory since the identifier will be changed after removed/recovered
    std::set<uint32_t> m_corrupteds;
};

} //namespace WCDB
