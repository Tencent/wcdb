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
}

bool Migration::Binder::rebind()
{
    m_migration.tryReduceBounds(m_bounds);
    for (const auto& iter : m_prepareds) {
        if (iter.second != nullptr) {
            m_bounds.emplace(iter.first, iter.second);
        }
    }
    m_prepareds.clear();
    if (m_bounds != m_applys) {
        if (!rebind(m_bounds)) {
            return false;
        }
        m_applys = m_bounds;
    }
    return true;
}

std::pair<bool, const MigrationInfo*> Migration::Binder::prepareInfo(const String& table)
{
    bool succeed = true;
    RecyclableMigrationInfo info = nullptr;
    if (!table.empty()) {
        std::tie(succeed, info) = m_migration.getOrBindInfo(*this, table);
        if (succeed) {
            m_prepareds.emplace(table, info);
        }
    }
    return { succeed, info.get() };
}

const MigrationInfo* Migration::Binder::getBoundInfo(const String& table)
{
    const MigrationInfo* info = nullptr;
    auto iter = m_applys.find(table);
    if (iter != m_applys.end()) {
        info = iter->second.get();
    }
    return info;
}

void Migration::Binder::clearPrepared()
{
    m_prepareds.clear();
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
Migration::getOrBindInfo(Binder& binder, const String& table)
{
    bool get;
    RecyclableMigrationInfo info = nullptr;
    {
        LockGuard lockGuard(m_lock);
        std::tie(get, info) = getBoundInfo(table);
        if (get) {
            return { true, info };
        }
    }
    MigrationUserInfo userInfo(binder.getMigratedDatabasePath(), table);
    m_filter(userInfo);
    do {
        if (userInfo.shouldMigrate()) {
            bool succeed;
            std::set<String> columns;
            std::tie(succeed, columns) = binder.getColumnsForSourceTable(userInfo);
            if (!succeed) {
                break;
            }
            LockGuard lockGuard(m_lock);
            // retry due to the lockfree code above
            std::tie(get, info) = getBoundInfo(table);
            if (get) {
                break;
            }

            if (!columns.empty()) {
                m_holder.push_back(MigrationInfo(userInfo, columns));
                const MigrationInfo* hold = &m_holder.back();
                m_migratings.emplace(hold);
                m_referenceds.emplace(hold, 0);
                m_filted.emplace(table, hold);
            } else {
                m_filted.emplace(table, nullptr);
            }

            std::tie(get, info) = getBoundInfo(table);
            WCTInnerAssert(get);
        } else {
            LockGuard lockGuard(m_lock);
            // retry due to the lockfree code above
            std::tie(get, info) = getBoundInfo(table);
            if (get) {
                break;
            }

            m_filted.emplace(table, nullptr);
            std::tie(get, info) = getBoundInfo(table);
            WCTInnerAssert(get);
            WCTInnerAssert(info == nullptr);
        }
    } while (false);
    return { get, info };
}

std::pair<bool, RecyclableMigrationInfo> Migration::getBoundInfo(const String& table)
{
    WCTInnerAssert(m_lock.writeSafety());
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

void Migration::retainInfo(const MigrationInfo* info)
{
    WCTInnerAssert(info != nullptr);
    LockGuard lockGuard(m_lock);
    ++m_referenceds.find(info)->second;
    WCTInnerAssert(m_referenceds.find(info) != m_referenceds.end());
}

void Migration::releaseInfo(const MigrationInfo* info)
{
    WCTInnerAssert(info != nullptr);
    LockGuard lockGuard(m_lock);
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
    std::tie(succeed, worked, done) = tryDropUnreferencedTable(stepper);
    if (!succeed) {
        return { false, false };
    }
    if (done) {
        return { true, true };
    }
    if (worked) {
        return { true, false };
    }
    return { tryMigrateRows(stepper), false };
}

std::tuple<bool, bool, bool> Migration::tryDropUnreferencedTable(Migration::Stepper& stepper)
{
    bool succeed = true;
    bool dropped = false;
    bool done = false;
    const MigrationInfo* info = nullptr;
    MigratedCallback migratedNotification;
    {
        SharedLockGuard lockGuard(m_lock);
        migratedNotification = m_migratedNotification;
        if (m_dumpster.empty()) {
            dropped = false;
            done = m_referenceds.empty();
        } else {
            info = *m_dumpster.begin();
        }
    }
    if (info) {
        succeed = stepper.dropSourceTable(info);
        dropped = succeed;
        LockGuard lockGuard(m_lock);
        m_dumpster.erase(info);
        migratedNotification(info);
        done = m_dumpster.empty() && m_referenceds.empty();
    }
    if (done && migratedNotification != nullptr) {
        migratedNotification(nullptr);
    }
    return { succeed, dropped, done };
}

bool Migration::tryMigrateRows(Migration::Stepper& stepper)
{
    const MigrationInfo* info = nullptr;
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_migratings.empty()) {
            return true;
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
