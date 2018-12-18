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
Migration::Migration() : m_filter(nullptr), m_migratedNotification(nullptr)
{
}

void Migration::filterTable(const Filter& filter)
{
    LockGuard lockGuard(m_lock);
    m_filter = filter;
    m_filted.clear();
    m_holder.clear();
    m_migratings.clear();
    m_dumpster.clear();
    m_referenceds.clear();
}

bool Migration::shouldMigrate() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_filter != nullptr;
}

#pragma mark - Bind
Migration::Binder::Binder(Migration& migration) : m_migration(migration)
{
}

Migration::Binder::~Binder()
{
    m_migration.unbind(m_bounds);
}

bool Migration::Binder::rebind()
{
    m_migration.tryReduceBounds(m_bounds);
    m_bounds.insert(m_cycledBounds.begin(), m_cycledBounds.end());
    m_cycledBounds.clear();
    if (m_bounds == m_applys) {
        return true;
    }
    if (!rebind(m_bounds)) {
        return false;
    }
    m_applys = m_bounds;
    return true;
}

std::pair<bool, const MigrationInfo*>
Migration::Binder::getOrInitBoundInfo(const String& table)
{
    auto result = m_migration.getOrInitBoundInfo(*this, table);
    if (result.first && result.second != nullptr) {
        m_cycledBounds.emplace(result.second);
    }
    return result;
}

void Migration::Binder::clearCycledBounds()
{
    m_cycledBounds.clear();
}

void Migration::tryReduceBounds(std::set<const MigrationInfo*>& bounds)
{
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_migratings == bounds) {
            return;
        }
    }
    std::set<const MigrationInfo*> toUnbinds;
    {
        LockGuard lockGuard(m_lock);
        for (const auto& bound : bounds) {
            if (m_migratings.find(bound) == m_migratings.end()) {
                toUnbinds.emplace(bound);
                releaseInfo(bound);
            }
        }
    }
    bounds.erase(toUnbinds.begin(), toUnbinds.end());
}

void Migration::unbind(const std::set<const MigrationInfo*>& bounds)
{
    for (const auto& bound : bounds) {
        auto iter = m_referenceds.find(bound);
        WCTInnerAssert(iter != m_referenceds.end());
        WCTInnerAssert(iter->second > 0);
        if (--iter->second == 0) {
            // unreference
            m_dumpster.emplace(bound);
            m_referenceds.erase(bound);
        }
    }
}

std::pair<bool, const MigrationInfo*>
Migration::getOrInitBoundInfo(Binder& binder, const String& table)
{
    bool get;
    const MigrationInfo* info = nullptr;
    {
        LockGuard lockGuard(m_lock);
        std::tie(get, info) = getBoundInfoForTable(table);
        if (get) {
            return { true, info };
        }
    }
    MigrationUserInfo userInfo(table);
    m_filter(userInfo);
    if (userInfo.shouldMigrate()) {
        bool succeed;
        std::set<String> columns;
        std::tie(succeed, columns)
        = binder.getColumns(userInfo.getOriginTable(), userInfo.getOriginDatabase());
        if (!succeed) {
            return { false, nullptr };
        }
        LockGuard lockGuard(m_lock);
        // retry due to the lockfree code above
        std::tie(get, info) = getBoundInfoForTable(table);
        if (get) {
            return { true, info };
        }

        if (!columns.empty()) {
            m_holder.push_back(MigrationInfo(userInfo, columns));
            info = &m_holder.back();
            m_migratings.emplace(info);
            m_referenceds.emplace(info, 0);
            retainInfo(info);
        }
        m_filted.emplace(table, info);
        return { true, info };
    } else {
        LockGuard lockGuard(m_lock);
        // retry due to the lockfree code above
        std::tie(get, info) = getBoundInfoForTable(table);
        if (get) {
            return { true, info };
        }

        m_filted.emplace(table, nullptr);
        return { true, nullptr };
    }
}

std::pair<bool, const MigrationInfo*> Migration::getBoundInfoForTable(const String& table)
{
    WCTInnerAssert(m_lock.writeSafety());
    auto iter = m_filted.find(table);
    if (iter != m_filted.end()) {
        const MigrationInfo* info = iter->second;
        if (m_migratings.find(info) == m_migratings.end()) {
            info = nullptr;
        } else {
            retainInfo(info);
        }
        return { true, info };
    }
    return { false, nullptr };
}

void Migration::retainInfo(const MigrationInfo* info)
{
    WCTInnerAssert(info != nullptr);
    WCTInnerAssert(m_lock.writeSafety());
    ++m_referenceds.find(info)->second;
    WCTInnerAssert(m_referenceds.find(info) != m_referenceds.end());
}

void Migration::releaseInfo(const MigrationInfo* info)
{
    WCTInnerAssert(info != nullptr);
    WCTInnerAssert(m_lock.writeSafety());
    auto iter = m_referenceds.find(info);
    WCTInnerAssert(iter != m_referenceds.end());
    WCTInnerAssert(iter->second > 0);
    if (--iter->second == 0) {
        // unreference
        m_dumpster.emplace(info);
        m_referenceds.erase(info);
    }
}

#pragma mark - Step
Migration::Stepper::~Stepper()
{
}

std::pair<bool, bool> Migration::step(Migration::Stepper& stepper)
{
    bool succeed, worked, done;
    std::tie(succeed, worked, done) = dropOriginTable(stepper);
    if (!succeed) {
        return { false, false };
    }
    if (done) {
        return { true, true };
    }
    if (worked) {
        return { true, false };
    }
    return { migrateRows(stepper), false };
}

std::tuple<bool, bool, bool> Migration::dropOriginTable(Migration::Stepper& stepper)
{
    const MigrationInfo* info = nullptr;
    MigratedCallback migratedNotification = nullptr;
    bool lastOne = false;
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_dumpster.empty()) {
            if (m_referenceds.empty()) {
                // done
                return { true, false, true };
            } else {
                return { true, false, false };
            }
        }
        info = *m_dumpster.begin();
        migratedNotification = m_migratedNotification;
        // last one
        lastOne = m_dumpster.size() == 1 && m_referenceds.empty();
    }
    WCTInnerAssert(info != nullptr);
    if (!stepper.dropOriginTable(info)) {
        return { false, false, false };
    }
    {
        LockGuard lockGuard(m_lock);
        m_dumpster.erase(info);
    }
    if (migratedNotification) {
        migratedNotification(info);
        if (lastOne) {
            migratedNotification(nullptr);
        }
    }
    return { true, true, lastOne };
}

bool Migration::migrateRows(Migration::Stepper& stepper)
{
    const MigrationInfo* info = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_migratings.empty()) {
            return true;
        }
        for (const auto& migrating : m_migratings) {
            if (!migrating->isSameDatabaseMigration()) {
                // cross database migration first to avoid additional database lock.
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
        return false;
    }
    if (done) {
        LockGuard lockGuard(m_lock);
        m_migratings.erase(info);
    }
    return true;
}

void Migration::setNotificationWhenMigrated(const MigratedCallback& callback)
{
    LockGuard lockGuard(m_lock);
    m_migratedNotification = callback;
}

} // namespace WCDB
