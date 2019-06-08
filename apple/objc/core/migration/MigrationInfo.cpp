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
#include <WCDB/MigrationInfo.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - MigrationBaseInfo
MigrationBaseInfo::MigrationBaseInfo()
{
}

MigrationBaseInfo::MigrationBaseInfo(const String& database, const String& table)
: m_database(database), m_table(table)
{
    WCTInnerAssert(!m_database.empty());
    WCTInnerAssert(!m_table.empty());
}

MigrationBaseInfo::~MigrationBaseInfo()
{
}

bool MigrationBaseInfo::shouldMigrate() const
{
    return !m_table.empty() && !m_sourceTable.empty();
}

bool MigrationBaseInfo::isCrossDatabase() const
{
    return m_sourceDatabase != m_database;
}

const String& MigrationBaseInfo::getTable() const
{
    return m_table;
}

const String& MigrationBaseInfo::getDatabase() const
{
    return m_database;
}

const String& MigrationBaseInfo::getSourceTable() const
{
    return m_sourceTable;
}

const String& MigrationBaseInfo::getSourceDatabase() const
{
    return m_sourceDatabase;
}

const char* MigrationBaseInfo::getSchemaPrefix()
{
    static const char* s_schemaPrefix = "WCDBMigration_";
    return s_schemaPrefix;
}

Schema MigrationBaseInfo::getSchemaForDatabase(const String& database)
{
    return getSchemaPrefix() + std::to_string(database.hash());
}

void MigrationBaseInfo::setSource(const String& table, const String& database)
{
    WCTInnerAssert(!table.empty());
    m_sourceTable = table;
    if (!database.empty()) {
        m_sourceDatabase = database;
    } else {
        m_sourceDatabase = m_database;
    }
}

#pragma mark - MigrationUserInfo
StatementAttach MigrationUserInfo::getStatementForAttachingSchema() const
{
    WCTInnerAssert(isCrossDatabase());
    return StatementAttach().attach(getSourceDatabase()).as(getSchemaForDatabase(getSourceDatabase()));
}

Schema MigrationUserInfo::getSchemaForSourceDatabase() const
{
    if (isCrossDatabase()) {
        return getSchemaForDatabase(getSourceDatabase());
    }
    return Schema::main();
}

#pragma mark - MigrationInfo
MigrationInfo::MigrationInfo(const MigrationUserInfo& userInfo,
                             const std::set<String>& uniqueColumns,
                             bool integerPrimaryKey)
