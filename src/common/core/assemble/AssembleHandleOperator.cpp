//
// Created by qiuwenchen on 2023/11/17.
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

#include "AssembleHandleOperator.hpp"

namespace WCDB {

AssembleHandleOperator::AssembleHandleOperator(InnerHandle *handle)
: HandleOperator(handle)
, Repair::AssembleDelegate()
, m_statementForDisableJounral(StatementPragma().pragma(Pragma::journalMode()).to("OFF"))
, m_statementForEnableMMap(StatementPragma().pragma(Pragma::mmapSize()).to(2147418112))
, m_integerPrimary(-1)
, m_withoutRowid(false)
, m_cellStatement(handle->getStatement(DecoratorAllType))
, m_statementForUpdateSequence(StatementUpdate()
                               .update("sqlite_sequence")
                               .set(Column("seq"))
                               .to(BindParameter(1))
                               .where(Column("name") == BindParameter(2)))
, m_statementForInsertSequence(StatementInsert()
                               .insertIntoTable("sqlite_sequence")
                               .columns({ Column("name"), Column("seq") })
                               .values(BindParameter::bindParameters(2)))
{
}

AssembleHandleOperator::~AssembleHandleOperator()
{
    getHandle()->returnStatement(m_cellStatement);
}

#pragma mark - Assemble
void AssembleHandleOperator::setAssemblePath(const UnsafeStringView &path)
{
    getHandle()->setPath(path);
}

const StringView &AssembleHandleOperator::getAssemblePath() const
{
    return getHandle()->getPath();
}

const Error &AssembleHandleOperator::getAssembleError() const
{
    return getHandle()->getError();
}

void AssembleHandleOperator::suspendAssemble()
{
    return getHandle()->suspend(true);
}

void AssembleHandleOperator::finishAssemble()
{
    getHandle()->close();
}

bool AssembleHandleOperator::markAsAssembling()
{
    InnerHandle *handle = getHandle();
    bool succeed = handle->open();
    if (succeed) {
        if (!handle->execute(m_statementForDisableJounral)
            || !handle->execute(m_statementForEnableMMap) || !markSequenceAsAssembling()) {
            handle->close();
            succeed = false;
        }
    }
    return succeed;
}

bool AssembleHandleOperator::markAsAssembled()
{
    m_table.clear();
    m_cellStatement->finalize();
    InnerHandle *handle = getHandle();
    bool succeed = markSequenceAsAssembled();
    if (handle->isInTransaction()) {
        succeed = handle->commitOrRollbackTransaction() && succeed;
    }
    // TODO: check integrity after assembled?
    handle->close();
    return succeed;
}

bool AssembleHandleOperator::markAsMilestone()
{
    InnerHandle *handle = getHandle();
    if (handle->isInTransaction()) {
        if (!handle->commitOrRollbackTransaction()) {
            return false;
        }
    }
    return handle->beginTransaction();
}

bool AssembleHandleOperator::assembleSQL(const UnsafeStringView &sql)
{
    InnerHandle *handle = getHandle();
    bool succeed = false;
    handle->markErrorAsIgnorable(Error::Code::Error);
    succeed = handle->executeSQL(sql);
    if (!succeed && handle->getError().code() == Error::Code::Error) {
        succeed = true;
    }
    handle->markErrorAsUnignorable();
    return succeed;
}

#pragma mark - Assemble - Table
bool AssembleHandleOperator::assembleTable(const UnsafeStringView &tableName,
                                           const UnsafeStringView &sql)
{
    m_cellStatement->finalize();
    m_table.clear();
    InnerHandle *handle = getHandle();
    bool succeed = false;
    handle->markErrorAsIgnorable(Error::Code::Error);
    succeed = handle->executeSQL(sql);
    if (!succeed && handle->getError().code() == Error::Code::Error) {
        succeed = true;
    }
    handle->markErrorAsUnignorable();
    if (succeed) {
        m_table = tableName;
        auto attribute = handle->getTableAttribute(Schema::main(), tableName);
        if (attribute.hasValue()) {
            m_withoutRowid = attribute->withoutRowid;
        }
    }
    return succeed;
}

bool AssembleHandleOperator::isAssemblingTableWithoutRowid() const
{
    return m_withoutRowid;
}

bool AssembleHandleOperator::assembleCell(const Repair::Cell &cell)
{
    WCTAssert(!m_table.empty());
    if (!lazyPrepareCell()) {
        return false;
    }
    WCTAssert(m_cellStatement->isPrepared());
    m_cellStatement->reset();
    if (!m_withoutRowid) {
        m_cellStatement->bindInteger(cell.getRowID(), 1);
    }
    for (int i = 0; i < cell.getCount(); ++i) {
        int bindIndex = m_withoutRowid ? i + 1 : i + 2;
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

void AssembleHandleOperator::markDuplicatedAsReplaceable(bool replaceable)
{
    if (isDuplicatedReplaceable() != replaceable && m_cellStatement->isPrepared()) {
        m_cellStatement->finalize();
    }
    AssembleDelegate::markDuplicatedAsReplaceable(replaceable);
}

bool AssembleHandleOperator::lazyPrepareCell()
{
    if (m_cellStatement->isPrepared()) {
        return true;
    }

    auto optionalMetas = getHandle()->getTableMeta(Schema(), m_table);
    if (!optionalMetas.succeed()) {
        return false;
    }
    auto &metas = optionalMetas.value();
    m_integerPrimary = ColumnMeta::getIndexOfIntegerPrimary(metas);

    Columns columns = {};
    if (!m_withoutRowid) {
        columns.push_back(Column::rowid());
    }
    for (const auto &meta : metas) {
        columns.push_back(Column(meta.name));
    }
    StatementInsert statement = StatementInsert().insertIntoTable(m_table);
    if (isDuplicatedReplaceable()) {
        statement.orReplace();
    } else if (isDuplicatedIgnorable()) {
        statement.orIgnore();
    }
    statement.columns(columns).values(BindParameter::bindParameters(columns.size()));

    return m_cellStatement->prepare(statement);
}

#pragma mark - Assemble - Sequence
bool AssembleHandleOperator::assembleSequence(const UnsafeStringView &tableName, int64_t sequence)
{
    if (sequence == 0) {
        return true;
    }
    bool succeed = false;
    auto worked = updateSequence(tableName, sequence);
    if (worked.succeed()) {
        if (worked.value()) {
            succeed = true;
        } else {
            succeed = insertSequence(tableName, sequence);
        }
    }
    return succeed;
}

Optional<bool>
AssembleHandleOperator::updateSequence(const UnsafeStringView &tableName, int64_t sequence)
{
    Optional<bool> worked;
    InnerHandle *handle = getHandle();
    if (handle->prepare(m_statementForUpdateSequence)) {
        handle->bindInteger(sequence, 1);
        handle->bindText(tableName, 2);
        if (handle->step()) {
            worked = handle->getChanges() > 0;
        }
        handle->finalize();
    }
    return worked;
}

bool AssembleHandleOperator::insertSequence(const UnsafeStringView &tableName, int64_t sequence)
{
    InnerHandle *handle = getHandle();
    bool succeed = false;
    if (handle->prepare(m_statementForInsertSequence)) {
        handle->bindText(tableName, 1);
        handle->bindInteger(sequence, 2);
        succeed = handle->step();
        handle->finalize();
    }
    return succeed;
}

bool AssembleHandleOperator::markSequenceAsAssembling()
{
    return getHandle()->executeStatement(
    StatementCreateTable()
    .createTable(s_dummySequence)
    .ifNotExists()
    .define(ColumnDef(Column("i"), ColumnType::Integer)
            .constraint(ColumnConstraint().primaryKey().autoIncrement())));
}

bool AssembleHandleOperator::markSequenceAsAssembled()
{
    return getHandle()->executeStatement(
    StatementDropTable().dropTable(s_dummySequence).ifExists());
}

} // namespace WCDB
