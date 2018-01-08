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

#import <WCDB/WCTCoding.h>
#import <WCDB/WCTExpression.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTResult.h>
#import <WCDB/WCTSelectBase+WCTExpression.h>
#import <WCDB/WCTValue.h>
#import <WCDB/error.hpp>

WCTExpression::WCTExpression()
    : WCDB::Expression()
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpression::WCTExpression(const WCTProperty &property)
    : WCDB::Expression(property)
    , WCTPropertyBase(property)
{
}

WCTExpression::WCTExpression(WCTSelectBase *select)
    : WCDB::Expression(select.statement)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpression::WCTExpression(const char *value)
    : WCDB::Expression(value)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpression::WCTExpression(const std::nullptr_t &value)
    : WCDB::Expression(value)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpression::WCTExpression(const typename WCDB::ColumnTypeInfo<WCDB::ColumnType::BLOB>::CType &value, int size)
    : WCDB::Expression(value, size)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpression::WCTExpression(WCTValue *value)
    : WCDB::Expression(literalValue(value))
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpression::WCTExpression(const WCDB::Expression &expr, const WCTPropertyBase &propertyBase)
    : WCDB::Expression(expr)
    , WCTPropertyBase(propertyBase)
{
}

WCTExpression::WCTExpression(const WCDB::Expression &expr)
    : WCDB::Expression(expr)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTResult WCTExpression::as(const WCTProperty &property)
{
    return WCTResult(*this).as(property);
}

WCTResultList WCTExpression::distinct() const
{
    return WCTResultList(*this).distinct();
}

WCTOrderBy WCTExpression::order(WCTOrderTerm term) const
{
    return WCTOrderBy(*this, (WCDB::OrderTerm) term);
}

WCTExpression WCTExpression::operator!() const
{
    return WCTExpression(Expression::operator!(), *this);
}
WCTExpression WCTExpression::operator+() const
{
    return WCTExpression(Expression::operator+(), *this);
}
WCTExpression WCTExpression::operator-() const
{
    return WCTExpression(Expression::operator-(), *this);
}

WCTExpression WCTExpression::operator~() const
{
    return WCTExpression(Expression::operator~(), *this);
}

WCTExpression WCTExpression::operator||(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator||(operand), *this);
}
WCTExpression WCTExpression::operator&&(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator&&(operand), *this);
}
WCTExpression WCTExpression::operator*(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator*(operand), *this);
}
WCTExpression WCTExpression::operator/(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator/(operand), *this);
}
WCTExpression WCTExpression::operator%(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator%(operand), *this);
}
WCTExpression WCTExpression::operator+(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator+(operand), *this);
}
WCTExpression WCTExpression::operator-(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator-(operand), *this);
}
WCTExpression WCTExpression::operator<<(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator<<(operand), *this);
}
WCTExpression WCTExpression::operator>>(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator>>(operand), *this);
}
WCTExpression WCTExpression::operator&(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator&(operand), *this);
}
WCTExpression WCTExpression::operator|(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator|(operand), *this);
}
WCTExpression WCTExpression::operator<(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator<(operand), *this);
}
WCTExpression WCTExpression::operator<=(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator<=(operand), *this);
}
WCTExpression WCTExpression::operator>(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator>(operand), *this);
}
WCTExpression WCTExpression::operator>=(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator>=(operand), *this);
}
WCTExpression WCTExpression::operator==(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator==(operand), *this);
}
WCTExpression WCTExpression::operator!=(const WCTExpression &operand) const
{
    return WCTExpression(Expression::operator!=(operand), *this);
}

WCTExpression WCTExpression::concat(const WCTExpression &operand) const
{
    return WCTExpression(Expression::concat(operand));
}

WCTExpression WCTExpression::substr(const WCTExpression &start, const WCTExpression &length) const
{
    return WCTExpression(Expression::substr(start, length), *this);
}

