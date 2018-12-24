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

MigrationStepperHandle::MigrationStepperHandle()
: m_migratingInfo(nullptr)
, m_migrateStatement(getStatement())
, m_removeMigratedStatement(getStatement())
{
}

MigrationStepperHandle::~MigrationStepperHandle()
{
    m_migrateStatement->finalize();
    returnStatement(m_migrateStatement);
    m_removeMigratedStatement->finalize();
    returnStatement(m_removeMigratedStatement);
}

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

#pragma mark - Stepper
bool MigrationStepperHandle::switchMigrating(const MigrationInfo* newInfo)
{
    WCTInnerAssert(newInfo != nullptr);
    if (newInfo == m_migratingInfo) {
        return true;
    }
    Schema oldSchema;
    if (m_migratingInfo) {
        oldSchema = m_migratingInfo->getSchemaForOriginDatabase();
    }
    Schema newSchema = newInfo->getSchemaForOriginDatabase();
    if (oldSchema.getDescription() != newSchema.getDescription()) {
        if (!oldSchema.syntax().isMain()) {
            if (!execute(MigrationInfo::getStatementForDetachingSchema(oldSchema))) {
                return false;
            }
        }
        if (!newSchema.syntax().isMain()) {
            if (!execute(newInfo->getStatementForAttachingSchema())) {
                return false;
            }
        }
    }
    m_migratingInfo = newInfo;
    m_migrateStatement->finalize();
    m_removeMigratedStatement->finalize();
    return true;
}

bool MigrationStepperHandle::dropOriginTable(const MigrationInfo* info)
{
    if (!switchMigrating(info)) {
        return false;
    }

    return execute(m_migratingInfo->getStatementForDroppingOriginTable());
}

bool MigrationStepperHandle::migrateRows(const MigrationInfo* info, bool& done)
{
    done = false;

    if (!switchMigrating(info)) {
        return false;
    }

    if (!m_migrateStatement->isPrepared()
        && !m_migrateStatement->prepare(m_migratingInfo->getStatementForMigratingOneRow())) {
        return false;
    }

    if (!m_removeMigratedStatement->isPrepared()
        && !m_removeMigratedStatement->prepare(
           m_migratingInfo->getStatementForDeletingSpecifiedRow())) {
        return false;
    }

    bool migrated = false;
    if (!runNestedTransaction([&migrated, this](Handle*) -> bool {
            // migrate one at least
            if (!migrateRow(migrated)) {
                return false;
            }

            const int dirtyPageCount = getDirtyPageCount();
            WCTInnerAssert(dirtyPageCount != 0);
            bool step = true;
            bool succeed = true;
            while (succeed && step && !migrated) {
                // try migrate one, or rollback to savepoint if the count of dirty pages is increased.
                succeed = runNestedTransaction(
                [&migrated, &step, dirtyPageCount, this](Handle*) -> bool {
                    if (!migrateRow(migrated)) {
                        return false;
                    }
                    step = getDirtyPageCount() > dirtyPageCount && !migrated;
                    return step;
                });
                if (!succeed) {
                    migrated = false;
                    if (!step) {
                        // failed since manual rollback
                        succeed = true;
                    }
                }
            }
            return succeed;
        })) {
        return false;
    }

    if (migrated) {
        done = true;
    }

    // detach should be executed when origin table is dropped.

    return true;
}

bool MigrationStepperHandle::migrateRow(bool& migrated)
{
    WCTInnerAssert(m_migrateStatement->isPrepared()
                   && m_removeMigratedStatement->isPrepared());
    WCTInnerAssert(isInTransaction());
    m_removeMigratedStatement->reset();
    m_removeMigratedStatement->bindInteger64(getLastInsertedRowID(), 1);
    bool succeed = m_migrateStatement->step() && m_removeMigratedStatement->step();
    migrated = succeed ? getChanges() == 0 : false;
    return succeed;
}

} // namespace WCDB