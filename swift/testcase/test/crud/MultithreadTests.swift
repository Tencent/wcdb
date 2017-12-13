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

func tick(_ block: () -> Void) -> TimeInterval {
    let before: TimeInterval = ProcessInfo.processInfo.systemUptime
    block()
    let after: TimeInterval = ProcessInfo.processInfo.systemUptime
    return after - before
}

class MultithreadTests: CRUDTestCase {

    var group: DispatchGroup!
    var queue: DispatchQueue!

    let multithreadPreinsertedCount = 10000

    override func setUp() {
        super.setUp()
        group = DispatchGroup()
        queue = DispatchQueue(label: String(recommendTag), attributes: .concurrent)

        let object = CRUDObject()
        object.isAutoIncrement = true
        let objects = Array(repeating: object, count: multithreadPreinsertedCount)
        XCTAssertNoThrow(try database.insert(objects: objects, intoTable: CRUDObject.name))
    }

    func testMultithreadReadRead() {
        //Get cache
        let expectedCount = multithreadPreinsertedCount + preInsertedObjects.count
        let results: [CRUDObject]? = try? database.getObjects(fromTable: CRUDObject.name)
        XCTAssertNotNil(results)
        XCTAssertEqual(results!.count, expectedCount)

        //Get single read time
        let singleReadCost = tick {
            let results: [CRUDObject]? = try? database.getObjects(fromTable: CRUDObject.name)
            XCTAssertNotNil(results)
            XCTAssertEqual(results!.count, expectedCount)
        }

        let doubleReadCost = tick {
            queue.async(group: group, execute: {
                let results: [CRUDObject]? = try? self.database.getObjects(fromTable: CRUDObject.name)
                XCTAssertNotNil(results)
                XCTAssertEqual(results!.count, expectedCount)
            })
            queue.async(group: group, execute: {
                let results: [CRUDObject]? = try? self.database.getObjects(fromTable: CRUDObject.name)
                XCTAssertNotNil(results)
                XCTAssertEqual(results!.count, expectedCount)
            })
            group.wait()
        }
        XCTAssertLessThanOrEqual(doubleReadCost, singleReadCost * 2)
    }

}
