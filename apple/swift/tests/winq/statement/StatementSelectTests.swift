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

class StatementSelectTests: BaseTestCase {

    func testStatementSelect() {
        // Give
        let column1 = Column(named: "column1")
        let column2 = Column(named: "column2")

        let table1 = "table1"
        let table2 = "table2"

        WINQAssertEqual(StatementSelect().select(column1).from(table1), "SELECT column1 FROM main.table1")

        WINQAssertEqual(
            StatementSelect().select(column1, column2).from(table1),
            "SELECT column1, column2 FROM main.table1"
        )

        WINQAssertEqual(
            StatementSelect().select(distinct: true, column1, column2).from(table1),
            "SELECT DISTINCT column1, column2 FROM main.table1"
        )

        WINQAssertEqual(
            StatementSelect().select(column1).from(table1, table2),
            "SELECT column1 FROM main.table1, main.table2"
        )

        WINQAssertEqual(
            StatementSelect().select(column1).from(table1).`where`(column1 > 1),
            "SELECT column1 FROM main.table1 WHERE column1 > 1"
        )

        WINQAssertEqual(
            StatementSelect().select(column1).from(table1).order(by: column1.asOrder(), column2.asOrder()),
            "SELECT column1 FROM main.table1 ORDER BY column1, column2"
        )

        WINQAssertEqual(
            StatementSelect().select(column1).from(table1).limit(1),
            "SELECT column1 FROM main.table1 LIMIT 1"
        )

        WINQAssertEqual(
            StatementSelect().select(column1).from(table1).limit(from: 1, to: 2),
            "SELECT column1 FROM main.table1 LIMIT 1, 2"
        )

        WINQAssertEqual(
            StatementSelect().select(column1).from(table1).limit(1).offset(3),
            "SELECT column1 FROM main.table1 LIMIT 1 OFFSET 3"
        )

        WINQAssertEqual(
            StatementSelect().select(column1).from(table1).group(by: column1),
            "SELECT column1 FROM main.table1 GROUP BY column1"
        )

        WINQAssertEqual(
            StatementSelect().select(column1).from(table1).group(by: column1, column2).having(column1>1),
            "SELECT column1 FROM main.table1 GROUP BY column1, column2 HAVING column1 > 1"
        )

        WINQAssertEqual(
            StatementSelect().select(column1).from(table1).asTableOrSubquery(),
            "(SELECT column1 FROM main.table1)"
        )

        WINQAssertEqual(
            StatementSelect().with(CommonTableExpression(table1).column(column1).as(StatementSelect().select(1)), CommonTableExpression(table2).column(column2).as(StatementSelect().select(2))).select(1,2),
            "WITH table1(column1) AS(SELECT 1), table2(column2) AS(SELECT 2) SELECT 1, 2"
        )

        WINQAssertEqual(
            StatementSelect().with(recursive: true, CommonTableExpression(table1).column(column1).as(StatementSelect().select(1)), CommonTableExpression(table2).column(column2).as(StatementSelect().select(2))).select(1,2),
            "WITH RECURSIVE table1(column1) AS(SELECT 1), table2(column2) AS(SELECT 2) SELECT 1, 2"
        )

        WINQAssertEqual(
            StatementSelect().select(1, 2).window("window1").as(WindowDef().order(1)).window("window2").as(WindowDef().order(2)),
            "SELECT 1, 2 WINDOW window1 AS (ORDER BY 1), window2 AS (ORDER BY 2)"
        )

        WINQAssertEqual(
            StatementSelect().value(1, 2),
            "VALUES(1, 2)"
        )
        WINQAssertEqual(
            StatementSelect().value(1, 2).value(1, 2),
            "VALUES(1, 2), (1, 2)"
        )

        WINQAssertEqual(
            StatementSelect().select(1, 2).group(by: 1, 2).union().select(1, 2).where(1),
            "SELECT 1, 2 GROUP BY 1, 2 UNION SELECT 1, 2 WHERE 1"
        )

        WINQAssertEqual(
            StatementSelect().select(1, 2).unionAll().value(1, 2),
            "SELECT 1, 2 UNION ALL VALUES(1, 2)"
        )

        WINQAssertEqual(
            StatementSelect().select(1, 2).intersect().value(1, 2),
            "SELECT 1, 2 INTERSECT VALUES(1, 2)"
        )

        WINQAssertEqual(
            StatementSelect().select(1, 2).except().value(1, 2),
            "SELECT 1, 2 EXCEPT VALUES(1, 2)"
        )
    }
}
