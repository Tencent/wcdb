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

class JoinTests: BaseTestCase {

    func testJoin() {
        WINQAssertEqual(
            Join(with: "testTable").join("testTable1"),
            "testTable JOIN testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").with("testTable1"),
            "testTable, testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").leftJoin("testTable1"),
            "testTable LEFT JOIN testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").leftOuterJoin("testTable1"),
            "testTable LEFT OUTER JOIN testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").innerJoin("testTable1"),
            "testTable INNER JOIN testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").crossJoin("testTable1"),
            "testTable CROSS JOIN testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").naturalJoin("testTable1"),
            "testTable NATURAL JOIN testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").naturalLeftJoin("testTable1"),
            "testTable NATURAL LEFT JOIN testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").naturalLeftOuterJoin("testTable1"),
            "testTable NATURAL LEFT OUTER JOIN testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").naturalInnerJoin("testTable1"),
            "testTable NATURAL INNER JOIN testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").naturalCrossJoin("testTable1"),
            "testTable NATURAL CROSS JOIN testTable1"
        )

        WINQAssertEqual(
            Join(with: "testTable").join("testTable1").on(1),
            "testTable JOIN testTable1 ON 1"
        )

        WINQAssertEqual(
            Join(with: "testTable").join("testTable1").using(Column(named: "testColumn1"), Column(named: "testColumn2")),
            "testTable JOIN testTable1 USING(testColumn1, testColumn2)"
        )

        WINQAssertEqual(
            Join(with: "testTable").join("testTable1").on(1).leftOuterJoin("testTable2").on(2),
            "testTable JOIN testTable1 ON 1 LEFT OUTER JOIN testTable2 ON 2"
        )
    }
}
