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

import XCTest
import WCDBSwift

class ExpresssionTests: BaseTestCase {

    func testExpression() {
        let integerExpression: Expression = 1
        WINQAssertEqual(integerExpression, "1")

        let floatExpression: Expression = 1.0
        WINQAssertEqual(floatExpression, "1.0")

        let stringExpression: Expression = "1"
        WINQAssertEqual(stringExpression, "'1'")

        let nilExpression: Expression = nil
        WINQAssertEqual(nilExpression, "NULL")

        let boolExpression: Expression = false
        WINQAssertEqual(boolExpression, "0")
    }

    func testOperable() {
        //Give
        let left: Expression = Column(named: "left").asExpression()
        let right: Expression = "right"

        //Then
        WINQAssertEqual(!left, "(NOT left)")
        WINQAssertEqual(+left, "(left)")
        WINQAssertEqual(-left, "(-left)")
        WINQAssertEqual(~left, "(~left)")

        WINQAssertEqual((left || right), "(left OR 'right')")

        WINQAssertEqual((left && right), "(left AND 'right')")

        WINQAssertEqual((left * right), "(left * 'right')")

        WINQAssertEqual((left / right), "(left / 'right')")

        WINQAssertEqual((left % right), "(left % 'right')")

        WINQAssertEqual((left + right), "(left + 'right')")

        WINQAssertEqual((left - right), "(left - 'right')")

        WINQAssertEqual((left << right), "(left << 'right')")

        WINQAssertEqual((left >> right), "(left >> 'right')")

        WINQAssertEqual((left & right), "(left & 'right')")

        WINQAssertEqual((left | right), "(left | 'right')")

        WINQAssertEqual((left < right), "(left < 'right')")

        WINQAssertEqual((left <= right), "(left <= 'right')")

        WINQAssertEqual((left > right), "(left > 'right')")

        WINQAssertEqual((left >= right), "(left >= 'right')")

        WINQAssertEqual((left == right), "(left = 'right')")

        WINQAssertEqual((left != right), "(left != 'right')")

        WINQAssertEqual(left.concat(right), "(left || 'right')")

        WINQAssertEqual(left.substr(start: 1, length: 2), "SUBSTR(left, 1, 2)")

        WINQAssertEqual(left.like(right), "(left LIKE 'right')")

        WINQAssertEqual(left.glob(right), "(left GLOB 'right')")

        WINQAssertEqual(left.match(right), "(left MATCH 'right')")

        WINQAssertEqual(left.regexp(right), "(left REGEXP 'right')")

        WINQAssertEqual(left.notLike(right), "(left NOT LIKE 'right')")

        WINQAssertEqual(left.notGlob(right), "(left NOT GLOB 'right')")

        WINQAssertEqual(left.notMatch(right), "(left NOT MATCH 'right')")

        WINQAssertEqual(left.notRegexp(right), "(left NOT REGEXP 'right')")

        WINQAssertEqual(left.like(right, escape: 2), "(left LIKE 'right' ESCAPE 2)")

        WINQAssertEqual(left.glob(right, escape: 2), "(left GLOB 'right' ESCAPE 2)")

        WINQAssertEqual(left.match(right, escape: 2), "(left MATCH 'right' ESCAPE 2)")

        WINQAssertEqual(left.regexp(right, escape: 2), "(left REGEXP 'right' ESCAPE 2)")

        WINQAssertEqual(left.notLike(right, escape: 2), "(left NOT LIKE 'right' ESCAPE 2)")

        WINQAssertEqual(left.notGlob(right, escape: 2), "(left NOT GLOB 'right' ESCAPE 2)")

        WINQAssertEqual(left.notMatch(right, escape: 2), "(left NOT MATCH 'right' ESCAPE 2)")

        WINQAssertEqual(left.notRegexp(right, escape: 2), "(left NOT REGEXP 'right' ESCAPE 2)")

        WINQAssertEqual(left.isNull(), "(left ISNULL)")

        WINQAssertEqual(left.isNotNull(), "(left NOTNULL)")

        WINQAssertEqual(left.`is`(right), "(left IS 'right')")

        WINQAssertEqual(left.isNot(right), "(left IS NOT 'right')")

        WINQAssertEqual(left.between(2, 10), "(left BETWEEN 2 AND 10)")

        WINQAssertEqual(left.notBetween(2, 10), "(left NOT BETWEEN 2 AND 10)")

        let statementSelect = StatementSelect().select(left).from("testExpressionTable")

        WINQAssertEqual(Expression.exists(statementSelect), "(EXISTS SELECT left FROM testExpressionTable)")

        WINQAssertEqual(Expression.notExists(statementSelect), "(NOT EXISTS SELECT left FROM testExpressionTable)")

        WINQAssertEqual(left.`in`(statementSelect), "(IN SELECT left FROM testExpressionTable)")

        WINQAssertEqual(left.notIn(statementSelect), "(NOT IN SELECT left FROM testExpressionTable)")

        WINQAssertEqual(left.`in`(right), "(left IN('right'))")

        WINQAssertEqual(left.notIn(right), "(left NOT IN('right'))")

        WINQAssertEqual(Expression.combine(left, right), "(left, 'right')")

        WINQAssertEqual(
            Expression.function(named: "testFunction", left, right, isDistinct: true),
            "testFunction(DISTINCT left, 'right')"
        )

        WINQAssertEqual(left.avg(), "AVG(left)")

        WINQAssertEqual(left.count(isDistinct: true), "COUNT(DISTINCT left)")

        WINQAssertEqual(left.groupConcat(), "GROUP_CONCAT(left)")

        WINQAssertEqual(left.groupConcat(isDistinct: true, separateBy: "-"), "GROUP_CONCAT(DISTINCT left, '-')")

        WINQAssertEqual(left.max(), "MAX(left)")

        WINQAssertEqual(left.min(), "MIN(left)")

        WINQAssertEqual(left.sum(), "SUM(left)")

        WINQAssertEqual(left.total(), "TOTAL(left)")

        WINQAssertEqual(left.abs(), "ABS(left)")

        WINQAssertEqual(left.hex(), "HEX(left)")

        WINQAssertEqual(left.length(), "LENGTH(left)")

        WINQAssertEqual(left.lower(), "LOWER(left)")

        WINQAssertEqual(left.upper(), "UPPER(left)")

        WINQAssertEqual(left.round(), "ROUND(left)")

        WINQAssertEqual(Expression.`case`(left, (when: 1, then: 2), else: 3), "CASE left WHEN 1 THEN 2 ELSE 3 END")

        WINQAssertEqual(left.matchinfo(), "MATCHINFO(left)")

        WINQAssertEqual(left.offsets(), "OFFSETS(left)")

        WINQAssertEqual(left.snippet(), "SNIPPET(left)")
    }
}
