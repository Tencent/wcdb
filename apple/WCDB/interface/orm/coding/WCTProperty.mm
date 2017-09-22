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

WCTExpr WCTProperty::avg(bool distinct) const
{
    return WCTExpr(*this).avg(distinct);
}

WCTExpr WCTProperty::count(bool distinct) const
{
    return WCTExpr(*this).count(distinct);
}

WCTExpr WCTProperty::groupConcat(bool distinct) const
{
    return WCTExpr(*this).groupConcat(distinct);
}

WCTExpr WCTProperty::groupConcat(NSString *seperator, bool distinct) const
{
    return WCTExpr(*this).groupConcat(seperator, distinct);
}

WCTExpr WCTProperty::max(bool distinct) const
{
    return WCTExpr(*this).max(distinct);
}

WCTExpr WCTProperty::min(bool distinct) const
{
    return WCTExpr(*this).min(distinct);
}

WCTExpr WCTProperty::sum(bool distinct) const
{
    return WCTExpr(*this).sum(distinct);
}

WCTExpr WCTProperty::total(bool distinct) const
{
    return WCTExpr(*this).total(distinct);
}

WCTExpr WCTProperty::abs(bool distinct) const
{
    return WCTExpr(*this).abs(distinct);
}

WCTExpr WCTProperty::hex(bool distinct) const
{
    return WCTExpr(*this).hex(distinct);
}

WCTExpr WCTProperty::length(bool distinct) const
{
    return WCTExpr(*this).length(distinct);
}

WCTExpr WCTProperty::lower(bool distinct) const
{
    return WCTExpr(*this).lower(distinct);
}

WCTExpr WCTProperty::upper(bool distinct) const
{
    return WCTExpr(*this).upper(distinct);
}

WCTExpr WCTProperty::round(bool distinct) const
{
    return WCTExpr(*this).round(distinct);
}

WCTExpr WCTProperty::matchinfo() const
{
    return WCTExpr(*this).matchinfo();
}

WCTExpr WCTProperty::offsets() const
{
    return WCTExpr(*this).offsets();
}

WCTExpr WCTProperty::snippet() const
{
    return WCTExpr(*this).snippet();
}

WCTColumnDef WCTProperty::def(WCTColumnType type, bool isPrimary, WCTOrderTerm term, bool autoIncrement) const
{
    WCDB::ColumnDef columnDef(*this, (WCDB::ColumnType) type);
    if (isPrimary) {
        columnDef.makePrimary((WCDB::OrderTerm) term, autoIncrement);
    }
    return columnDef;
}

WCTExpr WCTProperty::operator!() const
{
    return !WCTExpr(*this);
}

WCTExpr WCTProperty::operator+() const
{
    return +WCTExpr(*this);
}

WCTExpr WCTProperty::operator-() const
{
    return -WCTExpr(*this);
}

WCTExpr WCTProperty::operator~() const
{
    return ~WCTExpr(*this);
}

WCTExpr WCTProperty::operator||(const WCTExpr &operand) const
{
    return WCTExpr(*this) || operand;
}

WCTExpr WCTProperty::operator&&(const WCTExpr &operand) const
{
    return WCTExpr(*this) && operand;
}

WCTExpr WCTProperty::operator*(const WCTExpr &operand) const
{
    return WCTExpr(*this) * operand;
}

WCTExpr WCTProperty::operator/(const WCTExpr &operand) const
{
    return WCTExpr(*this) / operand;
}

WCTExpr WCTProperty::operator%(const WCTExpr &operand) const
{
    return WCTExpr(*this) % operand;
}

WCTExpr WCTProperty::operator+(const WCTExpr &operand) const
{
    return WCTExpr(*this) + operand;
}

WCTExpr WCTProperty::operator-(const WCTExpr &operand) const
{
    return WCTExpr(*this) - operand;
}

WCTExpr WCTProperty::operator<<(const WCTExpr &operand) const
{
    return WCTExpr(*this) << operand;
}

WCTExpr WCTProperty::operator>>(const WCTExpr &operand) const
{
    return WCTExpr(*this) >> operand;
}

WCTExpr WCTProperty::operator&(const WCTExpr &operand) const
{
    return WCTExpr(*this) & operand;
}

WCTExpr WCTProperty::operator|(const WCTExpr &operand) const
{
    return WCTExpr(*this) | operand;
}

WCTExpr WCTProperty::operator<(const WCTExpr &operand) const
{
    return WCTExpr(*this) < operand;
}

WCTExpr WCTProperty::operator<=(const WCTExpr &operand) const
{
    return WCTExpr(*this) <= operand;
}

