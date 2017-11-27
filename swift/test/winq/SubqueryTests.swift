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

class SubqueryTests: WCDBTestCase {

    func testSubquery() {
        //Give
        let table1 = "table1"
        let joinClause = JoinClause(withTable: table1)
        let statementSelect = StatementSelect().select(1).from(table1)
        let alias1 = "alias1"
        
        //Then
        WINQAssertEqual(Subquery(with: joinClause), "(table1)")
        
        WINQAssertEqual(Subquery(with: statementSelect), "(SELECT 1 FROM table1)")
        
        WINQAssertEqual(Subquery(withTable: table1), "table1")
        
        WINQAssertEqual(Subquery(withTable: table1).as(alias: alias1), "table1 AS alias1")
    }
}
