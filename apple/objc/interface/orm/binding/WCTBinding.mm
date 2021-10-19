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

#import <WCDB/Assertion.hpp>
#import <WCDB/Error.hpp>
#import <WCDB/WCTBinding.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTColumnCoding.h>
#import <WCDB/WCTMacro.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTTableCoding.h>
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

    unsigned int numberOfMethods = 0;
    Method *methods = class_copyMethodList(object_getClass(m_cls), &numberOfMethods);
    WCTAssert(methods != nullptr && numberOfMethods > 0);
    NSMutableArray<NSString *> *synthesizations = [NSMutableArray<NSString *> arrayWithCapacity:numberOfMethods];
    NSMutableArray<NSString *> *others = [NSMutableArray<NSString *> arrayWithCapacity:numberOfMethods];
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
    if (methods != nullptr) {
        free(methods);
    }

    static auto s_numbericComparator = ^NSComparisonResult(NSString *str1, NSString *str2) {
        return [str1 compare:str2 options:NSNumericSearch];
    };
    [synthesizations sortUsingComparator:s_numbericComparator];
    [others sortUsingComparator:s_numbericComparator];

    for (NSString *selName in synthesizations) {
        SEL selector = NSSelectorFromString(selName);
        IMP imp = [m_cls methodForSelector:selector];
        const WCTProperty &property = ((const WCTProperty &(*) (Class, SEL)) imp)(m_cls, selector);
        WCTRemedialAssert(getColumnDef(property) == nullptr, WCDB::StringView::formatted("Duplicated property [%s] found", property.getDescription().data()), continue;);
        m_properties.push_back(property);
        m_columnDefs.push_back({ property.getDescription(), WCDB::ColumnDef(property, property.getColumnBinding().getAccessor()->getColumnType()) });
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

void WCTBinding::assertNoInheritance(const WCTProperty &property, Class cls)
{
    WCTRemedialAssert(property.getColumnBinding().getClass() == cls, "Inheritance is not supported for ORM.", ;);
}

#pragma mark - Property
const WCTProperties &WCTBinding::getProperties() const
{
    return m_properties;
}

#pragma mark - Column Def
const WCDB::CaseInsensiveList<WCDB::ColumnDef> &WCTBinding::getColumnDefs() const
{
    return m_columnDefs;
}

WCDB::ColumnDef *WCTBinding::getColumnDef(const WCTProperty &property)
{
    WCDB::StringView name = property.getDescription();
    WCDB::ColumnDef *columnDef = nullptr;
    auto iter = m_columnDefs.caseInsensiveFind(name);
    if (iter != m_columnDefs.end()) {
        columnDef = &iter->second;
    }
    return columnDef;
}

#pragma mark - Table
WCDB::StatementCreateTable WCTBinding::generateCreateTableStatement(const WCDB::UnsafeStringView &tableName) const
{
    WCDB::StatementCreateTable statement = statementTable;
    statement.createTable(tableName).ifNotExists();
    for (const auto &iter : m_columnDefs) {
        statement.define(iter.second);
    }
    for (const auto &constraint : m_constraints) {
        statement.constraint(constraint.second);
    }
    return statement;
}

WCDB::StatementCreateVirtualTable
WCTBinding::generateCreateVirtualTableStatement(const WCDB::UnsafeStringView &tableName) const
{
    WCDB::StatementCreateVirtualTable statement = statementVirtualTable;
    statement.createVirtualTable(tableName).ifNotExists();
    std::list<WCDB::StringView> &arguments = statement.syntax().arguments;
    bool isFTS5 = statement.syntax().module.caseInsensiveEqual("fts5");
    for (const auto &iter : m_columnDefs) {
        if (isFTS5) {
            bool added = false;
            for (auto constrain : iter.second.syntax().constraints) {
                if (constrain.getDescription().find("UNINDEXED") == 0) {
                    arguments.push_back(WCDB::StringView().formatted("%s %s", iter.second.syntax().column.getDescription().data(), constrain.getDescription().data()));
                    added = YES;
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
WCDB::TableConstraint &WCTBinding::getOrCreateTableConstraint(const WCDB::UnsafeStringView &name)
{
    auto iter = m_constraints.find(name);
    if (iter == m_constraints.end()) {
        iter = m_constraints.emplace(name, WCDB::TableConstraint(name)).first;
    }
    return iter->second;
}

#pragma mark - Index

WCTBinding::Index::Index(const WCDB::UnsafeStringView &suffix_)
: suffix(suffix_)
, action(Action::Create)
{
}

WCTBinding::Index &WCTBinding::getOrCreateIndex(const WCDB::UnsafeStringView &suffix)
{
    auto iter = m_indexes.find(suffix);
    if (iter == m_indexes.end()) {
        iter = m_indexes.emplace(suffix, Index(suffix)).first;
    }
    WCTAssert(iter->first == iter->second.suffix);
    return iter->second;
}

std::pair<std::list<WCDB::StatementCreateIndex>, std::list<WCDB::StatementDropIndex>>
WCTBinding::generateIndexStatements(const WCDB::UnsafeStringView &tableName, bool isTableNewlyCreated) const
{
    std::pair<std::list<WCDB::StatementCreateIndex>, std::list<WCDB::StatementDropIndex>> pairs;
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
            WCDB::StatementCreateIndex statement = index.statement;
            std::ostringstream stream;
            stream << tableName << index.suffix;
            statement.createIndex(WCDB::StringView(stream.str())).ifNotExists().table(tableName);
            pairs.first.push_back(statement);
        } break;
        default:
            WCTAssert(index.action == Index::Action::Drop);
            std::ostringstream stream;
            stream << tableName << index.suffix;
            WCDB::StatementDropIndex statement = WCDB::StatementDropIndex().dropIndex(WCDB::StringView(stream.str())).ifExists();
            pairs.second.push_back(statement);
            break;
        }
    }
    return pairs;
}
