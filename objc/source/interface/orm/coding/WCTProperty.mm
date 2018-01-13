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

#import <WCDB/WCTExpression.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTResult.h>

WCTPropertyNamed WCTProperty::PropertyNamed = ^(NSString *propertyName) {
  return WCTProperty(propertyName);
};

WCTProperty::WCTProperty(const char *name)
    : WCDB::Column(name)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTProperty::WCTProperty(NSString *name)
    : WCDB::Column(name ? name.UTF8String : "")
    , WCTPropertyBase(nil, nullptr)
{
}

WCTProperty::WCTProperty(NSString *name, Class cls, const std::shared_ptr<WCTColumnBinding> &columnBinding)
: WCDB::Column(name ? name.UTF8String : "")
, WCTPropertyBase(cls, columnBinding)
{
}

WCTProperty::WCTProperty(const char *name, Class cls, const std::shared_ptr<WCTColumnBinding> &columnBinding)
    : WCDB::Column(name)
    , WCTPropertyBase(cls, columnBinding)
{
}

WCTProperty::WCTProperty(const WCDB::Column &column, Class cls, const std::shared_ptr<WCTColumnBinding> &columnBinding)
    : WCDB::Column(column)
    , WCTPropertyBase(cls, columnBinding)
{
}

WCTResultList WCTProperty::distinct() const
{
    return WCTResultList(*this).distinct();
}

WCTProperty WCTProperty::inTable(NSString *table) const
{
    return WCTProperty(WCDB::Column::inTable(table.UTF8String), m_cls, m_columnBinding);
}

WCTOrderBy WCTProperty::order(WCTOrderTerm term) const
{
    return WCTOrderBy(*this, (WCDB::OrderTerm) term);
}

WCTIndex WCTProperty::index(WCTOrderTerm term) const
{
    return WCDB::ColumnIndex(*this, (WCDB::OrderTerm) term);
}

WCTExpression WCTProperty::avg(bool distinct) const
{
    return WCTExpression(*this).avg(distinct);
}

WCTExpression WCTProperty::count(bool distinct) const
{
    return WCTExpression(*this).count(distinct);
}

WCTExpression WCTProperty::groupConcat(bool distinct) const
{
    return WCTExpression(*this).groupConcat(distinct);
}

WCTExpression WCTProperty::groupConcat(NSString *seperator, bool distinct) const
{
    return WCTExpression(*this).groupConcat(seperator, distinct);
}

WCTExpression WCTProperty::max(bool distinct) const
{
    return WCTExpression(*this).max(distinct);
}

WCTExpression WCTProperty::min(bool distinct) const
{
    return WCTExpression(*this).min(distinct);
}

WCTExpression WCTProperty::sum(bool distinct) const
{
    return WCTExpression(*this).sum(distinct);
}

WCTExpression WCTProperty::total(bool distinct) const
{
    return WCTExpression(*this).total(distinct);
}

WCTExpression WCTProperty::abs(bool distinct) const
{
    return WCTExpression(*this).abs(distinct);
}

WCTExpression WCTProperty::hex(bool distinct) const
{
    return WCTExpression(*this).hex(distinct);
}

WCTExpression WCTProperty::length(bool distinct) const
{
    return WCTExpression(*this).length(distinct);
}

WCTExpression WCTProperty::lower(bool distinct) const
{
    return WCTExpression(*this).lower(distinct);
}

WCTExpression WCTProperty::upper(bool distinct) const
{
    return WCTExpression(*this).upper(distinct);
}

WCTExpression WCTProperty::round(bool distinct) const
{
    return WCTExpression(*this).round(distinct);
}

WCTExpression WCTProperty::matchinfo() const
{
    return WCTExpression(*this).matchinfo();
}

WCTExpression WCTProperty::offsets() const
{
    return WCTExpression(*this).offsets();
}

WCTExpression WCTProperty::snippet() const
{
    return WCTExpression(*this).snippet();
}

