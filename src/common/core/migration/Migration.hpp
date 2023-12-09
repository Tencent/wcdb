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

#pragma once

#include "Lock.hpp"
#include "MigrationInfo.hpp"
#include "Recyclable.hpp"
#include "ThreadLocal.hpp"
#include "WCDBOptional.hpp"
#include <functional>
#include <map>
#include <set>

namespace WCDB {

typedef Recyclable<const MigrationInfo*> RecyclableMigrationInfo;
class InnerHandle;

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

    using TableFilter = MigrationDatabaseInfo::TableFilter;
    // addMigration should be called at the very beginning.
    void addMigration(const UnsafeStringView& sourcePath,
                      const UnsafeData& sourceCipher,
                      const TableFilter& filter);

    bool shouldMigrate() const;

    void purge();

    StringViewSet getPathsOfSourceDatabases() const;

protected:
    class InfoInitializer {
        friend class Migration;

    public:
        virtual ~InfoInitializer() = 0;

    protected:
        virtual bool attachSourceDatabase(const MigrationUserInfo& userInfo) = 0;
        virtual InnerHandle* getCurrentHandle() const = 0;
        virtual const StringView& getDatabasePath() const = 0;

        Optional<bool> checkSourceTableExistsAndHasRowid(const MigrationUserInfo& userInfo);
        bool getTargetTableInfo(const MigrationUserInfo& userInfo,
                                bool& exists,
                                StringViewSet& columns,
                                bool& autoincrement,
                                StringView& integerPrimaryKey);
        Optional<bool> tryUpdateSequence(const MigrationInfo& info);
    };

    bool initInfo(InfoInitializer& initializer, const UnsafeStringView& table);
    bool hintThatTableWillBeCreated(InfoInitializer& initializer,
                                    const UnsafeStringView& table);
    void markAsNoNeedToMigrate(const UnsafeStringView& table);

    void markAsUnreferenced(const MigrationInfo* info);
    void markAsDropped(const MigrationInfo* info);
    void markAsMigrated(const MigrationInfo* info);
    Optional<RecyclableMigrationInfo> getInfo(const UnsafeStringView& table);

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
    StringViewMap<const MigrationInfo*> m_filted;
    StringViewSet m_hints;

    void retainInfo(const MigrationInfo* info);
    void releaseInfo(const MigrationInfo* info);

    mutable SharedLock m_lock;

    StringViewMap<std::shared_ptr<MigrationDatabaseInfo>> m_migrationInfo;

#pragma mark - Update sequence
public:
    bool tryUpdateSequence(InfoInitializer& initializer, const MigrationInfo& info);
    void setTableInfoCommitted(bool committed);

protected:
    ThreadLocal<std::set<const MigrationInfo*>> m_commitingInfos;

#pragma mark - Bind
public:
    class Binder : public InfoInitializer {
        friend class Migration;

    public:
        Binder(Migration& migration);
        virtual ~Binder() override = 0;

        void startBinding();
        bool stopBinding(bool succeed);
        void stopReferenced();
        void setNeedRebind();

        Optional<const MigrationInfo*> bindTable(const UnsafeStringView& table);
        bool hintThatTableWillBeCreated(const UnsafeStringView& table);
        const MigrationInfo* getBoundInfo(const UnsafeStringView& table);

        virtual bool bindInfos(const StringViewMap<const MigrationInfo*>& infos) = 0;

        void setTableInfoCommitted(bool committed);

    private:
        Migration& m_migration;
        bool m_binding;
        StringViewMap<RecyclableMigrationInfo> m_referenceds; // all infos need to be bound during this cycle
        StringViewMap<const MigrationInfo*> m_bindings;
        StringViewMap<const MigrationInfo*> m_bounds;
        bool m_rebind;
    };

protected:
    Optional<RecyclableMigrationInfo>
    getOrInitInfo(InfoInitializer& initializer, const UnsafeStringView& table);
    void tryReduceBounds(StringViewMap<const MigrationInfo*>& bounds);

private:
#pragma mark - Step
public:
    class Stepper : public InfoInitializer {
        friend class Migration;

    public:
        virtual ~Stepper() override = 0;

    protected:
        virtual Optional<StringViewSet> getAllTables() = 0;
        virtual bool dropSourceTable(const MigrationInfo* info) = 0;
        virtual Optional<bool> migrateRows(const MigrationInfo* info) = 0;
    };

    // done
    Optional<bool> step(Migration::Stepper& stepper);

protected:
    // worked
    Optional<bool> tryDropUnreferencedTable(Migration::Stepper& stepper);
    Optional<bool> tryMigrateRows(Migration::Stepper& stepper);
    Optional<bool> tryAcquireTables(Migration::Stepper& stepper);

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
