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

#include <WCDB/database.hpp>
#include <WCDB/handle_statement.hpp>
#include <WCDB/macro.hpp>
#include <WCDB/utility.hpp>
#include <queue>
#include <sqlcipher/sqlite3.h>
#include <thread>
#include <vector>

namespace WCDB {

const std::string Database::defaultConfigName = "default";
const std::string Database::defaultCipherConfigName = "cipher";
const std::string Database::defaultTraceConfigName = "trace";
const std::string Database::defaultCheckpointConfigName = "checkpoint";
const std::string Database::defaultSyncName = "sync";
std::shared_ptr<Trace> Database::s_globalTrace = nullptr;

const Configs Database::defaultConfigs({
    {
        Database::defaultTraceConfigName,
        [](std::shared_ptr<Handle> &handle, Error &error) -> bool {
            std::shared_ptr<Trace> trace = s_globalTrace;
            if (trace) {
                handle->setTrace(*trace.get());
            }
            return true;
        },
        0,
    },
    {
        Database::defaultCipherConfigName,
        [](std::shared_ptr<Handle> &handle, Error &error) -> bool {
            //place holder
            return true;
        },
        1,
    },
    {
        Database::defaultConfigName,
        [](std::shared_ptr<Handle> &handle, Error &error) -> bool {

            //Locking Mode
            {
                static const StatementPragma s_getLockingMode =
                    StatementPragma().pragma(Pragma::LockingMode);
                static const StatementPragma s_setLockingModeNormal =
                    StatementPragma().pragma(Pragma::LockingMode, "NORMAL");

                //Get Locking Mode
                std::shared_ptr<StatementHandle> statementHandle =
                    handle->prepare(s_getLockingMode);
                if (!statementHandle) {
                    error = handle->getError();
                    return false;
                }
                statementHandle->step();
                if (!statementHandle->isOK()) {
                    error = statementHandle->getError();
                    return false;
                }
                std::string lockingMode =
                    statementHandle->getValue<WCDB::ColumnType::Text>(0);
                statementHandle->finalize();

                //Set Locking Mode
                if (strcasecmp(lockingMode.c_str(), "NORMAL") != 0 &&
                    !handle->exec(s_setLockingModeNormal)) {
                    error = handle->getError();
                    return false;
                }
            }

            //Journal Mode
            {
                static const StatementPragma s_getJournalMode =
                    StatementPragma().pragma(Pragma::JournalMode);
                static const StatementPragma s_setJournalModeWAL =
                    StatementPragma().pragma(Pragma::JournalMode, "WAL");

                //Get Journal Mode
                std::shared_ptr<StatementHandle> statementHandle =
                    handle->prepare(s_getJournalMode);
                if (!statementHandle) {
                    error = handle->getError();
                    return false;
                }
                statementHandle->step();
                if (!statementHandle->isOK()) {
                    error = statementHandle->getError();
                    return false;
                }
                std::string journalMode =
                    statementHandle->getValue<WCDB::ColumnType::Text>(0);
                statementHandle->finalize();

                //Set Journal Mode
                if (strcasecmp(journalMode.c_str(), "WAL") != 0 &&
                    !handle->exec(s_setJournalModeWAL)) {
                    error = handle->getError();
                    return false;
                }
            }

            error.reset();
            return true;
        },
        2,
    },
    {
        Database::defaultSyncName,
        nullptr, //placeholder
        3,
    },
    /*
     {
         Database::defaultCheckpointConfigName,
         [](std::shared_ptr<Handle> &handle, Error &error) -> bool {

             static std::unordered_map<std::string, int> s_checkpointStep;
             static std::mutex s_checkpointStepMutex;
             s_checkpointStepMutex.lock();
             auto iter = s_checkpointStep.find(handle->path);
             if (iter == s_checkpointStep.end()) {
                 iter = s_checkpointStep.insert({handle->path, 1000}).first;
             }
             int *checkpointStepPointer = &iter->second;
             s_checkpointStepMutex.unlock();

             handle->registerCommitedHook(
                 [](Handle *handle, int pages, void *p) {
                     //Since sqlite can't write concurrently, checkpointStepPointer does not need a mutex.
                     int *checkpointStepPointer = (int *) p;
                     if (*checkpointStepPointer != 0) {
                         if (pages > *checkpointStepPointer) {
                             *checkpointStepPointer = 0;
                             if (pthread_main_np() != 0) {
                                 //dispatch checkpoint op to sub-thread
                                 static std::queue<std::string> s_toCheckpoint;
                                 static std::condition_variable s_cond;
                                 static std::mutex s_mutex;
                                 static std::thread s_checkpointThread([]() {
                                     pthread_setname_np(
                                         ("WCDB-" +
                                          Database::defaultCheckpointConfigName)
                                             .c_str());
                                     while (true) {
                                         std::string path;
                                         {
                                             std::unique_lock<std::mutex>
                                                 lockGuard(s_mutex);
                                             if (s_toCheckpoint.empty()) {
                                                 s_cond.wait(lockGuard);
                                                 continue;
                                             }
                                             path = s_toCheckpoint.front();
                                             s_toCheckpoint.pop();
                                         }
                                         Database database(path);
                                         WCDB::Error innerError;
                                         database.exec(
                                             StatementPragma().pragma(
                                                 Pragma::WalCheckpoint),
                                             innerError);
                                     }
                                 });
                                 static std::once_flag s_flag;
                                 std::call_once(s_flag, []() {
                                     s_checkpointThread.detach();
                                 });

                                 std::unique_lock<std::mutex> lockGuard(
                                     s_mutex);
                                 s_toCheckpoint.push(handle->path);
                                 s_cond.notify_one();
                             } else {
                                 handle->exec(StatementPragma().pragma(
                                     Pragma::WalCheckpoint));
                             }
                         }
                     } else {
                         *checkpointStepPointer = pages + 1000;
                     }
                 },
                 checkpointStepPointer);
             return true;
         },
         4,
     }
*/
});

void Database::setConfig(const std::string &name,
                         const Config &config,
                         Configs::Order order)
{
    m_pool->setConfig(name, config, order);
}

void Database::setConfig(const std::string &name, const Config &config)
{
    m_pool->setConfig(name, config);
}

void Database::setCipherKey(const void *key, int size)
{
    std::shared_ptr<std::vector<unsigned char>> keys(
        new std::vector<unsigned char>(size));
    memcpy(keys->data(), key, size);
    m_pool->setConfig(
        Database::defaultCipherConfigName,
        [keys](std::shared_ptr<Handle> &handle, Error &error) -> bool {
            bool result =
                handle->setCipherKey(keys->data(), (int) keys->size());
            error = handle->getError();
            return result;
        });
}

void Database::setTrace(const Trace &trace)
{
    m_pool->setConfig(
        Database::defaultTraceConfigName,
        [trace](std::shared_ptr<Handle> &handle, Error &error) -> bool {
            handle->setTrace(trace);
            return true;
        });
}

void Database::setSyncEnabled(bool sync)
{
    if (sync) {
        m_pool->setConfig(
            Database::defaultSyncName,
            [](std::shared_ptr<Handle> &handle,
               Error &error) -> bool { //Synchronous
                {
                    static const StatementPragma s_setSynchronousFull =
                        StatementPragma().pragma(Pragma::Synchronous, "FULL");

                    if (!handle->exec(s_setSynchronousFull)) {
                        error = handle->getError();
                        return false;
                    }
                }

                //Checkpoint Fullfsync
                {
                    static const StatementPragma s_setCheckpointFullfsync =
                        StatementPragma().pragma(Pragma::CheckpointFullfsync,
                                                 true);

                    if (!handle->exec(s_setCheckpointFullfsync)) {
                        error = handle->getError();
                        return false;
                    }
                }

                //Fullfsync
                {
                    static const StatementPragma s_setFullFsync =
                        StatementPragma().pragma(Pragma::Fullfsync, true);

                    if (!handle->exec(s_setFullFsync)) {
                        error = handle->getError();
                        return false;
                    }
                }

                error.reset();
                return true;
            });
    } else {
        m_pool->setConfig(Database::defaultSyncName, nullptr);
    }
}

void Database::SetGlobalTrace(const Trace &globalTrace)
{
    s_globalTrace.reset(new Trace(globalTrace));
}

} //namespace WCDB
