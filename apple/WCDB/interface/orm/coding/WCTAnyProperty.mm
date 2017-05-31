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
#import <WCDB/WCTExpr.h>

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

WCTExpr WCTAnyProperty::avg(bool distinct) const
{
    return WCTExpr(WCDB::Expr(*this)).avg(distinct);
}

WCTExpr WCTAnyProperty::count(bool distinct) const
{
    return WCTExpr(WCDB::Expr(*this)).count(distinct);
}

WCTExpr WCTAnyProperty::groupConcat(bool distinct) const
{
    return WCTExpr(WCDB::Expr(*this)).groupConcat(distinct);
}

WCTExpr WCTAnyProperty::groupConcat(NSString *seperator, bool distinct) const
{
    return WCTExpr(WCDB::Expr(*this)).groupConcat(seperator, distinct);
}

WCTExpr WCTAnyProperty::max(bool distinct) const
{
    return WCTExpr(WCDB::Expr(*this)).max(distinct);
}

WCTExpr WCTAnyProperty::min(bool distinct) const
{
    return WCTExpr(WCDB::Expr(*this)).min(distinct);
}

WCTExpr WCTAnyProperty::sum(bool distinct) const
{
    return WCTExpr(WCDB::Expr(*this)).sum(distinct);
}

WCTExpr WCTAnyProperty::total(bool distinct) const
{
    return WCTExpr(WCDB::Expr(*this)).total(distinct);
}

Class WCTAnyProperty::getClass() const
{
    return m_cls;
}