WCTExpression WCTExpression::in(const WCTExprList &exprList) const
{
    return WCTExpression(Expression::in(exprList), *this);
}
WCTExpression WCTExpression::notIn(const WCTExprList &exprList) const
{
    WCTExpression expr;
    expr.m_description.append(m_description + " NOT IN(");
    expr.joinDescribableList(exprList);
    expr.m_description.append(")");
    return expr;
}
WCTExpression WCTExpression::in(const WCDB::StatementSelectList &statementSelectList) const
{
    return WCTExpression(Expression::in(statementSelectList), *this);
}
WCTExpression WCTExpression::notIn(const WCDB::StatementSelectList &statementSelectList) const
{
    return WCTExpression(Expression::notIn(statementSelectList), *this);
}
WCTExpression WCTExpression::in(NSString *table) const
{
    return WCTExpression(Expression::in(table.UTF8String), *this);
}
WCTExpression WCTExpression::notIn(NSString *table) const
{
    return WCTExpression(Expression::notIn(table.UTF8String), *this);
}
WCTExpression WCTExpression::in(NSArray<WCTValue *> *valueList) const
{
    WCTExprList exprList;
    for (WCTValue *value in valueList) {
        exprList.push_back(WCTExpression(value));
    }
    return WCTExpression::in(exprList);
}
WCTExpression WCTExpression::notIn(NSArray<WCTValue *> *valueList) const
{
    WCTExprList exprList;
    for (WCTValue *value in valueList) {
        exprList.push_back(WCTExpression(value));
    }
    return WCTExpression::notIn(exprList);
}
WCTExpression WCTExpression::between(const WCTExpression &left, const WCTExpression &right) const
{
    return WCTExpression(Expression::between(left, right), *this);
}
WCTExpression WCTExpression::notBetween(const WCTExpression &left, const WCTExpression &right) const
{
    return WCTExpression(Expression::notBetween(left, right), *this);
}

WCTExpression WCTExpression::like(const WCTExpression &operand) const
{
    return WCTExpression(Expression::like(operand), *this);
}
WCTExpression WCTExpression::glob(const WCTExpression &operand) const
{
    return WCTExpression(Expression::glob(operand), *this);
}
WCTExpression WCTExpression::match(const WCTExpression &operand) const
{
    return WCTExpression(Expression::match(operand), *this);
}
WCTExpression WCTExpression::regexp(const WCTExpression &operand) const
{
    return WCTExpression(Expression::regexp(operand), *this);
}
WCTExpression WCTExpression::notLike(const WCTExpression &operand) const
{
    return WCTExpression(Expression::notLike(operand), *this);
}
WCTExpression WCTExpression::notGlob(const WCTExpression &operand) const
{
    return WCTExpression(Expression::notGlob(operand), *this);
}
WCTExpression WCTExpression::notMatch(const WCTExpression &operand) const
{
    return WCTExpression(Expression::notMatch(operand), *this);
}
WCTExpression WCTExpression::notRegexp(const WCTExpression &operand) const
{
    return WCTExpression(Expression::notRegexp(operand), *this);
}

WCTExpression WCTExpression::like(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(Expression::like(operand, escape), *this);
}

WCTExpression WCTExpression::glob(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(Expression::glob(operand, escape), *this);
}

WCTExpression WCTExpression::match(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(Expression::match(operand, escape), *this);
}

WCTExpression WCTExpression::regexp(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(Expression::regexp(operand, escape), *this);
}

WCTExpression WCTExpression::notLike(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(Expression::notLike(operand, escape), *this);
}

WCTExpression WCTExpression::notGlob(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(Expression::notGlob(operand, escape), *this);
}

WCTExpression WCTExpression::notMatch(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(Expression::notMatch(operand, escape), *this);
}

WCTExpression WCTExpression::notRegexp(const WCTExpression &operand, const WCTExpression &escape) const
{
    return WCTExpression(Expression::notRegexp(operand, escape), *this);
}

WCTExpression WCTExpression::isNull() const
{
    return WCTExpression(Expression::isNull(), *this);
}
WCTExpression WCTExpression::isNotNull() const
{
    return WCTExpression(Expression::isNotNull(), *this);
}
WCTExpression WCTExpression::is(const WCTExpression &operand) const
{
    return WCTExpression(Expression::is(operand), *this);
}
WCTExpression WCTExpression::isNot(const WCTExpression &operand) const
{
    return WCTExpression(Expression::isNot(operand), *this);
}

WCTExpression WCTExpression::avg(bool distinct) const
{
    return WCTExpression(Expression::avg(distinct), *this);
}

WCTExpression WCTExpression::count(bool distinct) const
{
    return WCTExpression(Expression::count(distinct), *this);
}

