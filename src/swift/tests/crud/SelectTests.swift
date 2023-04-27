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

class SelectTests: CRUDTestCase {

    var select: Select!

    override func setUp() {
        super.setUp()

        select = WCDBAssertNoThrowReturned(try database.prepareSelect(of: TestObject.self, fromTable: TestObject.name))
    }

    func testSelect() {
        let results: [TestObject] = WCDBAssertNoThrowReturned(try select.allObjects(), whenFailed: [TestObject]())
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }

    func testTableSelect() {
        select = WCDBAssertNoThrowReturned(try table.prepareSelect(of: TestObject.self))
        let results: [TestObject] = WCDBAssertNoThrowReturned(try select.allObjects(), whenFailed: [TestObject]())
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }

    func testConditionalSelect() {
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try select.where(TestObject.Properties.variable1 == 2).allObjects(),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, "object2")
    }

    func testOrderedSelect() {
        let order = TestObject.Properties.variable2.order(.descending)
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try select.order(by: order).allObjects(),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results, preInsertedObjects.reversed())
    }

    func testLimitedSelect() {
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try select.limit(1).allObjects(),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testOffsetSelect() {
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try select.limit(from: 1, to: 1).allObjects(),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[1])
    }

    func testPartialSelect() {
        let select = WCDBAssertNoThrowReturned(try database.prepareSelect(on: TestObject.Properties.variable2.asProperty(), fromTable: TestObject.name))!
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try select.allObjects(),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results.map({ (object) -> String in
            XCTAssertNil(object.variable1)
            XCTAssertNotNil(object.variable2)
            return object.variable2!
        }), preInsertedObjects.map({
            XCTAssertNotNil($0.variable2)
            return $0.variable2!
        }))
    }

    func testTablePartialSelect() {
        let select = WCDBAssertNoThrowReturned(try table.prepareSelect(on: TestObject.Properties.variable2.asProperty()))!
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try select.allObjects(),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results.map({ (object) -> String in
            XCTAssertNil(object.variable1)
            XCTAssertNotNil(object.variable2)
            return object.variable2!
        }), preInsertedObjects.map({
            XCTAssertNotNil($0.variable2)
            return $0.variable2!
        }))
    }

    func testSelectIteration() {
        var results: [TestObject] = []
        while let result: TestObject = WCDBAssertNoThrowReturned(try select.nextObject()) {
            results.append(result)
        }
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }

    func testUnspecificSelect() {
        let results = WCDBAssertNoThrowReturned(try select.allObjects(), whenFailed: [Any]())
        let objects = results as? [TestObject]
        XCTAssertNotNil(objects)
        XCTAssertEqual(objects!.sorted(), preInsertedObjects.sorted())
    }

    func testUnspecificSelectIteration() {
        var results: [TestObject] = []
        while let result = WCDBAssertNoThrowReturned(try select.nextObject()) {
            let crudObject = result as? TestObject
            XCTAssertNotNil(crudObject)
            results.append(crudObject!)
        }
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }

    final class WrongObject: TableCodable {
        var variable1: Int? = 0
        var variable2: String? = ""
        enum CodingKeys: String, CodingTableKey {
            typealias Root = WrongObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }
    }

    func testSelectFailed() {
        let select = WCDBAssertNoThrowReturned(try database.prepareSelect(on: TestObject.Properties.variable2.asProperty(), fromTable: "nonexistentTable"))!
        XCTAssertThrowsError(try select.nextObject())
    }
}
