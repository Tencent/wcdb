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
import WCDB

class ExpresssionTests: BaseTestCase {

    func testExpression() {
        let integerExpression: Expression = 1
        WINQAssertEqual(integerExpression, "1")

        let floatExpression: Expression = 1.1
        WINQAssertEqual(floatExpression, "1.1000000000000001")

        let stringExpression: Expression = "1"
        WINQAssertEqual(stringExpression, "'1'")

        let nilExpression: Expression = nil
        WINQAssertEqual(nilExpression, "NULL")

        let boolExpression: Expression = false
        WINQAssertEqual(boolExpression, "FALSE")

        let columnExpression: Expression = Column(named: "testColumn").asExpression()
        WINQAssertEqual(columnExpression.asColumnResult(), "testColumn")
        WINQAssertEqual(columnExpression.order(.descending), "testColumn DESC")

        WINQAssertEqual(Expression(with: BindParameter(1)), "?1")

        WINQAssertEqual(Expression.cast(1).as(.integer64), "CAST(1 AS INTEGER)")

        WINQAssertEqual(Expression.windowFunction("testWindowFunction").invoke().arguments(1).filter(Filter().where(1)), "testWindowFunction(1) FILTER(WHERE 1)")
        WINQAssertEqual(Expression.windowFunction("testWindowFunction").invoke().arguments(1, 2).filter(Filter().where(1)).over("testWindow"), "testWindowFunction(1, 2) FILTER(WHERE 1) OVER testWindow")
        WINQAssertEqual(Expression.windowFunction("testWindowFunction").invoke().arguments(1, 2).filter(Filter().where(1)).over(WindowDef().partition(1, 2)), "testWindowFunction(1, 2) FILTER(WHERE 1) OVER(PARTITION BY 1, 2)")
    }

    func testCaseWhenThen() {
        WINQAssertEqual(Expression.case().when(2).then(3).else(4), "CASE WHEN 2 THEN 3 ELSE 4 END")
        WINQAssertEqual(Expression.case(1).when(2).then(3).else(4), "CASE 1 WHEN 2 THEN 3 ELSE 4 END")
        WINQAssertEqual(Expression.case(1).when(2).then(3).when(4).then(5).else(6), "CASE 1 WHEN 2 THEN 3 WHEN 4 THEN 5 ELSE 6 END")
        WINQAssertEqual(Expression.case(1).when(2).then(3), "CASE 1 WHEN 2 THEN 3 END")
    }

    final class OperableObject: TableCodable {
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
        // Give
        let left = OperableObject.Properties.left
        let right = OperableObject.Properties.right

        // Then
        WINQExpressionAssertEqual(!left, "NOT left")
        WINQExpressionAssertEqual(+left, "+left")
        WINQExpressionAssertEqual(-left, "-left")
        WINQExpressionAssertEqual(~left, "~left")

        WINQExpressionAssertEqual(left.concat(right), "left || right")

        WINQExpressionAssertEqual(left.substr(start: 1, length: 2), "SUBSTR(left, 1, 2)")

        WINQExpressionAssertEqual(left.like(right), "left LIKE right")

        WINQExpressionAssertEqual(left.glob(right), "left GLOB right")

        WINQExpressionAssertEqual(left.match(right), "left MATCH right")

        WINQExpressionAssertEqual(left.regexp(right), "left REGEXP right")

        WINQExpressionAssertEqual(left.notLike(right), "left NOT LIKE right")

        WINQExpressionAssertEqual(left.notGlob(right), "left NOT GLOB right")

        WINQExpressionAssertEqual(left.notMatch(right), "left NOT MATCH right")

        WINQExpressionAssertEqual(left.notRegexp(right), "left NOT REGEXP right")

        WINQExpressionAssertEqual(left.like(right).excape(2), "left LIKE right ESCAPE 2")

        WINQExpressionAssertEqual(left.glob(right).excape(2), "left GLOB right ESCAPE 2")

        WINQExpressionAssertEqual(left.match(right).excape(2), "left MATCH right ESCAPE 2")

        WINQExpressionAssertEqual(left.regexp(right).excape(2), "left REGEXP right ESCAPE 2")

        WINQExpressionAssertEqual(left.notLike(right).excape(2), "left NOT LIKE right ESCAPE 2")

        WINQExpressionAssertEqual(left.notGlob(right).excape(2), "left NOT GLOB right ESCAPE 2")

        WINQExpressionAssertEqual(left.notMatch(right).excape(2), "left NOT MATCH right ESCAPE 2")

        WINQExpressionAssertEqual(left.notRegexp(right).excape(2), "left NOT REGEXP right ESCAPE 2")

