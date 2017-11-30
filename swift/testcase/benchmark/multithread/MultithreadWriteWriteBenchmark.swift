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

class MultithreadWriteWriteBenchmark: BaseMultithreadBenchmark {
    
    override func setUp() {
        super.setUp()
        
        setUpWithPreCreateTable()
        
        setUpWithPreCreateObject(count: config.writeCount)
        
        clearCache()
        
        setUpDatabaseCache()
    }
    
    func testMultithreadWriteWrite() {
        let tableName = getTableName()
        self.measure {
            queue.async(group: group, execute: { 
                do {
                    try self.database.insert(objects: self.objects, intoTable: tableName)
                }catch let error as WCDB.Error {
                    XCTFail(error.description)
                }catch let error {
                    XCTFail(error.localizedDescription)
                }
            })
            queue.async(group: group, execute: { 
                do {
                    try self.database.insert(objects: self.objects, intoTable: tableName)
                }catch let error as WCDB.Error {
                    XCTFail(error.description)
                }catch let error {
                    XCTFail(error.localizedDescription)
                }
            })
            group.wait()
        }
        let count = try? database.getValue(on: Column.any, fromTable: tableName)
        XCTAssertEqual(Int(count?.int32Value ?? 0), config.writeCount * 2)
    }
}
