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
#include <WCDB/Notifier.hpp>

namespace WCDB {

Notifier *Notifier::shared()
{
    static Notifier *s_shared = new Notifier;
    return s_shared;
}

Notifier::Notifier()
{
}

void Notifier::setNotification(int order, const String &key, const Callback &callback)
{
    WCTInnerAssert(callback != nullptr);
    LockGuard lockGuard(m_lock);
    m_notifications.insert(order, key, callback);
}

void Notifier::unsetNotification(const String &key)
{
    LockGuard lockGuard(m_lock);
    m_notifications.erase(key);
}

void Notifier::setNotificationForPreprocessing(const String &key,
                                               const PreprocessCallback &callback)
{
    LockGuard lockGuard(m_lock);
    if (callback != nullptr) {
        m_preprocessNotifications[key] = callback;
    } else {
        m_preprocessNotifications.erase(key);
    }
}

void Notifier::notify(const Error &error) const
{
    Error::Infos infosToBeUpdated;
    SharedLockGuard lockGuard(m_lock);
    Error::Level newLevel = error.level;
    for (const auto &element : m_preprocessNotifications) {
        element.second(error, infosToBeUpdated, newLevel);
    }
    if (infosToBeUpdated.empty() && newLevel == error.level) {
        doNotify(error);
    } else {
        Error preprocessedError = error;
        preprocessedError.level = newLevel;
        for (const auto &info : infosToBeUpdated.getDoubles()) {
            preprocessedError.infos.set(info.first, info.second);
        }
        for (const auto &info : infosToBeUpdated.getStrings()) {
            preprocessedError.infos.set(info.first, info.second);
        }
        for (const auto &info : infosToBeUpdated.getIntegers()) {
            preprocessedError.infos.set(info.first, info.second);
        }
        doNotify(preprocessedError);
    }
}

void Notifier::doNotify(const Error &error) const
{
    for (const auto &element : m_notifications.elements()) {
        element.value(error);
    }
}

} //namespace WCDB
