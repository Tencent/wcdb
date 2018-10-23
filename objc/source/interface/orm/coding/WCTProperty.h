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

#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTCommon.h>

class WCTProperty : public WCDB::Column {
public:
    using WCDB::Column::Column;
    WCTProperty(const WCTColumnBinding &columnBinding);

    WCDB::IndexedColumn asIndex() const;
    WCDB::OrderingTerm asOrder() const;

    const WCTColumnBinding &getColumnBinding() const;
};

class WCTProperties : public WCDB::SyntaxList<WCTProperty> {
public:
    using WCDB::SyntaxList<WCTProperty>::SyntaxList;

    WCDB::Expression count() const;
};

//class WCTProperties : public std::list<WCTProperty> {
//public:
//    using list<WCTProperty>::list;
//    WCTProperties(const WCTProperty &property);
//
//    WCTProperties inTable(NSString *tableName) const;
//    WCTProperties inSchema(NSString *schemaName) const;
//
//    void addProperties(const WCTProperties &properties);
//
//    WCTProperties byAddingProperties(const WCTProperties &properties) const;
//
//    operator std::list<WCDB::Column>() const;
//    operator WCDB::Expressions() const;
//    operator WCDB::ResultColumns() const;
//    operator WCDB::OrderingTerms() const;
//    operator std::list<WCDB::IndexedColumn>() const;
//};
