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

#include <WCDB/AbstractHandle.hpp>
#include <WCDB/Assertion.hpp>
#include <WCDB/HandleNotification.hpp>
#include <WCDB/SQLite.h>
#include <WCDB/String.hpp>

namespace WCDB {

void HandleNotification::purge()
{
    bool isOpened = m_handle->isOpened();
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
            postSQLTraceNotification(sql);
        }
    } break;
    case SQLITE_TRACE_PROFILE: {
        const char *sql = sqlite3_sql(stmt);
        sqlite3_int64 *cost = (sqlite3_int64 *) X;
        sqlite3 *db = sqlite3_db_handle(stmt);
        postPerformanceTraceNotification(sql, *cost, !sqlite3_get_autocommit(db));
    } break;
    default:
        break;
    }
}

void HandleNotification::setupTraceNotifications()
{
    unsigned flag = 0;
    if (!m_sqlNotifications.empty()) {
        flag |= SQLITE_TRACE_STMT;
    }
    if (!m_performanceNotifications.empty()) {
        flag |= SQLITE_TRACE_PROFILE;
    }
    if (flag != 0) {
        exitAPI(sqlite3_trace_v2(getRawHandle(), flag, HandleNotification::traced, this));
    } else {
        exitAPI(sqlite3_trace_v2(getRawHandle(), 0, nullptr, nullptr));
    }
}

#pragma mark - SQL
bool HandleNotification::areSQLTraceNotificationsSet() const
{
    return !m_sqlNotifications.empty();
}

void HandleNotification::setNotificationWhenSQLTraced(const String &name,
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

void HandleNotification::postSQLTraceNotification(const String &sql)
{
    WCTInnerAssert(!m_sqlNotifications.empty());
    for (const auto &element : m_sqlNotifications) {
        element.second(sql);
    }
}

#pragma mark - Performance
HandleNotification::Footprint::Footprint(const String &sql_)
: sql(sql_), frequency(1)
{
}

bool HandleNotification::arePerformanceTraceNotificationsSet() const
{
    return !m_performanceNotifications.empty();
}

void HandleNotification::setNotificationWhenPerformanceTraced(const String &name,
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

void HandleNotification::postPerformanceTraceNotification(const String &sql,
                                                          const int64_t &cost,
                                                          bool isInTransaction)
{
    if (!m_footprints.empty()) {
        auto &footprint = m_footprints.back();
        if (footprint.sql == sql) {
            ++footprint.frequency;
            return;
        }
    }
    m_footprints.push_back(Footprint(sql));
    m_cost += cost;

    if (!isInTransaction) {
        WCTInnerAssert(!m_performanceNotifications.empty());
        for (const auto &element : m_performanceNotifications) {
            element.second(m_footprints, m_cost);
        }
        m_footprints.clear();
        m_cost = 0;
    }
}

#pragma mark - Committed
int HandleNotification::committed(void *p, sqlite3 *, const char *, int numberOfFrames)
{
    HandleNotification *notification = reinterpret_cast<HandleNotification *>(p);
    notification->postCommittedNotification(numberOfFrames);
    return SQLITE_OK;
}

void HandleNotification::setNotificationWhenCommitted(int order,
                                                      const String &name,
                                                      const CommittedNotification &onCommitted)
{
    WCTInnerAssert(onCommitted);
    bool stateBefore = isCommittedNotificationSet();
    m_committedNotifications.insert(order, name, onCommitted);
    bool stateAfter = isCommittedNotificationSet();
    if (stateBefore != stateAfter) {
        setupCommittedNotification();
    }
}

void HandleNotification::unsetNotificationWhenCommitted(const String &name)
{
    bool stateBefore = isCommittedNotificationSet();
    m_committedNotifications.erase(name);
    bool stateAfter = isCommittedNotificationSet();
    if (stateBefore != stateAfter) {
        setupCommittedNotification();
    }
}

void HandleNotification::setupCommittedNotification()
{
    if (!m_committedNotifications.elements().empty()) {
        sqlite3_wal_hook(getRawHandle(), HandleNotification::committed, this);
    } else {
        sqlite3_wal_hook(getRawHandle(), nullptr, nullptr);
    }
}

bool HandleNotification::isCommittedNotificationSet() const
{
    return !m_committedNotifications.elements().empty();
}

void HandleNotification::postCommittedNotification(int numberOfFrames)
{
    WCTInnerAssert(!m_committedNotifications.elements().empty());
    for (const auto &element : m_committedNotifications.elements()) {
        if (!element.value(m_handle->getPath(), numberOfFrames)) {
            break;
        }
    }
}

#pragma mark - Checkpoint
void HandleNotification::checkpointed(void *p)
{
    HandleNotification *notification = reinterpret_cast<HandleNotification *>(p);
    notification->postCheckpointNotification();
}

bool HandleNotification::areCheckpointNotificationsSet() const
{
    return !m_checkpointedNotifications.empty();
}

void HandleNotification::setupCheckpointNotifications()
{
    if (!m_checkpointedNotifications.empty()) {
        exitAPI(sqlite3_wal_checkpoint_handler(
        getRawHandle(), HandleNotification::checkpointed, this));
    } else {
        exitAPI(sqlite3_wal_checkpoint_handler(getRawHandle(), nullptr, nullptr));
    }
}

void HandleNotification::setNotificationWhenCheckpointed(const String &name,
                                                         const CheckpointedNotification &checkpointed)
{
    bool stateBefore = areCheckpointNotificationsSet();
    if (checkpointed != nullptr) {
        m_checkpointedNotifications.emplace(name, checkpointed);
    } else {
        m_checkpointedNotifications.erase(name);
    }
    bool stateAfter = areCheckpointNotificationsSet();
    if (stateBefore != stateAfter) {
        setupCheckpointNotifications();
    }
}

void HandleNotification::postCheckpointNotification()
{
    WCTInnerAssert(areCheckpointNotificationsSet());
    for (const auto &element : m_checkpointedNotifications) {
        element.second(m_handle->getPath());
    }
}

#pragma mark - Busy
int HandleNotification::busyRetry(void *p, int numberOfTimes)
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
        exitAPI(sqlite3_busy_handler(getRawHandle(), HandleNotification::busyRetry, this));
    } else {
        exitAPI(sqlite3_busy_handler(getRawHandle(), nullptr, nullptr));
    }
}

bool HandleNotification::postBusyNotification(int numberOfTimes)
{
    WCTInnerAssert(m_busyNotification != nullptr);
    bool retry = false;
    if (m_busyNotification != nullptr) {
        retry = m_busyNotification(m_handle->getPath(), numberOfTimes);
    }
    return retry;
}

#pragma mark - Did Step
void HandleNotification::setNotificationWhenStatementDidStep(const String &name,
                                                             const StatementDidStepNotification &notification)
{
    m_didStepNotifications.emplace(name, notification);
}

void HandleNotification::statementDidStep(HandleStatement *handleStatement, bool result)
{
    for (const auto &iter : m_didStepNotifications) {
        WCTInnerAssert(iter.second != nullptr);
        if (iter.second != nullptr) {
            iter.second(handleStatement, result);
        }
    }
}

#pragma mark - Will Step
void HandleNotification::setNotificationWhenStatementWillStep(
const String &name, const StatementWillStepNotification &notification)
{
    m_willStepNotifications.emplace(name, notification);
}

void HandleNotification::statementWillStep(HandleStatement *handleStatement)
{
    for (const auto &iter : m_willStepNotifications) {
        WCTInnerAssert(iter.second != nullptr);
        if (iter.second != nullptr) {
            iter.second(handleStatement);
        }
    }
}

} //namespace WCDB
