//
// Created by qiuwenchen on 2023/11/17.
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

#include "Assemble.hpp"
#include "HandleOperator.hpp"
#include "RepairKit.h"

namespace WCDB {

class AssembleHandleOperator : public HandleOperator, public Repair::AssembleDelegate {
public:
    AssembleHandleOperator(InnerHandle *handle);
    ~AssembleHandleOperator() override;

#pragma mark - Assemble
public:
    void setAssemblePath(const UnsafeStringView &path) override final;
    const StringView &getAssemblePath() const override final;
    const Error &getAssembleError() const override final;

    void suspendAssemble() override final;
    void finishAssemble() override final;

    bool markAsAssembling() override final;
    bool markAsAssembled() override final;

    bool markAsMilestone() override final;

    bool assembleSQL(const UnsafeStringView &sql) override final;

protected:
    StatementPragma m_statementForDisableJounral;
    StatementPragma m_statementForEnableMMap;

#pragma mark - Assemble - Table
public:
    bool assembleTable(const UnsafeStringView &tableName,
                       const UnsafeStringView &sql) override final;
    bool isAssemblingTableWithoutRowid() const override final;
    bool assembleCell(const Repair::Cell &cell) override final;
    void markDuplicatedAsReplaceable(bool replaceable) override final;

protected:
    bool lazyPrepareCell();
    int64_t m_integerPrimary;
    StringView m_table;
    bool m_withoutRowid;
    HandleStatement *m_cellStatement;

#pragma mark - Assemble - Sequence
public:
    bool assembleSequence(const UnsafeStringView &tableName, int64_t sequence) override final;

protected:
    Optional<bool> updateSequence(const UnsafeStringView &tableName, int64_t sequence);
    bool insertSequence(const UnsafeStringView &tableName, int64_t sequence);
    static constexpr const char *s_dummySequence = "wcdb_dummy_sqlite_sequence";
    bool markSequenceAsAssembling();
    bool markSequenceAsAssembled();

    StatementUpdate m_statementForUpdateSequence;
    StatementInsert m_statementForInsertSequence;
};

} //namespace WCDB
