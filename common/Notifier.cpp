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

void Notifier::setNotification(const std::string &key, const Callback &callback)
{
    LockGuard lockGuard(m_lock);
    if (callback != nullptr) {
        m_notifications[key] = callback;
    } else {
        m_notifications.erase(key);
    }
}

void Notifier::setNotificationForPreprocessing(const std::string &key,
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
    Error preprocessed = error;
    SharedLockGuard lockGuard(m_lock);
    for (const auto &element : m_preprocessNotifications) {
        element.second(preprocessed);
    }
    for (const auto &element : m_notifications) {
        element.second(preprocessed);
    }
}

void Notifier::fatal(const std::string &message, const char *file, int line)
{
    error(Error::Level::Fatal, message, file, line);
}

void Notifier::error(const std::string &message, const char *file, int line)
{
    error(Error::Level::Error, message, file, line);
}

void Notifier::warning(const std::string &message, const char *file, int line)
{
    error(Error::Level::Warning, message, file, line);
}

void Notifier::error(Error::Level level, const std::string &message, const char *file, int line)
{
    Error error;
    error.setCode(Error::Code::Misuse, "Assertion");
    error.level = level;
    error.message = message;
    if (file) {
        error.infos.set("File", file);
    }
    error.infos.set("Line", line);
    Notifier::shared()->notify(error);
}

} //namespace WCDB
