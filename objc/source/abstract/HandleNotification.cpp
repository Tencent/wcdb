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

#pragma mark - Trace
int HandleNotification::onTraced(unsigned int flag, void *P, void *X)
{
    sqlite3_stmt *stmt = (sqlite3_stmt *) P;
    const char *sql = sqlite3_sql(stmt);
    switch (flag) {
        case SQLITE_TRACE_STMT: {
            if (sql) {
                onSQLTraced(sql);
            }
        } break;
        case SQLITE_TRACE_PROFILE: {
            sqlite3_int64 *cost = (sqlite3_int64 *) X;
            sqlite3 *db = sqlite3_db_handle(stmt);
            onPerformanceTraced(sql ? sql : String::empty(), *cost,
                                !sqlite3_get_autocommit(db));
        } break;
        default:
            break;
    }
    return SQLITE_OK;
}

void HandleNotification::setupTrace()
{
    unsigned flag = 0;
    if (!m_sqlNotifications.empty()) {
        flag |= SQLITE_TRACE_STMT;
    }
    if (!m_performanceNotifications.empty()) {
        flag |= SQLITE_TRACE_PROFILE;
    }
    if (flag != 0) {
        sqlite3_trace_v2((sqlite3 *) getRawHandle(), flag,
                         [](unsigned int T, void *C, void *P, void *X) {
                             HandleNotification *notification =
                                 reinterpret_cast<HandleNotification *>(C);
                             return notification->onTraced(T, P, X);
                         },
                         this);
    } else {
        sqlite3_trace_v2((sqlite3 *) getRawHandle(), 0, nullptr, nullptr);
    }
}

#pragma mark - SQL
void HandleNotification::setNotificationWhenSQLTraced(
    const std::string &name, const SQLNotification &onTraced)
{
    if (onTraced) {
        m_sqlNotifications[name] = onTraced;
    } else {
        m_sqlNotifications.erase(name);
    }
    setupTrace();
}

void HandleNotification::onSQLTraced(const std::string &sql)
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

void HandleNotification::setNotificationWhenPerformanceTraced(
    const std::string &name, const PerformanceNotification &onTraced)
{
    if (onTraced) {
        m_performanceNotifications[name] = onTraced;
    } else {
        m_performanceNotifications.erase(name);
    }
    setupTrace();
}

void HandleNotification::onPerformanceTraced(const std::string &sql,
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
void HandleNotification::setNotificationWhenCommitted(
    const CommittedNotification &onCommitted)
{
    m_commitedNotification = onCommitted;
    if (m_commitedNotification) {
        sqlite3_wal_hook(
            (sqlite3 *) getRawHandle(),
            [](void *p, sqlite3 *, const char *, int frames) -> int {
                HandleNotification *notification =
                    reinterpret_cast<HandleNotification *>(p);
                notification->onCommitted(frames);
                return SQLITE_OK;
            },
            this);
    } else {
        sqlite3_wal_hook((sqlite3 *) getRawHandle(), nullptr, nullptr);
    }
}

void HandleNotification::onCommitted(int frames)
{
    WCTInnerAssert(m_commitedNotification != nullptr);
    m_commitedNotification(m_handle, frames);
}

#pragma mark - Checkpoint
void HandleNotification::setNotificationWhenCheckpoint(
    const CheckpointNotification &willCheckpoint)
{
    m_checkpointNotification = willCheckpoint;
    if (m_checkpointNotification) {
        sqlite3_wal_checkpoint_handler(
            (sqlite3 *) getRawHandle(),
            [](void *p, int frames) -> int {
                HandleNotification *notification =
                    reinterpret_cast<HandleNotification *>(p);
                if (notification->willCheckpoint(frames)) {
                    return SQLITE_OK;
                }
                return SQLITE_ABORT;
            },
            this);
    } else {
        sqlite3_wal_checkpoint_handler((sqlite3 *) getRawHandle(), nullptr,
                                       nullptr);
    }
}

bool HandleNotification::willCheckpoint(int frames)
{
    WCTInnerAssert(m_checkpointNotification != nullptr);
    return m_checkpointNotification(m_handle, frames);
}

} //namespace WCDB
