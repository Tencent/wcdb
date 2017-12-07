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

class RepairTests: BaseTestCase {
    
    class RepairObject: WCDB.TableCodable, Named, Equatable, Comparable {
        var variable1: Int = 0
        var variable2: String = ""
        required init() {}   
        enum CodingKeys: String, CodingTableKey {
            typealias Root = RepairObject
            case variable1
            case variable2
        }
        var isAutoIncrement: Bool = false
        var lastInsertedRowID: Int64 = 0
        
        static func ==(lhs: RepairObject, rhs: RepairObject) -> Bool {
            return lhs.variable1 == rhs.variable1 && lhs.variable2 == rhs.variable2
        }
        
        static func <(lhs: RepairObject, rhs: RepairObject) -> Bool {
            guard lhs.variable1 == rhs.variable1 else {
                return lhs.variable1 < rhs.variable1  
            }
            return lhs.variable2 < rhs.variable2
        }
    }

    var database: Database!
    var pageSize: Int32!
    let preInsertedObjects: [RepairObject] = {
        let object1 = RepairObject()
        object1.variable1 = 1
        object1.variable2 = "object1"
        let object2 = RepairObject()
        object2.variable1 = 2
        object2.variable2 = "object2"
        return [object1, object2]
    }()
    
    override func setUp() {
        super.setUp()
        database = Database(withFileURL: self.recommendedPath)
        
        let optionalCoreStatement = WCDBAssertNoThrowReturned(try database.prepare(StatementPragma().pragma(.pageSize)), whenFailed: nil) 
        XCTAssertNotNil(optionalCoreStatement)
        let coreStatement = optionalCoreStatement!
        
        XCTAssertNoThrow(try coreStatement.step())
        
        pageSize = coreStatement.value(atIndex: 0)
        
        XCTAssertEqual(pageSize >> 1 & pageSize, 0)
        
        XCTAssertGreaterThan(pageSize, 0)
        
        XCTAssertNoThrow(try database.create(table: RepairObject.name, of: RepairObject.self))
        
        XCTAssertNoThrow(try database.insert(objects: preInsertedObjects, intoTable: RepairObject.name))
    }
    
    func corrupt(database: Database) {
        database.close {
            let size = Int(self.pageSize)
            let data = Data(count: size)
            let fd = open(database.path, O_RDWR)
            _ = data.withUnsafeBytes({ (bytes) in
                write(fd, bytes, size)
            })
            close(fd)
        }
    }
    
    func testRepair() {
        //Give
        XCTAssertNoThrow(try database.backup())
        //Then
        do {
            let results: [RepairObject] = WCDBAssertNoThrowReturned(try database.getObjects(fromTable: RepairObject.name), whenFailed: [RepairObject]()) 
            XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
        }
        //When
        corrupt(database: database)
        //Then
        do {
            XCTAssertThrowsError(try database.getObjects(fromTable: RepairObject.name) as [RepairObject])
        }
        //When
        let recovered = Database(withPath: database.path+".recovered")
        database.close { 
            XCTAssertNoThrow(try recovered.recover(fromPath: self.database.path, withPageSize: self.pageSize))
        }
        //Then
        do {
            let results: [RepairObject] = WCDBAssertNoThrowReturned(try recovered.getObjects(fromTable: RepairObject.name), whenFailed: [RepairObject]()) 
            XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
        }
    }
    
    func testRepairWithCipher() {
        //Give
        let backupCipher = "backupCipher".data(using: .ascii)!
        let fakeBackupCipher = "fakeBackupCipher".data(using: .ascii)!
        XCTAssertNoThrow(try database.backup(withKey: backupCipher))
        //When
        let recovered = Database(withPath: database.path+".recovered")
        database.close { 
            XCTAssertThrowsError(try recovered.recover(fromPath: self.database.path, withPageSize: self.pageSize, backupKey: fakeBackupCipher))
        }
        //Then
        do {
            XCTAssertThrowsError(try recovered.getObjects(fromTable: RepairObject.name) as [RepairObject])
        }
        //When
        database.close {
            XCTAssertNoThrow(try recovered.recover(fromPath: self.database.path, withPageSize: self.pageSize, backupKey: backupCipher))
        }
        //Then
        do {
            let results: [RepairObject] = WCDBAssertNoThrowReturned(try recovered.getObjects(fromTable: RepairObject.name), whenFailed: [RepairObject]()) 
            XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
        }
    }

    func testRepairWithCipherDatabase() {
        //Give
        let databaseCipher = "databaseCipher".data(using: .ascii)!
        let fakeDatabaseCipher = "fakeDatabaseCipher".data(using: .ascii)!
        database.close { 
            XCTAssertNoThrow(try self.database.removeFiles())
        }
        database.setCipher(key: databaseCipher)
        
        do {
            let optionalCoreStatement = WCDBAssertNoThrowReturned(try database.prepare(StatementPragma().pragma(.pageSize)), whenFailed: nil) 
            XCTAssertNotNil(optionalCoreStatement)
            let coreStatement = optionalCoreStatement!
            
            XCTAssertNoThrow(try coreStatement.step())
            
            pageSize = coreStatement.value(atIndex: 0)
            
            XCTAssertEqual(pageSize >> 1 & pageSize, 0)
            
            XCTAssertGreaterThan(pageSize, 0)
        }
        
        do {
            XCTAssertNoThrow(try database.create(table: RepairObject.name, of: RepairObject.self))
            XCTAssertNoThrow(try database.insert(objects: preInsertedObjects, intoTable: RepairObject.name))
        }

        XCTAssertNoThrow(try database.backup())
        //When
        let recovered = Database(withPath: database.path+".recovered")
        database.close { 
            XCTAssertNoThrow(try recovered.recover(fromPath: self.database.path, withPageSize: self.pageSize, databaseKey: fakeDatabaseCipher))
        }
        //Then
        do {
            let results = WCDBAssertNoThrowReturned(try recovered.getObjects(fromTable: RepairObject.name), whenFailed: [RepairObject]())
            XCTAssertNotEqual(results.sorted(), preInsertedObjects.sorted())
        }
        //When
        database.close { 
            XCTAssertNoThrow(try recovered.recover(fromPath: self.database.path, withPageSize: self.pageSize, databaseKey: databaseCipher))
        }
        //Then
        do {
            let results: [RepairObject] = WCDBAssertNoThrowReturned(try recovered.getObjects(fromTable: RepairObject.name), whenFailed: [RepairObject]()) 
            XCTAssertEqual(results.sorted(), preInsertedObjects.sorted())
        }
    }

}
