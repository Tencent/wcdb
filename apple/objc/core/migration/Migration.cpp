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
#include <WCDB/Error.hpp>
#include <WCDB/Migration.hpp>
#include <WCDB/StringView.hpp>

namespace WCDB {

MigrationEvent::~MigrationEvent()
{
}

#pragma mark - Initialize
Migration::Migration(MigrationEvent* event)
: m_filter(nullptr), m_event(event), m_tableAcquired(false), m_migrated(true)
{
}

void Migration::filterTable(const Filter& filter)
{
    LockGuard lockGuard(m_lock);
    purge();
    m_filter = filter;
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
    return m_filter != nullptr;
}

std::set<StringView> Migration::getPathsOfSourceDatabases() const
{
    std::set<StringView> paths;
    {
        SharedLockGuard lockGuard(m_lock);
        for (const auto& info : m_holder) {
            if (info.isCrossDatabase()) {
                if (m_migratings.find(&info) != m_migratings.end()
                    || m_dumpster.find(&info) != m_dumpster.end()
                    || m_referenceds.find(&info) != m_referenceds.end()) {
                    // it's an active info
                    paths.emplace(info.getSourceDatabase());
                }
            }
        }
    }
    return paths;
}

Migration::InfoInitializer::~InfoInitializer()
{
}

bool Migration::initInfo(InfoInitializer& initializer, const UnsafeStringView& table)
{
    // do not migrate sqlite builtin table
    if (table.hasPrefix(Syntax::builtinTablePrefix)) {
        markAsNoNeedToMigrate(table);
        return true;
    }

    MigrationUserInfo userInfo(initializer.getDatabasePath(), table);
    {
        SharedLockGuard lockGuard(m_lock);
        WCTInnerAssert(m_filter != nullptr);
        m_filter(userInfo);
    }
    if (!userInfo.shouldMigrate()) {
        markAsNoNeedToMigrate(table);
        return true;
    }

    bool succeed = false;
    bool exists = false;
    std::tie(succeed, exists) = initializer.sourceTableExists(userInfo);
    if (!succeed) {
        return false;
    }
    if (!exists) {
        markAsNoNeedToMigrate(table);
        return true;
    }

    bool containsPrimaryKey = false;
    std::set<StringView> columns;
    std::tie(succeed, containsPrimaryKey, columns)
    = initializer.getColumnsOfUserInfo(userInfo);
    if (!succeed) {
        return false;
    }
    LockGuard lockGuard(m_lock);
    if (m_filted.find(table) == m_filted.end()) {
        m_migrated = false;
        if (columns.empty()) {
            // it's not created
            m_hints.emplace(table);
            m_tableAcquired = false;
        } else {
            m_holder.push_back(MigrationInfo(userInfo, columns, containsPrimaryKey));
            const MigrationInfo* hold = &m_holder.back();
            m_migratings.emplace(hold);
            m_referenceds.emplace(hold, 0);
            m_filted.emplace(table, hold);
            m_hints.erase(StringView(table));
        }
    }
    return true;
}

void Migration::markAsNoNeedToMigrate(const UnsafeStringView& table)
{
    LockGuard lockGuard(m_lock);
    if (m_filted.find(table) == m_filted.end()) {
        m_filted.emplace(table, nullptr);
    }
    m_hints.erase(StringView(table));
}

bool Migration::hintThatTableWillBeCreated(InfoInitializer& initializer,
                                           const UnsafeStringView& table)
{
    return initInfo(initializer, table);
}

void Migration::markAsUnreferenced(const MigrationInfo* info)
{
    LockGuard lockGuard(m_lock);
    WCTInnerAssert(info != nullptr);
    WCTInnerAssert(m_filted.find(info->getTable()) != m_filted.end());
    WCTInnerAssert(m_hints.find(info->getTable()) == m_hints.end());
    WCTInnerAssert(m_migratings.find(info) == m_migratings.end());
    WCTInnerAssert(m_referenceds.find(info) != m_referenceds.end());
    WCTInnerAssert(m_referenceds.find(info)->second == 0);
    WCTInnerAssert(m_dumpster.find(info) == m_dumpster.end());
    m_referenceds.erase(info);
    m_dumpster.emplace(info);
}

void Migration::markAsDropped(const MigrationInfo* info)
{
    LockGuard lockGuard(m_lock);
    WCTInnerAssert(info != nullptr);
    WCTInnerAssert(m_filted.find(info->getTable()) != m_filted.end());
    WCTInnerAssert(m_hints.find(info->getTable()) == m_hints.end());
    WCTInnerAssert(m_migratings.find(info) == m_migratings.end());
    WCTInnerAssert(m_referenceds.find(info) == m_referenceds.end());
    WCTInnerAssert(m_dumpster.find(info) != m_dumpster.end());
    m_dumpster.erase(info);
}

void Migration::markAsMigrated(const MigrationInfo* info)
{
    LockGuard lockGuard(m_lock);
    WCTInnerAssert(info != nullptr);
    WCTInnerAssert(m_filted.find(info->getTable()) != m_filted.end());
    WCTInnerAssert(m_hints.find(info->getTable()) == m_hints.end());
    WCTInnerAssert(m_migratings.find(info) != m_migratings.end());
    m_migratings.erase(info);
    auto iter = m_referenceds.find(info);
    WCTInnerAssert(iter != m_referenceds.end());
    if (iter->second == 0) {
        markAsUnreferenced(info);
    }
}

void Migration::retainInfo(const MigrationInfo* info)
{
    WCTInnerAssert(info != nullptr);
    LockGuard lockGuard(m_lock);
    WCTInnerAssert(m_filted.find(info->getTable()) != m_filted.end());
    WCTInnerAssert(m_hints.find(info->getTable()) == m_hints.end());
    WCTInnerAssert(m_migratings.find(info) != m_migratings.end());
    WCTInnerAssert(m_dumpster.find(info) == m_dumpster.end());
    auto iter = m_referenceds.find(info);
    WCTInnerAssert(iter != m_referenceds.end());
    ++iter->second;
}

void Migration::releaseInfo(const MigrationInfo* info)
{
    WCTInnerAssert(info != nullptr);
    LockGuard lockGuard(m_lock);
    WCTInnerAssert(m_filted.find(info->getTable()) != m_filted.end());
    WCTInnerAssert(m_hints.find(info->getTable()) == m_hints.end());
    WCTInnerAssert(m_dumpster.find(info) == m_dumpster.end());
    auto iter = m_referenceds.find(info);
    WCTInnerAssert(iter != m_referenceds.end());
    WCTInnerAssert(iter->second > 0);
    if (--iter->second == 0 && m_migratings.find(info) == m_migratings.end()) {
        markAsUnreferenced(info);
    }
}

std::pair<bool, RecyclableMigrationInfo> Migration::getInfo(const UnsafeStringView& table)
{
    LockGuard lockGuard(m_lock);
    bool filted = false;
    auto iter = m_filted.find(table);
    if (iter != m_filted.end()) {
        filted = true;
        const MigrationInfo* info = iter->second;
        if (m_migratings.find(info) != m_migratings.end()) {
            retainInfo(info);
            return { true,
                     RecyclableMigrationInfo(
                     info, std::bind(&Migration::releaseInfo, this, std::placeholders::_1)) };
        }
    }
    return { filted, nullptr };
}

#pragma mark - Bind
Migration::Binder::Binder(Migration& migration)
: m_migration(migration), m_binding(false), m_rebind(false)
{
}

Migration::Binder::~Binder()
{
}

void Migration::Binder::startBinding()
{
    WCTInnerAssert(m_binding == false);
    m_binding = true;
    WCTInnerAssert(m_referenceds.empty());
}

bool Migration::Binder::stopBinding(bool succeed)
{
    WCTInnerAssert(m_binding);
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

void Migration::Binder::stopReferenced()
{
    m_referenceds.clear();
}

std::pair<bool, const MigrationInfo*>
Migration::Binder::bindTable(const UnsafeStringView& table)
{
    WCTInnerAssert(m_binding);
    bool succeed = true;
    RecyclableMigrationInfo info = nullptr;
    if (!table.empty()) {
        std::tie(succeed, info) = m_migration.getOrInitInfo(*this, table);
        if (succeed) {
            m_referenceds.emplace(table, info);
        }
    }
    return { succeed, info.get() };
}

bool Migration::Binder::hintThatTableWillBeCreated(const UnsafeStringView& table)
{
    return m_migration.hintThatTableWillBeCreated(*this, table);
}

const MigrationInfo* Migration::Binder::getBoundInfo(const UnsafeStringView& table)
{
    WCTInnerAssert(m_binding == false);
    const MigrationInfo* info = nullptr;
    auto iter = m_bounds.find(table);
    if (iter != m_bounds.end()) {
        info = iter->second;
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

std::pair<bool, RecyclableMigrationInfo>
Migration::getOrInitInfo(InfoInitializer& initializer, const UnsafeStringView& table)
{
    bool get = false; // succeed
    RecyclableMigrationInfo info = nullptr;
    std::tie(get, info) = getInfo(table);
    if (!get && initInfo(initializer, table)) {
        std::tie(get, info) = getInfo(table);
    }
    return { get, info };
}

#pragma mark - Step
Migration::Stepper::~Stepper()
{
}

std::pair<bool, bool> Migration::step(Migration::Stepper& stepper)
{
    bool succeed = true;
    bool worked = false;
    bool retry = false;
    do {
        retry = false;
        std::tie(succeed, worked) = tryDropUnreferencedTable(stepper);
        if (!succeed || worked) {
            return { succeed, false };
        }
        std::tie(succeed, worked) = tryMigrateRows(stepper);
        if (!succeed || worked) {
            return { succeed, false };
        }
        // acquire table is a readonly operation.
        // retry if and only if it works.
        std::tie(succeed, retry) = tryAcquireTables(stepper);
        if (!succeed) {
            return { succeed, false };
        }
    } while (retry);
    WCTInnerAssert(succeed);
    WCTInnerAssert(!worked);

    {
        SharedLockGuard lockGuard(m_lock);
        WCTInnerAssert(m_tableAcquired);
        if (!m_dumpster.empty() || !m_migratings.empty() || !m_referenceds.empty()) {
            // wait until all tables unreferenced
            return { true, false };
        }
    }
    LockGuard lockGuard(m_lock);
    WCTInnerAssert(m_tableAcquired);
    WCTInnerAssert(m_dumpster.empty());
    WCTInnerAssert(m_migratings.empty());
    WCTInnerAssert(m_referenceds.empty());
    WCTInnerAssert(m_hints.empty());
    m_migrated = true;
    if (m_event != nullptr) {
        m_event->didMigrate(nullptr);
    }
    return { true, true };
}

std::pair<bool, bool> Migration::tryDropUnreferencedTable(Migration::Stepper& stepper)
{
    bool succeed = true;
    bool worked = false;
    const MigrationInfo* info = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        if (!m_dumpster.empty()) {
            info = *m_dumpster.begin();
            WCTInnerAssert(info != nullptr);
        }
    }
    if (info != nullptr) {
        succeed = stepper.dropSourceTable(info);
        if (succeed) {
            worked = true;
            markAsDropped(info);

            // table migrated
            SharedLockGuard lockGuard(m_lock);
            WCTInnerAssert(m_migratings.find(info) == m_migratings.end());
            WCTInnerAssert(m_referenceds.find(info) == m_referenceds.end());
            WCTInnerAssert(m_dumpster.find(info) == m_dumpster.end());
            if (m_event != nullptr) {
                m_event->didMigrate(info);
            }
        }
    }
    return { succeed, worked };
}

std::pair<bool, bool> Migration::tryMigrateRows(Migration::Stepper& stepper)
{
    const MigrationInfo* info = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_migratings.empty()) {
            return { true, false };
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
    WCTInnerAssert(info != nullptr);
    bool done = false;
    if (!stepper.migrateRows(info, done)) {
        return { false, false };
    }
    if (done) {
        markAsMigrated(info);
    }
    return { true, true };
}

std::pair<bool, bool> Migration::tryAcquireTables(Migration::Stepper& stepper)
{
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_tableAcquired) {
            return { true, false };
        }
    }
    bool succeed;
    std::set<StringView> tables;
    std::tie(succeed, tables) = stepper.getAllTables();
    if (succeed) {
        tables.insert(m_hints.begin(), m_hints.end());
        for (const auto& table : tables) {
            WCTInnerAssert(!table.hasPrefix(Syntax::builtinTablePrefix));
            if (!initInfo(stepper, table)) {
                return { false, false };
            }
        }
        LockGuard lockGuard(m_lock);
        m_tableAcquired = true;
    }
    return { succeed, succeed };
}

#pragma mark - Event
bool Migration::isMigrated() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_migrated;
}

} // namespace WCDB
