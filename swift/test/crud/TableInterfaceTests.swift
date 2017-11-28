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

class TableInterfaceTests: WCDBTestCase {    
    var database: Database!
    
    override func setUp() {
        super.setUp()
        database = Database(withFileURL: self.recommendedPath)
    }    
    
    class BaselineObject: CodableTable, Named {
        var anInt32: Int32 = -1
        var anInt64: Int64 = 17626545782784
        var aString: String = "string"
        var aData: Data = "data".data(using: .ascii)!
        var aDouble: Double = 0.001
        
        required init() {}        
        var isAutoIncrement: Bool = false
        var lastInsertedRowID: Int64 = 0
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\BaselineObject.anInt32, alias: "anInt32", isPrimary: true, orderBy: .Ascending, isAutoIncrement: true),
                ColumnBinding(\BaselineObject.anInt64, alias: "anInt64"),
                ColumnBinding(\BaselineObject.aString, alias: "aString"),
                ColumnBinding(\BaselineObject.aData, alias: "aData"),
                ColumnBinding(\BaselineObject.aDouble, alias: "aDouble")                
            ]
        }
    }
    func testCreateTable() {
        //Give
        let tableName = BaselineObject.name
        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: BaselineObject.self))
        //Then
        let optionalObject: Master? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: Master.tableName, where: \Master.name==tableName))
        XCTAssertNotNil(optionalObject)
        let object = optionalObject!
        XCTAssertEqual(object.sql!, "CREATE TABLE \(tableName)(anInt32 INTEGER PRIMARY KEY ASC AUTOINCREMENT, anInt64 INTEGER, aString TEXT, aData BLOB, aDouble REAL)")
    }

    class IndexObject: CodableTable, Named {
        var variable: Int32 = 0
        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\IndexObject.variable, alias: "variable"),
            ]
        }        
        static func indexBindings() -> [IndexBinding]? {
            return [
                IndexBinding(withSubfix: "_index", indexesBy: \IndexObject.variable)
            ]
        }
    }
    func testCreateTableWithIndex() {
        //Give
        let tableName = IndexObject.name
        let indexName = tableName+"_index"
        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: IndexObject.self))
        //Then
        let optionalObject: Master? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: Master.tableName, where: \Master.name==indexName))
        XCTAssertNotNil(optionalObject)
        let object = optionalObject!
        XCTAssertEqual(object.sql!, "CREATE INDEX \(indexName) ON IndexObject(variable)")
    }

    class ConstraintObject: CodableTable, Named {
        var variable1: Int32 = 0
        var variable2: Int32 = 0
        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\ConstraintObject.variable1, alias: "variable1"),
                ColumnBinding(\ConstraintObject.variable2, alias: "variable2"),
            ]
        }
        static func constraintBindings() -> [ConstraintBinding]? {
            return [
                MultiUniqueBinding(named: "ConstraintObjectConstraint", indexesBy: \ConstraintObject.variable1, \ConstraintObject.variable2)
            ]
        }
    }
    func testCreateTableWithConstraint() {
        //Give
        let tableName = ConstraintObject.name
        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: ConstraintObject.self))
        //Then
        let optionalObject: Master? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: Master.tableName, where: \Master.name==tableName))
        XCTAssertNotNil(optionalObject)
        let object = optionalObject!
        XCTAssertEqual(object.sql!, "CREATE TABLE \(tableName)(variable1 INTEGER, variable2 INTEGER, CONSTRAINT ConstraintObjectConstraint UNIQUE(variable1, variable2))")
    }

    class VirtualTableObject: CodableTable, Named {
        var variable1: Int32 = 0
        var variable2: Int32 = 0
        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\VirtualTableObject.variable1, alias: "variable1"),
                ColumnBinding(\VirtualTableObject.variable2, alias: "variable2"),
            ]
        }
        static func virtualTableBinding() -> VirtualTableBinding? {
            return VirtualTableBinding(with: .fts3, and: ModuleArgument(with: .WCDB))
        }
    }
    func testCreateVirtualTable() {
        //Give
        let tableName = VirtualTableObject.name
        database.setTokenizes(.WCDB)
        //When
        XCTAssertNoThrow(try database.create(virtualTable: tableName, of: VirtualTableObject.self))
        //Then
        let optionalObject: Master? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: Master.tableName, where: \Master.name==tableName))
        XCTAssertNotNil(optionalObject)
        let object = optionalObject!
        XCTAssertEqual(object.sql!, "CREATE VIRTUAL TABLE VirtualTableObject USING fts3(variable1 INTEGER, variable2 INTEGER, tokenize=WCDB)")
    }
    
    class AutoFitBaseLineObject: CodableTable, Named {
        var anInt32: Int32 = -1
        var anInt64: Int64 = 17626545782784
        var aString: String = "string"
        var aData: Data = "data".data(using: .ascii)!
        var aDouble: Double = 0.001
        var newColumn: Int = 0
        
        required init() {}        
        var isAutoIncrement: Bool = false
        var lastInsertedRowID: Int64 = 0
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\AutoFitBaseLineObject.anInt32, alias: "anInt32", isPrimary: true, orderBy: .Ascending, isAutoIncrement: true),
                ColumnBinding(\AutoFitBaseLineObject.anInt64, alias: "anInt64"),
                ColumnBinding(\AutoFitBaseLineObject.aString, alias: "aString"),
                ColumnBinding(\AutoFitBaseLineObject.aData, alias: "aData"),
                ColumnBinding(\AutoFitBaseLineObject.aDouble, alias: "aDouble"),
                ColumnBinding(\AutoFitBaseLineObject.newColumn, alias: "newColumn")                
            ]
        }
    }
    func testCreateTableAutoFitORM() {
        //Give
        let tableName = AutoFitBaseLineObject.name
        XCTAssertNoThrow(try database.create(table: tableName, of: BaselineObject.self))
        //Then
        XCTAssertNoThrow(try database.create(table: tableName, of: AutoFitBaseLineObject.self))
        let optionalObject: Master? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: Master.tableName, where: \Master.name==tableName))
        XCTAssertNotNil(optionalObject)
        let object = optionalObject!
        XCTAssertEqual(object.sql!, "CREATE TABLE \(tableName)(anInt32 INTEGER PRIMARY KEY ASC AUTOINCREMENT, anInt64 INTEGER, aString TEXT, aData BLOB, aDouble REAL, newColumn INTEGER)")
    }
    
    func testDropTable() {
        //Give
        let tableName = BaselineObject.name
        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: BaselineObject.self))
        XCTAssertNoThrow(try database.drop(table: tableName))
        //Then
        let optionalObject: Master? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: Master.tableName, where: \Master.name==tableName))
        XCTAssertNil(optionalObject)
    }

    func testDropIndex() {
        //Give
        let tableName = IndexObject.name
        let indexName = tableName+"_index"
        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: IndexObject.self))
        XCTAssertNoThrow(try database.drop(index: indexName))
        //Then
        let optionalObject: Master? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: Master.tableName, where: \Master.name==indexName))
        XCTAssertNil(optionalObject)
    }

    func testManuallyCreateTable() {
        //Give
        let tableName = BaselineObject.name
        let tableConstraint = TableConstraint(named: "BaselineObjectConstraint").check((\BaselineObject.anInt32)>0)
        let def1 = (\BaselineObject.anInt32).asDef(with: .Integer32)
        let def2 = (\BaselineObject.anInt64).asDef(with: .Integer64)
        //When
        XCTAssertNoThrow(try database.create(table: tableName, with: def1, def2, and: [tableConstraint]))
        let optionalObject: Master? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: Master.tableName, where: \Master.name==tableName))
        XCTAssertNotNil(optionalObject)
        let object = optionalObject!
        XCTAssertEqual(object.sql!, "CREATE TABLE \(tableName)(anInt32 INTEGER, anInt64 INTEGER, CONSTRAINT BaselineObjectConstraint CHECK(anInt32 > 0))")
    }
    
    func testManuallyAddColumn() {
        //Give
        let tableName = BaselineObject.name
        let def = Column(named: "newColumn").asDef(with: .Integer32)
        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: BaselineObject.self))
        XCTAssertNoThrow(try database.addColumn(with: def, forTable: tableName))
        //Then
        let optionalObject: Master? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: Master.tableName, where: \Master.name==tableName))
        XCTAssertNotNil(optionalObject)
        let object = optionalObject!
        XCTAssertEqual(object.sql!, "CREATE TABLE \(tableName)(anInt32 INTEGER PRIMARY KEY ASC AUTOINCREMENT, anInt64 INTEGER, aString TEXT, aData BLOB, aDouble REAL, newColumn INTEGER)")
    }
    
    
    func testManuallyCreateIndex() {
        //Give
        let tableName = BaselineObject.name
        let indexName = tableName+"_index"
        let index1 = (\BaselineObject.aString).asIndex()
        let index2 = (\BaselineObject.aDouble).asIndex()
        //When
        XCTAssertNoThrow(try database.create(table: tableName, of: BaselineObject.self))
        XCTAssertNoThrow(try database.create(index: indexName, with: index1, index2, forTable: tableName))
        //Then
        let optionalObject: Master? = WCDBAssertNoThrowReturned(try database.getObject(fromTable: Master.tableName, where: \Master.name==indexName))
        XCTAssertNotNil(optionalObject)
        let object = optionalObject!
        XCTAssertEqual(object.sql!, "CREATE INDEX \(indexName) ON \(tableName)(aString, aDouble)")
    }
    
    func testGetTable() {
        //Give
        let tableName = BaselineObject.name
        var table: Table<BaselineObject>? = nil
        //When
        table = WCDBAssertNoThrowReturned(try database.getTable(named: tableName))
        XCTAssertNil(table)
        XCTAssertNoThrow(try database.create(table: tableName, of: BaselineObject.self))
        //Then
        table = WCDBAssertNoThrowReturned(try database.getTable(named: tableName))
        XCTAssertNotNil(table)
    }
}
