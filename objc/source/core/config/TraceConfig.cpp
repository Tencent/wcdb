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

#include <WCDB/TraceConfig.hpp>

namespace WCDB {

TraceConfig::TraceConfig()
    : Config(), m_performanceTrace(nullptr), m_sqlTrace(nullptr)
{
}

void TraceConfig::setPerformanceTrace(
    const Handle::PerformanceTraceCallback &trace)
{
    LockGuard lockGuard(m_lock);
    m_performanceTrace = trace;
}

void TraceConfig::setSQLTrace(const Handle::SQLTraceCallback &trace)
{
    LockGuard lockGuard(m_lock);
    m_sqlTrace = trace;
}

bool TraceConfig::invoke(Handle *handle) const
{
    SharedLockGuard lockGuard(m_lock);
    handle->tracePerformance(m_performanceTrace);
    handle->traceSQL(m_sqlTrace);
    return true;
}

} //namespace WCDB
