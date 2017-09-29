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
    WCTExpr avg(bool distinct = false) const;
    WCTExpr count(bool distinct = false) const;
    WCTExpr groupConcat(bool distinct = false) const;
    WCTExpr groupConcat(NSString *seperator, bool distinct = false) const;
    WCTExpr max(bool distinct = false) const;
    WCTExpr min(bool distinct = false) const;
    WCTExpr sum(bool distinct = false) const;
    WCTExpr total(bool distinct = false) const;

    //core functions
    WCTExpr abs(bool distinct = false) const;
    WCTExpr hex(bool distinct = false) const;
    WCTExpr length(bool distinct = false) const;
    WCTExpr lower(bool distinct = false) const;
    WCTExpr upper(bool distinct = false) const;
    WCTExpr round(bool distinct = false) const;

    //FTS3
    //See http://www.sqlite.org/fts3.html#snippet for further information
    WCTExpr matchinfo() const;
    WCTExpr offsets() const;
    WCTExpr snippet() const;

    //def
    WCTColumnDef def(WCTColumnType type,
                     bool isPrimary = false,
                     WCTOrderTerm term = WCTOrderedNotSet,
                     bool autoIncrement = false) const;

    //condition
    //unary
    WCTExpr operator!() const;
    WCTExpr operator+() const;
    WCTExpr operator-() const;
    WCTExpr operator~() const;

    //binary
    WCTExpr operator||(const WCTExpr &operand) const; //or, not concat
    WCTExpr operator&&(const WCTExpr &operand) const;
    WCTExpr operator*(const WCTExpr &operand) const;
    WCTExpr operator/(const WCTExpr &operand) const;
    WCTExpr operator%(const WCTExpr &operand) const;
    WCTExpr operator+(const WCTExpr &operand) const;
    WCTExpr operator-(const WCTExpr &operand) const;
    WCTExpr operator<<(const WCTExpr &operand) const;
    WCTExpr operator>>(const WCTExpr &operand) const;
    WCTExpr operator&(const WCTExpr &operand) const;
    WCTExpr operator|(const WCTExpr &operand) const;
    WCTExpr operator<(const WCTExpr &operand) const;
    WCTExpr operator<=(const WCTExpr &operand) const;
    WCTExpr operator>(const WCTExpr &operand) const;
    WCTExpr operator>=(const WCTExpr &operand) const;
    WCTExpr operator==(const WCTExpr &operand) const;
    WCTExpr operator!=(const WCTExpr &operand) const;

    WCTExpr concat(const WCTExpr &operand) const;
    WCTExpr substr(const WCTExpr &start, const WCTExpr &length) const;

    WCTExpr in(const WCTExprList &exprList) const;
    WCTExpr notIn(const WCTExprList &exprList) const;
    WCTExpr in(const WCDB::StatementSelectList &statementSelectList) const;
    WCTExpr notIn(const WCDB::StatementSelectList &statementSelectList) const;
    WCTExpr in(NSArray<WCTValue *> *valueList) const;
    WCTExpr notIn(NSArray<WCTValue *> *valueList) const;
    WCTExpr in(NSString *table) const;
    WCTExpr notIn(NSString *table) const;
    WCTExpr between(const WCTExpr &left, const WCTExpr &right) const;
    WCTExpr notBetween(const WCTExpr &left, const WCTExpr &right) const;

    WCTExpr like(const WCTExpr &operand) const;
    WCTExpr glob(const WCTExpr &operand) const;
    WCTExpr match(const WCTExpr &operand) const;
    WCTExpr regexp(const WCTExpr &operand) const;
    WCTExpr notLike(const WCTExpr &operand) const;
    WCTExpr notGlob(const WCTExpr &operand) const;
    WCTExpr notMatch(const WCTExpr &operand) const;
    WCTExpr notRegexp(const WCTExpr &operand) const;

    WCTExpr like(const WCTExpr &operand, const WCTExpr &escape) const;
    WCTExpr glob(const WCTExpr &operand, const WCTExpr &escape) const;
    WCTExpr match(const WCTExpr &operand, const WCTExpr &escape) const;
    WCTExpr regexp(const WCTExpr &operand, const WCTExpr &escape) const;
    WCTExpr notLike(const WCTExpr &operand, const WCTExpr &escape) const;
    WCTExpr notGlob(const WCTExpr &operand, const WCTExpr &escape) const;
    WCTExpr notMatch(const WCTExpr &operand, const WCTExpr &escape) const;
    WCTExpr notRegexp(const WCTExpr &operand, const WCTExpr &escape) const;

    WCTExpr isNull() const;
    WCTExpr isNotNull() const;
    WCTExpr is(const WCTExpr &operand) const;
    WCTExpr isNot(const WCTExpr &operand) const;

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
