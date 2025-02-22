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

#include "BasicConfig.hpp"
#include "Assertion.hpp"
#include "CommonCore.hpp"
#include "CoreConst.h"
#include "InnerHandle.hpp"
#include "Macro.h"

namespace WCDB {

BasicConfig::BasicConfig()
: Config()
, m_getJournalMode(StatementPragma().pragma(Pragma::journalMode()))
, m_setTempStore(StatementPragma().pragma(Pragma::tempStore()).to(1))
{
}

BasicConfig::~BasicConfig() = default;

bool BasicConfig::invoke(InnerHandle* handle)
{
    static_assert(SQLITE_DEFAULT_SYNCHRONOUS == 1, "");
    static_assert(SQLITE_DEFAULT_WAL_SYNCHRONOUS == 1, "");
    static_assert(SQLITE_DEFAULT_LOCKING_MODE == 0, "");

    handle->enableExtendedResultCodes(true);
    handle->disableCheckpointWhenClosing(true);
    bool succeed = true;
    if (!handle->isReadonly()) {
        handle->setWALFilePersist(true);
        succeed = lazySetJournalMode(handle);
#ifndef __ANDROID__
        succeed = succeed && handle->execute(m_setTempStore);
#endif
    }
    return succeed;
}

#pragma mark - Journal Mode
bool BasicConfig::lazySetJournalMode(InnerHandle* handle)
{
    bool succeed = false;
    handle->markErrorAsIgnorable(Error::Code::Busy);
    int remainingNumberOfBusyRetryTimes = BasicConfigBusyRetryMaxAllowedNumberOfTimes;
    do {
        auto journalMode = getJournalMode(handle);
        if (!journalMode.hasValue()) {
            continue;
        }
        succeed = true;
        if (!handle->liteModeEnable()) {
            if (!journalMode.value().caseInsensitiveEqual("WAL")) {
                if (!handle->canWriteMainDB()) {
                    succeed = false;
                    break;
                } else {
                    succeed = setJournalMode(handle, "WAL");
                }
            }
        } else {
            if (!journalMode.value().caseInsensitiveEqual("OFF")) {
                succeed = setJournalMode(handle, "OFF");
            }
        }
        // These is no busy handler for this busy event.
        // So we should retry from outside.
    } while (--remainingNumberOfBusyRetryTimes > 0 && !succeed
             && handle->getError().isIgnorable());
    handle->markErrorAsUnignorable();
    return succeed;
}

Optional<StringView> BasicConfig::getJournalMode(InnerHandle* handle)
{
    Optional<StringView> ret;
    if (handle->prepare(m_getJournalMode)) {
        if (handle->step()) {
            ret = handle->getText();
        }
        handle->finalize();
    }
    return ret;
}

bool BasicConfig::setJournalMode(InnerHandle* handle, const UnsafeStringView& mode)
{
    return handle->execute(StatementPragma().pragma(Pragma::journalMode()).to(mode));
}

} //namespace WCDB
