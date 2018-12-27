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

#pragma mark - MigrationUserInfo
void MigrationUserInfo::setSource(const String& table, const String& database)
{
    WCTInnerAssert(!table.empty());
    m_sourceTable = table;
    if (!database.empty()) {
        m_sourceDatabase = database;
    } else {
        m_sourceDatabase = m_database;
    }
}

StatementAttach MigrationUserInfo::getStatementForAttachingSchema() const
{
    WCTInnerAssert(isCrossDatabase());
    return StatementAttach().attach(m_sourceDatabase).as(getSchemaForDatabase(m_sourceDatabase));
}

Schema MigrationUserInfo::getSchemaForSourceDatabase() const
{
    if (isCrossDatabase()) {
        return getSchemaForDatabase(m_sourceDatabase);
    }
    return Schema::main();
}

#pragma mark - MigrationInfo
MigrationInfo::MigrationInfo(const MigrationUserInfo& userInfo, const std::set<String>& columns)
: MigrationBaseInfo(userInfo)
{
    WCTInnerAssert(shouldMigrate());

    // Schema
    if (isCrossDatabase()) {
        m_schemaForSourceDatabase = getSchemaForDatabase(m_sourceDatabase);

        m_statementForAttachingSchema
        = StatementAttach().attach(m_sourceDatabase).as(m_schemaForSourceDatabase);
    } else {
        m_schemaForSourceDatabase = Schema::main();
    }

    WCTInnerAssert(!columns.empty());

    // View
    {
        m_unionedView = getUnionedViewPrefix() + m_table + "_" + m_sourceTable;

        std::list<ResultColumn> resultColumns;
        resultColumns.push_back(ResultColumn(Column::rowid()).as("rowid"));
        for (const auto& column : columns) {
            resultColumns.push_back(Column(column));
        }

        StatementSelect select
        = StatementSelect().select(resultColumns).from(TableOrSubquery(m_table).schema(Schema::main()));

        if (isCrossDatabase()) {
            // UNION ALL has better performance, but it will trigger a bug of SQLite. See https://github.com/RingoD/SQLiteBugOfUnionAll for further information.
            select.union_();
        } else {
            select.unionAll();
        }

        select.select(resultColumns)
        .from(TableOrSubquery(m_sourceTable).schema(m_schemaForSourceDatabase))
        .order(OrderingTerm(Column::rowid()).order(Order::ASC));

        m_statementForCreatingUnionedView
        = StatementCreateView().createView(m_unionedView).temp().ifNotExists().as(select);
    }

    // Migrate
    {
        std::list<Column> specificColumns;
        std::list<ResultColumn> specificResultColumns;
        specificColumns.push_back(Column::rowid());
        specificResultColumns.push_back(Column::rowid());
        for (const auto& column : columns) {
            Column specificColumn(column);
            specificColumns.push_back(specificColumn);
            specificResultColumns.push_back(specificColumn);
        }

        OrderingTerm descendingRowid = OrderingTerm(Column::rowid()).order(Order::DESC);

        m_statementForMigratingOneRow
        = StatementInsert()
          .insertIntoTable(m_table)
          .schema(Schema::main())
          .columns(specificColumns)
          .values(StatementSelect()
                  .select(specificResultColumns)
                  .from(TableOrSubquery(m_sourceTable).schema(m_schemaForSourceDatabase))
                  .order(descendingRowid)
                  .limit(1));

        m_statementForMigratingSpecifiedRowTemplate
        = StatementInsert()
          .insertIntoTable(m_table)
          .schema(Schema::main())
          .columns(specificColumns)
          .values(StatementSelect()
                  .select(specificResultColumns)
                  .from(TableOrSubquery(m_sourceTable).schema(m_schemaForSourceDatabase))
                  .where(Column::rowid() == BindParameter(1)));

        m_statementForDeletingSpecifiedRow
        = StatementDelete()
          .deleteFrom(QualifiedTable(m_sourceTable).schema(m_schemaForSourceDatabase))
          .where(Column::rowid() == BindParameter(1));

        m_statementForDroppingSourceTable = StatementDropTable()
                                            .dropTable(m_sourceTable)
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

const StatementDelete& MigrationInfo::getStatementForDeletingSpecifiedRow() const
{
    return m_statementForDeletingSpecifiedRow;
}

StatementInsert
MigrationInfo::getStatementForMigratingSpecifiedRow(bool useConflictAction,
                                                    Syntax::ConflictAction conflictAction) const
{
    StatementInsert statement = m_statementForMigratingSpecifiedRowTemplate;
    statement.syntax().useConflictAction = useConflictAction;
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

    coreSyntax.useCondition = updateSyntax.useCondition;
    coreSyntax.condition = updateSyntax.condition;
    updateSyntax.useCondition = false;

    selectSyntax.orderingTerms = updateSyntax.orderingTerms;
    updateSyntax.orderingTerms.clear();

    selectSyntax.useLimit = updateSyntax.useLimit;
    selectSyntax.limit = updateSyntax.limit;
    selectSyntax.limitParameterType = updateSyntax.limitParameterType;
    selectSyntax.limitParameter = updateSyntax.limitParameter;
    updateSyntax.useLimit = false;

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

    coreSyntax.useCondition = deleteSyntax.useCondition;
    coreSyntax.condition = deleteSyntax.condition;
    deleteSyntax.useCondition = false;

    selectSyntax.orderingTerms = deleteSyntax.orderingTerms;
    deleteSyntax.orderingTerms.clear();

    selectSyntax.useLimit = deleteSyntax.useLimit;
    selectSyntax.limit = deleteSyntax.limit;
    selectSyntax.limitParameterType = deleteSyntax.limitParameterType;
    selectSyntax.limitParameter = deleteSyntax.limitParameter;
    deleteSyntax.useLimit = false;

    statementDelete.where(Column::rowid().in(select));

    return statementDelete;
}

const StatementDropTable& MigrationInfo::getStatementForDroppingSourceTable() const
{
    return m_statementForDroppingSourceTable;
}

} // namespace WCDB
