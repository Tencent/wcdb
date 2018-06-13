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

#include <WCDB/BasicConfig.hpp>
#include <WCDB/Macro.hpp>

namespace WCDB {

const std::shared_ptr<Config> &BasicConfig::shared()
{
    static const std::shared_ptr<Config> *s_shared =
        new std::shared_ptr<Config>(new BasicConfig);
    return *s_shared;
}

BasicConfig::BasicConfig()
    : Config(BasicConfig::name)
    , m_getJournalMode(StatementPragma().pragma(Pragma::journalMode()))
    , m_getLockingMode(StatementPragma().pragma(Pragma::lockingMode()))
    , m_setFullFSync(StatementPragma().pragma(Pragma::fullfsync()).to(true))
    , m_setLockingModeNormal(
          StatementPragma().pragma(Pragma::lockingMode()).to("NORMAL"))
    , m_setSynchronousNormal(
          StatementPragma().pragma(Pragma::synchronous()).to("NORMAL"))
    , m_setJournalModeWAL(
          StatementPragma().pragma(Pragma::journalMode()).to("WAL"))
{
}

bool BasicConfig::invoke(Handle *handle)
{
    do {
        if (handle->isReadonly()) {
            //Get Journal Mode
            WCDB_BREAK_IF_NOT(handle->prepare(m_getJournalMode));
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

        //Get Locking Mode
        WCDB_BREAK_IF_NOT(handle->prepare(m_getLockingMode));
        WCDB_BREAK_IF_NOT(handle->step());
        std::string lockingMode = handle->getText(0);
        handle->finalize();
        if (strcasecmp(lockingMode.c_str(), "NORMAL") != 0) {
            //Set Locking Mode Normal
            WCDB_BREAK_IF_NOT(handle->execute(m_setLockingModeNormal));
        }

        //Set Synchronous Normal
        WCDB_BREAK_IF_NOT(handle->execute(m_setSynchronousNormal));

        //Get Journal Mode
        WCDB_BREAK_IF_NOT(handle->prepare(m_getJournalMode));
        WCDB_BREAK_IF_NOT(handle->step());
        std::string journalMode = handle->getText(0);
        handle->finalize();
        if (strcasecmp(journalMode.c_str(), "WAL") != 0) {
            //Set Journal Mode WAL
            WCDB_BREAK_IF_NOT(handle->execute(m_setJournalModeWAL));
        }

        //Enable Fullfsync
        WCDB_BREAK_IF_NOT(handle->execute(m_setFullFSync));

        return true;
    } while (false);
    handle->finalize();
    return false;
}

} //namespace WCDB
