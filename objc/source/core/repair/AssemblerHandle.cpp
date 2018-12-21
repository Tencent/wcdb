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

namespace WCDB {

AssemblerHandle::AssemblerHandle()
: Handle()
, Repair::Assembler()
, m_primary(-1)
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
{
}

AssemblerHandle::~AssemblerHandle()
{
    returnStatement(m_cellStatement);
}

void AssemblerHandle::setPath(const String &path)
{
    Handle::setPath(path);
}

const String &AssemblerHandle::getPath() const
{
    return Handle::getPath();
}

bool AssemblerHandle::markAsAssembling()
{
    bool succeed = false;
    if (open()) {
        succeed = execute(m_statementForDisableJounral)
                  && execute(m_statementForEnableMMap) && markSequenceAsAssembling();
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
    markErrorAsIgnorable((int) Error::Code::Error);
    bool succeed = execute(sql);
    markErrorAsUnignorable();
    return succeed;
}

const Error &AssemblerHandle::getError() const
{
    return Handle::getError();
}

#pragma mark - Table
bool AssemblerHandle::assembleTable(const String &tableName, const String &sql)
{
    m_cellStatement->finalize();
    m_table.clear();
    markErrorAsIgnorable((int) Error::Code::Error);
    bool succeed = execute(sql);
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
            if (i == m_primary) {
                m_cellStatement->bindInteger64(cell.getRowID(), bindIndex);
            } else {
                m_cellStatement->bindNull(bindIndex);
            }
            break;
        }
    }
    bool succeed = m_cellStatement->step();
    m_cellStatement->reset();
    return succeed;
}

bool AssemblerHandle::lazyPrepareCell()
{
    if (m_cellStatement->isPrepared()) {
        return true;
    }

    std::list<String> columnNames;
    if (!prepare(StatementPragma().pragma(Pragma::tableInfo()).with(m_table))) {
        return false;
    }
    int primary = -1;
    int maxpk = 0;
    bool done = false;
    while (step(done) && !done) {
        columnNames.push_back(getText(1));
        //check if and only if single column is primary key
        int pk = getInteger32(5);
        maxpk = std::max(pk, maxpk);
        if (pk == 1) {
            primary = (int) columnNames.size() - 1;
        }
    }
    finalize();

    if (!done) {
        return false;
    }

    m_primary = maxpk == 1 ? primary : -1;

    Columns columns = { Column::rowid() };
    for (const auto &columnName : columnNames) {
        columns.push_back(Column(columnName));
    }
    StatementInsert statement = StatementInsert().insertIntoTable(m_table);
    if (isDuplicatedIgnorable()) {
        statement.orIgnore();
    }
    statement.columns(columns).values(BindParameter::bindParameters(columns.size()));

    return m_cellStatement->prepare(statement);
}

#pragma mark - Sequence
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
    if (!prepare(m_statementForUpdateSequence)) {
        return { false, false };
    }
    bindInteger64(sequence, 1);
    bool succeed = step();
    finalize();
    if (succeed) {
        return { true, getChanges() > 0 };
    }
    return { false, false };
}

bool AssemblerHandle::insertSequence(const String &tableName, int64_t sequence)
{
    if (!prepare(m_statementForInsertSequence)) {
        return false;
    }
    bindText(tableName, 1);
    bindInteger64(sequence, 2);
    bool succeed = step();
    finalize();
    return succeed;
}

bool AssemblerHandle::markSequenceAsAssembling()
{
    return execute(StatementCreateTable()
                   .createTable(s_dummySequence)
                   .ifNotExists()
                   .define(ColumnDef(Column("i"), ColumnType::Integer32)
                           .constraint(ColumnConstraint().primaryKey().autoIncrement())));
}

bool AssemblerHandle::markSequenceAsAssembled()
{
    return execute(StatementDropTable().dropTable(s_dummySequence).ifExists());
}

} // namespace WCDB
