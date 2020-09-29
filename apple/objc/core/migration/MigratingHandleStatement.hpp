//
// Created by qiuwenchen on 2020/3/6.
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

#include <WCDB/HandleStatement.hpp>

namespace WCDB {

class MigratingHandle;

class MigratingHandleStatement final : public HandleStatement {
    friend class MigratingHandle;

private:
    using Super = HandleStatement;

public:
    MigratingHandleStatement() = delete;
    MigratingHandleStatement(const MigratingHandleStatement &) = delete;
    MigratingHandleStatement &operator=(const MigratingHandleStatement &) = delete;
    MigratingHandleStatement(MigratingHandleStatement &&other);
    ~MigratingHandleStatement() override final;

#pragma mark - Migration
public:
    bool prepare(const Statement &statement) override final;
    using Super::isPrepared;
    void finalize() override final;

    using Super::done;
    bool step() override final;
    void reset() override final;

    void bindInteger(const Integer &value, int index) override final;
    void bindDouble(const Float &value, int index) override final;
    void bindText(const Text &value, int index) override final;
    void bindBLOB(const BLOB &value, int index) override final;
    void bindNull(int index) override final;

    using Super::getInteger;
    using Super::getDouble;
    using Super::getText;
    using Super::getBLOB;

    using Super::getType;
    using Super::getOriginColumnName;
    using Super::getColumnName;
    using Super::getColumnTableName;

    using Super::isReadonly;
    using Super::getNumberOfColumns;

protected:
    bool realStep();
    std::optional<std::list<Statement>> process(const Statement &statement);
    bool tryFallbackToUnionedView(Syntax::Schema &schema, StringView &table);
    bool tryFallbackToSourceTable(Syntax::Schema &schema, StringView &table);
    bool m_processing;
    std::shared_ptr<HandleStatement> m_additionalStatement;

protected:
    using Super::isBusy;
    MigratingHandleStatement(MigratingHandle *handle);

#pragma mark - Migrate
protected:
    // For Insert Statement Only
    bool isMigratedPrepared();
    bool prepareMigrate(const Syntax::InsertSTMT &migrated,
                        const Syntax::InsertSTMT &falledBack);
    bool stepMigration(const int64_t &rowid);
    void finalizeMigrate();
    void resetMigrate();

private:
    std::shared_ptr<HandleStatement> m_migrateStatement;
    std::shared_ptr<HandleStatement> m_removeMigratedStatement;
    int m_rowidIndexOfMigratingStatement;
};

} //namespace WCDB
