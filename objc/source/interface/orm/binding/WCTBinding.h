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

#import <WCDB/SequantialIncreasingMap.hpp>
#import <WCDB/String.hpp>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTCommon.h>
#import <WCDB/WCTProperty.h>

class WCTBinding {
public:
    WCTBinding(Class cls);

    template <typename T>
    const WCTProperty &addColumnBinding(const std::string &propertyName,
                                        const std::string &columnName)
    {
        std::shared_ptr<WCTColumnBinding> columnBinding(new WCTColumnBinding(
            m_cls, propertyName, columnName, (T *) nullptr));
        return addColumnBinding(columnName, columnBinding);
    }

    std::shared_ptr<WCTColumnBinding>
    getColumnBinding(const WCTProperty &property) const;

    void addColumnConstraint(const WCDB::ColumnConstraint &columnConstraint,
                             const WCTProperty &property) const;

    std::shared_ptr<WCDB::TableConstraint>
    getOrCreateTableConstraint(const std::string &name);

    std::shared_ptr<WCDB::StatementCreateIndex>
    getOrCreateIndex(const std::string &subfix);

    const std::map<std::string,
                   std::shared_ptr<WCTColumnBinding>,
                   WCDB::String::CaseInsensiveComparator> &
    getColumnBindingMap() const;

    WCDB::StatementCreateVirtualTable statementVirtualTable;

    WCDB::StatementCreateTable
    generateCreateTableStatement(const std::string &tableName) const;
    WCDB::StatementCreateVirtualTable
    generateVirtualCreateTableStatement(const std::string &tableName) const;
    std::list<WCDB::StatementCreateIndex>
    generateCreateIndexStatements(const std::string &tableName) const;

    const WCTPropertyList &getAllProperties() const;

    static WCTColumnNamed getColumnGenerator();

protected:
    const WCTProperty &
    addColumnBinding(const std::string &columnName,
                     const std::shared_ptr<WCTColumnBinding> &columnBinding);
    WCTPropertyList m_properties;
    SequentialIncreasingMap<std::string,
                            WCTColumnBinding,
                            WCDB::String::CaseInsensiveComparator>
        m_columnBindings;

    SequentialIncreasingMap<std::string,
                            WCDB::TableConstraint,
                            WCDB::String::CaseInsensiveComparator>
        m_constraints;

    SequentialIncreasingMap<std::string,
                            WCDB::StatementCreateIndex,
                            WCDB::String::CaseInsensiveComparator>
        m_indexes;

    Class m_cls;
};

#endif /* WCTBinding_hpp */
