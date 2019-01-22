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

#include <WCDB/Assertion.hpp>
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
    bool schemaChanged = false;
    do {
        if (m_attached.getDescription() != newSchema.getDescription()) {
            if (!m_attached.syntax().isMain()) {
                if (!execute(WCDB::StatementDetach().detach(m_attached))) {
                    succeed = false;
                    break;
                }
                m_attached = Schema::main();
                schemaChanged = true;
            }
            if (!newSchema.syntax().isMain()) {
                if (!execute(WCDB::StatementAttach().attach(newPath).as(newSchema))) {
                    succeed = false;
                    break;
                }
                m_attached = newSchema;
                schemaChanged = true;
            }
        }
    } while (false);
    if (schemaChanged) {
        if (m_migratingInfo != nullptr
            && m_attached.getDescription()
               != m_migratingInfo->getSchemaForSourceDatabase().getDescription()) {
            // migrating info out of date
            m_migratingInfo = nullptr;
            finalizeMigrationStatement();
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
                     .where(type == "table" && name.notLike(pattern)),
                     0);
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
           m_migratingInfo->getStatementForDeletingMigratedOneRow())) {
        return false;
    }

    bool migrated = false;
    bool succeed = runTransaction([&migrated, this](Handle*) -> bool {
        // migrate one at least
        bool succeed;
        std::tie(succeed, migrated) = migrateRow();
        if (!succeed) {
            return false;
        }
        WCTInnerAssert(getNumberOfDirtyPages() != 0);
        if (migrated) {
            return true;
        }

        int numberOfDirtyPages = getNumberOfDirtyPages();
        bool worked = false;
        do {
            std::tie(succeed, worked, migrated)
            = tryMigrateRowWithoutIncreasingDirtyPage(numberOfDirtyPages);
        } while (succeed && worked && !migrated);
#warning TODO - wait for the answer of SQLite staff about the dirty page of ROLLBACK TO stmt.
        //        WCTInnerAssert(numberOfDirtyPages == getNumberOfDirtyPages());
        return succeed;
    });
    if (succeed && migrated) {
        done = true;
    }
    return succeed;
}

std::pair<bool, bool> MigrationStepperHandle::migrateRow()
{
    WCTInnerAssert(m_migrateStatement->isPrepared()
                   && m_removeMigratedStatement->isPrepared());
    WCTInnerAssert(isInTransaction());
    bool succeed = false;
    bool migrated = false;
    m_migrateStatement->reset();
    m_removeMigratedStatement->reset();
    if (m_migrateStatement->step()) {
        if (getChanges() != 0) {
            succeed = m_removeMigratedStatement->step();
        } else {
            succeed = true;
            migrated = true;
        }
    }
    return { succeed, migrated };
}

std::tuple<bool, bool, bool>
MigrationStepperHandle::tryMigrateRowWithoutIncreasingDirtyPage(int numberOfDirtyPages)
{
    WCTInnerAssert(m_migrateStatement->isPrepared()
                   && m_removeMigratedStatement->isPrepared());
    WCTInnerAssert(isInTransaction());
    bool needToWork = true;
    bool migrated = false;
    bool succeed = runNestedTransaction(
    [numberOfDirtyPages, &needToWork, &migrated, this](Handle*) -> bool {
        bool succeed;
        std::tie(succeed, migrated) = migrateRow();
        if (!succeed) {
            return false;
        }
        needToWork = getNumberOfDirtyPages() <= numberOfDirtyPages;
        return needToWork;
    });
    if (!succeed) {
        migrated = false;
        if (!needToWork) {
            // rollback manually
            succeed = true;
        }
    }
    return { succeed, needToWork, migrated };
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
