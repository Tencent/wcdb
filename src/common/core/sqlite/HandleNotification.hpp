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

#include "HandleRelated.hpp"
#include "Lock.hpp"
#include "SQLiteDeclaration.h"
#include "Tag.hpp"
#include "UniqueList.hpp"
#include "WCDBOptional.hpp"
#include <functional>
#include <map>

namespace WCDB {

class HandleNotification final : public HandleRelated {
public:
    using HandleRelated::HandleRelated;
    ~HandleNotification() override;

    void purge();

#pragma mark - Trace
private:
    void postTraceNotification(unsigned int flag, void *P, void *X);
    void setupTraceNotifications();
    static int traced(unsigned int T, void *C, void *P, void *X);

#pragma mark - SQL
public:
    typedef std::function<void(const Tag &tag, const UnsafeStringView &path, const void *handle, const UnsafeStringView &sql, const UnsafeStringView &info)> SQLNotification;
    void setNotificationWhenSQLTraced(const UnsafeStringView &name,
                                      const SQLNotification &onTraced);

    void postSQLTraceNotification(const Tag &tag,
                                  const UnsafeStringView &path,
                                  const void *handle,
                                  const UnsafeStringView &sql,
                                  const UnsafeStringView &info);

    void setFullSQLTraceEnable(bool enable);

private:
    bool areSQLTraceNotificationsSet() const;
    StringViewMap<SQLNotification> m_sqlNotifications;
    bool m_fullSQLTrace = false;

#pragma mark - Performance
public:
    typedef struct PerformanceInfo {
        int tablePageReadCount;
        int tablePageWriteCount;
        int indexPageReadCount;
        int indexPageWriteCount;
        int overflowPageReadCount;
        int overflowPageWriteCount;
        int64_t costInNanoseconds;
    } PerformanceInfo;
    typedef std::function<void(const Tag &tag, const UnsafeStringView &path, const void *handle, const UnsafeStringView &sql, PerformanceInfo info)> PerformanceNotification;
    void setNotificationWhenPerformanceTraced(const UnsafeStringView &name,
                                              const PerformanceNotification &onTraced);

private:
    bool arePerformanceTraceNotificationsSet() const;
    void postPerformanceTraceNotification(const Tag &tag,
                                          const UnsafeStringView &path,
                                          const void *handle,
                                          const UnsafeStringView &sql,
                                          const PerformanceInfo &info);
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
    typedef std::function<void(AbstractHandle *handle, uint32_t nBackFill, uint32_t mxFrame, uint32_t salt1, uint32_t salt2)> CheckpointBeginNotification;
    typedef std::function<void(AbstractHandle *handle, uint32_t pageNo, const UnsafeData &data)> CheckpointPageNotification;
    typedef std::function<void(AbstractHandle *handle, uint32_t nBackFill, uint32_t mxFrame, uint32_t salt1, uint32_t salt2)> CheckpointFinishNotification;
    typedef struct CheckPointNotification {
        CheckpointBeginNotification begin;
        CheckpointPageNotification page;
        CheckpointFinishNotification finish;
    } CheckPointNotification;
    void setNotificationWhenCheckpointed(const UnsafeStringView &name,
                                         Optional<CheckPointNotification> notification);

private:
    bool areCheckpointNotificationsSet() const;
    void setupCheckpointNotifications();

    static void
    checkpointBegin(void *ctx, int nBackFill, int mxFrame, int salt1, int salt2);
    void postCheckpointBeginNotification(AbstractHandle *handle,
                                         uint32_t nBackFill,
                                         uint32_t mxFrame,
                                         uint32_t salt1,
                                         uint32_t salt2);

    static void checkpointPage(void *ctx, int pageNo, void *data, int size);
    void postCheckpointPageNotification(AbstractHandle *handle,
                                        uint32_t pageNo,
                                        const UnsafeData &data);

    static void
    checkpointFinish(void *ctx, int nBackFill, int mxFrame, int salt1, int salt2);
    void postCheckpointFinishNotification(AbstractHandle *handle,
                                          uint32_t nBackFill,
                                          uint32_t mxFrame,
                                          uint32_t salt1,
                                          uint32_t salt2);

    StringViewMap<CheckPointNotification> m_checkpointedNotifications;

#pragma mark - Busy
public:
    typedef std::function<bool(const UnsafeStringView &path, int numberOfTimes)> BusyNotification;
    void setNotificationWhenBusy(const BusyNotification &busyNotification);

private:
    static int onBusy(void *p, int numberOfTimes);
    bool postBusyNotification(int numberOfTimes);
    BusyNotification m_busyNotification;

#pragma mark - Table Modification
public:
    typedef std::function<void(const UnsafeStringView &path, const UnsafeStringView &newTable, const UnsafeStringView &modifiedTable)> TableModifiedNotification;
    void setNotificationWhenTableModified(const UnsafeStringView &name,
                                          const TableModifiedNotification &tableModified);
    void postTableModifiedNotification(const UnsafeStringView &newTable,
                                       const UnsafeStringView &modifiedTable);
    bool needMonitorTable();

private:
    StringViewMap<TableModifiedNotification> m_tableModifiedNotifications;
};

} //namespace WCDB
