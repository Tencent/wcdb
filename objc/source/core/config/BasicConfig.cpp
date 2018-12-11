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
#include <WCDB/BasicConfig.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/Macro.hpp>

namespace WCDB {

BasicConfig::BasicConfig()
: Config()
, m_getJournalMode(StatementPragma().pragma(Pragma::journalMode()))
, m_getLockingMode(StatementPragma().pragma(Pragma::lockingMode()))
, m_setFullFSync(StatementPragma().pragma(Pragma::fullfsync()).to(true))
, m_setLockingModeNormal(StatementPragma().pragma(Pragma::lockingMode()).to("NORMAL"))
, m_setSynchronousNormal(StatementPragma().pragma(Pragma::synchronous()).to("NORMAL"))
, m_setJournalModeWAL(StatementPragma().pragma(Pragma::journalMode()).to("WAL"))
{
}

bool BasicConfig::invoke(Handle* handle)
{
    do {
        if (handle->isReadonly()) {
            //Get Journal Mode
            if (!handle->prepare(m_getJournalMode) || !handle->step()) {
                break;
            }
            String journalMode = handle->getText(0);
            handle->finalize();

            // See also: http://www.sqlite.org/wal.html#readonly
            WCTRemedialAssert(strcasecmp(journalMode.c_str(), "WAL") != 0,
                              "It is not possible to open read-only WAL databases.",
                              return false;);
            return true;
        }

        //Get Locking Mode
        if (!handle->prepare(m_getLockingMode) || !handle->step()) {
            break;
        }
        String lockingMode = handle->getText(0);
        handle->finalize();
        if (strcasecmp(lockingMode.c_str(), "NORMAL") != 0) {
            //Set Locking Mode Normal
            if (!handle->execute(m_setLockingModeNormal)) {
                break;
            }
        }

        //Set Synchronous Normal
        if (!handle->execute(m_setSynchronousNormal)) {
            break;
        }

        //Get Journal Mode
        bool retry = false;
        bool succeed = true;
        do {
            retry = false;
            if (!handle->prepare(m_getJournalMode) || !handle->step()) {
                succeed = false;
                break;
            }
            String journalMode = handle->getText(0);
            handle->finalize();
            if (strcasecmp(journalMode.c_str(), "WAL") != 0) {
                //Set Journal Mode WAL
                if (!handle->execute(m_setJournalModeWAL)) {
                    if (handle->getResultCode() == (int) Error::Code::Busy) {
                        retry = true;
                    } else {
                        succeed = false;
                        break;
                    }
                }
            }
        } while (retry);
        if (!succeed) {
            break;
        }

        //Enable Fullfsync
        if (!handle->execute(m_setFullFSync)) {
            break;
        }

        return true;
    } while (false);
    handle->finalize();
    return false;
}

} //namespace WCDB
