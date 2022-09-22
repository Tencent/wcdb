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

class StatementDeleteTests: BaseTestCase {

    func generateStatementDelete() -> StatementDelete {
        let testTable = "testTable"
        return StatementDelete().delete(from: testTable)
    }

    func testStatementDelete() {
        // Give
        let column1 = Column(named: "column1")
        let column2 = Column(named: "column2")

        WINQAssertEqual(generateStatementDelete().`where`(column1 > 1), "DELETE FROM main.testTable WHERE column1 > 1")

        WINQAssertEqual(StatementDelete().delete(from: "testTable".asQualifiedTable().of(schema: "testSchema")).`where`(column1 > 1), "DELETE FROM testSchema.testTable WHERE column1 > 1")

        WINQAssertEqual(
            generateStatementDelete().order(by: column1.asOrder(), column2.asOrder()),
            "DELETE FROM main.testTable ORDER BY column1, column2")

        WINQAssertEqual(generateStatementDelete().limit(1), "DELETE FROM main.testTable LIMIT 1")

        WINQAssertEqual(generateStatementDelete().limit(from: 1, to: 2), "DELETE FROM main.testTable LIMIT 1, 2")

        WINQAssertEqual(generateStatementDelete().limit(1).offset(3), "DELETE FROM main.testTable LIMIT 1 OFFSET 3")

        WINQAssertEqual(
            generateStatementDelete()
                .`where`(column1 > 1)
                .order(by: column1.asOrder(), column2.asOrder())
                .limit(1).offset(2),
            "DELETE FROM main.testTable WHERE column1 > 1 ORDER BY column1, column2 LIMIT 1 OFFSET 2"
        )

        WINQAssertEqual(
            generateStatementDelete()
                .with(CommonTableExpression("testTable").as(StatementSelect().select(1)))
                .delete(from: "testTable")
                .`where`(column1 > 1),
            "WITH testTable AS(SELECT 1) DELETE FROM main.testTable WHERE column1 > 1"
        )
    }
}
