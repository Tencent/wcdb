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

#import <WCDB/WCTAnyProperty.h>
#import <WCDB/WCTExpression.h>

WCTAnyProperty::WCTAnyProperty()
    : WCDB::Column(WCDB::Column::Any)
    , m_cls(nil)
{
}

WCTAnyProperty::WCTAnyProperty(Class cls)
    : WCDB::Column(WCDB::Column::Any)
    , m_cls(cls)
{
}

WCTAnyProperty::WCTAnyProperty(const Column &column, Class cls)
    : WCDB::Column(column)
    , m_cls(cls)
{
}

WCTAnyProperty WCTAnyProperty::inTable(NSString *tableName) const
{
    return WCTAnyProperty(WCDB::Column::inTable(tableName.UTF8String), m_cls);
}

WCTExpression WCTAnyProperty::avg(bool distinct) const
{
    return WCTExpression(WCDB::Expression(*this)).avg(distinct);
}

WCTExpression WCTAnyProperty::count(bool distinct) const
{
    return WCTExpression(WCDB::Expression(*this)).count(distinct);
}

WCTExpression WCTAnyProperty::groupConcat(bool distinct) const
{
    return WCTExpression(WCDB::Expression(*this)).groupConcat(distinct);
}

WCTExpression WCTAnyProperty::groupConcat(NSString *seperator, bool distinct) const
{
    return WCTExpression(WCDB::Expression(*this)).groupConcat(seperator, distinct);
}

WCTExpression WCTAnyProperty::max(bool distinct) const
{
    return WCTExpression(WCDB::Expression(*this)).max(distinct);
}

WCTExpression WCTAnyProperty::min(bool distinct) const
{
    return WCTExpression(WCDB::Expression(*this)).min(distinct);
}

WCTExpression WCTAnyProperty::sum(bool distinct) const
{
    return WCTExpression(WCDB::Expression(*this)).sum(distinct);
}

WCTExpression WCTAnyProperty::total(bool distinct) const
{
    return WCTExpression(WCDB::Expression(*this)).total(distinct);
}

Class WCTAnyProperty::getClass() const
{
    return m_cls;
}
