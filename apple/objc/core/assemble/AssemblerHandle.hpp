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

#include <WCDB/Handle.hpp>
#include <WCDB/RepairKit.h>

namespace WCDB {

class AssemblerHandle final : public Handle,
                              public Repair::Assembler,
                              public Repair::ReadLocker,
                              public Repair::WriteLocker {
public:
    AssemblerHandle();
    ~AssemblerHandle();

#pragma mark - Common
    void setPath(const String &path) override final;
    const String &getPath() const override final;
    const Error &getError() const override final;

#pragma mark - Assembler
public:
    bool markAsAssembling() override final;
    bool markAsAssembled() override final;

    bool markAsMilestone() override final;

    bool assembleSQL(const String &sql) override final;

protected:
    StatementPragma m_statementForDisableJounral;
    StatementPragma m_statementForEnableMMap;

#pragma mark - Assembler - Table
public:
    bool assembleTable(const String &tableName, const String &sql) override final;
    bool assembleCell(const Repair::Cell &cell) override final;

protected:
    bool lazyPrepareCell();
    int m_integerPrimary;
    String m_table;
    HandleStatement *m_cellStatement;

#pragma mark - Assembler - Sequence
public:
    bool assembleSequence(const String &tableName, int64_t sequence) override final;

protected:
    std::pair<bool, bool> updateSequence(const String &tableName, int64_t sequence);
    bool insertSequence(const String &tableName, int64_t sequence);
    static constexpr const char *s_dummySequence = "wcdb_dummy_sqlite_sequence";
    bool markSequenceAsAssembling();
    bool markSequenceAsAssembled();

    StatementUpdate m_statementForUpdateSequence;
    StatementInsert m_statementForInsertSequence;

#pragma mark - Backup
public:
    bool acquireReadLock() override final;
    bool releaseReadLock() override final;

    bool acquireWriteLock() override final;
    bool releaseWriteLock() override final;

protected:
    StatementBegin m_statementForReadTransaction;
    StatementSelect m_statementForAcquireReadLock;
};

} // namespace WCDB