WCTColumnDef WCTProperty::def(WCTColumnType type, bool isPrimary, WCTOrderTerm term, bool autoIncrement) const
{
    WCDB::ColumnDef columnDef(*this, (WCDB::ColumnType) type);
    if (isPrimary) {
        columnDef.makePrimary((WCDB::OrderTerm) term, autoIncrement);
    }
    return columnDef;
}

WCTExpression WCTProperty::operator!() const
{
    return !WCTExpression(*this);
}

WCTExpression WCTProperty::operator+() const
{
    return +WCTExpression(*this);
}

WCTExpression WCTProperty::operator-() const
{
    return -WCTExpression(*this);
}

WCTExpression WCTProperty::operator~() const
{
    return ~WCTExpression(*this);
}

WCTExpression WCTProperty::operator||(const WCTExpression &operand) const
{
    return WCTExpression(*this) || operand;
}

WCTExpression WCTProperty::operator&&(const WCTExpression &operand) const
{
    return WCTExpression(*this) && operand;
}

WCTExpression WCTProperty::operator*(const WCTExpression &operand) const
{
    return WCTExpression(*this) * operand;
}

WCTExpression WCTProperty::operator/(const WCTExpression &operand) const
{
    return WCTExpression(*this) / operand;
}

WCTExpression WCTProperty::operator%(const WCTExpression &operand) const
{
    return WCTExpression(*this) % operand;
}

WCTExpression WCTProperty::operator+(const WCTExpression &operand) const
{
    return WCTExpression(*this) + operand;
}

WCTExpression WCTProperty::operator-(const WCTExpression &operand) const
{
    return WCTExpression(*this) - operand;
}

WCTExpression WCTProperty::operator<<(const WCTExpression &operand) const
{
    return WCTExpression(*this) << operand;
}

WCTExpression WCTProperty::operator>>(const WCTExpression &operand) const
{
    return WCTExpression(*this) >> operand;
}

WCTExpression WCTProperty::operator&(const WCTExpression &operand) const
{
    return WCTExpression(*this) & operand;
}

WCTExpression WCTProperty::operator|(const WCTExpression &operand) const
{
    return WCTExpression(*this) | operand;
}

WCTExpression WCTProperty::operator<(const WCTExpression &operand) const
{
    return WCTExpression(*this) < operand;
}

WCTExpression WCTProperty::operator<=(const WCTExpression &operand) const
{
    return WCTExpression(*this) <= operand;
}

WCTExpression WCTProperty::operator>(const WCTExpression &operand) const
{
    return WCTExpression(*this) > operand;
}

WCTExpression WCTProperty::operator>=(const WCTExpression &operand) const
{
    return WCTExpression(*this) >= operand;
}

WCTExpression WCTProperty::operator==(const WCTExpression &operand) const
{
    return WCTExpression(*this) == operand;
}

WCTExpression WCTProperty::operator!=(const WCTExpression &operand) const
{
    return WCTExpression(*this) != operand;
}

WCTExpression WCTProperty::concat(const WCTExpression &operand) const
{
    return WCTExpression(*this).concat(operand);
}

WCTExpression WCTProperty::substr(const WCTExpression &start, const WCTExpression &length) const
{
    return WCTExpression(*this).substr(start, length);
}

WCTExpression WCTProperty::in(const WCTExprList &exprList) const
{
    return WCTExpression(*this).in(exprList);
}

WCTExpression WCTProperty::notIn(const WCTExprList &exprList) const
{
    return WCTExpression(*this).notIn(exprList);
}

WCTExpression WCTProperty::in(const WCDB::StatementSelectList &statementSelectList) const
{
    return WCTExpression(*this).in(statementSelectList);
}

WCTExpression WCTProperty::notIn(const WCDB::StatementSelectList &statementSelectList) const
{
    return WCTExpression(*this).notIn(statementSelectList);
}

