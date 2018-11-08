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
#import <WCDB/Error.hpp>
#import <WCDB/Macro.hpp>
#import <WCDB/WCTBinding.h>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTMacro.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTPropertyMacro.h>
#import <objc/runtime.h>

#pragma - Binding
const WCTBinding &WCTBinding::bindingWithClass(Class cls)
{
    static std::map<Class, WCTBinding> *s_bindings = new std::map<Class, WCTBinding>;
    static std::recursive_mutex *s_mutex = new std::recursive_mutex;
    std::lock_guard<std::recursive_mutex> lockGuard(*s_mutex);
    auto iter = s_bindings->find(cls);
    if (iter == s_bindings->end()) {
        iter = s_bindings->insert({ cls, WCTBinding(cls) }).first;
        iter->second.initialize();
    }
    return iter->second;
}

WCTBinding::WCTBinding(Class cls)
: m_cls(cls)
{
}

void WCTBinding::initialize()
{
    NSString *prefix = [NSString stringWithFormat:@"%s_%@_", WCDB_STRINGIFY(WCDB_ORM_PREFIX), NSStringFromClass(m_cls)];
    NSString *synthesizePrefix = @WCDB_STRINGIFY(WCDB_SYNTHESIZE_PREFIX);
    NSRange synthesizeRange = NSMakeRange(prefix.length, synthesizePrefix.length);

    NSMutableArray<NSString *> *synthesizations = [NSMutableArray<NSString *> array];
    NSMutableArray<NSString *> *others = [NSMutableArray<NSString *> array];

    unsigned int methodCount = 0;
    Method *methods = class_copyMethodList(object_getClass(m_cls), &methodCount);

    for (unsigned int i = 0; i < methodCount; i++) {
        Method method = methods[i];
        NSString *selName = NSStringFromSelector(method_getName(method));
        if (![selName hasPrefix:prefix]) {
            continue;
        }
        if ([selName compare:synthesizePrefix
                     options:0
                       range:synthesizeRange]
            == NSOrderedSame) {
            [synthesizations addObject:selName];
        } else {
            [others addObject:selName];
        }
    }

    free(methods);

    auto comparator = ^NSComparisonResult(NSString *str1, NSString *str2) {
        return [str1 compare:str2 options:NSNumericSearch];
    };
    [synthesizations sortUsingComparator:comparator];
    [others sortUsingComparator:comparator];

    for (NSString *selName in synthesizations) {
        SEL selector = NSSelectorFromString(selName);
        IMP imp = [m_cls methodForSelector:selector];
        ((void (*)(Class, SEL, WCTBinding &)) imp)(m_cls, selector, *this);
    }
    for (NSString *selName in others) {
        SEL selector = NSSelectorFromString(selName);
        IMP imp = [m_cls methodForSelector:selector];
        ((void (*)(Class, SEL, WCTBinding &)) imp)(m_cls, selector, *this);
    }

    // additional object relational mapping
    if ([m_cls respondsToSelector:@selector(additionalObjectRelationalMapping:)]) {
        [m_cls additionalObjectRelationalMapping:*this];
    }
}

void WCTBinding::checkInheritance(Class left, Class right)
{
    WCTRemedialAssert(left == right, "Inheritance is not supported for ORM.", ;);
}

#pragma - Property
WCDB::ColumnDef &WCTBinding::getColumnDef(const WCTProperty &property)
{
    auto iter = m_columnBindings.find(property.getColumnBinding().columnDef.syntax().column.name);
    WCTInnerAssert(iter != m_columnBindings.end());
    return iter->second.columnDef;
}

const std::map<WCDB::String, WCTColumnBinding, WCDB::String::CaseInsensiveComparator> &WCTBinding::getColumnBindings() const
{
    return m_columnBindings;
}

const WCTProperties &WCTBinding::getAllProperties() const
{
    return m_properties;
}

const WCTProperty &WCTBinding::getProperty(const WCDB::String &propertyName) const
{
    auto iter = m_mappedProperties.find(propertyName);
    WCTInnerAssert(iter != m_mappedProperties.end());
    return *iter->second;
}

void WCTBinding::addProperty(const WCDB::String &columnName,
                             const WCTColumnBinding &columnBinding)
{
    WCTInnerAssert(m_columnBindings.find(columnName) == m_columnBindings.end());
    auto iter = m_columnBindings.emplace(columnName, columnBinding).first;
    m_properties.push_back(iter->second);
    auto listIter = m_properties.end();
    std::advance(listIter, -1);
    m_mappedProperties.emplace(iter->second.propertyName, listIter);
}

#pragma mark - Table
WCDB::StatementCreateTable WCTBinding::generateCreateTableStatement(const WCDB::String &tableName) const
{
    WCDB::StatementCreateTable statement = WCDB::StatementCreateTable().createTable(tableName).ifNotExists();
    for (const auto &columnBinding : m_columnBindings) {
        statement.define(columnBinding.second.columnDef);
    }
    for (const auto &constraint : m_constraints) {
        WCDB::TableConstraint c = constraint.second;
        c.syntax().name = tableName + constraint.first;
        statement.constraint(c);
    }
    return statement;
}

WCDB::StatementCreateVirtualTable
WCTBinding::generateVirtualCreateTableStatement(const WCDB::String &tableName) const
{
    WCDB::StatementCreateVirtualTable statement = statementVirtualTable;
    statement.createVirtualTable(tableName).ifNotExists();
    std::list<WCDB::String> &arguments = statement.syntax().arguments;
    bool isFTS5 = statement.syntax().module.isCaseInsensiveEqual("fts5");
    for (const auto &columnBinding : m_columnBindings) {
        if (isFTS5) {
            // FTS5 does not need type
            arguments.push_back(columnBinding.second.columnDef.syntax().column.getDescription());
        } else {
            arguments.push_back(columnBinding.second.columnDef.getDescription());
        }
    }
    return statement;
}

#pragma mark - Table Constraint
WCDB::TableConstraint &WCTBinding::getOrCreateTableConstraint(const WCDB::String &subfix)
{
    auto iter = m_constraints.find(subfix);
    if (iter == m_constraints.end()) {
        iter = m_constraints.emplace(subfix, WCDB::TableConstraint()).first;
    }
    return iter->second;
}

#pragma mark - Index
WCDB::StatementCreateIndex &WCTBinding::getOrCreateIndex(const WCDB::String &subfix)
{
    auto iter = m_indexes.find(subfix);
    if (iter == m_indexes.end()) {
        iter = m_indexes.emplace(subfix, WCDB::StatementCreateIndex()).first;
    }
    return iter->second;
}

std::list<WCDB::StatementCreateIndex>
WCTBinding::generateCreateIndexStatements(const WCDB::String &tableName) const
{
    std::list<WCDB::StatementCreateIndex> statementCreateIndexs;
    for (const auto &iter : m_indexes) {
        WCDB::StatementCreateIndex statementCreateIndex = iter.second;
        statementCreateIndex.createIndex(tableName + iter.first).ifNotExists().table(tableName);
        statementCreateIndexs.push_back(statementCreateIndex);
    }
    return statementCreateIndexs;
}
