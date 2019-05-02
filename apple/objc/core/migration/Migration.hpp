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

#ifndef __WCDB_MIGRATION_HPP
#define __WCDB_MIGRATION_HPP

#include <WCDB/Lock.hpp>
#include <WCDB/MigrationInfo.hpp>
#include <WCDB/Recyclable.hpp>
#include <functional>
#include <map>
#include <set>

namespace WCDB {

typedef Recyclable<const MigrationInfo*> RecyclableMigrationInfo;

class MigrationEvent {
public:
    virtual ~MigrationEvent() = 0;

protected:
    friend class Migration;
    // parameter will be nullptr when all tables migrated.
    virtual void didMigrate(const MigrationBaseInfo* info) = 0;
};

// TODO: deny if user visiting source table by using authorize feature.
class Migration final {
#pragma mark - Initialize
public:
    Migration(MigrationEvent* event);

    typedef std::function<void(MigrationUserInfo&)> Filter;
    // filter should be called at the very beginning.
    void filterTable(const Filter& filter);

    bool shouldMigrate() const;

    void purge();

protected:
    class InfoInitializer {
        friend class Migration;

    public:
        virtual ~InfoInitializer() = 0;

    protected:
        // When succeed, the empty column means that table does not exist.
        // succeed, contains integer primary key, columns
        virtual std::tuple<bool, bool, std::set<String>>
        getColumnsForSourceTable(const MigrationUserInfo& userInfo) = 0;
        virtual String getDatabasePath() const = 0;
    };

    bool initInfo(InfoInitializer& initializer, const String& table);
    bool lazyInitInfo(InfoInitializer& initializer, const String& table);
    void markAsUnreferenced(const MigrationInfo* info);
    void markAsDropped(const MigrationInfo* info);
    void markAsMigrated(const MigrationInfo* info);
    std::pair<bool, RecyclableMigrationInfo> getInfo(const String& table);

private:
    // Those infos needed to be migrate will be held by m_migrating after initialized. (Other infos that already migrated or have no need to migrate will be dropped when initializing.)
    // And when all the columns inside source table is migrated, the info will not be dropped immedially but removed from m_migratings. The reason is that there might be some handles still holding it. At the same time, the source table will not be dropped too.
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

    void retainInfo(const MigrationInfo* info);
    void releaseInfo(const MigrationInfo* info);

    mutable SharedLock m_lock;

    Filter m_filter;

#pragma mark - Bind
public:
    class Binder : public InfoInitializer {
        friend class Migration;

    public:
        Binder(Migration& migration);
        virtual ~Binder() = 0;

    protected:
        void startBinding();
        bool stopBinding(bool succeed);

        std::pair<bool, const MigrationInfo*> bindTable(const String& table);
        bool hintTable(const String& table);
        const MigrationInfo* getBoundInfo(const String& table);

        virtual bool bindInfos(const std::map<String, RecyclableMigrationInfo>& infos) = 0;

    private:
        bool m_binding;
        Migration& m_migration;
        std::map<String, RecyclableMigrationInfo> m_cache; // all infos need to be bound during this cycle
        std::map<String, RecyclableMigrationInfo> m_boundsCache;
        std::map<String, RecyclableMigrationInfo> m_bounds;
    };

protected:
    std::pair<bool, RecyclableMigrationInfo>
    getOrInitInfo(InfoInitializer& initializer, const String& table);
    void tryReduceBounds(std::map<String, RecyclableMigrationInfo>& bounds);

private:
#pragma mark - Step
public:
    class Stepper : public InfoInitializer {
        friend class Migration;

    public:
        virtual ~Stepper() = 0;

    protected:
        virtual std::pair<bool, std::set<String>> getAllTables() = 0;
        virtual bool dropSourceTable(const MigrationInfo* info) = 0;
        virtual bool migrateRows(const MigrationInfo* info, bool& done) = 0;
    };

    // succeed, done
    std::pair<bool, bool> step(Migration::Stepper& stepper);

protected:
    // succeed, worked
    std::pair<bool, bool> tryDropUnreferencedTable(Migration::Stepper& stepper);
    std::pair<bool, bool> tryMigrateRows(Migration::Stepper& stepper);
    std::pair<bool, bool> tryAcquireTables(Migration::Stepper& stepper);

    void postMigratedNotification();
    void postTableMigratedNotification(const MigrationInfo* info);

private:
    bool m_tableAcquired;
    bool m_migrated;

#pragma mark - Event
public:
    bool isMigrated() const;

protected:
    MigrationEvent* m_event;
};

} // namespace WCDB

#endif /* __WCDB_MIGRATION_HPP */
