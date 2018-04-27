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
{
    //setup reusable statement
    OrderingTerm order = OrderingTerm(Column::rowid()).withOrder(Order::DESC);
    Expression limit = 10; //TODO opti the step of migration

    m_statementForMigration = StatementInsert()
                                  .insertOrIgnoreInto(targetTable)
                                  .values(StatementSelect()
                                              .select(ResultColumn::all())
                                              .from(getSourceTable())
                                              .orderBy(order)
                                              .limit(limit));

    m_statementForDeletingMigratedRow =
        StatementDelete()
            .deleteFrom(getQualifiedSourceTable())
            .orderBy(order)
            .limit(limit);

    m_statementForDeletingMigratedRow =
        StatementDelete()
            .deleteFrom(getQualifiedSourceTable())
            .where(Column::rowid() == BindParameter(1));

    m_statementForInsertingLastInsertedSourceIntoTargetTable =
        StatementInsert()
            .insertInto(targetTable)
            .values(
                StatementSelect()
                    .select(ResultColumn::all())
                    .from(getSourceTable())
                    .where(Column::rowid() == Expression::lastInsertedRowid()));
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
        return String::empty();
    }
    return MigrationInfo::schemaPrefix() +
           std::to_string(std::hash<std::string>{}(path));
}

bool MigrationInfo::isSameDatabaseMigration() const
{
    return schema.empty();
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
const StatementInsert &MigrationInfo::getStatementForMigration() const
{
    return m_statementForMigration;
}

const StatementDelete &
MigrationInfo::getStatementForDeleteingMigratedRow() const
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

const std::string &MigrationInfo::getUnionedViewPrefix()
{
    static const std::string s_unionedViewPrefix("WCDBUnioned");
    return s_unionedViewPrefix;
}

StatementCreateView MigrationInfo::getStatementForCreatingUnionedView() const
{
    return StatementCreateView()
        .createView(unionedViewName)
        .ifNotExists()
        .as(StatementSelect()
                .select(ResultColumn::all())
                .from(targetTable)
                .union_(SelectCore()
                            .select(ResultColumn::all())
                            .from(getSourceTable())));
}

StatementDropView MigrationInfo::getStatementForDroppingUnionedView() const
{
    return StatementDropView().dropView(unionedViewName).ifExists();
}

const StatementDelete &
MigrationInfo::getStatementForDeletingLastInsertedSourceRow() const
{
    return m_statementForDeletingMigratedRow;
}

StatementInsert
MigrationInfo::getStatementForInsertingLastInsertedSourceIntoTargetTable(
    const StatementInsert &origin) const
{
    StatementInsert statement =
        m_statementForInsertingLastInsertedSourceIntoTargetTable;
    statement.getCOWLang().get_or_copy<Lang::InsertSTMT>().type =
        origin.getCOWLang().get<Lang::InsertSTMT>().type;
    return statement;
}

} //namespace WCDB
