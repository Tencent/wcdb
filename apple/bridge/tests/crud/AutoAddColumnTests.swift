//
// Created by 陈秋文 on 2022/12/6.
//

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

class AutoAddColumnTests: DatabaseTestCase {

    func testAutoAddColumn() {
        let fakeTable = "fakeTable"
        let tableName = "AutoAddColumnObject"
        let fakeSchema = "notExistSchema"
        XCTAssertNoThrow(try self.database.create(table: fakeTable, of: AutoAddColumnObject.self))

        doTestAutoAdd(column: AutoAddColumnObject.insertValue(), is: true) {
            try self.database.insert(AutoAddColumnObject(), intoTable: tableName)
        }

        doTestAutoAdd(column: AutoAddColumnObject.updateValue(), is: true) {
            try self.database.update(table: tableName, on: AutoAddColumnObject.updateValue(), with: 1)
        }

        doTestAutoAdd(column: AutoAddColumnObject.deleteValue(), is: true) {
            try self.database.delete(fromTable: tableName, where: AutoAddColumnObject.deleteValue() == 1)
        }

        doTestAutoAdd(column: AutoAddColumnObject.deleteValue(), is: true) {
            try self.database.delete(fromTable: tableName, where: AutoAddColumnObject.deleteValue().in(table: tableName) == 1)
        }

        doTestAutoAdd(column: AutoAddColumnObject.deleteValue(), is: false) {
            try self.database.delete(fromTable: tableName, where: AutoAddColumnObject.deleteValue().in(table: fakeTable) == 1)
        }

        doTestAutoAdd(column: AutoAddColumnObject.deleteValue(), is: false) {
            try self.database.delete(fromTable: tableName, where: AutoAddColumnObject.deleteValue().in(table: tableName).of(schema: fakeSchema) == 1)
        }

        doTestAutoAdd(column: AutoAddColumnObject.selectValue(), is: true) {
            _ = try self.database.getColumn(on: AutoAddColumnObject.selectValue(), fromTable: tableName)
        }

        doTestAutoAdd(column: AutoAddColumnObject.selectValue(), is: true) {
            _ = try self.database.getColumn(on: AutoAddColumnObject.insertValue(), fromTable: tableName, where: AutoAddColumnObject.selectValue() == 1)
        }

        doTestAutoAdd(column: AutoAddColumnObject.selectValue(), is: true) {
            _ = try self.database.getColumn(on: AutoAddColumnObject.insertValue(), fromTable: tableName, orderBy: [AutoAddColumnObject.selectValue().asOrder()])
        }

        doTestAutoAdd(column: AutoAddColumnObject.selectValue(), is: true) {
            _ = try self.database.getColumn(on: AutoAddColumnObject.insertValue(), fromTable: tableName, orderBy: [AutoAddColumnObject.selectValue().in(table: tableName).asOrder()])
        }

        doTestAutoAdd(column: AutoAddColumnObject.selectValue(), is: false) {
            _ = try self.database.getColumn(on: AutoAddColumnObject.insertValue(), fromTable: tableName, orderBy: [AutoAddColumnObject.selectValue().in(table: fakeTable).asOrder()])
        }

        doTestAutoAdd(column: AutoAddColumnObject.selectValue(), is: false) {
            _ = try self.database.getColumn(on: AutoAddColumnObject.insertValue(), fromTable: tableName, orderBy: [AutoAddColumnObject.selectValue().in(table: tableName).of(schema: fakeSchema).asOrder()])
        }

        doTestAutoAdd(column: AutoAddColumnObject.multiSelectValue(), is: true) {
            let multiSelect = try self.database.prepareMultiSelect(on: AutoAddColumnObject.multiSelectValue().in(table: tableName), AutoAddColumnObject.multiSelectValue().in(table: fakeTable), fromTables: [fakeTable, tableName])
            _ = try multiSelect.allMultiObjects()
        }

        doTestAutoAdd(column: AutoAddColumnObject.primeryValue(), is: false) {
            _ = try self.database.getColumn(on: AutoAddColumnObject.primeryValue(), fromTable: tableName)
        }

        doTestAutoAdd(column: AutoAddColumnObject.uniqueValue(), is: false) {
            _ = try self.database.getColumn(on: AutoAddColumnObject.uniqueValue(), fromTable: tableName)
        }
    }

    func doTestAutoAdd(column codingKey: PropertyConvertible, is succeed: Bool, by execute: () throws -> Void) {
        let tableName = "AutoAddColumnObject"
        let propertyName = codingKey.asProperty().name
        let createTable = StatementCreateTable().create(table: tableName)
        let properties = AutoAddColumnObject.allProperties().filter { element in
            element.propertyName != propertyName
        }
        let columnDefs = properties.map {
            ColumnDef(with: $0, and: .integer64)
        }
        createTable.with(columns: columnDefs)
        XCTAssertNoThrow(try self.database.exec(createTable))
        var autoAdded = false
        self.database.trace { error in
            guard error.message == "Auto add column" else {
                return
            }
            autoAdded = true
            XCTAssertEqual(error.extInfos["Table"]?.stringValue, tableName)
            XCTAssertEqual(error.extInfos["Column"]?.stringValue, propertyName)
        }
        if succeed {
            XCTAssertNoThrow(try execute())
            XCTAssertEqual(autoAdded, true)
        } else {
            XCTAssertThrowsError(try execute())
            XCTAssertEqual(autoAdded, false)
        }
        XCTAssertNoThrow(try self.database.drop(table: tableName))
        self.database.trace(ofError: nil)
    }
}
