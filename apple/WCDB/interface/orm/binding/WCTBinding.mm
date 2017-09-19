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

#import <WCDB/WCTBinding.h>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTIndexBinding.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/error.hpp>
#import <objc/runtime.h>

WCTBinding::WCTBinding(Class cls)
    : m_cls(cls)
    , m_indexBindingMap(nullptr)
    , m_constraintBindingMap(nullptr)
    , m_constraintBindingList(nullptr)
    , m_virtualTableArgumentList(nullptr)
{
    if (![m_cls conformsToProtocol:@protocol(WCTTableCoding)]) {
        class_addProtocol(m_cls, @protocol(WCTTableCoding));
    }
}

void WCTBinding::_addColumnBinding(const std::string &columnName, const std::shared_ptr<WCTColumnBinding> &columnBinding)
{
    m_columnBindingList.push_back(columnBinding);
    m_columnBindingMap.insert({columnName, columnBinding});
}

std::shared_ptr<WCTColumnBinding> WCTBinding::getColumnBinding(const WCTProperty &property) const
{
    auto iter = m_columnBindingMap.find(property.getName());
    return iter != m_columnBindingMap.end() ? iter->second : nullptr;
}

void WCTBinding::lazyInitIndexBinding()
{
    if (!m_indexBindingMap) {
        m_indexBindingMap.reset(new WCTIndexBindingMap);
    }
}

std::shared_ptr<WCTIndexBinding> WCTBinding::getOrCreateIndexBinding(const std::string &indexSubfixName)
{
    lazyInitIndexBinding();
    auto iter = m_indexBindingMap->find(indexSubfixName);
    if (iter == m_indexBindingMap->end()) {
        std::shared_ptr<WCTIndexBinding> indexBinding(new WCTIndexBinding(indexSubfixName));
        m_indexBindingMap->insert({indexSubfixName, indexBinding});
        return indexBinding;
    }
    return iter->second;
}

const std::shared_ptr<WCTIndexBindingMap> WCTBinding::getIndexBindingMap() const
{
    return m_indexBindingMap;
}

const WCTColumnBindingList &WCTBinding::getColumnBindingList() const
{
    return m_columnBindingList;
}

const WCTColumnBindingMap &WCTBinding::getColumnBindingMap() const
{
    return m_columnBindingMap;
}

void WCTBinding::lazyInitConstraintBinding()
{
    if (!m_constraintBindingList) {
        m_constraintBindingList.reset(new WCTConstraintBindingList);
    }
    if (!m_constraintBindingMap) {
        m_constraintBindingMap.reset(new WCTConstraintBindingMap);
    }
}

WCTConstraintBindingBase *WCTBinding::getConstraint(const std::string &constraintName, WCTConstraintBindingType type)
{
    if (!m_constraintBindingMap) {
        return nullptr;
    }
    auto iter = m_constraintBindingMap->find(constraintName);
    if (iter != m_constraintBindingMap->end()) {
        WCTConstraintBindingBase *constraintBinding = iter->second.get();
        if (constraintBinding->type == type) {
            return constraintBinding;
        }
        WCDB::Error::Abort(([NSString stringWithFormat:@"There are already other constraints with different types of the same name %s", constraintName.c_str()].UTF8String));
    }
    return nullptr;
}

void WCTBinding::addConstraintBinding(const std::string &constraintName, const std::shared_ptr<WCTConstraintBindingBase> &constraintBinding)
{
    lazyInitConstraintBinding();
    m_constraintBindingMap->insert({constraintName, constraintBinding});
    m_constraintBindingList->push_back(constraintBinding);
}

const std::shared_ptr<std::list<std::pair<std::string, std::string>>> WCTBinding::getVirtualTableArgumentList() const
{
    return m_virtualTableArgumentList;
}

void WCTBinding::lazyInitVirtualTableArgumentList()
{
    if (!m_virtualTableArgumentList) {
        m_virtualTableArgumentList.reset(new std::list<std::pair<std::string, std::string>>);
    }
}

void WCTBinding::addVirtualTableArgument(const std::string &left, const std::string &right)
{
    lazyInitVirtualTableArgumentList();
    m_virtualTableArgumentList->push_back({left, right});
}

void WCTBinding::addVirtualTableArgument(const std::string &left, NSString *right)
{
    lazyInitVirtualTableArgumentList();
    m_virtualTableArgumentList->push_back({left, right.UTF8String});
}

WCDB::StatementCreateTable WCTBinding::generateCreateTableStatement(const std::string &tableName) const
{
    WCDB::ColumnDefList columnDefList;
    for (const auto &columnBinding : m_columnBindingList) {
        if (columnBinding) {
            columnDefList.push_back(columnBinding->getColumnDef());
        }
    }
    WCDB::TableConstraintList constraintList;
    if (m_constraintBindingList) {
        for (const auto &constraintBinding : *m_constraintBindingList.get()) {
            constraintList.push_back(constraintBinding->generateConstraint());
        }
    }
    return WCDB::StatementCreateTable().create(tableName, columnDefList, constraintList);
}

WCDB::StatementCreateVirtualTable WCTBinding::generateVirtualCreateTableStatement(const std::string &tableName) const
{
    WCDB::ModuleArgumentList moduleArgumentList;
    for (const auto &columnBinding : m_columnBindingList) {
        moduleArgumentList.push_back(columnBinding->getColumnDef());
    }
    if (m_constraintBindingList) {
        for (const auto &constraintBinding : *m_constraintBindingList.get()) {
            moduleArgumentList.push_back(constraintBinding->generateConstraint());
        }
    }
    if (m_virtualTableArgumentList) {
        for (const auto &moduleArgument : *m_virtualTableArgumentList.get()) {
            moduleArgumentList.push_back(WCDB::ModuleArgument(moduleArgument.first, moduleArgument.second));
        }
    }
    return WCDB::StatementCreateVirtualTable().create(tableName).usingModule(m_virtualTableModuleName, moduleArgumentList);
}

void WCTBinding::setVirtualTableModule(const std::string &moduleName)
{
    m_virtualTableModuleName = moduleName;
}

void WCTBinding::setVirtualTableModule(NSString *moduleName)
{
    m_virtualTableModuleName = moduleName.UTF8String;
}
