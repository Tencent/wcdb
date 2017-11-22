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

class StatementInsertTests: XCTestCase {

    override func setUp() {
        super.setUp()
    }
    
    override func tearDown() {
        super.tearDown()
    }

    func testStatementInsert() {
        //Give
        let column1 = Column(named: "column1")
        let column2 = Column(named: "column2")
        
        let table1 = "table1"
        
        //Then
        WINQAssertEqual(StatementInsert().insert(intoTable: table1).values(1, "value"), "INSERT INTO table1 VALUES(1, 'value')")
        
        WINQAssertEqual(StatementInsert().insert(intoTable: table1, with: column1, column2).values(1, "value"), "INSERT INTO table1(column1, column2) VALUES(1, 'value')")
        
        WINQAssertEqual(StatementInsert().insert(intoTable: table1, onConflict: .Replace).values(1, "value"), "INSERT OR REPLACE INTO table1 VALUES(1, 'value')")
    }
}
