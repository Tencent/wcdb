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

#pragma once

#include <WCDB/Config.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

class BasicConfig final : public Config {
public:
    BasicConfig();

    bool invoke(Handle* handle) override final;

#pragma mark - Pragma
private:
    bool getOrSetPragmaBegin(Handle* handle, const StatementPragma& get);
    bool getOrSetPragmaEnd(Handle* handle, const StatementPragma& set, bool conditionToSet);

#pragma mark - Pragma - Journal Mode
protected:
    bool lazySetJournalModeWAL(Handle* handle);

private:
    const StatementPragma m_getJournalMode;
    const StatementPragma m_setJournalModeWAL;

#pragma mark - Pragma - Locking Mode
protected:
    bool lazySetLockingModeNormal(Handle* handle);

private:
    const StatementPragma m_getLockingMode;
    const StatementPragma m_setLockingModeNormal;

#pragma mark - Pragma - Synchronous
protected:
    bool lazySetSynchronousNormal(Handle* handle);

private:
    const StatementPragma m_setSynchronousNormal;
    const StatementPragma m_getSynchronous;

#pragma mark - Pragma - FullFsync
protected:
    bool lazyEnableFullFsync(Handle* handle);

private:
    const StatementPragma m_enableFullfsync;
    const StatementPragma m_isFullfsync;
};

} //namespace WCDB
