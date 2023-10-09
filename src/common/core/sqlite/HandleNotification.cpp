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

#include "HandleNotification.hpp"
#include "AbstractHandle.hpp"
#include "Assertion.hpp"
#include "SQLite.h"
#include "StringView.hpp"

namespace WCDB {

HandleNotification::~HandleNotification() = default;

void HandleNotification::purge()
{
    bool isOpened = getHandle()->isOpened();
    bool set = areSQLTraceNotificationsSet() || arePerformanceTraceNotificationsSet();
    m_sqlNotifications.clear();
    m_performanceNotifications.clear();
    if (set && isOpened) {
        setupTraceNotifications();
    }

    set = isCommittedNotificationSet();
    m_committedNotifications.clear();
    if (set && isOpened) {
        setupCommittedNotification();
    }

    set = areCheckpointNotificationsSet();
    m_checkpointedNotifications.clear();
    if (set && isOpened) {
        setupCheckpointNotifications();
    }

    if (isOpened) {
        setNotificationWhenBusy(nullptr);
    }
}

#pragma mark - Trace
int HandleNotification::traced(unsigned int T, void *C, void *P, void *X)
{
    HandleNotification *notification = reinterpret_cast<HandleNotification *>(C);
    notification->postTraceNotification(T, P, X);
    return SQLITE_OK;
}

void HandleNotification::postTraceNotification(unsigned int flag, void *P, void *X)
{
    sqlite3_stmt *stmt = (sqlite3_stmt *) P;
    switch (flag) {
    case SQLITE_TRACE_STMT: {
        const char *sql = static_cast<const char *>(X);
        if (sql) {
            AbstractHandle *handle = getHandle();
            postSQLTraceNotification(
            handle->getTag(), handle->getPath(), getHandle(), sql, "");
        }
    } break;
    case SQLITE_TRACE_PROFILE: {
        const char *sql = sqlite3_sql(stmt);
        PerformanceInfo *info = (PerformanceInfo *) X;
        AbstractHandle *handle = getHandle();
        postPerformanceTraceNotification(
        handle->getTag(), handle->getPath(), getHandle(), sql, *info);
    } break;
    default:
        break;
    }
}

void HandleNotification::setupTraceNotifications()
{
    unsigned flag = 0;
    if (!m_sqlNotifications.empty() && !m_fullSQLTrace) {
        flag |= SQLITE_TRACE_STMT;
    }
    if (!m_performanceNotifications.empty()) {
        flag |= SQLITE_TRACE_PROFILE;
    }
    if (flag != 0) {
        APIExit(sqlite3_trace_v2(getRawHandle(), flag, HandleNotification::traced, this));
    } else {
        APIExit(sqlite3_trace_v2(getRawHandle(), 0, nullptr, nullptr));
    }
}

#pragma mark - SQL
void HandleNotification::setFullSQLTraceEnable(bool enable)
{
    bool before = m_fullSQLTrace;
    m_fullSQLTrace = enable;
    if (before != enable) {
        setupTraceNotifications();
    }
}

bool HandleNotification::areSQLTraceNotificationsSet() const
{
    return !m_sqlNotifications.empty();
}

void HandleNotification::setNotificationWhenSQLTraced(const UnsafeStringView &name,
                                                      const SQLNotification &onTraced)
{
    bool stateBefore = areSQLTraceNotificationsSet();
    if (onTraced != nullptr) {
        m_sqlNotifications[name] = onTraced;
    } else {
        m_sqlNotifications.erase(name);
    }
    bool stateAfter = areSQLTraceNotificationsSet();
    if (stateBefore != stateAfter) {
        setupTraceNotifications();
    }
}

void HandleNotification::postSQLTraceNotification(const Tag &tag,
                                                  const UnsafeStringView &path,
                                                  const void *handle,
                                                  const UnsafeStringView &sql,
                                                  const UnsafeStringView &info)
{
    for (const auto &element : m_sqlNotifications) {
        element.second(tag, path, handle, sql, info);
    }
}

#pragma mark - Performance

bool HandleNotification::arePerformanceTraceNotificationsSet() const
{
    return !m_performanceNotifications.empty();
}

void HandleNotification::setNotificationWhenPerformanceTraced(const UnsafeStringView &name,
                                                              const PerformanceNotification &onTraced)
{
    bool stateBefore = arePerformanceTraceNotificationsSet();
    if (onTraced != nullptr) {
        m_performanceNotifications[name] = onTraced;
    } else {
        m_performanceNotifications.erase(name);
    }
    bool stateAfter = arePerformanceTraceNotificationsSet();
    if (stateAfter != stateBefore) {
        setupTraceNotifications();
    }
}

void HandleNotification::postPerformanceTraceNotification(const Tag &tag,
                                                          const UnsafeStringView &path,
                                                          const void *handle,
                                                          const UnsafeStringView &sql,
                                                          const PerformanceInfo &info)
{
    WCTAssert(!m_performanceNotifications.empty());
    for (const auto &element : m_performanceNotifications) {
        element.second(tag, path, handle, sql, info);
    }
}

#pragma mark - Committed
int HandleNotification::committed(void *p, sqlite3 *handle, const char *name, int numberOfFrames)
{
    WCTAssert(p != nullptr);
    HandleNotification *notification = reinterpret_cast<HandleNotification *>(p);
    notification->postCommittedNotification(sqlite3_db_filename(handle, name), numberOfFrames);
    return SQLITE_OK;
}

void HandleNotification::setNotificationWhenCommitted(int order,
                                                      const UnsafeStringView &name,
                                                      const CommittedNotification &onCommitted)
{
    WCTAssert(onCommitted);
    bool stateBefore = isCommittedNotificationSet();
    m_committedNotifications.insert(StringView(name), onCommitted, order);
    bool stateAfter = isCommittedNotificationSet();
    if (stateBefore != stateAfter) {
        setupCommittedNotification();
    }
}

void HandleNotification::unsetNotificationWhenCommitted(const UnsafeStringView &name)
{
    bool stateBefore = isCommittedNotificationSet();
    m_committedNotifications.erase(StringView(name));
    bool stateAfter = isCommittedNotificationSet();
    if (stateBefore != stateAfter) {
        setupCommittedNotification();
    }
}

void HandleNotification::setupCommittedNotification()
{
    if (!m_committedNotifications.empty()) {
        sqlite3_wal_hook(getRawHandle(), HandleNotification::committed, this);
    } else {
        sqlite3_wal_hook(getRawHandle(), nullptr, nullptr);
    }
}

bool HandleNotification::isCommittedNotificationSet() const
{
    return !m_committedNotifications.empty();
}

void HandleNotification::postCommittedNotification(const UnsafeStringView &path, int numberOfFrames)
{
    WCTAssert(!m_committedNotifications.empty());
    for (const auto &element : m_committedNotifications) {
        if (!element.value()(path, numberOfFrames)) {
            break;
        }
    }
}

#pragma mark - Checkpoint

bool HandleNotification::areCheckpointNotificationsSet() const
{
    return !m_checkpointedNotifications.empty();
}

void HandleNotification::setupCheckpointNotifications()
{
    if (!m_checkpointedNotifications.empty()) {
        sqlite3_wal_checkpoint_handler(getRawHandle(),
                                       HandleNotification::checkpointBegin,
                                       HandleNotification::checkpointPage,
                                       HandleNotification::checkpointFinish,
                                       this);
    } else {
        sqlite3_wal_checkpoint_handler(getRawHandle(), nullptr, nullptr, nullptr, nullptr);
    }
}

void HandleNotification::setNotificationWhenCheckpointed(const UnsafeStringView &name,
                                                         Optional<CheckPointNotification> notification)
{
    bool stateBefore = areCheckpointNotificationsSet();
    if (notification.hasValue()) {
        m_checkpointedNotifications[name] = notification.value();
    } else {
        m_checkpointedNotifications.erase(name);
    }
    bool stateAfter = areCheckpointNotificationsSet();
    if (stateBefore != stateAfter) {
        setupCheckpointNotifications();
    }
}

void HandleNotification::checkpointBegin(void *ctx, int nBackFill, int mxFrame, int salt1, int salt2)
{
    WCTAssert(ctx != nullptr);
    HandleNotification *notification = reinterpret_cast<HandleNotification *>(ctx);
    notification->postCheckpointBeginNotification(
    notification->getHandle(), nBackFill, mxFrame, salt1, salt2);
}

void HandleNotification::postCheckpointBeginNotification(AbstractHandle *handle,
                                                         uint32_t nBackFill,
                                                         uint32_t mxFrame,
                                                         uint32_t salt1,
                                                         uint32_t salt2)
{
    WCTAssert(areCheckpointNotificationsSet());
    for (const auto &element : m_checkpointedNotifications) {
        if (element.second.begin == nullptr) {
            continue;
        }
        element.second.begin(handle, nBackFill, mxFrame, salt1, salt2);
    }
}

void HandleNotification::checkpointPage(void *ctx, int pageNo, void *data, int size)
{
    WCTAssert(ctx != nullptr);
    HandleNotification *notification = reinterpret_cast<HandleNotification *>(ctx);
    notification->postCheckpointPageNotification(
    notification->getHandle(), pageNo, UnsafeData((unsigned char *) data, size));
}

void HandleNotification::postCheckpointPageNotification(AbstractHandle *handle,
                                                        uint32_t pageNo,
                                                        const UnsafeData &data)
{
    WCTAssert(areCheckpointNotificationsSet());
    for (const auto &element : m_checkpointedNotifications) {
        if (element.second.page == nullptr) {
            continue;
        }
        element.second.page(handle, pageNo, data);
    }
}

void HandleNotification::checkpointFinish(void *ctx, int nBackFill, int mxFrame, int salt1, int salt2)
{
    WCTAssert(ctx != nullptr);
    HandleNotification *notification = reinterpret_cast<HandleNotification *>(ctx);
    notification->postCheckpointFinishNotification(
    notification->getHandle(), nBackFill, mxFrame, salt1, salt2);
}

void HandleNotification::postCheckpointFinishNotification(AbstractHandle *handle,
                                                          uint32_t nBackFill,
                                                          uint32_t mxFrame,
                                                          uint32_t salt1,
                                                          uint32_t salt2)
{
    WCTAssert(areCheckpointNotificationsSet());
    for (const auto &element : m_checkpointedNotifications) {
        if (element.second.finish == nullptr) {
            continue;
        }
        element.second.finish(handle, nBackFill, mxFrame, salt1, salt2);
    }
}

#pragma mark - Busy
int HandleNotification::onBusy(void *p, int numberOfTimes)
{
    HandleNotification *notification = reinterpret_cast<HandleNotification *>(p);
    int rc = SQLITE_OK;
    if (notification->postBusyNotification(numberOfTimes)) {
        // return any non-zero value to retry
        rc = SQLITE_BUSY;
    }
    return rc;
}

void HandleNotification::setNotificationWhenBusy(const BusyNotification &busyNotification)
{
    m_busyNotification = busyNotification;
    if (m_busyNotification != nullptr) {
        APIExit(sqlite3_busy_handler(getRawHandle(), HandleNotification::onBusy, this));
    } else {
        APIExit(sqlite3_busy_handler(getRawHandle(), nullptr, nullptr));
    }
}

bool HandleNotification::postBusyNotification(int numberOfTimes)
{
    WCTAssert(m_busyNotification != nullptr);
    bool retry = false;
    if (m_busyNotification != nullptr) {
        retry = m_busyNotification(getHandle()->getPath(), numberOfTimes);
    }
    return retry;
}

#pragma mark - Table Modification
void HandleNotification::setNotificationWhenTableModified(const UnsafeStringView &name,
                                                          const TableModifiedNotification &tableModified)
{
    if (tableModified != nullptr) {
        m_tableModifiedNotifications[name] = tableModified;
    } else {
        m_tableModifiedNotifications.erase(name);
    }
}

void HandleNotification::postTableModifiedNotification(const UnsafeStringView &newTable,
                                                       const UnsafeStringView &modifiedTable)
{
    WCTAssert(!m_tableModifiedNotifications.empty());
    for (const auto &element : m_tableModifiedNotifications) {
        element.second(getHandle()->getPath(), newTable, modifiedTable);
    }
}

bool HandleNotification::needMonitorTable()
{
    return !m_tableModifiedNotifications.empty();
}

} //namespace WCDB
