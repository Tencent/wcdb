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

class InitializationBenchmark: BaseBenchmark {
    
    override func setUp() {
        super.setUp()
        
        setUpWithPreCreateTable(count: config.tableCount)

        clearCache()        
    }

    func testBaselineRead() {
        self.measure {
            XCTAssertTrue(database.canOpen)
        }
        let results = try? database.getValue(on: Column.any, fromTable: Master.tableName)
        XCTAssertEqual(Int(results?.int32Value ?? 0), config.tableCount)
    }
}
