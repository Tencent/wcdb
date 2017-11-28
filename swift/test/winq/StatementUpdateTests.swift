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

class StatementUpdateTests: WCDBTestCase {

    func testStatementUpdate() {
        //Give
        let table1 = "table1"
        let column1 = Column(named: "column1")
        let column2 = Column(named: "column2")
        
        //Then
        WINQAssertEqual(StatementUpdate().update(table: table1).set((column1, 1)), "UPDATE table1 SET column1=1")
        
        WINQAssertEqual(StatementUpdate().update(table: table1, onConflict: .Replace).set((column1, 1), (column2, "test")), "UPDATE OR REPLACE table1 SET column1=1, column2='test'")

        WINQAssertEqual(StatementUpdate().update(table: table1).set((column1, 1), (column2, "test")), "UPDATE table1 SET column1=1, column2='test'")

        WINQAssertEqual(StatementUpdate().update(table: table1).set((column1, 1)).`where`(column1 > 1), "UPDATE table1 SET column1=1 WHERE (column1 > 1)")

        WINQAssertEqual(StatementUpdate().update(table: table1).set((column1, 1)).order(by: column1.asOrder(), column2.asOrder()), "UPDATE table1 SET column1=1 ORDER BY column1, column2")

        WINQAssertEqual(StatementUpdate().update(table: table1).set((column1, 1)).limit(1), "UPDATE table1 SET column1=1 LIMIT 1")

        WINQAssertEqual(StatementUpdate().update(table: table1).set((column1, 1)).limit(from: 1, to: 2), "UPDATE table1 SET column1=1 LIMIT 1, 2")

        WINQAssertEqual(StatementUpdate().update(table: table1).set((column1, 1)).limit(1, offset: 3), "UPDATE table1 SET column1=1 LIMIT 1 OFFSET 3")
    }
}
