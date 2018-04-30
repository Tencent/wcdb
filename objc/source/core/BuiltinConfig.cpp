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
#include <WCDB/Dispatch.hpp>
#include <WCDB/Macro.hpp>
#include <WCDB/String.hpp>
#include <WCDB/TimedQueue.hpp>
#include <queue>
#include <vector>

namespace WCDB {

BuiltinConfig *BuiltinConfig::shared()
{
    static BuiltinConfig s_builtinConfig;
    return &s_builtinConfig;
}

BuiltinConfig::BuiltinConfig()
    : basic("basic", BuiltinConfig::basicConfig, Order::Basic)
    , trace("trace", BuiltinConfig::traceConfig, Order::Trace)
    , checkpoint(
          "checkpoint", BuiltinConfig::checkpointConfig, Order::Checkpoint)
    , m_timedQueue(2)
    , defaultConfigs({trace, basic, checkpoint})
{
    Dispatch::async("com.Tencent.WCDB.Checkpoint",
                    [](const std::atomic<bool> &stop) {
                        if (stop.load()) {
                            return;
                        }
                        BuiltinConfig::shared()->loopTimedQueue(stop);
                    },
                    []() {
                        //Since Dispatch::async will wait until the callback done, it's not need to wait here.
                        BuiltinConfig::shared()->stopTimedQueue();
                    });
}

bool BuiltinConfig::basicConfig(Handle *handle)
{
    static const StatementPragma s_getJournalMode =
        StatementPragma().pragma(Pragma::journalMode());

    do {
#ifdef DEBUG
        if (handle->isReadonly()) {
            //Get Journal Mode
            WCDB_BREAK_IF_NOT(handle->prepare(s_getJournalMode));
            WCDB_BREAK_IF_NOT(handle->step());
            std::string journalMode = handle->getText(0);
            handle->finalize();

            // See also: http://www.sqlite.org/wal.html#readonly
            WCTRemedialAssert(
                strcasecmp(journalMode.c_str(), "WAL") != 0,
                "It is not possible to open read-only WAL databases.",
                return false;);
            return true;
        }
#endif //DEBUG

        //Get Locking Mode
        static const StatementPragma s_getLockingMode =
            StatementPragma().pragma(Pragma::lockingMode());
        WCDB_BREAK_IF_NOT(handle->prepare(s_getLockingMode));
        WCDB_BREAK_IF_NOT(handle->step());
        std::string lockingMode = handle->getText(0);
        handle->finalize();
        if (strcasecmp(lockingMode.c_str(), "NORMAL") != 0) {
            //Set Locking Mode Normal
            static const StatementPragma s_setLockingModeNormal =
                StatementPragma().pragma(Pragma::lockingMode()).to("NORMAL");
            WCDB_BREAK_IF_NOT(handle->execute(s_setLockingModeNormal));
        }

        //Set Synchronous Normal
        static const StatementPragma s_setSynchronousNormal =
            StatementPragma().pragma(Pragma::synchronous()).to("NORMAL");
        WCDB_BREAK_IF_NOT(handle->execute(s_setSynchronousNormal));

        //Get Journal Mode
        WCDB_BREAK_IF_NOT(handle->prepare(s_getJournalMode));
        WCDB_BREAK_IF_NOT(handle->step());
        std::string journalMode = handle->getText(0);
        handle->finalize();
        if (strcasecmp(journalMode.c_str(), "WAL") != 0) {
            //Set Journal Mode WAL
            static const StatementPragma s_setJournalModeWAL =
                StatementPragma().pragma(Pragma::journalMode()).to("WAL");
            WCDB_BREAK_IF_NOT(handle->execute(s_setJournalModeWAL));
        }

        //Enable Fullfsync
        static const StatementPragma s_setFullFSync =
            StatementPragma().pragma(Pragma::fullfsync()).to(true);
        WCDB_BREAK_IF_NOT(handle->execute(s_setFullFSync));

        return true;
    } while (false);
    handle->finalize();
    return false;
}

bool BuiltinConfig::traceConfig(Handle *handle)
{
    BuiltinConfig *builtinConfig = BuiltinConfig::shared();
    {
        std::shared_ptr<Handle::PerformanceTraceCallback> trace =
            builtinConfig->m_globalPerformanceTrace;
        if (trace) {
            handle->tracePerformance(*trace.get());
        }
    }
    {
        std::shared_ptr<Handle::SQLTraceCallback> trace =
            builtinConfig->m_globalSQLTrace;
        if (trace) {
            handle->traceSQL(*trace.get());
        }
    }
    return true;
}

bool BuiltinConfig::checkpointConfig(Handle *handle)
{
    handle->setCommittedHook(
        [](Handle *handle, int pages, void *) {
            if (pages > 1000) {
                BuiltinConfig::shared()->m_timedQueue.reQueue(handle->path,
                                                              pages);
            }
        },
        nullptr);
    return true;
}

void BuiltinConfig::stopTimedQueue()
{
    m_timedQueue.stop();
}

void BuiltinConfig::loopTimedQueue(const std::atomic<bool> &stop)
{
    m_timedQueue.loop([&stop](const std::string &path, const int &pages) {
        std::shared_ptr<Database> database =
            Database::databaseWithExistingPath(path);
        if (database == nullptr || !database->isOpened()) {
            return;
        }
        static const StatementPragma s_checkpointPassive =
            StatementPragma().pragma(Pragma::walCheckpoint()).to("PASSIVE");

        if (stop.load()) {
            return;
        }
        bool result = database->execute(s_checkpointPassive);
        if (result && pages > 5000 && !stop.load()) {
            //Passive checkpoint can write WAL data back to database file as much as possible without blocking the db. After this, Truncate checkpoint will write the rest WAL data back to db file and truncate it into zero byte file.
            //As a result, checkpoint process will not block the database too long.
            static const StatementPragma s_checkpointTruncate =
                StatementPragma()
                    .pragma(Pragma::walCheckpoint())
                    .to("TRUNCATE");

            database->execute(s_checkpointTruncate);
        }
    });
}

void BuiltinConfig::setGlobalPerformanceTrace(
    const Handle::PerformanceTraceCallback &globalTrace)
{
    m_globalPerformanceTrace.reset(
        new Handle::PerformanceTraceCallback(globalTrace));
}

void BuiltinConfig::setGlobalSQLTrace(
    const Handle::SQLTraceCallback &globalTrace)
{
    m_globalSQLTrace.reset(new Handle::SQLTraceCallback(globalTrace));
}

Config BuiltinConfig::cipherWithKey(const NoCopyData &cipher, int pageSize)
{
    std::vector<unsigned char> keys((unsigned char *) cipher.data,
                                    (unsigned char *) cipher.data +
                                        cipher.size);
    return Config(
        "cipher",
        [keys, pageSize](Handle *handle) -> bool {
            return handle->setCipherKey(keys.data(), (int) keys.size()) &&
                   handle->execute(StatementPragma()
                                       .pragma(Pragma::cipherPageSize())
                                       .to(pageSize));
        },
        Order::Cipher);
}

Config BuiltinConfig::tokenizeWithNames(const std::list<std::string> &names)
{
    return Config("tokenize",
                  [names](Handle *handle) -> bool {
                      for (const std::string &name : names) {
                          const unsigned char *address =
                              FTS::Modules::shared()->getAddress(name);
                          NoCopyData data((unsigned char *) &address,
                                          sizeof(unsigned char *));

                          //Setup Tokenize
                          static const StatementSelect s_fts3Tokenizer =
                              StatementSelect().select(Expression::function(
                                  "fts3_tokenizer",
                                  {BindParameter(1), BindParameter(2)}));
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

} //namespace WCDB
