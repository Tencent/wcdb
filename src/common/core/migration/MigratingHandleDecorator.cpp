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

#include "MigratingHandleDecorator.hpp"
#include "CompressionConst.hpp"
#include "MigratingStatementDecorator.hpp"

namespace WCDB {

#pragma mark - Basic

MigratingHandleDecorator::MigratingHandleDecorator(Migration& migration)
: HandleDecorator(), Migration::Binder(migration), m_createdNewViewInTransaction(false)
{
}

MigratingHandleDecorator::~MigratingHandleDecorator() = default;

void MigratingHandleDecorator::decorate(Decorative* handle)
{
    Super::decorate(handle);
    WCDBSwizzleDecorativeFunction(handle, MigratingHandleDecorator, getColumns);
    WCDBSwizzleDecorativeFunction(handle, MigratingHandleDecorator, addColumn);
    WCDBSwizzleDecorativeFunction(handle, MigratingHandleDecorator, commitTransaction);
    WCDBSwizzleDecorativeFunction(handle, MigratingHandleDecorator, rollbackTransaction);
    WCDBSwizzleDecorativeFunction(handle, MigratingHandleDecorator, getStatement);
    WCDBSwizzleDecorativeFunction(handle, MigratingHandleDecorator, finalizeStatements);
}

#pragma mark - Meta

Optional<const MigrationInfo*>
MigratingHandleDecorator::getBindingInfo(const UnsafeStringView& table)
{
    const MigrationInfo* boundInfo = getBoundInfo(table);
    if (boundInfo != nullptr) {
        return boundInfo;
    }
    Optional<const MigrationInfo*> result;

    startBinding();

    result = bindTable(table);

    bool needBinding = result.hasValue() && result.value() != nullptr;

    bool success = stopBinding(needBinding);
    if (needBinding && !success) {
        return Optional<const MigrationInfo*>();
    }

    return result;
}

Optional<StringViewSet>
MigratingHandleDecorator::getColumns(const Schema& schema, const UnsafeStringView& table)
{
    auto ret = Super::getColumns(schema, table);
    if (!ret.succeed()) {
        return ret;
    }
    if (!schema.syntax().isMain()) {
        return ret;
    }

    auto info = getBindingInfo(table);
    if (!info.succeed()) {
        return Optional<StringViewSet>();
    }
    if (info.value() == nullptr) {
        return ret;
    }
    auto sourceColumns = Super::getColumns(
    info.value()->getSchemaForSourceDatabase(), info.value()->getSourceTable());
    if (!sourceColumns.succeed()) {
        return NullOpt;
    }
    auto iterator = ret->begin();
    while (iterator != ret->end()) {
        if (sourceColumns->find(*iterator) == sourceColumns->end()) {
            iterator = ret->erase(iterator);
        } else {
            iterator++;
        }
    }
    return ret;
}

bool MigratingHandleDecorator::addColumn(const Schema& schema,
                                         const UnsafeStringView& table,
                                         const ColumnDef& column)
{
    if (!Super::addColumn(schema, table, column)) {
        return false;
    }

    if (!schema.syntax().isMain()
        || column.syntax().column.value().name.hasPrefix(CompressionColumnTypePrefix)) {
        return true;
    }

    auto info = getBindingInfo(table);
    if (!info.succeed()) {
        return false;
    }
    if (info.value() == nullptr) {
        return true;
    }

    return Super::addColumn(
    info.value()->getSchemaForSourceDatabase(), info.value()->getSourceTable(), column);
}

bool MigratingHandleDecorator::rebindUnionView(const UnsafeStringView& table,
                                               const Columns& columns)
{
    auto info = getBindingInfo(table);
    if (!info.succeed()) {
        return false;
    }
    if (info.value() == nullptr) {
        return true;
    }

    return getHandle()->runTransactionIfNotInTransaction([&](InnerHandle* handle) {
        HandleStatement handleStatement = HandleStatement(handle);
        bool succeed = handleStatement.prepare(MigrationInfo::getStatementForDroppingUnionedView(
                       info.value()->getUnionedView()))
                       && handleStatement.step();
        handleStatement.finalize();
        if (!succeed) {
            return false;
        }
        StatementCreateView createView
        = info.value()->getStatementForCreatingUnionedView(columns);
        succeed = handleStatement.prepare(createView) && handleStatement.step();
        handleStatement.finalize();
        return succeed;
    });
}

bool MigratingHandleDecorator::checkSourceTable(const UnsafeStringView& table,
                                                const UnsafeStringView& sourceTable)
{
    auto info = getBindingInfo(table);
    if (!info.succeed()) {
        return false;
    }
    if (info.value() == nullptr) {
        return false;
    }
    return info.value()->getSourceTable().compare(sourceTable) == 0;
}

bool MigratingHandleDecorator::attachDatabase(const MigrationBaseInfo* attachInfo)
{
    const StatementAttach& attach = attachInfo->getStatementForAttachingSchema();
    const Data& cipher = attachInfo->getSourceCipher();
    HandleStatement handleStatement(getHandle());
    bool succeed = handleStatement.prepare(attach);
    if (succeed) {
        if (!cipher.empty()) {
            handleStatement.bindBLOB(cipher);
        }
        succeed = handleStatement.step();
        handleStatement.finalize();
    }
    if (succeed && attachInfo->needRawSourceCipher()) {
        attachInfo->setRawSourceCipher(
        getHandle()->getRawCipherKey(attachInfo->getSchemaForSourceDatabase()));
    }
    return succeed;
}

#pragma mark - Info Initializer
bool MigratingHandleDecorator::attachSourceDatabase(const MigrationUserInfo& userInfo)
{
    const Schema& schema = userInfo.getSchemaForSourceDatabase();
    if (!schema.syntax().isMain()) {
        auto optionalAttacheds = getHandle()->getValues(
        MigrationInfo::getStatementForSelectingDatabaseList(), 1);
        if (!optionalAttacheds.succeed()) {
            return false;
        }
        StringViewSet& attacheds = optionalAttacheds.value();
        if (attacheds.find(schema.getDescription()) == attacheds.end()) {
            if (!attachDatabase(&userInfo) || !trySynchronousTransactionAfterAttached()) {
                return false;
            }
        }
    }
    return true;
}

InnerHandle* MigratingHandleDecorator::getCurrentHandle() const
{
    return getHandle();
}

const StringView& MigratingHandleDecorator::getDatabasePath() const
{
    return getHandle()->getPath();
}

#pragma mark - Binder
bool MigratingHandleDecorator::rebindViews(const StringViewMap<const MigrationInfo*>& migratings)
{
    StringViewMap<const MigrationInfo*> views2MigratingInfos;
    InnerHandle* handle = getHandle();
    for (const auto& iter : migratings) {
        const MigrationInfo* info = iter.second;
        WCTAssert(info != nullptr);
        views2MigratingInfos.emplace(info->getUnionedView(), info);
    }

    // get existing unioned views
    auto exists = handle->tableExists(Schema::temp(), Syntax::masterTable);
    if (!exists.succeed()) {
        return false;
    }
    if (exists.value()) {
        auto existingViews
        = handle->getValues(MigrationInfo::getStatementForSelectingUnionedView(), 0);
        if (!existingViews.succeed()) {
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
                if (!handle->executeStatement(
                    MigrationInfo::getStatementForDroppingUnionedView(existingView))) {
                    return false;
                }
            }
        }
    }

