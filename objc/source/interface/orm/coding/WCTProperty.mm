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

namespace WCDB {

Property::Property(const std::string &name, const std::shared_ptr<WCTColumnBinding> &columnBinding)
: Describable(name)
, m_columnBinding(columnBinding)
{
}

Property Property::inTable(NSString *tableName) const
{
    return Property(Column(m_description).inTable(tableName.UTF8String).getDescription(), m_columnBinding);
}

Property Property::inTable(const std::string& tableName) const
{
    return Property(Column(m_description).inTable(tableName).getDescription(), m_columnBinding);
}
    
const std::shared_ptr<WCTColumnBinding>& Property::getColumnBinding() const
{
    return m_columnBinding;
}
        
Expression Property::asExpression() const
{
    return Expression(*this);
}
    
Column ColumnConvertible<Property>::asColumn(const Property& property)
{
    return Column(property.getDescription());
}

Expression ExpressionConvertible<Property>::asExpression(const Property& property)
{
    return Expression(ColumnConvertible<Property>::asColumn(property));
}

ColumnResult ColumnResultConvertible<Property>::asColumnResult(const Property& property)
{
    return ColumnResult(ColumnConvertible<Property>::asColumn(property));
}

Order SpecificOrderConvertible<Property>::asOrder(const Property& property, OrderTerm term)
{
    return Order(ExpressionConvertible<Property>::asExpression(property), term);
}

Order OrderConvertible<Property>::asOrder(const Property& property)
{
    return SpecificOrderConvertible<Property>::asOrder(property, OrderTerm::NotSet);
}

ColumnIndex SpecificColumnIndexConvertible<Property>::asIndex(const Property& property, OrderTerm term)
{
    return ColumnIndex(ColumnConvertible<Property>::asColumn(property), term);
}

ColumnIndex ColumnIndexConvertible<Property>::asIndex(const Property& property)
{
    return SpecificColumnIndexConvertible<Property>::asIndex(property, OrderTerm::NotSet);
}

ColumnDef SpecificColumnDefConvertible<Property>::asDef(const Property& property, ColumnType propertyType)
{
    return ColumnDef(ColumnConvertible<Property>::asColumn(property), propertyType);
}
    
} //namespace WCDB
