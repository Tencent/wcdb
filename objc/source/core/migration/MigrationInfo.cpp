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

#include <WCDB/MigrationInfo.hpp>

namespace WCDB {

#pragma mark - Initialize
std::shared_ptr<MigrationInfo>
MigrationInfo::info(const std::string &targetTable,
                    const std::string &sourceTable,
                    const std::string &sourceDatabasePath)
{
    assert(!sourceDatabasePath.empty() || targetTable != sourceTable);
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
{
    prepareForMigrating();
}

#pragma mark - Basic
const std::string MigrationInfo::schemaPrefix()
{
    static const std::string s_schemaPrefix("WCDBMigration_");
    return s_schemaPrefix;
}

std::string MigrationInfo::resolvedSchema(const std::string &path)
{
    if (path.empty()) {
        return StatementAttach::getMainSchema();
    }
    return MigrationInfo::schemaPrefix() +
           std::to_string(std::hash<std::string>{}(path));
}

bool MigrationInfo::isSameDatabaseMigration() const
{
    return schema == StatementAttach::getMainSchema();
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
void MigrationInfo::prepareForMigrating()
{
    //setup reusable statement
    OrderingTerm order = OrderingTerm(Column::rowid).withOrder(Order::DESC);
    Expression limit = 1; //TODO opti the step of migration

    m_statementForMigration = StatementInsert()
                                  .insertOrIgnoreInto(targetTable)
                                  .withSchema(StatementAttach::getMainSchema())
                                  .values(StatementSelect()
                                              .select(ResultColumn::All)
                                              .from(getSourceTable())
                                              .orderBy(order)
                                              .limit(limit));

    m_statementForDeleteingMigratedRow =
        StatementDelete()
            .deleteFrom(getQualifiedSourceTable())
            .orderBy(order)
            .limit(limit);

    m_statementForCheckingSourceTableEmpty =
        StatementSelect().select(1).from(getSourceTable()).limit(1);
}

const StatementInsert &MigrationInfo::getStatementForMigration() const
{
    return m_statementForMigration;
}

const StatementDelete &
MigrationInfo::getStatementForDeleteingMigratedRow() const
{
    return m_statementForDeleteingMigratedRow;
}

const StatementSelect &
MigrationInfo::getStatementForCheckingSourceTableEmpty() const
{
    return m_statementForCheckingSourceTableEmpty;
}

StatementDropTable MigrationInfo::getStatementForDroppingOldTable() const
{
    return StatementDropTable()
        .dropTable(sourceTable)
        .withSchema(schema)
        .ifExists();
}

StatementSelect MigrationInfo::getStatementForGettingMaxRowID() const
{
    Expression maxRowID = Column("rowid").max();
    StatementSelect statementSelect =
        StatementSelect().select(maxRowID).from(getSourceTable());
    SelectCore selectionTargetMaxRowID = SelectCore().select(maxRowID).from(
        TableOrSubquery(targetTable)
            .withSchema(StatementAttach::getMainSchema()));
    statementSelect.union_(selectionTargetMaxRowID);
    return statementSelect;
}

SelectCore MigrationInfo::getSelectionForGettingSourceSequence() const
{
    return SelectCore()
        .select(Column("seq"))
        .from(TableOrSubquery("sqlite_sequence").withSchema(schema))
        .where(Column("name") == sourceTable);
}

SelectCore MigrationInfo::getSelectionForGettingTargetSequence() const
{
    return SelectCore()
        .select(Column("seq"))
        .from(TableOrSubquery("sqlite_sequence")
                  .withSchema(StatementAttach::getMainSchema()))
        .where(Column("name") == targetTable);
}

StatementInsert
MigrationInfo::getStatementForInsertingSequence(int sequence) const
{
    return StatementInsert()
        .insertInto("sqlite_sequence")
        .withSchema(StatementAttach::getMainSchema())
        .values({targetTable, sequence});
}

StatementUpdate
MigrationInfo::getStatementForUpdatingSequence(int sequence) const
{
    return StatementUpdate()
        .update(QualifiedTableName("sqlite_sequence")
                    .withSchema(StatementAttach::getMainSchema()))
        .set(Column("seq"), sequence)
        .where(Column("name") == targetTable);
}

StatementSelect MigrationInfo::getStatementForMergedSequence(
    const StatementSelect &statementSelect)
{
    Column seq("seq");
    std::string table = "merged";
    CommonTableExpression cte =
        CommonTableExpression(table).byAddingColumn(seq).as(statementSelect);
    return StatementSelect().with(cte).select(seq.max()).from(table);
}

} //namespace WCDB
