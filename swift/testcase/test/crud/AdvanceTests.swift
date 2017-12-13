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

        XCTAssertEqual(coreStatement.columnType(atIndex: 0), ColumnType.integer64)
        XCTAssertEqual(coreStatement.columnType(atIndex: 1), ColumnType.text)

        do {
            let variable1: Int32? = coreStatement.value(atIndex: 0)
            XCTAssertNotNil(variable1)
            XCTAssertEqual(variable1!, expectedVariable1)
            let variable2: String? = coreStatement.value(atIndex: 1)
            XCTAssertNotNil(variable2)
            XCTAssertEqual(variable2!, expectedVariable2)
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
        let bindingParameters = Array(repeating: Expression.bindingParameter, count: 2)
        let statement = StatementInsert().insert(intoTable: CRUDObject.name).values(bindingParameters)
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
        XCTAssertNoThrow(try coreStatement.bind(CRUDObject.Properties.variable1, of: expectedObject, toIndex: 1))
        XCTAssertNoThrow(try coreStatement.bind(CRUDObject.Properties.variable2, of: expectedObject, toIndex: 2))
        XCTAssertNoThrow(try coreStatement.step())

        XCTAssertNoThrow(try coreStatement.finalize())
        //Then
        do {
            let condition = CRUDObject.Properties.variable1 == expectedVariable1
            let result: CRUDObject? = WCDBAssertNoThrowReturned(
                try database.getObject(fromTable: CRUDObject.name, where: condition)
            )
            XCTAssertNotNil(result)
            XCTAssertEqual(result!.variable2, expectedVariable2)
        }
        do {
            let condition = CRUDObject.Properties.variable1 == expectedObject.variable1!
            let result: CRUDObject? = WCDBAssertNoThrowReturned(
                try database.getObject(fromTable: CRUDObject.name, where: condition)
            )
            XCTAssertNotNil(result)
            XCTAssertEqual(result!, expectedObject)
        }
    }

    func testFundamentalValue() {
        //Give
        let statement = StatementSelect().select(1, "2", 3.0).from(CRUDObject.name)
        let optionalCoreStatement = WCDBAssertNoThrowReturned(try database.prepare(statement), whenFailed: nil)
        XCTAssertNotNil(optionalCoreStatement)
        let coreStatement = optionalCoreStatement!

        //When
        XCTAssertNoThrow(try coreStatement.step())
        //Then
        let int32FundamentalValue = coreStatement.value(atIndex: 0)
        XCTAssertEqual(int32FundamentalValue.int32Value, 1)
        XCTAssertEqual(int32FundamentalValue.int64Value, 1)
        XCTAssertEqual(int32FundamentalValue.stringValue, "1")
        XCTAssertEqual(int32FundamentalValue.dataValue, "1".data(using: .ascii)!)
        XCTAssertEqual(int32FundamentalValue.doubleValue, 1.0)

        let stringFundamentalValue = coreStatement.value(atIndex: 1)
        XCTAssertEqual(stringFundamentalValue.int32Value, 2)
        XCTAssertEqual(stringFundamentalValue.int64Value, 2)
        XCTAssertEqual(stringFundamentalValue.stringValue, "2")
        XCTAssertEqual(stringFundamentalValue.dataValue, "2".data(using: .ascii)!)
        XCTAssertEqual(stringFundamentalValue.doubleValue, 2.0)

        let doubleFundamentalValue = coreStatement.value(atIndex: 2)
        XCTAssertEqual(doubleFundamentalValue.int32Value, 3)
        XCTAssertEqual(doubleFundamentalValue.int64Value, 3)
        XCTAssertEqual(doubleFundamentalValue.stringValue, "3.0")
        XCTAssertEqual(doubleFundamentalValue.dataValue, "3.0".data(using: .ascii)!)
        XCTAssertEqual(doubleFundamentalValue.doubleValue, 3.0)

        let nullFundamentalValue = coreStatement.value(atIndex: 3)
        XCTAssertEqual(nullFundamentalValue.int32Value, 0)
        XCTAssertEqual(nullFundamentalValue.int64Value, 0)
        XCTAssertEqual(nullFundamentalValue.stringValue, "")
        XCTAssertEqual(nullFundamentalValue.dataValue, Data())
        XCTAssertEqual(nullFundamentalValue.doubleValue, 0)
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
            let optionalCoreStatement = WCDBAssertNoThrowReturned(
                try database.prepare(StatementPragma().pragma(.secureDelete)),
                whenFailed: nil
            )
            XCTAssertNotNil(optionalCoreStatement)
            let coreStatement = optionalCoreStatement!
            XCTAssertNoThrow(try coreStatement.step())
            let isSecureDelete: Int32? = coreStatement.value(atIndex: 0)
            XCTAssertNotNil(isSecureDelete)
            XCTAssertEqual(isSecureDelete!, 0)
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
            let optionalCoreStatement = WCDBAssertNoThrowReturned(
                try database.prepare(StatementPragma().pragma(.secureDelete)),
                whenFailed: nil
            )
            XCTAssertNotNil(optionalCoreStatement)
            let coreStatement = optionalCoreStatement!
            XCTAssertNoThrow(try coreStatement.step())
            let isSecureDelete: Int32? = coreStatement.value(atIndex: 0)
            XCTAssertNotNil(isSecureDelete)
            XCTAssertEqual(isSecureDelete!, 1)
            XCTAssertNoThrow(try coreStatement.finalize())
        }
    }

    func testOrderedConfig() {
        //Then
        do {
            let optionalCoreStatement = WCDBAssertNoThrowReturned(
                try database.prepare(StatementPragma().pragma(.secureDelete)),
                whenFailed: nil
            )
            XCTAssertNotNil(optionalCoreStatement)
            let coreStatement = optionalCoreStatement!
            XCTAssertNoThrow(try coreStatement.step())
            let isSecureDelete: Int32? = coreStatement.value(atIndex: 0)
            XCTAssertNotNil(isSecureDelete)
            XCTAssertEqual(isSecureDelete!, 0)
            XCTAssertNoThrow(try coreStatement.finalize())
        }
        //Give
        database.close()
        database.setConfig(named: "SecureDelete", with: { (handle) throws in
            let statement = StatementPragma().pragma(.secureDelete, to: true)
            try handle.exec(statement)
        }, orderBy: -2)
        var tested = false
        database.setConfig(named: "test") { (handle) throws in
            tested = true
            //Then
            do {
                let handleStatement: HandleStatement? = WCDBAssertNoThrowReturned(
                    try handle.prepare(StatementPragma().pragma(.secureDelete))
                )
                XCTAssertNotNil(handleStatement)
                let wrappedHandleStatement = handleStatement!
                XCTAssertNoThrow(try wrappedHandleStatement.step())
                let isSecureDelete: Int32 = wrappedHandleStatement.columnValue(atIndex: 0)
                XCTAssertEqual(isSecureDelete, 1)
                XCTAssertNoThrow(try wrappedHandleStatement.finalize())
            }
        }
        //When
        XCTAssertTrue(database.canOpen)
        XCTAssertTrue(tested)
    }

    func testRedirect() {
        let property = CRUDObject.Properties.any.count().as(CRUDObject.Properties.variable1)
        let optionalObject: CRUDObject? = WCDBAssertNoThrowReturned(
            try database.getObject(on: property, fromTable: CRUDObject.name),
            whenFailed: nil
        )
        XCTAssertNotNil(optionalObject)
        XCTAssertEqual(optionalObject!.variable1, preInsertedObjects.count)
    }

    class AppleFTSObject: CRUDObjectBase, TableCodable {
        var variable1: Int = 0
        var variable2: String = ""

        enum CodingKeys: String, CodingTableKey {
            typealias Root = AppleFTSObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable1: ColumnConstraintBinding(isPrimary: true,
                                                            orderBy: .ascending,
                                                            isAutoIncrement: true)]
            }
            static var virtualTableBinding: VirtualTableBinding? {
                return VirtualTableBinding(with: .fts3, and: ModuleArgument(with: .Apple))
            }
        }

        override var hashValue: Int {
            return (String(variable1)+variable2).hashValue
        }
    }
    func testFTSWithAppleTokenize() {
        //Give
        database.close {
            XCTAssertNoThrow(try self.database.removeFiles())
        }
        database.setTokenizes(.Apple)
        XCTAssertNoThrow(try database.create(virtualTable: AppleFTSObject.name, of: AppleFTSObject.self))

        let preInsertedEnglishFTSObject: AppleFTSObject = {
            let object = AppleFTSObject()
            object.variable1 = 1
            object.variable2 = "This is English test content"
            return object
        }()
        let preInsertedChineseFTSObject: AppleFTSObject = {
            let object = AppleFTSObject()
            object.variable1 = 2
            object.variable2 = "这是中文测试内容"
            return object
        }()
        let preInsertedNumbericFTSObject: AppleFTSObject = {
            let object = AppleFTSObject()
            object.variable1 = 1
            object.variable2 = "123456"
            return object
        }()
        let preInsertedSymbolicFTSObject: AppleFTSObject = {
            let object = AppleFTSObject()
            object.variable1 = 1
            object.variable2 = "abc..def"
            return object
        }()
        let objects = [preInsertedEnglishFTSObject,
                       preInsertedChineseFTSObject,
                       preInsertedNumbericFTSObject,
                       preInsertedSymbolicFTSObject]
        XCTAssertNoThrow(try database.insert(objects: objects, intoTable: AppleFTSObject.name))

        //English
        do {
            //When
            let condition = AppleFTSObject.Properties.variable2.match("Engl*")
            let objects = WCDBAssertNoThrowReturned(
                try database.getObjects(fromTable: AppleFTSObject.name, where: condition),
                whenFailed: [AppleFTSObject]()
            )
            //Then
            XCTAssertEqual(objects.count, 1)
            XCTAssertEqual(objects[0], preInsertedEnglishFTSObject)
        }
        //Chinese
        do {
            //When
            let condition = AppleFTSObject.Properties.variable2.match("中文")
            let objects = WCDBAssertNoThrowReturned(
                try database.getObjects(fromTable: AppleFTSObject.name, where: condition),
                whenFailed: [AppleFTSObject]()
            )
            //Then
            XCTAssertEqual(objects.count, 1)
            XCTAssertEqual(objects[0], preInsertedChineseFTSObject)
        }
        //Numberic
        do {
            //When
            let condition = AppleFTSObject.Properties.variable2.match("123*")
            let objects = WCDBAssertNoThrowReturned(
                try database.getObjects(fromTable: AppleFTSObject.name, where: condition),
                whenFailed: [AppleFTSObject]()
            )
            //Then
            XCTAssertEqual(objects.count, 1)
            XCTAssertEqual(objects[0], preInsertedNumbericFTSObject)
        }
        //Symbolic
        do {
            //When
            let condition = AppleFTSObject.Properties.variable2.match("def")
            let objects = WCDBAssertNoThrowReturned(
                try database.getObjects(fromTable: AppleFTSObject.name, where: condition),
                whenFailed: [AppleFTSObject]()
            )
            //Then
            XCTAssertEqual(objects.count, 1)
            XCTAssertEqual(objects[0], preInsertedSymbolicFTSObject)
        }

        //Failed to find Chinese
        do {
            //When
            let condition = AppleFTSObject.Properties.variable2.match("文测")
            let objects = WCDBAssertNoThrowReturned(
                try database.getObjects(fromTable: AppleFTSObject.name, where: condition),
                whenFailed: [AppleFTSObject]()
            )
            //Then
            XCTAssertEqual(objects.count, 0)
        }
    }

    class WCDBFTSObject: CRUDObjectBase, TableCodable {
        var variable1: Int = 0
        var variable2: String = ""

        enum CodingKeys: String, CodingTableKey {
            typealias Root = WCDBFTSObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable1: ColumnConstraintBinding(isPrimary: true,
                                                            orderBy: .ascending,
                                                            isAutoIncrement: true)]
            }
            static var virtualTableBinding: VirtualTableBinding? {
                return VirtualTableBinding(with: .fts3, and: ModuleArgument(with: .WCDB))
            }
        }

        override var hashValue: Int {
            return (String(variable1)+variable2).hashValue
        }
    }
    func testFTSWithWCDBTokenize() {
        //Give
        database.close {
            XCTAssertNoThrow(try self.database.removeFiles())
        }
        database.setTokenizes(.WCDB)
        XCTAssertNoThrow(try database.create(virtualTable: WCDBFTSObject.name, of: WCDBFTSObject.self))

        let preInsertedEnglishFTSObject: WCDBFTSObject = {
            let object = WCDBFTSObject()
            object.variable1 = 1
            object.variable2 = "This is English test content"
            return object
        }()
        let preInsertedChineseFTSObject: WCDBFTSObject = {
            let object = WCDBFTSObject()
            object.variable1 = 2
            object.variable2 = "这是中文测试内容"
            return object
        }()
        let preInsertedNumbericFTSObject: WCDBFTSObject = {
            let object = WCDBFTSObject()
            object.variable1 = 1
            object.variable2 = "123456"
            return object
        }()
        let preInsertedSymbolicFTSObject: WCDBFTSObject = {
            let object = WCDBFTSObject()
            object.variable1 = 1
            object.variable2 = "abc..def"
            return object
        }()
        let objects = [preInsertedEnglishFTSObject,
                       preInsertedChineseFTSObject,
                       preInsertedNumbericFTSObject,
                       preInsertedSymbolicFTSObject]
        XCTAssertNoThrow(try database.insert(objects: objects, intoTable: WCDBFTSObject.name))

        //English
        do {
            //When
            let condition = WCDBFTSObject.Properties.variable2.match("Engl*")
            let objects = WCDBAssertNoThrowReturned(
                try database.getObjects(fromTable: WCDBFTSObject.name, where: condition),
                whenFailed: [WCDBFTSObject]()
            )
            //Then
            XCTAssertEqual(objects.count, 1)
            XCTAssertEqual(objects[0], preInsertedEnglishFTSObject)
        }
        //Chinese
        do {
            //When
            let condition = WCDBFTSObject.Properties.variable2.match("中文")
            let objects = WCDBAssertNoThrowReturned(
                try database.getObjects(fromTable: WCDBFTSObject.name, where: condition),
                whenFailed: [WCDBFTSObject]()
            )
            //Then
            XCTAssertEqual(objects.count, 1)
            XCTAssertEqual(objects[0], preInsertedChineseFTSObject)
        }
        //Numberic
        do {
            //When
            let condition = WCDBFTSObject.Properties.variable2.match("123*")
            let objects = WCDBAssertNoThrowReturned(
                try database.getObjects(fromTable: WCDBFTSObject.name, where: condition),
                whenFailed: [WCDBFTSObject]()
            )
            //Then
            XCTAssertEqual(objects.count, 1)
            XCTAssertEqual(objects[0], preInsertedNumbericFTSObject)
        }
        //Symbolic
        do {
            //When
            let condition = WCDBFTSObject.Properties.variable2.match("def")
            let objects = WCDBAssertNoThrowReturned(
                try database.getObjects(fromTable: WCDBFTSObject.name, where: condition),
                whenFailed: [WCDBFTSObject]()
            )
            //Then
            XCTAssertEqual(objects.count, 1)
            XCTAssertEqual(objects[0], preInsertedSymbolicFTSObject)
        }

        //Find Chinese
        do {
            //When
            let condition = WCDBFTSObject.Properties.variable2.match("文测")
            let objects = WCDBAssertNoThrowReturned(
                try database.getObjects(fromTable: WCDBFTSObject.name, where: condition),
                whenFailed: [WCDBFTSObject]()
            )
            //Then
            XCTAssertEqual(objects.count, 1)
            XCTAssertEqual(objects[0], preInsertedChineseFTSObject)
        }
    }
}
