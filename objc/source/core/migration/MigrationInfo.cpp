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
#include <WCDB/Core.h>

namespace WCDB {

#pragma mark - Initialize
std::shared_ptr<MigrationInfo>
MigrationInfo::info(const std::string &targetTable,
                    const std::string &sourceTable,
                    const std::string &sourceDatabasePath)
{
    WCTAssert(
        !sourceDatabasePath.empty() || targetTable != sourceTable,
        "Migrating the same name database to the same table is meaningless.");
    WCTAssert(!targetTable.empty() && !sourceTable.empty(),
              "Migrating table name can't not be empty.");
    return std::shared_ptr<MigrationInfo>(
        new MigrationInfo(targetTable, sourceTable, sourceDatabasePath));
}

MigrationInfo::MigrationInfo(const std::string &targetTable_,
                             const std::string &sourceTable_,
                             const std::string &sourceDatabasePath_)
    : targetTable(targetTable_)
    , sourceTable(sourceTable_)
    , sourceDatabasePath(sourceDatabasePath_)
    , schema(MigrationInfo::resolvedSchema(sourceDatabasePath_))
    , unionedViewName(getUnionedViewPrefix() + "_" + targetTable + "_" +
                      sourceTable)
    , m_inited(false)
{
}

#pragma mark - Basic
bool MigrationInfo::isInited() const
{
    return m_inited.load();
}

void MigrationInfo::initialize(const std::set<std::string> &columnNames)
{
    LockGuard lockGuard(m_spin);
    if (m_inited) {
        return;
    }
    std::list<Column> columns;
    std::list<ResultColumn> resultColumns;
    for (const auto &columnName : columnNames) {
        WCDB::Column column(columnName);
        resultColumns.push_back(column);
        columns.push_back(column);
    }

    //setup reusable statement
    OrderingTerm order = OrderingTerm(Column::rowid()).withOrder(Order::ASC);

    m_statementForPickingRowIDs = StatementSelect()
                                      .select(Column::rowid())
                                      .from(getSourceTable())
                                      .limit(BindParameter(1));

    m_statementForMigration =
        StatementInsert()
            .insertInto(targetTable)
            .on(columns)
            .values(StatementSelect()
                        .select(resultColumns)
                        .from(getSourceTable())
                        .where(Column::rowid() == BindParameter(1))
                        .limit(1));

    m_statementForDeletingMigratedRow =
        StatementDelete()
            .deleteFrom(getQualifiedSourceTable())
            .where(Column::rowid() == BindParameter(1))
            .limit(1);

    m_statementForCreatingUnionedView =
        StatementCreateView()
            .createView(unionedViewName)
            .temp()
            .ifNotExists()
            .as(StatementSelect()
                    .select(resultColumns)
                    .from(targetTable)
                    .union_(SelectCore()
                                .select(resultColumns)
                                .from(getSourceTable())));

    m_inited.store(true);
}

const std::string MigrationInfo::schemaPrefix()
{
    return "WCDBMigration_";
}

std::string MigrationInfo::resolvedSchema(const std::string &path)
{
    if (path.empty()) {
        return Schema::main();
    }
    return MigrationInfo::schemaPrefix() +
           std::to_string(std::hash<std::string>{}(path));
}

bool MigrationInfo::isSameDatabaseMigration() const
{
    return schema == Schema::main();
}

TableOrSubquery MigrationInfo::getSourceTable() const
{
    return TableOrSubquery(sourceTable).withSchema(schema);
}

QualifiedTableName MigrationInfo::getQualifiedSourceTable() const
{
    return QualifiedTableName(sourceTable).withSchema(schema);
}

#pragma mark - Statement
const StatementSelect &MigrationInfo::getStatementForPickingRowIDs() const
{
    return m_statementForPickingRowIDs;
}

const StatementDelete &MigrationInfo::getStatementForDeletingMigratedRow() const
{
    return m_statementForDeletingMigratedRow;
}

StatementDropTable MigrationInfo::getStatementForDroppingOldTable() const
{
    return StatementDropTable()
        .dropTable(sourceTable)
        .withSchema(schema)
        .ifExists();
}

const std::string MigrationInfo::getUnionedViewPrefix()
{
    return "WCDBUnioned";
}

StatementCreateView MigrationInfo::getStatementForCreatingUnionedView() const
{
    return m_statementForCreatingUnionedView;
}

StatementDropView MigrationInfo::getStatementForDroppingUnionedView() const
{
    return StatementDropView()
        .dropView(unionedViewName)
        .ifExists()
        .withSchema(Schema::temp());
}

StatementInsert MigrationInfo::getStatementForTamperingConflictType(
    const Lang::InsertSTMT::Type &type) const
{
    StatementInsert statement = m_statementForMigration;
    statement.getCOWLang().get_or_copy<Lang::InsertSTMT>().type = type;
    return statement;
}

} //namespace WCDB
