//
// Created by qiuwenchen on 2022/8/27.
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

#include "BaseBinding.hpp"
#include "Assertion.hpp"
#include "CompressionConst.hpp"
#include "DecorativeHandle.hpp"
#include "InnerHandle.hpp"
#include "MigratingHandleDecorator.hpp"
#include "Notifier.hpp"
#include "WCDBError.hpp"
#include <algorithm>
#include <string>

namespace WCDB {

BaseBinding::BaseBinding() = default;

BaseBinding::~BaseBinding() = default;

#pragma mark - Column Def
const CaseInsensitiveList<ColumnDef> &BaseBinding::getColumnDefs() const
{
    return m_columnDefs;
}

ColumnDef *BaseBinding::getColumnDef(const UnsafeStringView &columnName)
{
    ColumnDef *columnDef = nullptr;
    auto iter = m_columnDefs.caseInsensitiveFind(columnName);
    if (iter != m_columnDefs.end()) {
        columnDef = &iter->second;
    }
    return columnDef;
}

const ColumnDef *BaseBinding::getColumnDef(const UnsafeStringView &columnName) const
{
    const ColumnDef *columnDef = nullptr;
    auto iter = m_columnDefs.caseInsensitiveFind(columnName);
    if (iter != m_columnDefs.end()) {
        columnDef = &iter->second;
    }
    return columnDef;
}

void BaseBinding::enableAutoIncrementForExistingTable()
{
    m_enableAutoIncrementForExistingTable = true;
}

bool BaseBinding::configAutoincrementIfNeed(const UnsafeStringView &tableName,
                                            InnerHandle *handle) const
{
    auto tableConfig = handle->getTableAttribute(Schema::main(), tableName);
    if (!tableConfig.succeed()) {
        return false;
    }
    if (tableConfig.value().autoincrement) {
        return true;
    }
    if (tableConfig.value().withoutRowid) {
        Error error(Error::Code::Error,
                    Error::Level::Error,
                    "Without rowid table can not be configed as autoincrement");
        error.infos.insert_or_assign("Table", tableName);
        error.infos.insert_or_assign(ErrorStringKeyPath, handle->getPath());
        Notifier::shared().notify(error);
        return false;
    }
    const StringView &integerPrimaryKey = tableConfig.value().integerPrimaryKey;
    if (integerPrimaryKey.length() == 0) {
        Error error(Error::Code::Error, Error::Level::Error, "No integer primary key found");
        error.infos.insert_or_assign("Table", tableName);
        error.infos.insert_or_assign(ErrorStringKeyPath, handle->getPath());
        Notifier::shared().notify(error);
        return false;
    }
    WCTAssert(m_columnDefs.caseInsensitiveFind(integerPrimaryKey)
              != m_columnDefs.end());

    bool ret
    = handle->execute(StatementPragma().pragma(Pragma::writableSchema()).to(true));

    ret = ret && updateMasterTable(tableName, handle);

    ret = ret && updateSequeceTable(tableName, integerPrimaryKey, handle);

    ret = ret && handle->configAutoIncrement(tableName);

    handle->execute(StatementPragma().pragma(Pragma::writableSchema()).to(false));

    return ret;
}

bool BaseBinding::updateMasterTable(const UnsafeStringView &tableName, InnerHandle *handle) const
{
    StatementSelect select
    = StatementSelect()
      .select(Column("sql"))
      .from(Syntax::masterTable)
      .where(Column("name") == tableName && Column("type") == "table");
    auto sqls = handle->getValues(select, 0);
    if (sqls.failed()) {
        return false;
    }
    if (sqls.value().size() != 1) {
        Error error(Error::Code::Error, Error::Level::Error, "Can not read sql");
        error.infos.insert_or_assign("Table", tableName);
        error.infos.insert_or_assign(ErrorStringKeyPath, handle->getPath());
        Notifier::shared().notify(error);
        return false;
    }

    std::string sql = *sqls.value().begin();
    std::transform(sql.begin(), sql.end(), sql.begin(), ::toupper);

    size_t startPos = sql.find(" PRIMARY ");
    if (startPos == std::string::npos) {
        reportSqlParseFail(*sqls.value().begin(), tableName, handle);
        return false;
    }

    size_t endPos = sql.find(",", startPos);
    if (endPos == std::string::npos) {
        endPos = sql.find(")", startPos);
    }
    if (endPos == std::string::npos) {
        reportSqlParseFail(*sqls.value().begin(), tableName, handle);
        return false;
    }

    endPos = std::min(endPos, sql.find("NOT", startPos));
    endPos = std::min(endPos, sql.find("UNIQUE", startPos));
    endPos = std::min(endPos, sql.find("CHECK", startPos));
    endPos = std::min(endPos, sql.find("DEFAULT", startPos));
    endPos = std::min(endPos, sql.find("COLLATE", startPos));
    endPos = std::min(endPos, sql.find("GENERATED", startPos));
    endPos = std::min(endPos, sql.find("AS", startPos));

    std::string originSql = *sqls.value().begin();
    originSql.insert(endPos, " AUTOINCREMENT ");

    ConfiguredHandle memoryHandle;
    memoryHandle.setPath(":memory:");
    if (!memoryHandle.open()) {
        return false;
    }
    if (!memoryHandle.prepare(originSql)) {
        reportSqlParseFail(*sqls.value().begin(), tableName, handle);
        return false;
    }
    memoryHandle.finalize();
    memoryHandle.close();

    StatementUpdate update
    = StatementUpdate()
      .update(Syntax::masterTable)
      .set(Column("sql"))
      .to(originSql)
      .where(Column("name") == tableName && Column("type") == "table");

    return handle->execute(update);
}

void BaseBinding::reportSqlParseFail(const UnsafeStringView &sql,
                                     const UnsafeStringView &tableName,
                                     InnerHandle *handle) const
{
    Error error(Error::Code::Error, Error::Level::Error, "Can not parse create table sql");
    error.infos.insert_or_assign("Table", tableName);
    error.infos.insert_or_assign(ErrorStringKeyPath, handle->getPath());
    error.infos.insert_or_assign(ErrorStringKeySQL, sql);
    Notifier::shared().notify(error);
}

bool BaseBinding::updateSequeceTable(const UnsafeStringView &tableName,
                                     const UnsafeStringView &integerPrimaryKey,
                                     InnerHandle *handle) const
{
    StatementCreateTable createSeq
    = StatementCreateTable().createTable(Syntax::sequenceTable).ifNotExists();
    createSeq.define(ColumnDef("name"));
    createSeq.define(ColumnDef("seq"));
    if (!handle->execute(createSeq)) {
        return false;
    }
    if (!handle->prepare(
        StatementSelect().select(Column(integerPrimaryKey).max()).from(tableName))) {
        return false;
    }
    if (!handle->step()) {
        handle->finalize();
        return false;
    }
    int64_t maxRowid = 0;
    bool hasContent = false;
    if (!handle->done()) {
        maxRowid = handle->getInteger();
        hasContent = true;
    }
    handle->finalize();
    if (hasContent) {
        StatementInsert insert = StatementInsert().insertIntoTable(Syntax::sequenceTable);
        insert.column("name").column("seq");
        insert.value(tableName).value(std::max(maxRowid, (int64_t) 0));
        return handle->execute(insert);
    }
    return true;
}

#pragma mark - Table
bool BaseBinding::createTable(const UnsafeStringView &tableName, InnerHandle *handle) const
{
    WCTAssert(handle != nullptr);
    handle->markErrorNotAllowedWithinTransaction();
    return handle->runTransactionIfNotInTransaction([&](InnerHandle *handle) {
        auto exists = handle->tableExists(tableName);
        if (!exists.succeed()) {
            return false;
        }
        if (exists.value()) {
            auto optionalColumnNames = handle->getColumns(Schema::main(), tableName);
            if (!optionalColumnNames.succeed()) {
                return false;
            }
            StringViewSet &columnNames = optionalColumnNames.value();
            //Check whether the column names exists
            const auto &columnDefs = getColumnDefs();
            for (const auto &columnDef : columnDefs) {
                auto iter = columnNames.find(columnDef.first);
                if (iter == columnNames.end()) {
                    //Add new column
                    if (!handle->addColumn(Schema::main(), tableName, columnDef.second)) {
                        return false;
                    }
                } else {
                    columnNames.erase(iter);
                }
            }
            for (const auto &columnName : columnNames) {
                if (columnName.hasPrefix(CompressionColumnTypePrefix)) {
                    continue;
                }
                Error error(Error::Code::Mismatch, Error::Level::Notice, "Skip column");
                error.infos.insert_or_assign("Table", StringView(tableName));
                error.infos.insert_or_assign("Column", StringView(columnName));
                error.infos.insert_or_assign(ErrorStringKeyPath, handle->getPath());
                Notifier::shared().notify(error);
            }
            if (m_enableAutoIncrementForExistingTable
                && !configAutoincrementIfNeed(tableName, handle)) {
                return false;
            }
        } else {
            if (!handle->execute(generateCreateTableStatement(tableName))) {
                return false;
            }
        }
        std::list<StatementCreateIndex> createIndexStatements;
        std::list<StatementDropIndex> dropIndexStatements;
        std::tie(createIndexStatements, dropIndexStatements)
        = generateIndexStatements(tableName, !exists.value());
        for (const StatementCreateIndex &statement : createIndexStatements) {
            if (!handle->execute(statement)) {
                return false;
            }
        }
        for (const StatementDropIndex &statement : dropIndexStatements) {
            if (!handle->execute(statement)) {
                return false;
            }
        }
        return true;
    });
}

bool BaseBinding::createVirtualTable(const UnsafeStringView &tableName, InnerHandle *handle) const
{
    WCTAssert(handle != nullptr);
    return handle->execute(generateCreateVirtualTableStatement(tableName));
}

StatementCreateTable
BaseBinding::generateCreateTableStatement(const UnsafeStringView &tableName) const
{
    StatementCreateTable statement = statementTable;
    statement.createTable(tableName).ifNotExists();
    for (const auto &iter : m_columnDefs) {
        statement.define(iter.second);
    }
    for (const auto &constraint : m_constraints) {
        statement.constraint(constraint.second);
    }
    return statement;
}

StatementCreateVirtualTable
BaseBinding::generateCreateVirtualTableStatement(const UnsafeStringView &tableName) const
{
    StatementCreateVirtualTable statement = statementVirtualTable;
    statement.createVirtualTable(tableName).ifNotExists();
    std::list<StringView> &arguments = statement.syntax().arguments;
    bool isFTS5 = statement.syntax().module.caseInsensitiveEqual("fts5");
    for (const auto &iter : m_columnDefs) {
        if (isFTS5) {
            bool added = false;
            for (auto &constrain : iter.second.syntax().constraints) {
                if (constrain.switcher == WCDB::Syntax::ColumnConstraint::Switch::UnIndexed) {
                    arguments.push_back(StringView().formatted(
                    "%s %s",
                    iter.second.syntax().column.getOrCreate().getDescription().data(),
                    constrain.getDescription().data()));
                    added = true;
                    break;
                }
            }
            // FTS5 does not need type
            if (!added) {
                arguments.push_back(iter.second.syntax().column.getOrCreate().getDescription());
            }
        } else {
            arguments.push_back(iter.second.getDescription());
        }
    }
    return statement;
}

bool BaseBinding::tryRecoverColumn(const UnsafeStringView &columnName,
                                   const UnsafeStringView &tableName,
                                   const UnsafeStringView &schemaName,
                                   const UnsafeStringView &sql,
                                   InnerHandle *handle) const
{
    if (m_columnDefs.caseInsensitiveFind(columnName) == m_columnDefs.end()) {
        return false;
    }
    auto exist = handle->tableExists(schemaName, tableName);
    if (exist.failed() || !exist.value()) {
        return false;
    }
    auto optionalColumnNames = handle->getColumns(schemaName, tableName);
    if (!optionalColumnNames.succeed()) {
        return false;
    }
    StringViewSet &columnNames = optionalColumnNames.value();

    const auto &columnDefs = getColumnDefs();
    int matchCount = 0;
    for (const auto &columnDef : columnDefs) {
        if (columnNames.find(columnDef.first) != columnNames.end()) {
            matchCount++;
        }
    }
    if (matchCount != columnDefs.size() - 1 && matchCount < (columnDefs.size() + 1) / 2) {
        return false;
    }

    for (const auto &columnDef : columnDefs) {
        if (columnNames.find(columnDef.first) != columnNames.end()) {
            continue;
        }
        if (columnDef.second.syntax().isPrimaryKey()
            || columnDef.second.syntax().isUnique()) {
            continue;
        }
        //Add new column
        if (handle->addColumn(schemaName, tableName, columnDef.second)) {
            Error error(Error::Code::Warning, Error::Level::Warning, "Auto add column");
            error.infos.insert_or_assign(
            "Column", columnDef.second.syntax().column.getOrCreate().name);
            error.infos.insert_or_assign("Table", tableName);
            if (schemaName.length() > 0) {
                error.infos.insert_or_assign("Schema", schemaName);
            }
            error.infos.insert_or_assign(ErrorStringKeySQL, sql);
            error.infos.insert_or_assign(ErrorStringKeyPath, handle->getPath());
            Notifier::shared().notify(error);
        } else {
            return false;
        }
    }
    DecorativeHandle *decorativeHandle = dynamic_cast<DecorativeHandle *>(handle);
    if (decorativeHandle != nullptr
        && decorativeHandle->containDecorator(DecoratorMigratingHandle)) {
        MigratingHandleDecorator *decorator
        = decorativeHandle->getDecorator<MigratingHandleDecorator>(DecoratorMigratingHandle);
        Columns columns;
        for (const auto &columnDef : columnDefs) {
            columns.push_back(columnDef.second.syntax().column.getOrCreate().name);
        }
        return decorator->rebindUnionView(tableName, columns);
    }
    return true;
}

#pragma mark - Table Constraint
TableConstraint &BaseBinding::getOrCreateTableConstraint(const UnsafeStringView &name)
{
    auto iter = m_constraints.find(name);
    if (iter == m_constraints.end()) {
        iter = m_constraints.emplace(name, TableConstraint(name)).first;
    }
    return iter->second;
}

#pragma mark - Index

BaseBinding::Index::Index(const UnsafeStringView &suffix_, bool isFullName)
: nameOrSuffix(suffix_), isFullName(isFullName), action(Action::Create)
{
}

StringView BaseBinding::Index::getIndexName(const UnsafeStringView &tableName)
{
    if (!isFullName) {
        StringView closingQuotation;
        if (tableName.length() > 2) {
            if (tableName.hasPrefix("'") && tableName.hasSuffix("'")) {
                closingQuotation = "'";
            } else if (tableName.hasPrefix("\"") && tableName.hasSuffix("\"")) {
                closingQuotation = "\"";
            } else if (tableName.hasPrefix("[") && tableName.hasSuffix("]")) {
                closingQuotation = "]";
            } else if (tableName.hasPrefix("`") && tableName.hasSuffix("`")) {
                closingQuotation = "`";
            }
        }
        std::ostringstream stream;
        if (closingQuotation.length() == 0) {
            stream << tableName << nameOrSuffix;
        } else {
            stream << UnsafeStringView(tableName.data(), tableName.length() - 1);
            stream << nameOrSuffix;
            stream << closingQuotation;
        }
        return stream.str();
    } else {
        return nameOrSuffix;
    }
}

BaseBinding::Index &
BaseBinding::getOrCreateIndex(const UnsafeStringView &nameOrSuffix, bool isFullName)
{
    auto iter = m_indexes.find(nameOrSuffix);
    if (iter == m_indexes.end()) {
        iter = m_indexes.emplace(nameOrSuffix, Index(nameOrSuffix, isFullName)).first;
    }
    WCTAssert(iter->first == iter->second.nameOrSuffix);
    return iter->second;
}

std::pair<std::list<StatementCreateIndex>, std::list<StatementDropIndex>>
BaseBinding::generateIndexStatements(const UnsafeStringView &tableName, bool isTableNewlyCreated) const
{
    std::pair<std::list<StatementCreateIndex>, std::list<StatementDropIndex>> pairs;
    for (const auto &iter : m_indexes) {
        WCTAssert(iter.first == iter.second.nameOrSuffix);
        Index index = iter.second;
        switch (index.action) {
        case Index::Action::CreateForNewlyCreatedTableOnly:
            if (!isTableNewlyCreated) {
                break;
            }
            // fallthrough
        case Index::Action::Create: {
            StatementCreateIndex statement = index.statement;
            statement.createIndex(index.getIndexName(tableName)).ifNotExists().table(tableName);
            pairs.first.push_back(statement);
        } break;
        default:
            WCTAssert(index.action == Index::Action::Drop);
            StatementDropIndex statement
            = StatementDropIndex().dropIndex(index.getIndexName(tableName)).ifExists();
            pairs.second.push_back(statement);
            break;
        }
    }
    return pairs;
}

} //namespace WCDB
