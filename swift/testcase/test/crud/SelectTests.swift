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

class SelectTests: CRUDTestCase {

    var select: Select!

    override func setUp() {
        super.setUp()

        let optionalSelect = WCDBAssertNoThrowReturned(
            try database.prepareSelect(of: CRUDObject.self, fromTable: CRUDObject.name)
        )
        XCTAssertNotNil(optionalSelect)
        select = optionalSelect!
    }

    func testBase() {
        XCTAssertNotNil(select.tag)
        XCTAssertEqual(select.tag, database.tag)
        XCTAssertEqual(select.path, database.path)
    }

    func testSelect() {
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try select.allObjects(), whenFailed: [CRUDObject]())
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }

    func testConditionalSelect() {
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try select.where(CRUDObject.Properties.variable1 == 2).allObjects(),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, "object2")
    }

    func testOrderedSelect() {
        let order = (CRUDObject.Properties.variable2).asOrder(by: .descending)
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try select.order(by: order).allObjects(),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results, preInsertedObjects.reversed())
    }

    func testLimitedSelect() {
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try select.limit(1).allObjects(),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testOffsetSelect() {
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try select.limit(from: 1, to: 1).allObjects(),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[1])
    }

    func testPartialSelect() {
        let optionalSelect = WCDBAssertNoThrowReturned(
            try database.prepareSelect(on: CRUDObject.Properties.variable2.asProperty(), fromTable: CRUDObject.name)
        )
        XCTAssertNotNil(optionalSelect)
        let select = optionalSelect!
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try select.allObjects(),
            whenFailed: [CRUDObject]()
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
        var results: [CRUDObject] = []
        while let result: CRUDObject = WCDBAssertNoThrowReturned(try select.nextObject()) {
            results.append(result)
        }
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }

    func testUnspecificSelect() {
        let results = WCDBAssertNoThrowReturned(try select.allObjects(), whenFailed: [Any]())
        let objects = results as? [CRUDObject]
        XCTAssertNotNil(objects)
        XCTAssertEqual(objects!.sorted(), preInsertedObjects.sorted())
    }

    func testUnspecificSelectIteration() {
        var results: [CRUDObject] = []
        while let result = WCDBAssertNoThrowReturned(try select.nextObject()) {
            let crudObject = result as? CRUDObject
            XCTAssertNotNil(crudObject)
            results.append(crudObject!)
        }
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }

    class WrongObject: TableCodable {
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
        let optionalSelect = WCDBAssertNoThrowReturned(
            try database.prepareSelect(of: CRUDObject.self, fromTable: "nonexistentTable")
        )
        XCTAssertNotNil(optionalSelect)
        select = optionalSelect!
        XCTAssertThrowsError(try select.nextObject())
    }
}
