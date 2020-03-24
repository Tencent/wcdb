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

#include <WCDB/Assertion.hpp>
#include <WCDB/MigratingHandle.hpp>
#include <WCDB/MigrationInfo.hpp>
#include <WCDB/StringView.hpp>

namespace WCDB {

#pragma mark - Initialize
MigratingHandle::MigratingHandle(Migration& migration)
: Handle()
, Migration::Binder(migration)
{
    Super::returnStatement(m_mainStatement);
    m_mainStatement = getStatement();
}

MigratingHandle::~MigratingHandle()
{
    returnStatement(m_mainStatement);
    m_mainStatement = nullptr;
}

#pragma mark - Info Initializer
std::optional<std::pair<bool, std::set<StringView>>>
MigratingHandle::getColumnsOfUserInfo(const MigrationUserInfo& userInfo)
{
    auto exists = tableExists(Schema::main(), userInfo.getTable());
    if (!exists.has_value()) {
        return std::nullopt;
    }

    bool integerPrimary = false;
    std::set<StringView> names;
    if (exists.value()) {
        auto optionalMetas = getTableMeta(Schema::main(), userInfo.getTable());
        if (!optionalMetas.has_value()) {
            return std::nullopt;
        }
        auto& metas = optionalMetas.value();
        integerPrimary = ColumnMeta::getIndexOfIntegerPrimary(metas) >= 0;
        for (const auto& meta : metas) {
            names.emplace(meta.name);
        }
    }
    return std::make_pair(integerPrimary, names);
}

std::optional<bool> MigratingHandle::sourceTableExists(const MigrationUserInfo& userInfo)
{
    Schema schema = userInfo.getSchemaForSourceDatabase();
    if (!schema.syntax().isMain()) {
        auto optionalAttacheds
        = getValues(MigrationInfo::getStatementForSelectingDatabaseList(), 1);
        if (!optionalAttacheds.has_value()) {
            return std::nullopt;
        }
        std::set<StringView>& attacheds = optionalAttacheds.value();
        if (attacheds.find(schema.getDescription()) == attacheds.end()) {
            if (!executeStatement(userInfo.getStatementForAttachingSchema())
                || !trySynchronousTransactionAfterAttached()) {
                return std::nullopt;
            }
        }
    }
    return tableExists(schema, userInfo.getSourceTable());
}

const StringView& MigratingHandle::getDatabasePath() const
{
    return getPath();
}

#pragma mark - Binder
bool MigratingHandle::rebindViews(const StringViewMap<const MigrationInfo*>& migratings)
{
    StringViewMap<const MigrationInfo*> views2MigratingInfos;
    for (const auto& iter : migratings) {
        const MigrationInfo* info = iter.second;
        WCTAssert(info != nullptr);
        views2MigratingInfos.emplace(info->getUnionedView(), info);
    }

    // get existing unioned views
    auto exists = tableExists(Schema::temp(), Syntax::masterTable);
    if (!exists.has_value()) {
        return false;
    }
    if (exists.value()) {
        auto existingViews
        = getValues(MigrationInfo::getStatementForSelectingUnionedView(), 0);
        if (!existingViews.has_value()) {
            return false;
        }

        for (const auto& existingView : existingViews.value()) {
            WCTAssert(existingView.hasPrefix(MigrationInfo::getUnionedViewPrefix()));
            auto iter = views2MigratingInfos.find(existingView);
            if (iter != views2MigratingInfos.end()) {
                // it is already created
                views2MigratingInfos.erase(iter);
            } else {
                // it is no longer needed
                if (!executeStatement(
                    MigrationInfo::getStatementForDroppingUnionedView(existingView))) {
                    return false;
                }
            }
        }
    }

    // create all needed views
    for (const auto& iter : views2MigratingInfos) {
        if (!executeStatement(iter.second->getStatementForCreatingUnionedView())) {
            return false;
        }
    }
    return true;
}

bool MigratingHandle::rebindSchemas(const StringViewMap<const MigrationInfo*>& migratings)
{
    StringViewMap<const MigrationInfo*> schemas2MigratingInfos;
    for (const auto& iter : migratings) {
        const MigrationInfo* info = iter.second;
        WCTAssert(info != nullptr);
        if (info->isCrossDatabase()) {
            schemas2MigratingInfos.emplace(
            info->getSchemaForSourceDatabase().getDescription(), info);
        }
    }

    auto existingSchemas
    = getValues(MigrationInfo::getStatementForSelectingDatabaseList(), 1);
    if (!existingSchemas.has_value()) {
        return false;
    }

    for (const auto& existingSchema : existingSchemas.value()) {
        if (existingSchema.hasPrefix(MigrationInfo::getSchemaPrefix())) {
            auto iter = schemas2MigratingInfos.find(existingSchema);
            if (iter != schemas2MigratingInfos.end()) {
                // it is already attached
                schemas2MigratingInfos.erase(iter);
            } else {
                if (!isInTransaction()) {
                    // it is not longer needed
                    if (!executeStatement(
                        MigrationInfo::getStatementForDetachingSchema(existingSchema))) {
                        return false;
                    }
                }
            }
        }
    }
    bool attached = false;
    // attach all needed schemas
    for (const auto& iter : schemas2MigratingInfos) {
        if (!executeStatement(iter.second->getStatementForAttachingSchema())) {
            return false;
        }
        attached = true;
    }
    if (attached) {
        return trySynchronousTransactionAfterAttached();
    }
    return true;
}

bool MigratingHandle::bindInfos(const StringViewMap<const MigrationInfo*>& migratings)
{
    return rebindViews(migratings) && rebindSchemas(migratings);
}

bool MigratingHandle::trySynchronousTransactionAfterAttached()
{
    bool succeed = true;
    if (isInTransaction()) {
        markErrorAsIgnorable(Error::Code::Error);
        static const StatementBegin* s_synchronousTransaction
        = new StatementBegin(StatementBegin().beginImmediate());
        succeed = executeStatement(*s_synchronousTransaction);
        WCTAssert(!succeed);
        if (!succeed && getError().isIgnorable()) {
            succeed = true;
        }
        markErrorAsUnignorable();
    }
    return succeed;
}

#pragma mark - Statement
void MigratingHandle::finalize()
{
    Super::finalize();
    stopReferenced();
}

HandleStatement *MigratingHandle::getStatement()
{
    m_migratingHandleStatements.push_back(MigratingHandleStatement(this));
    return &m_migratingHandleStatements.back();
}

void MigratingHandle::returnStatement(HandleStatement *handleStatement)
{
    if (handleStatement != nullptr) {
        for (auto iter = m_migratingHandleStatements.begin(); iter != m_migratingHandleStatements.end(); ++iter) {
            if (&(*iter) == handleStatement) {
                m_migratingHandleStatements.erase(iter);
                return;
            }
        }
        WCTAssert(false);
    }
}

void MigratingHandle::finalizeStatements()
{
    for (auto &handleStatement : m_migratingHandleStatements) {
        handleStatement.finalize();
    }
}

void MigratingHandle::resetAllStatements()
{
    for (auto &handleStatement : m_migratingHandleStatements) {
        if(!handleStatement.isPrepared())continue;
        handleStatement.reset();
    }
}

} //namespace WCDB
