//
// Created by sanhuazhang on 2019/05/02
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

#include "Migration.hpp"
#include "Assertion.hpp"
#include "HandleStatement.hpp"
#include "InnerHandle.hpp"
#include "Notifier.hpp"
#include "StringView.hpp"
#include "WCDBError.hpp"

namespace WCDB {

MigrationEvent::~MigrationEvent() = default;

#pragma mark - Initialize
Migration::Migration(MigrationEvent* event)
: m_tableAcquired(false), m_migrated(true), m_event(event)
{
}

void Migration::addMigration(const UnsafeStringView& sourcePath,
                             const UnsafeData& sourceCipher,
                             const TableFilter& filter)
{
    LockGuard lockGuard(m_lock);
    if (filter != nullptr) {
        m_migrationInfo.insert_or_assign(
        sourcePath,
        std::make_shared<MigrationDatabaseInfo>(sourcePath, sourceCipher, filter));
    } else {
        m_migrationInfo.erase(sourcePath);
    }
    purge();
}

void Migration::purge()
{
    LockGuard lockGuard(m_lock);
    m_migratings.clear();
    m_dumpster.clear();
    m_referenceds.clear();
    m_holder.clear();
    m_filted.clear();
    m_hints.clear();
    m_tableAcquired = false;
    m_migrated = false;
}

bool Migration::shouldMigrate() const
{
    SharedLockGuard lockGuard(m_lock);
    return !m_migrationInfo.empty();
}

std::set<StringView> Migration::getPathsOfSourceDatabases() const
{
    std::set<StringView> paths;
    {
        SharedLockGuard lockGuard(m_lock);
        for (const auto& info : m_migrationInfo) {
            if (info.second->isCrossDatabase()) {
                paths.emplace(info.second->getSourceDatabase());
            }
        }
    }
    return paths;
}

Migration::InfoInitializer::~InfoInitializer() = default;

bool Migration::initInfo(InfoInitializer& initializer, const UnsafeStringView& table)
{
    // do not migrate sqlite builtin table
    if (table.hasPrefix(Syntax::builtinTablePrefix)) {
        markAsNoNeedToMigrate(table);
        return true;
    }

    StringView targetTable = StringView(table);

    for (auto& info : m_migrationInfo) {
        MigrationUserInfo userInfo = MigrationUserInfo(*info.second, targetTable);
        info.second->getFilter()(userInfo);
        if (!userInfo.shouldMigrate()) {
            continue;
        }

        if (!initializer.attachSourceDatabase(userInfo)) {
            return false;
        }

        auto exists = initializer.checkSourceTableExistsAndHasRowid(userInfo);
        if (!exists.succeed()) {
            return false;
        }
        if (!exists.value()) {
            markAsNoNeedToMigrate(targetTable);
            return true;
        }

        bool targetTableExsits, autoincrement;
        std::set<StringView> columns;
        const char* integerPrimaryKey = NULL;
        if (!initializer.getTargetTableInfo(
            userInfo, targetTableExsits, columns, autoincrement, &integerPrimaryKey)) {
            return false;
        }

        if (autoincrement && !initializer.tryUpdateSequence(userInfo, integerPrimaryKey)) {
            return false;
        }

        LockGuard lockGuard(m_lock);
        if (m_filted.find(targetTable) == m_filted.end()) {
            m_migrated = false;
            if (!targetTableExsits || columns.empty()) {
                // it's not created
                m_hints.emplace(targetTable);
                m_tableAcquired = false;
            } else {
                m_holder.push_back(MigrationInfo(
                userInfo, columns, autoincrement, integerPrimaryKey));
                const MigrationInfo* hold = &m_holder.back();
                m_migratings.emplace(hold);
                m_referenceds.emplace(hold, 0);
                m_filted.insert_or_assign(targetTable, hold);
                m_hints.erase(targetTable);
            }
        }
        return true;
    }

    markAsNoNeedToMigrate(targetTable);
    return true;
}

void Migration::markAsNoNeedToMigrate(const UnsafeStringView& table)
{
    LockGuard lockGuard(m_lock);
    if (m_filted.find(table) == m_filted.end()) {
        m_filted.insert_or_assign(table, nullptr);
    }
    m_hints.erase(table);
}

bool Migration::hintThatTableWillBeCreated(InfoInitializer& initializer,
                                           const UnsafeStringView& table)
{
    return initInfo(initializer, table);
}

void Migration::markAsUnreferenced(const MigrationInfo* info)
{
    LockGuard lockGuard(m_lock);
    WCTAssert(info != nullptr);
    WCTAssert(m_filted.find(info->getTable()) != m_filted.end());
    WCTAssert(m_hints.find(info->getTable()) == m_hints.end());
    WCTAssert(m_migratings.find(info) == m_migratings.end());
    WCTAssert(m_referenceds.find(info) != m_referenceds.end());
    WCTAssert(m_referenceds.find(info)->second == 0);
    WCTAssert(m_dumpster.find(info) == m_dumpster.end());
    m_referenceds.erase(info);
    m_dumpster.emplace(info);
}

void Migration::markAsDropped(const MigrationInfo* info)
{
    LockGuard lockGuard(m_lock);
    WCTAssert(info != nullptr);
    WCTAssert(m_filted.find(info->getTable()) != m_filted.end());
    WCTAssert(m_hints.find(info->getTable()) == m_hints.end());
    WCTAssert(m_migratings.find(info) == m_migratings.end());
    WCTAssert(m_referenceds.find(info) == m_referenceds.end());
    WCTAssert(m_dumpster.find(info) != m_dumpster.end());
    m_dumpster.erase(info);
}

void Migration::markAsMigrated(const MigrationInfo* info)
{
    LockGuard lockGuard(m_lock);
    WCTAssert(info != nullptr);
    WCTAssert(m_filted.find(info->getTable()) != m_filted.end());
    WCTAssert(m_hints.find(info->getTable()) == m_hints.end());
    WCTAssert(m_migratings.find(info) != m_migratings.end());
    m_migratings.erase(info);
    auto iter = m_referenceds.find(info);
    WCTAssert(iter != m_referenceds.end());
    if (iter->second == 0) {
        markAsUnreferenced(info);
    }
}

void Migration::retainInfo(const MigrationInfo* info)
{
    WCTAssert(info != nullptr);
    LockGuard lockGuard(m_lock);
    WCTAssert(m_filted.find(info->getTable()) != m_filted.end());
    WCTAssert(m_hints.find(info->getTable()) == m_hints.end());
    WCTAssert(m_migratings.find(info) != m_migratings.end());
    WCTAssert(m_dumpster.find(info) == m_dumpster.end());
    auto iter = m_referenceds.find(info);
    WCTAssert(iter != m_referenceds.end());
    ++iter->second;
}

void Migration::releaseInfo(const MigrationInfo* info)
{
    WCTAssert(info != nullptr);
    LockGuard lockGuard(m_lock);
    WCTAssert(m_filted.find(info->getTable()) != m_filted.end());
    WCTAssert(m_hints.find(info->getTable()) == m_hints.end());
    WCTAssert(m_dumpster.find(info) == m_dumpster.end());
    auto iter = m_referenceds.find(info);
    WCTAssert(iter != m_referenceds.end());
    WCTAssert(iter->second > 0);
    if (--iter->second == 0 && m_migratings.find(info) == m_migratings.end()) {
        markAsUnreferenced(info);
    }
}

Optional<RecyclableMigrationInfo> Migration::getInfo(const UnsafeStringView& table)
{
    LockGuard lockGuard(m_lock);
    auto iter = m_filted.find(table);
    if (iter != m_filted.end()) {
        const MigrationInfo* info = iter->second;
        if (m_migratings.find(info) != m_migratings.end()) {
            retainInfo(info);
            return RecyclableMigrationInfo(
            info, std::bind(&Migration::releaseInfo, this, std::placeholders::_1));
        }
        return nullptr;
    }
    return NullOpt;
}

#pragma mark - InfoInitializer
Optional<bool>
Migration::InfoInitializer::checkSourceTableExistsAndHasRowid(const MigrationUserInfo& userInfo)
{
    const Schema& schema = userInfo.getSchemaForSourceDatabase();
    const StringView& tableName = userInfo.getSourceTable();
    InnerHandle* handle = getCurrentHandle();
    Optional<bool> optionalExists = handle->tableExists(schema, tableName);
    if (!optionalExists.hasValue() || !optionalExists.value()) {
        return optionalExists;
    }
    bool autoincrement, withoutRowid;
    if (!handle->getTableConfig(schema, tableName, autoincrement, withoutRowid, nullptr)) {
        return NullOpt;
    }
    if (withoutRowid) {
        StringView msg = StringView::formatted(
        "Does not support migrating data from the table without rowid: %s",
        tableName.data());
        handle->notifyError((int) Error::Code::Error, nullptr, msg.data());
        return NullOpt;
    }
    return optionalExists;
}

bool Migration::InfoInitializer::getTargetTableInfo(const MigrationUserInfo& userInfo,
                                                    bool& exists,
                                                    std::set<StringView>& columns,
                                                    bool& autoincrement,
                                                    const char** integerPrimaryKey)
{
    const StringView& tableName = userInfo.getTable();
    InnerHandle* handle = getCurrentHandle();
    auto optionalExists = handle->tableExists(Schema::main(), tableName);
    if (!optionalExists.hasValue()) {
        return false;
    }
    exists = optionalExists.value();
    if (!exists) {
        return true;
    }
    auto optionalMetas = handle->getTableMeta(Schema::main(), userInfo.getTable());
    if (!optionalMetas.succeed()) {
        return false;
    }
    auto& metas = optionalMetas.value();
    for (const auto& meta : metas) {
        columns.emplace(meta.name);
    }
    bool withoutRowid;
    if (!handle->getTableConfig(
        Schema::main(), tableName, autoincrement, withoutRowid, integerPrimaryKey)) {
        return false;
    }
    if (withoutRowid) {
        StringView msg = StringView::formatted(
        "Does not support migrating data to the table without rowid: %s",
        tableName.data());
        handle->notifyError((int) Error::Code::Error, nullptr, msg.data());
        return false;
    }
    return true;
}

bool Migration::InfoInitializer::tryUpdateSequence(const MigrationUserInfo& userInfo,
                                                   const UnsafeStringView& primaryKey)
{
    const StringView& targetTable = userInfo.getTable();
    InnerHandle* handle = getCurrentHandle();
    Column name("name");
    auto sequenceTable = TableOrSubquery(Syntax::sequenceTable).schema(Schema::main());
    auto selectTable
    = StatementSelect().select(name).from(sequenceTable).where(name == targetTable);
    auto tableNames = handle->getValues(selectTable, 1);
    if (!tableNames.hasValue()) {
        return false;
    }
    if (tableNames.value().size() > 0) {
        return true;
    }
    return handle->runTransactionIfNotInTransaction([&](InnerHandle* handle) {
        auto tableNames = handle->getValues(selectTable, 1);
        if (!tableNames.hasValue()) {
            return false;
        }
        if (tableNames.value().size() > 0) {
            return true;
        }
        const StringView& sourceTable = userInfo.getSourceTable();
        const Schema& sourceSchema = userInfo.getSchemaForSourceDatabase();
        auto selectMaxPrimaryKey
        = StatementSelect()
          .select(Column(primaryKey).max())
          .from(TableOrSubquery(sourceTable).schema(sourceSchema));
        HandleStatement handleStatement(handle);
        if (!handleStatement.prepare(selectMaxPrimaryKey)) {
            return false;
        }
        if (!handleStatement.step()) {
            handleStatement.finalize();
            return false;
        }
        int64_t seq = 0;
        if (!handleStatement.done()) {
            seq = handleStatement.getInteger();
        }
        handleStatement.finalize();
        auto insertSeq = StatementInsert()
                         .insertIntoTable(Syntax::sequenceTable)
                         .schema(Schema::main())
                         .values({ targetTable, seq });
        if (!handleStatement.prepare(insertSeq)) {
            return false;
        }
        bool ret = handleStatement.step();
        handleStatement.finalize();
        return ret;
    });
}

#pragma mark - Bind
Migration::Binder::Binder(Migration& migration)
: m_migration(migration), m_binding(false), m_rebind(false)
{
}

Migration::Binder::~Binder() = default;

void Migration::Binder::startBinding()
{
    WCTAssert(m_binding == false);
    m_binding = true;
}

bool Migration::Binder::stopBinding(bool succeed)
{
    WCTAssert(m_binding);
    if (succeed) {
        m_migration.tryReduceBounds(m_bindings);
        for (const auto& iter : m_referenceds) {
            if (iter.second != nullptr) {
                m_bindings.emplace(iter.first, iter.second.get());
            }
        }

        if (m_rebind || m_bounds != m_bindings) {
            succeed = bindInfos(m_bindings);
            if (succeed) {
                m_rebind = false;
                m_bounds = m_bindings;
            } else {
                m_rebind = true;
            }
        } else {
            succeed = true;
        }
    }
    m_binding = false;
    return succeed;
}

void Migration::Binder::setNeedRebind()
{
    m_bounds.clear();
    m_rebind = true;
}

void Migration::Binder::stopReferenced()
{
    m_referenceds.clear();
}

Optional<const MigrationInfo*> Migration::Binder::bindTable(const UnsafeStringView& table)
{
    WCTAssert(m_binding);
    Optional<const MigrationInfo*> info = (MigrationInfo*) nullptr;
    if (!table.empty()) {
        auto optionalReferencedInfo = m_migration.getOrInitInfo(*this, table);
        if (optionalReferencedInfo.succeed()) {
            auto& referencedInfo = optionalReferencedInfo.value();
            m_referenceds.insert_or_assign(table, referencedInfo);
            info = referencedInfo.get();
        }
    }
    return info;
}

bool Migration::Binder::hintThatTableWillBeCreated(const UnsafeStringView& table)
{
    return m_migration.hintThatTableWillBeCreated(*this, table);
}

const MigrationInfo* Migration::Binder::getBoundInfo(const UnsafeStringView& table)
{
    WCTAssert(m_binding == false);
    const MigrationInfo* info = nullptr;
    auto iter = m_bounds.find(table);
    if (iter != m_bounds.end()) {
        info = iter->second;
    }
    return info;
}

Optional<RecyclableMigrationInfo>
Migration::getOrInitInfo(InfoInitializer& initializer, const UnsafeStringView& table)
{
    auto info = getInfo(table);
    if (!info.succeed() && initInfo(initializer, table)) {
        info = getInfo(table);
    }
    return info;
}

void Migration::tryReduceBounds(StringViewMap<const MigrationInfo*>& bounds)
{
    bool reduce = false;
    {
        SharedLockGuard lockGuard(m_lock);
        for (const auto& iter : bounds) {
            if (m_migratings.find(iter.second) == m_migratings.end()) {
                reduce = true;
                break;
            }
        }
    }
    if (reduce) {
        LockGuard lockGuard(m_lock);
        for (auto iter = bounds.begin(); iter != bounds.end();) {
            const MigrationInfo* info = iter->second;
            if (m_migratings.find(info) == m_migratings.end()) {
                iter = bounds.erase(iter);
            } else {
                ++iter;
            }
        }
    }
}

#pragma mark - Step
Migration::Stepper::~Stepper() = default;

Optional<bool> Migration::step(Migration::Stepper& stepper)
{
    auto worked = tryDropUnreferencedTable(stepper);
    if (!worked.succeed()) {
        return NullOpt;
    }
    if (worked.value()) {
        return false;
    }
    worked = tryMigrateRows(stepper);
    if (!worked.succeed()) {
        return NullOpt;
    }
    if (worked.value()) {
        return false;
    }
    // acquire table is a readonly operation.
    // retry if and only if it works.
    worked = tryAcquireTables(stepper);
    if (!worked.succeed()) {
        return NullOpt;
    }
    if (worked.value()) {
        return step(stepper);
    }
    WCTAssert(worked.succeed());
    WCTAssert(!worked.value());

    {
        SharedLockGuard lockGuard(m_lock);
        WCTAssert(m_tableAcquired);
        if (!m_dumpster.empty() || !m_migratings.empty() || !m_referenceds.empty()) {
            // wait until all tables unreferenced
            return false;
        }
    }
    LockGuard lockGuard(m_lock);
    WCTAssert(m_tableAcquired);
    WCTAssert(m_dumpster.empty());
    WCTAssert(m_migratings.empty());
    WCTAssert(m_referenceds.empty());
    WCTAssert(m_hints.empty());
    m_migrated = true;
    if (m_event != nullptr) {
        m_event->didMigrate(nullptr);
    }
    return true;
}

Optional<bool> Migration::tryDropUnreferencedTable(Migration::Stepper& stepper)
{
    const MigrationInfo* info = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        if (!m_dumpster.empty()) {
            info = *m_dumpster.begin();
            WCTAssert(info != nullptr);
        }
    }
    Optional<bool> worked = false;
    if (info != nullptr) {
        if (stepper.dropSourceTable(info)) {
            worked = true;
            markAsDropped(info);

            // table migrated
            SharedLockGuard lockGuard(m_lock);
            WCTAssert(m_migratings.find(info) == m_migratings.end());
            WCTAssert(m_referenceds.find(info) == m_referenceds.end());
            WCTAssert(m_dumpster.find(info) == m_dumpster.end());
            if (m_event != nullptr) {
                m_event->didMigrate(info);
            }
        } else {
            worked = NullOpt;
        }
    }
    return worked;
}

