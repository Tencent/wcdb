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

#include <WCDB/PerformanceTraceConfig.hpp>

namespace WCDB {

PerformanceTraceConfig::PerformanceTraceConfig(const std::string &name,
                                               const Handle::PerformanceNotification &trace)
: Config(name), m_performanceTrace(trace)
{
}

bool PerformanceTraceConfig::invoke(Handle *handle)
{
    handle->setNotificationWhenPerformanceTraced(name, m_performanceTrace);
    return true;
}

const std::shared_ptr<Config> &SharedPerformanceTraceConfig::shared()
{
    static const std::shared_ptr<Config> *s_shared = new std::shared_ptr<Config>(
    new SharedPerformanceTraceConfig("WCDBGlobalPerformanceTrace", nullptr));
    return *s_shared;
}

void SharedPerformanceTraceConfig::setPerformanceTrace(const Handle::PerformanceNotification &trace)
{
    LockGuard lockGuard(m_lock);
    m_performanceTrace = trace;
}

bool SharedPerformanceTraceConfig::invoke(Handle *handle)
{
    SharedLockGuard lockGuard(m_lock);
    return PerformanceTraceConfig::invoke(handle);
}

} //namespace WCDB
