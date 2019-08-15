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

#include <WCDB/CoreConst.h>
#include <WCDB/Handle.hpp>
#include <WCDB/RepairKit.h>

namespace WCDB {

class OperationHandle final : public Handle, public Repair::ReadLocker, public Repair::WriteLocker {
public:
    OperationHandle();
    ~OperationHandle() override final;

    void setType(HandleType type);

#pragma mark - Checkpoint
public:
    bool checkpoint();

#pragma mark - Integrity
public:
    void checkIntegrity();

protected:
    StatementPragma m_statementForIntegrityCheck;

#pragma mark - Backup
public:
    void setPath(const UnsafeStringView &path) override final;
    const StringView &getPath() const override final;
    const Error &getError() const override final;

    bool acquireReadLock() override final;
    bool releaseReadLock() override final;

    bool acquireWriteLock() override final;
    bool releaseWriteLock() override final;

    void finish() override final;

protected:
    StatementBegin m_statementForReadTransaction;
    StatementSelect m_statementForAcquireReadLock;
};

} // namespace WCDB
