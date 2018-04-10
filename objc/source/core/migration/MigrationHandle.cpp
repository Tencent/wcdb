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

#include <WCDB/MigrationHandle.hpp>

namespace WCDB {

#pragma mark - Initialize
std::shared_ptr<Handle> MigrationHandle::handleWithPath(const std::string &path,
                                                        Tag tag)
{
    return std::shared_ptr<Handle>(new MigrationHandle(path, tag));
}

MigrationHandle::MigrationHandle(const std::string &path, Tag tag)
    : Handle(path, tag)
{
}
#pragma mark - Override
bool MigrationHandle::execute(const Statement &statement)
{
    if (m_info) {
        Statement tamperedStatement(statement);
        bool tampered = false;
        bool migrating = m_info->isMigrating();
        if (!migrating &&
            statement.getStatementType() == Statement::Type::Insert) {
            tampered = m_info->tamperInsertStatementForUnstartedMigration(
                tamperedStatement);
        } else {
            tampered = m_info->tamper(tamperedStatement);
        }
        if (tampered) {
#ifdef DEBUG
            debugCheckStatementLegal(statement);
#endif
            // Since UPDATE and DELETE statements will execute on both migration table and migrated table, multiple statements should be run.
            switch (statement.getStatementType()) {
                case Statement::Type::Update:
                case Statement::Type::Delete:
                    return executeWithMultipleStatements(statement,
                                                         tamperedStatement);
                case Statement::Type::Insert:
                    return migrating
                               ? executeWithMultipleStatements(
                                     statement, tamperedStatement)
                               : executeWithoutTampering(tamperedStatement);
                default:
                    return executeWithoutTampering(tamperedStatement);
            }
        }
    }
    return executeWithoutTampering(statement);
}

bool MigrationHandle::prepare(const Statement &statement)
{
    if (m_info) {
        Statement tamperedStatement(statement);
        bool tampered = false;
        bool migrating = m_info->isMigrating();
        if (!migrating &&
            statement.getStatementType() == Statement::Type::Insert) {
            tampered = m_info->tamperInsertStatementForUnstartedMigration(
                tamperedStatement);
        } else {
            tampered = m_info->tamper(tamperedStatement);
        }
        if (tampered) {
#ifdef DEBUG
            debugCheckStatementLegal(statement);
#endif
            // Since UPDATE and DELETE statements will execute on both migration table and migrated table, multiple statements should be run.
            switch (statement.getStatementType()) {
                case Statement::Type::Update:
                case Statement::Type::Delete:
                    return prepareWithMultipleStatements(statement,
                                                         tamperedStatement);
                case Statement::Type::Insert:
                    return migrating
                               ? prepareWithMultipleStatements(
                                     statement, tamperedStatement)
                               : prepareWithoutTampering(tamperedStatement);
                default:
                    return prepareWithoutTampering(tamperedStatement);
            }
        }
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
}

#pragma mark - Migration
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

void MigrationHandle::setMigrationInfo(
    const std::shared_ptr<MigrationInfo> &info)
{
    if (m_info.get() != info.get()) {
        m_info = info;
    }
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
            if (!statementInsert.getCOWLang()
                     .get<Lang::InsertSTMT>()
                     .tableName.equal(m_info->getTargetTableName())) {
                return;
            }
            auto pair = getColumnsWithTable(m_info->getTargetTableName());
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
