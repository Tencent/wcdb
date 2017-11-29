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

class IndexBindingTests: BaseTestCase {

    class BaselineTestObject: WCDB.CodableTable {
        var variable: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\BaselineTestObject.variable, alias: "variable")]
        }
        static func indexBindings() -> [IndexBinding]? {
            return [IndexBinding(withSubfix: "_index", indexesBy: \BaselineTestObject.variable)]
        }
    }
    
    class UniqueTestObject: WCDB.CodableTable {
        var variable: Int = 0        
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [ColumnBinding(\UniqueTestObject.variable, alias: "variable")]
        }
        static func indexBindings() -> [IndexBinding]? {
            return [IndexBinding(withSubfix: "_index", isUnique: true, indexesBy: \UniqueTestObject.variable)]
        }
    }

    class MultiIndexesTestObject: WCDB.CodableTable {
        var variable1: Int = 0 
        var variable2: Int = 0 
        required init() {}        
        static func columnBindings() -> [AnyColumnBinding] {
            return [
                ColumnBinding(\MultiIndexesTestObject.variable1, alias: "variable1"),
                ColumnBinding(\MultiIndexesTestObject.variable2, alias: "variable2")
            ]
        }
        static func indexBindings() -> [IndexBinding]? {
            return [IndexBinding(withSubfix: "_index", indexesBy: \MultiIndexesTestObject.variable1, \MultiIndexesTestObject.variable2)]
        }
    }

    func testIndexBinding() {
        ORMIndexBindingAssertEqual(BaselineTestObject.self, "CREATE INDEX IF NOT EXISTS BaselineTestObject_index ON BaselineTestObject(variable)")
        
        ORMIndexBindingAssertEqual(UniqueTestObject.self, "CREATE UNIQUE INDEX IF NOT EXISTS UniqueTestObject_index ON UniqueTestObject(variable)")
        
        ORMIndexBindingAssertEqual(MultiIndexesTestObject.self, "CREATE INDEX IF NOT EXISTS MultiIndexesTestObject_index ON MultiIndexesTestObject(variable1, variable2)")
    }
}
