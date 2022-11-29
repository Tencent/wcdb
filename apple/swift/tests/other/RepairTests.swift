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

class RepairTests: DatabaseTestCase {
    let preInsertedObjects: [TestObject] = {
        let object1 = TestObject()
        object1.variable1 = 1
        object1.variable2 = "object1"
        let object2 = TestObject()
        object2.variable1 = 2
        object2.variable2 = "object2"
        return [object1, object2]
    }()

    func excuteTest(_ operation: () throws -> Void) {
        XCTAssertNoThrow(try database.create(table: TestObject.name, of: TestObject.self))
        XCTAssertNoThrow(try database.insert(preInsertedObjects, intoTable: TestObject.name))
        XCTAssertNoThrow(try operation())
        XCTAssertNoThrow(try database.removeFiles())
        database.setCipher(key: RandomData(withSeed: 0).data(withLength: 32))
        XCTAssertNoThrow(try database.create(table: TestObject.name, of: TestObject.self))
        XCTAssertNoThrow(try database.insert(preInsertedObjects, intoTable: TestObject.name))
        XCTAssertNoThrow(try operation())
    }

    func testBackup() {
        excuteTest {
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
    }

    func testBackupFilter() {
        excuteTest {
            database.filterBackup(tableShouldBeBackedUp: nil)

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
    }

    func testAutoBackup() {
        excuteTest {
            self.database.setAutoBackup(enable: true)
            let newContent = TestObject()
            newContent.variable1 = 3
            newContent.variable2 = "object3"
            XCTAssertFalse(self.fileManager.fileExists(atPath: self.database.firstMaterialPath))
            XCTAssertNoThrow(try self.database.insert(newContent, intoTable: TestObject.name))
            XCTAssertNoThrow(try self.database.passiveCheckpoint())
#if WCDB_QUICK_TESTS
            Thread.sleep(forTimeInterval: 12)
#else
            Thread.sleep(forTimeInterval: 66)
#endif
            XCTAssertTrue(self.fileManager.fileExists(atPath: self.database.firstMaterialPath))
        }
    }

    func testDeposit() {
        excuteTest {
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
            XCTAssertNoThrow(try self.database.insert(newObject, intoTable: TestObject.name))
            rowid += 1
            XCTAssertTrue(newObject.lastInsertedRowID == rowid)

            // 2.
            XCTAssertNoThrow(try database.backup())
            XCTAssertNoThrow(try database.deposit())
            let num2 = try? database.getValue(on: TestObject.Properties.any.count(), fromTable: TestObject.name)
            XCTAssertTrue(num2 != nil && num2!.int32Value == 0)

            newObject.variable2 = "object4"
            XCTAssertNoThrow(try self.database.insert(newObject, intoTable: TestObject.name))
            rowid += 1
            XCTAssertTrue(newObject.lastInsertedRowID == rowid)

            XCTAssertTrue(self.fileManager.fileExists(atPath: self.database.factoryPath))
            XCTAssertTrue(self.database.containDepositedFiles())
            XCTAssertNoThrow(try self.database.removeDepositedFiles())
            XCTAssertFalse(self.database.containDepositedFiles())
            XCTAssertFalse(self.fileManager.fileExists(atPath: self.database.factoryPath))
        }
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
        excuteTest {
            XCTAssertNoThrow(try database.backup())
            XCTAssertNoThrow(try database.deposit())
            XCTAssertNoThrow(try database.corruptHeader())
            doTestRetrieve(expecting: true)
            doTestObjectsRetrieved(expecting: true)
        }
    }

    func testRetrieveWithBackupAndWithoutDeposit() {
        excuteTest {
            XCTAssertNoThrow(try database.backup())
            XCTAssertNoThrow(try database.corruptHeader())
            doTestRetrieve(expecting: true)
            doTestObjectsRetrieved(expecting: true)
        }
    }

    func testRetrieveWithoutBackupAndWithDeposit() {
        excuteTest {
            XCTAssertNoThrow(try database.deposit())
            XCTAssertNoThrow(try database.corruptHeader())
            doTestRetrieve(expecting: false)
            doTestObjectsRetrieved(expecting: true)
        }
    }

    func testRetrieveWithoutBackupAndDeposit() {
        excuteTest {
            XCTAssertNoThrow(try database.corruptHeader())
            doTestRetrieve(expecting: false)
            doTestObjectsRetrieved(expecting: false)
        }
    }
}
