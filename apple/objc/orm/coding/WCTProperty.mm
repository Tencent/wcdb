//
// Created by sanhuazhang on 2019/05/02
//

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

#import <WCDB/Assertion.hpp>
#import <WCDB/WCTColumnBinding.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTResultColumn.h>

#pragma mark - WCTProperty

WCTProperty::WCTProperty() = default;
WCTProperty::~WCTProperty() = default;

WCTProperty::WCTProperty(const WCTColumnBinding& columnBinding)
: WCDB::Column(columnBinding.getPropertyName())
, WCTColumnBindingHolder(columnBinding)
{
}

WCTProperty::WCTProperty(const WCDB::UnsafeStringView& name, const WCTColumnBinding& columnBinding)
: WCDB::Column(name)
, WCTColumnBindingHolder(columnBinding)
{
}

WCDB::IndexedColumn WCTProperty::asIndex() const
{
    return WCDB::IndexedColumn(*this);
}

WCDB::IndexedColumn WCTProperty::asIndex(const WCTOrder& order) const
{
    return WCDB::IndexedColumn(*this).order(order);
}

WCDB::OrderingTerm WCTProperty::asOrder() const
{
    return WCDB::OrderingTerm(*this);
}

WCDB::OrderingTerm WCTProperty::asOrder(const WCTOrder& order) const
{
    return WCDB::OrderingTerm(*this).order(order);
}

WCDB::Column WCTProperty::table(const WCDB::UnsafeStringView& table) const
{
    return WCDB::Column(*this).table(table);
}

#pragma mark - WCTProperties
WCTProperties::~SyntaxList() = default;

WCDB::Expression WCTProperties::count() const
{
    return Expression::function("count").invokeAll();
}

WCTResultColumns WCTProperties::redirect(const WCDB::ResultColumns& resultColumns) const
{
    WCTResultColumns result;
    auto property = begin();
    auto resultColumn = resultColumns.begin();
    while (property != end() && resultColumn != resultColumns.end()) {
        result.push_back(WCTResultColumn(*resultColumn, property->getColumnBinding()));
        ++property;
        ++resultColumn;
    }
    return result;
}

WCTResultColumns WCTProperties::redirect(WCTRedirectBlock block) const
{
    WCTRemedialAssert(block != nil, "Redirect block can't be null.", return *this;);
    WCTResultColumns results;
    for (const auto& property : *this) {
        results.push_back(WCTResultColumn(block(property), property.getColumnBinding()));
    }
    return results;
}

WCDB::Expressions WCTProperties::table(NSString* table) const
{
    WCDB::Expressions expressions;
    for (const auto& property : *this) {
        expressions.push_back(property.table(table));
    }
    return expressions;
}

WCTProperties WCTProperties::propertiesByAddingNewProperties(const WCTProperties& properties) const
{
    WCTProperties newProperties = *this;
    newProperties.insert(newProperties.begin(), properties.begin(), properties.end());
    return newProperties;
}

WCTProperties& WCTProperties::addingNewProperties(const WCTProperties& properties)
{
    insert(end(), properties.begin(), properties.end());
    return *this;
}

bool WCTProperties::isEqual(const WCTProperty& left, const WCTProperty& right)
{
    return left.getColumnBinding() == right.getColumnBinding() && left.getDescription() == right.getDescription();
}

WCTProperties WCTProperties::propertiesByRemovingProperties(const WCTProperties& properties) const
{
    WCTProperties newProperties;
    for (const auto& property : *this) {
        if (std::find_if(properties.begin(), properties.end(), std::bind(&WCTProperties::isEqual, property, std::placeholders::_1)) == properties.end()) {
            newProperties.push_back(property);
        }
    }
    return newProperties;
}

WCTProperties& WCTProperties::removingProperties(const WCTProperties& properties)
{
    for (const auto& property : properties) {
        auto iter = std::find_if(begin(), end(), std::bind(&WCTProperties::isEqual, property, std::placeholders::_1));
        if (iter != end()) {
            erase(iter);
        }
    }
    return *this;
}
