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
#include <WCDB/Migration.hpp>

namespace WCDB {

class MigrationInfo;

class MigratingHandle final : public Handle, public Migration::Binder {
private:
    using Super = Handle;
#pragma mark - Initialize
public:
    MigratingHandle(Migration &migration);
    ~MigratingHandle();

#pragma mark - Binder
protected:
    bool bindInfos(const StringViewMap<const MigrationInfo *> &migratings) override final;

private:
    bool rebindViews(const StringViewMap<const MigrationInfo *> &migratings);
    bool rebindSchemas(const StringViewMap<const MigrationInfo *> &migratings);

    bool trySynchronousTransactionAfterAttached();

#pragma mark - Info Initializer
protected:
    std::tuple<bool, bool, std::set<StringView>>
    getColumnsOfUserInfo(const MigrationUserInfo &userInfo) override final;
    std::pair<bool, bool> sourceTableExists(const MigrationUserInfo &userInfo) override final;
    StringView getDatabasePath() const override final;

#pragma mark - Migration
public:
    bool prepare(const Statement &statement) override final;
    using Super::isPrepared;
    void finalize() override final;

    using Super::done;
    bool step() override final;
    void reset() override final;

    void bindInteger32(const Integer32 &value, int index) override final;
    void bindInteger64(const Integer64 &value, int index) override final;
    void bindDouble(const Float &value, int index) override final;
    void bindText(const Text &value, int index) override final;
    void bindBLOB(const BLOB &value, int index) override final;
    void bindNull(int index) override final;

    using Super::getInteger32;
    using Super::getInteger64;
    using Super::getDouble;
    using Super::getText;
    using Super::getBLOB;

    using Super::getType;
    using Super::getOriginColumnName;
    using Super::getColumnName;
    using Super::getColumnTableName;

    using Super::isStatementReadonly;
    using Super::getNumberOfColumns;

protected:
    bool realExecute(const std::list<Statement> &statements);
    bool realStep();
    std::pair<bool, std::list<Statement>> process(const Statement &statement);
    bool tryFallbackToUnionedView(Syntax::Schema &schema, StringView &table);
    bool tryFallbackToSourceTable(Syntax::Schema &schema, StringView &table);

    bool m_processing;
    HandleStatement *m_additionalStatement;

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
    HandleStatement *m_migrateStatement;
    HandleStatement *m_removeMigratedStatement;
    int m_rowidIndexOfMigratingStatement;
};

} //namespace WCDB
