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

class TracerTests: WCDBTestCase {
    
    func reset() {
        Database.globalTrace(ofPerformance: nil)
        Database.globalTrace(ofSQL: nil)
        Database.resetGlobalTraceOfError()
    }

    override func setUp() {
        super.setUp()
        reset()
    }
    
    override func tearDown() {
        reset()
        super.tearDown()
    }
    
    func testTraceSQL() {
        //Give 
        let expectedSQL = "SELECT * FROM sqlite_master"
        
        //Then
        var pass = false
        Database.globalTrace { (sql: String) in
            if sql == expectedSQL {
                pass = true
            } 
        }

        //Give
        let database = Database(withPath: self.recommendedPath)

        //When
        XCTAssertNoThrow(try database.getRows(on: Master.any, from: Master.tableName))

        XCTAssertTrue(pass)
    }
    
    func testTraceError() {
        //Give
        let expectedTag = 1234
        let expectedErrorCode = 1
        let expectedErrorMessage = "no such table: \(WCDB.Sequence.tableName)"
        let expectedOperation = 3
        let expectedSQL = "SELECT * FROM sqlite_sequence"
        
        //Then
        var `catch` = false
        Database.globalTrace { (error: WCDB.Error) in
            if error.type == .SQLite {
                let tag = error.tag
                XCTAssertNotNil(tag)
                XCTAssertEqual(tag!, expectedTag)
                
                let code = error.code
                XCTAssertEqual(code.value, expectedErrorCode)
                
                let message = error.message
                XCTAssertNotNil(message)
                XCTAssertEqual(message!, expectedErrorMessage)
                
                let operation = error.operationValue
                XCTAssertNotNil(operation)
                XCTAssertEqual(operation!, expectedOperation)
                
                let sql = error.sql
                XCTAssertNotNil(sql)
                XCTAssertEqual(sql!, expectedSQL)
                
                `catch` = true
            }
            print(error)
        }
        
        //Give
        let database = Database(withPath: self.recommendedPath)
        database.tag = expectedTag
        
        //When
        XCTAssertThrowsError(try database.getRows(on: WCDB.Sequence.any, from: WCDB.Sequence.tableName))
        
        XCTAssertTrue(`catch`)
    }
    
    class TracerObject: CodableTable, Named {
        var variable = 0
        required init() {}        
        var isAutoIncrement: Bool = false
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\TracerObject.variable, alias: "variable", isPrimary: true, orderBy: .Ascending, isAutoIncrement: true),
            ]
        }
    }
    
    func testTracePerformance() {
        //Give
        let tableName = TracerObject.name
        let expectedTag = 12352345
        let expectedSQL = "INSERT INTO \(tableName)(variable) VALUES(?)"
        
        //Then
        var `catch` = false
        Database.globalTrace { (tag, sqls, cost) in
            if tag != nil && tag! == expectedTag && sqls.contains(where: { (arg) -> Bool in
                return arg.key == expectedSQL
            }) {
                XCTAssertGreaterThan(cost, 0)
                `catch` = true
            }
        }
        
        //Give
        let database = Database(withPath: self.recommendedPath)
        database.close { 
            XCTAssertNoThrow(try database.removeFiles())
        }
        database.tag = expectedTag
        
        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: TracerObject.self))
        let template = TracerObject()
        template.isAutoIncrement = true
        let objects = Array<TracerObject>(repeating: template, count: 100000)
        XCTAssertNoThrow(try database.insert(objects: objects, into: tableName))
        XCTAssertNoThrow(database.close())
        
        XCTAssertTrue(`catch`)
    }
}
