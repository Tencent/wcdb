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
#import <WCDB/WCTExpr.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTResult.h>
#import <WCDB/WCTSelectBase+WCTExpr.h>
#import <WCDB/WCTValue.h>
#import <WCDB/error.hpp>

WCTExpr::WCTExpr()
    : WCDB::Expr()
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpr::WCTExpr(const WCTProperty &property)
    : WCDB::Expr(property)
    , WCTPropertyBase(property)
{
}

WCTExpr::WCTExpr(WCTSelectBase *select)
    : WCDB::Expr(select.statement)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpr::WCTExpr(const char *value)
    : WCDB::Expr(value)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpr::WCTExpr(const std::nullptr_t &value)
    : WCDB::Expr(value)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpr::WCTExpr(const typename WCDB::ColumnTypeInfo<WCDB::ColumnType::BLOB>::CType &value, int size)
    : WCDB::Expr(value, size)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpr::WCTExpr(WCTValue *value)
    : WCDB::Expr(literalValue(value))
    , WCTPropertyBase(nil, nullptr)
{
}

WCTExpr::WCTExpr(const WCDB::Expr &expr, const WCTPropertyBase &propertyBase)
    : WCDB::Expr(expr)
    , WCTPropertyBase(propertyBase)
{
}

WCTExpr::WCTExpr(const WCDB::Expr &expr)
    : WCDB::Expr(expr)
    , WCTPropertyBase(nil, nullptr)
{
}

WCTResult WCTExpr::as(const WCTProperty &property)
{
    return WCTResult(*this).as(property);
}

WCTResultList WCTExpr::distinct() const
{
    return WCTResultList(*this).distinct();
}

WCTOrderBy WCTExpr::order(WCTOrderTerm term) const
{
    return WCTOrderBy(*this, (WCDB::OrderTerm) term);
}

WCTExpr WCTExpr::operator!() const
{
    return WCTExpr(Expr::operator!(), *this);
}
WCTExpr WCTExpr::operator+() const
{
    return WCTExpr(Expr::operator+(), *this);
}
WCTExpr WCTExpr::operator-() const
{
    return WCTExpr(Expr::operator-(), *this);
}

WCTExpr WCTExpr::operator~() const
{
    return WCTExpr(Expr::operator~(), *this);
}

WCTExpr WCTExpr::operator||(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator||(operand), *this);
}
WCTExpr WCTExpr::operator&&(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator&&(operand), *this);
}
WCTExpr WCTExpr::operator*(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator*(operand), *this);
}
WCTExpr WCTExpr::operator/(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator/(operand), *this);
}
WCTExpr WCTExpr::operator%(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator%(operand), *this);
}
WCTExpr WCTExpr::operator+(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator+(operand), *this);
}
WCTExpr WCTExpr::operator-(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator-(operand), *this);
}
WCTExpr WCTExpr::operator<<(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator<<(operand), *this);
}
WCTExpr WCTExpr::operator>>(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator>>(operand), *this);
}
WCTExpr WCTExpr::operator&(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator&(operand), *this);
}
WCTExpr WCTExpr::operator|(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator|(operand), *this);
}
WCTExpr WCTExpr::operator<(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator<(operand), *this);
}
WCTExpr WCTExpr::operator<=(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator<=(operand), *this);
}
WCTExpr WCTExpr::operator>(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator>(operand), *this);
}
WCTExpr WCTExpr::operator>=(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator>=(operand), *this);
}
WCTExpr WCTExpr::operator==(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator==(operand), *this);
}
WCTExpr WCTExpr::operator!=(const WCTExpr &operand) const
{
    return WCTExpr(Expr::operator!=(operand), *this);
}

WCTExpr WCTExpr::concat(const WCTExpr &operand) const
{
    return WCTExpr(Expr::concat(operand));
}

