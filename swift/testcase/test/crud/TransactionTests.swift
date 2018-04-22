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

class TransactionTests: CRUDTestCase {

    func testBase() {
        //When
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //Then
        XCTAssertNotNil(transaction.tag)
        XCTAssertEqual(transaction.tag, database.tag)
        XCTAssertEqual(transaction.path, database.path)
    }

    func testCommitTransactionWithDatabase() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.begin())
        XCTAssertNoThrow(try database.insert(objects: object, intoTable: CRUDObject.name))
        XCTAssertNoThrow(try database.commit())
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, self.name)
    }

    func testRollbackTransactionWithDatabase() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.begin())
        XCTAssertNoThrow(try database.insert(objects: object, intoTable: CRUDObject.name))
        XCTAssertNoThrow(try database.rollback())
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 0)
    }

    func testControllableTransactionFailedWithDatabase() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        //When
        XCTAssertThrowsError(try database.run(controllableTransaction: {
            try database.insert(objects: object, intoTable: "nonexistentTable")
            return true
        }))
    }

    func testRunTransactionWithDatabase() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.run(transaction: {
            try database.insert(objects: object, intoTable: CRUDObject.name)
        }))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, self.name)
    }

    func testRunTransactionFailedWithDatabase() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        //When
        XCTAssertThrowsError(try database.run(transaction: {
            try database.insert(objects: object, intoTable: "nonexistentTable")
        }))
    }

    func testRollbackControllableTransactionWithDatabase() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.run(controllableTransaction: {
            try database.insert(objects: object, intoTable: CRUDObject.name)
            return false
        }))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 0)
    }

    func testCommitControllableTransactionWithDatabase() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.run(controllableTransaction: {
            try database.insert(objects: object, intoTable: CRUDObject.name)
            return true
        }))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, self.name)
    }

    func testRunEmbededTransactionWithDatabase() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        //When
        XCTAssertNoThrow(try database.run(transaction: {
            try database.run(embeddedTransaction: {
                try database.insert(objects: object, intoTable: CRUDObject.name)
            })
        }))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, self.name)
    }

    func testCommitDeferedTransactionWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertNoThrow(try transaction.begin(.deferred))
        XCTAssertNoThrow(try transaction.insert(objects: object, intoTable: CRUDObject.name))
        XCTAssertNoThrow(try transaction.commit())
        //Then
        XCTAssertEqual(transaction.changes, 1)
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, self.name)
    }

    func testCommitTransactionWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertNoThrow(try transaction.begin())
        XCTAssertNoThrow(try transaction.insert(objects: object, intoTable: CRUDObject.name))
        XCTAssertNoThrow(try transaction.commit())
        //Then
        XCTAssertEqual(transaction.changes, 1)
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, self.name)
    }

    func testRollbackTransactionWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertNoThrow(try transaction.begin())
        XCTAssertNoThrow(try transaction.insert(objects: object, intoTable: CRUDObject.name))
        XCTAssertNoThrow(try transaction.rollback())
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 0)
    }

    func testRunTransactionWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertNoThrow(try transaction.run(transaction: {
            try transaction.insert(objects: object, intoTable: CRUDObject.name)
        }))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, self.name)
    }

    func testRunTransactionFailedWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertThrowsError(try transaction.run(transaction: {
            try transaction.insert(objects: object, intoTable: "nonexistentTable")
        }))
    }

    func testRollbackControllableTransactionWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertNoThrow(try transaction.run(controllableTransaction: {
            try transaction.insert(objects: object, intoTable: CRUDObject.name)
            return false
        }))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 0)
    }

    func testControllableTransactionFailedWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertThrowsError(try transaction.run(controllableTransaction: {
            try transaction.insert(objects: object, intoTable: "nonexistentTable")
            return true
        }))
    }

    func testCommitControllableTransactionWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertNoThrow(try transaction.run(controllableTransaction: {
            try transaction.insert(objects: object, intoTable: CRUDObject.name)
            return true
        }))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, self.name)
    }

    func testRunEmbededTransactionWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertNoThrow(try transaction.run(transaction: {
            try transaction.run(embeddedTransaction: {
                try transaction.insert(objects: object, intoTable: CRUDObject.name)
            })
        }))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, self.name)
    }

    func testPrepareWithTransaction() {
        //Give
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        let optionalCoreStatement = WCDBAssertNoThrowReturned(
            try transaction.prepare(StatementPragma().pragma(.journalMode)),
            whenFailed: nil
        )
        XCTAssertNotNil(optionalCoreStatement)
        let coreStatement = optionalCoreStatement!
        XCTAssertNoThrow(try coreStatement.step())
        //Then
        let journalMode: String = coreStatement.value(atIndex: 0) ?? ""
        XCTAssertEqual(journalMode.lowercased(), "wal")
    }

    func testExecWithTransaction() {
        //Give
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertNoThrow(try transaction.exec(StatementDelete().delete(fromTable: CRUDObject.name)))
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 0)
    }

    func testIsTableExistsWithTransaction() {
        //Give
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //Then
        let randomTableExists = WCDBAssertNoThrowReturned(
            try transaction.isTableExists("randomTable"),
            whenFailed: nil
        )
        XCTAssertNotNil(randomTableExists)
        XCTAssertFalse(randomTableExists!)

        let crudTableExists = WCDBAssertNoThrowReturned(
            try transaction.isTableExists(CRUDObject.name),
            whenFailed: nil
        )
        XCTAssertNotNil(crudTableExists)
        XCTAssertTrue(crudTableExists!)
    }

    func testEmbeddedTransactionWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
        XCTAssertNotNil(optionalTransaction)
        let transaction = optionalTransaction!
        //When
        XCTAssertNoThrow(try transaction.run(embeddedTransaction: {
            try transaction.insert(objects: object, intoTable: CRUDObject.name)
        }))
        //Then
        XCTAssertEqual(transaction.changes, 1)
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 1)
        XCTAssertEqual(results[0].variable2, self.name)
    }

    func testAutoRollbackTransactionWithTransaction() {
        //Give
        let object = CRUDObject()
        object.variable1 = 3
        object.variable2 = self.name
        do {
            let optionalTransaction = WCDBAssertNoThrowReturned(try database.getTransaction(), whenFailed: nil)
            XCTAssertNotNil(optionalTransaction)
            let transaction = optionalTransaction!
            //When
            XCTAssertNoThrow(try transaction.begin())
            XCTAssertNoThrow(try transaction.insert(objects: object, intoTable: CRUDObject.name))
        }
        //Then
        let results: [CRUDObject] = WCDBAssertNoThrowReturned(
            try database.getObjects(fromTable: CRUDObject.name, where: CRUDObject.Properties.variable1 == 3),
            whenFailed: [CRUDObject]()
        )
        XCTAssertEqual(results.count, 0)
    }

}
