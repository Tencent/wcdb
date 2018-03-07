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
#include <WCDB/Database.hpp>
#include <WCDB/Macro.hpp>
#include <WCDB/String.hpp>
#include <WCDB/TimedQueue.hpp>
#include <queue>
#include <thread>
#include <vector>

namespace WCDB {

const Config BuiltinConfig::basic(
    "basic",
    [](Handle *handle) -> bool {

        static const StatementPragma s_getJournalMode =
            StatementPragma().pragma(Pragma::JournalMode);
        do {
            if (handle->isReadonly()) {
                //Get Journal Mode
                WCDB_BREAK_IF_NOT(handle->prepare(s_getJournalMode));
                WCDB_BREAK_IF_NOT(handle->step());
                std::string journalMode = handle->getText(0);
                handle->finalize();

                if (strcasecmp(journalMode.c_str(), "WAL") == 0) {
                    // See also: http://www.sqlite.org/wal.html#readonly
                    Error::Abort("It is not possible to open read-only WAL "
                                 "databases.");
                    return false;
                }
                return true;
            }

            //Get Locking Mode
            static const StatementPragma s_getLockingMode =
                StatementPragma().pragma(Pragma::LockingMode);
            WCDB_BREAK_IF_NOT(handle->prepare(s_getLockingMode));
            WCDB_BREAK_IF_NOT(handle->step());
            std::string lockingMode = handle->getText(0);
            handle->finalize();
            if (strcasecmp(lockingMode.c_str(), "NORMAL") != 0) {
                //Set Locking Mode Normal
                static const StatementPragma s_setLockingModeNormal =
                    StatementPragma().pragma(Pragma::LockingMode, "NORMAL");
                WCDB_BREAK_IF_NOT(handle->execute(s_setLockingModeNormal));
            }

            //Set Synchronous Normal
            static const StatementPragma s_setSynchronousNormal =
                StatementPragma().pragma(Pragma::Synchronous, "NORMAL");
            WCDB_BREAK_IF_NOT(handle->execute(s_setSynchronousNormal));

            //Get Journal Mode
            WCDB_BREAK_IF_NOT(handle->prepare(s_getJournalMode));
            WCDB_BREAK_IF_NOT(handle->step());
            std::string journalMode = handle->getText(0);
            handle->finalize();
            if (strcasecmp(journalMode.c_str(), "WAL") != 0) {
                //Set Journal Mode WAL
                static const StatementPragma s_setJournalModeWAL =
                    StatementPragma().pragma(Pragma::JournalMode, "WAL");
                WCDB_BREAK_IF_NOT(handle->execute(s_setJournalModeWAL));
            }

            //Enable Fullfsync
            static const StatementPragma s_setFullFSync =
                StatementPragma().pragma(Pragma::Fullfsync, true);
            WCDB_BREAK_IF_NOT(handle->execute(s_setFullFSync));

            return true;
        } while (false);
        handle->finalize();
        return false;
    },
    Order::Basic);

std::shared_ptr<PerformanceTraceCallback>
    BuiltinConfig::s_globalPerformanceTrace;
std::shared_ptr<SQLTraceCallback> BuiltinConfig::s_globalSQLTrace;

void BuiltinConfig::SetGlobalPerformanceTrace(
    const PerformanceTraceCallback &globalTrace)
{
    s_globalPerformanceTrace.reset(new PerformanceTraceCallback(globalTrace));
}

void BuiltinConfig::SetGlobalSQLTrace(const SQLTraceCallback &globalTrace)
{
    s_globalSQLTrace.reset(new SQLTraceCallback(globalTrace));
}

const Config BuiltinConfig::trace(
    "trace",
    [](Handle *handle) -> bool {
        {
            std::shared_ptr<PerformanceTraceCallback> trace =
                s_globalPerformanceTrace;
            if (trace) {
                handle->setPerformanceTrace(*trace.get());
            }
        }
        {
            std::shared_ptr<SQLTraceCallback> trace = s_globalSQLTrace;
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
                  [keys, pageSize](Handle *handle) -> bool {
                      return handle->setCipherKey(keys->data(),
                                                  (int) keys->size()) &&
                             handle->execute(StatementPragma().pragma(
                                 Pragma::CipherPageSize, pageSize));
                  },
                  Order::Cipher);
}

const Config BuiltinConfig::checkpoint(
    "checkpoint",
    [](Handle *handle) -> bool {
        static std::once_flag s_flag;
        std::call_once(s_flag, []() { s_checkpointThread.detach(); });
        handle->setCommittedHook(
            [](Handle *handle, int pages, void *) {
                if (pages > 1000) {
                    s_timedQueue.reQueue(handle->path, pages);
                }
            },
            nullptr);
        return true;
    },
    Order::Checkpoint);

TimedQueue<std::string, const int> BuiltinConfig::s_timedQueue(2);

std::thread BuiltinConfig::s_checkpointThread([]() {
    pthread_setname_np("WCDB-checkpoint");
    atexit([]() {
        s_timedQueue.stop();
        while (s_timedQueue.running())
            ;
    });
    s_timedQueue.loop([](const std::string &path, int pages) {
        std::shared_ptr<Database> database =
            Database::databaseWithExistingPath(path);
        if (database != nullptr) {
            static const StatementPragma s_checkpointPassive =
                StatementPragma().pragma(Pragma::WalCheckpoint, "PASSIVE");

            database->execute(s_checkpointPassive);
            if (pages > 5000) {
                //Passive checkpoint can write WAL data back to database file as much as possible without blocking the db. After this, Truncate checkpoint will write the rest WAL data back to db file and truncate it into zero byte file.
                //As a result, checkpoint process will not block the database too long.
                static const StatementPragma s_checkpointTruncate =
                    StatementPragma().pragma(Pragma::WalCheckpoint, "TRUNCATE");

                database->execute(s_checkpointTruncate);
            }
        }
    });
});

void BuiltinConfig::removeKeyFromTimedQueue(const std::string &key)
{
    s_timedQueue.remove(key);
}

const Config
BuiltinConfig::tokenizeWithNames(const std::list<std::string> &names)
{
    return Config(
        "tokenize",
        [names](Handle *handle) -> bool {
            for (const std::string &name : names) {
                const unsigned char *address =
                    FTS::Modules::SharedModules()->getAddress(name);
                NoCopyData data((unsigned char *) &address,
                                sizeof(unsigned char *));

                //Setup Tokenize
                static const StatementSelect s_fts3Tokenizer =
                    StatementSelect().select(Expression::Function(
                        "fts3_tokenizer",
                        std::list<Expression>(2, BindParameter::default_)));
                if (handle->prepare(s_fts3Tokenizer)) {
                    handle->bindText(name.c_str(), 1);
                    handle->bindBLOB(data, 2);
                    bool result = handle->step();
                    handle->finalize();
                    return result;
                }
            }
            return true;
        },
        Order::Tokenize);
}

const Configs &BuiltinConfigs::defaultConfig()
{
    static const Configs s_default({BuiltinConfig::trace, BuiltinConfig::basic,
                                    BuiltinConfig::checkpoint});
    return s_default;
}

} //namespace WCDB
