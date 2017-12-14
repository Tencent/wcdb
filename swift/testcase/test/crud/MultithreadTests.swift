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
    let tableName = CRUDObject.name

    override func setUp() {
        super.setUp()
        group = DispatchGroup()
        queue = DispatchQueue(label: String(recommendTag), attributes: .concurrent)

        XCTAssertNoThrow(try database.delete(fromTable: tableName))
        database.close()
    }

    func preInsert(withCount count: Int) {
        let object = CRUDObject()
        object.isAutoIncrement = true
        let objects = Array(repeating: object, count: count)
        XCTAssertNoThrow(try database.insert(objects: objects, intoTable: tableName))
    }

    func testMultithreadReadRead() {
        //Give
        let expectedCount = 10000
        preInsert(withCount: expectedCount)
        database.close()

        let singleReadCost = tick {
            let results: [CRUDObject]? = try? database.getObjects(fromTable: tableName)
            XCTAssertNotNil(results)
            XCTAssertEqual(results!.count, expectedCount)
        }
        database.close()
        //When
        let readReadCost = tick {
            queue.async(group: group, execute: {
                let results: [CRUDObject]? = try? self.database.getObjects(fromTable: self.tableName)
                XCTAssertNotNil(results)
                XCTAssertEqual(results!.count, expectedCount)
            })
            queue.async(group: group, execute: {
                let results: [CRUDObject]? = try? self.database.getObjects(fromTable: self.tableName)
                XCTAssertNotNil(results)
                XCTAssertEqual(results!.count, expectedCount)
            })
            group.wait()
        }
        //Then
        XCTAssertLessThanOrEqual(readReadCost, singleReadCost * 2)
        print("readReadCost, singleReadCost \(readReadCost), \(singleReadCost)")
    }

    func testMultithreadReadWrite() {
        //Give
        let expectedCount = 10000

        let singleWriteCost = tick {
            preInsert(withCount: expectedCount)
        }
        database.close()

        //Get single read time
        let singleReadCost = tick {
            let results: [CRUDObject]? = try? database.getObjects(fromTable: tableName)
            XCTAssertNotNil(results)
            XCTAssertEqual(results!.count, expectedCount)
        }
        database.close()
        //When
        let readWriteCost = tick {
            queue.async(group: group, execute: {
                self.preInsert(withCount: expectedCount)
            })
            queue.async(group: group, execute: {
                let results: [CRUDObject]? = try? self.database.getObjects(fromTable: self.tableName)
                XCTAssertNotNil(results)
                XCTAssertEqual(results!.count, expectedCount)
            })
            group.wait()
        }

        //Then
        XCTAssertLessThanOrEqual(readWriteCost, singleReadCost + singleWriteCost)
        print("readWriteCost, singleReadCost, singleWriteCost, \(readWriteCost), \(singleReadCost), \(singleWriteCost)")
    }

    func testMultithreadWriteWrite() {
        //Give
        let expectedCount = 10000

        let singleWriteCost = tick {
            preInsert(withCount: expectedCount)
        }
        database.close()

        //Get single read time
        XCTAssertNoThrow(try database.delete(fromTable: tableName))
        database.close()
        //When
        let writeWriteCost = tick {
            queue.async(group: group, execute: {
                self.preInsert(withCount: expectedCount)
            })
            queue.async(group: group, execute: {
                self.preInsert(withCount: expectedCount)
            })
            group.wait()
        }
        //Then
        XCTAssertLessThanOrEqual(writeWriteCost, singleWriteCost * 2)
        print("writeWriteCost, singleWriteCost, \(writeWriteCost), \(singleWriteCost)")
    }

    final class ConditionLock {
        private var mutex = pthread_mutex_t()
        private var cond = pthread_cond_t()

        init() {
            pthread_mutex_init(&mutex, nil)
            pthread_cond_init(&cond, nil)
        }

        deinit {
            pthread_cond_destroy(&cond)
            pthread_mutex_destroy(&mutex)
        }

        func lock() {
            pthread_mutex_lock(&mutex)
        }

        func unlock() {
            pthread_mutex_unlock(&mutex)
        }

        func wait() {
            pthread_cond_wait(&cond, &mutex)
        }

        func wait(timeout: Int) {
            var ts = timespec()
            ts.tv_sec = timeout
            pthread_cond_timedwait(&cond, &mutex, &ts)
        }

        func broadcast() {
            pthread_cond_broadcast(&cond)
        }
    }

    func testConcurrencyExceeds() {
        let maxConcurrency = max(ProcessInfo.processInfo.processorCount, 64)
        var currentConcurrency = 0
        let cond = ConditionLock()
        for _ in 0..<maxConcurrency {
            queue.async(group: group, execute: {
                cond.lock(); defer { cond.unlock() }
                currentConcurrency += 1

                XCTAssertNotNil(try? self.database.begin(.deferred))

                cond.wait()

                currentConcurrency -= 1
            })
        }
        while true {
            cond.lock(); defer { cond.unlock() }
            guard currentConcurrency == maxConcurrency else {
                continue
            }
            break
        }
        XCTAssertEqual(currentConcurrency, maxConcurrency)
        XCTAssertThrowsError(try database.begin(.deferred))
        cond.broadcast()
        group.wait()
        XCTAssertEqual(currentConcurrency, 0)
    }
}
