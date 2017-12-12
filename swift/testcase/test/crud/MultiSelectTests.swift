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

extension Dictionary where Key==String, Value: CRUDObjectBase {
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

extension Array where Element==[String: CRUDObjectBase] {
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

    class MultiSelectObject: CRUDObjectBase, TableCodable {
        var variable1: Int = 0
        var variable2: String = ""

        enum CodingKeys: String, CodingTableKey {
            typealias Root = MultiSelectObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable1: ColumnConstraintBinding(isPrimary: true,
                                                            orderBy: .ascending,
                                                            isAutoIncrement: true)]
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

    lazy var preInsertedMultiObjects: [[String: CRUDObjectBase]] = [
        [
            CRUDObject.name: preInsertedObjects[0],
            MultiSelectObject.name: preInsertedMultiSelectObjects[0]
        ],
        [
            CRUDObject.name: preInsertedObjects[1],
            MultiSelectObject.name: preInsertedMultiSelectObjects[1]
        ]
    ]

    var multiSelect: MultiSelect!

    override func setUp() {
        super.setUp()

        XCTAssertNoThrow(try database.create(table: MultiSelectObject.name, of: MultiSelectObject.self))

        XCTAssertNoThrow(try database.insert(objects: preInsertedMultiSelectObjects, intoTable: MultiSelectObject.name))

        let tables = [CRUDObject.name, MultiSelectObject.name]
        let optionalMultiSelect = WCDBAssertNoThrowReturned(
            try database.prepareMultiSelect(on:
                CRUDObject.Properties.variable1.in(table: CRUDObject.name),
                CRUDObject.Properties.variable2.in(table: CRUDObject.name),
                MultiSelectObject.Properties.variable1.in(table: MultiSelectObject.name),
                MultiSelectObject.Properties.variable2.in(table: MultiSelectObject.name),
                                            fromTables: tables),
            whenFailed: nil)
        XCTAssertNotNil(optionalMultiSelect)
        let property1 = CRUDObject.Properties.variable1.in(table: CRUDObject.name)
        let property2 = MultiSelectObject.Properties.variable1.in(table: MultiSelectObject.name)
        multiSelect = optionalMultiSelect!.where(property1 == property2)
    }

    func testBase() {
        XCTAssertEqual(multiSelect.tag, database.tag)
        XCTAssertEqual(multiSelect.path, database.path)
    }

    func testSelect() {
        //When
        let results = WCDBAssertNoThrowReturned(
            try multiSelect.allMultiObjects(),
            whenFailed: [[String: TableDecodableBase]]()
        )
        let baseResults = results as? [[String: CRUDObjectBase]]
        //Then
        XCTAssertNotNil(baseResults)
        for i in 0..<2 {
            print(baseResults![i])
//            print(preInsertedMultiObjects[i])
        }
        XCTAssertTrue(baseResults!.sorted() == preInsertedMultiObjects.sorted())
    }

    func testOrderedSelect() {
        //When
        let order = CRUDObject.Properties.variable1.in(table: CRUDObject.name).asOrder(by: .descending)
        let results = WCDBAssertNoThrowReturned(
            try multiSelect.order(by: order).allMultiObjects(),
            whenFailed: [[String: TableDecodableBase]]())
        let baseResults = results as? [[String: CRUDObjectBase]]
        //Then
        XCTAssertNotNil(baseResults)
        XCTAssertTrue(baseResults! == preInsertedMultiObjects.sorted().reversed())
    }

    func testLimitedSelect() {
        //When
        let results = WCDBAssertNoThrowReturned(
            try multiSelect.limit(1).allMultiObjects(),
            whenFailed: [[String: TableDecodableBase]]()
        )
        let baseResults = results as? [[String: CRUDObjectBase]]
        //Then
        XCTAssertNotNil(baseResults)
        XCTAssertEqual(baseResults!.count, 1)
        XCTAssertTrue(baseResults![0] == preInsertedMultiObjects.sorted()[0])
    }

    func testOffsetSelect() {
        //When
        let results = WCDBAssertNoThrowReturned(
            try multiSelect.limit(1, offset: 1).allMultiObjects(),
            whenFailed: [[String: TableDecodableBase]]()
        )
        let baseResults = results as? [[String: CRUDObjectBase]]
        //Then
        XCTAssertNotNil(baseResults)
        XCTAssertEqual(baseResults!.count, 1)
        XCTAssertTrue(baseResults![0] == preInsertedMultiObjects.sorted()[1])
    }

    func testSelectIteration() {
        //When
        var results: [[String: CRUDObjectBase]] = []
        while let result = WCDBAssertNoThrowReturned(try multiSelect.nextMultiObject(), whenFailed: nil) {
            let crudObject = result as? [String: CRUDObjectBase]
            XCTAssertNotNil(crudObject)
            results.append(crudObject!)
        }
        //Then
        XCTAssertTrue(results.sorted() == preInsertedMultiObjects.sorted())
    }
}
