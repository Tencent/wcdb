//
// Created by 陈秋文 on 2022/8/27.
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
#include "Error.hpp"
#include "InnerHandle.hpp"
#include "MigratingHandle.hpp"
#include "Notifier.hpp"

namespace WCDB {

BaseBinding::BaseBinding() = default;

BaseBinding::~BaseBinding() = default;

#pragma mark - Column Def
const CaseInsensiveList<ColumnDef> &BaseBinding::getColumnDefs() const
{
    return m_columnDefs;
}

ColumnDef *BaseBinding::getColumnDef(const UnsafeStringView &columnName)
{
    ColumnDef *columnDef = nullptr;
    auto iter = m_columnDefs.caseInsensiveFind(columnName);
    if (iter != m_columnDefs.end()) {
        columnDef = &iter->second;
    }
    return columnDef;
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
            auto optionalColumnNames = handle->getColumns(tableName);
            if (!optionalColumnNames.succeed()) {
                return false;
            }
            std::set<StringView> &columnNames = optionalColumnNames.value();
            //Check whether the column names exists
            const auto &columnDefs = getColumnDefs();
            for (const auto &columnDef : columnDefs) {
                auto iter = columnNames.find(columnDef.first);
                if (iter == columnNames.end()) {
                    //Add new column
                    if (!handle->addColumn(Schema(), tableName, columnDef.second)) {
                        return false;
                    }
                } else {
                    columnNames.erase(iter);
                }
            }
            for (const auto &columnName : columnNames) {
                Error error(Error::Code::Mismatch, Error::Level::Notice, "Skip column");
                error.infos.insert_or_assign("Table", StringView(tableName));
                error.infos.insert_or_assign("Column", StringView(columnName));
                error.infos.insert_or_assign(ErrorStringKeyPath, handle->getPath());
                Notifier::shared().notify(error);
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
    bool isFTS5 = statement.syntax().module.caseInsensiveEqual("fts5");
    for (const auto &iter : m_columnDefs) {
        if (isFTS5) {
            bool added = false;
            for (auto constrain : iter.second.syntax().constraints) {
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
    if (m_columnDefs.caseInsensiveFind(columnName) == m_columnDefs.end()) {
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
    std::set<StringView> &columnNames = optionalColumnNames.value();

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
    MigratingHandle *migratingHandle = dynamic_cast<MigratingHandle *>(handle);
    if (migratingHandle != nullptr) {
        Columns columns;
        for (const auto &columnDef : columnDefs) {
            columns.push_back(columnDef.second.syntax().column.getOrCreate().name);
        }
        return migratingHandle->rebindUnionView(tableName, columns);
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

BaseBinding::Index::Index(const UnsafeStringView &suffix_)
: suffix(suffix_), action(Action::Create)
{
}

BaseBinding::Index &BaseBinding::getOrCreateIndex(const UnsafeStringView &suffix)
{
    auto iter = m_indexes.find(suffix);
    if (iter == m_indexes.end()) {
        iter = m_indexes.emplace(suffix, Index(suffix)).first;
    }
    WCTAssert(iter->first == iter->second.suffix);
    return iter->second;
}

std::pair<std::list<StatementCreateIndex>, std::list<StatementDropIndex>>
BaseBinding::generateIndexStatements(const UnsafeStringView &tableName, bool isTableNewlyCreated) const
{
    std::pair<std::list<StatementCreateIndex>, std::list<StatementDropIndex>> pairs;
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

    for (const auto &iter : m_indexes) {
        WCTAssert(iter.first == iter.second.suffix);
        Index index = iter.second;
        switch (index.action) {
        case Index::Action::CreateForNewlyCreatedTableOnly:
            if (!isTableNewlyCreated) {
                break;
            }
            // fallthrough
        case Index::Action::Create: {
            StatementCreateIndex statement = index.statement;
            std::ostringstream stream;
            if (closingQuotation.length() == 0) {
                stream << tableName << index.suffix;
            } else {
                stream << UnsafeStringView(tableName.data(), tableName.length() - 1);
                stream << index.suffix;
                stream << closingQuotation;
            }
            statement.createIndex(StringView(stream.str())).ifNotExists().table(tableName);
            pairs.first.push_back(statement);
        } break;
        default:
            WCTAssert(index.action == Index::Action::Drop);
            std::ostringstream stream;
            stream << tableName << index.suffix;
            StatementDropIndex statement
            = StatementDropIndex().dropIndex(StringView(stream.str())).ifExists();
            pairs.second.push_back(statement);
            break;
        }
    }
    return pairs;
}

} //namespace WCDB
