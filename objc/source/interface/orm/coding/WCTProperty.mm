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

#import <WCDB/WCTProperty.h>

WCTProperty::WCTProperty(const std::string &name, const std::shared_ptr<WCTColumnBinding> &columnBinding)
: Describable(name)
, m_columnBinding(columnBinding)
{
}

WCTProperty WCTProperty::inTable(NSString *tableName) const
{
    return WCTProperty(WCDB::Column(m_description).inTable(tableName.UTF8String).getDescription(), m_columnBinding);
}

WCTProperty WCTProperty::inTable(const std::string& tableName) const
{
    return WCTProperty(WCDB::Column(m_description).inTable(tableName).getDescription(), m_columnBinding);
}
    
const std::shared_ptr<WCTColumnBinding>& WCTProperty::getColumnBinding() const
{
    return m_columnBinding;
}
    
WCDB::Column WCTProperty::asColumn() const
{
    return WCDB::Column(*this);
}
    
WCDB::ColumnIndex WCTProperty::asIndex() const
{
    return WCDB::ColumnIndex(asColumn());
}    
    
WCDB::ColumnIndex WCTProperty::asIndex(WCTOrderTerm term) const
{
    return WCDB::ColumnIndex(asColumn(), (WCDB::OrderTerm)term);
}
        
WCDB::Expression WCTProperty::asExpression() const
{
    return WCDB::Expression(*this);
}

namespace WCDB {
    
Column ColumnConvertible<WCTProperty>::asColumn(const WCTProperty& property)
{
    return Column(property.getDescription());
}

Expression ExpressionConvertible<WCTProperty>::asExpression(const WCTProperty& property)
{
    return Expression(ColumnConvertible<WCTProperty>::asColumn(property));
}

ColumnResult ColumnResultConvertible<WCTProperty>::asColumnResult(const WCTProperty& property)
{
    return ColumnResult(ColumnConvertible<WCTProperty>::asColumn(property));
}

Order SpecificOrderConvertible<WCTProperty>::asOrder(const WCTProperty& property, OrderTerm term)
{
    return Order(ExpressionConvertible<WCTProperty>::asExpression(property), term);
}

Order OrderConvertible<WCTProperty>::asOrder(const WCTProperty& property)
{
    return SpecificOrderConvertible<WCTProperty>::asOrder(property, OrderTerm::NotSet);
}

ColumnIndex SpecificColumnIndexConvertible<WCTProperty>::asIndex(const WCTProperty& property, OrderTerm term)
{
    return ColumnIndex(ColumnConvertible<WCTProperty>::asColumn(property), term);
}

ColumnIndex ColumnIndexConvertible<WCTProperty>::asIndex(const WCTProperty& property)
{
    return SpecificColumnIndexConvertible<WCTProperty>::asIndex(property, OrderTerm::NotSet);
}

ColumnDef SpecificColumnDefConvertible<WCTProperty>::asDef(const WCTProperty& property, ColumnType propertyType)
{
    return ColumnDef(ColumnConvertible<WCTProperty>::asColumn(property), propertyType);
}
    
} //namespace WCDB
