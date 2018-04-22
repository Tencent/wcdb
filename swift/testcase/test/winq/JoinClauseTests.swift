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

class JoinClauseTests: BaseTestCase {

    func generateJoinClause() -> JoinClause {
        return JoinClause(with: "testJoinClauseTable")
    }

    func testJoinClause() {
        WINQAssertEqual(
            generateJoinClause().asTableOrSubquery(),
            "(testJoinClauseTable)"
        )

        WINQAssertEqual(
            generateJoinClause().join("testJoinClauseTable2", with: .left),
            "testJoinClauseTable LEFT JOIN testJoinClauseTable2"
        )

        WINQAssertEqual(
            generateJoinClause().naturalJoin("testJoinClauseTable2", with: .leftOuter),
            "testJoinClauseTable NATURAL LEFT OUTER JOIN testJoinClauseTable2"
        )

        WINQAssertEqual(generateJoinClause().join("testJoinClauseTable2", with: .inner),
                        "testJoinClauseTable INNER JOIN testJoinClauseTable2"
        )

        WINQAssertEqual(
            generateJoinClause().naturalJoin("testJoinClauseTable2", with: .cross),
            "testJoinClauseTable NATURAL CROSS JOIN testJoinClauseTable2"
        )

        WINQAssertEqual(generateJoinClause().on(1), "testJoinClauseTable ON 1")

        WINQAssertEqual(
            generateJoinClause().using(Column(named: "testColumn1"), Column(named: "testColumn2")),
            "testJoinClauseTable USING testColumn1, testColumn2"
        )
    }
}
