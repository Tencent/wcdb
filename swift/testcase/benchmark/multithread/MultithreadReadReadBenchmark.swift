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

class MultithreadReadReadBenchmark: BaseMultithreadBenchmark {
    
    override func setUp() {
        super.setUp()

        setUpWithPreCreateTable()
        
        setUpWithPreInsertObjects(count: config.readCount)
    }

    func testMultithreadReadRead() {
        let tableName = getTableName()
        var results1: [BenchmarkObject]? = nil
        var results2: [BenchmarkObject]? = nil

        measure(onSetUp: { 
            results1 = nil
            
            results2 = nil
            
            tearDownDatabaseCache()
            
            setUpDatabaseCache()
        }, for: { 
            queue.async(group: group, execute: { 
                results1 = try? self.database.getObjects(fromTable: tableName)
            })
            queue.async(group: group, execute: { 
                results2 = try? self.database.getObjects(fromTable: tableName)
            })
            group.wait()
        }, checkCorrectness: {
            XCTAssertEqual(results1?.count, config.readCount)
            XCTAssertEqual(results2?.count, config.readCount)
        })
    }
}
