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

#include <WCDB/Assertion.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/HandleNotification.hpp>
#include <WCDB/SQLite.h>
#include <WCDB/String.hpp>

namespace WCDB {

void HandleNotification::purge()
{
    bool isOpened = m_handle->isOpened();
    bool set = isSQLTraceNotificationSet() || isPerformanceTraceNotificationSet();
    m_sqlNotifications.clear();
    m_performanceNotifications.clear();
    if (set && isOpened) {
        setupTraceNotification();
    }

    set = isCommittedNotificationSet();
    m_commitedNotifications.clear();
    if (set && isOpened) {
        setupCommittedNotification();
    }

    set = isCheckpointNotificationSet();
    m_willCheckpointNotifications.clear();
    m_checkpointedNotifications.clear();
    if (set && isOpened) {
        setupCheckpointNotification();
    }
}

#pragma mark - Trace
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
        sql ? sql : String::null(), *cost, !sqlite3_get_autocommit(db));
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
bool HandleNotification::isSQLTraceNotificationSet() const
{
    return !m_sqlNotifications.empty();
}

void HandleNotification::setNotificationWhenSQLTraced(const String &name,
                                                      const SQLNotification &onTraced)
{
    bool stateBefore = isSQLTraceNotificationSet();
    if (onTraced) {
        m_sqlNotifications[name] = onTraced;
    } else {
        m_sqlNotifications.erase(name);
    }
    bool stateAfter = isSQLTraceNotificationSet();
    if (stateBefore != stateAfter) {
        setupTraceNotification();
    }
}

void HandleNotification::dispatchSQLTraceNotification(const String &sql)
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

bool HandleNotification::isPerformanceTraceNotificationSet() const
{
    return !m_performanceNotifications.empty();
}

void HandleNotification::setNotificationWhenPerformanceTraced(const String &name,
                                                              const PerformanceNotification &onTraced)
{
    bool stateBefore = isPerformanceTraceNotificationSet();
    if (onTraced) {
        m_performanceNotifications[name] = onTraced;
    } else {
        m_performanceNotifications.erase(name);
    }
    bool stateAfter = isPerformanceTraceNotificationSet();
    if (stateAfter != stateBefore) {
        setupTraceNotification();
    }
}

void HandleNotification::dispatchPerformanceTraceNotification(const String &sql,
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
                                                      const String &name,
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

void HandleNotification::unsetNotificationWhenCommitted(const String &name)
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
    return !m_willCheckpointNotifications.elements().empty()
           || !m_checkpointedNotifications.empty();
}

bool HandleNotification::setupCheckpointNotification(bool ignorable)
{
    int rc = SQLITE_OK;
    if (!m_willCheckpointNotifications.elements().empty()
        || !m_checkpointedNotifications.empty()) {
        rc = sqlite3_wal_checkpoint_handler(
        (sqlite3 *) getRawHandle(),
        [](void *p, int rc) -> int {
            HandleNotification *notification
            = reinterpret_cast<HandleNotification *>(p);
            if (notification->dispatchCheckpointNotification(rc)) {
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

bool HandleNotification::setNotificationWhenWillCheckpoint(
int order, const String &name, const WillCheckpointNotification &willCheckpoint, bool ignorable)
{
    WCTInnerAssert(willCheckpoint != nullptr);
    const auto *element = m_willCheckpointNotifications.find(name);
    int oldOrder = 0;
    WillCheckpointNotification oldNotification = nullptr;
    if (element != nullptr) {
        oldOrder = element->order;
        oldNotification = element->value;
    }
    bool stateBefore = isCheckpointNotificationSet();
    m_willCheckpointNotifications.insert(order, name, willCheckpoint);
    bool stateAfter = isCheckpointNotificationSet();
    if (stateBefore != stateAfter) {
        if (!setupCheckpointNotification(ignorable)) {
            //recover
            if (oldNotification) {
                m_willCheckpointNotifications.insert(oldOrder, name, oldNotification);
            }
            return false;
        }
    }
    return true;
}

bool HandleNotification::unsetNotificationWhenWillCheckpoint(const String &name, bool ignorable)
{
    const auto *element = m_willCheckpointNotifications.find(name);
    int oldOrder = 0;
    WillCheckpointNotification oldNotification = nullptr;
    if (element != nullptr) {
        oldOrder = element->order;
        oldNotification = element->value;
    }
    bool stateBefore = isCheckpointNotificationSet();
    m_willCheckpointNotifications.erase(name);
    bool stateAfter = isCheckpointNotificationSet();
    if (stateBefore != stateAfter) {
        if (!setupCheckpointNotification(ignorable)) {
            //recover
            if (oldNotification) {
                m_willCheckpointNotifications.insert(oldOrder, name, oldNotification);
            }
            return false;
        }
    }
    return true;
}

bool HandleNotification::setNotificationWhenCheckpointed(
const String &name, const CheckpointedNotification &checkpointed, bool ignorable)
{
    CheckpointedNotification oldNotification = nullptr;
    auto iter = m_checkpointedNotifications.find(name);
    if (iter != m_checkpointedNotifications.end()) {
        oldNotification = iter->second;
    }
    bool stateBefore = isCheckpointNotificationSet();
    if (checkpointed) {
        m_checkpointedNotifications.emplace(name, checkpointed);
    } else {
        m_checkpointedNotifications.erase(name);
    }
    bool stateAfter = isCheckpointNotificationSet();
    if (stateBefore != stateAfter) {
        if (!setupCheckpointNotification(ignorable)) {
            //recover
            if (oldNotification) {
                m_checkpointedNotifications.emplace(name, oldNotification);
            }
            return false;
        }
    }
    return true;
}

bool HandleNotification::dispatchCheckpointNotification(int rc)
{
    WCTInnerAssert(isCheckpointNotificationSet());
    if (rc < 0) {
        // will checkpoint
        for (const auto &element : m_willCheckpointNotifications.elements()) {
            if (!element.value(m_handle)) {
                return false;
            }
        }
    } else {
        // checkpointed
        for (const auto &element : m_checkpointedNotifications) {
            element.second(m_handle, rc);
        }
    }
    return true;
}

} //namespace WCDB
