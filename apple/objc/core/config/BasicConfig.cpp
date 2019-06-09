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
#include <WCDB/CoreConst.h>
#include <WCDB/Handle.hpp>
#include <WCDB/Macro.hpp>

namespace WCDB {

BasicConfig::BasicConfig()
: Config()
// Journal Mode
, m_getJournalMode(StatementPragma().pragma(Pragma::journalMode()))
, m_setJournalModeWAL(StatementPragma().pragma(Pragma::journalMode()).to("WAL"))
// Locking Mode
, m_getLockingMode(StatementPragma().pragma(Pragma::lockingMode()))
, m_setLockingModeNormal(StatementPragma().pragma(Pragma::lockingMode()).to("NORMAL"))
// Synchronous
, m_setSynchronousNormal(StatementPragma().pragma(Pragma::synchronous()).to("NORMAL"))
, m_getSynchronous(StatementPragma().pragma(Pragma::synchronous()))
// Fullfsync
, m_enableFullfsync(StatementPragma().pragma(Pragma::fullfsync()).to(true))
, m_isFullfsync(StatementPragma().pragma(Pragma::fullfsync()))
{
}

bool BasicConfig::getOrSetPragmaBegin(Handle* handle, const StatementPragma& get)
{
    bool succeed = false;
    if (handle->prepare(get)) {
        succeed = handle->step();
        if (!succeed) {
            handle->finalize();
        }
    }
    return succeed;
}

bool BasicConfig::getOrSetPragmaEnd(Handle* handle, const StatementPragma& set, bool conditionToSet)
{
    WCTInnerAssert(handle->isPrepared());
    handle->finalize();
    bool succeed = true;
    if (conditionToSet) {
        succeed = handle->execute(set);
    }
    return succeed;
}

bool BasicConfig::invoke(Handle* handle)
{
    if (handle->isReadonly()) {
        return true;
    }

    handle->enableExtendedResultCodes(true);
    handle->disableCheckpointWhenClosing(true);

    bool succeed = false;
    handle->markErrorAsIgnorable(Error::Code::Busy);
    for (int i = 0; i < BasicConfigBusyRetryMaxAllowedNumberOfTimes && !succeed; ++i) {
        succeed = getOrSetPragmaBegin(handle, m_getJournalMode)
                  && getOrSetPragmaEnd(handle,
                                       m_setJournalModeWAL,
                                       !handle->getText(0).isCaseInsensiveEqual("WAL"));
        if (!succeed && !handle->isErrorIgnorable()) {
            break;
        }
    }
    handle->markErrorAsUnignorable();
    if (!succeed) {
        return false;
    }

    if (!getOrSetPragmaBegin(handle, m_getLockingMode)
        || !getOrSetPragmaEnd(handle,
                              m_setLockingModeNormal,
                              !handle->getText(0).isCaseInsensiveEqual("NORMAL"))) {
        return false;
    }

    // 1 for Normal: https://sqlite.org/pragma.html#pragma_synchronous
    if (!getOrSetPragmaBegin(handle, m_getSynchronous)
        || !getOrSetPragmaEnd(handle, m_setSynchronousNormal, handle->getInteger32(0) != 1)) {
        return false;
    }

    if (!getOrSetPragmaBegin(handle, m_isFullfsync)
        || !getOrSetPragmaEnd(handle, m_enableFullfsync, handle->getInteger32(0) != 1)) {
        return false;
    }

    return true;
}

} //namespace WCDB
