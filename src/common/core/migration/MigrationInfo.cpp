//
// Created by sanhuazhang on 2019/05/02
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

#include "MigrationInfo.hpp"
#include "Assertion.hpp"
#include "StringView.hpp"

namespace WCDB {

const char* MigrationDatabaseInfo::getSchemaPrefix()
{
    static const char* s_schemaPrefix = "wcdb_migration_";
    return s_schemaPrefix;
}

#pragma mark - MigrationDatabaseInfo
MigrationDatabaseInfo::MigrationDatabaseInfo(const UnsafeStringView& path,
                                             const UnsafeData& cipher,
                                             const TableFilter& filter)
: m_sourcePath(path)
, m_cipher(cipher)
, m_filter(filter)
, m_needRawCipher(!cipher.empty())
{
    if (!m_sourcePath.empty()) {
        std::ostringstream stream;
        stream << getSchemaPrefix() << m_sourcePath.hash();
        m_schema = stream.str();
        m_statementForAttachingSchema = StatementAttach().attach(m_sourcePath).as(m_schema);
        if (!cipher.empty()) {
            m_statementForAttachingSchema.key(BindParameter(1));
        }
    } else {
        m_schema = Schema::main();
    }
}

bool MigrationDatabaseInfo::isCrossDatabase() const
{
    return !m_sourcePath.empty();
}

const StringView& MigrationDatabaseInfo::getSourceDatabase() const
{
    return m_sourcePath;
}

Data MigrationDatabaseInfo::getCipher() const
{
    SharedLockGuard lockGoard(m_lock);
    return m_cipher;
}

void MigrationDatabaseInfo::setRawCipher(const UnsafeData& rawCipher) const
{
    LockGuard lockGuard(m_lock);
    m_cipher = rawCipher;
    m_needRawCipher = false;
}

bool MigrationDatabaseInfo::needRawCipher() const
{
    return m_needRawCipher;
}

const MigrationDatabaseInfo::TableFilter& MigrationDatabaseInfo::getFilter() const
{
    return m_filter;
}

const Schema& MigrationDatabaseInfo::getSchemaForSourceDatabase() const
{
    return m_schema;
}

const StatementAttach& MigrationDatabaseInfo::getStatementForAttachingSchema() const
{
    return m_statementForAttachingSchema;
}

#pragma mark - MigrationBaseInfo
MigrationBaseInfo::MigrationBaseInfo(MigrationDatabaseInfo& databaseInfo,
                                     const UnsafeStringView& table)
: m_databaseInfo(databaseInfo), m_table(table)
{
    WCTAssert(!m_table.empty());
}

MigrationBaseInfo::~MigrationBaseInfo() = default;

const StringView& MigrationBaseInfo::getTable() const
{
    return m_table;
}

const StringView& MigrationBaseInfo::getSourceTable() const
{
    return m_sourceTable;
}

const Schema& MigrationBaseInfo::getSchemaForSourceDatabase() const
{
    return m_databaseInfo.getSchemaForSourceDatabase();
}

const StatementAttach& MigrationBaseInfo::getStatementForAttachingSchema() const
{
    return m_databaseInfo.getStatementForAttachingSchema();
}

void MigrationBaseInfo::setSource(const UnsafeStringView& table)
{
    WCTRemedialAssert(!table.empty() && (table != m_table || isCrossDatabase()),
                      "Invalid migration source.",
                      return;);
    m_sourceTable = table;
}

bool MigrationBaseInfo::shouldMigrate() const
{
    return !m_sourceTable.empty();
}

bool MigrationBaseInfo::isCrossDatabase() const
{
    return m_databaseInfo.isCrossDatabase();
}

const StringView& MigrationBaseInfo::getSourceDatabase() const
{
    return m_databaseInfo.getSourceDatabase();
}

Data MigrationBaseInfo::getSourceCipher() const
{
    return m_databaseInfo.getCipher();
}

void MigrationBaseInfo::setRawSourceCipher(const UnsafeData& rawCipher) const
{
    return m_databaseInfo.setRawCipher(rawCipher);
}

bool MigrationBaseInfo::needRawSourceCipher() const
{
    return m_databaseInfo.needRawCipher();
}

#pragma mark - MigrationUserInfo
MigrationUserInfo::~MigrationUserInfo() = default;

#pragma mark - MigrationInfo
MigrationInfo::MigrationInfo(const MigrationUserInfo& userInfo,
                             const std::set<StringView>& uniqueColumns,
                             bool autoincrement,
                             const UnsafeStringView& integerPrimaryKey)
: MigrationBaseInfo(userInfo)
, m_autoincrement(autoincrement)
, m_integerPrimaryKey(integerPrimaryKey)
{
    WCTAssert(!uniqueColumns.empty());

    Column rowid = Column::rowid();
    Columns columns;
    columns.push_back(rowid);
    columns.insert(columns.end(), uniqueColumns.begin(), uniqueColumns.end());

    ResultColumns resultColumns;
    resultColumns.insert(resultColumns.begin(), columns.begin(), columns.end());

    const Schema& sourceSchema = m_databaseInfo.getSchemaForSourceDatabase();

    TableOrSubquery sourceTableQuery
    = TableOrSubquery(getSourceTable()).schema(sourceSchema);
    QualifiedTable qualifiedSourceTable
    = QualifiedTable(getSourceTable()).schema(sourceSchema);

    // View
    {
        std::ostringstream stream;
        stream << getUnionedViewPrefix() << getTable();
        m_unionedView = StringView(stream.str());

        m_statementForCreatingUnionedView = StatementCreateView()
                                            .createView(m_unionedView)
                                            .temp()
                                            .ifNotExists()
                                            .columns(columns)
                                            .as(StatementSelect()
                                                .select(resultColumns)
                                                .from(TableOrSubquery(getTable()))
                                                .unionAll()
                                                .select(resultColumns)
                                                .from(sourceTableQuery));
    }

    // Migrate
    {
        OrderingTerm migrateOrder = m_integerPrimaryKey.empty() ?
                                    OrderingTerm(rowid).order(Order::DESC) :
                                    OrderingTerm(m_integerPrimaryKey).order(Order::DESC);

        m_statementForMigratingOneRow = StatementInsert()
                                        .insertIntoTable(getTable())
                                        .orReplace()
                                        .columns(columns)
                                        .values(StatementSelect()
                                                .select(resultColumns)
                                                .from(sourceTableQuery)
                                                .order(migrateOrder)
                                                .limit(1));

        m_statementForDeletingMigratedOneRow = StatementDelete()
                                               .deleteFrom(qualifiedSourceTable)
                                               .orders(migrateOrder)
                                               .limit(1);
    }

    // Compatible
    {
        if (m_integerPrimaryKey.empty()) {
            m_statementForSelectingMaxID
            = StatementSelect()
              .select(rowid.max() + 1)
              .from(TableOrSubquery(m_table).schema(Schema::main()));
        } else if (!autoincrement) {
            m_statementForSelectingMaxID
            = StatementSelect()
              .select(Column(m_integerPrimaryKey).max() + 1)
              .from(TableOrSubquery(m_unionedView).schema(Schema::temp()));
        }

        m_statementForDeletingSpecifiedRow
        = StatementDelete().deleteFrom(qualifiedSourceTable).where(rowid == BindParameter(1));

        m_statementForDroppingSourceTable
        = StatementDropTable().dropTable(getSourceTable()).schema(sourceSchema).ifExists();
    }
}

MigrationInfo::~MigrationInfo() = default;

bool MigrationInfo::isAutoIncrement() const
{
    return m_autoincrement;
}

const StringView& MigrationInfo::getIntegerPrimaryKey() const
{
    return m_integerPrimaryKey;
}

#pragma mark - Schema

const StringView& MigrationInfo::getUnionedView() const
{
    return m_unionedView;
}

StatementDetach MigrationInfo::getStatementForDetachingSchema(const Schema& schema)
{
    WCTAssert(schema.getDescription().hasPrefix(MigrationDatabaseInfo::getSchemaPrefix()));
    return StatementDetach().detach(schema);
}

StatementPragma MigrationInfo::getStatementForSelectingDatabaseList()
{
    return StatementPragma().pragma(Pragma::databaseList());
}

#pragma mark - View
const char* MigrationInfo::getUnionedViewPrefix()
{
    return "wcdb_union_";
}

const StatementCreateView& MigrationInfo::getStatementForCreatingUnionedView() const
{
    return m_statementForCreatingUnionedView;
}

StatementCreateView
MigrationInfo::getStatementForCreatingUnionedView(const Columns& columns) const
{
    StatementCreateView createView = getStatementForCreatingUnionedView();
    Columns newColumns = columns;
    newColumns.push_back(Column::rowid());
    createView.columns(newColumns);
    ResultColumns resultColumns;
    resultColumns.insert(resultColumns.begin(), newColumns.begin(), newColumns.end());
    createView.syntax().select.getOrCreate().select.getOrCreate().resultColumns = resultColumns;
    for (auto& core : createView.syntax().select.getOrCreate().cores) {
        core.resultColumns = resultColumns;
    }
    return createView;
}

const StatementDropView
MigrationInfo::getStatementForDroppingUnionedView(const UnsafeStringView& unionedView)
{
    WCTAssert(unionedView.hasPrefix(getUnionedViewPrefix()));
    return StatementDropView().dropView(unionedView).schema(Schema::temp()).ifExists();
}

StatementSelect MigrationInfo::getStatementForSelectingUnionedView()
{
    Column name("name");
    Column type("type");
    StringView pattern = StringView::formatted("%s%%", getUnionedViewPrefix());
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

StatementInsert MigrationInfo::getStatementForMigrating(const Syntax::InsertSTMT& stmt) const
{
    StatementInsert statement(stmt);

    auto& syntax = statement.syntax();
    syntax.schema = Schema::main();
    syntax.table = getTable();
    WCTAssert(!syntax.isMultiWrite());
    if (m_autoincrement) {
        return statement;
    }
    auto& columns = syntax.columns;
    WCTAssert(!columns.empty());
    columns.insert(columns.end(), Column("rowid"));

    if (!syntax.expressionsValues.empty()) {
        auto& expressions = syntax.expressionsValues;
        WCTAssert(expressions.size() == 1);
        auto& values = *expressions.begin();
        Expression rowid = BindParameter(getIndexOfRowIdOrPrimaryKey());
        values.insert(values.end(), rowid);
    }
    return statement;
}

const StatementSelect& MigrationInfo::getStatementForSelectingMaxID() const
{
    return m_statementForSelectingMaxID;
}

int MigrationInfo::getIndexOfRowIdOrPrimaryKey()
{
    return SQLITE_MAX_VARIABLE_NUMBER;
}

std::pair<StatementSelect, StatementUpdate>
MigrationInfo::getStatementsForLimitedUpdatingTable(const Statement& sourceStatement) const
{
    WCTAssert(sourceStatement.getType() == Syntax::Identifier::Type::UpdateSTMT);
    StatementUpdate statementUpdate((const StatementUpdate&) sourceStatement);

    Syntax::UpdateSTMT& updateSyntax = statementUpdate.syntax();
    StatementSelect select
    = StatementSelect().from(TableOrSubquery(getUnionedView()).schema(Schema::temp()));
    if (m_integerPrimaryKey.empty()) {
        select.select(Column::rowid());
    } else {
        select.select(m_integerPrimaryKey);
    }

    Syntax::SelectSTMT& selectSyntax = select.syntax();
    Syntax::SelectCore& coreSyntax = selectSyntax.select.getOrCreate();

    coreSyntax.condition = updateSyntax.condition;
    updateSyntax.condition.getOrCreate().__is_valid = Syntax::Identifier::invalid;

    selectSyntax.orderingTerms = updateSyntax.orderingTerms;
    updateSyntax.orderingTerms.clear();

    selectSyntax.limit = updateSyntax.limit;
    selectSyntax.limitParameterType = updateSyntax.limitParameterType;
    selectSyntax.limitParameter = updateSyntax.limitParameter;
    updateSyntax.limit.getOrCreate().__is_valid = Syntax::Identifier::invalid;

    if (m_integerPrimaryKey.empty()) {
        statementUpdate.where(Column::rowid()
                              == BindParameter(getIndexOfRowIdOrPrimaryKey()));
    } else {
        statementUpdate.where(Column(m_integerPrimaryKey)
                              == BindParameter(getIndexOfRowIdOrPrimaryKey()));
    }

    return { select, statementUpdate };
}

std::pair<StatementSelect, StatementDelete>
MigrationInfo::getStatementsForLimitedDeletingFromTable(const Statement& sourceStatement) const
{
    WCTAssert(sourceStatement.getType() == Syntax::Identifier::Type::DeleteSTMT);
    StatementDelete statementDelete((const StatementDelete&) sourceStatement);

    Syntax::DeleteSTMT& deleteSyntax = statementDelete.syntax();
    StatementSelect select
    = StatementSelect().from(TableOrSubquery(getUnionedView()).schema(Schema::temp()));
    if (m_integerPrimaryKey.empty()) {
        select.select(Column::rowid());
    } else {
        select.select(m_integerPrimaryKey);
    }

    Syntax::SelectSTMT& selectSyntax = select.syntax();
    Syntax::SelectCore& coreSyntax = selectSyntax.select.getOrCreate();

    coreSyntax.condition = deleteSyntax.condition;
    deleteSyntax.condition.getOrCreate().__is_valid = Syntax::Identifier::invalid;

    selectSyntax.orderingTerms = deleteSyntax.orderingTerms;
    deleteSyntax.orderingTerms.clear();

    selectSyntax.limit = deleteSyntax.limit;
    selectSyntax.limitParameterType = deleteSyntax.limitParameterType;
    selectSyntax.limitParameter = deleteSyntax.limitParameter;
    deleteSyntax.limit.getOrCreate().__is_valid = Syntax::Identifier::invalid;

    statementDelete.where(Column::rowid().in(select));

    if (m_integerPrimaryKey.empty()) {
        statementDelete.where(Column::rowid()
                              == BindParameter(getIndexOfRowIdOrPrimaryKey()));
    } else {
        statementDelete.where(Column(m_integerPrimaryKey)
                              == BindParameter(getIndexOfRowIdOrPrimaryKey()));
    }

    return { select, statementDelete };
}

StatementDelete
MigrationInfo::getStatementForDeletingFromTable(const Statement& sourceStatement) const
{
    WCTAssert(sourceStatement.getType() == Syntax::Identifier::Type::DropTableSTMT);

    const Syntax::DropTableSTMT& dropTableSyntax
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
