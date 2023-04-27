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
        let table: Table<TestObject> = database.getTable(named: TestObject.name)
        // When
        let results: [TestObject] = WCDBAssertNoThrowReturned(
            try table.getObjects(), whenFailed: [TestObject]()
        )
        // Then
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }

    func testRowSelect() {
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(
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
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(
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
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(
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
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(
            try database.getRows(fromTable: TestObject.name, limit: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testOffsetRowSelect() {
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(
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
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(
            try database.getRows(on: TestObject.Properties.variable2, fromTable: TestObject.name)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(results[row: 0, column: 0].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(results[row: 1, column: 0].stringValue, preInsertedObjects[1].variable2)
    }

    func testRowSelectOne() {
        // When
        let results: OneRowValue = WCDBAssertNoThrowReturned(
            try database.getRow(fromTable: TestObject.name)
        )
        // Then
        XCTAssertEqual(results.count, 2)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[1].stringValue, preInsertedObjects[0].variable2)
    }

    func testRowSelectColumn() {
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try database.getColumn(on: TestObject.Properties.variable1, fromTable: TestObject.name)
        )
        // Then
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[1].variable1)
    }

    func testConditionalRowSelectColumn() {
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
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
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try database.getColumn(on: TestObject.Properties.variable1, fromTable: TestObject.name, orderBy: order)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[0].variable1)
    }

    func testLimitedRowSelectColumn() {
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try database.getColumn(on: TestObject.Properties.variable1, fromTable: TestObject.name, limit: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
    }

    func testOffsetRowSelectColumn() {
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
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
        XCTAssertNoThrow(try database.insert(object1, intoTable: TestObject.name))
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
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
        XCTAssertNoThrow(try database.insert(object1, intoTable: TestObject.name))
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
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
        XCTAssertNoThrow(try database.insert(object1, intoTable: TestObject.name))
        // Give
        let order = [TestObject.Properties.variable2.order(.descending)]
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
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
        XCTAssertNoThrow(try database.insert(object1, intoTable: TestObject.name))
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
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
        XCTAssertNoThrow(try database.insert(object1, intoTable: TestObject.name))
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
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
        let result: Value? = WCDBAssertNoThrowReturned(
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
        // When
        let results: MultiRowsValue = WCDBAssertNoThrowReturned(try table.getRows())
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testTableRowSelectOne() {
        // When
        let results: OneRowValue = WCDBAssertNoThrowReturned(
            try table.getRow()
        )
        // Then
        XCTAssertEqual(results.count, 2)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[1].stringValue, preInsertedObjects[0].variable2)
    }

    func testTableRowSelectColumn() {
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try table.getColumn(on: TestObject.Properties.variable1)
        )
        // Then
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[1].variable1)
    }

    func testTableConditionalRowSelectColumn() {
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try table.getColumn(on: TestObject.Properties.variable1, where: TestObject.Properties.variable1 == 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
    }

    func testTableOrderedRowSelectColumn() {
        // Give
        let order = [TestObject.Properties.variable1.order(.descending)]
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try table.getColumn(on: TestObject.Properties.variable1, orderBy: order)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(Int(results[1].int64Value), preInsertedObjects[0].variable1)
    }

    func testTableLimitedRowSelectColumn() {
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try table.getColumn(on: TestObject.Properties.variable1, limit: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[0].variable1)
    }

    func testTableOffsetRowSelectColumn() {
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try table.getColumn(on: TestObject.Properties.variable1, limit: 1, offset: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[0].int64Value), preInsertedObjects[1].variable1)
    }

    func testTableRowSelectDistinctColumn() {
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(object1, intoTable: TestObject.name))
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try table.getDistinctColumn(on: TestObject.Properties.variable2)
        )
        // Then
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(results[1].stringValue, preInsertedObjects[1].variable2)
    }

    func testTableConditionalRowSelectDistinctColumn() {
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(object1, intoTable: TestObject.name))
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try table.getDistinctColumn(on: TestObject.Properties.variable2,
                                         where: TestObject.Properties.variable1 == 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[0].variable2)
    }

    func testTableOrderedRowSelectDistinctColumn() {
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(object1, intoTable: TestObject.name))
        // Give
        let order = [TestObject.Properties.variable2.order(.descending)]
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try table.getDistinctColumn(on: TestObject.Properties.variable2, orderBy: order)
        )
        // Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[1].variable2)
        XCTAssertEqual(results[1].stringValue, preInsertedObjects[0].variable2)
    }

    func testTableLimitedRowSelectDistinctColumn() {
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(object1, intoTable: TestObject.name))
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try table.getDistinctColumn(on: TestObject.Properties.variable2, limit: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[0].variable2)
    }

    func testTableOffsetRowSelectDistinctColumn() {
        let object1 = TestObject()
        object1.variable1 = 3
        object1.variable2 = "object1"
        XCTAssertNoThrow(try database.insert(object1, intoTable: TestObject.name))
        // When
        let results: OneColumnValue = WCDBAssertNoThrowReturned(
            try table.getDistinctColumn(on: TestObject.Properties.variable2, limit: 1, offset: 1)
        )
        // Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].stringValue, preInsertedObjects[1].variable2)
    }

    func testTableRowSelectValue() {
        // When
        let result: Value? = WCDBAssertNoThrowReturned(
            try table.getValue(on: TestObject.Properties.variable2)
        )
        // Then
        XCTAssertNotNil(result)
        XCTAssertEqual(result!.stringValue, preInsertedObjects[0].variable2)
    }

    func testTableSelectOne() {
        // When
        let result: TestObject? = WCDBAssertNoThrowReturned(
            try table.getObject()
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
        let table: Table<TestObject> =  database.getTable(named: TestObject.name)
        // Then
        do {
            let row = try table.getRow(on: Column.all)
            XCTAssertEqual(row.count, 0)

            let distinctValue = try table.getDistinctValue(on: Column.all)
            XCTAssertEqual(distinctValue.type, .null)

            let value = try table.getValue(on: Column.all)
            XCTAssertEqual(value.type, .null)
        } catch let error {
            XCTFail(error.localizedDescription)
        }
    }

    func testSelectValue() {
        let value: Value? = WCDBAssertNoThrowReturned(
            try database.getValue(from: StatementSelect().select(TestObject.Properties.variable1.max()).from(TestObject.name))
        )
        XCTAssertNotNil(value)
        XCTAssertEqual(value!.int64Value, Int64(preInsertedObjects[1].variable1!))
    }

    func testSelectOneColumn() {
        let column: OneColumnValue = WCDBAssertNoThrowReturned(
            try database.getColumn(from: StatementSelect().select(TestObject.Properties.variable1).from(TestObject.name))
        )
        XCTAssertEqual(column.count, preInsertedObjects.count)
        XCTAssertEqual(column[0].int64Value, Int64(preInsertedObjects[0].variable1!))
        XCTAssertEqual(column[1].int64Value, Int64(preInsertedObjects[1].variable1!))
    }

    func testSelectOneRow() {
        let row: OneRowValue? = WCDBAssertNoThrowReturned(
            try database.getRow(from: StatementSelect().select(TestObject.Properties.all).from(TestObject.name).limit(1))
        )
        XCTAssertNotNil(row)
        XCTAssertEqual(row!.count, TestObject.Properties.all.count)
        XCTAssertEqual(row![0].int64Value, Int64(preInsertedObjects[0].variable1!))
        XCTAssertEqual(row![1].stringValue, preInsertedObjects[0].variable2!)
    }

    func testSelectMultiRows() {
        let rows: MultiRowsValue = WCDBAssertNoThrowReturned(
            try database.getRows(from: StatementSelect().select(TestObject.Properties.all).from(TestObject.name))
        )
        XCTAssertEqual(rows.count, preInsertedObjects.count)
        XCTAssertEqual(rows[0][0].int64Value, Int64(preInsertedObjects[0].variable1!))
        XCTAssertEqual(rows[1][1].stringValue, preInsertedObjects[1].variable2)
    }
}
