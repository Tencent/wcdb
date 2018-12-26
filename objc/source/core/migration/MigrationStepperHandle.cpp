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
    finalizeMigrationStatement();
    returnStatement(m_migrateStatement);
    returnStatement(m_removeMigratedStatement);
}

bool MigrationStepperHandle::reAttach(const String& newPath, const Schema& newSchema)
{
    bool succeed = true;
    bool detached = false;
    bool attached = false;
    do {
        if (m_attached.getDescription() != newSchema.getDescription()) {
            if (!m_attached.syntax().isMain()) {
                if (!execute(WCDB::StatementDetach().detach(m_attached))) {
                    succeed = false;
                    break;
                }
                detached = true;
            }
            if (!newSchema.syntax().isMain()) {
                if (!execute(WCDB::StatementAttach().attach(newPath).as(newSchema))) {
                    succeed = false;
                    break;
                }
                attached = true;
            }
        }
    } while (false);
    if (succeed) {
        if (detached) {
            if (attached) {
                m_attached = newSchema;
            } else {
                m_attached = Schema::main();
            }
            if (m_migratingInfo != nullptr
                && m_attached.getDescription()
                   != m_migratingInfo->getSchemaForSourceDatabase().getDescription()) {
                // migrating info out of date
                m_migratingInfo = nullptr;
                finalizeMigrationStatement();
            }
        }
    }
    return succeed;
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
std::pair<bool, std::set<String>> MigrationStepperHandle::getAllTables()
{
    Column name("name");
    Column type("type");
    String pattern = String::formatted("%s%%", Syntax::builtinTablePrefix);
    return getValues(StatementSelect()
                     .select(name)
                     .from(TableOrSubquery::master())
                     .where(type == "table" && name.like(pattern)),
                     1);
}

bool MigrationStepperHandle::dropSourceTable(const MigrationInfo* info)
{
    WCTInnerAssert(info != nullptr);
    return reAttach(info->getSourceDatabase(), info->getSchemaForSourceDatabase())
           && execute(m_migratingInfo->getStatementForDroppingSourceTable());
}

bool MigrationStepperHandle::migrateRows(const MigrationInfo* info, bool& done)
{
    WCTInnerAssert(info != nullptr);
    done = false;

    if (m_migratingInfo != info) {
        if (!reAttach(info->getSourceDatabase(), info->getSchemaForSourceDatabase())) {
            return false;
        }
        m_migratingInfo = info;
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

void MigrationStepperHandle::finalizeMigrationStatement()
{
    m_migrateStatement->finalize();
    m_removeMigratedStatement->finalize();
}

#pragma mark - Info Initializer
std::pair<bool, std::set<String>>
MigrationStepperHandle::getColumnsForSourceTable(const MigrationUserInfo& userInfo)
{
    if (!reAttach(userInfo.getSourceDatabase(), userInfo.getSchemaForSourceDatabase())) {
        return { false, {} };
    }
    return getColumns(userInfo.getSchemaForSourceDatabase(), userInfo.getSourceTable());
}

String MigrationStepperHandle::getDatabasePath() const
{
    return getPath();
}

} // namespace WCDB
