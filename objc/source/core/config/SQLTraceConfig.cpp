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
#include <WCDB/SQLTraceConfig.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

SQLTraceConfig::SQLTraceConfig(const Notification &notification)
: Config(), m_identifier(String::formatted("SQLTrace-%p", this)), m_notification(notification)
{
}

bool SQLTraceConfig::invoke(Handle *handle)
{
    handle->setNotificationWhenSQLTraced(m_identifier, m_notification);
    return true;
}

bool SQLTraceConfig::uninvoke(Handle *handle)
{
    handle->setNotificationWhenSQLTraced(m_identifier, nullptr);
    return true;
}

ShareableSQLTraceConfig::ShareableSQLTraceConfig() : SQLTraceConfig(nullptr)
{
}

void ShareableSQLTraceConfig::setNotification(const Notification &notification)
{
    LockGuard lockGuard(m_lock);
    m_notification = notification;
}

bool ShareableSQLTraceConfig::invoke(Handle *handle)
{
    SharedLockGuard lockGuard(m_lock);
    return SQLTraceConfig::invoke(handle);
}

} //namespace WCDB
