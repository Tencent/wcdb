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

#ifndef _WCDB_MIGRATION_HPP
#define _WCDB_MIGRATION_HPP

#include <WCDB/Lock.hpp>
#include <WCDB/MigrationInfo.hpp>
#include <WCDB/Recyclable.hpp>
#include <functional>
#include <map>
#include <set>

namespace WCDB {

class Migration final {
#pragma mark - Initialize
public:
    Migration();

    class Initializer {
        friend class Migration;

    public:
        virtual ~Initializer();

        virtual const Error& getError() const = 0;

    protected:
        virtual const String& getDatabasePath() const = 0;

        virtual std::pair<bool, std::set<String>> getTables() = 0;
        // When succeed, the empty column means that table does not exist.
        virtual std::pair<bool, std::set<String>>
        getColumns(const String& table, const String& database) = 0;

        virtual void setError(const Error& error) = 0;
    };
    // Resolve user infos and get the column names of origin table
    bool initialize(Initializer& initializer);
    bool isInitialized() const;
    bool shouldMigrate() const;

private:
    bool m_initialized;

    // Those infos needed to be migrate will be held by m_migrating after initialized. (Other infos that already migrated or have no need to migrate will be dropped when initializing.)
    // And when all the columns inside original table is migrated, the info will not be dropped immedially but removed from m_migratings. The reason is that there might be some handles still holding it. At the same time, the original table will not be dropped too.
    // Until there is really no one holding them, it will be moved to dumpster and wait to be dropped.
    // During these routine, info will not be removed from holder to avoid memory issue.
    //        ┌──────┐                                ┌────────┐
    // ┌────┐ │Inited│  ┌───────────────────────────┐ │Migrated│ ┌─────────────┐
    // │Info│─┴──────┴─▶│m_migratings, m_referenceds│─┴────────┴▶│m_referenceds│─▶
    // └────┘           └───────────────────────────┘            └─────────────┘
    //    ┌──────────────────┐              ┌───────┐
    //    │Until Unreferenced│ ┌──────────┐ │Dropped│ ┌───────┐
    //  ──┴──────────────────┴▶│m_dumpster│─┴───────┴▶│Removed│
    //                         └──────────┘           └───────┘
    std::set<const MigrationInfo*> m_migratings;
    std::set<const MigrationInfo*> m_dumpster;

    std::map<const MigrationInfo*, std::atomic<int>> m_referenceds;
    std::list<MigrationInfo> m_holder;

    mutable SharedLock m_lock;

#pragma mark - Filter
public:
    typedef std::function<void(MigrationUserInfo&)> Filter;
    void filterTable(const Filter& filter);

protected:
    Filter m_filter;

#pragma mark - Bind
public:
    class Binder {
    public:
        Binder(Migration& migration);
        virtual ~Binder();
        bool rebindMigration();

    protected:
        const std::set<const MigrationInfo*>& getBounds() const;
        virtual bool rebind(const std::set<const MigrationInfo*>& toRebinds) = 0;

    private:
        Migration& m_migration;
        std::set<const MigrationInfo*> m_bounds;
    };

    std::set<const MigrationInfo*> getMigratingInfos() const;
    bool shouldRebind(const std::set<const MigrationInfo*>& bounds) const;

protected:
    void markAsRebound(const std::set<const MigrationInfo*>& oldBounds,
                       const std::set<const MigrationInfo*>& newBounds);

#pragma mark - Step
public:
    class Stepper {
        friend class Migration;

    public:
        virtual ~Stepper();

    protected:
        virtual bool dropOriginTable(const MigrationInfo* info) = 0;
        virtual bool migrateRows(const MigrationInfo* info, bool& done) = 0;
    };

    // succeed, done
    std::pair<bool, bool> step(Migration::Stepper& stepper);

    // parameter will be nullptr when all tables migrated.
    typedef std::function<void(const MigrationBaseInfo*)> MigratedCallback;
    void setNotificationWhenMigrated(const MigratedCallback& callback);

protected:
    // succeed, worked, done
    std::tuple<bool, bool, bool> dropOriginTable(Migration::Stepper& stepper);

    bool migrateRows(Migration::Stepper& stepper);

    MigratedCallback m_migratedNotification;
};

} // namespace WCDB

#endif /* _WCDB_MIGRATION_HPP */
