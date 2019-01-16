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

#include <WCDB/BusyRetryConfig.hpp>
#include <WCDB/CoreConst.h>
#include <WCDB/Handle.hpp>

namespace WCDB {

BusyRetryConfig::BusyRetryConfig()
: Config()
, m_identifier(String::formatted("Busy-%p", this))
, m_numberOfWaitingHandles(0)
, m_numberOfSteppingHandles(0)
{
}

bool BusyRetryConfig::invoke(Handle* handle)
{
    handle->setNotificationWhenBusy(std::bind(
    &BusyRetryConfig::onBusy, this, std::placeholders::_1, std::placeholders::_2));
    handle->setNotificationWhenStatementWillStep(
    m_identifier, std::bind(&BusyRetryConfig::willStep, this, std::placeholders::_1));
    handle->setNotificationWhenStatementDidStep(
    m_identifier,
    std::bind(&BusyRetryConfig::didStep, this, std::placeholders::_1, std::placeholders::_2));
    return true;
}

bool BusyRetryConfig::uninvoke(Handle* handle)
{
    handle->setNotificationWhenBusy(nullptr);
    handle->setNotificationWhenStatementDidStep(m_identifier, nullptr);
    handle->setNotificationWhenStatementWillStep(m_identifier, nullptr);
    return true;
}

void BusyRetryConfig::willStep(HandleStatement* handleStatement)
{
    ++m_numberOfSteppingHandles;
}

void BusyRetryConfig::didStep(HandleStatement* handleStatement, bool result)
{
    --m_numberOfSteppingHandles;

    AbstractHandle* handle = handleStatement->getHandle();
    if (!handle->isInTransaction()) {
        std::lock_guard<decltype(m_mutex)> lockGuard(m_mutex);
        if (m_numberOfWaitingHandles > 0) {
            m_cond.notify_all();
        }
    }
}

bool BusyRetryConfig::onBusy(const String& path, int numberOfTimes)
{
    bool retry = false;
    if (numberOfTimes <= BusyRetryMaxAllowedNumberOfTimes) {
        retry = true;
        double timeOut;
        if (pthread_main_np() != 0) {
            timeOut = BusyRetryTimeOutForMainThread;
        } else {
            timeOut = BusyRetryTimeOutForOtherThread;
        }
        std::unique_lock<decltype(m_mutex)> lockGuard(m_mutex);
        ++m_numberOfWaitingHandles;
        if (m_numberOfSteppingHandles > 0) {
            m_cond.wait_for(
            lockGuard, std::chrono::microseconds((long long) (timeOut * 1000000)));
        }
        --m_numberOfWaitingHandles;
    }
    return retry;
}

} // namespace WCDB
