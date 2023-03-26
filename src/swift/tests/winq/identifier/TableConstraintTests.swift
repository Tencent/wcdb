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

class TableConstraintTests: BaseTestCase {

    func testTableConstraint() {
        // Give
        let column1 = Column(named: "column1")
        let column2 = Column(named: "column2")
        let index1 = column1.asIndex()
        let index2 = Column(named: "column2").asIndex()
        let foreignKey1 = ForeignKey().references(with: "testTable").columns(column1)
        let foreignKey2 = ForeignKey().references(with: "testTable").columns(column1, column2)

        WINQAssertEqual(
            TableConstraint().primaryKey().indexed(by: index1, index2),
            "PRIMARY KEY(column1, column2)"
        )

        WINQAssertEqual(
            TableConstraint(named: "testConstraint").primaryKey().indexed(by: index1, index2),
            "CONSTRAINT testConstraint PRIMARY KEY(column1, column2)"
        )

        WINQAssertEqual(
            TableConstraint().unique().indexed(by: index1, index2),
            "UNIQUE(column1, column2)"
        )

        WINQAssertEqual(
            TableConstraint().unique().indexed(by: index1, index2).onConflict(.Fail),
            "UNIQUE(column1, column2) ON CONFLICT FAIL"
        )

        WINQAssertEqual(
            TableConstraint().check(column1 > 1),
            "CHECK(column1 > 1)"
        )

        WINQAssertEqual(
            TableConstraint().foreignKey(column1, foreignKey: foreignKey1),
            "FOREIGN KEY(column1) REFERENCES testTable(column1)"
        )

        WINQAssertEqual(
            TableConstraint().foreignKey(column1, column2, foreignKey: foreignKey2),
            "FOREIGN KEY(column1, column2) REFERENCES testTable(column1, column2)"
        )
    }
}
