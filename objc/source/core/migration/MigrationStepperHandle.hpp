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

#ifndef _WCDB_MIGRATIONSTEPPERHANDLE_HPP
#define _WCDB_MIGRATIONSTEPPERHANDLE_HPP

#include <WCDB/Configs.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/Migration.hpp>

namespace WCDB {

// Migration step should as least as possible to avoid blocking user operations.
// However, it's very wasteful for those resource(CPU, IO...) when the step is too tiny.
// So stepper will try to migrate one by one until the count of dirty pages(to be written) is changed.
// In addition, stepper can/will be interrupted when database is not idled.
class MigrationStepperHandle final : public ConfigurableHandle, public Migration::Stepper {
public:
    MigrationStepperHandle();
    ~MigrationStepperHandle();

#pragma mark - Schema
protected:
    bool reAttach(const String& newPath, const Schema& newSchema);

private:
    Schema m_attached;

#pragma mark - Interrupt
public:
    void setInterruptible(bool interruptible);
    void interrupt();

protected:
    using Handle::interrupt;
    std::atomic<bool> m_interruptible;

#pragma mark - Stepper
protected:
    std::pair<bool, std::set<String>> getAllTables() override final;
    bool dropSourceTable(const MigrationInfo* info) override final;
    bool migrateRows(const MigrationInfo* info, bool& done) override final;
    std::pair<bool, bool> migrateRow();
    // succeed, worked, migrated
    std::tuple<bool, bool, bool> tryMigrateRowWithoutIncreasingDirtyPage();

    bool reAttachMigrationInfo(const MigrationInfo* info);
    void finalizeMigrationStatement();

private:
    const MigrationInfo* m_migratingInfo;
    HandleStatement* m_migrateStatement;
    HandleStatement* m_removeMigratedStatement;

#pragma mark - Info Initializer
protected:
    std::pair<bool, std::set<String>>
    getColumnsForSourceTable(const MigrationUserInfo& userInfo) override final;
    String getDatabasePath() const override final;
};

} // namespace WCDB

#endif /* _WCDB_MIGRATIONSTEPPERHANDLE_HPP */
