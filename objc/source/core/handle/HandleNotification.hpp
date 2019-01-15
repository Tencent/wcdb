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

class HandleNotification final : public HandleRelated {
public:
    using HandleRelated::HandleRelated;

    void purge();

#pragma mark - Trace
protected:
    void dispatchTraceNotification(unsigned int flag, void *P, void *X);
    void setupTraceNotification();

#pragma mark - SQL
public:
    typedef std::function<void(const String & /* sql */)> SQLNotification;
    void setNotificationWhenSQLTraced(const String &name, const SQLNotification &onTraced);

protected:
    bool isSQLTraceNotificationSet() const;
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
    typedef std::function<void(const Footprints &, const int64_t & /* cost */)> PerformanceNotification;
    void setNotificationWhenPerformanceTraced(const String &name,
                                              const PerformanceNotification &onTraced);

protected:
    bool isPerformanceTraceNotificationSet() const;
    void dispatchPerformanceTraceNotification(const String &sql,
                                              const int64_t &cost,
                                              bool isInTransaction);

    Footprints m_footprints;
    int64_t m_cost = 0;
    std::map<String, PerformanceNotification> m_performanceNotifications;

#pragma mark - Committed
public:
    //committed dispatch will abort if any notification return false
    typedef std::function<bool(const String & /* path */, int /* frame */)> CommittedNotification;
    void setNotificationWhenCommitted(int order,
                                      const String &name,
                                      const CommittedNotification &onCommitted);
    void unsetNotificationWhenCommitted(const String &name);

protected:
    bool isCommittedNotificationSet() const;
    void setupCommittedNotification();

    void dispatchCommittedNotification(int frames);
    OrderedUniqueList<String, CommittedNotification> m_committedNotifications;

#pragma mark - Checkpoint
public:
    typedef std::function<void(const String & /* path */)> CheckpointedNotification;
    void setNotificationWhenCheckpointed(const String &name,
                                         const CheckpointedNotification &checkpointed);

protected:
    bool isCheckpointNotificationSet() const;
    void setupCheckpointNotification();
    void dispatchCheckpointNotification();
    std::map<String, CheckpointedNotification> m_checkpointedNotifications;
};

} //namespace WCDB

#endif /* _WCDB_HANDLENOTIFICATION_HPP */