WCTExpr WCTProperty::operator>(const WCTExpr &operand) const
{
    return WCTExpr(*this) > operand;
}

WCTExpr WCTProperty::operator>=(const WCTExpr &operand) const
{
    return WCTExpr(*this) >= operand;
}

WCTExpr WCTProperty::operator==(const WCTExpr &operand) const
{
    return WCTExpr(*this) == operand;
}

WCTExpr WCTProperty::operator!=(const WCTExpr &operand) const
{
    return WCTExpr(*this) != operand;
}

WCTExpr WCTProperty::concat(const WCTExpr &operand) const
{
    return WCTExpr(*this).concat(operand);
}

WCTExpr WCTProperty::substr(const WCTExpr &start, const WCTExpr &length) const
{
    return WCTExpr(*this).substr(start, length);
}

WCTExpr WCTProperty::in(const WCTExprList &exprList) const
{
    return WCTExpr(*this).in(exprList);
}

WCTExpr WCTProperty::notIn(const WCTExprList &exprList) const
{
    return WCTExpr(*this).notIn(exprList);
}

WCTExpr WCTProperty::in(const WCDB::StatementSelectList &statementSelectList) const
{
    return WCTExpr(*this).in(statementSelectList);
}

WCTExpr WCTProperty::notIn(const WCDB::StatementSelectList &statementSelectList) const
{
    return WCTExpr(*this).notIn(statementSelectList);
}

WCTExpr WCTProperty::in(NSString *table) const
{
    return WCTExpr(*this).in(table);
}

WCTExpr WCTProperty::notIn(NSString *table) const
{
    return WCTExpr(*this).notIn(table);
}

WCTExpr WCTProperty::in(NSArray<WCTValue *> *valueList) const
{
    return WCTExpr(*this).in(valueList);
}
WCTExpr WCTProperty::notIn(NSArray<WCTValue *> *valueList) const
{
    return WCTExpr(*this).notIn(valueList);
}

WCTExpr WCTProperty::between(const WCTExpr &left, const WCTExpr &right) const
{
    return WCTExpr(*this).between(left, right);
}

WCTExpr WCTProperty::notBetween(const WCTExpr &left, const WCTExpr &right) const
{
    return WCTExpr(*this).notBetween(left, right);
}

WCTExpr WCTProperty::like(const WCTExpr &operand) const
{
    return WCTExpr(*this).like(operand);
}

WCTExpr WCTProperty::glob(const WCTExpr &operand) const
{
    return WCTExpr(*this).glob(operand);
}

WCTExpr WCTProperty::match(const WCTExpr &operand) const
{
    return WCTExpr(*this).match(operand);
}

WCTExpr WCTProperty::regexp(const WCTExpr &operand) const
{
    return WCTExpr(*this).regexp(operand);
}

WCTExpr WCTProperty::notLike(const WCTExpr &operand) const
{
    return WCTExpr(*this).notLike(operand);
}

WCTExpr WCTProperty::notGlob(const WCTExpr &operand) const
{
    return WCTExpr(*this).notGlob(operand);
}

WCTExpr WCTProperty::notMatch(const WCTExpr &operand) const
{
    return WCTExpr(*this).notMatch(operand);
}

WCTExpr WCTProperty::notRegexp(const WCTExpr &operand) const
{
    return WCTExpr(*this).notRegexp(operand);
}

WCTExpr WCTProperty::like(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(*this).like(operand, escape);
}

WCTExpr WCTProperty::glob(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(*this).glob(operand, escape);
}

WCTExpr WCTProperty::match(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(*this).match(operand, escape);
}

WCTExpr WCTProperty::regexp(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(*this).regexp(operand, escape);
}

WCTExpr WCTProperty::notLike(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(*this).notLike(operand, escape);
}

WCTExpr WCTProperty::notGlob(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(*this).notGlob(operand, escape);
}

WCTExpr WCTProperty::notMatch(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(*this).notMatch(operand, escape);
}

WCTExpr WCTProperty::notRegexp(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(*this).notRegexp(operand, escape);
}

WCTExpr WCTProperty::isNull() const
{
    return WCTExpr(*this).isNull();
}

WCTExpr WCTProperty::isNotNull() const
{
    return WCTExpr(*this).isNotNull();
}

WCTExpr WCTProperty::is(const WCTExpr &operand) const
{
    return WCTExpr(*this).is(operand);
}

WCTExpr WCTProperty::isNot(const WCTExpr &operand) const
{
    return WCTExpr(*this).isNot(operand);
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
