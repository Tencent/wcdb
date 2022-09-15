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

class TracerTests: BaseTestCase {

    func reset() {
        Database.globalTrace(ofPerformance: nil)
        Database.globalTrace(ofSQL: nil)
        Database.globalTrace(ofError: nil)
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
        // Give
        let expectedSQL = "SELECT * FROM main.sqlite_master"

        // Then
        var pass = false
        Database.globalTrace { (_, _, sql) in
            if sql == expectedSQL {
                pass = true
            }
        }

        // Give
        let database = Database(at: self.recommendedPath)

        // When
        XCTAssertNoThrow(try database.getRows(on: Master.Properties.any, fromTable: Master.builtinTableName))

        XCTAssertTrue(pass)
    }

    func testTraceError() {
        // Give
        let tableName = "nonexistentTable"
        let expectedTag = self.recommendTag
        let expectedErrorCode = 1
        let expectedErrorMessage = "no such table: main.\(tableName)"
        let expectedSQL = "SELECT * FROM main.\(tableName)"
        let expectedPath = self.recommendedPath

        // Then
        var `catch` = false
        Database.globalTrace { (error: WCDB.Error) in
            let tag = error.tag
            XCTAssertNotNil(tag)
            XCTAssertEqual(tag!, expectedTag)

            let code = error.code
            XCTAssertEqual(code.rawValue, expectedErrorCode)

            let message = error.message
            XCTAssertNotNil(message)
            XCTAssertEqual(message!, expectedErrorMessage)

            let sql = error.sql
            XCTAssertNotNil(sql)
            XCTAssertEqual(sql!, expectedSQL)

            let path = error.path
            XCTAssertNotNil(path)
            XCTAssertEqual(path!, expectedPath.path)

            XCTAssertNil(error.extendedCode)

            let wrongStringType = error.infos[.tag]?.stringValue
            XCTAssertNotNil(wrongStringType)
            XCTAssertEqual(wrongStringType, String(expectedTag))

            let wrongIntType = error.infos[.path]?.intValue
            XCTAssertNotNil(wrongIntType)
            XCTAssertEqual(wrongIntType!, 0)

            `catch` = true
            print(error)
        }

        // Give
        let database = Database(at: expectedPath)
        database.tag = expectedTag

        // When
        XCTAssertThrowsError(
            try database.getRows(on: Column.all, fromTable: tableName)
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
        var isAutoIncrement: Bool = false
    }

    func testGlobalTracePerformanceCommit() {
        // Give
        let tableName = TracerObject.name
        let expectedPath = self.recommendedPath.path
        let expectedSQL = "INSERT INTO main.\(tableName)(variable) VALUES(?1)"

        // Then
        var `catch` = false
        Database.globalTrace { (path, _, sql, cost) in
            if path == expectedPath && sql == expectedSQL && cost > 0 {
                `catch` = true
            }
        }

        // Give
        let database = Database(at: self.recommendedPath)
        XCTAssertNoThrow(try database.close {
            try database.removeFiles()
        })
        database.tag = self.recommendTag

        // When
        XCTAssertNoThrow(try database.create(table: tableName, of: TracerObject.self))
        let template = TracerObject()
        template.isAutoIncrement = true
        let objects = [TracerObject](repeating: template, count: 100000)
        XCTAssertNoThrow(try database.insert(objects, intoTable: tableName))
        XCTAssertNoThrow(database.close())

        XCTAssertTrue(`catch`)
    }

    func testTraceRollback() {
        // Give
        let tableName = TracerObject.name
        let expectedPath = self.recommendedPath.path
        let expectedSQL = "INSERT INTO main.\(tableName)(variable) VALUES(?1)"
        let expectedRollback = "ROLLBACK"

        // Then
        var catchInsert = false
        var catchRollback = false
        Database.globalTrace { (path, _, sql, _) in
            if path == expectedPath && sql == expectedSQL {
                catchInsert = true
            } else if path == expectedPath && sql == expectedRollback {
                catchRollback = true
            }
        }

        // Give
        let database = Database(at: self.recommendedPath)
        XCTAssertNoThrow(try database.close {
            try database.removeFiles()
        })
        database.tag = self.recommendTag

        // When
        XCTAssertNoThrow(try database.create(table: tableName, of: TracerObject.self))
        let template = TracerObject()
        template.isAutoIncrement = true
        let objects = [TracerObject](repeating: template, count: 100000)
        XCTAssertNoThrow(try database.run { (_) -> Bool in
            try database.insert(objects, intoTable: tableName)
            return false
        })
        XCTAssertNoThrow(database.close())

        XCTAssertTrue(catchInsert && catchRollback)
    }
}
