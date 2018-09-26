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
#include <WCDB/BackupQueue.hpp>
#include <WCDB/DatabasePool.hpp>

namespace WCDB {

BackupQueue::BackupQueue(const std::string& name, DatabasePool* databasePool)
: AsyncQueue(name), m_databasePool(databasePool)
{
    WCTInnerAssert(m_databasePool);
}

BackupQueue::~BackupQueue()
{
    m_timedQueue.stop();
    m_timedQueue.waitUntilDone();
}

void BackupQueue::loop()
{
    m_timedQueue.loop(std::bind(
    &BackupQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

void BackupQueue::put(const std::string& path, double delay, int frames)
{
    m_timedQueue.reQueue(path, delay, frames);
}

int BackupQueue::getBackedUpFrames(const std::string& path)
{
    SharedLockGuard lockGuard(m_lock);
    return m_backedUp[path];
}

bool BackupQueue::onTimed(const std::string& path, const int& frames)
{
    if (exit()) {
        m_timedQueue.stop();
        return true;
    }

    auto database = m_databasePool->get(path);
    if (database == nullptr) {
        return true;
    }
    bool result = database->backup();
    if (!result) {
        // retry after 15.0s if failed
        m_timedQueue.reQueue(path, 15.0, frames);
    }
    LockGuard lockGuard(m_lock);
    m_backedUp[path] = frames;
    return result;
}

} // namespace WCDB
