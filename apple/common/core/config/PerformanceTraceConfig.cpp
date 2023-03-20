//
// Created by sanhuazhang on 2019/05/02
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

#include "PerformanceTraceConfig.hpp"
#include "Assertion.hpp"
#include "StringView.hpp"

namespace WCDB {

#pragma mark - PerformanceTracer
PerformanceTracer::PerformanceTracer()
: m_identifier(StringView::formatted("PerformanceTrace-%p", this))
, m_notification(nullptr)
{
}

PerformanceTracer::~PerformanceTracer() = default;

void PerformanceTracer::setNotification(const Notification &notification)
{
    m_notification = notification;
}

bool PerformanceTracer::invoke(InnerHandle *handle)
{
    handle->setNotificationWhenPerformanceTraced(m_identifier, m_notification);
    return true;
}

bool PerformanceTracer::uninvoke(InnerHandle *handle)
{
    handle->setNotificationWhenPerformanceTraced(m_identifier, nullptr);
    return true;
}

#pragma mark - PerformanceTraceConfig
PerformanceTraceConfig::PerformanceTraceConfig(const Notification &notification)
{
    setNotification(notification);
}

PerformanceTraceConfig::~PerformanceTraceConfig() = default;

bool PerformanceTraceConfig::invoke(InnerHandle *handle)
{
    return PerformanceTracer::invoke(handle);
}

bool PerformanceTraceConfig::uninvoke(InnerHandle *handle)
{
    return PerformanceTracer::uninvoke(handle);
}

#pragma mark - ShareablePerformanceTraceConfig
ShareablePerformanceTraceConfig::ShareablePerformanceTraceConfig() = default;

ShareablePerformanceTraceConfig::~ShareablePerformanceTraceConfig() = default;

void ShareablePerformanceTraceConfig::setNotification(const Notification &notification)
{
    LockGuard lockGuard(m_lock);
    WCTAssert(m_notification == nullptr || notification == nullptr);
    PerformanceTracer::setNotification(notification);
}

bool ShareablePerformanceTraceConfig::invoke(InnerHandle *handle)
{
    SharedLockGuard lockGuard(m_lock);
    return PerformanceTracer::invoke(handle);
}

bool ShareablePerformanceTraceConfig::uninvoke(InnerHandle *handle)
{
    SharedLockGuard lockGuard(m_lock);
    return PerformanceTracer::uninvoke(handle);
}

} //namespace WCDB
