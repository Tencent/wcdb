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

class ColumnConstraintBindingTests: BaseTestCase {

    class BaselineTestObject: TableCodable {
        let variable: Int = 0
        required init() {}
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineTestObject
            case variable
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
        }
    }

    class PrimaryTestObject: TableCodable {
        let variable: Int = 0
        required init() {}
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
        let variable: Int = 0
        required init() {}
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
        let variable: Int = 0
        required init() {}
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
        let variable: Int = 0
        required init() {}
        enum CodingKeys: String, CodingTableKey {
            typealias Root = ConflictPrimaryTestObject
            case variable = "conflictPrimaryVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(isPrimary: true, onConflict: .replace)]
            }
        }
    }

    class NotNullTestObject: TableCodable {
        let variable: Int = 0
        required init() {}
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
        let variable: Int = 0
        required init() {}
        enum CodingKeys: String, CodingTableKey {
            typealias Root = UniqueTestObject
            case variable = "uniqueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(isUnique: true)]
            }
        }
    }

    class DefaultValueTestObject: TableCodable {
        let variable: Int = 0
        required init() {}
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultValueTestObject
            case variable = "defaultValueVariable"
            static let objectRelationalMapping = TableBinding(CodingKeys.self)
            static var columnConstraintBindings: [CodingKeys: ColumnConstraintBinding]? {
                return [.variable: ColumnConstraintBinding(defaultTo: 1)]
            }
        }
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
            DefaultValueTestObject.self,
            "CREATE TABLE IF NOT EXISTS DefaultValueTestObject(defaultValueVariable INTEGER DEFAULT 1)"
        )
    }
}
