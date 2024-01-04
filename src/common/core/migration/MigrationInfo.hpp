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

#pragma once

#include "Lock.hpp"
#include "StringView.hpp"
#include "WINQ.h"
#include <set>

namespace WCDB {

class MigrationUserInfo;

#pragma mark - MigrationDatabaseInfo
class MigrationDatabaseInfo {
public:
    typedef std::function<void(MigrationUserInfo&)> TableFilter;

    MigrationDatabaseInfo(const UnsafeStringView& path,
                          const UnsafeData& cipher,
                          const TableFilter& filter);

    bool isCrossDatabase() const;

    static const char* getSchemaPrefix();
    const StringView& getSourceDatabase() const;

    Data getCipher() const;
    void setRawCipher(const UnsafeData& rawCipher) const;
    bool needRawCipher() const;

    const TableFilter& getFilter() const;

    const Schema& getSchemaForSourceDatabase() const;
    const StatementAttach& getStatementForAttachingSchema() const;

private:
    StringView m_sourcePath;
    mutable Data m_cipher;
    TableFilter m_filter;

    mutable bool m_needRawCipher;
    mutable SharedLock m_lock;

    // wcdb_migration_ + hash(m_sourcePath)
    Schema m_schema;
    StatementAttach m_statementForAttachingSchema;
};

#pragma mark - MigrationBaseInfo
class MigrationBaseInfo {
public:
    MigrationBaseInfo();
    MigrationBaseInfo(MigrationDatabaseInfo& databaseInfo, const UnsafeStringView& table);
    virtual ~MigrationBaseInfo() = 0;

    const StringView& getTable() const;
    const StringView& getSourceTable() const;
    const Expression& getFilterCondition() const;

    bool shouldMigrate() const;
    bool isCrossDatabase() const;
    const StringView& getSourceDatabase() const;

    Data getSourceCipher() const;
    void setRawSourceCipher(const UnsafeData& rawCipher) const;
    bool needRawSourceCipher() const;

    const Schema& getSchemaForSourceDatabase() const;
    const StatementAttach& getStatementForAttachingSchema() const;

protected:
    void setSource(const UnsafeStringView& table);
    void setFilter(Expression filterCondition);
    void tryFallbackToSourceTable(Syntax::Schema& schema, StringView& table) const;

    MigrationDatabaseInfo& m_databaseInfo;
    StringView m_table;
    StringView m_sourceTable;
    Expression m_filterCondition;
};

#pragma mark - MigrationUserInfo
class MigrationUserInfo final : public MigrationBaseInfo {
public:
    using MigrationBaseInfo::MigrationBaseInfo;
    ~MigrationUserInfo() override;

    using MigrationBaseInfo::setSource;
    using MigrationBaseInfo::setFilter;
};

#pragma mark - MigrationInfo
class MigrationInfo final : public MigrationBaseInfo {
public:
    MigrationInfo(const MigrationUserInfo& userInfo,
                  const StringViewSet& columns,
                  bool autoincrement,
                  const UnsafeStringView& integerPrimaryKey);
    ~MigrationInfo() override;

    bool isAutoIncrement() const;
    const StringView& getIntegerPrimaryKey() const;
    bool needUpdateSequance() const;
    void setNeedUpdateSequence(bool needUpdate) const;

protected:
    bool m_autoincrement;
    StringView m_integerPrimaryKey;
    mutable bool m_needUpdateSequence;

#pragma mark - Schema
public:
    /*
     DETACH [schemaForSourceDatabase]
     */
    static StatementDetach getStatementForDetachingSchema(const Schema& schema);

    /*
     PRAGMA main.database_list
     */
    static StatementPragma getStatementForSelectingDatabaseList();

#pragma mark - View
public:
    // wcdb_union_
    static const char* getUnionedViewPrefix();

    const StringView& getUnionedView() const;

    /*
     CREATE TEMP VIEW IF NOT EXISTS [unionedView] AS
     SELECT rowid, [columns]
     FROM [schemaForSourceDatabase].[sourceTable]
     UNION/UNION ALL
     SELECT rowid, [columns]
     FROM main.[table]
     ORDER BY rowid ASC
     */
    const StatementCreateView& getStatementForCreatingUnionedView() const;
    StatementCreateView getStatementForCreatingUnionedView(const Columns& columns) const;

    /*
     DROP VIEW IF EXISTS temp.[unionedView]
     */
    static const StatementDropView
    getStatementForDroppingUnionedView(const UnsafeStringView& unionedView);