WCTExpression WCTProperty::in(NSString *table) const
{
    return WCTExpression(*this).in(table);
}

WCTExpression WCTProperty::notIn(NSString *table) const
{
    return WCTExpression(*this).notIn(table);
}

WCTExpression WCTProperty::in(NSArray<WCTValue *> *valueList) const
{
    return WCTExpression(*this).in(valueList);
}
WCTExpression WCTProperty::notIn(NSArray<WCTValue *> *valueList) const
{
    return WCTExpression(*this).notIn(valueList);
}

WCTExpression WCTProperty::between(const WCTExpression &left, const WCTExpression &right) const
{
    return WCTExpression(*this).between(left, right);
}

WCTExpression WCTProperty::notBetween(const WCTExpression &left, const WCTExpression &right) const
{
    return WCTExpression(*this).notBetween(left, right);
}

WCTExpression WCTProperty::like(const WCTExpression &operand) const
{
    return WCTExpression(*this).like(operand);
}

WCTExpression WCTProperty::glob(const WCTExpression &operand) const
{
    return WCTExpression(*this).glob(operand);
}

WCTExpression WCTProperty::match(const WCTExpression &operand) const
{
    return WCTExpression(*this).match(operand);
}

WCTExpression WCTProperty::regexp(const WCTExpression &operand) const
{
    return WCTExpression(*this).regexp(operand);
}

WCTExpression WCTProperty::notLike(const WCTExpression &operand) const
{
    return WCTExpression(*this).notLike(operand);
}

WCTExpression WCTProperty::notGlob(const WCTExpression &operand) const
{
    return WCTExpression(*this).notGlob(operand);
}

WCTExpression WCTProperty::notMatch(const WCTExpression &operand) const
{
    return WCTExpression(*this).notMatch(operand);
}

WCTExpression WCTProperty::notRegexp(const WCTExpression &operand) const
{
    return WCTExpression(*this).notRegexp(operand);
}

WCTExpression WCTProperty::like(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(*this).like(operand, escape);
}

WCTExpression WCTProperty::glob(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(*this).glob(operand, escape);
}

WCTExpression WCTProperty::match(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(*this).match(operand, escape);
}

WCTExpression WCTProperty::regexp(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(*this).regexp(operand, escape);
}

WCTExpression WCTProperty::notLike(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(*this).notLike(operand, escape);
}

WCTExpression WCTProperty::notGlob(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(*this).notGlob(operand, escape);
}

WCTExpression WCTProperty::notMatch(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(*this).notMatch(operand, escape);
}

WCTExpression WCTProperty::notRegexp(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(*this).notRegexp(operand, escape);
}

WCTExpression WCTProperty::isNull() const
{
    return WCTExpression(*this).isNull();
}

WCTExpression WCTProperty::isNotNull() const
{
    return WCTExpression(*this).isNotNull();
}

WCTExpression WCTProperty::is(const WCTExpression &operand) const
{
    return WCTExpression(*this).is(operand);
}

WCTExpression WCTProperty::isNot(const WCTExpression &operand) const
{
    return WCTExpression(*this).isNot(operand);
}

NSString *WCTProperty::getDescription() const
{
    return [NSString stringWithUTF8String:WCDB::Column::getDescription().c_str()];
}

WCTPropertyList::WCTPropertyList()
    : std::list<const WCTProperty>()
{
}

WCTPropertyList::WCTPropertyList(const WCTProperty &property)
    : std::list<const WCTProperty>({property})
{
}

WCTPropertyList::WCTPropertyList(std::initializer_list<const WCTProperty> il)
    : std::list<const WCTProperty>(il)
{
}

WCTPropertyList WCTPropertyList::inTable(NSString *tableName) const
{
    WCTPropertyList propertyList;
    for (auto iter : *this) {
        propertyList.push_back(iter.inTable(tableName));
    }
    return propertyList;
}
