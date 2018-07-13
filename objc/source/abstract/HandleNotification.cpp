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

#include <WCDB/Abstract.h>
#include <WCDB/Assertion.hpp>
#include <WCDB/String.hpp>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

void HandleNotification::purge()
{
    bool set = isTraceNotificationSet();
    m_sqlNotifications.clear();
    m_performanceNotifications.clear();
    if (set) {
        setupTraceNotification();
    }
    set = isCommittedNotificationSet();
    m_commitedNotifications.clear();
    if (set) {
        setupCommittedNotification();
    }
    set = isCheckpointNotificationSet();
    m_checkpointNotifications.clear();
    if (set) {
        setupCheckpointNotification();
    }
}

#pragma mark - Trace
bool HandleNotification::isTraceNotificationSet() const
{
    return !m_sqlNotifications.empty() || !m_performanceNotifications.empty();
}

void HandleNotification::dispatchTraceNotification(unsigned int flag, void *P, void *X)
{
    sqlite3_stmt *stmt = (sqlite3_stmt *) P;
    const char *sql = sqlite3_sql(stmt);
    switch (flag) {
    case SQLITE_TRACE_STMT: {
        if (sql) {
            dispatchSQLTraceNotification(sql);
        }
    } break;
    case SQLITE_TRACE_PROFILE: {
        sqlite3_int64 *cost = (sqlite3_int64 *) X;
        sqlite3 *db = sqlite3_db_handle(stmt);
        dispatchPerformanceTraceNotification(
        sql ? sql : String::empty(), *cost, !sqlite3_get_autocommit(db));
    } break;
    default:
        break;
    }
}

void HandleNotification::setupTraceNotification()
{
    unsigned flag = 0;
    if (!m_sqlNotifications.empty()) {
        flag |= SQLITE_TRACE_STMT;
    }
    if (!m_performanceNotifications.empty()) {
        flag |= SQLITE_TRACE_PROFILE;
    }
    if (flag != 0) {
        sqlite3_trace_v2((sqlite3 *) getRawHandle(),
                         flag,
                         [](unsigned int T, void *C, void *P, void *X) {
                             HandleNotification *notification
                             = reinterpret_cast<HandleNotification *>(C);
                             notification->dispatchTraceNotification(T, P, X);
                             return SQLITE_OK;
                         },
                         this);
    } else {
        sqlite3_trace_v2((sqlite3 *) getRawHandle(), 0, nullptr, nullptr);
    }
}

#pragma mark - SQL
void HandleNotification::setNotificationWhenSQLTraced(const std::string &name,
                                                      const SQLNotification &onTraced)
{
    bool stateBefore = isTraceNotificationSet();
    if (onTraced) {
        m_sqlNotifications[name] = onTraced;
    } else {
        m_sqlNotifications.erase(name);
    }
    bool stateAfter = isTraceNotificationSet();
    if (stateBefore != stateAfter) {
        setupTraceNotification();
    }
}

void HandleNotification::dispatchSQLTraceNotification(const std::string &sql)
{
    WCTInnerAssert(!m_sqlNotifications.empty());
    for (const auto &element : m_sqlNotifications) {
        element.second(sql);
    }
}

#pragma mark - Performance
HandleNotification::Footprint::Footprint(const std::string &sql_)
: sql(sql_), frequency(1)
{
}

void HandleNotification::setNotificationWhenPerformanceTraced(const std::string &name,
                                                              const PerformanceNotification &onTraced)
{
    bool stateBefore = isTraceNotificationSet();
    if (onTraced) {
        m_performanceNotifications[name] = onTraced;
    } else {
        m_performanceNotifications.erase(name);
    }
    bool stateAfter = isTraceNotificationSet();
    if (stateAfter != stateBefore) {
        setupTraceNotification();
    }
}

