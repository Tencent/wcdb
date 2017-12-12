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

class StatementCreateTableTests: BaseTestCase {

    func testCreateTable() {
        //Give
        let column1 = Column(named: "column1")
        let column2 = Column(named: "column2")

        let def1 = column1.asDef(with: .integer32)
        let def2 = column2.asDef(with: .text)

        let constraint1 = TableConstraint(named: "constraint1").makePrimary(indexesBy: column1.asIndex())
        let constraint2 = TableConstraint(named: "constraint2").makeUnique(indexesBy: column2.asIndex())

        let table1 = "table1"
        let table2 = "table2"

        let statementSelect = StatementSelect().select(column1).from(table1)

        //Then
        XCTAssertEqual(StatementCreateTable().statementType, StatementType.createTable)

        WINQAssertEqual(
            StatementCreateTable().create(table: table1, with: def1, def2),
            "CREATE TABLE IF NOT EXISTS table1(column1 INTEGER, column2 TEXT)"
        )

        WINQAssertEqual(
            StatementCreateTable().create(table: table1, ifNotExists: false, with: def1, def2),
            "CREATE TABLE table1(column1 INTEGER, column2 TEXT)"
        )

        WINQAssertEqual(
            StatementCreateTable().create(table: table1, with: def1, def2, and: [constraint1, constraint2]),
            """
            CREATE TABLE IF NOT EXISTS table1\
            (column1 INTEGER, column2 TEXT, \
            CONSTRAINT constraint1 PRIMARY KEY(column1), \
            CONSTRAINT constraint2 UNIQUE(column2))
            """
        )

        WINQAssertEqual(
            StatementCreateTable().create(table: table2, as: statementSelect),
            "CREATE TABLE IF NOT EXISTS table2 AS SELECT column1 FROM table1"
        )

        WINQAssertEqual(
            StatementCreateTable().create(table: table2, ifNotExists: false, as: statementSelect),
            "CREATE TABLE table2 AS SELECT column1 FROM table1"
        )
    }
}
