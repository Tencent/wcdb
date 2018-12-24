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

MigrationBaseInfo::MigrationBaseInfo(const String& migratedDatabase, const String& migratedTable)
: m_migratedDatabase(migratedDatabase), m_migratedTable(migratedTable)
{
    WCTInnerAssert(!m_migratedDatabase.empty());
    WCTInnerAssert(!m_migratedTable.empty());
}

MigrationBaseInfo::~MigrationBaseInfo()
{
}

bool MigrationBaseInfo::shouldMigrate() const
{
    return !m_migratedTable.empty() && !m_originTable.empty();
}

bool MigrationBaseInfo::isSameDatabaseMigration() const
{
    return m_originDatabase == m_migratedDatabase;
}

const String& MigrationBaseInfo::getMigratedTable() const
{
    return m_migratedTable;
}

const String& MigrationBaseInfo::getOriginTable() const
{
    return m_originTable;
}

const String& MigrationBaseInfo::getOriginDatabase() const
{
    return m_originDatabase;
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

String MigrationBaseInfo::getDebugDescription() const
{
    std::ostringstream stream;
    stream << "Migrated: " << m_migratedTable;
    if (!m_originTable.empty()) {
        stream << ", Origin: " << m_originTable;
    }
    if (!m_originDatabase.empty()) {
        stream << ", Database: " << m_originDatabase;
    }
    return stream.str();
}

#pragma mark - MigrationUserInfo
void MigrationUserInfo::setOrigin(const String& table, const String& database)
{
    WCTInnerAssert(!table.empty());
    m_originTable = table;
    if (!database.empty()) {
        m_originDatabase = database;
    } else {
        m_originDatabase = m_migratedDatabase;
    }
}

StatementAttach MigrationUserInfo::getStatementForAttachingSchema() const
{
    WCTInnerAssert(!isSameDatabaseMigration());
    return StatementAttach().attach(m_originDatabase).as(getSchemaForDatabase(m_originDatabase));
}

Schema MigrationUserInfo::getSchemaForOriginDatabase() const
{
    if (isSameDatabaseMigration()) {
        return Schema::main();
    }
    return getSchemaForDatabase(m_originDatabase);
}

#pragma mark - MigrationInfo
MigrationInfo::MigrationInfo(const MigrationUserInfo& userInfo, const std::set<String>& columns)
: MigrationBaseInfo(userInfo)
{
    WCTInnerAssert(shouldMigrate());

    // Schema
    if (!isSameDatabaseMigration()) {
        m_schemaForOriginDatabase = getSchemaForDatabase(m_originDatabase);

        m_statementForAttachingSchema
        = StatementAttach().attach(m_originDatabase).as(m_schemaForOriginDatabase);
    } else {
        m_schemaForOriginDatabase = Schema::main();
    }

    WCTInnerAssert(!columns.empty());

    // View
    {
        m_unionedView = getUnionedViewPrefix() + m_migratedTable + "_" + m_originTable;

        std::list<ResultColumn> resultColumns;
        resultColumns.push_back(Column::rowid());
        for (const auto& column : columns) {
            resultColumns.push_back(Column(column));
        }

        StatementSelect select
        = StatementSelect()
          .select(resultColumns)
          .from(TableOrSubquery(m_migratedTable).schema(Schema::main()));

        if (isSameDatabaseMigration()) {
            // UNION ALL has better performance, but it will trigger a bug of SQLite. See https://github.com/RingoD/SQLiteBugOfUnionAll for further information.
            select.unionAll();
        } else {
            select.union_();
        }

        select.select(resultColumns).from(TableOrSubquery(m_originTable).schema(m_schemaForOriginDatabase));

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
          .insertIntoTable(m_migratedTable)
          .schema(Schema::main())
          .columns(specificColumns)
          .values(StatementSelect()
                  .select(specificResultColumns)
                  .from(TableOrSubquery(m_originTable).schema(m_schemaForOriginDatabase))
                  .order(descendingRowid)
                  .limit(1));

        m_statementForMigratingSpecifiedRowTemplate
        = StatementInsert()
          .insertIntoTable(m_migratedTable)
          .schema(Schema::main())
          .columns(specificColumns)
          .values(StatementSelect()
                  .select(specificResultColumns)
                  .from(TableOrSubquery(m_originTable).schema(m_schemaForOriginDatabase))
                  .where(Column::rowid() == BindParameter(1)));

        m_statementForDeletingSpecifiedRow
        = StatementDelete()
          .deleteFrom(QualifiedTable(m_originTable).schema(m_schemaForOriginDatabase))
          .where(Column::rowid() == BindParameter(1));

        m_statementForDroppingOriginTable = StatementDropTable()
                                            .dropTable(m_originTable)
                                            .schema(m_schemaForOriginDatabase)
                                            .ifExists();
    }
}

#pragma mark - Schema
const Schema& MigrationInfo::getSchemaForOriginDatabase() const
{
    return m_schemaForOriginDatabase;
}

const String& MigrationInfo::getUnionedView() const
{
    return m_unionedView;
}

const StatementAttach& MigrationInfo::getStatementForAttachingSchema() const
{
    WCTInnerAssert(!isSameDatabaseMigration());
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
MigrationInfo::getStatementForMigratingSpecifiedRow(const Statement& originStatement) const
{
    WCTInnerAssert(originStatement.getType() == Syntax::Identifier::Type::InsertSTMT);
    StatementInsert statement = m_statementForMigratingSpecifiedRowTemplate;
    const Syntax::InsertSTMT* syntax
    = static_cast<Syntax::InsertSTMT*>(originStatement.getSyntaxIdentifier());
    statement.syntax().useConflictAction = syntax->useConflictAction;
    statement.syntax().conflictAction = syntax->conflictAction;
    return statement;
}

StatementUpdate
MigrationInfo::getStatementForLimitedUpdatingTable(const Statement& originStatement) const
{
    WCTInnerAssert(originStatement.getType() == Syntax::Identifier::Type::UpdateSTMT);
    StatementUpdate statementUpdate(originStatement);

    Syntax::UpdateSTMT& updateSyntax = statementUpdate.syntax();
    StatementSelect select
    = StatementSelect().select(Column::rowid()).from(updateSyntax.table.table);

    Syntax::SelectSTMT& selectSyntax = select.syntax();
    WCTInnerAssert(!selectSyntax.cores.empty());
    Syntax::SelectCore& coreSyntax = selectSyntax.cores.back();

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
MigrationInfo::getStatementForLimitedDeletingFromTable(const Statement& originStatement) const
{
    WCTInnerAssert(originStatement.getType() == Syntax::Identifier::Type::DeleteSTMT);
    StatementDelete statementDelete(originStatement);

    Syntax::DeleteSTMT& deleteSyntax = statementDelete.syntax();
    StatementSelect select
    = StatementSelect().select(Column::rowid()).from(deleteSyntax.table.table);

    Syntax::SelectSTMT& selectSyntax = select.syntax();
    WCTInnerAssert(!selectSyntax.cores.empty());
    Syntax::SelectCore& coreSyntax = selectSyntax.cores.back();

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

const StatementDropTable& MigrationInfo::getStatementForDroppingOriginTable() const
{
    return m_statementForDroppingOriginTable;
}

} // namespace WCDB
