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

#include <WCDB/KeyValueTable.hpp>
#include <WCDB/MigrationHandle.hpp>

namespace WCDB {

#pragma mark - Initialize
std::shared_ptr<Handle>
MigrationHandle::handleWithPath(const std::string &path,
                                Tag tag,
                                const std::shared_ptr<MigrationInfos> &infos)
{
    return std::shared_ptr<Handle>(new MigrationHandle(path, tag, infos));
}

MigrationHandle::MigrationHandle(const std::string &path,
                                 Tag tag,
                                 const std::shared_ptr<MigrationInfos> &infos)
    : Handle(path, tag), m_infos(infos)
{
}

#pragma mark - Override
bool MigrationHandle::execute(const Statement &statement)
{
    if (m_infos) {
        //Avoid migration info and source table changed
        SharedLockGuard lockGuard(m_infos->getSharedLock());
        Statement tamperedStatement(statement);
        if (m_infos->tamper(tamperedStatement)) {
#ifdef DEBUG
            debugCheckStatementLegal(statement);
#endif
            // Since UPDATE, DELETE, DROPTABLE statements will execute on both migration table and migrated table, multiple statements should be run.
            // INSERT statement will execute on both migration table and migrated table if and only if it's the migrating table. Or it will be executed on origin table only.
            switch (statement.getStatementType()) {
                case Statement::Type::Update:
                case Statement::Type::Delete:
                case Statement::Type::DropTable:
                    return executeWithMultipleStatements(statement,
                                                         tamperedStatement);
                case Statement::Type::Insert: {
                    StatementInsert statementInsert = statement.getCOWLang();
                    if (m_infos->getMigratingInfo() &&
                        m_infos->getMigratingInfo()->targetTable ==
                            statementInsert.getCOWLang()
                                .get<Lang::InsertSTMT>()
                                .tableName.get()) {
                        return executeWithMultipleStatements(statement,
                                                             tamperedStatement);
                    }
                    return executeWithoutTampering(tamperedStatement);
                }
                default:
                    return executeWithoutTampering(tamperedStatement);
            }
        }
    }
    return executeWithoutTampering(statement);
}

bool MigrationHandle::prepare(const Statement &statement)
{
    if (m_infos) {
        m_infos->getSharedLock().lockShared();
        Statement tamperedStatement(statement);
        if (m_infos->tamper(tamperedStatement)) {
#ifdef DEBUG
            debugCheckStatementLegal(statement);
#endif
            // Since UPDATE, DELETE, DROPTABLE statements will execute on both migration table and migrated table, multiple statements should be run.
            // INSERT statement will execute on both migration table and migrated table if and only if it's the migrating table. Or it will be executed on origin table only.
            switch (statement.getStatementType()) {
                case Statement::Type::Update:
                case Statement::Type::Delete:
                case Statement::Type::DropTable:
                    return prepareWithMultipleStatements(statement,
                                                         tamperedStatement);
                case Statement::Type::Insert: {
                    StatementInsert statementInsert = statement.getCOWLang();
                    if (m_infos->getMigratingInfo() &&
                        m_infos->getMigratingInfo()->targetTable ==
                            statementInsert.getCOWLang()
                                .get<Lang::InsertSTMT>()
                                .tableName.get()) {
                        return prepareWithMultipleStatements(statement,
                                                             tamperedStatement);
                    }
                    return prepareWithoutTampering(tamperedStatement);
                }
                default:
                    return prepareWithoutTampering(tamperedStatement);
            }
        }
        m_infos->getSharedLock().unlockShared();
    }
    return prepareWithoutTampering(statement);
}

bool MigrationHandle::step(bool &done)
{
    if (!m_tamperedHandleStatement.isPrepared()) {
        return Handle::step(done);
    } else {
        if (!beginNestedTransaction()) {
            return false;
        }
        assert(m_tamperedHandleStatement.getStatement().getStatementType() ==
                   Statement::Type::Update ||
               m_tamperedHandleStatement.getStatement().getStatementType() ==
                   Statement::Type::Delete ||
               m_tamperedHandleStatement.getStatement().getStatementType() ==
                   Statement::Type::Insert);
        if (Handle::step(done) &&
            Handle::step(m_tamperedHandleStatement, done)) {
            return commitOrRollbackNestedTransaction();
        }
        rollbackNestedTransaction();
        return false;
    }
}

void MigrationHandle::reset()
{
    Handle::reset();
    m_tamperedHandleStatement.reset();
}

void MigrationHandle::bindInteger32(const Integer32 &value, int index)
{
    Handle::bindInteger32(value, index);
    if (m_tamperedHandleStatement.isPrepared()) {
        m_tamperedHandleStatement.bindInteger32(value, index);
    }
}

void MigrationHandle::bindInteger64(const Integer64 &value, int index)
{
    Handle::bindInteger64(value, index);
    if (m_tamperedHandleStatement.isPrepared()) {
        m_tamperedHandleStatement.bindInteger64(value, index);
    }
}

void MigrationHandle::bindDouble(const Float &value, int index)
{
    Handle::bindDouble(value, index);
    if (m_tamperedHandleStatement.isPrepared()) {
        m_tamperedHandleStatement.bindDouble(value, index);
    }
}

void MigrationHandle::bindText(const Text &value, int length, int index)
{
    Handle::bindText(value, length, index);
    if (m_tamperedHandleStatement.isPrepared()) {
        m_tamperedHandleStatement.bindText(value, length, index);
    }
}

void MigrationHandle::bindBLOB(const BLOB &value, int index)
{
    Handle::bindBLOB(value, index);
    if (m_tamperedHandleStatement.isPrepared()) {
        m_tamperedHandleStatement.bindBLOB(value, index);
    }
}

void MigrationHandle::bindNull(int index)
{
    Handle::bindNull(index);
    if (m_tamperedHandleStatement.isPrepared()) {
        m_tamperedHandleStatement.bindNull(index);
    }
}

void MigrationHandle::finalize()
{
    Handle::finalize();
    m_tamperedHandleStatement.finalize();
    m_infos->getSharedLock().unlockShared();
}

#pragma mark - Migration
bool MigrationHandle::lazySetupVeryFirstMigratingInfo()
{
    if (m_infos->didMigratingStart()) {
        return true;
    }
    LockGuard lockGuard(m_infos->getSharedLock());
    if (m_infos->didMigratingStart()) {
        return true;
    }
    std::pair<bool, std::string> migratingTable = {false, {}};
    runNestedTransaction([&migratingTable, this](Handle *handle) -> bool {
        do {
            MigrationHandle *migrationHandle =
                static_cast<MigrationHandle *>(handle);
            KeyValueTable kvTable(migrationHandle);
            auto exists = kvTable.isTableExists();
            if (!exists.first) {
                break;
            }
            if (!exists.second) {
                migratingTable.first = true;
                break;
            }
            migratingTable =
                kvTable.getTextValue(KeyValueTable::Key::Migrating);
        } while (false);
        if (migratingTable.first) {
            if (!migratingTable.second.empty()) {
                m_infos->markAsMigrating(migratingTable.second);
            } else {
                m_infos->markAsMigrationStarted();
            }
        }
        return false;
    });
    return migratingTable.first;
}

bool MigrationHandle::executeWithMultipleStatements(
    const Statement &statement, const Statement &tamperedStatement)
{
    if (!beginNestedTransaction()) {
        return false;
    }
    if (executeWithoutTampering(statement) &&
        executeWithoutTampering(tamperedStatement)) {
        return commitOrRollbackNestedTransaction();
    }
    rollbackNestedTransaction();
    return false;
}

bool MigrationHandle::prepareWithMultipleStatements(
    const Statement &statement, const Statement &tamperedStatement)
{
    return prepareWithoutTampering(statement) &&
           Handle::prepare(tamperedStatement, m_tamperedHandleStatement);
}

#pragma mark - Migration
bool MigrationHandle::executeWithoutTampering(const Statement &statement)
{
    return Handle::execute(statement);
}

bool MigrationHandle::prepareWithoutTampering(const Statement &statement)
{
    return Handle::prepare(statement);
}

#ifdef DEBUG
void MigrationHandle::debugCheckStatementLegal(const Statement &statement)
{
    switch (statement.getStatementType()) {
        case Statement::Type::Update: {
            StatementUpdate statementUpdate(statement.getCOWLang());
            assert(!statementUpdate.isLimited());
        } break;
        case Statement::Type::Delete: {
            StatementDelete statementDelete(statement.getCOWLang());
            assert(!statementDelete.isLimited());
        } break;
        case Statement::Type::Insert: {
            StatementInsert statementInsert(statement.getCOWLang());
            auto iter = m_infos->getInfos().find(statementInsert.getCOWLang()
                                                     .get<Lang::InsertSTMT>()
                                                     .tableName.get());
            if (iter == m_infos->getInfos().end()) {
                return;
            }
            auto pair = getColumnsWithTable(iter->second->targetTable);
            assert(pair.first);
            auto columns = pair.second;
            const auto &specifiedColumns =
                statementInsert.getSpecifiedColumns();
            if (!specifiedColumns.empty()) {
                for (const auto &specifiedColumn : specifiedColumns.get()) {
                    auto iter = std::find(columns.begin(), columns.end(),
                                          specifiedColumn.description().get());
                    if (iter != columns.end()) {
                        columns.erase(iter);
                    }
                }
                //all columns should be specific
                assert(columns.empty());
                break;
            }
        } break;
        default:
            break;
    }
}
#endif

} //namespace WCDB
