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
#include <WCDB/fts_modules.hpp>
#include <WCDB/handle_statement.hpp>
#include <WCDB/macro.hpp>
#include <WCDB/timed_queue.hpp>
#include <WCDB/utility.hpp>
#include <queue>
#include <thread>
#include <vector>

namespace WCDB {

const std::string Database::defaultBasicConfigName = "basic";
const std::string Database::defaultCipherConfigName = "cipher";
const std::string Database::defaultTraceConfigName = "trace";
const std::string Database::defaultCheckpointConfigName = "checkpoint";
const std::string Database::defaultSynchronousConfigName = "synchronous";
const std::string Database::defaultTokenizeConfigName = "tokenize";
std::shared_ptr<PerformanceTrace> Database::s_globalPerformanceTrace = nullptr;
std::shared_ptr<SQLTrace> Database::s_globalSQLTrace = nullptr;

const Configs Database::defaultConfigs(
    {{
         Database::defaultTraceConfigName,
         [](std::shared_ptr<Handle> &handle, Error &error) -> bool {
             {
                 std::shared_ptr<PerformanceTrace> trace =
                     s_globalPerformanceTrace;
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
         (Configs::Order) Database::ConfigOrder::Trace,
     },
     {
         Database::defaultCipherConfigName,
         [](std::shared_ptr<Handle> &handle, Error &error) -> bool {
             //place holder
             return true;
         },
         (Configs::Order) Database::ConfigOrder::Cipher,
     },
     {
         Database::defaultBasicConfigName,
         [](std::shared_ptr<Handle> &handle, Error &error) -> bool {

             if (handle->isReadonly()) {
#if DEBUG
                 static const StatementPragma s_getJournalMode =
                     StatementPragma().pragma(Pragma::JournalMode);

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

                 if (strcasecmp(journalMode.c_str(), "WAL") == 0) {
                     Error::Abort("It is not possible to open read-only WAL "
                                  "databases. See also: "
                                  "http://www.sqlite.org/wal.html#readonly");
                     return false;
                 }
#endif
                 return true;
             }

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

             //Synchronous
             {
                 static const StatementPragma s_setSynchronousNormal =
                     StatementPragma().pragma(Pragma::Synchronous, "NORMAL");

                 if (!handle->exec(s_setSynchronousNormal)) {
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
         },
         (Configs::Order) Database::ConfigOrder::Basic,
     },
     {
         Database::defaultSynchronousConfigName,
         nullptr, //placeholder
         (Configs::Order) Database::ConfigOrder::Synchronous,
     },
     {
         Database::defaultCheckpointConfigName,
         [](std::shared_ptr<Handle> &handle, Error &error) -> bool {
             handle->registerCommittedHook(
                 [](Handle *handle, int pages, void *) {
                     static TimedQueue<std::string> s_timedQueue(2);
                     if (pages > 1000) {
                         s_timedQueue.reQueue(handle->path);
                     }
                     static std::thread s_checkpointThread([]() {
                         pthread_setname_np(
                             ("WCDB-" + Database::defaultCheckpointConfigName)
                                 .c_str());
                         while (true) {
                             s_timedQueue.waitUntilExpired(
                                 [](const std::string &path) {
                                     Database database(path);
                                     WCDB::Error innerError;
                                     database.exec(StatementPragma().pragma(
                                                       Pragma::WalCheckpoint),
                                                   innerError);
                                 });
                         }
                     });
                     static std::once_flag s_flag;
                     std::call_once(s_flag,
                                    []() { s_checkpointThread.detach(); });
                 },
                 nullptr);
             return true;
         },
         (Configs::Order) Database::ConfigOrder::Checkpoint,
     },
     {
         Database::defaultTokenizeConfigName,
         nullptr, //placeholder
         (Configs::Order) Database::ConfigOrder::Tokenize,
     }});

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

void Database::setCipher(const void *key, int keySize, int pageSize)
{
    std::shared_ptr<std::vector<unsigned char>> keys(
        new std::vector<unsigned char>(keySize));
    memcpy(keys->data(), key, keySize);
    m_pool->setConfig(Database::defaultCipherConfigName,
                      [keys, pageSize](std::shared_ptr<Handle> &handle,
                                       Error &error) -> bool {

                          //Set Cipher Key
                          bool result = handle->setCipherKey(
                              keys->data(), (int) keys->size());
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
                      });
}

void Database::setPerformanceTrace(const PerformanceTrace &trace)
{
    m_pool->setConfig(
        Database::defaultTraceConfigName,
        [trace](std::shared_ptr<Handle> &handle, Error &error) -> bool {
            handle->setPerformanceTrace(trace);
            return true;
        });
}

void Database::setSynchronousFull(bool full)
{
    if (full) {
        m_pool->setConfig(
            Database::defaultSynchronousConfigName,
            [](std::shared_ptr<Handle> &handle, Error &error) -> bool {

                //Synchronous
                {
                    static const StatementPragma s_setSynchronousFull =
                        StatementPragma().pragma(Pragma::Synchronous, "FULL");

                    if (!handle->exec(s_setSynchronousFull)) {
                        error = handle->getError();
                        return false;
                    }
                }

                error.reset();
                return true;
            });
        //disable checkpoint opti for sync
        m_pool->setConfig(Database::defaultCheckpointConfigName, nullptr);
    } else {
        m_pool->setConfig(Database::defaultSynchronousConfigName, nullptr);
        m_pool->setConfig(Database::defaultCheckpointConfigName,
                          Database::defaultConfigs.getConfigByName(
                              Database::defaultCheckpointConfigName));
    }
}

void Database::setTokenizes(const std::list<std::string> &tokenizeNames)
{
    m_pool->setConfig(
        Database::defaultTokenizeConfigName,
        [tokenizeNames](std::shared_ptr<Handle> &handle, Error &error) -> bool {
            for (const std::string &tokenizeName : tokenizeNames) {
                const void *address =
                    FTS::Modules::SharedModules()->getAddress(tokenizeName);

                if (!address) {
                    Error::Abort("Tokenize name is not registered");
                }

                //Tokenize
                {
                    std::shared_ptr<StatementHandle> statementHandle =
                        handle->prepare(StatementSelect::Fts3Tokenizer);
                    if (!statementHandle) {
                        error = handle->getError();
                        return false;
                    }
                    statementHandle->bind<WCDB::ColumnType::Text>(
                        tokenizeName.c_str(), 1);
                    statementHandle->bind<WCDB::ColumnType::BLOB>(
                        &address, sizeof(address), 2);
                    statementHandle->step();
                    if (!statementHandle->isOK()) {
                        error = statementHandle->getError();
                        return false;
                    }
                }
            }

            error.reset();
            return true;
        });
}

void Database::SetGlobalPerformanceTrace(const PerformanceTrace &globalTrace)
{
    s_globalPerformanceTrace.reset(new PerformanceTrace(globalTrace));
}

void Database::SetGlobalSQLTrace(const SQLTrace &globalTrace)
{
    s_globalSQLTrace.reset(new SQLTrace(globalTrace));
}

} //namespace WCDB
