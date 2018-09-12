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

namespace WCDB {

CheckpointQueue::CheckpointQueue(const std::string& name, DatabasePool* databasePool)
: AsyncQueue(name)
, m_databasePool(databasePool)
, m_checkpointPassive(StatementPragma().pragma(Pragma::walCheckpoint()).to("PASSIVE"))
, m_checkpointTruncate(StatementPragma().pragma(Pragma::walCheckpoint()).to("TRUNCATE"))
{
    WCTInnerAssert(m_databasePool != nullptr);
}

CheckpointQueue::~CheckpointQueue()
{
    m_timedQueue.stop();
    m_timedQueue.waitUntilDone();
}

void CheckpointQueue::loop()
{
    m_timedQueue.loop(std::bind(
    &CheckpointQueue::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

bool CheckpointQueue::onTimed(const std::string& path, const int& frames)
{
    if (exit()) {
        m_timedQueue.stop();
        return true;
    }

    auto database = m_databasePool->get(path);
    if (database == nullptr) {
        return true;
    }
    bool result;
    if (frames > framesForFull) {
        result = database->execute(m_checkpointTruncate);
    } else {
        result = database->execute(m_checkpointPassive);
    }
    if (!result) {
        // retry after 10.0s if failed
        m_timedQueue.reQueue(path, 10.0, frames);
    }
    return result;
}

void CheckpointQueue::put(const std::string& path, double delay, int frames)
{
    m_timedQueue.reQueue(path, delay, frames);
}

void CheckpointQueue::remove(const std::string& path)
{
    m_timedQueue.remove(path);
}

} // namespace WCDB
