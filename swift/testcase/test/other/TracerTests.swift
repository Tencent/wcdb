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

class TracerTests: BaseTestCase {

    func reset() {
        Database.globalTrace(ofPerformance: nil)
        Database.globalTrace(ofSQL: nil)
        Database.globalTrace(ofError: nil)
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
        let database = Database(withFileURL: self.recommendedPath)

        //When
        XCTAssertNoThrow(try database.getRows(on: Master.Properties.any, fromTable: Master.tableName))

        XCTAssertTrue(pass)
    }

    func testTraceError() {
        //Give
        let tableName = "nonexistentTable"
        let expectedTag = self.recommendTag
        let expectedErrorCode = 1
        let expectedErrorMessage = "no such table: \(tableName)"
        let expectedOperation = 3
        let expectedSQL = "SELECT * FROM \(tableName)"

        //Then
        var `catch` = false
        Database.globalTrace { (error: WCDBSwift.Error) in
            if error.type == .sqlite {
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
        let database = Database(withFileURL: self.recommendedPath)
        database.tag = expectedTag

        //When
        XCTAssertThrowsError(
            try database.getRows(on: Column.any, fromTable: tableName)
        )

        XCTAssertTrue(`catch`)
    }

    class TracerObject: TableCodable, Named {
        var variable = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = TracerObject
            case variable
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(isPrimary: true, orderBy: .ascending, isAutoIncrement: true)]
            }
        }
        required init() {}
        var isAutoIncrement: Bool = false
    }

    func testGlobalTracePerformanceCommit() {
        //Give
        let tableName = TracerObject.name
        let expectedTag = self.recommendTag
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
        let database = Database(withFileURL: self.recommendedPath)
        database.close {
            XCTAssertNoThrow(try database.removeFiles())
        }
        database.tag = expectedTag

        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: TracerObject.self))
        let template = TracerObject()
        template.isAutoIncrement = true
        let objects = [TracerObject](repeating: template, count: 1000000)
        XCTAssertNoThrow(try database.insert(objects: objects, intoTable: tableName))
        XCTAssertNoThrow(database.close())

        XCTAssertTrue(`catch`)
    }

    func testTracePerformanceCommit() {
        //Give
        let tableName = TracerObject.name
        let expectedTag = self.recommendTag
        let expectedSQL = "INSERT INTO \(tableName)(variable) VALUES(?)"

        //Give
        let database = Database(withFileURL: self.recommendedPath)
        database.close {
            XCTAssertNoThrow(try database.removeFiles())
        }
        database.tag = expectedTag

        //Then
        var `catch` = false
        database.trace { (tag, sqls, cost) in
            if tag != nil && tag! == expectedTag && sqls.contains(where: { (arg) -> Bool in
                return arg.key == expectedSQL
            }) {
                XCTAssertGreaterThan(cost, 0)
                `catch` = true
            }
        }

        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: TracerObject.self))
        let template = TracerObject()
        template.isAutoIncrement = true
        let objects = [TracerObject](repeating: template, count: 1000000)
        XCTAssertNoThrow(try database.insert(objects: objects, intoTable: tableName))
        XCTAssertNoThrow(database.close())

        XCTAssertTrue(`catch`)
    }

    func testTraceRollback() {
        //Give
        let tableName = TracerObject.name
        let expectedTag = self.recommendTag
        let expectedSQL = "INSERT INTO \(tableName)(variable) VALUES(?)"
        let expectedRollback = "ROLLBACK"

        //Then
        var `catch` = false
        Database.globalTrace { (tag, sqls, _) in
            if tag != nil && tag! == expectedTag && sqls.contains(where: { (arg) -> Bool in
                return arg.key == expectedSQL
            }) {
                XCTAssertTrue(sqls.contains(where: { (arg) -> Bool in
                    return arg.key == expectedRollback
                }))
                `catch` = true
            }
        }

        //Give
        let database = Database(withFileURL: self.recommendedPath)
        database.close {
            XCTAssertNoThrow(try database.removeFiles())
        }
        database.tag = expectedTag

        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: TracerObject.self))
        let template = TracerObject()
        template.isAutoIncrement = true
        let objects = [TracerObject](repeating: template, count: 1000000)
        XCTAssertNoThrow(try database.run { () -> Bool in
            try database.insert(objects: objects, intoTable: tableName)
            return false
        })
        XCTAssertNoThrow(database.close())

        XCTAssertTrue(`catch`)
    }
}
