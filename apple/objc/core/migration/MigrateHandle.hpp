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

#include <WCDB/Handle.hpp>
#include <WCDB/Migration.hpp>

namespace WCDB {

// Each step of migration should be as small as possible to avoid blocking user operations.
// However, it's very wasteful for those resources(CPU, IO...) when the step is too small.
// So stepper will try to migrate one by one until the count of dirty pages(to be written) is changed.
// In addition, stepper can/will be interrupted when database is not idled.
class MigrateHandle final : public Handle, public Migration::Stepper {
public:
    MigrateHandle();
    ~MigrateHandle();

#pragma mark - Schema
protected:
    bool reAttach(const String& newPath, const Schema& newSchema);

private:
    Schema m_attached;

#pragma mark - Stepper
protected:
    std::pair<bool, std::set<String>> getAllTables() override final;
    bool dropSourceTable(const MigrationInfo* info) override final;
    bool migrateRows(const MigrationInfo* info, bool& done) override final;
    std::pair<bool, bool> migrateRow();

    bool reAttachMigrationInfo(const MigrationInfo* info);
    void finalizeMigrationStatement();

private:
    const MigrationInfo* m_migratingInfo;
    HandleStatement* m_migrateStatement;
    HandleStatement* m_removeMigratedStatement;

#pragma mark - Info Initializer
protected:
    std::pair<bool, bool> sourceTableExists(const MigrationUserInfo& userInfo) override final;
    std::tuple<bool, bool, std::set<String>>
    getColumnsOfUserInfo(const MigrationUserInfo& userInfo) override final;
    String getDatabasePath() const override final;
};

} // namespace WCDB
