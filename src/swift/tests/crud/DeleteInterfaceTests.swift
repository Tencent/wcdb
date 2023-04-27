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

class DeleteInterfaceTests: CRUDTestCase {

    func testDelete() {
        // When
        XCTAssertNoThrow(try database.delete(fromTable: TestObject.name))
        // Then
        let results: [TestObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: TestObject.name))
        XCTAssertEqual(results.count, 0)
    }

    func testConditionalDelete() {
        // When
        XCTAssertNoThrow(try database.delete(fromTable: TestObject.name, where: TestObject.Properties.variable1 == 2))
        // Then
        let results: [TestObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: TestObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testOrderedDelete() {
        // Give
        let order = [TestObject.Properties.variable1.order(.descending)]
        // When
        XCTAssertNoThrow(try database.delete(fromTable: TestObject.name, orderBy: order, limit: 1))
        // Then
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: TestObject.name, orderBy: order)
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testLimitedDelete() {
        // When
        XCTAssertNoThrow(try database.delete(fromTable: TestObject.name, limit: 1))
        // Then
        let results: [TestObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: TestObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[1])
    }

    func testOffsetDelete() {
        // When
        XCTAssertNoThrow(try database.delete(fromTable: TestObject.name, limit: 1, offset: 1))
        // Then
        let results: [TestObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: TestObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testTableDelete() {
        // When
        XCTAssertNoThrow(try table.delete())
        // Then
        let results: [TestObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: TestObject.name))
        XCTAssertEqual(results.count, 0)
    }

    func testTableConditionalDelete() {
        // When
        XCTAssertNoThrow(try table.delete(where: TestObject.Properties.variable1 == 2))
        // Then
        let results: [TestObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: TestObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testTableOrderedDelete() {
        // Give
        let order = [TestObject.Properties.variable1.order(.descending)]
        // When
        XCTAssertNoThrow(try table.delete(orderBy: order, limit: 1))
        // Then
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: TestObject.name, orderBy: order)
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testTableLimitedDelete() {
        // When
        XCTAssertNoThrow(try table.delete(limit: 1))
        // Then
        let results: [TestObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: TestObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[1])
    }

    func testTableOffsetDelete() {
        // When
        XCTAssertNoThrow(try table.delete(limit: 1, offset: 1))
        // Then
        let results: [TestObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: TestObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

}
