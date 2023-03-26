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

class MultithreadReadWriteBenchmark: BaseMultithreadBenchmark {

    override func setUp() {
        super.setUp()

        setUpWithPreCreateObject(count: config.batchWriteCount)
    }

    func testMultithreadReadWrite() {
        let tableName = getTableName()
        var results: [BenchmarkObject]?
        measure(onSetUp: {
            results = nil

            tearDownDatabase()

            setUpWithPreCreateTable()

            setUpWithPreInsertObjects(count: config.readCount)

            tearDownDatabaseCache()

            setUpDatabaseCache()
        }, for: {
            queue.async(group: group, execute: {
                results = try? self.database.getObjects(fromTable: tableName)
            })
            queue.async(group: group, execute: {
                do {
                    try self.database.insert(self.objects, intoTable: tableName)
                } catch let error as WCDBError {
                    XCTFail(error.description)
                } catch let error {
                    XCTFail(error.localizedDescription)
                }
            })
            group.wait()
        }, checkCorrectness: {
            XCTAssertEqual(results?.count, config.readCount)
            let count = try? database.getValue(on: Column.all.count(), fromTable: tableName)
            XCTAssertEqual(Int(count?.int32Value ?? 0), config.readCount + config.batchWriteCount)
        })
    }
}
