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

#include <WCDB/Core.h>

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
    : Handle(path, tag), m_infos(infos), m_unlockShared(false)
{
}

#pragma mark - Override
bool MigrationHandle::execute(const Statement &statement)
{
    if (m_infos->isMigrated()) {
        return executeWithoutTampering(statement);
    }
#ifdef DEBUG
    debug_checkStatementLegal(statement);
#endif
    //Avoid migration info and source table changed
    SharedLockGuard lockGuard(m_infos->getSharedLock());
    MigrationTamperer tamperer(m_infos.get(), statement);
    const Statement &source = tamperer.didSourceTampered()
                                  ? tamperer.getTamperedSourceStatement()
                                  : statement;
    if (!tamperer.didTampered()) {
        return executeWithoutTampering(source);
    }
    return executeWithMultipleStatements(source,
                                         tamperer.getTamperedStatement());
}

bool MigrationHandle::prepare(const Statement &statement)
{
    if (m_infos->isMigrated()) {
        return prepareWithoutTampering(statement);
    }
#ifdef DEBUG
    debug_checkStatementLegal(statement);
#endif
    //Avoid migration info and source table changed
    SharedLockGuard lockGuard(m_infos->getSharedLock());
    MigrationTamperer tamperer(m_infos.get(), statement);
    const Statement &source = tamperer.didSourceTampered()
                                  ? tamperer.getTamperedSourceStatement()
                                  : statement;
    if (!tamperer.didTampered()) {
        return prepareWithoutTampering(source);
    }
    m_unlockShared = true;
    m_infos->getSharedLock().lockShared();
    return prepareWithMultipleStatements(source,
                                         tamperer.getTamperedStatement());
}

bool MigrationHandle::step(bool &done)
{
    if (!m_tamperedHandleStatement.isPrepared()) {
        return Handle::step(done);
    } else {
        if (!beginNestedTransaction()) {
            return false;
        }
        WCTInnerAssert(
            m_tamperedHandleStatement.getStatement().getStatementType() ==
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
    if (m_unlockShared) {
        m_unlockShared = false;
        m_infos->getSharedLock().unlockShared();
    }
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

#ifdef DEBUG
void MigrationHandle::debug_checkStatementLegal(const Statement &statement)
{
    switch (statement.getStatementType()) {
        case Statement::Type::AlterTable: {
            //Alter statement is not allowed
            const Lang::AlterTableSTMT &lang =
                statement.getCOWLang().get<Lang::AlterTableSTMT>();
            if (!lang.schemaName.empty() ||
                !lang.schemaName.equal(StatementAttach::getMainSchema())) {
                return;
            }
            auto iter = m_infos->getInfos().find(lang.tableName.get());
            WCTAssert(iter == m_infos->getInfos().end(),
                      "Executing Alter Table Statement on a migrating table is "
                      "not allowed yet.");
        } break;
        case Statement::Type::Update: {
            //Update statement with orderBy/limit/offset is not allowed.
            StatementUpdate statementUpdate(statement.getCOWLang());
            WCTAssert(!statementUpdate.isLimited(),
                      "Executing Update Statement with OrderBy/Limit/Offset on "
                      "a migrating table is not allowed yet.");
        } break;
        case Statement::Type::Delete: {
            //Delete statement with orderBy/limit/offset is not allowed.
            StatementDelete statementDelete(statement.getCOWLang());
            WCTAssert(!statementDelete.isLimited(),
                      "Executing Delete Statement with OrderBy/Limit/Offset on "
                      "a migrating table is not allowed yet.");
        } break;
        case Statement::Type::Insert: {
            //Partial replace statement not allowed.
            StatementInsert statementInsert(statement.getCOWLang());
            if (!statementInsert.isReplace()) {
                return;
            }
            const Lang::InsertSTMT &lang =
                statementInsert.getCOWLang().get<Lang::InsertSTMT>();
            if (!lang.schemaName.empty() ||
                !lang.schemaName.equal(StatementAttach::getMainSchema())) {
                return;
            }
            auto iter = m_infos->getInfos().find(lang.tableName.get());
            if (iter == m_infos->getInfos().end()) {
                return;
            }
            auto pair = getUnorderedColumnsWithTable(iter->second->targetTable);
            if (!pair.first) {
                return;
            }
            auto columns = pair.second;
            const auto &specifiedColumns =
                statementInsert.getSpecifiedColumns();
            if (!specifiedColumns.empty()) {
                for (const auto &specifiedColumn : specifiedColumns.get()) {
                    auto iter =
                        columns.find(specifiedColumn.description().get());
                    if (iter != columns.end()) {
                        columns.erase(iter);
                    }
                }
                //all columns should be specific
                WCTAssert(columns.empty(), "Executing Insert Statement by "
                                           "replacing without all columns "
                                           "specificed is not allowed yet. You "
                                           "can use Update Statement instead.");
                break;
            }
        } break;
        default:
            break;
    }
}
#endif

} //namespace WCDB
