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

#include <WCDB/Tracer.hpp>
#include <sqlite3.h>

namespace WCDB {

Tracer::Footprint::Footprint(const std::string &sql_) : sql(sql_), frequency(1)
{
}

int Tracer::TraceCallback(unsigned int flag, void *M, void *P, void *X)
{
    Tracer *tracer = (Tracer *) M;
    sqlite3_stmt *stmt = (sqlite3_stmt *) P;
    switch (flag) {
        case SQLITE_TRACE_STMT: {
            const char *sql = sqlite3_sql(stmt);
            if (sql) {
                tracer->reportSQL(sql);
            }
        } break;
        case SQLITE_TRACE_PROFILE: {
            sqlite3_int64 *cost = (sqlite3_int64 *) X;

            sqlite3 *db = sqlite3_db_handle(stmt);

            const char *sql = sqlite3_sql(stmt);
            if (sql) {
                tracer->addPerformanceTrace(sql, *cost);
            }

            //report per transaction
            if (sqlite3_get_autocommit(db)) {
                tracer->reportPerformance();
            }
        } break;
        default:
            break;
    }
    return SQLITE_OK;
}

void Tracer::setupTrace(void *handle)
{
    unsigned flag = 0;
    if (m_sqlTrace) {
        flag |= SQLITE_TRACE_STMT;
    }
    if (m_performanceTrace) {
        flag |= SQLITE_TRACE_PROFILE;
    }
    if (flag != 0) {
        sqlite3_trace_v2((sqlite3 *) handle, flag, Tracer::TraceCallback, this);
    } else {
        sqlite3_trace_v2((sqlite3 *) handle, 0, nullptr, nullptr);
    }
}

void Tracer::traceSQL(void *handle, const SQLTraceCallback &trace)
{
    m_sqlTrace = trace;
    setupTrace(handle);
}

void Tracer::tracePerformance(void *handle,
                              const PerformanceTraceCallback &trace)
{
    m_performanceTrace = trace;
    setupTrace(handle);
}

void Tracer::addPerformanceTrace(const std::string &sql, const int64_t &cost)
{
    if (!m_footprints.empty()) {
        auto &footprint = m_footprints.front();
        if (footprint.sql == sql) {
            ++footprint.frequency;
            return;
        }
    }
    m_footprints.push_back(Footprint(sql));
    m_cost += cost;
}

void Tracer::reportPerformance()
{
    if (!m_footprints.empty()) {
        m_performanceTrace(m_footprints, m_cost);
        m_footprints.clear();
        m_cost = 0;
    }
}

void Tracer::reportSQL(const std::string &sql)
{
    if (m_sqlTrace) {
        m_sqlTrace(sql);
    }
}

} //namespace WCDB