        WINQExpressionAssertEqual(left.isNull(), "left ISNULL")

        WINQExpressionAssertEqual(left.isNotNull(), "left NOTNULL")

        WINQExpressionAssertEqual(left.`is`(right), "left IS right")

        WINQExpressionAssertEqual(left.isNot(right), "left IS NOT right")

        WINQExpressionAssertEqual(left.between(2, 10), "left BETWEEN 2 AND 10")

        WINQExpressionAssertEqual(left.notBetween(2, 10), "left NOT BETWEEN 2 AND 10")

        let statementSelect = StatementSelect().select(left).from("testExpressionTable")

        WINQExpressionAssertEqual(
            Expression.exists(statementSelect),
            "EXISTS(SELECT left FROM testExpressionTable)"
        )

        WINQExpressionAssertEqual(
            Expression.notExists(statementSelect),
            "NOT EXISTS(SELECT left FROM testExpressionTable)"
        )

        WINQExpressionAssertEqual(left.`in`(statementSelect), "left IN(SELECT left FROM testExpressionTable)")

        WINQExpressionAssertEqual(left.notIn(statementSelect), "left NOT IN(SELECT left FROM testExpressionTable)")

        WINQExpressionAssertEqual(left.`in`(right), "left IN(right)")

        WINQExpressionAssertEqual(left.notIn(right), "left NOT IN(right)")

        WINQExpressionAssertEqual(left.in(function: "testFunction").schema("testSchema").invoke().arguments(1, 2), "left IN testSchema.testFunction(1, 2)")

        WINQExpressionAssertEqual(left.notIn(function: "testFunction").invoke().arguments(1, 2), "left NOT IN testFunction(1, 2)")

        WINQExpressionAssertEqual(left.notIn(function: "testFunction"), "left NOT IN testFunction()")

        WINQExpressionAssertEqual(left.inTable("testTable").schema("testSchema"), "left IN testSchema.testTable")

        WINQExpressionAssertEqual(left.notInTable("testTable"), "left NOT IN testTable")

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

        WINQExpressionAssertEqual(left.collate("BINARY"), "left COLLATE BINARY")
    }

    func testOperableXOperable() {
        // Give
        let left = OperableObject.Properties.left
        let right = OperableObject.Properties.right

        // Then
        WINQExpressionAssertEqual((left || right), "left OR right")

        WINQExpressionAssertEqual((left && right), "left AND right")

        WINQExpressionAssertEqual((left * right), "left * right")

        WINQExpressionAssertEqual((left / right), "left / right")

        WINQExpressionAssertEqual((left % right), "left % right")

        WINQExpressionAssertEqual((left + right), "left + right")

        WINQExpressionAssertEqual((left - right), "left - right")

        WINQExpressionAssertEqual((left << right), "left << right")

        WINQExpressionAssertEqual((left >> right), "left >> right")

        WINQExpressionAssertEqual((left & right), "left & right")

        WINQExpressionAssertEqual((left | right), "left | right")

        WINQExpressionAssertEqual((left < right), "left < right")

        WINQExpressionAssertEqual((left <= right), "left <= right")

        WINQExpressionAssertEqual((left > right), "left > right")

        WINQExpressionAssertEqual((left >= right), "left >= right")

        WINQExpressionAssertEqual((left == right), "left == right")

        WINQExpressionAssertEqual((left != right), "left != right")
    }

    func testOperableXCanBeOperated() {
        // Give
        let left = OperableObject.Properties.left
        let right = 1

        // Then
        WINQExpressionAssertEqual((left || right), "left OR 1")

        WINQExpressionAssertEqual((left && right), "left AND 1")

        WINQExpressionAssertEqual((left * right), "left * 1")

        WINQExpressionAssertEqual((left / right), "left / 1")

        WINQExpressionAssertEqual((left % right), "left % 1")

        WINQExpressionAssertEqual((left + right), "left + 1")

        WINQExpressionAssertEqual((left - right), "left - 1")

        WINQExpressionAssertEqual((left << right), "left << 1")

        WINQExpressionAssertEqual((left >> right), "left >> 1")

        WINQExpressionAssertEqual((left & right), "left & 1")

        WINQExpressionAssertEqual((left | right), "left | 1")

        WINQExpressionAssertEqual((left < right), "left < 1")

        WINQExpressionAssertEqual((left <= right), "left <= 1")

        WINQExpressionAssertEqual((left > right), "left > 1")

        WINQExpressionAssertEqual((left >= right), "left >= 1")

        WINQExpressionAssertEqual((left == right), "left == 1")

        WINQExpressionAssertEqual((left != right), "left != 1")
    }
}
