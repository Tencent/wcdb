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

#include <WCDB/CoreConst.h>
#include <WCDB/Error.hpp>
#include <WCDB/MigrationQueue.hpp>
#include <WCDB/Notifier.hpp>

namespace WCDB {

MigrationEvent::~MigrationEvent()
{
}

MigrationQueue::MigrationQueue(const String& name, MigrationEvent* event)
: AsyncQueue(name, event), m_dirty(false)
{
}

void MigrationQueue::put(const String& path)
{
    if (!exit()) {
        bool notify = false;
        {
            std::lock_guard<std::mutex> lockGuard(m_mutex);
            notify = m_migratings.empty();
            m_migratings.emplace(path);
        }
        if (notify) {
            m_cond.notify_all();
        }
        lazyRun();
    }
}

void MigrationQueue::remove(const String& path)
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    if (m_migratings.erase(path) > 0) {
        m_dirty = true;
    }
}

void MigrationQueue::loop()
{
    while (!exit()) {
        String path;
        {
            std::unique_lock<std::mutex> lockGuard(m_mutex);
            if (m_migratings.empty()) {
                m_cond.wait(lockGuard);
                continue;
            }
            path = *m_migratings.begin();
        }
        int failedCount = 0;
        while (!exit()) {
            std::this_thread::sleep_for(std::chrono::microseconds(
            (long long) (MigrationQueueTimeIntervalForMigrating * 1000000)));
            bool succeed, done;
            std::tie(succeed, done)
            = static_cast<MigrationEvent*>(m_event)->databaseShouldMigrate(path);
            if ((!succeed && ++failedCount >= MigrationQueueTolerableFailures) || done) {
                Error error;
                error.level = Error::Level::Notice;
                error.setCode(Error::Code::Notice);
                error.message = "Async migration stopped due to the error.";
                error.infos.set("Path", path);
                Notifier::shared()->notify(error);
                break;
            }
            if (m_dirty) {
                std::lock_guard<std::mutex> lockGuard(m_mutex);
                bool stop = m_migratings.find(path) == m_migratings.end();
                m_dirty = false;
                if (stop) {
                    break;
                }
            }
        }
        std::unique_lock<std::mutex> lockGuard(m_mutex);
        m_migratings.erase(path);
    }
}

} // namespace WCDB
