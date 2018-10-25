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

#ifndef WCTBinding_hpp
#define WCTBinding_hpp

#import <WCDB/String.hpp>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTCommon.h>
#import <WCDB/WCTProperty.h>
#import <map>

class WCTBinding {
public:
    static const WCTBinding &bindingWithClass(Class cls);

    template<typename T>
    void addColumnBinding(const WCDB::String &propertyName, const WCDB::String &columnName)
    {
        WCTColumnBinding columnBinding(m_cls, propertyName, columnName, (T *) nullptr);
        addColumnBinding(columnName, std::move(columnBinding));
    }

    const WCTProperty &getProperty(const WCDB::String &propertyName) const;

    const WCTColumnBinding &getColumnBinding(const WCDB::String &columnName) const;

    WCDB::ColumnDef &getColumnDef(const WCTProperty &property);

    WCDB::TableConstraint &getOrCreateTableConstraint(const WCDB::String &name);

    WCDB::StatementCreateIndex &getOrCreateIndex(const WCDB::String &subfix);

    const std::map<WCDB::String, WCTColumnBinding, WCDB::String::CaseInsensiveComparator> &
    getColumnBindings() const;

    WCDB::StatementCreateVirtualTable statementVirtualTable;

    WCDB::StatementCreateTable
    generateCreateTableStatement(const WCDB::String &tableName) const;
    WCDB::StatementCreateVirtualTable
    generateVirtualCreateTableStatement(const WCDB::String &tableName) const;
    std::list<WCDB::StatementCreateIndex>
    generateCreateIndexStatements(const WCDB::String &tableName) const;

    const WCTProperties &getAllProperties() const;

    static WCTColumnNamed getColumnGenerator();

    void checkSafeCall(Class cls) const;

protected:
    WCTBinding(Class cls);

    void initialize();

    void addColumnBinding(const WCDB::String &columnName, const WCTColumnBinding &columnBinding);

    WCTProperties m_properties;
    std::map<WCDB::String, WCTProperties::iterator> m_mappedProperties;

    std::map<WCDB::String, WCTColumnBinding, WCDB::String::CaseInsensiveComparator> m_columnBindings;

    std::map<WCDB::String, WCDB::TableConstraint> m_constraints;

    std::map<WCDB::String, WCDB::StatementCreateIndex> m_indexes;

    Class m_cls;
};

#endif /* WCTBinding_hpp */
