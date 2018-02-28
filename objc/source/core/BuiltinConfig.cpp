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

#include <WCDB/BuiltinConfig.hpp>
#include <WCDB/BuiltinStatement.hpp>
#include <WCDB/Database.hpp>
#include <WCDB/HandleStatement.hpp>
#include <WCDB/timed_queue.hpp>
#include <WCDB/utility.hpp>
#include <queue>
#include <thread>
#include <vector>

namespace WCDB {

const Config BuiltinConfig::basic(
    "basic",
    [](std::shared_ptr<Handle> &handle, Error &error) -> bool {

        if (handle->isReadonly()) {
            //Get Journal Mode
            std::shared_ptr<HandleStatement> handleStatement =
                handle->prepare(BuiltinStatement::getJournalMode);
            if (!handleStatement) {
                error = handle->getError();
                return false;
            }
            handleStatement->step();
            if (!handleStatement->isOK()) {
                error = handleStatement->getError();
                return false;
            }
            std::string journalMode =
                handleStatement->getValue<WCDB::ColumnType::Text>(0);
            handleStatement->finalize();

            if (strcasecmp(journalMode.c_str(), "WAL") == 0) {
                // See also: http://www.sqlite.org/wal.html#readonly
                Error::Abort("It is not possible to open read-only WAL "
                             "databases.");
                return false;
            }
            return true;
        }

        //Locking Mode
        {
            //Get Locking Mode
            std::shared_ptr<HandleStatement> handleStatement =
                handle->prepare(BuiltinStatement::getLockingMode);
            if (!handleStatement) {
                error = handle->getError();
                return false;
            }
            handleStatement->step();
            if (!handleStatement->isOK()) {
                error = handleStatement->getError();
                return false;
            }
            std::string lockingMode =
                handleStatement->getValue<WCDB::ColumnType::Text>(0);
            handleStatement->finalize();

            //Set Locking Mode
            if (strcasecmp(lockingMode.c_str(), "NORMAL") != 0 &&
                !handle->exec(BuiltinStatement::setLockingModeNormal)) {
                error = handle->getError();
                return false;
            }
        }

        //Synchronous
        {

            if (!handle->exec(BuiltinStatement::setSynchronousNormal)) {
                error = handle->getError();
                return false;
            }
        }

        //Journal Mode
        {
            //Get Journal Mode
            std::shared_ptr<HandleStatement> handleStatement =
                handle->prepare(BuiltinStatement::getJournalMode);
            if (!handleStatement) {
                error = handle->getError();
                return false;
            }
            handleStatement->step();
            if (!handleStatement->isOK()) {
                error = handleStatement->getError();
                return false;
            }
            std::string journalMode =
                handleStatement->getValue<WCDB::ColumnType::Text>(0);
            handleStatement->finalize();

            //Set Journal Mode
            if (strcasecmp(journalMode.c_str(), "WAL") != 0 &&
                !handle->exec(BuiltinStatement::setJournalModeWAL)) {
                error = handle->getError();
                return false;
            }
        }

        //Fullfsync
        {
            if (!handle->exec(BuiltinStatement::setFullFSync)) {
                error = handle->getError();
                return false;
            }
        }

        error.reset();
        return true;
    },
    Order::Basic);

std::shared_ptr<PerformanceTrace> BuiltinConfig::s_globalPerformanceTrace;
std::shared_ptr<SQLTrace> BuiltinConfig::s_globalSQLTrace;

void BuiltinConfig::SetGlobalPerformanceTrace(
    const PerformanceTrace &globalTrace)
{
    s_globalPerformanceTrace.reset(new PerformanceTrace(globalTrace));
}

void BuiltinConfig::SetGlobalSQLTrace(const SQLTrace &globalTrace)
{
    s_globalSQLTrace.reset(new SQLTrace(globalTrace));
}

const Config BuiltinConfig::trace(
    "trace",
    [](std::shared_ptr<Handle> &handle, Error &error) -> bool {
        {
            std::shared_ptr<PerformanceTrace> trace = s_globalPerformanceTrace;
            if (trace) {
                handle->setPerformanceTrace(*trace.get());
            }
        }
        {
            std::shared_ptr<SQLTrace> trace = s_globalSQLTrace;
            if (trace) {
                handle->setSQLTrace(*trace.get());
            }
        }
        return true;
    },
    Order::Trace);

const Config
BuiltinConfig::cipherWithKey(const void *key, int keySize, int pageSize)
{
    std::shared_ptr<std::vector<unsigned char>> keys(
        new std::vector<unsigned char>((unsigned char *) key,
                                       (unsigned char *) key + keySize));
    return Config("cipher",
                  [keys, pageSize](std::shared_ptr<Handle> &handle,
                                   Error &error) -> bool {

                      //Set Cipher Key
                      bool result = handle->setCipherKey(keys->data(),
                                                         (int) keys->size());
                      if (!result) {
                          error = handle->getError();
                          return false;
                      }

                      //Set Cipher Page Size
                      if (!handle->exec(StatementPragma().pragma(
                              Pragma::CipherPageSize, pageSize))) {
                          error = handle->getError();
                          return false;
                      }

                      error.reset();
                      return true;
                  },
                  Order::Cipher);
}

const Config BuiltinConfig::checkpoint(
    "checkpoint",
    [](std::shared_ptr<Handle> &handle, Error &error) -> bool {
        static std::once_flag s_flag;
        std::call_once(s_flag, []() { s_checkpointThread.detach(); });
        handle->registerCommittedHook(
            [](Handle *handle, int pages, void *) {
                if (pages > 1000) {
                    s_timedQueue.reQueue(handle->path, pages);
                }
            },
            nullptr);
        return true;
    },
    Order::Checkpoint);

TimedQueue<std::string, int> BuiltinConfig::s_timedQueue(2);

std::thread BuiltinConfig::s_checkpointThread([]() {
    pthread_setname_np("WCDB-checkpoint");
    static std::atomic<bool> s_stop(false);
    atexit([]() {
        s_timedQueue.stop();
        while (s_timedQueue.running());
    });
    s_timedQueue.loop(
        [](const std::string &path, const int &pages) {
            Database database(path, true); // Get Existing Database Only
            if (database.getType() != CoreType::None) {
                printf("will checkpoint\n");
                WCDB::Error innerError;
                if (pages > 5000) {
                    database.exec(BuiltinStatement::checkpointTruncate,
                                  innerError);
                } else {
                    database.exec(BuiltinStatement::checkpointPassive,
                                  innerError);
                }
            }
        });
});
    
void BuiltinConfig::removeKeyFromTimedQueue(const std::string& key)
{
    s_timedQueue.remove(key);
}

const Config
BuiltinConfig::tokenizeWithNames(const std::list<std::string> &names)
{
    return Config(
        "tokenize",
        [names](std::shared_ptr<Handle> &handle, Error &error) -> bool {
            for (const std::string &name : names) {
                const unsigned char *address =
                    FTS::Modules::SharedModules()->getAddress(name);
                NoCopyData data((unsigned char *) &address,
                                sizeof(unsigned char *));

                //Tokenize
                {
                    std::shared_ptr<HandleStatement> handleStatement =
                        handle->prepare(BuiltinStatement::fts3Tokenizer);
                    if (!handleStatement) {
                        error = handle->getError();
                        return false;
                    }
                    handleStatement->bind<ColumnType::Text>(name.c_str(), 1);
                    handleStatement->bind<ColumnType::BLOB>(data, 2);
                    handleStatement->step();
                    if (!handleStatement->isOK()) {
                        error = handleStatement->getError();
                        return false;
                    }
                }
            }

            error.reset();
            return true;
        },
        Order::Tokenize);
}

const Configs BuiltinConfigs::default_({BuiltinConfig::trace,
                                        BuiltinConfig::basic,
                                        BuiltinConfig::checkpoint});

} //namespace WCDB
