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

class TracerTests: DatabaseTestCase, @unchecked Sendable {

    func reset() {
        Database.globalTracePerformance(nil)
        Database.globalTraceSQL(nil)
        Database.globalTraceError(nil)
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
        let expectedSQL = "SELECT * FROM sqlite_master"

        // Then
        var pass = false
        Database.globalTraceSQL { (tag, path, _, sql, _) in
            guard path == self.database.path else {
                return
            }
            XCTAssertEqual(tag, self.database.tag)
            if sql == expectedSQL {
                pass = true
            }
        }

        // When
        XCTAssertNoThrow(try database.getRows(on: Master.Properties.any, fromTable: Master.builtinTableName))

        XCTAssertTrue(pass)
    }

    func testGlobalTraceError() {
        // Give
        let tableName = "nonexistentTable"
        let expectedTag = self.database.tag
        let expectedErrorCode = 1
        let expectedErrorMessage = "no such table: \(tableName)"
        let expectedSQL = "SELECT * FROM \(tableName)"
        let expectedPath = self.database.path

        // Then
        var `catch` = false
        Database.globalTraceError { (error: WCDBError) in
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
            XCTAssertEqual(path!, expectedPath)

            XCTAssertNil(error.extendedCode)

            let wrongStringType = error.infos[.tag]?.stringValue
            XCTAssertNotNil(wrongStringType)
            XCTAssertEqual(wrongStringType, String(expectedTag!))

            let wrongIntType = error.infos[.path]?.intValue
            XCTAssertNotNil(wrongIntType)
            XCTAssertEqual(wrongIntType!, 0)

            `catch` = true
            print(error)
        }

        // When
        XCTAssertThrowsError(
            try database.getRows(on: Column.all(), fromTable: tableName)
        )

        XCTAssertTrue(`catch`)
    }

    func testTraceError() {
        // Give
        let tableName = "nonexistentTable"
        let expectedTag = self.database.tag
        let expectedErrorCode = 1
        let expectedErrorMessage = "no such table: \(tableName)"
        let expectedSQL = "SELECT * FROM \(tableName)"
        let expectedPath = self.database.path

        // Then
        var `catch` = false
        database.traceError { (error: WCDBError) in
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
            XCTAssertEqual(path!, expectedPath)

            XCTAssertNil(error.extendedCode)

            let wrongStringType = error.infos[.tag]?.stringValue
            XCTAssertNotNil(wrongStringType)
            XCTAssertEqual(wrongStringType, String(expectedTag!))

            let wrongIntType = error.infos[.path]?.intValue
            XCTAssertNotNil(wrongIntType)
            XCTAssertEqual(wrongIntType!, 0)

            `catch` = true
            print(error)
        }

        // When
        XCTAssertThrowsError(
            try database.getRows(on: Column.all(), fromTable: tableName)
        )

        XCTAssertTrue(`catch`)
    }