WCTExpression WCTExpression::groupConcat(bool distinct) const
{
    return WCTExpression(Expression::groupConcat(distinct), *this);
}

WCTExpression WCTExpression::groupConcat(NSString *seperator, bool distinct) const
{
    return WCTExpression(Expression::groupConcat(seperator.UTF8String, distinct), *this);
}

WCTExpression WCTExpression::max(bool distinct) const
{
    return WCTExpression(Expression::max(distinct), *this);
}

WCTExpression WCTExpression::min(bool distinct) const
{
    return WCTExpression(Expression::min(distinct), *this);
}

WCTExpression WCTExpression::sum(bool distinct) const
{
    return WCTExpression(Expression::sum(distinct), *this);
}

WCTExpression WCTExpression::total(bool distinct) const
{
    return WCTExpression(Expression::total(distinct), *this);
}

WCTExpression WCTExpression::abs(bool distinct) const
{
    return WCTExpression(Expression::abs(distinct), *this);
}

WCTExpression WCTExpression::hex(bool distinct) const
{
    return WCTExpression(Expression::hex(distinct), *this);
}

WCTExpression WCTExpression::length(bool distinct) const
{
    return WCTExpression(Expression::length(distinct), *this);
}

WCTExpression WCTExpression::lower(bool distinct) const
{
    return WCTExpression(Expression::lower(distinct), *this);
}

WCTExpression WCTExpression::upper(bool distinct) const
{
    return WCTExpression(Expression::upper(distinct), *this);
}

WCTExpression WCTExpression::round(bool distinct) const
{
    return WCTExpression(Expression::round(distinct), *this);
}

WCTExpression WCTExpression::matchinfo() const
{
    return WCTExpression(Expression::matchinfo(), *this);
}

WCTExpression WCTExpression::offsets() const
{
    return WCTExpression(Expression::offsets(), *this);
}

WCTExpression WCTExpression::snippet() const
{
    return WCTExpression(Expression::snippet(), *this);
}

NSString *WCTExpression::getDescription() const
{
    return [NSString stringWithUTF8String:WCDB::Expression::getDescription().c_str()];
}

WCTExpression WCTExpression::Function(NSString *function, const WCTExprList &exprList)
{
    return WCTExpression(Expression::Function(function.UTF8String, exprList));
}

WCDB::LiteralValue WCTExpression::literalValue(WCTValue *value)
{
    WCTValueType valueType = [value valueType];
    if (valueType == WCTValueTypeColumnCoding) {
        value = [(id<WCTColumnCoding>) value archivedWCTValue];
        valueType = [value valueType];
    }
    switch (valueType) {
        case WCTValueTypeString: {
            NSString *string = (NSString *) value;
            return WCDB::LiteralValue(string.UTF8String);
        } break;
        case WCTValueTypeNumber: {
            NSNumber *number = (NSNumber *) value;
            if (CFNumberIsFloatType((CFNumberRef) number)) {
                return WCDB::LiteralValue(number.doubleValue);
            } else {
                if (CFNumberGetByteSize((CFNumberRef) number) <= 4) {
                    return WCDB::LiteralValue(number.intValue);
                } else {
                    return WCDB::LiteralValue(number.longLongValue);
                }
            }
        } break;
        case WCTValueTypeData: {
            NSData *data = (NSData *) value;
            return WCDB::LiteralValue(data.bytes, (int) data.length);
        } break;
        case WCTValueTypeNil: {
            return WCDB::LiteralValue(nullptr);
        } break;
        default:
            WCDB::Error::Abort(([NSString stringWithFormat:@"Converting an unknown class [%@]", NSStringFromClass(value.class)].UTF8String));
            break;
    }
    return "";
}

WCTExpression WCTExpression::Case(const WCTExpression &case_, const std::list<std::pair<WCTExpression, WCTExpression>> &when, const std::list<WCTExpression> &else_)
{
    return WCTExpression(Expression::Case(case_, when, else_));
}

WCTExprList::WCTExprList()
    : std::list<const WCTExpression>()
{
}

WCTExprList::WCTExprList(const WCTExpression &expr)
    : std::list<const WCTExpression>({expr})
{
}

WCTExprList::WCTExprList(std::initializer_list<const WCTExpression> il)
    : std::list<const WCTExpression>(il.begin(), il.end())
{
}
