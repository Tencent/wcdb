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

class MigrationStepperHandle final : public ConfigurableHandle, public Migration::Stepper {
#pragma mark - Interrupt
public:
    void setInterruptible(bool interruptible);
    void interrupt();

protected:
    using Handle::interrupt;
    std::atomic<bool> m_interruptible;

#pragma mark - Stepper
protected:
    bool dropOriginTable(const MigrationInfo* info) override final;
    bool migrateRows(const MigrationInfo* info, bool& done) override final;

private:
    bool lazyOpen();
    bool switchMigrating(const MigrationInfo* newInfo);
    Schema m_attached;
};

} // namespace WCDB

#endif /* _WCDB_MIGRATIONSTEPPERHANDLE_HPP */
