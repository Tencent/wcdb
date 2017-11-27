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

class ConvenienceInterfaceTests: WCDBTestCase {

    class ConvenienceObject: WCDB.CodableTable, Named, Equatable, Comparable {        
        var variable1: Int = 0
        var variable2: String = ""
        
        required init() {}        
        var isAutoIncrement: Bool = false
        var lastInsertedRowID: Int64 = 0
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\ConvenienceObject.variable1, alias: "variable1", isPrimary: true, orderBy: .Ascending, isAutoIncrement: true),
                ColumnBinding(\ConvenienceObject.variable2, alias: "variable2")
            ]
        }
        
        static func ==(lhs: ConvenienceObject, rhs: ConvenienceObject) -> Bool {
            return lhs.variable1==rhs.variable1 && lhs.variable2==rhs.variable2
        }
        
        static func <(lhs: ConvenienceObject, rhs: ConvenienceObject) -> Bool {
            guard lhs.variable1 != rhs.variable1 else {
                return lhs.variable2 < lhs.variable2
            }
            return lhs.variable1 < rhs.variable1
        }
    }
    
    var database: Database!
    lazy var preInsertedObjects: [ConvenienceObject] = {
        let object1 = ConvenienceObject()
        object1.variable1 = 1
        object1.variable2 = "object1"
        let object2 = ConvenienceObject()
        object2.variable1 = 2
        object2.variable2 = "object2"
        return [object1, object2]
    }()
    
    override func setUp() {
        super.setUp()
        database = Database(withFileURL: self.recommendedPath)
        
        XCTAssertNoThrow(try database.create(table: ConvenienceObject.name, of: ConvenienceObject.self))
        
        XCTAssertNoThrow(try database.insert(objects: preInsertedObjects, into: ConvenienceObject.name))
    }
    
    func testInsert() {
        //Give
        let object = ConvenienceObject()
        object.variable1 = preInsertedObjects.count + 1
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.insert(objects: object, into: ConvenienceObject.name))
        //Then
        let result: ConvenienceObject? = WCDBAssertNoThrowReturned(try database.getObject(from: ConvenienceObject.name, where: \ConvenienceObject.variable1 == object.variable1))
        XCTAssertNotNil(result)
        XCTAssertEqual(result!, object)
    }
        
    func testAutoIncrementInsert() {
        //Give
        let object = ConvenienceObject()
        let expectedRowID = preInsertedObjects.count + 1 
        object.isAutoIncrement = true
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.insert(objects: object, into: ConvenienceObject.name))
        //Then
        XCTAssertEqual(object.lastInsertedRowID, Int64(expectedRowID))
        let result: ConvenienceObject? = WCDBAssertNoThrowReturned(try database.getObject(from: ConvenienceObject.name, where: \ConvenienceObject.variable1 == expectedRowID))
        XCTAssertNotNil(result)
        XCTAssertEqual(result!.variable1, expectedRowID)
        XCTAssertEqual(result!.variable2, object.variable2)
    }
    
    func testInsertOrReplace() {
        //Give
        let object = ConvenienceObject()
        let expectedReplacedRowID = 1
        object.variable1 = expectedReplacedRowID
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.insertOrReplace(objects: object, into: ConvenienceObject.name))
        //Then
        let result: ConvenienceObject? = WCDBAssertNoThrowReturned(try database.getObject(from: ConvenienceObject.name, where: \ConvenienceObject.variable1 == expectedReplacedRowID))
        XCTAssertNotNil(result)
        XCTAssertEqual(result!.variable2, self.name)
    }
    
    func testHalfInsert() {
        //Give
        let object = ConvenienceObject()
        object.variable1 = preInsertedObjects.count + 1
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.insert(objects: object, on: [\ConvenienceObject.variable1], into: ConvenienceObject.name))
        //Then
        let result: ConvenienceObject? = WCDBAssertNoThrowReturned(try database.getObject(from: ConvenienceObject.name, where: \ConvenienceObject.variable1 == object.variable1))
        XCTAssertNotNil(result)
        XCTAssertEqual(result!.variable2, "")
    }
    
    func testTableInsert() {
        //Give
        let object = ConvenienceObject()
        object.variable1 = preInsertedObjects.count + 1
        object.variable2 = self.name
        let table = WCDBAssertNoThrowReturned(try database.getTable(named: ConvenienceObject.name, ofType: ConvenienceObject.self))
        XCTAssertNotNil(table)        
        //When
        XCTAssertNoThrow(try table!.insert(objects: object))
        //Then
        let result = WCDBAssertNoThrowReturned(try table!.getObject(where: \ConvenienceObject.variable1 == object.variable1))
        XCTAssertNotNil(result)
        XCTAssertEqual(result!, object)
    }    

    func testSelect() {
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name), whenFailed: [ConvenienceObject]())
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }
    
    func testConditionalSelect() {
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name, where: \ConvenienceObject.variable1 == 2), whenFailed: [ConvenienceObject]())
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, "object2")
    }
    
    func testOrderedSelect() {
        let order = [(\ConvenienceObject.variable2).asOrder(by: .Descending)]
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name, orderBy: order), whenFailed: [ConvenienceObject]())
        XCTAssertEqual(results, preInsertedObjects.sorted().reversed())
    }
    
    func testLimitedSelect() {
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name, limit: 1), whenFailed: [ConvenienceObject]())
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects.sorted()[0])
    }
    
    func testOffsetSelect() {
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name, limit: 1, offset: 1), whenFailed: [ConvenienceObject]())
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects.sorted()[1])
    }
    
    func testHalfSelect() {
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(on: \ConvenienceObject.variable2, from: ConvenienceObject.name), whenFailed: [ConvenienceObject]())
        XCTAssertEqual(results.map({ $0.variable1 }), Array(repeating: 0, count: preInsertedObjects.count))
        XCTAssertEqual(results.map({ $0.variable2 }), preInsertedObjects.map({ $0.variable2 }))
    }
    
    func testTableSelect() {
        //Give
        let table: Table<ConvenienceObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: ConvenienceObject.name))
        XCTAssertNotNil(table)
        //When
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try table!.getObjects(), whenFailed: [ConvenienceObject]())
        //Then
        XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
    }
    
    func testRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(from: ConvenienceObject.name))
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[1].variable2)
    }
    
    func testConditionalRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(from: ConvenienceObject.name, where: \ConvenienceObject.variable1 == 1))
        //Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }
    
    func testOrderedRowSelect() {
        //Give
        let order = [(\ConvenienceObject.variable1).asOrder(by: .Descending)]
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(from: ConvenienceObject.name, orderBy: order))
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[1].variable2)
        XCTAssertEqual(Int(results[row: 1, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 1, column: 1].stringValue, preInsertedObjects[0].variable2)
    }
    
    func testLimitedRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(from: ConvenienceObject.name, limit: 1))
        //Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[0].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[0].variable2)
    }

    func testOffsetRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(from: ConvenienceObject.name, limit: 1, offset: 1))
        //Then
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(Int(results[row: 0, column: 0].int64Value), preInsertedObjects[1].variable1)
        XCTAssertEqual(results[row: 0, column: 1].stringValue, preInsertedObjects[1].variable2)
    }
    
    func testHalfRowSelect() {
        //When
        let results: FundamentalRowXColumn = WCDBAssertNoThrowReturned(try database.getRows(on: \ConvenienceObject.variable2, from: ConvenienceObject.name))
        //Then
        XCTAssertEqual(results.count, preInsertedObjects.count)
        XCTAssertEqual(results[row: 0, column: 0].stringValue, preInsertedObjects[0].variable2)
        XCTAssertEqual(results[row: 1, column: 0].stringValue, preInsertedObjects[1].variable2)
    }
    
    func testTableRowSelect() {
        //Give
        let table: Table<ConvenienceObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: ConvenienceObject.name))
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
        let object = ConvenienceObject()
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.update(table: ConvenienceObject.name, on: \ConvenienceObject.variable2, with: object))
        //Then
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name), whenFailed: [ConvenienceObject]())
        XCTAssertEqual(Array(repeating: self.name, count: preInsertedObjects.count), results.map({ $0.variable2 }))
    }
    
    func testConditionalUpdate() {
        //Give
        let object = ConvenienceObject()
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.update(table: ConvenienceObject.name, on: \ConvenienceObject.variable2, with: object, where: \ConvenienceObject.variable1 == 1))
        //Then
        let result: ConvenienceObject? = WCDBAssertNoThrowReturned(try database.getObject(from: ConvenienceObject.name, where: \ConvenienceObject.variable1 == 1))
        XCTAssertNotNil(result)
        XCTAssertEqual(self.name, result!.variable2)
    }
    
    func testOrderedUpdate() {
        //Give
        let object = ConvenienceObject()
        object.variable2 = self.name
        let order = [(\ConvenienceObject.variable1).asOrder(by: .Descending)]
        //When
        XCTAssertNoThrow(try database.update(table: ConvenienceObject.name, on: \ConvenienceObject.variable2, with: object, orderBy: order, limit: 1))
        //Then
        let result: ConvenienceObject? = WCDBAssertNoThrowReturned(try database.getObject(from: ConvenienceObject.name, where: \ConvenienceObject.variable1 == 2))
        XCTAssertNotNil(result)
        XCTAssertEqual(self.name, result!.variable2)
    }
    
    func testLimitedUpdate() {
        //Give
        let object = ConvenienceObject()
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.update(table: ConvenienceObject.name, on: \ConvenienceObject.variable2, with: object, limit: 1))
        //Then
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name))
        XCTAssertEqual(results[0].variable2, self.name)
        XCTAssertEqual(results[1], preInsertedObjects[1])
    }

    func testOffsetUpdate() {
        //Give
        let object = ConvenienceObject()
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.update(table: ConvenienceObject.name, on: \ConvenienceObject.variable2, with: object, limit: 1, offset: 1))
        //Then
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name))
        XCTAssertEqual(results[0], preInsertedObjects[0])
        XCTAssertEqual(results[1].variable2, self.name)
    }
    
    func testTableUpdate() {
        //Give
        let object = ConvenienceObject()
        object.variable2 = self.name
        let table: Table<ConvenienceObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: ConvenienceObject.name))
        XCTAssertNotNil(table)
        //When
        XCTAssertNoThrow(try table!.update(on: \ConvenienceObject.variable2, with: object))
        //Then
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try table!.getObjects())
        XCTAssertEqual(results.map({ $0.variable2 }), Array(repeating: self.name, count: preInsertedObjects.count))
    }

    func testDelete() {
        //When
        XCTAssertNoThrow(try database.delete(from: ConvenienceObject.name))
        //Then
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name))
        XCTAssertEqual(results.count, 0)
    }
    
    func testConditionalDelete() {
        //When
        XCTAssertNoThrow(try database.delete(from: ConvenienceObject.name, where: \ConvenienceObject.variable1 == 2))
        //Then
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }
    
    func testOrderedDelete() {
        //Give
        let order = [(\ConvenienceObject.variable1).asOrder(by: .Descending)]
        //When
        XCTAssertNoThrow(try database.delete(from: ConvenienceObject.name, orderBy: order, limit: 1))
        //Then
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name, orderBy: order))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testLimitedDelete() {
        //When
        XCTAssertNoThrow(try database.delete(from: ConvenienceObject.name, limit: 1))
        //Then
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[1])
    }
    
    func testOffsetDelete() {
        //When
        XCTAssertNoThrow(try database.delete(from: ConvenienceObject.name, limit: 1, offset: 1))
        //Then
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name))
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0], preInsertedObjects[0])
    }

    func testTableDelete() {
        //Give
        let table: Table<ConvenienceObject>? = WCDBAssertNoThrowReturned(try database.getTable(named: ConvenienceObject.name))
        XCTAssertNotNil(table)
        //When
        XCTAssertNoThrow(try table!.delete())
        //Then
        let results: [ConvenienceObject] = WCDBAssertNoThrowReturned(try database.getObjects(from: ConvenienceObject.name))
        XCTAssertEqual(results.count, 0)
    }
}