: MigrationBaseInfo(userInfo), m_integerPrimaryKey(integerPrimaryKey)
{
    WCTInnerAssert(shouldMigrate());
    WCTInnerAssert(!uniqueColumns.empty());

    // Schema
    if (isCrossDatabase()) {
        m_schemaForSourceDatabase = getSchemaForDatabase(getSourceDatabase());

        m_statementForAttachingSchema
        = StatementAttach().attach(getSourceDatabase()).as(m_schemaForSourceDatabase);
    } else {
        m_schemaForSourceDatabase = Schema::main();
    }

    Columns columns;
    columns.push_back(Column::rowid());
    columns.insert(columns.end(), uniqueColumns.begin(), uniqueColumns.end());

    ResultColumns resultColumns;
    resultColumns.insert(resultColumns.begin(), columns.begin(), columns.end());

    // View
    {
        m_unionedView = getUnionedViewPrefix() + getTable();

        StatementSelect select
        = StatementSelect()
          .select(resultColumns)
          .from(TableOrSubquery(getTable()).schema(Schema::main()));

        if (isCrossDatabase()) {
            // UNION ALL has better performance, but it will trigger a bug of SQLite. See https://github.com/RingoD/SQLiteBugOfUnionAll for further information.
            select.union_();
        } else {
            select.unionAll();
        }

        select.select(resultColumns)
        .from(TableOrSubquery(getSourceTable()).schema(m_schemaForSourceDatabase))
        .order(OrderingTerm(Column::rowid()).order(Order::ASC));

        m_statementForCreatingUnionedView = StatementCreateView()
                                            .createView(m_unionedView)
                                            .temp()
                                            .ifNotExists()
                                            .columns(columns)
                                            .as(select);
    }

    // Migrate
    {
        OrderingTerm descendingRowid = OrderingTerm(Column::rowid()).order(Order::DESC);

        m_statementForMigratingOneRow
        = StatementInsert()
          .insertIntoTable(getTable())
          .orReplace()
          .schema(Schema::main())
          .columns(columns)
          .values(StatementSelect()
                  .select(resultColumns)
                  .from(TableOrSubquery(getSourceTable()).schema(m_schemaForSourceDatabase))
                  .order(descendingRowid)
                  .limit(1));

        m_statementForDeletingMigratedOneRow
        = StatementDelete()
          .deleteFrom(QualifiedTable(getSourceTable()).schema(m_schemaForSourceDatabase))
          .order(descendingRowid)
          .limit(1);
    }

    // Compatible
    {
        ResultColumns resultColumnsForRowIDCompatible = resultColumns;
        if (!m_integerPrimaryKey) {
            Column maxRowIDColumn("maxRowID");

            ResultColumn maxRowIDResultColumn = Column::rowid().max();
            maxRowIDResultColumn.as(maxRowIDColumn.getDescription());

            ResultColumn expectingRowID = maxRowIDColumn.max() + 1;
            expectingRowID.as(Column::rowid().getDescription());

            resultColumnsForRowIDCompatible.front()
            = StatementSelect()
              .select(expectingRowID)
              .from(StatementSelect()
                    .select(maxRowIDResultColumn)
                    .from(TableOrSubquery(getSourceTable()).schema(m_schemaForSourceDatabase))
                    .unionAll()
                    .select(maxRowIDResultColumn)
                    .from(TableOrSubquery(getTable()).schema(Schema::main())));
        }

        m_statementForMigratingSpecifiedRowTemplate
        = StatementInsert()
          .insertIntoTable(getTable())
          .schema(Schema::main())
          .columns(columns)
          .values(StatementSelect()
                  .select(resultColumnsForRowIDCompatible)
                  .from(TableOrSubquery(getSourceTable()).schema(m_schemaForSourceDatabase))
                  .where(Column::rowid() == BindParameter(1)));

        m_statementForDeletingSpecifiedRow
        = StatementDelete()
          .deleteFrom(QualifiedTable(getSourceTable()).schema(m_schemaForSourceDatabase))
          .where(Column::rowid() == BindParameter(1));

        m_statementForDroppingSourceTable = StatementDropTable()
                                            .dropTable(getSourceTable())
                                            .schema(m_schemaForSourceDatabase)
                                            .ifExists();
    }
}

#pragma mark - Schema
const Schema& MigrationInfo::getSchemaForSourceDatabase() const
{
    return m_schemaForSourceDatabase;
}

const String& MigrationInfo::getUnionedView() const
{
    return m_unionedView;
}

const StatementAttach& MigrationInfo::getStatementForAttachingSchema() const
{
    WCTInnerAssert(isCrossDatabase());
    return m_statementForAttachingSchema;
}

StatementDetach MigrationInfo::getStatementForDetachingSchema(const Schema& schema)
{
    WCTInnerAssert(schema.getDescription().hasPrefix(getSchemaPrefix()));
    return StatementDetach().detach(schema);
}

StatementPragma MigrationInfo::getStatementForSelectingDatabaseList()
{
    return StatementPragma().pragma(Pragma::databaseList()).schema(Schema::main());
}

#pragma mark - View
const String& MigrationInfo::getUnionedViewPrefix()
{
    static const String* s_viewPrefix = new String("WCDBUnioned_");
    return *s_viewPrefix;
}

const StatementCreateView& MigrationInfo::getStatementForCreatingUnionedView() const
{
    return m_statementForCreatingUnionedView;
}

const StatementDropView
MigrationInfo::getStatementForDroppingUnionedView(const String& unionedView)
{
    WCTInnerAssert(unionedView.hasPrefix(getUnionedViewPrefix()));
    return StatementDropView().dropView(unionedView).schema(Schema::temp()).ifExists();
}

StatementSelect MigrationInfo::getStatementForSelectingUnionedView()
{
    Column name("name");
    Column type("type");
    String pattern = String::formatted("%s%%", getUnionedViewPrefix().c_str());
    return StatementSelect()
    .select(name)
    .from(TableOrSubquery::master().schema(Schema::temp()))
    .where(type == "view" && name.like(pattern));
}

