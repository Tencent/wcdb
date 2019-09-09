//
// Created by sanhuazhang on 2019/05/02
//

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
// Fullfsync
, m_enableFullfsync(StatementPragma().pragma(Pragma::checkpointFullfsync()).to(true))
{
}

BasicConfig::~BasicConfig() = default;

bool BasicConfig::invoke(Handle* handle)
{
    static_assert(SQLITE_DEFAULT_SYNCHRONOUS == 1, "");
    static_assert(SQLITE_DEFAULT_WAL_SYNCHRONOUS == 1, "");
    static_assert(SQLITE_DEFAULT_LOCKING_MODE == 0, "");

    handle->enableExtendedResultCodes(true);
    handle->disableCheckpointWhenClosing(true);
    bool succeed = true;
    if (!handle->isReadonly()) {
        succeed = lazySetJournalModeWAL(handle) && handle->execute(m_enableFullfsync);
    }
    return succeed;
}

#pragma mark - Pragma
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
    WCTAssert(handle->isPrepared());
    handle->finalize();
    bool succeed = true;
    if (conditionToSet) {
        succeed = handle->execute(set);
    }
    return succeed;
}

#pragma mark - Pragma - Journal Mode
bool BasicConfig::lazySetJournalModeWAL(Handle* handle)
{
    bool succeed = false;
    handle->markErrorAsIgnorable(Error::Code::Busy);
    int remainingNumberOfBusyRetryTimes = BasicConfigBusyRetryMaxAllowedNumberOfTimes;
    do {
        succeed
        = getOrSetPragmaBegin(handle, m_getJournalMode)
          && getOrSetPragmaEnd(
          handle, m_setJournalModeWAL, !handle->getText(0).caseInsensiveEqual("WAL"));
    } while (--remainingNumberOfBusyRetryTimes > 0 && !succeed
             && handle->getError().isIgnorable());
    handle->markErrorAsUnignorable();
    return succeed;
}

} //namespace WCDB
