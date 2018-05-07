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

std::shared_ptr<Config> TraceConfig::config()
{
    static std::shared_ptr<Config> s_config(new TraceConfig());
    return s_config;
}

void TraceConfig::setPerformanceTrace(
    const Handle::PerformanceTraceCallback &trace)
{
    std::shared_ptr<Handle::PerformanceTraceCallback> newTrace(
        new Handle::PerformanceTraceCallback);
    if (newTrace) {
        m_performanceTrace = newTrace;
    }
}

void TraceConfig::setSQLTrace(const Handle::SQLTraceCallback &trace)
{
    std::shared_ptr<Handle::SQLTraceCallback> newTrace(
        new Handle::SQLTraceCallback);
    if (newTrace) {
        m_sqlTrace = newTrace;
    }
}

bool TraceConfig::invoke(Handle *handle) const
{
    {
        std::shared_ptr<Handle::PerformanceTraceCallback> trace =
            m_performanceTrace;
        if (trace) {
            handle->tracePerformance(*trace.get());
        }
    }
    {
        std::shared_ptr<Handle::SQLTraceCallback> trace = m_sqlTrace;
        if (trace) {
            handle->traceSQL(*trace.get());
        }
    }
    return true;
}

TraceConfig::TraceConfig()
    : Config("trace", TraceConfig::order)
    , m_performanceTrace(nullptr)
    , m_sqlTrace(nullptr)
{
}

} //namespace WCDB
