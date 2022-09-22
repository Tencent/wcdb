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

class ColumnConstraintBindingTests: BaseTestCase {

    class BaselineTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineTestObject
            case variable
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }
    }

    class PrimaryTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = PrimaryTestObject
            case variable = "primaryVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(isPrimary: true)]
            }
        }
    }

    class OrderedPrimaryTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = OrderedPrimaryTestObject
            case variable = "orderedPrimaryVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(isPrimary: true, orderBy: .ascending)]
            }
        }
    }

    class AutoIncrementPrimaryTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = AutoIncrementPrimaryTestObject
            case variable = "autoIncrementPrimaryVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(isPrimary: true, isAutoIncrement: true)]
            }
        }
    }
    class ConflictPrimaryTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = ConflictPrimaryTestObject
            case variable = "conflictPrimaryVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(isPrimary: true, onConflict: .Replace)]
            }
        }
    }

    class NotNullTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = NotNullTestObject
            case variable = "notNullVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(isNotNull: true)]
            }
        }
    }

    class UniqueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = UniqueTestObject
            case variable = "uniqueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(isUnique: true)]
            }
        }
    }

    class DefaultInt32ValueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultInt32ValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(defaultTo: Int32(1))]
            }
        }
    }

    class DefaultInt64ValueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultInt64ValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(defaultTo: Int64(1))]
            }
        }
    }

    class DefaultFloatValueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultFloatValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(defaultTo: Double(1.2))]
            }
        }
    }

    class DefaultTextValueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultTextValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(defaultTo: "defaultString")]
            }
        }
    }

    class DefaultBLOBValueTestObject: TableCodable {
        var variable: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultBLOBValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(defaultTo: "defaultData".data(using: .ascii)!)]
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
            "CREATE TABLE IF NOT EXISTS main.BaselineTestObject(variable INTEGER)"
        )

        ORMConstraintBindingAssertEqual(
            PrimaryTestObject.self,
            "CREATE TABLE IF NOT EXISTS main.PrimaryTestObject(primaryVariable INTEGER PRIMARY KEY)"
        )

        ORMConstraintBindingAssertEqual(
            OrderedPrimaryTestObject.self,
            "CREATE TABLE IF NOT EXISTS main.OrderedPrimaryTestObject(orderedPrimaryVariable INTEGER PRIMARY KEY ASC)"
        )

        ORMConstraintBindingAssertEqual(
            AutoIncrementPrimaryTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS main.AutoIncrementPrimaryTestObject\
            (autoIncrementPrimaryVariable INTEGER PRIMARY KEY AUTOINCREMENT)
            """
        )

        ORMConstraintBindingAssertEqual(
            ConflictPrimaryTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS main.ConflictPrimaryTestObject\
            (conflictPrimaryVariable INTEGER PRIMARY KEY ON CONFLICT REPLACE)
            """
        )

        ORMConstraintBindingAssertEqual(
            NotNullTestObject.self,
            "CREATE TABLE IF NOT EXISTS main.NotNullTestObject(notNullVariable INTEGER NOT NULL)"
        )

        ORMConstraintBindingAssertEqual(
            UniqueTestObject.self,
            "CREATE TABLE IF NOT EXISTS main.UniqueTestObject(uniqueVariable INTEGER UNIQUE)"
        )

        ORMConstraintBindingAssertEqual(
            DefaultInt32ValueTestObject.self,
            "CREATE TABLE IF NOT EXISTS main.DefaultInt32ValueTestObject(defaultValueVariable INTEGER DEFAULT 1)"
        )

        ORMConstraintBindingAssertEqual(
            DefaultInt64ValueTestObject.self,
            "CREATE TABLE IF NOT EXISTS main.DefaultInt64ValueTestObject(defaultValueVariable INTEGER DEFAULT 1)"
        )

        ORMConstraintBindingAssertEqual(
            DefaultFloatValueTestObject.self,
            "CREATE TABLE IF NOT EXISTS main.DefaultFloatValueTestObject(defaultValueVariable INTEGER DEFAULT 1.2)"
        )

        ORMConstraintBindingAssertEqual(
            DefaultTextValueTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS main.DefaultTextValueTestObject\
            (defaultValueVariable INTEGER DEFAULT 'defaultString')
            """
        )

        ORMConstraintBindingAssertEqual(
            DefaultBLOBValueTestObject.self,
            "CREATE TABLE IF NOT EXISTS main.DefaultBLOBValueTestObject(defaultValueVariable INTEGER DEFAULT 'defaultData')"
        )
    }
}
