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

        let columnExpression: Expression = Column(named: "testColumn").asExpression()
        WINQAssertEqual(columnExpression.asColumnResult(), "testColumn")
        WINQAssertEqual(columnExpression.asOrder(by: .descending), "testColumn DESC")
    }

    class OperableObject: TableCodable {
        var left: Int = 0
        var right: String = ""
        enum CodingKeys: String, CodingTableKey {
            typealias Root = OperableObject
            case left
            case right
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }
    }

    func testOperable() {
        //Give
        let left = OperableObject.Properties.left
        let right = OperableObject.Properties.right

        //Then
        WINQExpressionAssertEqual(!left, "(NOT left)")
        WINQExpressionAssertEqual(+left, "(left)")
        WINQExpressionAssertEqual(-left, "(-left)")
        WINQExpressionAssertEqual(~left, "(~left)")

        WINQExpressionAssertEqual(left.concat(right), "(left || right)")

        WINQExpressionAssertEqual(left.substr(start: 1, length: 2), "SUBSTR(left, 1, 2)")

        WINQExpressionAssertEqual(left.like(right), "(left LIKE right)")

        WINQExpressionAssertEqual(left.glob(right), "(left GLOB right)")

        WINQExpressionAssertEqual(left.match(right), "(left MATCH right)")

        WINQExpressionAssertEqual(left.regexp(right), "(left REGEXP right)")

        WINQExpressionAssertEqual(left.notLike(right), "(left NOT LIKE right)")

        WINQExpressionAssertEqual(left.notGlob(right), "(left NOT GLOB right)")

        WINQExpressionAssertEqual(left.notMatch(right), "(left NOT MATCH right)")

        WINQExpressionAssertEqual(left.notRegexp(right), "(left NOT REGEXP right)")

        WINQExpressionAssertEqual(left.like(right, escape: 2), "(left LIKE right ESCAPE 2)")

        WINQExpressionAssertEqual(left.glob(right, escape: 2), "(left GLOB right ESCAPE 2)")

        WINQExpressionAssertEqual(left.match(right, escape: 2), "(left MATCH right ESCAPE 2)")

        WINQExpressionAssertEqual(left.regexp(right, escape: 2), "(left REGEXP right ESCAPE 2)")

        WINQExpressionAssertEqual(left.notLike(right, escape: 2), "(left NOT LIKE right ESCAPE 2)")

        WINQExpressionAssertEqual(left.notGlob(right, escape: 2), "(left NOT GLOB right ESCAPE 2)")

        WINQExpressionAssertEqual(left.notMatch(right, escape: 2), "(left NOT MATCH right ESCAPE 2)")

        WINQExpressionAssertEqual(left.notRegexp(right, escape: 2), "(left NOT REGEXP right ESCAPE 2)")

        WINQExpressionAssertEqual(left.isNull(), "(left ISNULL)")

        WINQExpressionAssertEqual(left.isNotNull(), "(left NOTNULL)")

        WINQExpressionAssertEqual(left.`is`(right), "(left IS right)")

        WINQExpressionAssertEqual(left.isNot(right), "(left IS NOT right)")

        WINQExpressionAssertEqual(left.between(2, 10), "(left BETWEEN 2 AND 10)")

        WINQExpressionAssertEqual(left.notBetween(2, 10), "(left NOT BETWEEN 2 AND 10)")

        let statementSelect = StatementSelect().select(left).from("testExpressionTable")

        WINQExpressionAssertEqual(
            Expression.exists(statementSelect),
            "(EXISTS SELECT left FROM testExpressionTable)"
        )

        WINQExpressionAssertEqual(
            Expression.notExists(statementSelect),
            "(NOT EXISTS SELECT left FROM testExpressionTable)"
        )

        WINQExpressionAssertEqual(left.`in`(statementSelect), "(IN SELECT left FROM testExpressionTable)")

        WINQExpressionAssertEqual(left.notIn(statementSelect), "(NOT IN SELECT left FROM testExpressionTable)")

        WINQExpressionAssertEqual(left.`in`(right), "(left IN(right))")

        WINQExpressionAssertEqual(left.notIn(right), "(left NOT IN(right))")

        WINQExpressionAssertEqual(Expression.combine(left, right), "(left, right)")

        WINQExpressionAssertEqual(
            Expression.function(named: "testFunction", left, right, isDistinct: true),
            "testFunction(DISTINCT left, right)"
        )

        WINQExpressionAssertEqual(left.avg(), "AVG(left)")

        WINQExpressionAssertEqual(left.count(isDistinct: true), "COUNT(DISTINCT left)")

        WINQExpressionAssertEqual(left.groupConcat(), "GROUP_CONCAT(left)")

        WINQExpressionAssertEqual(
            left.groupConcat(isDistinct: true, separateBy: "-"),
            "GROUP_CONCAT(DISTINCT left, '-')"
        )

        WINQExpressionAssertEqual(left.max(), "MAX(left)")

        WINQExpressionAssertEqual(left.min(), "MIN(left)")

        WINQExpressionAssertEqual(left.sum(), "SUM(left)")

        WINQExpressionAssertEqual(left.total(), "TOTAL(left)")

        WINQExpressionAssertEqual(left.abs(), "ABS(left)")

        WINQExpressionAssertEqual(left.hex(), "HEX(left)")

        WINQExpressionAssertEqual(left.length(), "LENGTH(left)")

        WINQExpressionAssertEqual(left.lower(), "LOWER(left)")

        WINQExpressionAssertEqual(left.upper(), "UPPER(left)")

        WINQExpressionAssertEqual(left.round(), "ROUND(left)")

        WINQExpressionAssertEqual(
            Expression.`case`(left, (when: 1, then: 2), else: 3),
            "CASE left WHEN 1 THEN 2 ELSE 3 END"
        )

        WINQExpressionAssertEqual(left.matchinfo(), "MATCHINFO(left)")

        WINQExpressionAssertEqual(left.offsets(), "OFFSETS(left)")

        WINQExpressionAssertEqual(left.snippet(), "SNIPPET(left)")
    }

    func testOperableXOperable() {
        //Give
        let left = OperableObject.Properties.left
        let right = OperableObject.Properties.right

        //Then
        WINQExpressionAssertEqual((left || right), "(left OR right)")

        WINQExpressionAssertEqual((left && right), "(left AND right)")

        WINQExpressionAssertEqual((left * right), "(left * right)")

        WINQExpressionAssertEqual((left / right), "(left / right)")

        WINQExpressionAssertEqual((left % right), "(left % right)")

        WINQExpressionAssertEqual((left + right), "(left + right)")

        WINQExpressionAssertEqual((left - right), "(left - right)")

        WINQExpressionAssertEqual((left << right), "(left << right)")

        WINQExpressionAssertEqual((left >> right), "(left >> right)")

        WINQExpressionAssertEqual((left & right), "(left & right)")

        WINQExpressionAssertEqual((left | right), "(left | right)")

        WINQExpressionAssertEqual((left < right), "(left < right)")

        WINQExpressionAssertEqual((left <= right), "(left <= right)")

        WINQExpressionAssertEqual((left > right), "(left > right)")

        WINQExpressionAssertEqual((left >= right), "(left >= right)")

        WINQExpressionAssertEqual((left == right), "(left = right)")

        WINQExpressionAssertEqual((left != right), "(left != right)")
    }

    func testOperableXCanBeOperated() {
        //Give
        let left = OperableObject.Properties.left
        let right = 1

        //Then
        WINQExpressionAssertEqual((left || right), "(left OR 1)")

        WINQExpressionAssertEqual((left && right), "(left AND 1)")

        WINQExpressionAssertEqual((left * right), "(left * 1)")

        WINQExpressionAssertEqual((left / right), "(left / 1)")

        WINQExpressionAssertEqual((left % right), "(left % 1)")

        WINQExpressionAssertEqual((left + right), "(left + 1)")

        WINQExpressionAssertEqual((left - right), "(left - 1)")

        WINQExpressionAssertEqual((left << right), "(left << 1)")

        WINQExpressionAssertEqual((left >> right), "(left >> 1)")

        WINQExpressionAssertEqual((left & right), "(left & 1)")

        WINQExpressionAssertEqual((left | right), "(left | 1)")

        WINQExpressionAssertEqual((left < right), "(left < 1)")

        WINQExpressionAssertEqual((left <= right), "(left <= 1)")

        WINQExpressionAssertEqual((left > right), "(left > 1)")

        WINQExpressionAssertEqual((left >= right), "(left >= 1)")

        WINQExpressionAssertEqual((left == right), "(left = 1)")

        WINQExpressionAssertEqual((left != right), "(left != 1)")
    }

    func testCanBeOperatedXOperable() {
        //Give
        let left = 1
        let right = OperableObject.Properties.right

        //Then
        WINQExpressionAssertEqual((left || right), "(1 OR right)")

        WINQExpressionAssertEqual((left && right), "(1 AND right)")

        WINQExpressionAssertEqual((left * right), "(1 * right)")

        WINQExpressionAssertEqual((left / right), "(1 / right)")

        WINQExpressionAssertEqual((left % right), "(1 % right)")

        WINQExpressionAssertEqual((left + right), "(1 + right)")

        WINQExpressionAssertEqual((left - right), "(1 - right)")

        WINQExpressionAssertEqual((left << right), "(1 << right)")

        WINQExpressionAssertEqual((left >> right), "(1 >> right)")

        WINQExpressionAssertEqual((left & right), "(1 & right)")

        WINQExpressionAssertEqual((left | right), "(1 | right)")

        WINQExpressionAssertEqual((left < right), "(1 < right)")

        WINQExpressionAssertEqual((left <= right), "(1 <= right)")

        WINQExpressionAssertEqual((left > right), "(1 > right)")

        WINQExpressionAssertEqual((left >= right), "(1 >= right)")

        WINQExpressionAssertEqual((left == right), "(1 = right)")

        WINQExpressionAssertEqual((left != right), "(1 != right)")
    }
}
