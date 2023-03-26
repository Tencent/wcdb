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

extension Dictionary where Key==String, Value: TestObjectBase {
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

extension Array where Element==[String: TestObjectBase] {
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

    final class MultiSelectObject: TestObjectBase, TableCodable {
        var variable1: Int = 0
        var variable2: String = ""

        enum CodingKeys: String, CodingTableKey {
            typealias Root = MultiSelectObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(.variable1, isPrimary: true, orderBy: .ascending, isAutoIncrement: true)
            }
        }

        override var hashValue: Int {
            return (String(variable1)+variable2).hashValue
        }

        override var debugDescription: String {
            return "\(variable1), \(variable2)"
        }
    }

    var preInsertedMultiSelectObjects: [MultiSelectObject] = {
        let object1 = MultiSelectObject()
        object1.variable1 = 1
        object1.variable2 = "multiObject1"
        let object2 = MultiSelectObject()
        object2.variable1 = 2
        object2.variable2 = "multiObject2"
        return [object1, object2]
    }()

    lazy var preInsertedMultiObjects: [[String: TestObjectBase]] = [
        [
            TestObject.name: preInsertedObjects[0],
            MultiSelectObject.name: preInsertedMultiSelectObjects[0]
        ],
        [
            TestObject.name: preInsertedObjects[1],
            MultiSelectObject.name: preInsertedMultiSelectObjects[1]
        ]
    ]

    var multiSelect: MultiSelect!

    override func setUp() {
        super.setUp()

        XCTAssertNoThrow(try database.create(table: MultiSelectObject.name, of: MultiSelectObject.self))

        XCTAssertNoThrow(try database.insert(preInsertedMultiSelectObjects, intoTable: MultiSelectObject.name))

        let tables = [TestObject.name, MultiSelectObject.name]
        let optionalMultiSelect = WCDBAssertNoThrowReturned(
            try database.prepareMultiSelect(on:
                TestObject.Properties.variable1.in(table: TestObject.name),
                TestObject.Properties.variable2.in(table: TestObject.name),
                MultiSelectObject.Properties.variable1.in(table: MultiSelectObject.name),
                MultiSelectObject.Properties.variable2.in(table: MultiSelectObject.name),
                                            fromTables: tables))
        XCTAssertNotNil(optionalMultiSelect)
        let property1 = TestObject.Properties.variable1.in(table: TestObject.name)
        let property2 = MultiSelectObject.Properties.variable1.in(table: MultiSelectObject.name)
        multiSelect = optionalMultiSelect!.where(property1 == property2)
    }

    func testSelect() {
        // When
        let results = WCDBAssertNoThrowReturned(
            try multiSelect.allMultiObjects(),
            whenFailed: [[String: TableDecodableBase]]()
        )
        let baseResults = results as? [[String: TestObjectBase]]
        // Then
        XCTAssertNotNil(baseResults)
        XCTAssertTrue(baseResults! == preInsertedMultiObjects)
    }

    func testOrderedSelect() {
        // When
        let order = TestObject.Properties.variable1.in(table: TestObject.name).order(.descending)
        let results = WCDBAssertNoThrowReturned(
            try multiSelect.order(by: order).allMultiObjects(),
            whenFailed: [[String: TableDecodableBase]]())
        let baseResults = results as? [[String: TestObjectBase]]
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
        let baseResults = results as? [[String: TestObjectBase]]
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
        let baseResults = results as? [[String: TestObjectBase]]
        // Then
        XCTAssertNotNil(baseResults)
        XCTAssertEqual(baseResults!.count, 1)
        XCTAssertTrue(baseResults![0] == preInsertedMultiObjects[1])
    }

    func testSelectIteration() {
        // When
        var results: [[String: TestObjectBase]] = []
        while let result = WCDBAssertNoThrowReturned(try multiSelect.nextMultiObject()) {
            let crudObject = result as? [String: TestObjectBase]
            XCTAssertNotNil(crudObject)
            results.append(crudObject!)
        }
        // Then
        XCTAssertTrue(results == preInsertedMultiObjects)
    }
}