WCTExpr WCTExpr::substr(const WCTExpr &start, const WCTExpr &length) const
{
    return WCTExpr(Expr::substr(start, length), *this);
}

WCTExpr WCTExpr::in(const WCTExprList &exprList) const
{
    return WCTExpr(Expr::in(exprList), *this);
}
WCTExpr WCTExpr::notIn(const WCTExprList &exprList) const
{
    WCTExpr expr;
    expr.m_description.append(m_description + " NOT IN(");
    expr.joinDescribableList(exprList);
    expr.m_description.append(")");
    return expr;
}
WCTExpr WCTExpr::in(const WCDB::StatementSelectList &statementSelectList) const
{
    return WCTExpr(Expr::in(statementSelectList), *this);
}
WCTExpr WCTExpr::notIn(const WCDB::StatementSelectList &statementSelectList) const
{
    return WCTExpr(Expr::notIn(statementSelectList), *this);
}
WCTExpr WCTExpr::in(NSString *table) const
{
    return WCTExpr(Expr::in(table.UTF8String), *this);
}
WCTExpr WCTExpr::notIn(NSString *table) const
{
    return WCTExpr(Expr::notIn(table.UTF8String), *this);
}
WCTExpr WCTExpr::in(NSArray<WCTValue *> *valueList) const
{
    WCTExprList exprList;
    for (WCTValue *value in valueList) {
        exprList.push_back(WCTExpr(value));
    }
    return WCTExpr::in(exprList);
}
WCTExpr WCTExpr::notIn(NSArray<WCTValue *> *valueList) const
{
    WCTExprList exprList;
    for (WCTValue *value in valueList) {
        exprList.push_back(WCTExpr(value));
    }
    return WCTExpr::notIn(exprList);
}
WCTExpr WCTExpr::between(const WCTExpr &left, const WCTExpr &right) const
{
    return WCTExpr(Expr::between(left, right), *this);
}
WCTExpr WCTExpr::notBetween(const WCTExpr &left, const WCTExpr &right) const
{
    return WCTExpr(Expr::notBetween(left, right), *this);
}

WCTExpr WCTExpr::like(const WCTExpr &operand) const
{
    return WCTExpr(Expr::like(operand), *this);
}
WCTExpr WCTExpr::glob(const WCTExpr &operand) const
{
    return WCTExpr(Expr::glob(operand), *this);
}
WCTExpr WCTExpr::match(const WCTExpr &operand) const
{
    return WCTExpr(Expr::match(operand), *this);
}
WCTExpr WCTExpr::regexp(const WCTExpr &operand) const
{
    return WCTExpr(Expr::regexp(operand), *this);
}
WCTExpr WCTExpr::notLike(const WCTExpr &operand) const
{
    return WCTExpr(Expr::notLike(operand), *this);
}
WCTExpr WCTExpr::notGlob(const WCTExpr &operand) const
{
    return WCTExpr(Expr::notGlob(operand), *this);
}
WCTExpr WCTExpr::notMatch(const WCTExpr &operand) const
{
    return WCTExpr(Expr::notMatch(operand), *this);
}
WCTExpr WCTExpr::notRegexp(const WCTExpr &operand) const
{
    return WCTExpr(Expr::notRegexp(operand), *this);
}

WCTExpr WCTExpr::like(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(Expr::like(operand, escape), *this);
}

WCTExpr WCTExpr::glob(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(Expr::glob(operand, escape), *this);
}

WCTExpr WCTExpr::match(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(Expr::match(operand, escape), *this);
}

WCTExpr WCTExpr::regexp(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(Expr::regexp(operand, escape), *this);
}

WCTExpr WCTExpr::notLike(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(Expr::notLike(operand, escape), *this);
}

WCTExpr WCTExpr::notGlob(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(Expr::notGlob(operand, escape), *this);
}

WCTExpr WCTExpr::notMatch(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(Expr::notMatch(operand, escape), *this);
}

