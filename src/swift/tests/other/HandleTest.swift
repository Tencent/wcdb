//
// Created by 陈秋文 on 2023/8/13.
//

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

class HandleTests: DatabaseTestCase, @unchecked Sendable {

    func testCancellationSignal() {
        XCTAssertNoThrow(try database.create(table: TestObject.name, of: TestObject.self))
        var objects: [TestObject] = []
        for i in 0...10000 {
            let object = TestObject()
            object.variable1 = i
            object.variable2 = "\(i)"
            objects.append(object)
        }

        XCTAssertNoThrow(try database.insert(objects, intoTable: TestObject.name))

        var hasTestInterrupt = false
        database.traceError { error in
            guard error.level == .Error else {
                return
            }
            XCTAssertEqual(error.code, .Interrupt)
            hasTestInterrupt = true
        }

        let signal = Handle.CancellatiionSignal()
        let group = DispatchGroup()
        DispatchQueue(label: "test").async(group: group) {
            let handle = try? self.database.getHandle()
            guard let handle = handle else {
                return
            }
            handle.attach(cancellationSignal: signal)
            let objects: [TestObject]? = try? handle.getObjects(on: TestObject.Properties.all, fromTable: TestObject.name)
            XCTAssertNil(objects)
            handle.detachCancellationSignal()
        }
        usleep(10000)
        signal.cancel()
        group.wait()
        XCTAssertTrue(hasTestInterrupt)
        self.database.traceError(nil)
    }

    func testWriteWithHandleCountLimit() {
        var maxHandleCount = 0
        Database.globalTraceDatabaseOperation {
            _, operation, info in
            guard operation == .OpenHandle else {
                return
            }
            maxHandleCount = max(maxHandleCount, info[Database.OperationInfoKeyHandleCount]?.intValue ?? 0)
        }
        XCTAssertNoThrow(try database.create(table: TestObject.name, of: TestObject.self))
        let table = database.getTable(named: TestObject.name, of: TestObject.self)
        let group = DispatchGroup()
        for i in 0..<80 {
            DispatchQueue(label: "test").async(group: group) {
                do {
                    switch i % 8 {
                    case 0:
                        let objects = Random.testObjects(startWith: i * 100, count: 100)
                        try table.insert(objects)
                    case 1:
                        try table.update(on: TestObject.Properties.variable2, with: ["abc"], where: TestObject.Properties.variable1 > (i - 1) * 100 && TestObject.Properties.variable1 <= i * 100)
                    case 2:
                        try table.delete(where: TestObject.Properties.variable1 > ((i - 2) * 100 + 50) && TestObject.Properties.variable1 <= i * 100)
                    case 3:
                        try self.database.run(transaction: { _ in
                            try table.update(on: TestObject.Properties.variable2, with: ["abc2"], where: TestObject.Properties.variable1 > (i - 1) * 100 && TestObject.Properties.variable1 <= i * 100)
                        })
                    case 4:
                        let objects = Random.testObjects(startWith: i * 100, count: 100)
                        try self.database.insert(objects, intoTable: TestObject.name)
                    case 5:
                        try self.database.update(table: TestObject.name, on: TestObject.Properties.variable2, with: ["abc"], where: TestObject.Properties.variable1 > (i - 1) * 100 && TestObject.Properties.variable1 <= i * 100)
                    case 6:
                        try self.database.delete(fromTable: TestObject.name, where: TestObject.Properties.variable1 > ((i - 2) * 100 + 50) && TestObject.Properties.variable1 <= i * 100)
                    case 7:
                        try self.database.run(transaction: { _ in
                            try self.database.update(table: TestObject.name, on: TestObject.Properties.variable2, with: ["abc2"], where: TestObject.Properties.variable1 > (i - 1) * 100 && TestObject.Properties.variable1 <= i * 100)
                        })
                    default:
                        break
                    }
                    usleep(100000)
                } catch let error as WCDBError {
                    XCTFail(error.description)
                } catch let error {
                    XCTFail(error.localizedDescription)
                }
            }
        }
        group.wait()
        XCTAssertTrue(maxHandleCount <= 4)
        Database.globalTraceDatabaseOperation(nil)
    }

