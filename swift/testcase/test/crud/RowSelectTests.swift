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

class RowSelectTests: CRUDTestCase {

    var rowSelect: RowSelect!

    override func setUp() {
        super.setUp()

        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(fromTable: CRUDObject.name),
            whenFailed: nil
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!
    }

    func testBase() {
        XCTAssertNotNil(rowSelect.tag)
        XCTAssertEqual(rowSelect.tag, database.tag)
        XCTAssertEqual(rowSelect.path, database.path)
    }

    func testRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try rowSelect.allRows())
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testConditionalRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(
            try rowSelect.where(CRUDObject.Properties.variable1 == 1).allRows()
        )
        //Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testOrderedRowSelect() {
        //Give
        let order = (CRUDObject.Properties.variable1).asOrder(by: .descending)
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try rowSelect.order(by: order).allRows())
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[1].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testLimitedRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try rowSelect.limit(1).allRows())
        //Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testOffsetRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try rowSelect.limit(1, offset: 1).allRows())
        //Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testGroupByRowSelect() {
        //Give
        let preInsertedObjects: [CRUDObject] = {
            let object1 = CRUDObject()
            object1.variable1 = 3
            object1.variable2 = "object1"
            let object2 = CRUDObject()
            object2.variable1 = 4
            object2.variable2 = "object2"
            return [object1, object2]
        }()
        XCTAssertNoThrow(try database.insert(objects: preInsertedObjects, intoTable: CRUDObject.name))

        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(on: Column.any.count(),
                                          fromTable: CRUDObject.name),
            whenFailed: nil
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!
        rowSelect.group(by: CRUDObject.Properties.variable2).having(CRUDObject.Properties.variable1 > 0)
        //When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(try rowSelect.allValues())
        //Then
        XCTAssertEqual(results.count, 2)
        XCTAssertEqual(results[0].int32Value, 2)
        XCTAssertEqual(results[1].int32Value, 2)
    }

    func testPartialRowSelect() {
        //Give
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(on: CRUDObject.Properties.variable2, fromTable: CRUDObject.name),
            whenFailed: nil
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try rowSelect.allRows())
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(results[row: 0, column: 0].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(results[row: 1, column: 0].stringValue, preInsertedObjects[1].variable2)
    }

    func testRowSelectIteration() {
        //When
        var results: FundamentalRowXColumn = []
        while let result = WCDBAssertNoThrowReturned(try rowSelect.nextRow(), whenFailed: nil) {
            results.append(result)
        }
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testRowSelectValue() {
        //When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(try rowSelect.allValues())
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[1].variable1)
    }

    func testRowSelectValueIteraion() {
        //When
        var results: FundamentalColumn = []
        while let result = WCDBAssertNoThrowReturned(try rowSelect.nextValue(), whenFailed: nil) {
            results.append(result)
        }
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[1].variable1)
    }

    func testRowSelectMultiTables() {
        //Give
        let table = CRUDObject.name
        let table2 = "\(CRUDObject.name)_2"
        XCTAssertNoThrow(try database.create(table: table2, of: CRUDObject.self))

        let object1 = CRUDObject()
        object1.variable1 = 1
        object1.variable2 = "object3"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: table2))

        let column1Table1 = CRUDObject.Properties.variable1.in(table: table)
        let column1Table2 = CRUDObject.Properties.variable1.in(table: table2)
        let column2Table1 = CRUDObject.Properties.variable2.in(table: table)
        let column2Table2 = CRUDObject.Properties.variable2.in(table: table2)
        //When
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(on: column2Table1, column2Table2,
                                          fromTables: [table, table2])
                .where(column1Table1 == column1Table2),
            whenFailed: nil
        )
        XCTAssertNotNil(optionalRowSelect)
        let rowSelect = optionalRowSelect!
        let rows = WCDBAssertNoThrowReturned(try rowSelect.allRows())
        //Then
        XCTAssertEqual(rows.count, 1)
        XCTAssertEqual(rows[row: 0, column: 0].stringValue, "object1")
        XCTAssertEqual(rows[row: 0, column: 1].stringValue, "object3")
    }

    func testRowSelectAllMultiTables() {
        //Give
        let table = CRUDObject.name
        let table2 = "\(CRUDObject.name)_2"
        XCTAssertNoThrow(try database.create(table: table2, of: CRUDObject.self))

        let object1 = CRUDObject()
        object1.variable1 = 1
        object1.variable2 = "object3"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: table2))

        let column1Table1 = CRUDObject.Properties.variable1.in(table: table)
        let column1Table2 = CRUDObject.Properties.variable1.in(table: table2)
        //When
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(fromTables: [table, table2])
                .where(column1Table1 == column1Table2),
            whenFailed: nil
        )
        XCTAssertNotNil(optionalRowSelect)
        let rowSelect = optionalRowSelect!
        let rows = WCDBAssertNoThrowReturned(try rowSelect.allRows())
        //Then
        XCTAssertEqual(rows.count, 1)
        XCTAssertEqual(Int(rows[row: 0, column: 0].int32Value), 1)
        XCTAssertEqual(rows[row: 0, column: 1].stringValue, "object1")
        XCTAssertEqual(Int(rows[row: 0, column: 2].int32Value), 1)
        XCTAssertEqual(rows[row: 0, column: 3].stringValue, "object3")
    }

    func testRowSelectFailed() {
        XCTAssertThrowsError(try database.prepareRowSelect(fromTables: []))
    }
    func testRowSelectType() {
        //Give
        let optionalRowSelect = WCDBAssertNoThrowReturned(
            try database.prepareRowSelect(on: 1, 2.0, "3", fromTable: CRUDObject.name),
            whenFailed: nil
        )
        XCTAssertNotNil(optionalRowSelect)
        rowSelect = optionalRowSelect!

        //When
        let results: FundamentalRow? = WCDBAssertNoThrowReturned(try rowSelect.nextRow())
        XCTAssertNotNil(results)
        XCTAssertEqual(results!.count, 3)
        //Then
        XCTAssertEqual(results![0].int32Value, 1)
        XCTAssertEqual(results![1].doubleValue, 2.0)
        XCTAssertEqual(results![2].stringValue, "3")
    }
}
