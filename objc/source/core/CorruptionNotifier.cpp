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
#include <WCDB/CorruptionNotifier.hpp>
#include <WCDB/Dispatch.hpp>
#include <WCDB/HandlePools.hpp>
#include <WCDB/Notifier.hpp>

namespace WCDB {

CorruptionNotifier *CorruptionNotifier::shared()
{
    static CorruptionNotifier *s_corruptionNotifer = new CorruptionNotifier;
    return s_corruptionNotifer;
}

void CorruptionNotifier::boot()
{
    Notifier::shared()->setCorruptionNotification([](const std::string &path) {
        CorruptionNotifier::shared()->addPath(path);
    });
}

void CorruptionNotifier::addPath(const std::string &path)
{
    Dispatch::async("com.Tencent.WCDB.CorruptionNotifier",
                    []() { CorruptionNotifier::shared()->loop(); });
    auto pool = HandlePools::defaultPools()->getExistingPool(path);
    if (pool == nullptr) {
        return;
    }
    if (pool->attachment.corruption.markAsCorrupted()) {
        std::unique_lock<std::mutex> lockGuard(m_mutex);
        bool notify = m_paths.empty();
        m_paths.insert(path);
        if (notify) {
            m_cond.notify_all();
        }
    }
}

void CorruptionNotifier::loop()
{
    std::unique_lock<std::mutex> lockGuard(m_mutex);
    while (true) {
        if (m_paths.empty()) {
            m_cond.wait(lockGuard);
            continue;
        }
        std::string path = std::move(*m_paths.begin());
        m_paths.erase(m_paths.begin());
        lockGuard.unlock();
        auto pool = HandlePools::defaultPools()->getExistingPool(path);
        if (pool == nullptr) {
            continue;
        }
        pool->attachment.corruption.notify();
        lockGuard.lock();
    }
}

} //namespace WCDB
