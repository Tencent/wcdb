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

MigrationUserInfo::MigrationUserInfo(const String& migratedTable)
: m_migratedTable(migratedTable)
{
    WCTInnerAssert(!migratedTable.empty());
}

MigrationUserInfo::~MigrationUserInfo()
{
}

bool MigrationUserInfo::shouldMigrate() const
{
    return !m_migratedTable.empty() && !m_originTable.empty();
}

bool MigrationUserInfo::isSameDatabaseMigration() const
{
    return m_originDatabase.empty();
}

const String& MigrationUserInfo::getMigratedTable() const
{
    return m_migratedTable;
}

const String& MigrationUserInfo::getOriginTable() const
{
    return m_originTable;
}

const String& MigrationUserInfo::getOriginDatabase() const
{
    return m_originDatabase;
}

void MigrationUserInfo::setOrigin(const String& table, const String& database)
{
    WCTInnerAssert(!table.empty());
    m_originTable = table;
    m_originDatabase = database;
}

String MigrationUserInfo::getDebugDescription() const
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

#pragma mark - Initialize
MigrationInfo::MigrationInfo(const MigrationUserInfo& userInfo, const std::set<String>& columns)
: MigrationUserInfo(userInfo)
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

        m_statementForMigratingRow
        = StatementInsert()
          .insertIntoTable(m_migratedTable)
          .schema(Schema::main())
          .columns(specificColumns)
          .values(StatementSelect()
                  .select(specificResultColumns)
                  .from(TableOrSubquery(m_originTable).schema(m_schemaForOriginDatabase))
                  .order(descendingRowid)
                  .limit(BindParameter(1)));

        m_statementForDeletingMigratedRow
        = StatementDelete()
          .deleteFrom(QualifiedTable(m_originTable).schema(m_schemaForOriginDatabase))
          .order(descendingRowid)
          .limit(BindParameter(1));

        m_statementForDroppingOriginTable = StatementDropTable()
                                            .dropTable(m_originTable)
                                            .schema(m_schemaForOriginDatabase)
                                            .ifExists();
    }
}

#pragma mark - Schema
const String& MigrationInfo::getSchemaPrefix()
{
    static const String* s_schemaPrefix = new String("WCDBMigration_");
    return *s_schemaPrefix;
}

const String& MigrationInfo::getUnionedView() const
{
    return m_unionedView;
}

Schema MigrationInfo::getSchemaForDatabase(const String& database)
{
    if (database.empty()) {
        return Schema::main();
    }
    return getSchemaPrefix() + std::to_string(database.hash());
}

const Schema& MigrationInfo::getSchemaForOriginDatabase() const
{
    return m_schemaForOriginDatabase;
}

const StatementAttach& MigrationInfo::getStatementForAttachingSchema() const
{
    WCTInnerAssert(!isSameDatabaseMigration());
    return m_statementForAttachingSchema;
}

const StatementDetach MigrationInfo::getStatementForDetachingSchema(const Schema& schema)
{
    WCTInnerAssert(schema.getDescription().hasPrefix(getSchemaPrefix()));
    return StatementDetach().detach(schema);
}

const StatementPragma MigrationInfo::getStatementForSelectingDatabaseList()
{
    return StatementPragma().pragma(Pragma::databaseList());
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

const StatementSelect MigrationInfo::getStatementForSelectingUnionedView()
{
    Column name("name");
    Column type("type");
    String pattern = String::formatted("%s%%", getUnionedViewPrefix().c_str());
    return StatementSelect()
    .select(name)
    .from(TableOrSubquery("sqlite_master").schema(Schema::temp()))
    .where(type == "view" && name.like(pattern));
}

#pragma mark - Migrate
const StatementDelete& MigrationInfo::getStatementForDeletingMigratedRow() const
{
    return m_statementForDeletingMigratedRow;
}

const StatementInsert& MigrationInfo::getStatementForMigratingRow() const
{
    return m_statementForMigratingRow;
}

const StatementDropTable& MigrationInfo::getStatementForDroppingOriginTable() const
{
    return m_statementForDroppingOriginTable;
}

} // namespace WCDB
