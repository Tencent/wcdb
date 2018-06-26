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
#include <WCDB/BackupConfig.hpp>
#include <WCDB/CheckpointConfig.hpp>
#include <WCDB/Database.hpp>

namespace WCDB {

static_assert(BackupConfig::framesForMandatoryCheckpoint >
                  CheckpointConfig::framesForFull,
              "");

const std::shared_ptr<Config> &BackupConfig::shared()
{
    static const std::shared_ptr<Config> *s_shared =
        new std::shared_ptr<Config>(new BackupConfig);
    return *s_shared;
}

BackupConfig::BackupConfig() : Config(BackupConfig::name)
{
}

BackupConfig::~BackupConfig()
{
    m_timedQueue.stop();
    m_timedQueue.waitUntilDone();
}

bool BackupConfig::invoke(Handle *handle)
{
    if (!handle->beginTransaction()) {
        return false;
    }
    bool result = handle->setNotificationWhenCheckpoint(
        std::bind(&BackupConfig::willCheckpoint, this, std::placeholders::_1,
                  std::placeholders::_2));
    handle->rollbackTransaction();
    if (result) {
        handle->setNotificationWhenCommitted(
            "backup", std::bind(&BackupConfig::onCommitted, this,
                                std::placeholders::_1, std::placeholders::_2));
    }
    return result;
}

void BackupConfig::onCommitted(Handle *handle, int frames)
{
    int backedUp = 0;
    {
        SharedLockGuard lockGuard(m_lock);
        backedUp = m_backedUp[handle->path];
    }
    if (frames > backedUp + framesIntervalForAutoBackup) {
        m_timedQueue.reQueue(handle->path, 0, frames);
    }
}

bool BackupConfig::willCheckpoint(Handle *handle, int frames)
{
    std::shared_ptr<Database> database =
        Database::databaseWithExistingPath(handle->path);
    WCTInnerAssert(database != nullptr);
    m_timedQueue.reQueue(handle->path, 60.0, 0);
    if (database->backup(frames)) {
        return true;
    }
    if (frames > framesForMandatoryCheckpoint) {
        Error error;
        error.level = Error::Level::Warning;
        error.setCode(Error::Code::Exceed);
        error.message = "Number of frames exceeds, mandatory checkpoint.";
        error.infos.set("Path", handle->path);
        error.infos.set("Frames", frames);
        Notifier::shared()->notify(error);
        m_timedQueue.reQueue(handle->path, 10.0, 0);
        return true;
    }
    return false;
}

} //namespace WCDB
