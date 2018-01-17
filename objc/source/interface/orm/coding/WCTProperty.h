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

#import <WCDB/WCTDeclare.h>
#import <WCDB/abstract.h>

class WCTColumnBinding;

namespace WCDB {
    
class Property: public Describable, public Operable
{
public:   
    Property(const std::string& name, const std::shared_ptr<WCTColumnBinding> &columnBinding);
    
    Property inTable(NSString *tableName) const;
    Property inTable(const std::string& tableName) const;

    const std::shared_ptr<WCTColumnBinding>& getColumnBinding() const;
    
    virtual Expression asExpression() const override; 
protected:    
    std::shared_ptr<WCTColumnBinding> m_columnBinding;
};
    
template <>
struct ColumnConvertible<Property> : public std::true_type
{
public:
    static Column asColumn(const Property &property);
};

template <>
struct ExpressionConvertible<Property> : public std::true_type
{
public:
    static Expression asExpression(const Property &property);
};

template <>
struct ColumnResultConvertible<Property> : public std::true_type
{
public:
    static ColumnResult asColumnResult(const Property &property);
};

template <>
struct SpecificOrderConvertible<Property> : public std::true_type
{
public:
    static Order asOrder(const Property &property, OrderTerm term);
};

template <>
struct OrderConvertible<Property> : public std::true_type
{
public:
    static Order asOrder(const Property &property);
};

template <>
struct SpecificColumnIndexConvertible<Property> : public std::true_type
{
public:
    static ColumnIndex asIndex(const Property &property, OrderTerm term) ;
};

template <>
struct ColumnIndexConvertible<Property> : public std::true_type
{
public:
    static ColumnIndex asIndex(const Property &property) ;
};

template <>
struct SpecificColumnDefConvertible<Property> : public std::true_type
{
public:
    static ColumnDef asDef(const Property &property, ColumnType columnType);
};

//TODO
//template <>
//Property Operable<Property>::operator !() const;
//template <>
//Property Operable<Property>::operator +() const;
//template <>
//Property Operable<Property>::operator -() const;
//template <>
//Property Operable<Property>::operator ~() const;
//
//template <>
//Property Operable<Property>::operator ||(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator &&(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator *(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator /(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator %(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator +(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator -(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator <<(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator >>(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator &(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator |(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator <(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator <=(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator >(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator >=(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator ==(const Expression& operand) const;
//template <>
//Property Operable<Property>::operator !=(const Expression& operand) const;
//
//template <>
//Property Operable<Property>::concat(const Expression& operand) const;
//template <>
//Property Operable<Property>::substr(const Expression& start, const Expression& length) const;
//template <>
//Property Operable<Property>::like(const Expression& operand) const;
//template <>
//Property Operable<Property>::glob(const Expression& operand) const;
//template <>
//Property Operable<Property>::match(const Expression& operand) const;
//template <>
//Property Operable<Property>::regexp(const Expression& operand) const;
//template <>
//Property Operable<Property>::notLike(const Expression& operand) const;
//template <>
//Property Operable<Property>::notGlob(const Expression& operand) const;
//template <>
//Property Operable<Property>::notMatch(const Expression& operand) const;
//template <>
//Property Operable<Property>::notRegexp(const Expression& operand) const;
//template <>
//Property Operable<Property>::like(const Expression& operand, const Expression& escape) const;
//template <>
//Property Operable<Property>::glob(const Expression& operand, const Expression& escape) const;
//template <>
//Property Operable<Property>::match(const Expression& operand, const Expression& escape) const;
//template <>
//Property Operable<Property>::regexp(const Expression& operand, const Expression& escape) const;
//template <>
//Property Operable<Property>::notLike(const Expression& operand, const Expression& escape) const;
//template <>
//Property Operable<Property>::notGlob(const Expression& operand, const Expression& escape) const;
//template <>
//Property Operable<Property>::notMatch(const Expression& operand, const Expression& escape) const;
//template <>
//Property Operable<Property>::notRegexp(const Expression& operand, const Expression& escape) const;
//template <>
//Property Operable<Property>::isNull() const;
//template <>
//Property Operable<Property>::isNotNull() const;
//template <>
//Property Operable<Property>::is(const Expression& operand) const;
//template <>
//Property Operable<Property>::isNot(const Expression& operand) const;
//template <>
//Property Operable<Property>::between(const Expression& begin, const Expression& end) const;
//template <>
//Property Operable<Property>::notBetween(const Expression& begin, const Expression& end) const;
//template <>
//Property Operable<Property>::in(const StatementSelect& statementSelect) const;
//template <>
//Property Operable<Property>::notIn(const StatementSelect&  statementSelect) const;
//
//template <>
//template <typename U>
//typename std::enable_if<ExpressionConvertible<U>::value, Expression>::type Operable<Expression>::in(const std::list<const U> &list) const {
//    return Property(ExpressionConvertible<Property>::asExpression(*this)) Operator::operateWithPostfix(*this, "IN(" + stringByJoiningList(list) + ")");
//}
//
//template <>
//template <typename U>
//typename std::enable_if<ExpressionConvertible<U>::value, Expression>::type Operable<Expression>::notIn(const std::list<const U> &list) const {
//    return Operator::operateWithPostfix(*this, "NOT IN(" + stringByJoiningList(list) + ")");
//}
//    
//template <>
//Property Operable<Property>::in(const std::list<const Expression> &list) const;
//template <>
//Property Operable<Property>::notIn(const std::list<const Expression> &list) const;
//template <>
//Property Operable<Property>::in(const Expression &expression) const;
//template <>
//Property Operable<Property>::notIn(const Expression &expression) const;
//
//template <>
//Property Operable<Property>::avg(bool isDistinct) const;
//template <>
//Property Operable<Property>::count(bool isDistinct) const;
//template <>
//Property Operable<Property>::groupConcat(bool isDistinct) const;
//template <>
//Property Operable<Property>::groupConcat(bool isDistinct, const std::string& seperator) const;
//template <>
//Property Operable<Property>::max(bool isDistinct) const;
//template <>
//Property Operable<Property>::min(bool isDistinct) const;
//template <>
//Property Operable<Property>::sum(bool isDistinct) const;
//template <>
//Property Operable<Property>::total(bool isDistinct) const;
//template <>
//Property Operable<Property>::abs(bool isDistinct) const;
//template <>
//Property Operable<Property>::hex(bool isDistinct) const;
//template <>
//Property Operable<Property>::length(bool isDistinct) const;
//template <>
//Property Operable<Property>::lower(bool isDistinct) const;
//template <>
//Property Operable<Property>::upper(bool isDistinct) const;
//template <>
//Property Operable<Property>::round(bool isDistinct) const;
//template <>
//Property Operable<Property>::matchinfo() const;
//template <>
//Property Operable<Property>::offsets() const;
//template <>
//Property Operable<Property>::snippet() const;
    
} //namespace WCDB

typedef WCDB::Property WCTProperty;
typedef std::list<const WCDB::Property> WCTPropertyList;
