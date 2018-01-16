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
#import <WCDB/WCTPropertyBase.h>
#import <WCDB/abstract.h>

class WCTExpression : public WCDB::Expression, public WCTPropertyBase {
public:
    WCTExpression();
    WCTExpression(const WCTProperty &column);

    WCTExpression(WCTValue *value);

    WCTExpression(WCTSelectBase *select);

    template <typename T>
    WCTExpression(
        const T &value,
        typename std::enable_if<std::is_arithmetic<T>::value ||
                                std::is_enum<T>::value>::type * = nullptr)
        : Expression(value), WCTPropertyBase(nil, nullptr)
    {
    }

    WCTExpression(const char *value);

    WCTExpression(const std::nullptr_t &);

    WCTExpression(
        const typename WCDB::ColumnTypeInfo<WCDB::ColumnType::BLOB>::UnderlyingType
            &value,
        int size);

    WCTResult as(const WCTProperty &property);

    WCTResultList distinct() const;

    //order
    WCTOrderBy order(WCTOrderTerm term = WCTOrderedNotSet) const;

    //unary
    WCTExpression operator!() const;
    WCTExpression operator+() const;
    WCTExpression operator-() const;
    WCTExpression operator~() const;

    //binary
    WCTExpression
    operator||(const WCTExpression &operand) const; //or, not concat
    WCTExpression operator&&(const WCTExpression &operand) const;
    WCTExpression operator*(const WCTExpression &operand) const;
    WCTExpression operator/(const WCTExpression &operand) const;
    WCTExpression operator%(const WCTExpression &operand) const;
    WCTExpression operator+(const WCTExpression &operand) const;
    WCTExpression operator-(const WCTExpression &operand) const;
    WCTExpression operator<<(const WCTExpression &operand) const;
    WCTExpression operator>>(const WCTExpression &operand) const;
    WCTExpression operator&(const WCTExpression &operand) const;
    WCTExpression operator|(const WCTExpression &operand) const;
    WCTExpression operator<(const WCTExpression &operand) const;
    WCTExpression operator<=(const WCTExpression &operand) const;
    WCTExpression operator>(const WCTExpression &operand) const;
    WCTExpression operator>=(const WCTExpression &operand) const;
    WCTExpression operator==(const WCTExpression &operand) const;
    WCTExpression operator!=(const WCTExpression &operand) const;

    WCTExpression concat(const WCTExpression &operand) const;
    WCTExpression substr(const WCTExpression &start,
                         const WCTExpression &length) const;

    WCTExpression in(const WCTExprList &exprList) const;
    WCTExpression notIn(const WCTExprList &exprList) const;
    WCTExpression in(NSArray<WCTValue *> *valueList) const;
    WCTExpression notIn(NSArray<WCTValue *> *valueList) const;
    WCTExpression
    in(const WCDB::StatementSelectList &statementSelectList) const;
    WCTExpression
    notIn(const WCDB::StatementSelectList &statementSelectList) const;
    WCTExpression in(NSString *table) const;
    WCTExpression notIn(NSString *table) const;
    WCTExpression between(const WCTExpression &left,
                          const WCTExpression &right) const;
    WCTExpression notBetween(const WCTExpression &left,
                             const WCTExpression &right) const;

    WCTExpression like(const WCTExpression &operand) const;
    WCTExpression glob(const WCTExpression &operand) const;
    WCTExpression match(const WCTExpression &operand) const;
    WCTExpression regexp(const WCTExpression &operand) const;
    WCTExpression notLike(const WCTExpression &operand) const;
    WCTExpression notGlob(const WCTExpression &operand) const;
    WCTExpression notMatch(const WCTExpression &operand) const;
    WCTExpression notRegexp(const WCTExpression &operand) const;

    WCTExpression like(const WCTExpression &operand,
                       const WCTExpression &escape) const;
    WCTExpression glob(const WCTExpression &operand,
                       const WCTExpression &escape) const;
    WCTExpression match(const WCTExpression &operand,
                        const WCTExpression &escape) const;
    WCTExpression regexp(const WCTExpression &operand,
                         const WCTExpression &escape) const;
    WCTExpression notLike(const WCTExpression &operand,
                          const WCTExpression &escape) const;
    WCTExpression notGlob(const WCTExpression &operand,
                          const WCTExpression &escape) const;
    WCTExpression notMatch(const WCTExpression &operand,
                           const WCTExpression &escape) const;
    WCTExpression notRegexp(const WCTExpression &operand,
                            const WCTExpression &escape) const;

    WCTExpression isNull() const;
    WCTExpression isNotNull() const;
    WCTExpression is(const WCTExpression &operand) const;
    WCTExpression isNot(const WCTExpression &operand) const;

    //aggregate functions
    WCTExpression avg(bool distinct = false) const;
    WCTExpression count(bool distinct = false) const;
    WCTExpression groupConcat(bool distinct = false) const;
    WCTExpression groupConcat(NSString *seperator, bool distinct = false) const;
    WCTExpression max(bool distinct = false) const;
    WCTExpression min(bool distinct = false) const;
    WCTExpression sum(bool distinct = false) const;
    WCTExpression total(bool distinct = false) const;

    //core functions
    WCTExpression abs(bool distinct = false) const;
    WCTExpression hex(bool distinct = false) const;
    WCTExpression length(bool distinct = false) const;
    WCTExpression lower(bool distinct = false) const;
    WCTExpression upper(bool distinct = false) const;
    WCTExpression round(bool distinct = false) const;

    //FTS3
    WCTExpression matchinfo() const;
    WCTExpression offsets() const;
    WCTExpression snippet() const;

    /**
     @brief Call other function
     
         //substr(columnName, 1, 2)
         WCTExprList exprList = {columnName, 1, 2};
         WCTExpression substrExpr = WCTExpression::Function(@"substr", exprList);
     
     @param function function name
     @param exprList param list
     @return WCTExpression
     */
    static WCTExpression Function(NSString *function,
                                  const WCTExprList &exprList);

    static WCTExpression
    Case(const WCTExpression &case_,
         const std::list<std::pair<WCTExpression, WCTExpression>> &when,
         const std::list<WCTExpression> &else_);

    WCTExpression(const WCDB::Expression &expr);
    WCTExpression(const WCDB::Expression &expr,
                  const WCTPropertyBase &propertyBase);

    NSString *getDescription() const;

protected:
    Class m_cls;
    WCDB::LiteralValue literalValue(WCTValue *value);
};

class WCTExprList : public std::list<const WCTExpression> {
public:
    WCTExprList();
    WCTExprList(const WCTExpression &expr);
    WCTExprList(std::initializer_list<const WCTExpression> il);
};
