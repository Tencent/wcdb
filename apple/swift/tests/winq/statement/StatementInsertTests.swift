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

class StatementInsertTests: BaseTestCase {

    func testStatementInsert() {
        // Give
        let column1 = Column(named: "column1")
        let column2 = Column(named: "column2")

        // Then

        WINQAssertEqual(
            StatementInsert().insert(intoTable: "testTable").values(1, "value"),
            "INSERT INTO testTable VALUES(1, 'value')"
        )

        WINQAssertEqual(
            StatementInsert().insert(intoTable: "testTable").of(schema: "testSchema").values(1, "value"),
            "INSERT INTO testSchema.testTable VALUES(1, 'value')"
        )

        WINQAssertEqual(
            StatementInsert().insert(intoTable: "testTable").of(schema: "testSchema").as("newTable").values(1, "value"),
            "INSERT INTO testSchema.testTable AS newTable VALUES(1, 'value')"
        )

        WINQAssertEqual(
            StatementInsert().insert(intoTable: "testTable").columns(column1, column2).values(1, "value"),
            "INSERT INTO testTable(column1, column2) VALUES(1, 'value')"
        )

        WINQAssertEqual(
            StatementInsert().insert(intoTable: "testTable").onConflict(.Replace).values(1, "value"),
            "INSERT OR REPLACE INTO testTable VALUES(1, 'value')"
        )

        WINQAssertEqual(
            StatementInsert().insert(intoTable: "testTable").defaultValues(),
            "INSERT INTO testTable DEFAULT VALUES"
        )

        WINQAssertEqual(
            StatementInsert().insert(intoTable: "testTable").columns(column1, column2).values(1, "value").upsert(Upsert().doNothing()),
            "INSERT INTO testTable(column1, column2) VALUES(1, 'value') ON CONFLICT DO NOTHING"
        )
    }
}
