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

#include <WCDB/MigrationStepperHandle.hpp>

namespace WCDB {

#pragma mark - Interrupt
void MigrationStepperHandle::setInterruptible(bool interruptible)
{
    m_interruptible = interruptible;
}

void MigrationStepperHandle::interrupt()
{
    if (m_interruptible.load()) {
        Handle::interrupt();
    }
}

#pragma mark - Configurable
Handle* MigrationStepperHandle::getConfigurator()
{
    return this;
}

#pragma mark - Stepper
bool MigrationStepperHandle::lazyOpen()
{
    if (isOpened()) {
        return true;
    }
    return open();
}

bool MigrationStepperHandle::switchMigrating(const MigrationInfo* migrating)
{
    String newSchema = migrating->getSchemaForOriginDatabase().getDescription();
    String oldSchema = m_attached.getDescription();
    if (oldSchema == newSchema) {
        return true;
    }
    String main = Schema::main().getDescription();
    if (oldSchema != main) {
        if (!execute(MigrationInfo::getStatementForDetachingSchema(m_attached))) {
            return false;
        }
    }
    if (newSchema != main) {
        if (!execute(migrating->getStatementForAttachingSchema())) {
            return false;
        }
    }
    m_attached = migrating->getSchemaForOriginDatabase();
    return true;
}

bool MigrationStepperHandle::dropOriginTable(const MigrationInfo* info)
{
    if (!lazyOpen() || !switchMigrating(info)) {
        return false;
    }

    return execute(info->getStatementForDroppingOriginTable());
}

bool MigrationStepperHandle::migrateRows(const MigrationInfo* info, bool& done)
{
    done = false;

    if (!lazyOpen() || !switchMigrating(info)) {
        return false;
    }

    bool migrated = false;
    if (!runNestedTransaction([info, &migrated](Handle* handle) -> bool {
            int step = 10;

            if (!handle->prepare(info->getStatementForMigratingRow())) {
                return false;
            }
            handle->bindInteger32(step, 1);
            bool succeed = handle->step();
            handle->finalize();
            if (!succeed) {
                return false;
            }

            if (!handle->prepare(info->getStatementForMigratingRow())) {
                return false;
            }
            handle->bindInteger32(step, 1);
            succeed = handle->step();
            handle->finalize();
            if (!succeed) {
                return false;
            }

            if (handle->getChanges() < step) {
                // nothing more to migrate
                migrated = true;
            }
            return true;
        })) {
        return false;
    }

    if (migrated) {
        done = true;
    }

    // detach should be executed when origin table is dropped.

    return true;
}

} // namespace WCDB
