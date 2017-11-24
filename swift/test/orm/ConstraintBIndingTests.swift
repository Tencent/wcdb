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

class ConstraintTests: XCTestCase {

    class BaselineMultiPrimaryTestObject: WCDB.CodableTable {
        var variable1: Int = 0        
        var variable2: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\BaselineMultiPrimaryTestObject.variable1, alias: "variable1"),
                ColumnBinding(\BaselineMultiPrimaryTestObject.variable2, alias: "variable2")
            ]
        }
        static let constraintName = String(describing: BaselineMultiPrimaryTestObject.self)+"Constraint"
        static func constraintBindings() -> [ConstraintBinding]? {
            return [MultiPrimaryBinding(named: constraintName, indexesBy: \BaselineMultiPrimaryTestObject.variable1, \BaselineMultiPrimaryTestObject.variable2)]
        }
    }
    
    class MultiPrimaryConflictTestObject: WCDB.CodableTable {
        var variable1: Int = 0        
        var variable2: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\MultiPrimaryConflictTestObject.variable1, alias: "variable1"),
                ColumnBinding(\MultiPrimaryConflictTestObject.variable2, alias: "variable2")
            ]
        }
        static let constraintName = String(describing: MultiPrimaryConflictTestObject.self)+"Constraint"
        static func constraintBindings() -> [ConstraintBinding]? {
            return [MultiPrimaryBinding(named: constraintName, indexesBy: \MultiPrimaryConflictTestObject.variable1, \MultiPrimaryConflictTestObject.variable2, onConflict: .Replace)]
        }
    }
    
    class MultiPrimaryCheckTestObject: WCDB.CodableTable {
        var variable1: Int = 0        
        var variable2: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\MultiPrimaryCheckTestObject.variable1, alias: "variable1"),
                ColumnBinding(\MultiPrimaryCheckTestObject.variable2, alias: "variable2")
            ]
        }
        static let constraintName = String(describing: MultiPrimaryCheckTestObject.self)+"Constraint"
        static func constraintBindings() -> [ConstraintBinding]? {
            return [MultiPrimaryBinding(named: constraintName, indexesBy: \MultiPrimaryCheckTestObject.variable1, \MultiPrimaryCheckTestObject.variable2, check: \MultiPrimaryCheckTestObject.variable1 > 1)]
        }
    }

    func testMultiPrimaryBinding() {
        ORMColumnBindingAssertEqual(BaselineMultiPrimaryTestObject.self, "CREATE TABLE IF NOT EXISTS BaselineMultiPrimaryTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT BaselineMultiPrimaryTestObjectConstraint PRIMARY KEY(variable1, variable2))")
        
        ORMColumnBindingAssertEqual(MultiPrimaryConflictTestObject.self, "CREATE TABLE IF NOT EXISTS MultiPrimaryConflictTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT MultiPrimaryConflictTestObjectConstraint PRIMARY KEY(variable1, variable2) ON CONFLICT REPLACE)")

        ORMColumnBindingAssertEqual(MultiPrimaryCheckTestObject.self, "CREATE TABLE IF NOT EXISTS MultiPrimaryCheckTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT MultiPrimaryCheckTestObjectConstraint PRIMARY KEY(variable1, variable2) CHECK(variable1 > 1))")
    }
    
    class BaselineMultiUniqueTestObject: WCDB.CodableTable {
        var variable1: Int = 0        
        var variable2: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\BaselineMultiUniqueTestObject.variable1, alias: "variable1"),
                ColumnBinding(\BaselineMultiUniqueTestObject.variable2, alias: "variable2")
            ]
        }
        static let constraintName = String(describing: BaselineMultiUniqueTestObject.self)+"Constraint"
        static func constraintBindings() -> [ConstraintBinding]? {
            return [MultiUniqueBinding(named: constraintName, indexesBy: \BaselineMultiUniqueTestObject.variable1, \BaselineMultiUniqueTestObject.variable2)]
        }
    }
    
    class MultiUniqueConflictTestObject: WCDB.CodableTable {
        var variable1: Int = 0        
        var variable2: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\MultiUniqueConflictTestObject.variable1, alias: "variable1"),
                ColumnBinding(\MultiUniqueConflictTestObject.variable2, alias: "variable2")
            ]
        }
        static let constraintName = String(describing: MultiUniqueConflictTestObject.self)+"Constraint"
        static func constraintBindings() -> [ConstraintBinding]? {
            return [MultiUniqueBinding(named: constraintName, indexesBy: \MultiUniqueConflictTestObject.variable1, \MultiUniqueConflictTestObject.variable2, onConflict: .Replace)]
        }
    }
    
    class MultiUniqueCheckTestObject: WCDB.CodableTable {
        var variable1: Int = 0        
        var variable2: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\MultiUniqueCheckTestObject.variable1, alias: "variable1"),
                ColumnBinding(\MultiUniqueCheckTestObject.variable2, alias: "variable2")
            ]
        }
        static let constraintName = String(describing: MultiUniqueCheckTestObject.self)+"Constraint"
        static func constraintBindings() -> [ConstraintBinding]? {
            return [MultiUniqueBinding(named: constraintName, indexesBy: \MultiUniqueCheckTestObject.variable1, \MultiUniqueCheckTestObject.variable2, check: \MultiUniqueCheckTestObject.variable1 > 1)]
        }
    }

    func testMultiUniqueBinding() {
        ORMColumnBindingAssertEqual(BaselineMultiUniqueTestObject.self, "CREATE TABLE IF NOT EXISTS BaselineMultiUniqueTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT BaselineMultiUniqueTestObjectConstraint UNIQUE(variable1, variable2))")
        
        ORMColumnBindingAssertEqual(MultiUniqueConflictTestObject.self, "CREATE TABLE IF NOT EXISTS MultiUniqueConflictTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT MultiUniqueConflictTestObjectConstraint UNIQUE(variable1, variable2) ON CONFLICT REPLACE)")
        
        ORMColumnBindingAssertEqual(MultiUniqueCheckTestObject.self, "CREATE TABLE IF NOT EXISTS MultiUniqueCheckTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT MultiUniqueCheckTestObjectConstraint UNIQUE(variable1, variable2) CHECK(variable1 > 1))")       
    }
}
