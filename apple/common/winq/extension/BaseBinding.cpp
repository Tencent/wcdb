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

#import <WCDB/Assertion.hpp>
#include <WCDB/BaseBinding.hpp>
#import <WCDB/Error.hpp>

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
                if (constrain.getDescription().find("UNINDEXED") == 0) {
                    arguments.push_back(StringView().formatted(
                    "%s %s",
                    iter.second.syntax().column.getDescription().data(),
                    constrain.getDescription().data()));
                    added = true;
                    break;
                }
            }
            // FTS5 does not need type
            if (!added) {
                arguments.push_back(iter.second.syntax().column.getDescription());
            }
        } else {
            arguments.push_back(iter.second.getDescription());
        }
    }
    return statement;
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
