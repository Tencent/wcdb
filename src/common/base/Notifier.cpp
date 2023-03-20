//
// Created by sanhuazhang on 2018/06/07
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

#include "Notifier.hpp"
#include "Assertion.hpp"

namespace WCDB {

Notifier &Notifier::shared()
{
    static Notifier *s_notifier = new Notifier;
    return *s_notifier;
}

Notifier::Notifier() = default;

void Notifier::setNotification(int order, const UnsafeStringView &key, const Callback &callback)
{
    WCTAssert(callback != nullptr);
    LockGuard lockGuard(m_lock);
    WCTAssert(m_notifications.find(StringView(key)) == m_notifications.end());
    m_notifications.insert(StringView(key), callback, order);
}

void Notifier::unsetNotification(const UnsafeStringView &key)
{
    LockGuard lockGuard(m_lock);
    m_notifications.erase(StringView(key));
}

void Notifier::setNotificationForPreprocessing(const UnsafeStringView &key,
                                               const PreprocessCallback &callback)
{
    LockGuard lockGuard(m_lock);
    if (callback != nullptr) {
        m_preprocessNotifications[key] = callback;
    } else {
        m_preprocessNotifications.erase(key);
    }
}

void Notifier::notify(Error &error) const
{
    SharedLockGuard lockGuard(m_lock);
    for (const auto &element : m_preprocessNotifications) {
        element.second(error);
    }
    for (const auto &element : m_notifications) {
        element.value()(error);
    }
}

} //namespace WCDB
