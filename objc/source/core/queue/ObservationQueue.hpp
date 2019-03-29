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

class ObservationQueue final : public AsyncQueue {
public:
    ObservationQueue(const String& name);
    ~ObservationQueue();

protected:
    // path -> identifier of the corrupted database that is pending to be notified
    TimedQueue<String, uint32_t> m_pendings;

    bool onTimed(const String& path, const uint32_t& identifier);
    void loop() override final;
    SharedLock m_lock;

#pragma mark - Corruption
public:
    // return true to mark as resolved
    typedef std::function<bool(const String& path, uint32_t identifier)> Notification;
    void setNotificationWhenCorrupted(const String& path, const Notification& notification);

    bool isFileObservedCorrupted(const String& path);

protected:
    // return true to mark as resolved
    bool notifyCorruptedEvent(const String& path, uint32_t identifier);
    void handleError(const Error& error);

    // path -> user notification
    std::map<String, Notification> m_notifications;

    // identifier of the corrupted database file
    // it will be kept forever in memory since the identifier will be changed after removed/recovered
    std::set<uint32_t> m_corrupteds;
};

} //namespace WCDB

#endif /* __WCDB_CORRUPTION_QUEUE_HPP */
