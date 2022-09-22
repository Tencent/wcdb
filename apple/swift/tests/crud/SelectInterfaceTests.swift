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

class SelectInterfaceTests: CRUDTestCase {

    func testSelect() {
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: TestObject.name),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }

    func testConditionalSelect() {
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: TestObject.name, where: TestObject.Properties.variable1 == 2),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, "object2")
    }

    func testOrderedSelect() {
        let order = [TestObject.Properties.variable2.order(.descending)]
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: TestObject.name, orderBy: order),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results, preInsertedObjects.reversed())
    }

    func testLimitedSelect() {
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: TestObject.name, limit: 1),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testOffsetSelect() {
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: TestObject.name, limit: 1, offset: 1),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[1])
    }

    func testPartialSelect() {
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(on: TestObject.Properties.variable2, fromTable: TestObject.name),
            whenFailed: [TestObject]()
        )
        XCTAssertEqual(results.map({ (object) in
            XCTAssertNil(object.variable1)
            XCTAssertNotNil(object.variable2)
            return object.variable2!
        }), preInsertedObjects.map { $0.variable2! })
    }

    func testTableSelect() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(
            try database.getTable(named: TestObject.name)
        )
        XCTAssertNotNil(table)
        // When
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try table!.getObjects(), whenFailed: [TestObject]()
        )
        // Then
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }

    func testRowSelect() {
        // When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(
            try database.getRows(fromTable: TestObject.name)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testConditionalRowSelect() {
        // When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(
            try database.getRows(fromTable: TestObject.name, where: TestObject.Properties.variable1 == 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testOrderedRowSelect() {
        // Give
        let order = [TestObject.Properties.variable1.order(.descending)]
        // When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(
            try database.getRows(fromTable: TestObject.name, orderBy: order)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[1].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testLimitedRowSelect() {
        // When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(
            try database.getRows(fromTable: TestObject.name, limit: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testOffsetRowSelect() {
        // When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(
            try database.getRows(fromTable: TestObject.name, limit: 1, offset: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        let row = results[row: 0]
        XCTAssertEqual(Int(row[0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(row[1].stringValue, preInsertedObjects[1].variable2)
    }

    func testPartialRowSelect() {
        // When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(
            try database.getRows(on: TestObject.Properties.variable2, fromTable: TestObject.name)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(results[row: 0, column: 0].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(results[row: 1, column: 0].stringValue, preInsertedObjects[1].variable2)
    }

    func testRowSelectOne() {
        // When
        let results: FundamentalRow = WCDBAssertNoThrowReturned(
            try database.getRow(fromTable: TestObject.name)
        )
        // Then
        XCTAssertEqual(results.count, 2)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[1].stringValue, preInsertedObjects[0].variable2)
    }

    func testRowSelectColumn() {
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try database.getColumn(on: TestObject.Properties.variable1, fromTable: TestObject.name)
        )
        // Then
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[1].variable1)
    }

    func testConditionalRowSelectColumn() {
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try database.getColumn(on: TestObject.Properties.variable1,
                                   fromTable: TestObject.name,
                                   where: TestObject.Properties.variable1 == 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
    }

    func testOrderedRowSelectColumn() {
        // Give
        let order = [TestObject.Properties.variable1.order(.descending)]
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try database.getColumn(on: TestObject.Properties.variable1, fromTable: TestObject.name, orderBy: order)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[0].variable1)
    }

    func testLimitedRowSelectColumn() {
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try database.getColumn(on: TestObject.Properties.variable1, fromTable: TestObject.name, limit: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
    }

    func testOffsetRowSelectColumn() {
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try database.getColumn(on: TestObject.Properties.variable1, fromTable: TestObject.name, limit: 1, offset: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[1].variable1)
    }

    func testRowSelectDistinctColumn() {
        // Give
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: TestObject.name))
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try database.getDistinctColumn(on: TestObject.Properties.variable2, fromTable: TestObject.name)
        )
        // Then
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(results[1].stringValue, preInsertedObjects[1].variable2)
    }

    func testConditionalRowSelectDistinctColumn() {
        // Give
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: TestObject.name))
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try database.getDistinctColumn(on: TestObject.Properties.variable2,
                                           fromTable: TestObject.name,
                                           where: TestObject.Properties.variable1 == 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[0].variable2)
    }

    func testOrderedRowSelectDistinctColumn() {
        // Give
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: TestObject.name))
        // Give
        let order = [TestObject.Properties.variable2.order(.descending)]
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try database.getDistinctColumn(on: TestObject.Properties.variable2,
                                           fromTable: TestObject.name,
                                           orderBy: order)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[1].variable2)
        XCTAssertEqual(results[1].stringValue, preInsertedObjects[0].variable2)
    }

    func testLimitedRowSelectDistinctColumn() {
        // Give
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: TestObject.name))
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try database.getDistinctColumn(on: TestObject.Properties.variable2, fromTable: TestObject.name, limit: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[0].variable2)
    }

    func testOffsetRowSelectDistinctColumn() {
        // Give
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: TestObject.name))
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try database.getDistinctColumn(on: TestObject.Properties.variable2,
                                           fromTable: TestObject.name,
                                           limit: 1,
                                           offset: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[1].variable2)
    }

    func testRowSelectValue() {
        // When
        let result: FundamentalValue? = WCDBAssertNoThrowReturned(
            try database.getValue(on: TestObject.Properties.variable2, fromTable: TestObject.name)
        )
        // Then
        XCTAssertNotNil(result)
        XCTAssertEqual(result!.stringValue, preInsertedObjects[0].variable2)
    }

    func testSelectOne() {
        let result: TestObject? = WCDBAssertNoThrowReturned(
            try database.getObject(fromTable: TestObject.name)
        )
        XCTAssertNotNil(result)
        XCTAssertEqual(result!, preInsertedObjects[0])
    }

    func testTableRowSelect() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        // When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try table!.getRows())
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testTableRowSelectOne() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        // When
        let results: FundamentalRow = WCDBAssertNoThrowReturned(
            try table!.getRow()
        )
        // Then
        XCTAssertEqual(results.count, 2)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[1].stringValue, preInsertedObjects[0].variable2)
    }

    func testTableRowSelectColumn() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try table!.getColumn(on: TestObject.Properties.variable1)
        )
        // Then
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[1].variable1)
    }

    func testTableConditionalRowSelectColumn() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try table!.getColumn(on: TestObject.Properties.variable1, where: TestObject.Properties.variable1 == 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
    }

    func testTableOrderedRowSelectColumn() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        // Give
        let order = [TestObject.Properties.variable1.order(.descending)]
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try table!.getColumn(on: TestObject.Properties.variable1, orderBy: order)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[0].variable1)
    }

    func testTableLimitedRowSelectColumn() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try table!.getColumn(on: TestObject.Properties.variable1, limit: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
    }

    func testTableOffsetRowSelectColumn() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try table!.getColumn(on: TestObject.Properties.variable1, limit: 1, offset: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[1].variable1)
    }

    func testTableRowSelectDistinctColumn() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: TestObject.name))
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try table!.getDistinctColumn(on: TestObject.Properties.variable2)
        )
        // Then
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(results[1].stringValue, preInsertedObjects[1].variable2)
    }

    func testTableConditionalRowSelectDistinctColumn() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: TestObject.name))
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try table!.getDistinctColumn(on: TestObject.Properties.variable2,
                                         where: TestObject.Properties.variable1 == 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[0].variable2)
    }

    func testTableOrderedRowSelectDistinctColumn() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: TestObject.name))
        // Give
        let order = [TestObject.Properties.variable2.order(.descending)]
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try table!.getDistinctColumn(on: TestObject.Properties.variable2, orderBy: order)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[1].variable2)
        XCTAssertEqual(results[1].stringValue, preInsertedObjects[0].variable2)
    }

    func testTableLimitedRowSelectDistinctColumn() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: TestObject.name))
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try table!.getDistinctColumn(on: TestObject.Properties.variable2, limit: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[0].variable2)
    }

    func testTableOffsetRowSelectDistinctColumn() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(objects: object1, intoTable: TestObject.name))
        // When
        let results: FundamentalColumn = WCDBAssertNoThrowReturned(
            try table!.getDistinctColumn(on: TestObject.Properties.variable2, limit: 1, offset: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[1].variable2)
    }

    func testTableRowSelectValue() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        // When
        let result: FundamentalValue? = WCDBAssertNoThrowReturned(
            try table!.getValue(on: TestObject.Properties.variable2)
        )
        // Then
        XCTAssertNotNil(result)
        XCTAssertEqual(result!.stringValue, preInsertedObjects[0].variable2)
    }

    func testTableSelectOne() {
        // Give
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        // When
        let result: TestObject? = WCDBAssertNoThrowReturned(
            try table!.getObject()
        )
        // Then
        XCTAssertNotNil(result)
        XCTAssertEqual(result!, preInsertedObjects[0])
    }

    func testSelectEmpty() {
        let tableName = TestObject.name
        // Give
        XCTAssertNoThrow(try database.delete(fromTable: tableName))
        // Then
        do {
            let row = try database.getRow(on: Column.all, fromTable: tableName)
            XCTAssertEqual(row.count, 0)

            let distinctValue = try database.getDistinctValue(on: Column.all, fromTable: tableName)
            XCTAssertEqual(distinctValue.type, .null)

            let value = try database.getValue(on: Column.all, fromTable: tableName)
            XCTAssertEqual(value.type, .null)
        } catch let error {
            XCTFail(error.localizedDescription)
        }
    }

    func testTableSelectEmpty() {
        // Give
        XCTAssertNoThrow(try database.delete(fromTable: TestObject.name))
        let table: Table<TestObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: TestObject.name))
        XCTAssertNotNil(table)
        let wrappedTable = table!
        // Then
        do {
            let row = try wrappedTable.getRow(on: Column.all)
            XCTAssertEqual(row.count, 0)

            let distinctValue = try wrappedTable.getDistinctValue(on: Column.all)
            XCTAssertEqual(distinctValue.type, .null)

            let value = try wrappedTable.getValue(on: Column.all)
            XCTAssertEqual(value.type, .null)
        } catch let error {
            XCTFail(error.localizedDescription)
        }
    }
}
