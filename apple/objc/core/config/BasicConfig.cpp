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
, m_journalModeWAL(StatementPragma().pragma(Pragma::journalMode()).to("WAL"))
// Locking Mode
, m_lockingModeNormal(StatementPragma().pragma(Pragma::lockingMode()).to("NORMAL"))
// Synchronous
, m_synchronousNormal(StatementPragma().pragma(Pragma::synchronous()).to("NORMAL"))
// Fullfsync
, m_fullfsync(StatementPragma().pragma(Pragma::fullfsync()).to(true))
{
}

bool BasicConfig::executeStatement(Handle* handle, const StatementPragma& statement, int& numberOfRemainingAttempts)
{
    handle->markErrorAsIgnorable(Error::Code::Busy);
    bool succeed = false;
    while (!succeed && numberOfRemainingAttempts > 0) {
        succeed = handle->execute(statement);
        if (!succeed) {
            if (handle->isErrorIgnorable()) {
                --numberOfRemainingAttempts;
            } else {
                numberOfRemainingAttempts = -1;
            }
        }
    }
    handle->markErrorAsUnignorable();
    return succeed;
}

bool BasicConfig::invoke(Handle* handle)
{
    if (handle->isReadonly()) {
        return true;
    }

    handle->enableExtendedResultCodes(true);
    handle->disableCheckpointWhenClosing(true);

    int numberOfRemainingAttempts = BasicConfigBusyRetryMaxAllowedNumberOfTimes;

    return executeStatement(handle, m_lockingModeNormal, numberOfRemainingAttempts)
           && executeStatement(handle, m_synchronousNormal, numberOfRemainingAttempts)
           && executeStatement(handle, m_journalModeWAL, numberOfRemainingAttempts)
           && executeStatement(handle, m_fullfsync, numberOfRemainingAttempts);
}

} //namespace WCDB
