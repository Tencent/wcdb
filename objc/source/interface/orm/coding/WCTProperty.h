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

#import <WCDB/WCTDeclare.h>
#import <WCDB/abstract.h>

class WCTColumnBinding;

class WCTProperty : public WCDB::Describable, public WCDB::Operable {
public:
    WCTProperty(const std::string &name,
                const std::shared_ptr<WCTColumnBinding> &columnBinding);

    WCTProperty inTable(NSString *tableName) const;
    WCTProperty inTable(const std::string &tableName) const;

    const std::shared_ptr<WCTColumnBinding> &getColumnBinding() const;

    WCDB::Column asColumn() const;
    WCDB::ColumnIndex asIndex() const;
    WCDB::ColumnIndex asIndex(WCTOrderTerm term) const;
    WCDB::Order asOrder() const;
    WCDB::Order asOrder(WCTOrderTerm term) const;
    WCDB::ColumnDef asDef() const;
    WCDB::ColumnDef asDef(WCTColumnType columnType) const;

    virtual WCDB::Expression asExpression() const override;

    operator std::list<const WCTProperty>() const;
    operator std::list<const WCDB::Order>() const;
    operator std::list<const WCDB::ColumnResult>() const;
    operator std::list<const WCDB::Expression>() const;

protected:
    std::shared_ptr<WCTColumnBinding> m_columnBinding;
};

namespace WCDB {

template <>
struct ColumnConvertible<WCTProperty> : public std::true_type {
public:
    static Column asColumn(const WCTProperty &property);
};

template <>
struct ExpressionConvertible<WCTProperty> : public std::true_type {
public:
    static Expression asExpression(const WCTProperty &property);
};

template <>
struct ColumnResultConvertible<WCTProperty> : public std::true_type {
public:
    static ColumnResult asColumnResult(const WCTProperty &property);
};

template <>
struct SpecificOrderConvertible<WCTProperty> : public std::true_type {
public:
    static Order asOrder(const WCTProperty &property, OrderTerm term);
};

template <>
struct OrderConvertible<WCTProperty> : public std::true_type {
public:
    static Order asOrder(const WCTProperty &property);
};

template <>
struct SpecificColumnIndexConvertible<WCTProperty> : public std::true_type {
public:
    static ColumnIndex asIndex(const WCTProperty &property, OrderTerm term);
};

template <>
struct ColumnIndexConvertible<WCTProperty> : public std::true_type {
public:
    static ColumnIndex asIndex(const WCTProperty &property);
};

template <>
struct SpecificColumnDefConvertible<WCTProperty> : public std::true_type {
public:
    static ColumnDef asDef(const WCTProperty &property, ColumnType columnType);
};

} //namespace WCDB

typedef std::list<const WCTProperty> WCTPropertyList;
