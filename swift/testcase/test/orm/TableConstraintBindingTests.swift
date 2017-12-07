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

    class BaselineMultiPrimaryTestObject: WCDB.TableCodable, Named {
        let variable1: Int = 0        
        let variable2: Int = 0    
        required init() {}
        static let constraintName = BaselineMultiPrimaryTestObject.name + "Constraint"
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineMultiPrimaryTestObject
            case variable1
            case variable2
            static var __tableConstraintBindings: [TableConstraintBinding.Name:TableConstraintBinding]? {
                return [constraintName:MultiPrimaryBinding(indexesBy: CodingKeys.variable1, CodingKeys.variable2)]
            }
        }
    }
    
    class MultiPrimaryConflictTestObject: WCDB.TableCodable, Named {
        let variable1: Int = 0        
        let variable2: Int = 0      
        required init() {}   
        static let constraintName = MultiPrimaryConflictTestObject.name + "Constraint"
        enum CodingKeys: String, CodingTableKey {
            typealias Root = MultiPrimaryConflictTestObject
            case variable1
            case variable2
            static var __tableConstraintBindings: [TableConstraintBinding.Name:TableConstraintBinding]? {
                return [constraintName:MultiPrimaryBinding(indexesBy: CodingKeys.variable1, CodingKeys.variable2, onConflict: .Replace)]
            }
        }
    }
    
    class MultiPrimaryCheckTestObject: WCDB.TableCodable, Named {
        let variable1: Int = 0        
        let variable2: Int = 0 
        required init() {}
        static let constraintName = MultiPrimaryCheckTestObject.name + "Constraint"
        enum CodingKeys: String, CodingTableKey {
            typealias Root = MultiPrimaryCheckTestObject
            case variable1
            case variable2
            static var __tableConstraintBindings: [TableConstraintBinding.Name:TableConstraintBinding]? {
                return [constraintName:MultiPrimaryBinding(indexesBy: CodingKeys.variable1, CodingKeys.variable2, check: CodingKeys.variable1 > 1)]
            }
        }
    }

    func testMultiPrimaryBinding() {
        ORMColumnConstraintBindingAssertEqual(BaselineMultiPrimaryTestObject.self, "CREATE TABLE IF NOT EXISTS BaselineMultiPrimaryTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT BaselineMultiPrimaryTestObjectConstraint PRIMARY KEY(variable1, variable2))")
        
        ORMColumnConstraintBindingAssertEqual(MultiPrimaryConflictTestObject.self, "CREATE TABLE IF NOT EXISTS MultiPrimaryConflictTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT MultiPrimaryConflictTestObjectConstraint PRIMARY KEY(variable1, variable2) ON CONFLICT REPLACE)")

        ORMColumnConstraintBindingAssertEqual(MultiPrimaryCheckTestObject.self, "CREATE TABLE IF NOT EXISTS MultiPrimaryCheckTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT MultiPrimaryCheckTestObjectConstraint PRIMARY KEY(variable1, variable2) CHECK(variable1 > 1))")
    }
    
    class BaselineMultiUniqueTestObject: WCDB.TableCodable, Named {
        let variable1: Int = 0        
        let variable2: Int = 0    
        required init() {}  
        static let constraintName = BaselineMultiUniqueTestObject.name + "Constraint"
        enum CodingKeys: String, CodingTableKey {
            typealias Root = BaselineMultiUniqueTestObject
            case variable1
            case variable2
            static var __tableConstraintBindings: [TableConstraintBinding.Name:TableConstraintBinding]? {
                return [constraintName:MultiUniqueBinding(indexesBy: CodingKeys.variable1, CodingKeys.variable2)]
            }
        }
    }
    
    class MultiUniqueConflictTestObject: WCDB.TableCodable, Named {
        let variable1: Int = 0        
        let variable2: Int = 0    
        required init() {}
        static let constraintName = MultiUniqueConflictTestObject.name + "Constraint"
        enum CodingKeys: String, CodingTableKey {
            typealias Root = MultiUniqueConflictTestObject
            case variable1
            case variable2
            static var __tableConstraintBindings: [TableConstraintBinding.Name:TableConstraintBinding]? {
                return [constraintName:MultiUniqueBinding(indexesBy: CodingKeys.variable1, CodingKeys.variable2, onConflict: .Replace)]
            }
        }
    }
    
    class MultiUniqueCheckTestObject: WCDB.TableCodable, Named {
        let variable1: Int = 0        
        let variable2: Int = 0     
        required init() {}  
        static let constraintName = MultiUniqueCheckTestObject.name + "Constraint"
        enum CodingKeys: String, CodingTableKey {
            typealias Root = MultiUniqueCheckTestObject
            case variable1
            case variable2
            static var __tableConstraintBindings: [TableConstraintBinding.Name:TableConstraintBinding]? {
                return [constraintName:MultiUniqueBinding(indexesBy: CodingKeys.variable1, CodingKeys.variable2, check: CodingKeys.variable1 > 1)]
            }
        }
    }

    func testMultiUniqueBinding() {
        ORMColumnConstraintBindingAssertEqual(BaselineMultiUniqueTestObject.self, "CREATE TABLE IF NOT EXISTS BaselineMultiUniqueTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT BaselineMultiUniqueTestObjectConstraint UNIQUE(variable1, variable2))")
        
        ORMColumnConstraintBindingAssertEqual(MultiUniqueConflictTestObject.self, "CREATE TABLE IF NOT EXISTS MultiUniqueConflictTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT MultiUniqueConflictTestObjectConstraint UNIQUE(variable1, variable2) ON CONFLICT REPLACE)")
        
        ORMColumnConstraintBindingAssertEqual(MultiUniqueCheckTestObject.self, "CREATE TABLE IF NOT EXISTS MultiUniqueCheckTestObject(variable1 INTEGER, variable2 INTEGER, CONSTRAINT MultiUniqueCheckTestObjectConstraint UNIQUE(variable1, variable2) CHECK(variable1 > 1))")       
    }
}