Optional<bool> Migration::tryMigrateRows(Migration::Stepper& stepper)
{
    const MigrationInfo* info = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_migratings.empty()) {
            return false;
        }
        for (const auto& migrating : m_migratings) {
            if (migrating->isCrossDatabase()) {
                // cross database migration first to reduce additional database lock.
                info = migrating;
                break;
            }
        }
        if (info == nullptr) {
            info = *m_migratings.begin();
        }
    }
    WCTAssert(info != nullptr);
    auto migrated = stepper.migrateRows(info);
    if (migrated.succeed() && migrated.value()) {
        markAsMigrated(info);
    }
    return migrated;
}

Optional<bool> Migration::tryAcquireTables(Migration::Stepper& stepper)
{
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_tableAcquired) {
            return false;
        }
    }
    auto optionalTables = stepper.getAllTables();
    if (!optionalTables.succeed()) {
        return NullOpt;
    }
    std::set<StringView>& tables = optionalTables.value();
    tables.insert(m_hints.begin(), m_hints.end());
    for (const auto& table : tables) {
        WCTAssert(!table.hasPrefix(Syntax::builtinTablePrefix));
        if (!initInfo(stepper, table)) {
            return NullOpt;
        }
    }
    LockGuard lockGuard(m_lock);
    m_tableAcquired = true;
    return true;
}

#pragma mark - Event
bool Migration::isMigrated() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_migrated;
}

} // namespace WCDB
