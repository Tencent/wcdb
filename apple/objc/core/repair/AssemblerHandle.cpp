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

#include <WCDB/AssemblerHandle.hpp>
#include <WCDB/CoreConst.h>

namespace WCDB {

AssemblerHandle::AssemblerHandle()
: Handle()
, Repair::Assembler()
, m_integerPrimary(-1)
, m_statementForDisableJounral(StatementPragma().pragma(Pragma::journalMode()).to("OFF"))
, m_statementForEnableMMap(StatementPragma().pragma(Pragma::mmapSize()).to(2147418112))
, m_statementForInsertSequence(StatementInsert()
                               .insertIntoTable("sqlite_sequence")
                               .columns({ Column("name"), Column("seq") })
                               .values(BindParameter::bindParameters(2)))
, m_statementForUpdateSequence(StatementUpdate()
                               .update("sqlite_sequence")
                               .set(Column("seq"))
                               .to(BindParameter(1))
                               .where(Column("name") == BindParameter(2)))
, m_cellStatement(getStatement())
, m_statementForAcquireReadLock(
  StatementSelect().select(1).from(Syntax::masterTable).limit(0))
, m_statementForReadTransaction(StatementBegin().beginDeferred())
{
    m_error.infos.set(ErrorStringKeyAction, ErrorActionAssembler);
}

AssemblerHandle::~AssemblerHandle()
{
    returnStatement(m_cellStatement);
}

#pragma mark - Common
void AssemblerHandle::setPath(const String &path)
{
    Handle::setPath(path);
}

void AssemblerHandle::clearPath()
{
    Handle::clearPath();
}

const String &AssemblerHandle::getPath() const
{
    return Handle::getPath();
}

const Error &AssemblerHandle::getError() const
{
    return Handle::getError();
}

#pragma mark - Assembler
bool AssemblerHandle::markAsAssembling()
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

bool AssemblerHandle::markAsAssembled()
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

bool AssemblerHandle::markAsMilestone()
{
    if (isInTransaction()) {
        if (!commitOrRollbackTransaction()) {
            return false;
        }
    }
    return beginTransaction();
}

bool AssemblerHandle::assembleSQL(const String &sql)
{
    markErrorAsIgnorable(Error::Code::Error);
    bool succeed = executeSQL(sql);
    if (!succeed && getError().code() == Error::Code::Error) {
        succeed = true;
    }
    markErrorAsUnignorable();
    return succeed;
}

#pragma mark - Assembler - Table
bool AssemblerHandle::assembleTable(const String &tableName, const String &sql)
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

bool AssemblerHandle::assembleCell(const Repair::Cell &cell)
{
    WCTInnerAssert(!m_table.empty());
    if (!lazyPrepareCell()) {
        return false;
    }
    WCTInnerAssert(m_cellStatement->isPrepared());
    m_cellStatement->reset();
    m_cellStatement->bindInteger64(cell.getRowID(), 1);
    for (int i = 0; i < cell.getCount(); ++i) {
        int bindIndex = i + 2;
        switch (cell.getValueType(i)) {
        case Repair::Cell::Integer:
            m_cellStatement->bindInteger64(cell.integerValue(i), bindIndex);
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
                m_cellStatement->bindInteger64(cell.getRowID(), bindIndex);
            } else {
                m_cellStatement->bindNull(bindIndex);
            }
            break;
        }
    }
    bool succeed = m_cellStatement->step();
    return succeed;
}

bool AssemblerHandle::lazyPrepareCell()
{
    if (m_cellStatement->isPrepared()) {
        return true;
    }

    bool succeed = false;
    std::vector<ColumnMeta> columnMetas;
    std::tie(succeed, columnMetas) = getTableMeta(Schema::main(), m_table);
    if (!succeed) {
        return false;
    }
    m_integerPrimary = ColumnMeta::getIndexOfIntegerPrimary(columnMetas);

    Columns columns = { Column::rowid() };
    for (const auto &columnMeta : columnMetas) {
        columns.push_back(Column(columnMeta.name));
    }
    StatementInsert statement = StatementInsert().insertIntoTable(m_table);
    if (isDuplicatedIgnorable()) {
        statement.orIgnore();
    }
    statement.columns(columns).values(BindParameter::bindParameters(columns.size()));

    return m_cellStatement->prepare(statement);
}

#pragma mark - Assembler - Sequence
bool AssemblerHandle::assembleSequence(const String &tableName, int64_t sequence)
{
    bool succeed, updated;
    std::tie(succeed, updated) = updateSequence(tableName, sequence);
    if (!succeed) {
        return false;
    }
    if (updated) {
        return true;
    }
    return insertSequence(tableName, sequence);
}

std::pair<bool, bool>
AssemblerHandle::updateSequence(const String &tableName, int64_t sequence)
{
    bool succeed = false;
    bool worked = false;
    if (prepare(m_statementForUpdateSequence)) {
        bindInteger64(sequence, 1);
        bindText(tableName, 2);
        succeed = step();
        finalize();
        if (succeed) {
            worked = getChanges() > 0;
        }
    }
    return { succeed, worked };
}

bool AssemblerHandle::insertSequence(const String &tableName, int64_t sequence)
{
    bool succeed = false;
    if (prepare(m_statementForInsertSequence)) {
        bindText(tableName, 1);
        bindInteger64(sequence, 2);
        succeed = step();
        finalize();
    }
    return succeed;
}

bool AssemblerHandle::markSequenceAsAssembling()
{
    return executeStatement(
    StatementCreateTable()
    .createTable(s_dummySequence)
    .ifNotExists()
    .define(ColumnDef(Column("i"), ColumnType::Integer32)
            .constraint(ColumnConstraint().primaryKey().autoIncrement())));
}

bool AssemblerHandle::markSequenceAsAssembled()
{
    return executeStatement(StatementDropTable().dropTable(s_dummySequence).ifExists());
}

#pragma mark - Backup
bool AssemblerHandle::acquireReadLock()
{
    return open() && execute(m_statementForReadTransaction)
           && execute(m_statementForAcquireReadLock);
}

bool AssemblerHandle::releaseReadLock()
{
    rollbackTransaction();
    return true;
}

bool AssemblerHandle::acquireWriteLock()
{
    return open() && beginTransaction();
}

bool AssemblerHandle::releaseWriteLock()
{
    rollbackTransaction();
    return true;
}

} // namespace WCDB