    /*
     SELECT name
     FROM temp.sqlite_master
     WHERE type == "view" AND name LIKE "wcdb_union_%"
     */
    static StatementSelect getStatementForSelectingUnionedView();

protected:
    // wcdb_union_ + [table]
    StringView m_unionedView;
    StatementCreateView m_statementForCreatingUnionedView;

#pragma mark - Compatible
public:
    /*
     Firstly,
     
     INSERT INTO [schemaForSourceDatabase].[sourceTable]([columns]) VALUES (...)
     DELETE FROM [schemaForSourceDatabase].[sourceTable] WHERE rowid == ?1
     
     And then
     
     1. for the tables with autoincrement integer primary key:
     
        INSERT INTO main.targetTable([columns]) VALUES (...)
     
     2. for the tables with integer primary key:
     
        INSERT INTO main.targetTable([columns], rowid) VALUES (..., ?rowidIndex)
     
        Note that newRowid is
        (1) the primmay key assigned from statement
        (2) or SELECT max(primary key)+1 FROM temp.[unionedView]
     
     3. for the tables with normal primary key or no primary key:
     
        INSERT INTO main.targetTable([columns], rowid) VALUES (..., ?rowidIndex)
     
        Note that newRowid is SELECT max(rowid)+1 FROM temp.[unionedView]
     */
    void generateStatementsForInsertMigrating(const Statement& sourceStatement,
                                              std::list<Statement>& statements,
                                              int& primaryKeyIndex,
                                              int rowidBindIndex,
                                              Optional<int64_t>& assignedPrimaryKey) const;

    /*
     SELECT [rowid/primary key] FROM temp.[unionedView] WHERE ... ORDER BY ... LIMIT ... OFFSET ...
     UPDATE [schemaForSourceDatabase].[sourceTable] SET ... TO ... WHERE [rowid/primary key] == ?index
     UPDATE main.targetTable SET ... TO ... WHERE [rowid/primary key] == ?index
     
     For the tables with integer primary key, it uses primary key. For the other tables, it uses rowid.
     */
    void generateStatementsForUpdateMigrating(const Statement& sourceStatement,
                                              std::list<Statement>& statements,
                                              int rowidBindIndex) const;

    /*
     SELECT [rowid/primary key] FROM temp.[unionedView] WHERE ... ORDER BY ... LIMIT ... OFFSET ...
     DELETE FROM [schemaForSourceDatabase].[sourceTable] WHERE [rowid/primary key] == ?index
     DELETE FROM main.targetTable WHERE [rowid/primary key] == ?index
     
     For the tables with integer primary key, it uses primary key. For the other tables, it uses rowid.
     */
    void generateStatementsForDeleteMigrating(const Statement& sourceStatement,
                                              std::list<Statement>& statements,
                                              int rowidBindIndex) const;

    StatementDelete getStatementForDeletingFromTable(const Statement& sourceStatement) const;

protected:
    StatementDelete m_statementForDeletingSpecifiedRow;
    StatementSelect m_statementForSelectingMaxID;

#pragma mark - Migrate
public:
    /*
     INSERT [columns]
     INTO rowid, main.[table]
     SELECT rowid, [columns]
     FROM [schemaForSourceDatabase].[sourceTable]
     ORDER BY [rowid/primary key] DESC
     LIMIT 1
     
     For the tables with integer primary key, it uses primary key. For the other tables, it uses rowid.
     */
    const StatementInsert& getStatementForMigratingOneRow() const;

    /*
     DELETE FROM [schemaForSourceDatabase].[sourceTable]
     ORDER BY [rowid/primary key] DESC
     LIMIT 1
     
     For the tables with integer primary key, it uses primary key. For the other tables, it uses rowid.
     */
    const StatementDelete& getStatementForDeletingMigratedOneRow() const;

    /*
     SELECT * FROM [schemaForSourceDatabase].[sourceTable] LIMIT 1
     */
    const StatementSelect& getStatementForSelectingAnyRowFromSourceTable() const;

    /*
     DROP TABLE IF EXISTS [schemaForSourceDatabase].[sourceTable]
     */
    const StatementDropTable& getStatementForDroppingSourceTable() const;

protected:
    StatementInsert m_statementForMigratingOneRow;
    StatementDelete m_statementForDeletingMigratedOneRow;
    StatementDropTable m_statementForDroppingSourceTable;
    StatementSelect m_statementForSelectingAnyRowFromSourceTable;
};

} // namespace WCDB
