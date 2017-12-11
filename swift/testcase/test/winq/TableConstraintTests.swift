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

class TableConstraintTests: BaseTestCase {

    func generateTableConstraint() -> TableConstraint {
        let table1 = "table1"
        return TableConstraint(named: table1)
    }

    func testTableConstraint() {
        //Give
        let column1 = Column(named: "column1")
        let index1 = column1.asIndex()
        let index2 = Column(named: "column2").asIndex()

        WINQAssertEqual(
            generateTableConstraint().makePrimary(indexesBy: index1, index2),
            "CONSTRAINT table1 PRIMARY KEY(column1, column2)"
        )

        WINQAssertEqual(
            generateTableConstraint().makeUnique(indexesBy: index1, index2),
            "CONSTRAINT table1 UNIQUE(column1, column2)"
        )

        WINQAssertEqual(
            generateTableConstraint().onConflict(.fail),
            "CONSTRAINT table1 ON CONFLICT FAIL"
        )

        WINQAssertEqual(
            generateTableConstraint().check(column1 > 1),
            "CONSTRAINT table1 CHECK(column1 > 1)"
        )
    }
}