WCTExpr WCTExpr::notRegexp(const WCTExpr &operand, const WCTExpr &escape) const
{
    return WCTExpr(Expr::notRegexp(operand, escape), *this);
}

WCTExpr WCTExpr::isNull() const
{
    return WCTExpr(Expr::isNull(), *this);
}
WCTExpr WCTExpr::isNotNull() const
{
    return WCTExpr(Expr::isNotNull(), *this);
}
WCTExpr WCTExpr::is(const WCTExpr &operand) const
{
    return WCTExpr(Expr::is(operand), *this);
}
WCTExpr WCTExpr::isNot(const WCTExpr &operand) const
{
    return WCTExpr(Expr::isNot(operand), *this);
}

WCTExpr WCTExpr::avg(bool distinct) const
{
    return WCTExpr(Expr::avg(distinct), *this);
}

WCTExpr WCTExpr::count(bool distinct) const
{
    return WCTExpr(Expr::count(distinct), *this);
}

WCTExpr WCTExpr::groupConcat(bool distinct) const
{
    return WCTExpr(Expr::groupConcat(distinct), *this);
}

WCTExpr WCTExpr::groupConcat(NSString *seperator, bool distinct) const
{
    return WCTExpr(Expr::groupConcat(seperator.UTF8String, distinct), *this);
}

WCTExpr WCTExpr::max(bool distinct) const
{
    return WCTExpr(Expr::max(distinct), *this);
}

WCTExpr WCTExpr::min(bool distinct) const
{
    return WCTExpr(Expr::min(distinct), *this);
}

WCTExpr WCTExpr::sum(bool distinct) const
{
    return WCTExpr(Expr::sum(distinct), *this);
}

WCTExpr WCTExpr::total(bool distinct) const
{
    return WCTExpr(Expr::total(distinct), *this);
}

WCTExpr WCTExpr::abs(bool distinct) const
{
    return WCTExpr(Expr::abs(distinct), *this);
}

WCTExpr WCTExpr::hex(bool distinct) const
{
    return WCTExpr(Expr::hex(distinct), *this);
}

WCTExpr WCTExpr::length(bool distinct) const
{
    return WCTExpr(Expr::length(distinct), *this);
}

WCTExpr WCTExpr::lower(bool distinct) const
{
    return WCTExpr(Expr::lower(distinct), *this);
}

WCTExpr WCTExpr::upper(bool distinct) const
{
    return WCTExpr(Expr::upper(distinct), *this);
}

WCTExpr WCTExpr::round(bool distinct) const
{
    return WCTExpr(Expr::round(distinct), *this);
}

WCTExpr WCTExpr::matchinfo() const
{
    return WCTExpr(Expr::matchinfo(), *this);
}

WCTExpr WCTExpr::offsets() const
{
    return WCTExpr(Expr::offsets(), *this);
}

WCTExpr WCTExpr::snippet() const
{
    return WCTExpr(Expr::snippet(), *this);
}

NSString *WCTExpr::getDescription() const
{
    return [NSString stringWithUTF8String:WCDB::Expr::getDescription().c_str()];
}

WCTExpr WCTExpr::Function(NSString *function, const WCTExprList &exprList)
{
    return WCTExpr(Expr::Function(function.UTF8String, exprList));
}

WCDB::LiteralValue WCTExpr::literalValue(WCTValue *value)
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

WCTExpr WCTExpr::Case(const WCTExpr &case_, const std::list<std::pair<WCTExpr, WCTExpr>> &when, const std::list<WCTExpr> &else_)
{
    return WCTExpr(Expr::Case(case_, when, else_));
}

WCTExprList::WCTExprList()
    : std::list<const WCTExpr>()
{
}

WCTExprList::WCTExprList(const WCTExpr &expr)
    : std::list<const WCTExpr>({expr})
{
}

WCTExprList::WCTExprList(std::initializer_list<const WCTExpr> il)
    : std::list<const WCTExpr>(il.begin(), il.end())
{
}
