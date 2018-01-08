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

typedef WCTProperty (^WCTPropertyNamed)(NSString *);

class WCTProperty : public WCDB::Column, public WCTPropertyBase {
public:
    WCTProperty(const char *name = "");
    WCTProperty(NSString *name = @"");
    WCTProperty(const char *name,
                Class cls,
                const std::shared_ptr<WCTColumnBinding> &columnBinding);

    //distinct
    WCTResultList distinct() const;

    //table
    WCTProperty inTable(NSString *table) const;

    //order
    WCTOrderBy order(WCTOrderTerm term = WCTOrderedNotSet) const;

    //index
    WCTIndex index(WCTOrderTerm term = WCTOrderedNotSet) const;

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
    //See http://www.sqlite.org/fts3.html#snippet for further information
    WCTExpression matchinfo() const;
    WCTExpression offsets() const;
    WCTExpression snippet() const;

    //def
    WCTColumnDef def(WCTColumnType type,
                     bool isPrimary = false,
                     WCTOrderTerm term = WCTOrderedNotSet,
                     bool autoIncrement = false) const;

    //condition
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
    WCTExpression
    in(const WCDB::StatementSelectList &statementSelectList) const;
    WCTExpression
    notIn(const WCDB::StatementSelectList &statementSelectList) const;
    WCTExpression in(NSArray<WCTValue *> *valueList) const;
    WCTExpression notIn(NSArray<WCTValue *> *valueList) const;
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

    NSString *getDescription() const;

    static WCTPropertyNamed PropertyNamed;

protected:
    WCTProperty(const WCDB::Column &column,
                Class cls,
                const std::shared_ptr<WCTColumnBinding> &columnBinding);
};

class WCTPropertyList : public std::list<const WCTProperty> {
public:
    WCTPropertyList();
    WCTPropertyList(const WCTProperty &property);
    WCTPropertyList(std::initializer_list<const WCTProperty> il);

    WCTPropertyList inTable(NSString *tableName) const;
};
