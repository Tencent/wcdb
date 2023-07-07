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

class ColumnConstraintBindingTests: ORMTestCase {

    final class BaselineTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineTestObject
            case variable
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }
    }

    final class PrimaryTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = PrimaryTestObject
            case variable = "primaryVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, isPrimary: true)
            }
        }
    }

    final class OrderedPrimaryTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = OrderedPrimaryTestObject
            case variable = "orderedPrimaryVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, isPrimary: true, orderBy: .ascending)
            }
        }
    }

    final class AutoIncrementPrimaryTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = AutoIncrementPrimaryTestObject
            case variable = "autoIncrementPrimaryVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, isPrimary: true, isAutoIncrement: true)
            }
        }
    }
    final class ConflictPrimaryTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = ConflictPrimaryTestObject
            case variable = "conflictPrimaryVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, isPrimary: true, onConflict: .Replace)
            }
        }
    }

    final class NotNullTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = NotNullTestObject
            case variable = "notNullVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, isNotNull: true)
            }
        }
    }

    final class UniqueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = UniqueTestObject
            case variable = "uniqueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, isUnique: true)
            }
        }
    }

    final class DefaultInt32ValueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultInt32ValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, defaultTo: Int32(1))
            }
        }
    }

    final class DefaultInt64ValueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultInt64ValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, defaultTo: Int64(1))
            }
        }
    }

    final class DefaultFloatValueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultFloatValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, defaultTo: Double(1.2))
            }
        }
    }

    final class DefaultTextValueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultTextValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, defaultTo: "defaultString")
            }
        }
    }

    final class DefaultBLOBValueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultBLOBValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(variable, defaultTo: "defaultData".data(using: .ascii)!)
            }
        }
    }

    func testProperty() {
        XCTAssertEqual(BaselineTestObject.Properties.variable.asProperty().codingTableKey!.stringValue,
                       BaselineTestObject.Properties.variable.stringValue)

        XCTAssertEqual(BaselineTestObject.Properties.variable.asProperty().description,
                       BaselineTestObject.Properties.variable.asProperty().asProperty().description)

        XCTAssertEqual(BaselineTestObject.Properties.variable.asProperty().codingTableKey!.stringValue,
                       BaselineTestObject.Properties.variable.asProperty().asProperty().codingTableKey!.stringValue)
    }

    func testCantSetLastInsertedRowID() {
        var object = BaselineTestObject()
        // Can't set
        object.lastInsertedRowID = Int64.max
        XCTAssertEqual(object.lastInsertedRowID, Int64.min)
    }

    func testColumnConstraintBinding() {

        ORMConstraintBindingAssertEqual(
            BaselineTestObject.self,
            "CREATE TABLE IF NOT EXISTS BaselineTestObject(variable INTEGER)"
        )

        ORMConstraintBindingAssertEqual(
            PrimaryTestObject.self,
            "CREATE TABLE IF NOT EXISTS PrimaryTestObject(primaryVariable INTEGER PRIMARY KEY)"
        )

        ORMConstraintBindingAssertEqual(
            OrderedPrimaryTestObject.self,
            "CREATE TABLE IF NOT EXISTS OrderedPrimaryTestObject(orderedPrimaryVariable INTEGER PRIMARY KEY ASC)"
        )

        ORMConstraintBindingAssertEqual(
            AutoIncrementPrimaryTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS AutoIncrementPrimaryTestObject\
            (autoIncrementPrimaryVariable INTEGER PRIMARY KEY AUTOINCREMENT)
            """
        )

        ORMConstraintBindingAssertEqual(
            ConflictPrimaryTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS ConflictPrimaryTestObject\
            (conflictPrimaryVariable INTEGER PRIMARY KEY ON CONFLICT REPLACE)
            """
        )

        ORMConstraintBindingAssertEqual(
            NotNullTestObject.self,
            "CREATE TABLE IF NOT EXISTS NotNullTestObject(notNullVariable INTEGER NOT NULL)"
        )

        ORMConstraintBindingAssertEqual(
            UniqueTestObject.self,
            "CREATE TABLE IF NOT EXISTS UniqueTestObject(uniqueVariable INTEGER UNIQUE)"
        )

        ORMConstraintBindingAssertEqual(
            DefaultInt32ValueTestObject.self,
            "CREATE TABLE IF NOT EXISTS DefaultInt32ValueTestObject(defaultValueVariable INTEGER DEFAULT 1)"
        )

        ORMConstraintBindingAssertEqual(
            DefaultInt64ValueTestObject.self,
            "CREATE TABLE IF NOT EXISTS DefaultInt64ValueTestObject(defaultValueVariable INTEGER DEFAULT 1)"
        )

        ORMConstraintBindingAssertEqual(
            DefaultFloatValueTestObject.self,
            "CREATE TABLE IF NOT EXISTS DefaultFloatValueTestObject(defaultValueVariable INTEGER DEFAULT 1.2)"
        )

        ORMConstraintBindingAssertEqual(
            DefaultTextValueTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS DefaultTextValueTestObject\
            (defaultValueVariable INTEGER DEFAULT 'defaultString')
            """
        )

        ORMConstraintBindingAssertEqual(
            DefaultBLOBValueTestObject.self,
            "CREATE TABLE IF NOT EXISTS DefaultBLOBValueTestObject(defaultValueVariable INTEGER DEFAULT 'defaultData')"
        )
    }

    final class EnablePrimaryAutoIncrement: TableCodable {
        var id: Int = 0

        var isAutoIncrement: Bool = false
        var lastInsertedRowID: Int64 = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = EnablePrimaryAutoIncrement
            case id
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(id, isPrimary: true,
                                     isAutoIncrement: true,
                                     enableAutoIncrementForExistingTable: true)
            }
        }
    }

    final class PrimaryNotAutoIncrement: TableCodable {
        var id: Int = 0
        var lastInsertedRowID: Int64 = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = PrimaryNotAutoIncrement
            case id
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindColumnConstraint(id, isPrimary: true)
            }
        }
    }

    func testPrimaryEnableAutoIncrementForExistingTable() {
        XCTAssertNoThrow(try database.create(table: "testTable", of: PrimaryNotAutoIncrement.self))
        let obj1 = PrimaryNotAutoIncrement()
        obj1.id = 1
        XCTAssertNoThrow(try database.insert(obj1, intoTable: "testTable"))

        XCTAssertNoThrow(try database.create(table: "testTable", of: EnablePrimaryAutoIncrement.self))

        XCTAssertNoThrow(try database.delete(fromTable: "testTable"))

        let obj2 = EnablePrimaryAutoIncrement()
        obj2.isAutoIncrement = true
        XCTAssertNoThrow(try database.insert(obj2, intoTable: "testTable"))
        XCTAssertEqual(obj2.lastInsertedRowID, 2)
    }
}
