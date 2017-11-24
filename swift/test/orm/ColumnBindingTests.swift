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

class ColumnBindingTests: XCTestCase {
    
    class BaselineTestObject: WCDB.CodableTable {
        var variable: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\BaselineTestObject.variable, alias: "variable")]
        }
    }
    
    class OptionalTestObject: WCDB.CodableTable {
        var variable: Int? = nil        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\OptionalTestObject.variable, alias: "optionalVariable")]
        }
    }
    
    class PrimaryTestObject: WCDB.CodableTable {
        var variable: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\PrimaryTestObject.variable, alias: "primaryVariable", isPrimary: true)]
        }
    }

    class OrderedPrimaryTestObject: WCDB.CodableTable {
        var variable: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\OrderedPrimaryTestObject.variable, alias: "orderedPrimaryVariable", isPrimary: true, orderBy: .Ascending)]
        }
    }

    class AutoIncrementPrimaryTestObject: WCDB.CodableTable {
        var variable: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\AutoIncrementPrimaryTestObject.variable, alias: "autoIncrementPrimaryVariable", isPrimary: true, isAutoIncrement: true)]
        }
    }
    class ConflictPrimaryTestObject: WCDB.CodableTable {
        var variable: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\ConflictPrimaryTestObject.variable, alias: "conflictPrimaryVariable", isPrimary: true, onConflict: .Replace)]
        }
    }

    class NotNullTestObject: WCDB.CodableTable {
        var variable: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\NotNullTestObject.variable, alias: "notNullVariable", isNotNull: true)]
        }
    }

    class UniqueTestObject: WCDB.CodableTable {
        var variable: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\UniqueTestObject.variable, alias: "uniqueVariable", isUnique: true)]
        }
    }

    class DefaultValueTestObject: WCDB.CodableTable {
        var variable: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\DefaultValueTestObject.variable, alias: "defaultValueVariable", defaultTo: 1)]
        }
    }
    
    func testColumnBinding() {
        
        ORMColumnBindingAssertEqual(BaselineTestObject.self, "CREATE TABLE IF NOT EXISTS BaselineTestObject(variable INTEGER)")
        
        ORMColumnBindingAssertEqual(OptionalTestObject.self, "CREATE TABLE IF NOT EXISTS OptionalTestObject(optionalVariable INTEGER)")
        
        ORMColumnBindingAssertEqual(PrimaryTestObject.self, "CREATE TABLE IF NOT EXISTS PrimaryTestObject(primaryVariable INTEGER PRIMARY KEY)")
        
        ORMColumnBindingAssertEqual(OrderedPrimaryTestObject.self, "CREATE TABLE IF NOT EXISTS OrderedPrimaryTestObject(orderedPrimaryVariable INTEGER PRIMARY KEY ASC)")
        
        ORMColumnBindingAssertEqual(AutoIncrementPrimaryTestObject.self, "CREATE TABLE IF NOT EXISTS AutoIncrementPrimaryTestObject(autoIncrementPrimaryVariable INTEGER PRIMARY KEY AUTOINCREMENT)")
        
        ORMColumnBindingAssertEqual(ConflictPrimaryTestObject.self, "CREATE TABLE IF NOT EXISTS ConflictPrimaryTestObject(conflictPrimaryVariable INTEGER PRIMARY KEY ON CONFLICT REPLACE)")
        
        ORMColumnBindingAssertEqual(NotNullTestObject.self, "CREATE TABLE IF NOT EXISTS NotNullTestObject(notNullVariable INTEGER NOT NULL)")
        
        ORMColumnBindingAssertEqual(UniqueTestObject.self, "CREATE TABLE IF NOT EXISTS UniqueTestObject(uniqueVariable INTEGER UNIQUE)")
        
        ORMColumnBindingAssertEqual(DefaultValueTestObject.self, "CREATE TABLE IF NOT EXISTS DefaultValueTestObject(defaultValueVariable INTEGER DEFAULT 1)")
    }
}
