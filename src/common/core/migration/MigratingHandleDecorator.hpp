//
// Created by qiuwenchen on 2023/11/15.
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

#include "DecorativeHandle.hpp"
#include "Migration.hpp"

namespace WCDB {

class MigrationInfo;

class MigratingHandleDecorator : public HandleDecorator, public Migration::Binder {
private:
    using Super = HandleDecorator;
#pragma mark - Initialize
public:
    MigratingHandleDecorator(Migration &migration);
    ~MigratingHandleDecorator() override;

    void decorate(Decorative *handle) override final;

#pragma mark - Meta
    Optional<StringViewSet>
    getColumns(const Schema &schema, const UnsafeStringView &table) override final;
    bool addColumn(const Schema &schema,
                   const UnsafeStringView &table,
                   const ColumnDef &column) override final;
    bool rebindUnionView(const UnsafeStringView &table, const Columns &columns);
    bool checkSourceTable(const UnsafeStringView &table, const UnsafeStringView &sourceTable);
    bool attachDatabase(const MigrationBaseInfo *attachInfo);

#pragma mark - Binder
protected:
    bool commitTransaction() override final;
    void rollbackTransaction() override final;

    bool bindInfos(const StringViewMap<const MigrationInfo *> &migratings) override final;
    Optional<const MigrationInfo *> getBindingInfo(const UnsafeStringView &table);

private:
    bool rebindViews(const StringViewMap<const MigrationInfo *> &migratings);
    bool rebindSchemas(const StringViewMap<const MigrationInfo *> &migratings);

    bool trySynchronousTransactionAfterAttached();

    bool m_createdNewViewInTransaction;

#pragma mark - Info Initializer
protected:
    bool attachSourceDatabase(const MigrationUserInfo &userInfo) override final;
    InnerHandle *getCurrentHandle() const override final;
    const StringView &getDatabasePath() const override final;

#pragma mark - Statement
public:
    DecorativeHandleStatement *
    getStatement(const UnsafeStringView &skipDecorator) override final;
    void finalizeStatements() override final;
};

} // namespace WCDB
