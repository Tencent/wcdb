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
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initialize
Migration::Migration()
: m_filter(nullptr), m_migratedNotification(nullptr), m_tableAcquired(false)
{
}

void Migration::filterTable(const Filter& filter)
{
    WCTInnerAssert(m_filted.empty());
    m_filter = filter;
}

bool Migration::shouldMigrate() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_filter != nullptr;
}

Migration::InfoInitializer::~InfoInitializer()
{
}

bool Migration::initInfo(InfoInitializer& initializer, const String& table)
{
    bool succeed = true;
    bool skip = true;
    // do not migrate sqlite builtin table
    if (!table.hasPrefix(Syntax::builtinTablePrefix)) {
        MigrationUserInfo userInfo(initializer.getDatabasePath(), table);
        m_filter(userInfo);
        if (userInfo.shouldMigrate()) {
            std::set<String> columns;
            std::tie(succeed, columns) = initializer.getColumnsForSourceTable(userInfo);
            if (succeed) {
                LockGuard lockGuard(m_lock);
                if (m_filted.find(table) == m_filted.end()) {
                    if (!columns.empty()) {
                        m_holder.push_back(MigrationInfo(userInfo, columns));
                        const MigrationInfo* hold = &m_holder.back();
                        m_migratings.emplace(hold);
                        m_referenceds.emplace(hold, 0);
                        m_filted.emplace(table, hold);
                        skip = false;
                    }
                }
            }
        }
    }
    if (skip) {
        LockGuard lockGuard(m_lock);
        if (m_filted.find(table) == m_filted.end()) {
            m_filted.emplace(table, nullptr);
        }
    }
    return succeed;
}

bool Migration::lazyInitInfo(InfoInitializer& initializer, const String& table)
{
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_filted.find(table) != m_filted.end()) {
            return true;
        }
    }
    return initInfo(initializer, table);
}

void Migration::markAsUnreferenced(const MigrationInfo* info)
{
    LockGuard lockGuard(m_lock);
    WCTInnerAssert(info != nullptr);
    WCTInnerAssert(m_filted.find(info->getTable()) != m_filted.end());
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
    WCTInnerAssert(m_dumpster.find(info) == m_dumpster.end());
    auto iter = m_referenceds.find(info);
    WCTInnerAssert(iter != m_referenceds.end());
    WCTInnerAssert(iter->second > 0);
    if (--iter->second == 0 && m_migratings.find(info) == m_migratings.end()) {
        markAsUnreferenced(info);
    }
}

std::pair<bool, RecyclableMigrationInfo> Migration::getInfo(const String& table)
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
: m_migration(migration), m_binding(false)
{
}

Migration::Binder::~Binder()
{
}

void Migration::Binder::startBinding()
{
    WCTInnerAssert(m_binding == false);
    m_binding = true;
    WCTInnerAssert(m_cache.empty());
}

bool Migration::Binder::stopBinding(bool succeed)
{
    WCTInnerAssert(m_binding);
    if (succeed) {
        m_migration.tryReduceBounds(m_boundsCache);
        for (const auto& iter : m_cache) {
            if (iter.second != nullptr) {
                m_boundsCache.emplace(iter.first, iter.second);
            }
        }
        m_cache.clear();

        if (m_bounds != m_boundsCache) {
            succeed = bindInfos(m_boundsCache);
            if (succeed) {
                m_bounds = m_boundsCache;
            }
        } else {
            succeed = true;
        }
    } else {
        m_cache.clear();
    }
    m_binding = false;
    return succeed;
}

std::pair<bool, const MigrationInfo*> Migration::Binder::bindTable(const String& table)
{
    WCTInnerAssert(m_binding);
    bool succeed = true;
    RecyclableMigrationInfo info = nullptr;
    if (!table.empty()) {
        std::tie(succeed, info) = m_migration.getOrInitInfo(*this, table);
        if (succeed) {
            m_cache.emplace(table, info);
        }
    }
    return { succeed, info.get() };
}

bool Migration::Binder::hintTable(const String& table)
{
    return m_migration.lazyInitInfo(*this, table);
}

const MigrationInfo* Migration::Binder::getBoundInfo(const String& table)
{
    WCTInnerAssert(m_binding == false);
    const MigrationInfo* info = nullptr;
    auto iter = m_bounds.find(table);
    if (iter != m_bounds.end()) {
        info = iter->second.get();
    }
    return info;
}

void Migration::tryReduceBounds(std::map<String, RecyclableMigrationInfo>& bounds)
{
    bool reduce = false;
    {
        SharedLockGuard lockGuard(m_lock);
        for (const auto& iter : bounds) {
            if (m_migratings.find(iter.second.get()) == m_migratings.end()) {
                reduce = true;
                break;
            }
        }
    }
    if (reduce) {
        LockGuard lockGuard(m_lock);
        for (auto iter = bounds.begin(); iter != bounds.end();) {
            const MigrationInfo* info = iter->second.get();
            if (m_migratings.find(info) == m_migratings.end()) {
                iter = bounds.erase(iter);
            } else {
                ++iter;
            }
        }
    }
}

std::pair<bool, RecyclableMigrationInfo>
Migration::getOrInitInfo(InfoInitializer& initializer, const String& table)
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
    bool done = false;
    bool worked = false;
    bool retry = false;
    do {
        retry = false;
        std::tie(succeed, worked) = tryDropUnreferencedTable(stepper);
        if (!succeed || worked) {
            break;
        }
        std::tie(succeed, worked) = tryMigrateRows(stepper);
        if (!succeed || worked) {
            break;
        }
        std::tie(succeed, worked) = tryAcquireTables(stepper);
        if (!succeed) {
            break;
        }
        if (worked) {
            retry = true;
        } else {
            done = true;
            triggerMigratedNotification();
        }
    } while (retry);
    return { succeed, done };
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
    if (info) {
        succeed = stepper.dropSourceTable(info);
        worked = succeed;
        markAsDropped(info);
        triggerTableMigratedNotification(info);
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
    std::set<String> tables;
    std::tie(succeed, tables) = stepper.getAllTables();
    if (succeed) {
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

void Migration::setNotificationWhenMigrated(const MigratedCallback& callback)
{
    LockGuard lockGuard(m_lock);
    m_migratedNotification = callback;
}

void Migration::triggerMigratedNotification()
{
    MigratedCallback callback = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        WCTInnerAssert(m_tableAcquired);
        WCTInnerAssert(m_dumpster.empty());
        WCTInnerAssert(m_migratings.empty());
        WCTInnerAssert(m_referenceds.empty());
        callback = m_migratedNotification;
    }
    if (callback) {
        callback(nullptr);
    }
}

void Migration::triggerTableMigratedNotification(const MigrationInfo* info)
{
    MigratedCallback callback = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        WCTInnerAssert(m_migratings.find(info) == m_migratings.end());
        WCTInnerAssert(m_referenceds.find(info) == m_referenceds.end());
        WCTInnerAssert(m_dumpster.find(info) == m_dumpster.end());
        callback = m_migratedNotification;
    }
    if (callback) {
        callback(info);
    }
}

} // namespace WCDB
