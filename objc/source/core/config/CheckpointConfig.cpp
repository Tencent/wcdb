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
#include <WCDB/CheckpointConfig.hpp>
#include <WCDB/Database.hpp>
#include <WCDB/Dispatch.hpp>
#include <mutex>

namespace WCDB {

static_assert(CheckpointConfig::framesForPassive <
                  CheckpointConfig::framesForFull,
              "");

const std::shared_ptr<Config> &CheckpointConfig::shared()
{
    static const std::shared_ptr<Config> *s_shared =
        new std::shared_ptr<Config>(new CheckpointConfig);
    return *s_shared;
}

CheckpointConfig::CheckpointConfig()
    : Config(CheckpointConfig::name)
    , m_timedQueue(2)
    , m_checkpointTruncate(
          StatementPragma().pragma(Pragma::walCheckpoint()).to("TRUNCATE"))
    , m_checkpointPassive(
          StatementPragma().pragma(Pragma::walCheckpoint()).to("PASSIVE"))
{
    Dispatch::async("com.Tencent.WCDB.Checkpoint",
                    std::bind(&CheckpointConfig::loop, this));
}

CheckpointConfig::~CheckpointConfig()
{
    m_timedQueue.stop();
    m_timedQueue.waitUntilDone();
}

bool CheckpointConfig::invoke(Handle *handle)
{
    handle->setNotificationWhenCommitted(
        std::bind(&CheckpointConfig::onCommitted, this, std::placeholders::_1,
                  std::placeholders::_2));
    return true;
}

void CheckpointConfig::onCommitted(Handle *handle, int frames)
{
    if (frames > framesForPassive) {
        m_timedQueue.reQueue(handle->path, frames);
    }
}

void CheckpointConfig::loop()
{
    m_timedQueue.loop(std::bind(&CheckpointConfig::onTimed, this,
                                std::placeholders::_1, std::placeholders::_2));
}

void CheckpointConfig::onTimed(const std::string &path, const int &frames) const
{
    static std::atomic<bool> s_exit(false);
    atexit([]() { s_exit.store(true); });
    if (s_exit.load()) {
        return;
    }

    std::shared_ptr<Database> database =
        Database::databaseWithExistingPath(path);
    if (database == nullptr || !database->isOpened()) {
        return;
    }
    if (frames < framesForFull) {
        database->execute(m_checkpointPassive);
    } else {
        database->execute(m_checkpointTruncate);
    }
}

} //namespace WCDB
