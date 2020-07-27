//
// Created by sanhuazhang on 2019/05/19
//

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

#include <WCDB/HandleRelated.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/SQLiteDeclaration.h>
#include <WCDB/UniqueList.hpp>
#include <functional>
#include <map>

namespace WCDB {

class HandleNotification final : public HandleRelated {
public:
    using HandleRelated::HandleRelated;
    ~HandleNotification() override final;

    void purge();

#pragma mark - Trace
private:
    void postTraceNotification(unsigned int flag, void *P, void *X);
    void setupTraceNotifications();
    static int traced(unsigned int T, void *C, void *P, void *X);

#pragma mark - SQL
public:
    typedef std::function<void(const UnsafeStringView &sql, const void* handle)> SQLNotification;
    void setNotificationWhenSQLTraced(const UnsafeStringView &name,
                                      const SQLNotification &onTraced);

private:
    bool areSQLTraceNotificationsSet() const;
    void postSQLTraceNotification(const UnsafeStringView &sql, const void* handle);
    StringViewMap<SQLNotification> m_sqlNotifications;

#pragma mark - Performance
public:
    typedef std::function<void(const UnsafeStringView &sql, double cost, const void* handle)> PerformanceNotification;
    void setNotificationWhenPerformanceTraced(const UnsafeStringView &name,
                                              const PerformanceNotification &onTraced);

private:
    bool arePerformanceTraceNotificationsSet() const;
    void postPerformanceTraceNotification(const UnsafeStringView &sql,
                                          const int64_t &cost,
                                          const void* handle);
    StringViewMap<PerformanceNotification> m_performanceNotifications;

#pragma mark - Committed
public:
    //committed dispatch will abort if any notification return false
    typedef std::function<bool(const UnsafeStringView &path, int numberOfFrames)> CommittedNotification;
    void setNotificationWhenCommitted(int order,
                                      const UnsafeStringView &name,
                                      const CommittedNotification &onCommitted);
    void unsetNotificationWhenCommitted(const UnsafeStringView &name);

private:
    static int committed(void *p, sqlite3 *, const char *, int numberOfFrames);

    bool isCommittedNotificationSet() const;
    void setupCommittedNotification();

    void postCommittedNotification(const UnsafeStringView &path, int numberOfFrames);
    UniqueList<StringView, CommittedNotification> m_committedNotifications;

#pragma mark - Checkpoint
public:
    typedef std::function<void(const UnsafeStringView &path)> CheckpointedNotification;
    void setNotificationWhenCheckpointed(const UnsafeStringView &name,
                                         const CheckpointedNotification &checkpointed);

private:
    static void checkpointed(void *p, sqlite3 *handle, const char *name);

    bool areCheckpointNotificationsSet() const;
    void setupCheckpointNotifications();
    void postCheckpointNotification(const UnsafeStringView &path);
    StringViewMap<CheckpointedNotification> m_checkpointedNotifications;

#pragma mark - Busy
public:
    typedef std::function<bool(const UnsafeStringView &path, int numberOfTimes)> BusyNotification;
    void setNotificationWhenBusy(const BusyNotification &busyNotification);

private:
    static int onBusy(void *p, int numberOfTimes);
    bool postBusyNotification(int numberOfTimes);
    BusyNotification m_busyNotification;
};

} //namespace WCDB