void HandleNotification::dispatchPerformanceTraceNotification(const std::string &sql,
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
void HandleNotification::setNotificationWhenCommitted(int order,
                                                      const std::string &name,
                                                      const CommittedNotification &onCommitted)
{
    WCTInnerAssert(onCommitted);
    bool stateBefore = isCommittedNotificationSet();
    m_commitedNotifications.insert(order, name, onCommitted);
    bool stateAfter = isCommittedNotificationSet();
    if (stateBefore != stateAfter) {
        setupCommittedNotification();
    }
}

void HandleNotification::unsetNotificationWhenCommitted(const std::string &name)
{
    bool stateBefore = isCommittedNotificationSet();
    m_commitedNotifications.erase(name);
    bool stateAfter = isCommittedNotificationSet();
    if (stateBefore != stateAfter) {
        setupCommittedNotification();
    }
}

void HandleNotification::setupCommittedNotification()
{
    if (!m_commitedNotifications.elements().empty()) {
        sqlite3_wal_hook((sqlite3 *) getRawHandle(),
                         [](void *p, sqlite3 *, const char *, int frames) -> int {
                             HandleNotification *notification
                             = reinterpret_cast<HandleNotification *>(p);
                             notification->dispatchCommittedNotification(frames);
                             return SQLITE_OK;
                         },
                         this);
    } else {
        sqlite3_wal_hook((sqlite3 *) getRawHandle(), nullptr, nullptr);
    }
}

bool HandleNotification::isCommittedNotificationSet() const
{
    return !m_commitedNotifications.elements().empty();
}

void HandleNotification::dispatchCommittedNotification(int frames)
{
    WCTInnerAssert(!m_commitedNotifications.elements().empty());
    for (const auto &element : m_commitedNotifications.elements()) {
        if (!element.value(m_handle, frames)) {
            break;
        }
    }
}

#pragma mark - Checkpoint
bool HandleNotification::isCheckpointNotificationSet() const
{
    return !m_checkpointNotifications.elements().empty();
}

bool HandleNotification::setupCheckpointNotification(bool ignorable)
{
    int rc = SQLITE_OK;
    if (!m_checkpointNotifications.elements().empty()) {
        rc = sqlite3_wal_checkpoint_handler(
        (sqlite3 *) getRawHandle(),
        [](void *p, int frames) -> int {
            HandleNotification *notification
            = reinterpret_cast<HandleNotification *>(p);
            if (notification->dispatchCheckpointNotification(frames)) {
                return SQLITE_OK;
            }
            return SQLITE_ABORT;
        },
        this);
    } else {
        rc = sqlite3_wal_checkpoint_handler((sqlite3 *) getRawHandle(), nullptr, nullptr);
    }
    if (rc == SQLITE_OK || ignorable) {
        return true;
    }
    setError(rc);
    return false;
}

bool HandleNotification::setNotificationWhenCheckpoint(int order,
                                                       const std::string &name,
                                                       const CheckpointNotification &willCheckpoint,
                                                       bool ignorable)
{
    WCTInnerAssert(willCheckpoint != nullptr);
    const auto *element = m_checkpointNotifications.find(name);
    int oldOrder = 0;
    CheckpointNotification oldNotification = nullptr;
    if (element != nullptr) {
        oldOrder = element->order;
        oldNotification = element->value;
    }
    bool stateBefore = isCheckpointNotificationSet();
    m_checkpointNotifications.insert(order, name, willCheckpoint);
    bool stateAfter = isCheckpointNotificationSet();
    if (stateBefore != stateAfter) {
        if (!setupCheckpointNotification(ignorable)) {
            //recover
            if (oldNotification) {
                m_checkpointNotifications.insert(oldOrder, name, oldNotification);
            }
            return false;
        }
    }
    return true;
}

bool HandleNotification::dispatchCheckpointNotification(int frames)
{
    WCTInnerAssert(!m_checkpointNotifications.elements().empty());
    for (const auto &element : m_checkpointNotifications.elements()) {
        if (!element.value(m_handle, frames)) {
            return false;
        }
    }
    return true;
}

} //namespace WCDB
