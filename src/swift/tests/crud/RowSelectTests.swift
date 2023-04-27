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

class RowSelectTests: CRUDTestCase {

    var rowSelect: RowSelect!

    override func setUp() {
        super.setUp()

        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(fromTable: TestObject.name)
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!
    }

    func testRowSelect() {
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(try rowSelect.allRows())
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testTableRowSelect() {
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try table.prepareRowSelect()
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(try rowSelect.allRows())
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testConditionalRowSelect() {
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(
            try rowSelect.where(TestObject.Properties.variable1 == 1).allRows()
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testOrderedRowSelect() {
        // Give
        let order = TestObject.Properties.variable1.order(.descending)
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(try rowSelect.order(by: order).allRows())
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[1].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testLimitedRowSelect() {
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(try rowSelect.limit(1).allRows())
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testOffsetRowSelect() {
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(try rowSelect.limit(1, offset: 1).allRows())
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testGroupByRowSelect() {
        // Give
        let preInsertedObjects: [TestObject] = {
            let object1 = TestObject()
            object1.variable1 = 3
            object1.variable2 = "object1"
            let object2 = TestObject()
            object2.variable1 = 4
            object2.variable2 = "object2"
            return [object1, object2]
        }()
        XCTAssertNoThrow(try database.insert(preInsertedObjects, intoTable: TestObject.name))

        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(on: Column.all.count(),
                                          fromTable: TestObject.name)
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!
        rowSelect.group(by: TestObject.Properties.variable2).having(TestObject.Properties.variable1 > 0)
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(try rowSelect.allValues())
        // Then
        XCTAssertEqual(results.count, 2)
        XCTAssertEqual(results[0].int32Value, 2)
        XCTAssertEqual(results[1].int32Value, 2)
    }

    func testTableGroupByRowSelect() {
        // Give
        var optionalRowSelect = WCDBAssertNoThrowReturned(
            try table.prepareRowSelect()
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!
        let preInsertedObjects: [TestObject] = {
            let object1 = TestObject()
            object1.variable1 = 3
            object1.variable2 = "object1"
            let object2 = TestObject()
            object2.variable1 = 4
            object2.variable2 = "object2"
            return [object1, object2]
        }()
        XCTAssertNoThrow(try table.insert(preInsertedObjects))

        optionalRowSelect = WCDBAssertNoThrowReturned(
            try table.prepareRowSelect(on: Column.all.count())
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!
        rowSelect.group(by: TestObject.Properties.variable2).having(TestObject.Properties.variable1 > 0)
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(try rowSelect.allValues())
        // Then
        XCTAssertEqual(results.count, 2)
        XCTAssertEqual(results[0].int32Value, 2)
        XCTAssertEqual(results[1].int32Value, 2)
    }

    func testPartialRowSelect() {
        // Give
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(on: TestObject.Properties.variable2, fromTable: TestObject.name)
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(try rowSelect.allRows())
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(results[row: 0, column: 0].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(results[row: 1, column: 0].stringValue, preInsertedObjects[1].variable2)
    }

    func testTablePartialRowSelect() {
        // Give
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try table.prepareRowSelect(on: TestObject.Properties.variable2)
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(try rowSelect.allRows())
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(results[row: 0, column: 0].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(results[row: 1, column: 0].stringValue, preInsertedObjects[1].variable2)
    }

    func testRowSelectIteration() {
        // When
        var results: MultiRowsValue = []
        while let result = WCDBAssertNoThrowReturned(try rowSelect.nextRow()) {
            results.append(result)
        }
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testRowSelectValue() {
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(try rowSelect.allValues())
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[1].variable1)
    }

    func testRowSelectValueIteraion() {
        // When
        var results: OneColumnValue = []
        while let result = WCDBAssertNoThrowReturned(try rowSelect.nextValue()) {
            results.append(result)
        }
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[1].variable1)
    }

    func testRowSelectMultiTables() {
        // Give
        let table = TestObject.name
        let table2 = "\(TestObject.name)_2"
        XCTAssertNoThrow(try database.create(table: table2, of: TestObject.self))

        let object1 = TestObject()
        object1.variable1 = 1
        object1.variable2 = "object3"
        XCTAssertNoThrow(try database.insert(object1, intoTable: table2))

        let column1Table1 = TestObject.Properties.variable1.in(table: table)
        let column1Table2 = TestObject.Properties.variable1.in(table: table2)
        let column2Table1 = TestObject.Properties.variable2.in(table: table)
        let column2Table2 = TestObject.Properties.variable2.in(table: table2)
        // When
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(on: column2Table1, column2Table2,
                                          fromTables: [table, table2])
                .where(column1Table1 == column1Table2)
        )
        XCTAssertNotNil(optionalRowSelect)
        let rowSelect = optionalRowSelect!
        let rows = WCDBAssertNoThrowReturned(try rowSelect.allRows())
        // Then
        XCTAssertEqual(rows.count, 1)
        XCTAssertEqual(rows[row: 0, column: 0].stringValue, "object1")
        XCTAssertEqual(rows[row: 0, column: 1].stringValue, "object3")
    }

    func testRowSelectAllMultiTables() {
        // Give
        let table = TestObject.name
        let table2 = "\(TestObject.name)_2"
        XCTAssertNoThrow(try database.create(table: table2, of: TestObject.self))

        let object1 = TestObject()
        object1.variable1 = 1
        object1.variable2 = "object3"
        XCTAssertNoThrow(try database.insert(object1, intoTable: table2))

        let column1Table1 = TestObject.Properties.variable1.in(table: table)
        let column1Table2 = TestObject.Properties.variable1.in(table: table2)
        // When
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(fromTables: [table, table2])
                .where(column1Table1 == column1Table2)
        )
        XCTAssertNotNil(optionalRowSelect)
        let rowSelect = optionalRowSelect!
        let rows = WCDBAssertNoThrowReturned(try rowSelect.allRows())
        // Then
        XCTAssertEqual(rows.count, 1)
        XCTAssertEqual(Int(rows[row: 0, column: 0].int32Value), 1)
        XCTAssertEqual(rows[row: 0, column: 1].stringValue, "object1")
        XCTAssertEqual(Int(rows[row: 0, column: 2].int32Value), 1)
        XCTAssertEqual(rows[row: 0, column: 3].stringValue, "object3")
    }

    func testRowSelectFailed() {
        XCTAssertNoThrow(try database.prepareRowSelect(fromTables: []))
    }

    func testRowSelectType() {
        // Give
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(on: 1, 2.0, "3", fromTable: TestObject.name)
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!

        // When
        let results: OneRowValue? = WCDBAssertNoThrowReturned(try rowSelect.nextRow())
        XCTAssertNotNil(results)
        XCTAssertEqual(results!.count, 3)
        // Then
        XCTAssertEqual(results![0].int32Value, 1)
        XCTAssertEqual(results![1].doubleValue, 2.0)
        XCTAssertEqual(results![2].stringValue, "3")
    }

    func testTableRowSelectType() {
        // Give
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try table.prepareRowSelect(on: 1, 2.0, "3")
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!

        // When
        let results: OneRowValue? = WCDBAssertNoThrowReturned(try rowSelect.nextRow())
        XCTAssertNotNil(results)
        XCTAssertEqual(results!.count, 3)
        // Then
        XCTAssertEqual(results![0].int32Value, 1)
        XCTAssertEqual(results![1].doubleValue, 2.0)
        XCTAssertEqual(results![2].stringValue, "3")
    }
}
