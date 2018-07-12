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
#include <WCDB/Dispatch.hpp>

namespace WCDB {

static_assert(BackupConfig::framesForMandatoryCheckpoint > CheckpointConfig::framesForFull, "");

const std::shared_ptr<Config> &BackupConfig::shared()
{
    static const std::shared_ptr<Config> *s_shared
    = new std::shared_ptr<Config>(new BackupConfig);
    return *s_shared;
}

BackupConfig::BackupConfig() : Config(BackupConfig::name)
{
    Dispatch::async("com.Tencent.WCDB.Backup", std::bind(&BackupConfig::loop, this));
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
    0,
    "backup",
    std::bind(&BackupConfig::willCheckpoint, this, std::placeholders::_1, std::placeholders::_2));
    handle->rollbackTransaction();
    if (result) {
        handle->setNotificationWhenCommitted(
        "backup",
        std::bind(&BackupConfig::onCommitted, this, std::placeholders::_1, std::placeholders::_2));
    }
    return result;
}

void BackupConfig::loop()
{
    m_timedQueue.loop(std::bind(
    &BackupConfig::onTimed, this, std::placeholders::_1, std::placeholders::_2));
}

void BackupConfig::onTimed(const std::string &path, const int &frames)
{
    static std::atomic<bool> *s_exit = new std::atomic<bool>(false);
    atexit([]() { s_exit->store(true); });
    if (s_exit->load()) {
        return;
    }

    std::shared_ptr<Database> database = Database::databaseWithExistingPath(path);
    if (database == nullptr || !database->isOpened()) {
        return;
    }
    database->backup();
    LockGuard lockGuard(m_lock);
    m_backedUp[path] = frames;
}

void BackupConfig::onCommitted(Handle *handle, int frames)
{
    int backedUp = 0;
    {
        SharedLockGuard lockGuard(m_lock);
        backedUp = m_backedUp[handle->path];
    }
    if (frames > backedUp + framesIntervalForAutoBackup || frames < backedUp) {
        m_timedQueue.reQueue(handle->path, 0, frames);
    }
}

bool BackupConfig::willCheckpoint(Handle *handle, int frames)
{
    std::shared_ptr<Database> database
    = Database::databaseWithExistingPath(handle->path);
    WCTInnerAssert(database != nullptr);
    if (database->backup(frames)) {
        m_timedQueue.reQueue(handle->path, 60.0, frames);
        return true;
    }
    if (frames > framesForMandatoryCheckpoint) {
        m_timedQueue.reQueue(handle->path, 10.0, frames);
        return true;
    }
    return false;
}

} //namespace WCDB
