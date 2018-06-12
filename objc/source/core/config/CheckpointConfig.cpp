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

#include <WCDB/CheckpointConfig.hpp>
#include <WCDB/Database.hpp>
#include <WCDB/Dispatch.hpp>
#include <mutex>

namespace WCDB {

std::shared_ptr<Config> CheckpointConfig::config()
{
    static std::shared_ptr<Config> *s_config =
        new std::shared_ptr<Config>(new CheckpointConfig());
    return *s_config;
}

bool CheckpointConfig::invoke(Handle *handle) const
{
    static std::once_flag s_once;
    std::call_once(s_once, []() {
        Dispatch::async("com.Tencent.WCDB.Checkpoint",
                        std::bind(&CheckpointConfig::loopQueue,
                                  static_cast<CheckpointConfig *>(
                                      CheckpointConfig::config().get())));
    });

    handle->setNotificationWhenCommitted(
        std::bind(
            &CheckpointConfig::onCommited,
            static_cast<CheckpointConfig *>(CheckpointConfig::config().get()),
            std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3),
        nullptr);
    return true;
}

void CheckpointConfig::onCommited(Handle *handle, int pages, void *)
{
    reQueue(handle->path, pages);
}

void CheckpointConfig::loopQueue()
{
    m_timedQueue.loop(std::bind(&CheckpointConfig::onTimed, this,
                                std::placeholders::_1, std::placeholders::_2));
}

void CheckpointConfig::onTimed(const std::string &path, const int &pages) const
{
    std::shared_ptr<Database> database =
        Database::databaseWithExistingPath(path);
    if (database == nullptr || !database->isOpened()) {
        return;
    }
    bool result = database->execute(m_checkpointPassive);
    if (result && pages > 1000) {
        //Passive checkpoint can write WAL data back to database file as much as possible without blocking the db. After this, Truncate checkpoint will write the rest WAL data back to db file and truncate it into zero byte file.
        //As a result, checkpoint process will not block the database too long.
        database->execute(m_checkpointTruncate);
    }
}

void CheckpointConfig::reQueue(const std::string &path, int pages)
{
    if (pages > 100) {
        m_timedQueue.reQueue(path, pages);
    }
}

void CheckpointConfig::blockedStopQueue()
{
    m_timedQueue.stop();
    m_timedQueue.waitUntilDone();
}

CheckpointConfig::CheckpointConfig()
    : Config("checkpoint", CheckpointConfig::order)
    , m_timedQueue(2)
    , m_checkpointTruncate(
          StatementPragma().pragma(Pragma::walCheckpoint()).to("TRUNCATE"))
    , m_checkpointPassive(
          StatementPragma().pragma(Pragma::walCheckpoint()).to("PASSIVE"))
{
}

CheckpointConfig::~CheckpointConfig()
{
    blockedStopQueue();
}

} //namespace WCDB
