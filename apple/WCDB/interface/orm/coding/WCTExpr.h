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

class WCTExpr : public WCDB::Expr, public WCTPropertyBase {
public:
    WCTExpr();
    WCTExpr(const WCTProperty &column);

    WCTExpr(WCTValue *value);

    WCTExpr(WCTSelectBase *select);

    template <typename T>
    WCTExpr(const T &value,
            typename std::enable_if<std::is_arithmetic<T>::value ||
                                    std::is_enum<T>::value>::type * = nullptr)
        : Expr(value), WCTPropertyBase(nil, nullptr)
    {
    }

    WCTExpr(const char *value);

    WCTExpr(const std::nullptr_t &);

    WCTExpr(const typename WCDB::ColumnTypeInfo<WCDB::ColumnType::BLOB>::CType
                &value,
            int size);

    WCTResult as(const WCTProperty &property);

    WCTResultList distinct() const;

    //order
    WCTOrderBy order(WCTOrderTerm term = WCTOrderedNotSet) const;

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
    WCTExpr in(NSArray<WCTValue *> *valueList) const;
    WCTExpr notIn(NSArray<WCTValue *> *valueList) const;
    WCTExpr in(const WCDB::StatementSelectList &statementSelectList) const;
    WCTExpr notIn(const WCDB::StatementSelectList &statementSelectList) const;
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
    WCTExpr matchinfo() const;
    WCTExpr offsets() const;
    WCTExpr snippet() const;

    /**
     @brief Call other function
     
         //substr(columnName, 1, 2)
         WCTExprList exprList = {columnName, 1, 2};
         WCTExpr substrExpr = WCTExpr::Function(@"substr", exprList);
     
     @param function function name
     @param exprList param list
     @return WCTExpr
     */
    static WCTExpr Function(NSString *function, const WCTExprList &exprList);

    static WCTExpr Case(const WCTExpr &case_,
                        const std::list<std::pair<WCTExpr, WCTExpr>> &when,
                        const std::list<WCTExpr> &else_);

    WCTExpr(const WCDB::Expr &expr);
    WCTExpr(const WCDB::Expr &expr, const WCTPropertyBase &propertyBase);

    NSString *getDescription() const;

protected:
    Class m_cls;
    WCDB::LiteralValue literalValue(WCTValue *value);
};

class WCTExprList : public std::list<const WCTExpr> {
public:
    WCTExprList();
    WCTExprList(const WCTExpr &expr);
    WCTExprList(std::initializer_list<const WCTExpr> il);
};
