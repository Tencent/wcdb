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

MigrationUserInfo::MigrationUserInfo(const std::string& migratedTable)
: m_migratedTable(migratedTable)
{
    WCTInnerAssert(!migratedTable.empty());
}

bool MigrationUserInfo::shouldMigrate() const
{
    return !m_migratedTable.empty() && !m_originTable.empty();
}

bool MigrationUserInfo::isSameDatabaseMigration() const
{
    return m_originDatabase.empty();
}

const std::string& MigrationUserInfo::getMigratedTable() const
{
    return m_migratedTable;
}

const std::string& MigrationUserInfo::getOriginTable() const
{
    return m_originTable;
}

const std::string& MigrationUserInfo::getOriginDatabase() const
{
    return m_originDatabase;
}

const std::string& MigrationUserInfo::getSchemaForOriginDatabase() const
{
    return m_schemaForOriginDatabase;
}

void MigrationUserInfo::setOrigin(const std::string& table, const std::string& database)
{
    WCTInnerAssert(!table.empty());
    m_originTable = table;
    m_originDatabase = database;
    if (isSameDatabaseMigration()) {
        m_schemaForOriginDatabase = Schema::main();
    } else {
        m_schemaForOriginDatabase
        = getSchemaPrefix() + std::to_string(String::hash(m_originDatabase));
    }
}

const std::string& MigrationUserInfo::getSchemaPrefix()
{
    static const std::string* s_schemaPrefix = new std::string("WCDBMigration_");
    return *s_schemaPrefix;
}

std::string MigrationUserInfo::getDebugDescription() const
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
MigrationInfo::MigrationInfo(const MigrationUserInfo& userInfo,
                             const std::set<std::string>& columns)
: MigrationUserInfo(userInfo)
{
    WCTInnerAssert(shouldMigrate());

    // Schema
    if (!isSameDatabaseMigration()) {
        m_statementForAttachingSchema
        = StatementAttach().attach(m_originDatabase).as(m_schemaForOriginDatabase);

        m_statementForDetachingSchema = StatementDetach().detach(m_schemaForOriginDatabase);
    }

    WCTInnerAssert(!columns.empty());

    // View
    {
        m_unionedView = "WCDBUnioned_" + m_migratedTable + "_" + m_originTable;

        std::list<ResultColumn> resultColumns;
        resultColumns.push_back(Column::rowid());
        for (const auto& column : columns) {
            resultColumns.push_back(Column(column));
        }

        StatementSelect migratedSelection
        = StatementSelect()
          .select(resultColumns)
          .from(TableOrSubquery(m_migratedTable).as(Schema::main()));

        SelectCore originSelection
        = SelectCore().select(resultColumns).from(TableOrSubquery(m_originTable).as(m_schemaForOriginDatabase));

        // UNION ALL has better performance, but it will trigger a bug of SQLite. See https://github.com/RingoD/SQLiteBugOfUnionAll for further information.
        StatementSelect selection;
        if (isSameDatabaseMigration()) {
            selection = migratedSelection.unionAll(originSelection);
        } else {
            selection = migratedSelection.union_(originSelection);
        }

        m_statementForCreatingUnionedView = StatementCreateView()
                                            .createView(m_unionedView)
                                            .withSchema(Schema::main())
                                            .ifNotExists()
                                            .as(selection);

        m_statementForDroppingUnionedView
        = StatementDropView().dropView(m_unionedView).withSchema(Schema::main()).ifExists();
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

        m_statementForMigratingRow
        = StatementInsert()
          .insertInto(m_migratedTable)
          .withSchema(Schema::main())
          .on(specificColumns)
          .values(StatementSelect()
                  .select(specificResultColumns)
                  .from(TableOrSubquery(m_originTable).withSchema(m_schemaForOriginDatabase)));

        m_statementForDeletingMigratedRow
        = StatementDelete()
          .deleteFrom(QualifiedTableName(m_originTable).withSchema(m_schemaForOriginDatabase))
          .limit(BindParameter(1));

        m_statementForDroppingOriginTable = StatementDropTable()
                                            .dropTable(m_originTable)
                                            .withSchema(m_schemaForOriginDatabase)
                                            .ifExists();
    }
}

#pragma mark - Schema
const StatementAttach& MigrationInfo::getStatementForAttachingSchema() const
{
    WCTInnerAssert(!isSameDatabaseMigration());
    return m_statementForAttachingSchema;
}

const StatementDetach& MigrationInfo::getStatementForDetachingSchema() const
{
    WCTInnerAssert(!isSameDatabaseMigration());
    return m_statementForDetachingSchema;
}

#pragma mark - View
const StatementCreateView& MigrationInfo::getStatementForCreatingUnionedView() const
{
    return m_statementForCreatingUnionedView;
}

const StatementDropView& MigrationInfo::getStatementForDroppingUnionedView() const
{
    return m_statementForDroppingUnionedView;
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
