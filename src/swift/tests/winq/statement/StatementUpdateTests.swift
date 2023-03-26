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
#if TEST_WCDB_SWIFT
import WCDBSwift
#else
import WCDB
#endif

class StatementUpdateTests: BaseTestCase {

    func testStatementUpdate() {
        // Give
        let column1 = Column(named: "column1")
        let column2 = Column(named: "column2")

        WINQAssertEqual(
            StatementUpdate().update(table: "testTable").set(column1).to(1),
            "UPDATE testTable SET column1 = 1"
        )

        WINQAssertEqual(
            StatementUpdate().update(table: "testTable").onConflict( .Replace).set(column1).to(1).set(column2).to("test"),
            "UPDATE OR REPLACE testTable SET column1 = 1, column2 = 'test'"
        )

        WINQAssertEqual(
            StatementUpdate().update(table: "testTable").set(column1).to(1).set(column2).to("test"),
            "UPDATE testTable SET column1 = 1, column2 = 'test'"
        )

        WINQAssertEqual(
            StatementUpdate().update(table: "testTable").set(column1).to(1).`where`(column1 > 1),
            "UPDATE testTable SET column1 = 1 WHERE column1 > 1"
        )

        WINQAssertEqual(
            StatementUpdate()
                .update(table: "testTable")
                .set(column1).to(1)
                .order(by: column1.asOrder(), column2.asOrder()),
            "UPDATE testTable SET column1 = 1 ORDER BY column1, column2"
        )

        WINQAssertEqual(
            StatementUpdate().update(table: "testTable").set(column1).to(1).limit(1),
            "UPDATE testTable SET column1 = 1 LIMIT 1"
        )

        WINQAssertEqual(
            StatementUpdate().update(table: "testTable").set(column1).to(1).limit(from: 1, to: 2),
            "UPDATE testTable SET column1 = 1 LIMIT 1, 2"
        )

        WINQAssertEqual(
            StatementUpdate().update(table: "testTable").set(column1).to(1).limit(1).offset(3),
            "UPDATE testTable SET column1 = 1 LIMIT 1 OFFSET 3"
        )

        WINQAssertEqual(
            StatementUpdate().with(CommonTableExpression("testTable").column(column1).as(StatementSelect().select(1))).update(table: "testTable").set(column1).to(1),
            "WITH testTable(column1) AS(SELECT 1) UPDATE testTable SET column1 = 1"
        )

        WINQAssertEqual(
            StatementUpdate().with(recursive: true, CommonTableExpression("table1").column(column1).as(StatementSelect().select(1)), CommonTableExpression("table2").column(column2).as(StatementSelect().select(2))).update(table: "testTable").set(column1).to(1),
            "WITH RECURSIVE table1(column1) AS(SELECT 1), table2(column2) AS(SELECT 2) UPDATE testTable SET column1 = 1"
        )
    }
}
