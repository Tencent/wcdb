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
    
    class BaselineTestObject: WCDB.TableCodable {
        var variable: Int = 0 
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineTestObject
            case variable
        }
    }
    
    class PrimaryTestObject: WCDB.TableCodable {
        var variable: Int = 0  
        enum CodingKeys: String, CodingTableKey {
            typealias Root = PrimaryTestObject
            case variable = "primaryVariable"
            static var __columnConstraintBindings: [CodingKeys:ColumnConstraintBinding]? {
                return [.variable:ColumnConstraintBinding(isPrimary: true)]
            }
        }
    }

    class OrderedPrimaryTestObject: WCDB.TableCodable {
        var variable: Int = 0        
        enum CodingKeys: String, CodingTableKey {
            typealias Root = OrderedPrimaryTestObject
            case variable = "orderedPrimaryVariable"
            static var __columnConstraintBindings: [CodingKeys:ColumnConstraintBinding]? {
                return [.variable:ColumnConstraintBinding(isPrimary: true, orderBy: .Ascending)]
            }
        }
    }

    class AutoIncrementPrimaryTestObject: WCDB.TableCodable {
        var variable: Int = 0       
        enum CodingKeys: String, CodingTableKey {
            typealias Root = AutoIncrementPrimaryTestObject
            case variable = "autoIncrementPrimaryVariable"
            static var __columnConstraintBindings: [CodingKeys:ColumnConstraintBinding]? {
                return [.variable:ColumnConstraintBinding(isPrimary: true, isAutoIncrement: true)]
            }
        }
    }
    class ConflictPrimaryTestObject: WCDB.TableCodable {
        var variable: Int = 0    
        enum CodingKeys: String, CodingTableKey {
            typealias Root = ConflictPrimaryTestObject
            case variable = "conflictPrimaryVariable"
            static var __columnConstraintBindings: [CodingKeys:ColumnConstraintBinding]? {
                return [.variable:ColumnConstraintBinding(isPrimary: true, onConflict: .Replace)]
            }
        }
    }

    class NotNullTestObject: WCDB.TableCodable {
        var variable: Int = 0        
        enum CodingKeys: String, CodingTableKey {
            typealias Root = NotNullTestObject
            case variable = "notNullVariable"
            static var __columnConstraintBindings: [CodingKeys:ColumnConstraintBinding]? {
                return [.variable:ColumnConstraintBinding(isNotNull: true)]
            }
        }
    }

    class UniqueTestObject: WCDB.TableCodable {
        var variable: Int = 0        
        enum CodingKeys: String, CodingTableKey {
            typealias Root = UniqueTestObject
            case variable = "uniqueVariable"
            static var __columnConstraintBindings: [CodingKeys:ColumnConstraintBinding]? {
                return [.variable:ColumnConstraintBinding(isUnique: true)]
            }
        }
    }

    class DefaultValueTestObject: WCDB.TableCodable {
        var variable: Int = 0        
        enum CodingKeys: String, CodingTableKey {
            typealias Root = DefaultValueTestObject
            case variable = "defaultValueVariable"
            static var __columnConstraintBindings: [CodingKeys:ColumnConstraintBinding]? {
                return [.variable:ColumnConstraintBinding(defaultTo: 1)]
            }
        }
    }
    
    func testColumnConstraintBinding() {
        
        ORMColumnConstraintBindingAssertEqual(BaselineTestObject.self, "CREATE TABLE IF NOT EXISTS BaselineTestObject(variable)")
        
        ORMColumnConstraintBindingAssertEqual(PrimaryTestObject.self, "CREATE TABLE IF NOT EXISTS PrimaryTestObject(primaryVariable PRIMARY KEY)")
        
        ORMColumnConstraintBindingAssertEqual(OrderedPrimaryTestObject.self, "CREATE TABLE IF NOT EXISTS OrderedPrimaryTestObject(orderedPrimaryVariable PRIMARY KEY ASC)")
        
        ORMColumnConstraintBindingAssertEqual(AutoIncrementPrimaryTestObject.self, "CREATE TABLE IF NOT EXISTS AutoIncrementPrimaryTestObject(autoIncrementPrimaryVariable PRIMARY KEY AUTOINCREMENT)")
        
        ORMColumnConstraintBindingAssertEqual(ConflictPrimaryTestObject.self, "CREATE TABLE IF NOT EXISTS ConflictPrimaryTestObject(conflictPrimaryVariable PRIMARY KEY ON CONFLICT REPLACE)")
        
        ORMColumnConstraintBindingAssertEqual(NotNullTestObject.self, "CREATE TABLE IF NOT EXISTS NotNullTestObject(notNullVariable NOT NULL)")
        
        ORMColumnConstraintBindingAssertEqual(UniqueTestObject.self, "CREATE TABLE IF NOT EXISTS UniqueTestObject(uniqueVariable UNIQUE)")
        
        ORMColumnConstraintBindingAssertEqual(DefaultValueTestObject.self, "CREATE TABLE IF NOT EXISTS DefaultValueTestObject(defaultValueVariable DEFAULT 1)")
    }
}
