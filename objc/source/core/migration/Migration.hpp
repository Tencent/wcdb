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

typedef Recyclable<const MigrationInfo*> RecyclableMigrationInfo;

class Migration final {
#pragma mark - Initialize
public:
    Migration();

    typedef std::function<void(MigrationUserInfo&)> Filter;
    // filter should be called at the very beginning.
    void filterTable(const Filter& filter);

    bool shouldMigrate() const;

private:
    // Those infos needed to be migrate will be held by m_migrating after initialized. (Other infos that already migrated or have no need to migrate will be dropped when initializing.)
    // And when all the columns inside original table is migrated, the info will not be dropped immedially but removed from m_migratings. The reason is that there might be some handles still holding it. At the same time, the original table will not be dropped too.
    // Until there is really no one holding them, it will be moved to dumpster and wait to be dropped.
    // During these routine, info will not be erased from holder to avoid memory issue.
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

    std::map<const MigrationInfo*, int> m_referenceds;
    std::list<MigrationInfo> m_holder;
    std::map<String, const MigrationInfo*> m_filted;

    mutable SharedLock m_lock;

    Filter m_filter;

#pragma mark - Bind
public:
    class Binder {
        friend class Migration;

    public:
        Binder(Migration& migration);
        virtual ~Binder();

    protected:
        bool rebind();
        std::pair<bool, const MigrationInfo*> prepareInfo(const String& table);
        const MigrationInfo* getBoundInfo(const String& table);
        void clearPrepared();

        virtual bool rebind(const std::map<String, RecyclableMigrationInfo>& toRebinds) = 0;
        // When succeed, the empty column means that table does not exist.
        virtual std::pair<bool, std::set<String>>
        getOriginColumns(const MigrationUserInfo& userInfo) = 0;
        virtual String getMigratedDatabasePath() const = 0;

    private:
        Migration& m_migration;
        std::map<String, RecyclableMigrationInfo> m_prepareds; // all infos need to be bound during this rebind cycle
        std::map<String, RecyclableMigrationInfo> m_bounds;
        std::map<String, RecyclableMigrationInfo> m_applys;
    };

protected:
    std::pair<bool, RecyclableMigrationInfo>
    getOrBindInfo(Binder& binder, const String& table);
    // {get, info}
    void tryReduceBounds(std::map<String, RecyclableMigrationInfo>& bounds);

private:
    std::pair<bool, RecyclableMigrationInfo> getBoundInfo(const String& table);
    void retainInfo(const MigrationInfo* info);
    void releaseInfo(const MigrationInfo* info);

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
    // succeed, dropped, done
    std::tuple<bool, bool, bool> tryDropUnreferencedTable(Migration::Stepper& stepper);

    bool tryMigrateRows(Migration::Stepper& stepper);

    MigratedCallback m_migratedNotification;
};

} // namespace WCDB

#endif /* _WCDB_MIGRATION_HPP */
