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

class AdvanceTests: CRUDTestCase {
    
    func testCoreStatementRead() {
        //Give
        let statement = StatementSelect().select(Column.any).from(CRUDObject.name)
        let optionalCoreStatement = WCDBAssertNoThrowReturned(try database.prepare(statement), whenFailed: nil)
        XCTAssertNotNil(optionalCoreStatement)
        let coreStatement = optionalCoreStatement!
        
        let expectedVariable1: Int32 = 1
        let expectedVariable2 = "object1"
        //When
        XCTAssertNoThrow(try coreStatement.step())
        //Then
        XCTAssertEqual(coreStatement.columnCount(), 2)
        
        XCTAssertEqual(coreStatement.columnName(atIndex: 0), "variable1")
        XCTAssertEqual(coreStatement.columnName(atIndex: 1), "variable2")
        
        XCTAssertEqual(coreStatement.columnTableName(atIndex: 0), CRUDObject.name)
        XCTAssertEqual(coreStatement.columnTableName(atIndex: 1), CRUDObject.name)
        
        XCTAssertEqual(coreStatement.index(byName: "variable1"), 0)
        XCTAssertEqual(coreStatement.index(byName: "variable2"), 1)
        
        XCTAssertEqual(coreStatement.type(atIndex: 0), ColumnType.Integer64)
        XCTAssertEqual(coreStatement.type(atIndex: 1), ColumnType.Text)
        
        do {
            let variable1: Int32 = coreStatement.value(atIndex: 0) 
            XCTAssertEqual(variable1, expectedVariable1)
            let variable2: String = coreStatement.value(atIndex: 1) 
            XCTAssertEqual(variable2, expectedVariable2)
        }
        
        do {
            let variable1: Int32? = coreStatement.value(byName: "variable1")
            XCTAssertNotNil(variable1)
            XCTAssertEqual(variable1!, expectedVariable1)
            let variable2: String? = coreStatement.value(byName: "variable2")
            XCTAssertNotNil(variable2)
            XCTAssertEqual(variable2!, expectedVariable2)
        }
        
        XCTAssertNoThrow(try coreStatement.finalize())
    }
    
    func testCoreStatementWrite() {
        //Give
        let statement = StatementInsert().insert(intoTable: CRUDObject.name).values(Array(repeating: Expression.bindingParameter, count: 2))
        let optionalCoreStatement = WCDBAssertNoThrowReturned(try database.prepare(statement), whenFailed: nil)
        XCTAssertNotNil(optionalCoreStatement)
        let coreStatement = optionalCoreStatement!
        
        let expectedVariable1: Int32 = 3
        let expectedVariable2 = "object3"
        //When
        coreStatement.bind(nil, toIndex: 1)
        coreStatement.bind(expectedVariable2, toIndex: 2)
        XCTAssertNoThrow(try coreStatement.step())
        //Then
        XCTAssertEqual(coreStatement.lastInsertedRowID, 3)
        XCTAssertEqual(coreStatement.changes, 1)
        //When
        XCTAssertNoThrow(try coreStatement.reset())
        let expectedObject = CRUDObject()
        expectedObject.variable1 = 4
        expectedObject.variable2 = "object4"
        coreStatement.bind(\CRUDObject.variable1, of: expectedObject, toIndex: 1)
        coreStatement.bind(\CRUDObject.variable2, of: expectedObject, toIndex: 2)
        XCTAssertNoThrow(try coreStatement.step())
        
        XCTAssertNoThrow(try coreStatement.finalize())
        //Then
        do {
            let result: CRUDObject? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: CRUDObject.name, where: \CRUDObject.variable1 == expectedVariable1))
            XCTAssertNotNil(result)
            XCTAssertEqual(result!.variable2, expectedVariable2)
        }
        do {
            let result: CRUDObject? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: CRUDObject.name, where: \CRUDObject.variable1 == expectedObject.variable1))
            XCTAssertNotNil(result)
            XCTAssertEqual(result!, expectedObject)
        }
    }
    
    func testCipher() {
        //Give
        database.close {
            XCTAssertNoThrow(try self.database.removeFiles())
        }
        let password = "password".data(using: .ascii)! 
        let wrongPassword = "wrongPassword".data(using: .ascii)!
        //When
        database.setCipher(key: password)            
        XCTAssertNoThrow(try database.create(table: CRUDObject.name, of: CRUDObject.self))
        //Then
        database.close()
        database.setCipher(key: wrongPassword)        
        XCTAssertThrowsError(try database.isTableExists(CRUDObject.name))

        database.close()
        database.setCipher(key: nil)            
        XCTAssertThrowsError(try database.isTableExists(CRUDObject.name))

        database.close()
        database.setCipher(key: password)            
        let exists = WCDBAssertNoThrowReturned(try database.isTableExists(CRUDObject.name), whenFailed: nil)
        XCTAssertNotNil(exists)
        XCTAssertTrue(exists!)
    }
    
    func testConfig() {
        //Then
        do {
            let optionalCoreStatement = WCDBAssertNoThrowReturned(try database.prepare(StatementPragma().pragma(.secureDelete)), whenFailed: nil)
            XCTAssertNotNil(optionalCoreStatement)
            let coreStatement = optionalCoreStatement!
            XCTAssertNoThrow(try coreStatement.step())
            let isSecureDelete: Int32 = coreStatement.value(atIndex: 0) 
            XCTAssertEqual(isSecureDelete, 0)
            XCTAssertNoThrow(try coreStatement.finalize())
        }
        //Give
        database.close()
        database.setConfig(named: "SecureDelete") { (handle) throws in
            let statement = StatementPragma().pragma(.secureDelete, to: true)
            try handle.exec(statement)
        }
        //When
        XCTAssertTrue(database.canOpen)
        //Then
        do {
            let optionalCoreStatement = WCDBAssertNoThrowReturned(try database.prepare(StatementPragma().pragma(.secureDelete)), whenFailed: nil)
            XCTAssertNotNil(optionalCoreStatement)
            let coreStatement = optionalCoreStatement!
            XCTAssertNoThrow(try coreStatement.step())
            let isSecureDelete: Int32 = coreStatement.value(atIndex: 0) 
            XCTAssertNotEqual(isSecureDelete, 0)
            XCTAssertNoThrow(try coreStatement.finalize())
        }
    }
    
    func testRedirect() {
        let optionalObject: CRUDObject? = WCDBAssertNoThrowReturned(try database.getObject(on: CRUDObject.any.count().as(\CRUDObject.variable1), fromTable: CRUDObject.name), whenFailed: nil)
        XCTAssertNotNil(optionalObject)
        XCTAssertEqual(optionalObject!.variable1, preInsertedObjects.count)
    }
    
//    func testFTS() {
//        //Give
//        database.close { 
//            XCTAssertNoThrow(try self.database.removeFiles())
//        }
//        database.setTokenizes(.Apple)
//        XCTAssertNoThrow(try database.create(virtualTable: CRUDObject.name, of: CRUDObject.self))
//        XCTAssertNoThrow(try database.insert(objects: preInsertedObjects, intoTable: CRUDObject.name))
//        //When
//        let objects = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: CRUDObject.name, where: (\CRUDObject.variable2).match("object*")), whenFailed: [CRUDObject]())
//        //Then
//        XCTAssertEqual(objects, preInsertedObjects)
//    }
    
}