#pragma mark - Migrate
const StatementInsert& MigrationInfo::getStatementForMigratingOneRow() const
{
    return m_statementForMigratingOneRow;
}

const StatementDelete& MigrationInfo::getStatementForDeletingMigratedOneRow() const
{
    return m_statementForDeletingMigratedOneRow;
}

const StatementDelete& MigrationInfo::getStatementForDeletingSpecifiedRow() const
{
    return m_statementForDeletingSpecifiedRow;
}

StatementInsert
MigrationInfo::getStatementForMigratingSpecifiedRow(Syntax::ConflictAction conflictAction) const
{
    StatementInsert statement = m_statementForMigratingSpecifiedRowTemplate;
    statement.syntax().conflictAction = conflictAction;
    return statement;
}

StatementUpdate
MigrationInfo::getStatementForLimitedUpdatingTable(const Statement& sourceStatement) const
{
    WCTInnerAssert(sourceStatement.getType() == Syntax::Identifier::Type::UpdateSTMT);
    StatementUpdate statementUpdate(sourceStatement);

    Syntax::UpdateSTMT& updateSyntax = statementUpdate.syntax();
    StatementSelect select
    = StatementSelect()
      .select(Column::rowid())
      .from(TableOrSubquery(getUnionedView()).schema(Schema::temp()));

    Syntax::SelectSTMT& selectSyntax = select.syntax();
    Syntax::SelectCore& coreSyntax = selectSyntax.select;

    coreSyntax.condition = updateSyntax.condition;
    updateSyntax.condition.__valid = Syntax::Identifier::invalid;

    selectSyntax.orderingTerms = updateSyntax.orderingTerms;
    updateSyntax.orderingTerms.clear();

    selectSyntax.limit = updateSyntax.limit;
    selectSyntax.limitParameterType = updateSyntax.limitParameterType;
    selectSyntax.limitParameter = updateSyntax.limitParameter;
    updateSyntax.limit.__valid = Syntax::Identifier::invalid;

    statementUpdate.where(Column::rowid().in(select));

    return statementUpdate;
}

StatementDelete
MigrationInfo::getStatementForLimitedDeletingFromTable(const Statement& sourceStatement) const
{
    WCTInnerAssert(sourceStatement.getType() == Syntax::Identifier::Type::DeleteSTMT);
    StatementDelete statementDelete(sourceStatement);

    Syntax::DeleteSTMT& deleteSyntax = statementDelete.syntax();
    StatementSelect select
    = StatementSelect()
      .select(Column::rowid())
      .from(TableOrSubquery(getUnionedView()).schema(Schema::temp()));

    Syntax::SelectSTMT& selectSyntax = select.syntax();
    Syntax::SelectCore& coreSyntax = selectSyntax.select;

    coreSyntax.condition = deleteSyntax.condition;
    deleteSyntax.condition.__valid = Syntax::Identifier::invalid;

    selectSyntax.orderingTerms = deleteSyntax.orderingTerms;
    deleteSyntax.orderingTerms.clear();

    selectSyntax.limit = deleteSyntax.limit;
    selectSyntax.limitParameterType = deleteSyntax.limitParameterType;
    selectSyntax.limitParameter = deleteSyntax.limitParameter;
    deleteSyntax.limit.__valid = Syntax::Identifier::invalid;

    statementDelete.where(Column::rowid().in(select));

    return statementDelete;
}

StatementDelete
MigrationInfo::getStatementForDeletingFromTable(const Statement& sourceStatement) const
{
    WCTInnerAssert(sourceStatement.getType() == Syntax::Identifier::Type::DropTableSTMT);

    Syntax::DropTableSTMT& dropTableSyntax
    = ((const StatementDropTable&) sourceStatement).syntax();
    WCDB::QualifiedTable table(dropTableSyntax.table);
    table.syntax().schema = dropTableSyntax.schema;

    return StatementDelete().deleteFrom(table);
}

const StatementDropTable& MigrationInfo::getStatementForDroppingSourceTable() const
{
    return m_statementForDroppingSourceTable;
}

} // namespace WCDB
