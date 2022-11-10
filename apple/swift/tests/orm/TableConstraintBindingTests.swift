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

class TableConstraintBindingTests: BaseTestCase {

    class BaselineMultiPrimaryTestObject: TableCodable, Named {
        var variable1: Int = 0
        var variable2: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineMultiPrimaryTestObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindMultiPrimary(.variable1, .variable2)
            }
        }
    }

    class MultiPrimaryConflictTestObject: TableCodable, Named {
        var variable1: Int = 0
        var variable2: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = MultiPrimaryConflictTestObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindMultiPrimary(.variable1, .variable2, onConflict: .Replace)
            }
        }
    }

    func testMultiPrimaryBinding() {
        ORMConstraintBindingAssertEqual(
            BaselineMultiPrimaryTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS main.BaselineMultiPrimaryTestObject\
            (variable1 INTEGER, variable2 INTEGER, \
            PRIMARY KEY(variable1, variable2))
            """
        )

        ORMConstraintBindingAssertEqual(
            MultiPrimaryConflictTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS main.MultiPrimaryConflictTestObject\
            (variable1 INTEGER, variable2 INTEGER, \
            PRIMARY KEY(variable1, variable2) ON CONFLICT REPLACE)
            """
        )
    }

    class BaselineMultiUniqueTestObject: TableCodable, Named {
        var variable1: Int = 0
        var variable2: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineMultiUniqueTestObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindMultiUnique(variable1, variable2)
            }
        }
    }

    class MultiUniqueConflictTestObject: TableCodable, Named {
        var variable1: Int = 0
        var variable2: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = MultiUniqueConflictTestObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindMultiUnique(.variable1, .variable2, onConflict: .Replace)
            }
        }
    }
    func testMultiUniqueBinding() {
        ORMConstraintBindingAssertEqual(
            BaselineMultiUniqueTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS main.BaselineMultiUniqueTestObject\
            (variable1 INTEGER, \
            variable2 INTEGER, \
            UNIQUE(variable1, variable2))
            """
        )

        ORMConstraintBindingAssertEqual(
            MultiUniqueConflictTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS main.MultiUniqueConflictTestObject\
            (variable1 INTEGER, variable2 INTEGER, \
            UNIQUE(variable1, variable2) ON CONFLICT REPLACE)
            """
        )
    }

    class CheckTestObject: TableCodable, Named {
        var variable1: Int = 0
        var variable2: Int = 0
        static let constraintName = CheckTestObject.name + "Constraint"
        enum CodingKeys: String, CodingTableKey {
            typealias Root = CheckTestObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindChecks {
                    variable1 > 1
                }
            }
        }
    }
    func testCheckBinding() {
        ORMConstraintBindingAssertEqual(
            CheckTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS main.CheckTestObject\
            (variable1 INTEGER, \
            variable2 INTEGER, \
            CHECK(variable1 > 1))
            """
        )
    }

    class ForeignKeyTestObject: TableCodable, Named {
        var variable1: Int = 0
        var variable2: Int = 0
        enum CodingKeys: String, CodingTableKey {
            typealias Root = ForeignKeyTestObject
            case variable1
            case variable2
            static let objectRelationalMapping = TableBinding(CodingKeys.self) {
                BindForeginKey(variable1, foreignKey: ForeignKey().references(with: ForeignKeyTestObject.name).columns(variable2))
            }
        }
    }
    func testForeignKeyBinding() {
        ORMConstraintBindingAssertEqual(
            ForeignKeyTestObject.self,
            """
            CREATE TABLE IF NOT EXISTS main.ForeignKeyTestObject\
            (variable1 INTEGER, \
            variable2 INTEGER, \
            FOREIGN KEY(variable1) REFERENCES ForeignKeyTestObject(variable2))
            """
        )
    }

}
