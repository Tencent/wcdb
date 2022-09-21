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
    var database: Database!
    var pageSize: Int32!
    let preInsertedObjects: [TestObject] = {
        let object1 = TestObject()
        object1.variable1 = 1
        object1.variable2 = "object1"
        let object2 = TestObject()
        object2.variable1 = 2
        object2.variable2 = "object2"
        return [object1, object2]
    }()

    override func setUp() {
        super.setUp()
        database = Database(withFileURL: self.recommendedPath)

        let handle = WCDBAssertNoThrowReturned(try database.getHandle())!
        XCTAssertNoThrow(try handle.prepare(StatementPragma().pragma(.pageSize)))

        XCTAssertNoThrow(try handle.step())

        pageSize = handle.value(atIndex: 0)

        handle.finalize()

        XCTAssertEqual(pageSize >> 1 & pageSize, 0)

        XCTAssertGreaterThan(pageSize, 0)

        XCTAssertNoThrow(try database.create(table: TestObject.name, of: TestObject.self))

        XCTAssertNoThrow(try database.insert(objects: preInsertedObjects, intoTable: TestObject.name))
    }

    func testBackup() {
        XCTAssertFalse(self.fileManager.fileExists(atPath: self.database.firstMaterialPath))
        XCTAssertFalse(self.fileManager.fileExists(atPath: self.database.lastMaterialPath))

        XCTAssertNoThrow(try self.database.backup())
        XCTAssertTrue(self.fileManager.fileExists(atPath: self.database.firstMaterialPath))
        XCTAssertFalse(self.fileManager.fileExists(atPath: self.database.lastMaterialPath))

        Thread.sleep(forTimeInterval: 1)

        XCTAssertNoThrow(try self.database.backup())
        XCTAssertTrue(self.fileManager.fileExists(atPath: self.database.firstMaterialPath))
        XCTAssertTrue(self.fileManager.fileExists(atPath: self.database.lastMaterialPath))
    }

    func testBackupFilter() {

        XCTAssertNoThrow(try self.database.backup())
        XCTAssertTrue(self.fileManager.fileExists(atPath: self.database.firstMaterialPath))
        XCTAssertFalse(self.fileManager.fileExists(atPath: self.database.lastMaterialPath))

        database.filterBackup { _ in
            return false
        }

        Thread.sleep(forTimeInterval: 1)

        XCTAssertNoThrow(try self.database.backup())
        XCTAssertTrue(self.fileManager.fileExists(atPath: self.database.firstMaterialPath))
        XCTAssertTrue(self.fileManager.fileExists(atPath: self.database.lastMaterialPath))

        let firstSize = self.fileManager.fileSize(of: self.database.firstMaterialPath)
        let lastSize = self.fileManager.fileSize(of: self.database.lastMaterialPath)
        XCTAssertTrue(firstSize > lastSize)
    }

    func testAutoBackup() {
        self.database.setAutoBackup(enable: true)
        let newContent = TestObject()
        newContent.variable1 = 3
        newContent.variable2 = "object3"
        XCTAssertFalse(self.fileManager.fileExists(atPath: self.database.firstMaterialPath))
        XCTAssertNoThrow(try self.database.insert(objects: newContent, intoTable: TestObject.name))
        XCTAssertNoThrow(try self.database.passiveCheckpoint())
        Thread.sleep(forTimeInterval: 66)
        XCTAssertTrue(self.fileManager.fileExists(atPath: self.database.firstMaterialPath))
    }

    func testDeposit() {
        // 0.
        let num0 = try? database.getValue(on: TestObject.Properties.any.count(), fromTable: TestObject.name)
        XCTAssertTrue(num0 != nil && num0!.int32Value > 0)
        var rowid = num0!.int32Value

        // 1.
        XCTAssertNoThrow(try database.backup())
        XCTAssertNoThrow(try database.deposit())

        let num1 = try? database.getValue(on: TestObject.Properties.any.count(), fromTable: TestObject.name)
        XCTAssertTrue(num1 != nil && num1!.int32Value == 0)

        let newObject = TestObject()
        newObject.variable2 = "object3"
        newObject.isAutoIncrement = true
        XCTAssertNoThrow(try self.database.insert(objects: newObject, intoTable: TestObject.name))
        rowid += 1
        XCTAssertTrue(newObject.lastInsertedRowID == rowid)

        // 2.
        XCTAssertNoThrow(try database.backup())
        XCTAssertNoThrow(try database.deposit())
        let num2 = try? database.getValue(on: TestObject.Properties.any.count(), fromTable: TestObject.name)
        XCTAssertTrue(num2 != nil && num2!.int32Value == 0)

        newObject.variable2 = "object4"
        XCTAssertNoThrow(try self.database.insert(objects: newObject, intoTable: TestObject.name))
        rowid += 1
        XCTAssertTrue(newObject.lastInsertedRowID == rowid)

        XCTAssertTrue(self.fileManager.fileExists(atPath: self.database.factoryPath))
        XCTAssertTrue(self.database.containDepositedFiles())
        XCTAssertNoThrow(try self.database.removeDepositedFiles())
        XCTAssertFalse(self.database.containDepositedFiles())
        XCTAssertFalse(self.fileManager.fileExists(atPath: self.database.factoryPath))
    }

    func doTestRetrieve(expecting success: Bool) {
        var lastPercentage = 0.0
        var sanity = true
        let score = database.retrieve { percentage, increment in
            if percentage - lastPercentage != increment || increment <= 0 {
                XCTFail()
                sanity = false
            }
            lastPercentage = percentage
        }
        XCTAssertTrue(sanity)
        XCTAssertTrue((success && score == 1.0) || (!success && score < 1.0))
        XCTAssertTrue(lastPercentage == 1.0)
    }

    func doTestObjectsRetrieved(expecting success: Bool) {
        let allObject: [TestObject]? = try? database.getObjects(fromTable: TestObject.name)
        if success {
            XCTAssertTrue(allObject != nil)
            XCTAssertEqual(allObject!.sorted(), preInsertedObjects.sorted())
        } else {
            XCTAssertTrue(allObject == nil || allObject!.count == 0)
        }
    }

    func testRetrieveWithBackupAndDeposit() {
        XCTAssertNoThrow(try database.backup())
        XCTAssertNoThrow(try database.deposit())
        XCTAssertNoThrow(try database.corruptHeader())
        doTestRetrieve(expecting: true)
        doTestObjectsRetrieved(expecting: true)
    }

    func testRetrieveWithBackupAndWithoutDeposit() {
        XCTAssertNoThrow(try database.backup())
        XCTAssertNoThrow(try database.corruptHeader())
        doTestRetrieve(expecting: true)
        doTestObjectsRetrieved(expecting: true)
    }

    func testRetrieveWithoutBackupAndWithDeposit() {
        XCTAssertNoThrow(try database.deposit())
        XCTAssertNoThrow(try database.corruptHeader())
        doTestRetrieve(expecting: false)
        doTestObjectsRetrieved(expecting: true)
    }

    func testRetrieveWithoutBackupAndDeposit() {
        XCTAssertNoThrow(try database.corruptHeader())
        doTestRetrieve(expecting: false)
        doTestObjectsRetrieved(expecting: false)
    }
}