    final class TracerObject: TableCodable, Named {
        var variable = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = TracerObject
            case variable
            nonisolated(unsafe) static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, isPrimary: true, orderBy: .ascending, isAutoIncrement: true)
            }
        }
        var isAutoIncrement: Bool = false
    }

    func testGlobalTracePerformance() {
        // Give
        let tableName = TracerObject.name
        let expectedTag = self.database.tag
        var objects: [TestObject] = []
        for _ in (0..<1000) {
            let object = TestObject()
            object.isAutoIncrement = true
            object.variable2 = Random.string(withLength: 4096)
            objects.append(object)
        }
        var testCount = 0
        var lastSQLIsInsert = false
        // Then
        Database.globalTracePerformance { (tag, path, _, sql, info) in
            guard path == self.database.path else {
                return
            }
            XCTAssertEqual(tag, expectedTag)
            if sql.hasPrefix("COMMIT") && lastSQLIsInsert {
                XCTAssertTrue(info.costInNanoseconds > 0)
                XCTAssertTrue(info.tablePageWriteCount > 0)
                XCTAssertTrue(info.indexPageWriteCount == 0)
                XCTAssertTrue(info.overflowPageWriteCount > 0)
                XCTAssertTrue(info.tablePageReadCount == 0)
                XCTAssertTrue(info.indexPageReadCount == 0)
                XCTAssertTrue(info.overflowPageReadCount == 0)
                testCount += 1
            } else if sql.hasPrefix("CREATE INDEX") {
                XCTAssertTrue(info.costInNanoseconds > 0)
                XCTAssertTrue(info.tablePageWriteCount == 1)
                XCTAssertTrue(info.indexPageWriteCount > 0)
                XCTAssertTrue(info.overflowPageWriteCount == objects.count)
                XCTAssertTrue(info.tablePageReadCount > 0)
                XCTAssertTrue(info.indexPageReadCount >= 0)
                XCTAssertTrue(info.overflowPageReadCount == objects.count)
                testCount += 1
            } else if sql.hasPrefix("SELECT") {
                XCTAssertTrue(info.costInNanoseconds > 0)
                XCTAssertTrue(info.tablePageWriteCount == 0)
                XCTAssertTrue(info.indexPageWriteCount == 0)
                XCTAssertTrue(info.overflowPageWriteCount == 0)
                testCount += 1
                if sql.hasSuffix("ORDER BY variable2") {
                    XCTAssertTrue(info.tablePageReadCount == 0)
                    XCTAssertTrue(info.indexPageReadCount > 0)
                    XCTAssertTrue(info.overflowPageReadCount == objects.count)
                } else {
                    XCTAssertTrue(info.tablePageReadCount > 0)
                    XCTAssertTrue(info.indexPageReadCount == 0)
                    XCTAssertTrue(info.overflowPageReadCount == objects.count)
                }
            }
            lastSQLIsInsert = sql.hasPrefix("INSERT")
        }

        // Give
        let database = Database(at: self.recommendedPath)
        XCTAssertNoThrow(try database.close {
            try database.removeFiles()
        })
        database.tag = self.recommendTag

        // When
        XCTAssertNoThrow(try database.create(table: tableName, of: TestObject.self))
        XCTAssertNoThrow(try database.insert(objects, intoTable: tableName))
        XCTAssertNoThrow(try database.create(index: "testIndex", with: TestObject.Properties.variable2, forTable: tableName))
        let result1: [TestObject] = WCDBAssertNoThrowReturned(try self.database.getObjects(fromTable: tableName))
        XCTAssertTrue(result1.count == objects.count)
        let result2: [TestObject] = WCDBAssertNoThrowReturned(try self.database.getObjects(fromTable: tableName, orderBy: [TestObject.Properties.variable2]))
        XCTAssertTrue(result2.count == objects.count)
        XCTAssertNoThrow(database.close())
        XCTAssertTrue(testCount == 4)
    }

    func testTraceRollback() {
        // Give
        let tableName = TracerObject.name
        let expectedTag = self.database.tag
        let expectedSQL = "INSERT INTO \(tableName)(variable) VALUES(?1)"
        let expectedRollback = "ROLLBACK"

        // Then
        var catchInsert = false
        var catchRollback = false
        Database.globalTraceSQL { (tag, path, _, sql, _) in
            guard path == self.database.path else {
                return
            }
            XCTAssertEqual(tag, expectedTag)
            if sql == expectedSQL {
                catchInsert = true
            } else if sql == expectedRollback {
                catchRollback = true
            }
        }

        // Give
        XCTAssertNoThrow(try database.close {
            try self.database.removeFiles()
        })

        // When
        XCTAssertNoThrow(try database.create(table: tableName, of: TracerObject.self))
        let template = TracerObject()
        template.isAutoIncrement = true
        let objects = [TracerObject](repeating: template, count: 100000)
        XCTAssertNoThrow(try database.run(controllableTransaction: { (_) -> Bool in
            try self.database.insert(objects, intoTable: tableName)
            return false
        }))
        XCTAssertNoThrow(database.close())

        XCTAssertTrue(catchInsert && catchRollback)
    }

    func testTraceDatabaseOperation() {
        var tag = 0
        var path = ""
        var openCount = 0
        var tableCount = 0
        var indexCount = 0
        Database.globalTraceDatabaseOperation {
            (database, operation, info) in
            switch operation {
            case .Create:
                path = database.path
            case .SetTag:
                tag = database.tag ?? 0
            case .OpenHandle:
                openCount += 1
                XCTAssertEqual(info[Database.OperationInfoKeyHandleCount]?.intValue, 1)
                XCTAssertTrue(info[Database.OperationInfoKeyHandleOpenTime]?.intValue ?? 0 > 0)
                XCTAssertTrue(info[Database.OperationInfoKeyHandleOpenCPUTime]?.intValue ?? 0 > 0)
                XCTAssertTrue(info[Database.OperationInfoKeySchemaUsage]?.intValue ?? 0 > 0)
                XCTAssertEqual(info[Database.OperationInfoKeyTriggerCount]?.intValue, 0)
                tableCount = info[Database.OperationInfoKeyTableCount]?.intValue ?? 0
                indexCount = info[Database.OperationInfoKeyIndexCount]?.intValue ?? 0
            }
        }
        let database = Database(at: "\(self.recommendedPath.path)_testOperation")
        database.tag = self.recommendTag
        XCTAssertNoThrow(try database.create(table: TracerObject.name, of: TracerObject.self))
        XCTAssertNoThrow(try database.exec(StatementCreateIndex().create(index: "testIndex").on(table: TracerObject.name).indexesBy(TracerObject.Properties.variable)))

        XCTAssertTrue(tag == database.tag)
        XCTAssertTrue(path == database.path)
        XCTAssertTrue(openCount == 1)

        database.close()

        let template = TracerObject()
        template.isAutoIncrement = true
        let objects = [TracerObject](repeating: template, count: 10)
        XCTAssertNoThrow(try database.insert(objects, intoTable: TracerObject.name))

        XCTAssertTrue(openCount == 2)
        XCTAssertTrue(tableCount == 4)
        XCTAssertTrue(indexCount == 1)
        Database.globalTracePerformance(nil)
    }

    var dispatchTid: UInt64 = 0
    var objects: [TestObject] = []
    func testGlobalTraceDatabaseBusy() {
        var testTid: UInt64 = 0
        Database.globalTraceBusy({ tag, path, tid, sql in
            XCTAssertEqual(tag, self.database.tag)
            XCTAssertEqual(path, self.database.path)
            XCTAssertEqual(sql, "INSERT INTO TestObject(variable1, variable2) VALUES(?1, ?2)")
            testTid = tid
        }, timeOut: 0.1)

        XCTAssertNoThrow(try database.create(table: TestObject.name, of: TestObject.self))
        let group = DispatchGroup()
        let queue = DispatchQueue(label: String(recommendTag), attributes: .concurrent)
        objects = Random.testObjects(startWith: 0, count: 50000)
        queue.async(group: group, execute: {
            pthread_threadid_np(nil, &self.dispatchTid)
            try! self.database.insert(self.objects, intoTable: TestObject.name)
        })
        usleep(10000)
        XCTAssertNoThrow(try database.insert(TestObject(variable1: 100001, variable2: "abdad"),
                                             intoTable: TestObject.name))

        print("finish insert2")
        group.wait()
        XCTAssertTrue(testTid != 0 && testTid == dispatchTid)
        Database.globalTraceBusy(nil, timeOut: 0)
    }
}
