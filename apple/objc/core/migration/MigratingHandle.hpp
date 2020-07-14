//
// Created by sanhuazhang on 2019/05/23
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

#include <WCDB/Handle.hpp>
#include <WCDB/Migration.hpp>
#include <WCDB/MigratingHandleStatement.hpp>

namespace WCDB {

class MigrationInfo;

class MigratingHandle final : public Handle, public Migration::Binder {
private:
    using Super = Handle;
#pragma mark - Initialize
public:
    MigratingHandle(Migration &migration);
    ~MigratingHandle() override final;
    
#pragma mark - Meta
    virtual std::optional<std::set<StringView>>
    getColumns(const Schema &schema, const UnsafeStringView &table) override final;

#pragma mark - Binder
protected:
    bool bindInfos(const StringViewMap<const MigrationInfo *> &migratings) override final;

private:
    bool rebindViews(const StringViewMap<const MigrationInfo *> &migratings);
    bool rebindSchemas(const StringViewMap<const MigrationInfo *> &migratings);

    bool trySynchronousTransactionAfterAttached();

#pragma mark - Info Initializer
protected:
    // integer primary, columns
    std::optional<std::pair<bool, std::set<StringView>>>
    getColumnsOfUserInfo(const MigrationUserInfo &userInfo) override final;
    std::optional<bool> sourceTableExists(const MigrationUserInfo &userInfo) override final;
    const StringView &getDatabasePath() const override final;

#pragma mark - Statement
public:
    virtual HandleStatement *getStatement() override final;
    virtual void returnStatement(HandleStatement *handleStatement) override final;
    void finalize() override final;
    void resetAllStatements() override final;

protected:
    virtual void finalizeStatements() override final;
    
private:
    std::list<MigratingHandleStatement> m_migratingHandleStatements;
};

} //namespace WCDB
