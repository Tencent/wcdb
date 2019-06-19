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
#include <WCDB/StringView.hpp>

namespace WCDB {

#pragma mark - PerformanceTracer
PerformanceTracer::PerformanceTracer()
: m_identifier(StringView::formatted("PerformanceTrace-%p", this))
, m_notification(nullptr)
{
}

PerformanceTracer::~PerformanceTracer()
{
}

void PerformanceTracer::setNotification(const Notification &notification)
{
    m_notification = notification;
}

bool PerformanceTracer::invoke(Handle *handle)
{
    handle->setNotificationWhenPerformanceTraced(m_identifier, m_notification);
    return true;
}

bool PerformanceTracer::uninvoke(Handle *handle)
{
    handle->setNotificationWhenPerformanceTraced(m_identifier, nullptr);
    return true;
}

#pragma mark - PerformanceTraceConfig
PerformanceTraceConfig::PerformanceTraceConfig(const Notification &notification)
{
    setNotification(notification);
}

bool PerformanceTraceConfig::invoke(Handle *handle)
{
    return PerformanceTracer::invoke(handle);
}

bool PerformanceTraceConfig::uninvoke(Handle *handle)
{
    return PerformanceTracer::uninvoke(handle);
}

#pragma mark - ShareablePerformanceTraceConfig
ShareablePerformanceTraceConfig::ShareablePerformanceTraceConfig()
{
}

void ShareablePerformanceTraceConfig::setNotification(const Notification &notification)
{
    LockGuard lockGuard(m_lock);
    PerformanceTracer::setNotification(notification);
}

bool ShareablePerformanceTraceConfig::invoke(Handle *handle)
{
    SharedLockGuard lockGuard(m_lock);
    return PerformanceTracer::invoke(handle);
}

bool ShareablePerformanceTraceConfig::uninvoke(Handle *handle)
{
    SharedLockGuard lockGuard(m_lock);
    return PerformanceTracer::uninvoke(handle);
}

} //namespace WCDB
