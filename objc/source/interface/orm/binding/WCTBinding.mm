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

WCTBinding *WCTBinding::bindingWithClass(Class cls)
{
    static std::map<Class, WCTBinding> s_bindings;
    static std::mutex s_mutex;
    std::lock_guard<std::mutex> lockGuard(s_mutex);
    auto iter = s_bindings.find(cls);
    if (iter == s_bindings.end()) {
        iter = s_bindings.insert({cls, WCTBinding(cls)}).first;
    }
    return &iter->second;
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
    for (const auto &columnBinding : m_columnBindings.getList()) {
        statement.define(columnBinding->columnDef);
    }
    for (const auto &constraint : m_constraints.getList()) {
        statement.addTableConstraint(*constraint.get());
    }
    return statement;
}

const std::map<std::string, std::shared_ptr<WCTColumnBinding>, WCDB::String::CaseInsensiveComparator> &WCTBinding::getColumnBindingMap() const
{
    return m_columnBindings.getMap();
}

WCDB::StatementCreateVirtualTable
WCTBinding::generateVirtualCreateTableStatement(const std::string &tableName) const
{
    WCDB::StatementCreateVirtualTable statement = statementVirtualTable;
    statement.createVirtualTable(tableName);
    for (const auto &columnBinding : m_columnBindings.getList()) {
        statement.on(columnBinding->columnDef);
    }
    for (const auto &tableConstraint : m_constraints.getList()) {
        statement.on(*tableConstraint.get());
    }
    return statement;
}

void WCTBinding::addColumnConstraint(const WCDB::ColumnConstraint &columnConstraint, const WCTProperty &property) const
{
    const auto &map = m_columnBindings.getMap();
    auto iter = map.find(property.getDescription());
    WCTInnerAssert(iter != map.end());
    iter->second->columnDef.byAddingConstraint(columnConstraint);
}

std::shared_ptr<WCDB::TableConstraint> WCTBinding::getOrCreateTableConstraint(const std::string &name)
{
    const auto &map = m_constraints.getMap();
    if (map.find(name) == map.end()) {
        std::shared_ptr<WCDB::TableConstraint> tableConstraint(new WCDB::TableConstraint(name));
        m_constraints.append(name, tableConstraint);
    }
    return map.at(name);
}

std::shared_ptr<WCDB::StatementCreateIndex> WCTBinding::getOrCreateIndex(const std::string &subfix)
{
    const auto &map = m_indexes.getMap();
    if (map.find(subfix) == map.end()) {
        std::shared_ptr<WCDB::StatementCreateIndex> statementCreateIndex(new WCDB::StatementCreateIndex());
        m_indexes.append(subfix, statementCreateIndex);
    }
    return map.at(subfix);
}

std::shared_ptr<WCTColumnBinding> WCTBinding::getColumnBinding(const WCTProperty &property) const
{
    const auto &map = m_columnBindings.getMap();
    auto iter = map.find(property.getDescription());
    WCTInnerAssert(iter != map.end());
    return iter->second;
}

std::list<WCDB::StatementCreateIndex>
WCTBinding::generateCreateIndexStatements(const std::string &tableName) const
{
    std::list<WCDB::StatementCreateIndex> statementCreateIndexs;
    for (const auto &pair : m_indexes.getMap()) {
        WCDB::StatementCreateIndex statementCreateIndex = *pair.second.get();
        statementCreateIndex.createIndex(tableName + pair.first).on(tableName);
        statementCreateIndexs.push_back(statementCreateIndex);
    }
    return statementCreateIndexs;
}

const WCTPropertyList &WCTBinding::getAllProperties() const
{
    return m_properties;
}

const WCTProperty &WCTBinding::addColumnBinding(const std::string &columnName,
                                                const std::shared_ptr<WCTColumnBinding> &columnBinding)
{
    m_columnBindings.append(columnName, columnBinding);
    WCTProperty property(columnBinding);
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
