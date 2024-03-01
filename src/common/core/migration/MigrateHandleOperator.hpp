//
// Created by qiuwenchen on 2023/11/17.
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

#include "HandleOperator.hpp"
#include "Migration.hpp"
#include <array>
#include <set>

namespace WCDB {

// Each step of migration should be as small as possible to avoid blocking user operations.
// However, it's very wasteful for those resources(CPU, IO...) when the step is too small.
// So stepper will try to migrate one by one until the count of dirty pages(to be written) is changed.
// In addition, stepper can/will be interrupted when database is not idled.
class MigrateHandleOperator final : public HandleOperator, public Migration::Stepper {
public:
    MigrateHandleOperator(InnerHandle* handle);
    ~MigrateHandleOperator() override;

    void onDecorationChange() override final;

#pragma mark - Schema
protected:
    bool reAttach(const MigrationBaseInfo* info);
    bool attach(const MigrationBaseInfo* info);
    bool detach();

private:
    Schema m_attached;

#pragma mark - Stepper
protected:
    Optional<StringViewSet> getAllTables() override final;
    bool dropSourceTable(const MigrationInfo* info) override final;
    Optional<bool> migrateRows(const MigrationInfo* info) override final;
    Optional<bool> migrateRow();

    bool reAttachMigrationInfo(const MigrationInfo* info);
    void finalizeMigrationStatement();

private:
    const MigrationInfo* m_migratingInfo;
    HandleStatement* m_migrateStatement;
    HandleStatement* m_removeMigratedStatement;

#pragma mark - Sample
protected:
    void addSample(double timeIntervalWithinTransaction, double timeIntervalForWholeTransaction);
    double calculateTimeIntervalWithinTransaction() const;

private:
    static constexpr const int numberOfSamples = 10;
    struct Sample {
        Sample();
        double timeIntervalWithinTransaction;
        double timeIntervalWholeTransaction;
    };
    typedef struct Sample Sample;
    std::array<Sample, numberOfSamples> m_samples; // FILO
    int m_samplePointing;

#pragma mark - Info Initializer
protected:
    bool attachSourceDatabase(const MigrationUserInfo& userInfo) override final;
    InnerHandle* getCurrentHandle() const override final;
    const StringView& getDatabasePath() const override final;
};

} // namespace WCDB