    func testReadWithHandleCountLimit() {
        var maxHandleCount = 0
        Database.globalTraceDatabaseOperation {
            _, operation, info in
            guard operation == .OpenHandle else {
                return
            }
            maxHandleCount = max(maxHandleCount, info[Database.OperationInfoKeyHandleCount]?.intValue ?? 0)
        }
        XCTAssertNoThrow(try database.create(table: TestObject.name, of: TestObject.self))
        let table = database.getTable(named: TestObject.name, of: TestObject.self)
        let objects = Random.testObjects(startWith: 0, count: 64000)
        XCTAssertNoThrow(try table.insert(objects))
        let group = DispatchGroup()

        for i in 0..<640 {
            DispatchQueue(label: "test").async(group: group) {
                do {
                    switch i % 10 {
                    case 0:
                        let ret = try table.getObjects(on: TestObject.Properties.all, limit: 100, offset: i * 100)
                        XCTAssertEqual(ret.count, 100)
                    case 1:
                        let count = try self.database.getValue(from: StatementSelect().select(TestObject.Properties.any.count()).from(TestObject.name))
                        XCTAssertEqual(count?.intValue, 64000)
                    case 2:
                        let ret = try table.getColumn(on: TestObject.Properties.variable2, limit: 100, offset: i * 100)
                        XCTAssertEqual(ret.count, 100)
                    case 3:
                        let ret = try table.getRows(on: TestObject.Properties.all, limit: 100, offset: i * 100)
                        XCTAssertEqual(ret.count, 100)
                    case 4:
                        let ret = try table.getRow(on: TestObject.Properties.all)
                        XCTAssertEqual(ret.count, 2)
                    case 5:
                        let ret: [TestObject] = try self.database.getObjects(on: TestObject.Properties.all, fromTable: TestObject.name, limit: 100, offset: i * 100)
                        XCTAssertEqual(ret.count, 100)
                    case 6:
                        let ret = try self.database.getColumn(on: TestObject.Properties.variable2, fromTable: TestObject.name, limit: 100, offset: i * 100)
                        XCTAssertEqual(ret.count, 100)
                    case 7:
                        let ret = try self.database.getRows(on: TestObject.Properties.all, fromTable: TestObject.name, limit: 100, offset: i * 100)
                        XCTAssertEqual(ret.count, 100)
                    case 8:
                        let ret = try self.database.getRow(on: TestObject.Properties.all, fromTable: TestObject.name)
                        XCTAssertEqual(ret.count, 2)
                    default:
                        break
                    }
                    usleep(100000)
                } catch let error as WCDBError {
                    XCTFail(error.description)
                } catch let error {
                    XCTFail(error.localizedDescription)
                }
            }
        }

        group.wait()
        XCTAssertTrue(maxHandleCount > 4 && maxHandleCount <= 32)
        Database.globalTraceDatabaseOperation(nil)
    }

    func testReadWriteWithHandleCountLimit() {
        var maxHandleCount = 0
        Database.globalTraceDatabaseOperation({
            _, operation, info in
            guard operation == .OpenHandle else {
                return
            }
            maxHandleCount = max(maxHandleCount, info[Database.OperationInfoKeyHandleCount]?.intValue ?? 0)
        })
        XCTAssertNoThrow(try database.create(table: TestObject.name, of: TestObject.self))
        let table = database.getTable(named: TestObject.name, of: TestObject.self)
        let objects = Random.testObjects(startWith: 0, count: 32000)
        XCTAssertNoThrow(try table.insert(objects))
        let group = DispatchGroup()

        for i in 0..<320 {
            DispatchQueue(label: "test").async(group: group) {
                do {
                    if i % 8 == 0 {
                        let objects = Random.testObjects(startWith: i * 100 + 32000, count: 100)
                        try table.insert(objects)
                    } else {
                        let ret = try table.getObjects(on: TestObject.Properties.all, limit: 100, offset: i * 100)
                        XCTAssertEqual(ret.count, 100)
                    }
                    usleep(100000)
                } catch let error as WCDBError {
                    XCTFail(error.description)
                } catch let error {
                    XCTFail(error.localizedDescription)
                }
            }
        }

        group.wait()
        XCTAssertTrue(maxHandleCount > 4 && maxHandleCount <= 32)
        Database.globalTraceDatabaseOperation(nil)
    }
}
