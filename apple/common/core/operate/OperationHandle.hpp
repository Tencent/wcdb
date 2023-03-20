//
// Created by sanhuazhang on 2019/06/06
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

#pragma once

#include "CoreConst.h"
#include "InnerHandle.hpp"
#include "RepairKit.h"

namespace WCDB {

class OperationHandle final : public InnerHandle,
                              public Repair::BackupSharedDelegate,
                              public Repair::BackupExclusiveDelegate {
public:
    OperationHandle();
    ~OperationHandle() override final;

protected:
    void doSuspend(bool suspend) override final;

#pragma mark - Integrity
public:
    void checkIntegrity();

protected:
    StatementPragma m_statementForIntegrityCheck;

#pragma mark - Backup
public:
    void setBackupPath(const UnsafeStringView &path) override final;
    const StringView &getBackupPath() const override final;
    const Error &getBackupError() const override final;

    bool acquireBackupSharedLock() override final;
    bool releaseBackupSharedLock() override final;

    bool acquireBackupExclusiveLock() override final;
    bool releaseBackupExclusiveLock() override final;

    void finishBackup() override final;

#pragma mark - Cipher
public:
    const Error &getCipherError() const override final;
    bool openCipherInMemory(bool onlyUsedCipherKey) override final;
    bool isCipherDB() const override final;
    void closeCipher() override final;

    void *getCipherContext() override final;
    size_t getCipherPageSize() override final;
    StringView getCipherSalt() override final;
    bool setCipherSalt(const UnsafeStringView &salt) override final;

protected:
    StatementBegin m_statementForReadTransaction;
    StatementSelect m_statementForAcquireReadLock;
    StatementSelect m_statementForGetFTSTable;
};

} // namespace WCDB
