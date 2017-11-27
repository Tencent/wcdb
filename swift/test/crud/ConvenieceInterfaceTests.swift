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

class ConvenienceInterfaceTests: CURDTestCase {

    func testInsert() {
        //Give
        let object = CRUDObject()
        object.variable1 = preInsertedObjects.count + 1
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.insert(objects: object, into: CRUDObject.name))
        //Then
        let result: CRUDObject? = WCDBAssertNoThrowReturned(try database.getObject(from: CRUDObject.name, where: \CRUDObject.variable1 == object.variable1))
        XCTAssertNotNil(result)
        XCTAssertEqual(result!, object)
    }
        
    func testAutoIncrementInsert() {
        //Give
        let object = CRUDObject()
        let expectedRowID = preInsertedObjects.count + 1 
        object.isAutoIncrement = true
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.insert(objects: object, into: CRUDObject.name))
        //Then
        XCTAssertEqual(object.lastInsertedRowID, Int64(expectedRowID))
        let result: CRUDObject? = WCDBAssertNoThrowReturned(try database.getObject(from: CRUDObject.name, where: \CRUDObject.variable1 == expectedRowID))
        XCTAssertNotNil(result)
        XCTAssertEqual(result!.variable1, expectedRowID)
        XCTAssertEqual(result!.variable2, object.variable2)
    }
    
    func testInsertOrReplace() {
        //Give
        let object = CRUDObject()
        let expectedReplacedRowID = 1
        object.variable1 = expectedReplacedRowID
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.insertOrReplace(objects: object, into: CRUDObject.name))
        //Then
        let result: CRUDObject? = WCDBAssertNoThrowReturned(try database.getObject(from: CRUDObject.name, where: \CRUDObject.variable1 == expectedReplacedRowID))
        XCTAssertNotNil(result)
        XCTAssertEqual(result!.variable2, self.name)
    }
    
    func testHalfInsert() {
        //Give
        let object = CRUDObject()
        object.variable1 = preInsertedObjects.count + 1
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.insert(objects: object, on: [\CRUDObject.variable1], into: CRUDObject.name))
        //Then
        let result: CRUDObject? = WCDBAssertNoThrowReturned(try database.getObject(from: CRUDObject.name, where: \CRUDObject.variable1 == object.variable1))
        XCTAssertNotNil(result)
        XCTAssertEqual(result!.variable2, "")
    }
    
    func testTableInsert() {
        //Give
        let object = CRUDObject()
        object.variable1 = preInsertedObjects.count + 1
        object.variable2 = self.name
        let table = WCDBAssertNoThrowReturned(try database.getTable(named: CRUDObject.name, ofType: CRUDObject.self))
        XCTAssertNotNil(table)        
        //When
        XCTAssertNoThrow(try table!.insert(objects: object))
        //Then
        let result = WCDBAssertNoThrowReturned(try table!.getObject(where: \CRUDObject.variable1 == object.variable1))
        XCTAssertNotNil(result)
        XCTAssertEqual(result!, object)
    }    

    func testSelect() {
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name), whenFailed: [CRUDObject]())
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }
    
    func testConditionalSelect() {
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name, where: \CRUDObject.variable1 == 2), whenFailed: [CRUDObject]())
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, "object2")
    }
    
    func testOrderedSelect() {
        let order = [(\CRUDObject.variable2).asOrder(by: .Descending)]
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name, orderBy: order), whenFailed: [CRUDObject]())
        XCTAssertEqual(results, preInsertedObjects.sorted().reversed())
    }
    
    func testLimitedSelect() {
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name, limit: 1), whenFailed: [CRUDObject]())
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects.sorted()[0])
    }
    
    func testOffsetSelect() {
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name, limit: 1, offset: 1), whenFailed: [CRUDObject]())
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects.sorted()[1])
    }
    
    func testHalfSelect() {
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(on: \CRUDObject.variable2, from: CRUDObject.name), whenFailed: [CRUDObject]())
        XCTAssertEqual(results.map({ $0.variable1 }), Array(repeating: 0, count: preInsertedObjects.count))
        XCTAssertEqual(results.map({ $0.variable2 }), preInsertedObjects.map({ $0.variable2 }))
    }
    
    func testTableSelect() {
        //Give
        let table: Table<CRUDObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: CRUDObject.name))
        XCTAssertNotNil(table)
        //When
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try table!.getObjects(), whenFailed: [CRUDObject]())
        //Then
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }
    
    func testRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(from: CRUDObject.name))
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }
    
    func testConditionalRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(from: CRUDObject.name, where: \CRUDObject.variable1 == 1))
        //Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }
    
    func testOrderedRowSelect() {
        //Give
        let order = [(\CRUDObject.variable1).asOrder(by: .Descending)]
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(from: CRUDObject.name, orderBy: order))
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[1].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[0].variable2)
    }
    
    func testLimitedRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(from: CRUDObject.name, limit: 1))
        //Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testOffsetRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(from: CRUDObject.name, limit: 1, offset: 1))
        //Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[1].variable2)
    }
    
    func testHalfRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(on: \CRUDObject.variable2, from: CRUDObject.name))
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(results[row: 0, column: 0].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(results[row: 1, column: 0].stringValue, preInsertedObjects[1].variable2)
    }
    
    func testTableRowSelect() {
        //Give
        let table: Table<CRUDObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: CRUDObject.name))
        XCTAssertNotNil(table)
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try table!.getRows())
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }

    func testUpdate() {
        //Give
        let object = CRUDObject()
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.update(table: CRUDObject.name, on: \CRUDObject.variable2, with: object))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name), whenFailed: [CRUDObject]())
        XCTAssertEqual(Array(repeating: self.name, count: preInsertedObjects.count), results.map({ $0.variable2 }))
    }
    
    func testConditionalUpdate() {
        //Give
        let object = CRUDObject()
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.update(table: CRUDObject.name, on: \CRUDObject.variable2, with: object, where: \CRUDObject.variable1 == 1))
        //Then
        let result: CRUDObject? = WCDBAssertNoThrowReturned(try database.getObject(from: CRUDObject.name, where: \CRUDObject.variable1 == 1))
        XCTAssertNotNil(result)
        XCTAssertEqual(self.name, result!.variable2)
    }
    
    func testOrderedUpdate() {
        //Give
        let object = CRUDObject()
        object.variable2 = self.name
        let order = [(\CRUDObject.variable1).asOrder(by: .Descending)]
        //When
        XCTAssertNoThrow(try database.update(table: CRUDObject.name, on: \CRUDObject.variable2, with: object, orderBy: order, limit: 1))
        //Then
        let result: CRUDObject? = WCDBAssertNoThrowReturned(try database.getObject(from: CRUDObject.name, where: \CRUDObject.variable1 == 2))
        XCTAssertNotNil(result)
        XCTAssertEqual(self.name, result!.variable2)
    }
    
    func testLimitedUpdate() {
        //Give
        let object = CRUDObject()
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.update(table: CRUDObject.name, on: \CRUDObject.variable2, with: object, limit: 1))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name))
        XCTAssertEqual(results[0].variable2, self.name)
        XCTAssertEqual(results[1], preInsertedObjects[1])
    }

    func testOffsetUpdate() {
        //Give
        let object = CRUDObject()
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.update(table: CRUDObject.name, on: \CRUDObject.variable2, with: object, limit: 1, offset: 1))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name))
        XCTAssertEqual(results[0], preInsertedObjects[0])
        XCTAssertEqual(results[1].variable2, self.name)
    }
    
    func testTableUpdate() {
        //Give
        let object = CRUDObject()
        object.variable2 = self.name
        let table: Table<CRUDObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: CRUDObject.name))
        XCTAssertNotNil(table)
        //When
        XCTAssertNoThrow(try table!.update(on: \CRUDObject.variable2, with: object))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try table!.getObjects())
        XCTAssertEqual(results.map({ $0.variable2 }), Array(repeating: self.name, count: preInsertedObjects.count))
    }

    func testDelete() {
        //When
        XCTAssertNoThrow(try database.delete(from: CRUDObject.name))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name))
        XCTAssertEqual(results.count, 0)
    }
    
    func testConditionalDelete() {
        //When
        XCTAssertNoThrow(try database.delete(from: CRUDObject.name, where: \CRUDObject.variable1 == 2))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }
    
    func testOrderedDelete() {
        //Give
        let order = [(\CRUDObject.variable1).asOrder(by: .Descending)]
        //When
        XCTAssertNoThrow(try database.delete(from: CRUDObject.name, orderBy: order, limit: 1))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name, orderBy: order))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testLimitedDelete() {
        //When
        XCTAssertNoThrow(try database.delete(from: CRUDObject.name, limit: 1))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[1])
    }
    
    func testOffsetDelete() {
        //When
        XCTAssertNoThrow(try database.delete(from: CRUDObject.name, limit: 1, offset: 1))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testTableDelete() {
        //Give
        let table: Table<CRUDObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: CRUDObject.name))
        XCTAssertNotNil(table)
        //When
        XCTAssertNoThrow(try table!.delete())
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: CRUDObject.name))
        XCTAssertEqual(results.count, 0)
    }
}
