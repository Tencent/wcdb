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

namespace WCDB {
    
class Property: public Describable, public Operable
{
public:   
    Property(const std::string& name, const std::shared_ptr<WCTColumnBinding> &columnBinding);
    
    Property inTable(NSString *tableName) const;
    Property inTable(const std::string& tableName) const;

    const std::shared_ptr<WCTColumnBinding>& getColumnBinding() const;
    
    Column asColumn() const;
    ColumnIndex asIndex() const;
    ColumnIndex asIndex(WCTOrderTerm term) const;
    
    virtual Expression asExpression() const override; 
protected:    
    std::shared_ptr<WCTColumnBinding> m_columnBinding;
};
    
template <>
struct ColumnConvertible<Property> : public std::true_type
{
public:
    static Column asColumn(const Property &property);
};

template <>
struct ExpressionConvertible<Property> : public std::true_type
{
public:
    static Expression asExpression(const Property &property);
};

template <>
struct ColumnResultConvertible<Property> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const Property &property);
};

template <>
struct SpecificOrderConvertible<Property> : public std::true_type
{
public:
    static Order asOrder(const Property &property, OrderTerm term);
};

template <>
struct OrderConvertible<Property> : public std::true_type
{
public:
    static Order asOrder(const Property &property);
};

template <>
struct SpecificColumnIndexConvertible<Property> : public std::true_type
{
public:
    static ColumnIndex asIndex(const Property &property, OrderTerm term) ;
};

template <>
struct ColumnIndexConvertible<Property> : public std::true_type
{
public:
    static ColumnIndex asIndex(const Property &property) ;
};

template <>
struct SpecificColumnDefConvertible<Property> : public std::true_type
{
public:
    static ColumnDef asDef(const Property &property, ColumnType columnType);
};
    
} //namespace WCDB

typedef WCDB::Property WCTProperty;
typedef std::list<const WCDB::Property> WCTPropertyList;
