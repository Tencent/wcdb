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

#include "SQLTraceConfig.hpp"
#include "Assertion.hpp"
#include "StringView.hpp"

namespace WCDB {

SQLTracer::SQLTracer()
: m_identifier(StringView::formatted("SQLTrace-%p", this))
{
}

SQLTracer::~SQLTracer() = default;

bool SQLTracer::invoke(InnerHandle *handle)
{
    handle->setNotificationWhenSQLTraced(m_identifier, m_notification);
    return true;
}

bool SQLTracer::uninvoke(InnerHandle *handle)
{
    handle->setNotificationWhenSQLTraced(m_identifier, nullptr);
    return true;
}

void SQLTracer::setNotification(const Notification &notification)
{
    m_notification = notification;
}

SQLTraceConfig::SQLTraceConfig(const Notification &notification)
{
    setNotification(notification);
}

SQLTraceConfig::~SQLTraceConfig() = default;

bool SQLTraceConfig::invoke(InnerHandle *handle)
{
    return SQLTracer::invoke(handle);
}

bool SQLTraceConfig::uninvoke(InnerHandle *handle)
{
    return SQLTracer::uninvoke(handle);
}

ShareableSQLTraceConfig::ShareableSQLTraceConfig() = default;

ShareableSQLTraceConfig::~ShareableSQLTraceConfig() = default;

void ShareableSQLTraceConfig::setNotification(const Notification &notification)
{
    LockGuard lockGuard(m_lock);
    WCTAssert(m_notification == nullptr || notification == nullptr);
    SQLTracer::setNotification(notification);
}

bool ShareableSQLTraceConfig::invoke(InnerHandle *handle)
{
    SharedLockGuard lockGuard(m_lock);
    return SQLTracer::invoke(handle);
}

bool ShareableSQLTraceConfig::uninvoke(InnerHandle *handle)
{
    SharedLockGuard lockGuard(m_lock);
    return SQLTracer::uninvoke(handle);
}

} //namespace WCDB
