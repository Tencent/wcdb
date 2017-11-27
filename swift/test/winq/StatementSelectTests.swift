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

class StatementSelectTests: WCDBTestCase {

    func testStatementSelect() {
        //Give
        let column1 = Column(named: "column1")
        let column2 = Column(named: "column2")
        
        let table1 = "table1"
        let table2 = "table2"

        WINQAssertEqual(StatementSelect().select(column1).from(table1), "SELECT column1 FROM table1")

        WINQAssertEqual(StatementSelect().select(column1, column2).from(table1), "SELECT column1, column2 FROM table1")
        
        WINQAssertEqual(StatementSelect().select(distinct: true, column1, column2).from(table1), "SELECT DISTINCT column1, column2 FROM table1")
        
        WINQAssertEqual(StatementSelect().select(column1).from(table1, table2), "SELECT column1 FROM table1, table2")
        
        WINQAssertEqual(StatementSelect().select(column1).from(table1).`where`(column1 > 1), "SELECT column1 FROM table1 WHERE (column1 > 1)")

        WINQAssertEqual(StatementSelect().select(column1).from(table1).order(by: column1.asOrder(), column2.asOrder()), "SELECT column1 FROM table1 ORDER BY column1, column2")

        WINQAssertEqual(StatementSelect().select(column1).from(table1).limit(1), "SELECT column1 FROM table1 LIMIT 1")

        WINQAssertEqual(StatementSelect().select(column1).from(table1).limit(from: 1, to: 2), "SELECT column1 FROM table1 LIMIT 1, 2")

        WINQAssertEqual(StatementSelect().select(column1).from(table1).offset(3), "SELECT column1 FROM table1 OFFSET 3")
    }
}