    bool hasNewView = false;
    // create all needed views
    for (const auto& iter : views2MigratingInfos) {
        HandleStatement* statement = handle->getStatement(DecoratorMigratingHandleStatement);
        bool ret = statement->prepare(iter.second->getStatementForCreatingUnionedView());
        ret = ret && statement->step();
        statement->finalize();
        handle->returnStatement(statement);
        if (!ret) {
            return false;
        }
        hasNewView = true;
    }
    if (hasNewView && handle->isInTransaction()) {
        m_createdNewViewInTransaction = true;
    }
    return true;
}

bool MigratingHandleDecorator::rebindSchemas(const StringViewMap<const MigrationInfo*>& migratings)
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

    auto existingSchemas = getHandle()->getValues(
    MigrationInfo::getStatementForSelectingDatabaseList(), 1);
    if (!existingSchemas.succeed()) {
        return false;
    }

    for (const auto& existingSchema : existingSchemas.value()) {
        if (existingSchema.hasPrefix(MigrationDatabaseInfo::getSchemaPrefix())) {
            auto iter = schemas2MigratingInfos.find(existingSchema);
            if (iter != schemas2MigratingInfos.end()) {
                // it is already attached
                schemas2MigratingInfos.erase(iter);
            } else {
                if (!getHandle()->isInTransaction()) {
                    // it is not longer needed
                    if (!getHandle()->executeStatement(
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
        if (!attachDatabase(iter.second)) {
            return false;
        }
        attached = true;
    }
    if (attached) {
        return trySynchronousTransactionAfterAttached();
    }
    return true;
}

bool MigratingHandleDecorator::commitTransaction()
{
    bool ret = Super::commitTransaction();
    if (ret && !getHandle()->isInTransaction()) {
        m_createdNewViewInTransaction = false;
        setTableInfoCommitted(true);
    }
    return ret;
}

void MigratingHandleDecorator::rollbackTransaction()
{
    Super::rollbackTransaction();
    if (m_createdNewViewInTransaction) {
        setNeedRebind();
        m_createdNewViewInTransaction = false;
        setTableInfoCommitted(false);
    }
}

bool MigratingHandleDecorator::bindInfos(const StringViewMap<const MigrationInfo*>& migratings)
{
    return rebindViews(migratings) && rebindSchemas(migratings);
}

bool MigratingHandleDecorator::trySynchronousTransactionAfterAttached()
{
    bool succeed = true;
    if (getHandle()->isInTransaction()) {
        getHandle()->markErrorAsIgnorable(Error::Code::Error);
        static const StatementBegin* s_synchronousTransaction
        = new StatementBegin(StatementBegin().beginImmediate());
        succeed = getHandle()->executeStatement(*s_synchronousTransaction);
        WCTAssert(!succeed);
        if (!succeed && getHandle()->getError().isIgnorable()) {
            succeed = true;
        }
        getHandle()->markErrorAsUnignorable();
    }
    return succeed;
}

#pragma mark - Statement
DecorativeHandleStatement*
MigratingHandleDecorator::getStatement(const UnsafeStringView& skipDecorator)
{
    DecorativeHandleStatement* statement = Super::getStatement(skipDecorator);
    if (!statement->containDecorator(DecoratorMigratingHandleStatement)
        && !skipDecorator.equal(DecoratorMigratingHandleStatement)
        && !skipDecorator.equal(DecoratorAllType)) {
        statement->tryAddDecorator<MigratingStatementDecorator>(
        DecoratorMigratingHandleStatement, this);
    }
    return statement;
}

void MigratingHandleDecorator::finalizeStatements()
{
    Super::finalizeStatements();
    stopReferenced();
}

} //namespace WCDB
