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

#import <WCDB/WCTExpr.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTResult.h>

WCTResult::WCTResult(const WCTExpr &expr)
    : WCDB::ColumnResult(expr)
    , WCTPropertyBase(expr)
{
}

WCTResult::WCTResult(const WCTProperty &property)
    : WCDB::ColumnResult(property)
    , WCTPropertyBase(property)
{
}

NSString *WCTResult::getDescription() const
{
    return [NSString stringWithUTF8String:WCDB::ColumnResult::getDescription().c_str()];
}

WCTResult &WCTResult::as(const WCTProperty &property)
{
    WCDB::ColumnResult::as(property.getName());
    setBinding(property);
    return *this;
}

WCTResultList WCTResult::distinct() const
{
    return WCTResultList(*this).distinct();
}

WCTResultList::WCTResultList()
    : std::list<const WCTResult>()
    , m_distinct(false)
{
}

WCTResultList::WCTResultList(const WCTPropertyList &propertyList)
    : std::list<const WCTResult>(propertyList.begin(), propertyList.end())
    , m_distinct(false)
{
}

WCTResultList::WCTResultList(const WCTExprList &exprList)
    : std::list<const WCTResult>(exprList.begin(), exprList.end())
    , m_distinct(false)
{
}

WCTResultList::WCTResultList(std::initializer_list<const WCTExpr> il)
    : std::list<const WCTResult>(il.begin(), il.end())
    , m_distinct(false)
{
}

WCTResultList::WCTResultList(std::initializer_list<const WCTProperty> il)
    : std::list<const WCTResult>(il.begin(), il.end())
    , m_distinct(false)
{
}

WCTResultList &WCTResultList::distinct()
{
    m_distinct = true;
    return *this;
}

bool WCTResultList::isDistinct() const
{
    return m_distinct;
}

WCTResultList::WCTResultList(std::initializer_list<const WCTPropertyList> il)
    : std::list<const WCTResult>()
{
    for (const auto &propertyList : il) {
        for (const auto &property : propertyList) {
            push_back(property);
        }
    }
}
