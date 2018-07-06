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

#ifndef HandleNotification_hpp
#define HandleNotification_hpp

#include <WCDB/HandleRelated.hpp>
#include <list>

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
    typedef std::function<void(const std::string &)> SQLNotification;
    void setNotificationWhenSQLTraced(const std::string &name, const SQLNotification &onTraced);

protected:
    void dispatchSQLTraceNotification(const std::string &sql);
    std::map<std::string, SQLNotification> m_sqlNotifications;

#pragma mark - Performance
public:
    struct Footprint {
        std::string sql;
        unsigned int frequency;
        Footprint(const std::string &sql);
    };
    typedef struct Footprint Footprint;

    using Footprints = std::list<Footprint>;
    typedef std::function<void(const Footprints &, const int64_t &)> PerformanceNotification;
    void setNotificationWhenPerformanceTraced(const std::string &name,
                                              const PerformanceNotification &onTraced);

protected:
    void dispatchPerformanceTraceNotification(const std::string &sql,
                                              const int64_t &cost,
                                              bool isInTransaction);

    Footprints m_footprints;
    int64_t m_cost;
    std::map<std::string, PerformanceNotification> m_performanceNotifications;

#pragma mark - Committed
public:
    typedef std::function<void(Handle *, int)> CommittedNotification;
    void setNotificationWhenCommitted(const std::string &name,
                                      const CommittedNotification &onCommitted);

protected:
    bool isCommittedNotificationSet() const;
    void setupCommittedNotification();

    void dispatchCommittedNotification(int frames);
    std::map<std::string, CommittedNotification> m_commitedNotifications;

#pragma mark - Checkpoint
public:
    typedef std::function<bool(Handle *, int)> CheckpointNotification;
    bool setNotificationWhenCheckpoint(const std::string &name,
                                       const CheckpointNotification &willCheckpoint,
                                       bool ignorable = false);

protected:
    bool isCheckpointNotificationSet() const;
    bool setupCheckpointNotification(bool ignorable = false);
    bool dispatchCheckpointNotification(int frames);
    std::map<std::string, CheckpointNotification> m_checkpointNotifications;
};

} //namespace WCDB

#endif /* HandleNotification_hpp */
