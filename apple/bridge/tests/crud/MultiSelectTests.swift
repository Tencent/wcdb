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

extension Dictionary where Key==String, Value: CRUDObject {
    static func == (lhs: [Key: Value], rhs: [Key: Value]) -> Bool {
        guard lhs.count == rhs.count else {
            return false
        }
        for (key, value) in lhs {
            guard rhs[key] == value else {
                return false
            }
        }
        return true
    }

    static func < (lhs: [Key: Value], rhs: [Key: Value]) -> Bool {
        guard lhs.count == rhs.count else {
            return lhs.count < rhs.count
        }
        for i in 0..<lhs.count {
            let lhsWrap = lhs[lhs.index(lhs.startIndex, offsetBy: i)]
            let rhsWrap = rhs[rhs.index(rhs.startIndex, offsetBy: i)]
            guard lhsWrap.key == rhsWrap.key else {
                return lhsWrap.key < rhsWrap.key
            }
            guard lhsWrap.value == rhsWrap.value else {
                return lhsWrap.value < rhsWrap.value
            }
        }
        return false
    }
}

extension Array where Element==[String: CRUDObject] {
    static func == (lhs: [Element], rhs: [Element]) -> Bool {
        guard lhs.count == rhs.count else {
            return false
        }
        for i in 0..<lhs.count {
            guard lhs[i] == rhs[i] else {
                return false
            }
        }
        return true
    }

    func sorted() -> [Element] {
        return self.sorted { (lhs, rhs) -> Bool in
            return lhs < rhs
        }
    }
}

class MultiSelectTests: CRUDTestCase {

    var preInsertedCRUDObjects: [CRUDObject] = {
        let object1 = CRUDObject()
        object1.variable1 = 1
        object1.variable2 = "multiObject1"
        let object2 = CRUDObject()
        object2.variable1 = 2
        object2.variable2 = "multiObject2"
        return [object1, object2]
    }()

    lazy var preInsertedMultiObjects: [[String: CRUDObject]] = [
        [
            CRUDObject.name: preInsertedObjects[0],
            CRUDObject.nameForMultiSelect(): preInsertedCRUDObjects[0]
        ],
        [
            CRUDObject.name: preInsertedObjects[1],
            CRUDObject.nameForMultiSelect(): preInsertedCRUDObjects[1]
        ]
    ]

    var multiSelect: MultiSelect!

    override func setUp() {
        super.setUp()

        XCTAssertNoThrow(try database.create(table: CRUDObject.nameForMultiSelect(), of: CRUDObject.self))
        XCTAssertNoThrow(try database.insert(objects: preInsertedCRUDObjects, intoTable: CRUDObject.nameForMultiSelect()))

        let tables = [CRUDObject.name, CRUDObject.nameForMultiSelect()]
        let property1 = CRUDObject.variable1().in(table: CRUDObject.name)
        let property2 = CRUDObject.variable1().in(table: CRUDObject.nameForMultiSelect())
        multiSelect = WCDBAssertNoThrowReturned(
            try database.prepareMultiSelect(on: CRUDObject.variable1().in(table: CRUDObject.name),
                                            CRUDObject.variable2().in(table: CRUDObject.name),
                                            CRUDObject.variable1().in(table: CRUDObject.nameForMultiSelect()),
                                            CRUDObject.variable2().in(table: CRUDObject.nameForMultiSelect()),
                                            fromTables: tables).where(property1 == property2)
        )!
    }

    func testSelect() {
        // When
        let results = WCDBAssertNoThrowReturned(
            try multiSelect.allMultiObjects(),
            whenFailed: [[String: TableDecodableBase]]()
        )
        let baseResults = results as? [[String: CRUDObject]]
        // Then
        XCTAssertNotNil(baseResults)
        XCTAssertTrue(baseResults! == preInsertedMultiObjects)
    }

    func testOrderedSelect() {
        // When
        let order = CRUDObject.variable1().in(table: CRUDObject.name).order(.descending)
        let results = WCDBAssertNoThrowReturned(
            try multiSelect.order(by: order).allMultiObjects(),
            whenFailed: [[String: TableDecodableBase]]())
        let baseResults = results as? [[String: CRUDObject]]
        // Then
        XCTAssertNotNil(baseResults)
        XCTAssertTrue(baseResults! == preInsertedMultiObjects.reversed())
    }

    func testLimitedSelect() {
        // When
        let results = WCDBAssertNoThrowReturned(
            try multiSelect.limit(1).allMultiObjects(),
            whenFailed: [[String: TableDecodableBase]]()
        )
        let baseResults = results as? [[String: CRUDObject]]
        // Then
        XCTAssertNotNil(baseResults)
        XCTAssertEqual(baseResults!.count, 1)
        XCTAssertTrue(baseResults![0] == preInsertedMultiObjects[0])
    }

    func testOffsetSelect() {
        // When
        let results = WCDBAssertNoThrowReturned(
            try multiSelect.limit(1, offset: 1).allMultiObjects(),
            whenFailed: [[String: TableDecodableBase]]()
        )
        let baseResults = results as? [[String: CRUDObject]]
        // Then
        XCTAssertNotNil(baseResults)
        XCTAssertEqual(baseResults!.count, 1)
        XCTAssertTrue(baseResults![0] == preInsertedMultiObjects[1])
    }

    func testSelectIteration() {
        // When
        var results: [[String: CRUDObject]] = []
        while let result = WCDBAssertNoThrowReturned(try multiSelect.nextMultiObject()) {
            let crudObject = result as? [String: CRUDObject]
            XCTAssertNotNil(crudObject)
            results.append(crudObject!)
        }
        // Then
        XCTAssertTrue(results == preInsertedMultiObjects)
    }
}
