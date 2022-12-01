//
// Created by sanhuazhang on 2019/08/27
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

#include <WCDB/AssembleHandle.hpp>
#include <WCDB/CoreConst.h>

namespace WCDB {

AssembleHandle::AssembleHandle()
: InnerHandle()
, Repair::AssembleDelegate()
, m_statementForDisableJounral(StatementPragma().pragma(Pragma::journalMode()).to("OFF"))
, m_statementForEnableMMap(StatementPragma().pragma(Pragma::mmapSize()).to(2147418112))
, m_integerPrimary(-1)
, m_cellStatement(getStatement())
, m_statementForUpdateSequence(StatementUpdate()
                               .update("sqlite_sequence")
                               .set(Column("seq"))
                               .to(BindParameter(1))
                               .where(Column("name") == BindParameter(2)))
, m_statementForInsertSequence(StatementInsert()
                               .insertIntoTable("sqlite_sequence")
                               .columns({ Column("name"), Column("seq") })
                               .values(BindParameter::bindParameters(2)))
, m_statementForReadTransaction(StatementBegin().beginDeferred())
, m_statementForAcquireReadLock(
  StatementSelect().select(1).from(Syntax::masterTable).limit(0))
{
}

AssembleHandle::~AssembleHandle()
{
    returnStatement(m_cellStatement);
}

#pragma mark - Assemble
void AssembleHandle::setAssemblePath(const UnsafeStringView &path)
{
    InnerHandle::setPath(path);
}

const StringView &AssembleHandle::getAssemblePath() const
{
    return InnerHandle::getPath();
}

const Error &AssembleHandle::getAssembleError() const
{
    return InnerHandle::getError();
}

void AssembleHandle::finishAssemble()
{
    InnerHandle::close();
}

bool AssembleHandle::markAsAssembling()
{
    bool succeed = open();
    if (succeed) {
        if (!execute(m_statementForDisableJounral)
            || !execute(m_statementForEnableMMap) || !markSequenceAsAssembling()) {
            close();
            succeed = false;
        }
    }
    return succeed;
}

bool AssembleHandle::markAsAssembled()
{
    m_table.clear();
    m_cellStatement->finalize();
    bool succeed = markSequenceAsAssembled();
    if (isInTransaction()) {
        succeed = commitOrRollbackTransaction() && succeed;
    }
    // TODO: check integrity after assembled?
    close();
    return succeed;
}

bool AssembleHandle::markAsMilestone()
{
    if (isInTransaction()) {
        if (!commitOrRollbackTransaction()) {
            return false;
        }
    }
    return beginTransaction();
}

bool AssembleHandle::assembleSQL(const UnsafeStringView &sql)
{
    markErrorAsIgnorable(Error::Code::Error);
    bool succeed = executeSQL(sql);
    if (!succeed && getError().code() == Error::Code::Error) {
        succeed = true;
    }
    markErrorAsUnignorable();
    return succeed;
}

#pragma mark - Assemble - Table
bool AssembleHandle::assembleTable(const UnsafeStringView &tableName,
                                   const UnsafeStringView &sql)
{
    m_cellStatement->finalize();
    m_table.clear();
    markErrorAsIgnorable(Error::Code::Error);
    bool succeed = executeSQL(sql);
    if (!succeed && getError().code() == Error::Code::Error) {
        succeed = true;
    }
    markErrorAsUnignorable();
    if (succeed) {
        m_table = tableName;
    }
    return succeed;
}

bool AssembleHandle::assembleCell(const Repair::Cell &cell)
{
    WCTAssert(!m_table.empty());
    if (!lazyPrepareCell()) {
        return false;
    }
    WCTAssert(m_cellStatement->isPrepared());
    m_cellStatement->reset();
    m_cellStatement->bindInteger(cell.getRowID(), 1);
    for (int i = 0; i < cell.getCount(); ++i) {
        int bindIndex = i + 2;
        switch (cell.getValueType(i)) {
        case Repair::Cell::Integer:
            m_cellStatement->bindInteger(cell.integerValue(i), bindIndex);
            break;
        case Repair::Cell::Text: {
            m_cellStatement->bindText(cell.textValue(i), bindIndex);
            break;
        }
        case Repair::Cell::BLOB: {
            m_cellStatement->bindBLOB(cell.blobValue(i), bindIndex);
            break;
        }
        case Repair::Cell::Real:
            m_cellStatement->bindDouble(cell.doubleValue(i), bindIndex);
            break;
        case Repair::Cell::Null:
            if (i == m_integerPrimary) {
                m_cellStatement->bindInteger(cell.getRowID(), bindIndex);
            } else {
                m_cellStatement->bindNull(bindIndex);
            }
            break;
        }
    }
    bool succeed = m_cellStatement->step();
    return succeed;
}

bool AssembleHandle::lazyPrepareCell()
{
    if (m_cellStatement->isPrepared()) {
        return true;
    }

    auto optionalMetas = getTableMeta(Schema::main(), m_table);
    if (!optionalMetas.has_value()) {
        return false;
    }
    auto &metas = optionalMetas.value();
    m_integerPrimary = ColumnMeta::getIndexOfIntegerPrimary(metas);

    Columns columns = { Column::rowid() };
    for (const auto &meta : metas) {
        columns.push_back(Column(meta.name));
    }
    StatementInsert statement = StatementInsert().insertIntoTable(m_table);
    if (isDuplicatedIgnorable()) {
        statement.orIgnore();
    }
    statement.columns(columns).values(BindParameter::bindParameters(columns.size()));

    return m_cellStatement->prepare(statement);
}

#pragma mark - Assemble - Sequence
bool AssembleHandle::assembleSequence(const UnsafeStringView &tableName, int64_t sequence)
{
    if (sequence == 0) {
        return true;
    }
    bool succeed = false;
    auto worked = updateSequence(tableName, sequence);
    if (worked.has_value()) {
        if (worked.value()) {
            succeed = true;
        } else {
            succeed = insertSequence(tableName, sequence);
        }
    }
    return succeed;
}

std::optional<bool>
AssembleHandle::updateSequence(const UnsafeStringView &tableName, int64_t sequence)
{
    std::optional<bool> worked;
    if (prepare(m_statementForUpdateSequence)) {
        bindInteger(sequence, 1);
        bindText(tableName, 2);
        if (step()) {
            worked = getChanges() > 0;
        }
        finalize();
    }
    return worked;
}

bool AssembleHandle::insertSequence(const UnsafeStringView &tableName, int64_t sequence)
{
    bool succeed = false;
    if (prepare(m_statementForInsertSequence)) {
        bindText(tableName, 1);
        bindInteger(sequence, 2);
        succeed = step();
        finalize();
    }
    return succeed;
}

bool AssembleHandle::markSequenceAsAssembling()
{
    return executeStatement(
    StatementCreateTable()
    .createTable(s_dummySequence)
    .ifNotExists()
    .define(ColumnDef(Column("i"), ColumnType::Integer)
            .constraint(ColumnConstraint().primaryKey().autoIncrement())));
}

bool AssembleHandle::markSequenceAsAssembled()
{
    return executeStatement(StatementDropTable().dropTable(s_dummySequence).ifExists());
}

#pragma mark - Backup
void AssembleHandle::setBackupPath(const UnsafeStringView &path)
{
    InnerHandle::setPath(path);
}

const StringView &AssembleHandle::getBackupPath() const
{
    return InnerHandle::getPath();
}

const Error &AssembleHandle::getBackupError() const
{
    return InnerHandle::getError();
}

void AssembleHandle::finishBackup()
{
    InnerHandle::close();
}

bool AssembleHandle::acquireBackupSharedLock()
{
    return open() && execute(m_statementForReadTransaction)
           && execute(m_statementForAcquireReadLock);
}

bool AssembleHandle::releaseBackupSharedLock()
{
    rollbackTransaction();
    return true;
}

bool AssembleHandle::acquireBackupExclusiveLock()
{
    return open() && beginTransaction();
}

bool AssembleHandle::releaseBackupExclusiveLock()
{
    rollbackTransaction();
    return true;
}

const Error &AssembleHandle::getCipherError() const
{
    return InnerHandle::getError();
}

bool AssembleHandle::openCipherInMemory(bool onlyUsedCipherKey)
{
    InnerHandle::setPath(":memory:");
    if (onlyUsedCipherKey) {
        return InnerHandle::openPureCipherDB();
    } else {
        return InnerHandle::open();
    }
}

bool AssembleHandle::isCipherDB() const
{
    return InnerHandle::isCipherDB();
}

void AssembleHandle::closeCipher()
{
    InnerHandle::close();
}

void *AssembleHandle::getCipherContext()
{
    if (!isOpened()) {
        return nullptr;
    }
    return AbstractHandle::getCipherContext();
}

size_t AssembleHandle::getCipherPageSize()
{
    return AbstractHandle::getCipherPageSize();
}

StringView AssembleHandle::getCipherSalt()
{
    return AbstractHandle::getCipherSalt();
}

bool AssembleHandle::setCipherSalt(const UnsafeStringView &salt)
{
    return AbstractHandle::setCipherSalt(salt);
}

} // namespace WCDB
