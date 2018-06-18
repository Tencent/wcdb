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
                                MigrationSetting &setting)
{
    return std::shared_ptr<Handle>(new MigrationHandle(path, setting));
}

MigrationHandle::MigrationHandle(const std::string &path,
                                 MigrationSetting &setting)
    : Handle(path)
    , m_setting(setting)
    , m_unlockShared(false)
    , m_tamperer(setting.getInfos())
    , m_extraHandleStatement1(this)
    , m_extraHandleStatement2(this)
    , m_tamperedHandleStatement(this)
{
}

#pragma mark - Override
bool MigrationHandle::execute(const Statement &statement)
{
    if (m_setting.isMigrated()) {
        return executeWithoutTampering(statement);
    }
#ifdef DEBUG
    debug_checkStatementLegal(statement);
#endif
    //Avoid migration info and source table changed
    SharedLockGuard lockGuard(m_setting.getSharedLock());
    if (m_setting.isMigrated()) {
        return executeWithoutTampering(statement);
    }
    m_tamperer.tamper(statement);
    const Statement &source = m_tamperer.getSourceStatement();
    bool result = false;
    if (m_tamperer.isTampered()) {
        if (beginNestedTransaction()) {
            switch (source.getStatementType()) {
                case Statement::Type::Insert:
                    result =
                        executeWithoutTampering(source) &&
                        migrateWithRowID(
                            getLastInsertedRowID(),
                            m_tamperer.getAssociatedInfo(),
                            source.getCOWLang().get<Lang::InsertSTMT>().type);
                    m_extraHandleStatement1.finalize();
                    m_extraHandleStatement2.finalize();
                    break;
                default:
                    result = executeWithoutTampering(source) &&
                             executeWithoutTampering(
                                 m_tamperer.getTamperedStatement());
                    break;
            }
            if (result) {
                result = commitOrRollbackNestedTransaction();
            } else {
                rollbackNestedTransaction();
            }
        }
    } else {
        result = executeWithoutTampering(source);
    }
    m_tamperer.reset();
    return result;
}

bool MigrationHandle::prepare(const Statement &statement)
{
    if (m_setting.isMigrated()) {
        return prepareWithoutTampering(statement);
    }
#ifdef DEBUG
    debug_checkStatementLegal(statement);
#endif
    //Avoid migration info and source table changed
    SharedLockGuard lockGuard(m_setting.getSharedLock());
    if (m_setting.isMigrated()) {
        return executeWithoutTampering(statement);
    }
    m_tamperer.tamper(statement);
    if (!m_tamperer.isTampered()) {
        return prepareWithoutTampering(m_tamperer.getSourceStatement());
    }
    const Statement &source = m_tamperer.getSourceStatement();
    if (!prepareWithoutTampering(source)) {
        return false;
    }
    if (source.getStatementType() != Statement::Type::Insert &&
        !m_tamperedHandleStatement.prepare(m_tamperer.getTamperedStatement())) {
        m_handleStatement.finalize();
        return false;
    }
    WCTInnerAssert(m_unlockShared == false);
    m_unlockShared = true;
    m_setting.getSharedLock().lockShared();
    return true;
}

bool MigrationHandle::step(bool &done)
{
    if (!m_tamperer.isTampered()) {
        return Handle::step(done);
    } else {
        if (!beginNestedTransaction()) {
            return false;
        }
        const Statement &source = m_tamperer.getSourceStatement();
        if (source.getStatementType() == Statement::Type::Insert) {
            if (Handle::step(done) &&
                migrateWithRowID(
                    getLastInsertedRowID(), m_tamperer.getAssociatedInfo(),
                    source.getCOWLang().get<Lang::InsertSTMT>().type)) {
                return commitOrRollbackNestedTransaction();
            }
        } else {
            if (Handle::step(done) && m_tamperedHandleStatement.step(done)) {
                return commitOrRollbackNestedTransaction();
            }
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
    m_extraHandleStatement1.finalize();
    m_extraHandleStatement2.finalize();
    m_tamperer.reset();
    if (m_unlockShared) {
        m_unlockShared = false;
        m_setting.getSharedLock().unlockShared();
    }
}

#pragma mark - Migration
bool MigrationHandle::migrateWithRowID(
    const long long &rowid,
    const std::shared_ptr<MigrationInfo> &info,
    const Lang::InsertSTMT::Type &onConflict)
{
    WCTInnerAssert(info != nullptr && isInTransaction());
    if (!m_extraHandleStatement1.isPrepared() ||
        m_associatedConflictType != onConflict) {
        if (!m_extraHandleStatement1.prepare(
                info->getStatementForTamperingConflictType(onConflict))) {
            return false;
        }
    }
    if (!m_extraHandleStatement2.isPrepared()) {
        if (!m_extraHandleStatement2.prepare(
                info->getStatementForDeletingMigratedRow())) {
            m_extraHandleStatement1.finalize();
            return false;
        }
    }
    if (!_migrateWithRowID(rowid, m_extraHandleStatement1) ||
        !_migrateWithRowID(rowid, m_extraHandleStatement2)) {
        m_extraHandleStatement1.finalize();
        m_extraHandleStatement2.finalize();
        return false;
    }
    return true;
}

bool MigrationHandle::_migrateWithRowID(const long long &rowid,
                                        HandleStatement &handleStatement)
{
    handleStatement.bindInteger64(rowid, 1);
    bool done = false;
    bool result = handleStatement.step(done);
    handleStatement.reset();
    return result;
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
    SharedLockGuard lockGuard(m_setting.getSharedLock());
    switch (statement.getStatementType()) {
        case Statement::Type::AlterTable: {
            if (((const StatementAlterTable &) statement)
                    .isAlteringDefaultValueColumn()) {
                return;
            }
            const Lang::AlterTableSTMT &lang =
                statement.getCOWLang().get<Lang::AlterTableSTMT>();
            if (!lang.schemaName.empty() ||
                !lang.schemaName.equal(Lang::mainSchema())) {
                return;
            }
            auto iter = m_setting.getInfos().find(lang.tableName.get());
            WCTAssert(iter == m_setting.getInfos().end(),
                      "Altering a column without default value on a migrating "
                      "table is not allowed yet.");
        } break;
        case Statement::Type::Update: {
            //Update statement with orderBy/limit/offset is not allowed.
            StatementUpdate statementUpdate(statement.getCOWLang());
            if (!statementUpdate.isLimited()) {
                return;
            }
            auto iter = m_setting.getInfos().find(
                statementUpdate.getCOWLang()
                    .get<Lang::UpdateSTMT>()
                    .qualifiedTableName.get<Lang::QualifiedTableName>()
                    .tableName.get());
            WCTAssert(iter == m_setting.getInfos().end(),
                      "Updating with OrderBy/Limit/Offset on "
                      "a migrating table is not allowed yet.");
        } break;
        case Statement::Type::Delete: {
            //Delete statement with orderBy/limit/offset is not allowed.
            StatementDelete statementDelete(statement.getCOWLang());
            if (!statementDelete.isLimited()) {
                return;
            }
            auto iter = m_setting.getInfos().find(
                statementDelete.getCOWLang()
                    .get<Lang::DeleteSTMT>()
                    .qualifiedTableName.get<Lang::QualifiedTableName>()
                    .tableName.get());
            WCTAssert(iter == m_setting.getInfos().end(),
                      "Deleting with OrderBy/Limit/Offset on "
                      "a migrating table is not allowed yet.");
        } break;
        default:
            break;
    }
}
#endif

} //namespace WCDB
