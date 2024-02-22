//
// Created by 陈秋文 on 2023/11/15.
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

#include "DecorativeHandleStatement.hpp"
#include "Migration.hpp"
#include <list>

namespace WCDB {

class MigratingStatementDecorator final : public HandleStatementDecorator {
#pragma mark - Basic
public:
    MigratingStatementDecorator(Migration::Binder *binder);
    MigratingStatementDecorator(MigratingStatementDecorator &&other);
    ~MigratingStatementDecorator() override;

    MigratingStatementDecorator() = delete;
    MigratingStatementDecorator(const MigratingStatementDecorator &) = delete;
    MigratingStatementDecorator &operator=(const MigratingStatementDecorator &) = delete;

    void decorate(Decorative *handleStatement) override final;

protected:
    using Super = HandleStatementDecorator;

private:
    Migration::Binder *m_migrationBinder;

#pragma mark - Decorated functions
public:
    bool prepare(const Statement &statement) override final;
    void finalize() override final;
    bool step() override final;
    void reset() override final;
    void clearBindings() override final;

    void bindInteger(const Integer &value, int index) override final;
    void bindDouble(const Float &value, int index) override final;
    void bindText(const Text &value, int index) override final;
    void bindText16(const char16_t *value, size_t valueLength, int index) override final;
    void bindBLOB(const BLOB &value, int index) override final;
    void bindNull(int index) override final;
    void bindPointer(void *ptr, int index, const Text &type, void (*destructor)(void *)) override final;

#pragma mark - Migration
protected:
    bool realStep();
    Optional<std::list<Statement>> process(const Statement &statement);
    bool tryFallbackToUnionedView(Syntax::Schema &schema, StringView &table);
    bool tryFallbackToSourceTable(Syntax::Schema &schema, StringView &table);
    bool m_processing;

    using StatementType = Syntax::Identifier::Type;
    StatementType m_currentStatementType;
    std::list<HandleStatement> m_additionalStatements;

#pragma mark - Insert
protected:
    bool stepInsert();
    void clearMigrateStatus();

private:
    const MigrationInfo *m_migratingInfo;
    Optional<int64_t> m_assignedPrimaryKey;
    int m_primaryKeyIndex;
    int m_rowidBindIndex;

#pragma mark - Update/Delete
protected:
    bool stepUpdateOrDelete();
};

} // namespace WCDB
