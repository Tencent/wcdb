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

#ifndef _WCDB_HANDLENOTIFICATION_HPP
#define _WCDB_HANDLENOTIFICATION_HPP

#include <WCDB/HandleRelated.hpp>
#include <WCDB/OrderedUniqueList.hpp>
#include <functional>
#include <map>

namespace WCDB {

class HandleNotification : public HandleRelated {
public:
    using HandleRelated::HandleRelated;

    void purge();

#pragma mark - Trace
protected:
    bool isTraceNotificationSet() const;
    void dispatchTraceNotification(unsigned int flag, void *P, void *X);
    void setupTraceNotification();

#pragma mark - SQL
public:
    typedef std::function<void(const String &)> SQLNotification;
    void setNotificationWhenSQLTraced(const String &name, const SQLNotification &onTraced);

protected:
    void dispatchSQLTraceNotification(const String &sql);
    std::map<String, SQLNotification> m_sqlNotifications;

#pragma mark - Performance
public:
    struct Footprint {
        String sql;
        unsigned int frequency;
        Footprint(const String &sql);
    };
    typedef struct Footprint Footprint;

    using Footprints = std::list<Footprint>;
    typedef std::function<void(const Footprints &, const int64_t &)> PerformanceNotification;
    void setNotificationWhenPerformanceTraced(const String &name,
                                              const PerformanceNotification &onTraced);

protected:
    void dispatchPerformanceTraceNotification(const String &sql,
                                              const int64_t &cost,
                                              bool isInTransaction);

    Footprints m_footprints;
    int64_t m_cost;
    std::map<String, PerformanceNotification> m_performanceNotifications;

#pragma mark - Committed
public:
    //commited dispatch will abort if any notification return false
    typedef std::function<bool(Handle *, int)> CommittedNotification;
    void setNotificationWhenCommitted(int order,
                                      const String &name,
                                      const CommittedNotification &onCommitted);
    void unsetNotificationWhenCommitted(const String &name);

protected:
    bool isCommittedNotificationSet() const;
    void setupCommittedNotification();

    void dispatchCommittedNotification(int frames);
    OrderedUniqueList<String, CommittedNotification> m_commitedNotifications;

#pragma mark - Checkpoint
public:
    //checkpoint will abort if any notification return false
    typedef std::function<bool(Handle *)> WillCheckpointNotification;
    bool setNotificationWhenWillCheckpoint(int order,
                                           const String &name,
                                           const WillCheckpointNotification &willCheckpoint,
                                           bool ignorable = false);
    bool unsetNotificationWhenWillCheckpoint(const String &name, bool ignorable = false);

    typedef std::function<void(Handle *, int)> CheckpointedNotification;
    bool setNotificationWhenCheckpointed(const String &name,
                                         const CheckpointedNotification &checkpointed,
                                         bool ignorable = false);

protected:
    bool isCheckpointNotificationSet() const;
    bool setupCheckpointNotification(bool ignorable = false);
    bool dispatchCheckpointNotification(int rc);
    OrderedUniqueList<String, WillCheckpointNotification> m_willCheckpointNotifications;
    std::map<String, CheckpointedNotification> m_checkpointedNotifications;
};

} //namespace WCDB

#endif /* _WCDB_HANDLENOTIFICATION_HPP */
