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

#include <WCDB/Core.h>
#include <WCDB/String.hpp>
#include <iostream>

namespace WCDB {

CoreNotifier::CoreNotifier()
: DatabasePoolRelated(), m_callback(CoreNotifier::logger), m_corruptionQueue(nullptr)
{
    Notifier::shared()->setNotification(
    std::bind(&CoreNotifier::notify, this, std::placeholders::_1));
}

void CoreNotifier::setNotification(const Callback& callback)
{
    LockGuard lockGuard(m_lock);
    m_callback = callback;
}

void CoreNotifier::setCorruptionQueue(CorruptionQueue* queue)
{
    LockGuard lockGuard(m_lock);
    m_corruptionQueue = queue;
}

void CoreNotifier::setRelatedDatabasePool(DatabasePool* databasePool)
{
    LockGuard lockGuard(m_lock);
    DatabasePoolRelated::setRelatedDatabasePool(databasePool);
}

void CoreNotifier::notify(const Error& error)
{
    Error processed = error;

    const auto& stringInfos = error.infos.getStrings();

    SharedLockGuard lockGuard(m_lock);
    if (m_databasePool) {
        auto iter = stringInfos.find("Path");
        if (iter != stringInfos.end()) {
            auto pool = m_databasePool->getExistingPool(iter->second);
            if (pool != nullptr) {
                if (pool->getTag() != Tag::invalid()) {
                    processed.infos.set("Tag", pool->getTag());
                }
                if (error.isCorruption()) {
                    if (m_corruptionQueue) {
                        m_corruptionQueue->put(iter->second);
                    }
                }
            }
        }
    }

    if (m_callback) {
        m_callback(processed);
    }
}

void CoreNotifier::globalLogger(void* unused, int code, const char* message)
{
    Error error;
    switch (code) {
    case SQLITE_WARNING:
        error.level = Error::Level::Warning;
        break;
    case SQLITE_NOTICE:
        error.level = Error::Level::Ignore;
        break;
    default:
        error.level = Error::Level::Debug;
        break;
    }
    error.setSQLiteCode(code);
    error.message = message ? message : String::empty();
    Notifier::shared()->notify(error);
}

void CoreNotifier::logger(const Error& error)
{
    bool log = error.level != Error::Level::Ignore
#ifndef DEBUG
               && error.level != Error::Level::Debug
#endif
    ;

    if (log) {
        std::ostringstream stream;
        stream << "[" << Error::levelName(error.level) << ": ";
        stream << (int) error.code();
        if (!error.message.empty()) {
            stream << ", " << error.message;
        }
        stream << "]";

        for (const auto& info : error.infos.getIntegers()) {
            stream << ", " << info.first << ": " << info.second;
        }
        for (const auto& info : error.infos.getStrings()) {
            if (!info.second.empty()) {
                stream << ", " << info.first << ": " << info.second;
            }
        }
        for (const auto& info : error.infos.getDoubles()) {
            stream << ", " << info.first << ": " << info.second;
        }
        stream << std::endl;
        std::cout << stream.str();
#ifdef DEBUG
        if (error.level >= Error::Level::Error) {
            std::cout << "Set breakpoint in CoreNotifier::logger to debug.\n";
        }
#endif
    }

    if (error.level == Error::Level::Fatal) {
        abort();
    }
}

} // namespace WCDB
