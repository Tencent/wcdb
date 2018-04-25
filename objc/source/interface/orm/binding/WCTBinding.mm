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

#import <WCDB/Error.hpp>
#import <WCDB/WCTBinding.h>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTProperty.h>
#import <objc/runtime.h>

WCTBinding &WCTBinding::bindingWithClass(Class cls)
{
    static std::map<Class, WCTBinding> s_bindings;
    static std::mutex s_mutex;
    std::lock_guard<std::mutex> lockGuard(s_mutex);
    auto iter = s_bindings.find(cls);
    if (iter == s_bindings.end()) {
        iter = s_bindings.insert({cls, WCTBinding(cls)}).first;
    }
    return iter->second;
}

WCTBinding::WCTBinding(Class cls)
    : m_cls(cls)
{
    if (!class_conformsToProtocol(m_cls, @protocol(WCTTableCoding))) {
        class_addProtocol(m_cls, @protocol(WCTTableCoding));
    }
}

WCDB::StatementCreateTable WCTBinding::generateCreateTableStatement(const std::string &tableName) const
{
    WCDB::StatementCreateTable statement = WCDB::StatementCreateTable().createTable(tableName);
    for (const auto &columnBinding : m_columnBindings) {
        statement.define(columnBinding.second.columnDef);
    }
    for (const auto &constraint : m_constraints) {
        statement.addTableConstraint(constraint.second);
    }
    return statement;
}

const std::map<std::string, WCTColumnBinding, WCDB::String::CaseInsensiveComparator> &WCTBinding::getColumnBindings() const
{
    return m_columnBindings;
}

WCDB::StatementCreateVirtualTable
WCTBinding::generateVirtualCreateTableStatement(const std::string &tableName) const
{
    WCDB::StatementCreateVirtualTable statement = statementVirtualTable;
    statement.createVirtualTable(tableName);
    for (const auto &columnBinding : m_columnBindings) {
        statement.on(columnBinding.second.columnDef);
    }
    for (const auto &tableConstraint : m_constraints) {
        statement.on(tableConstraint.second);
    }
    return statement;
}

void WCTBinding::addColumnConstraint(const WCDB::ColumnConstraint &columnConstraint, const WCTProperty &property)
{
    auto iter = m_columnBindings.find(property.getDescription());
    WCTInnerAssert(iter != m_columnBindings.end());
    iter->second.columnDef.byAddingConstraint(columnConstraint);
}

WCDB::TableConstraint &WCTBinding::getOrCreateTableConstraint(const std::string &name)
{
    auto iter = m_constraints.find(name);
    if (iter == m_constraints.end()) {
        iter = m_constraints.insert({name, WCDB::TableConstraint(name)}).first;
    }
    return iter->second;
}

WCDB::StatementCreateIndex &WCTBinding::getOrCreateIndex(const std::string &subfix)
{
    auto iter = m_indexes.find(subfix);
    if (iter == m_indexes.end()) {
        iter = m_indexes.insert({subfix, WCDB::StatementCreateIndex()}).first;
    }
    return iter->second;
}

const WCTColumnBinding &WCTBinding::getColumnBinding(const WCTProperty &property) const
{
    auto iter = m_columnBindings.find(property.getDescription());
    WCTInnerAssert(iter != m_columnBindings.end());
    return iter->second;
}

std::list<WCDB::StatementCreateIndex>
WCTBinding::generateCreateIndexStatements(const std::string &tableName) const
{
    std::list<WCDB::StatementCreateIndex> statementCreateIndexs;
    for (const auto &iter : m_indexes) {
        WCDB::StatementCreateIndex statementCreateIndex = iter.second;
        statementCreateIndex.createIndex(tableName + iter.first).on(tableName);
        statementCreateIndexs.push_back(statementCreateIndex);
    }
    return statementCreateIndexs;
}

const WCTPropertyList &WCTBinding::getAllProperties() const
{
    return m_properties;
}

const WCTProperty &WCTBinding::addColumnBinding(const std::string &columnName,
                                                WCTColumnBinding &columnBinding)
{
    auto iter = m_columnBindings.insert({columnName, std::move(columnBinding)}).first;
    WCTProperty property(&iter->second);
    m_properties.push_back(property);
    return m_properties.back();
}

WCTColumnNamed WCTBinding::getColumnGenerator()
{
    static WCTColumnNamed s_columnNamed = ^WCDB::Column(NSString *name)
    {
        return WCDB::Column(name ? name.cppString : WCDB::String::empty());
    };
    return s_columnNamed;
}
