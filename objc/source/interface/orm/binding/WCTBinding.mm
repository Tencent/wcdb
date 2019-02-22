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
#import <WCDB/WCTBinding.h>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTMacro.h>
#import <WCDB/WCTProperty.h>
#import <objc/runtime.h>

#pragma mark - Binding
WCTBinding::WCTBinding(Class cls)
: m_cls(cls)
{
    // __wcdb_
    NSString *prefix = @WCDB_STRINGIFY(WCDB_ORM_PREFIX) "_";
    // synthesize_
    NSString *synthesize = @WCDB_STRINGIFY(WCDB_ORM_TYPE_SYNTHESIZE) "_";
    NSRange synthesizeRange = NSMakeRange(prefix.length, synthesize.length);

    NSMutableArray<NSString *> *synthesizations = [NSMutableArray<NSString *> array];
    NSMutableArray<NSString *> *others = [NSMutableArray<NSString *> array];

    unsigned int numberOfMethods = 0;
    Method *methods = class_copyMethodList(object_getClass(m_cls), &numberOfMethods);
    for (unsigned int i = 0; i < numberOfMethods; i++) {
        Method method = methods[i];
        NSString *selName = NSStringFromSelector(method_getName(method));
        if ([selName hasPrefix:prefix]) {
            if ([selName compare:synthesize options:0 range:synthesizeRange] == NSOrderedSame) {
                [synthesizations addObject:selName];
            } else {
                [others addObject:selName];
            }
        }
    }
    free(methods);

    static auto s_numbericComparator = ^NSComparisonResult(NSString *str1, NSString *str2) {
        return [str1 compare:str2 options:NSNumericSearch];
    };
    [synthesizations sortUsingComparator:s_numbericComparator];
    [others sortUsingComparator:s_numbericComparator];

    for (NSString *selName in synthesizations) {
        SEL selector = NSSelectorFromString(selName);
        IMP imp = [m_cls methodForSelector:selector];
        const WCTProperty &property = ((const WCTProperty &(*) (Class, SEL)) imp)(m_cls, selector);
        m_properties.push_back(property);
        getOrCreateColumnDef(m_properties.back()); // trigger column def creation
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

void WCTBinding::assertNoInheritance(Class left, Class right)
{
    WCTRemedialAssert(left == right, "Inheritance is not supported for ORM.", ;);
}

#pragma mark - Property
const WCTProperties &WCTBinding::getProperties() const
{
    return m_properties;
}

#pragma mark - Column Def
const std::map<WCDB::String, WCDB::ColumnDef, WCDB::String::CaseInsensiveComparator> &WCTBinding::getColumnDefs() const
{
    return m_columnDefs;
}

WCDB::ColumnDef &WCTBinding::getOrCreateColumnDef(const WCTProperty &property)
{
#warning TODO - refactor to use order of definition
    WCDB::String name = property.getDescription();
    auto iter = m_columnDefs.find(name);
    if (iter == m_columnDefs.end()) {
        iter = m_columnDefs.emplace(name, WCDB::ColumnDef(property, property.getColumnBinding().getAccessor()->getColumnType())).first;
    }
    return iter->second;
}

#pragma mark - Table
WCDB::StatementCreateTable WCTBinding::generateCreateTableStatement(const WCDB::String &tableName) const
{
    WCDB::StatementCreateTable statement = WCDB::StatementCreateTable().createTable(tableName).ifNotExists();
    for (const auto &iter : m_columnDefs) {
        statement.define(iter.second);
    }
    for (const auto &constraint : m_constraints) {
        statement.constraint(constraint.second);
    }
    return statement;
}

WCDB::StatementCreateVirtualTable
WCTBinding::generateCreateVirtualTableStatement(const WCDB::String &tableName) const
{
    WCDB::StatementCreateVirtualTable statement = statementVirtualTable;
    statement.createVirtualTable(tableName).ifNotExists();
    std::list<WCDB::String> &arguments = statement.syntax().arguments;
    bool isFTS5 = statement.syntax().module.isCaseInsensiveEqual("fts5");
    for (const auto &iter : m_columnDefs) {
        if (isFTS5) {
            // FTS5 does not need type
            arguments.push_back(iter.second.syntax().column.getDescription());
        } else {
            arguments.push_back(iter.second.getDescription());
        }
    }
    return statement;
}

#pragma mark - Table Constraint
WCDB::TableConstraint &WCTBinding::getOrCreateTableConstraint(const WCDB::String &name)
{
    auto iter = m_constraints.find(name);
    if (iter == m_constraints.end()) {
        iter = m_constraints.emplace(name, WCDB::TableConstraint(name)).first;
    }
    return iter->second;
}

#pragma mark - Index
WCDB::StatementCreateIndex &WCTBinding::getOrCreateIndexStatement(const WCDB::String &subfix)
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
