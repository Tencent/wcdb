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

import Foundation
import WCDB

class CRUDObject: WCDB.CodableTable, Named, Equatable, Comparable {        
    var variable1: Int = 0
    var variable2: String = ""
    
    required init() {}        
    var isAutoIncrement: Bool = false
    var lastInsertedRowID: Int64 = 0
    static func columnBindings() -> [AnyColumnBinding] {
        return [
            ColumnBinding(\CRUDObject.variable1, alias: "variable1", isPrimary: true, orderBy: .Ascending, isAutoIncrement: true),
            ColumnBinding(\CRUDObject.variable2, alias: "variable2")
        ]
    }
    
    static func ==(lhs: CRUDObject, rhs: CRUDObject) -> Bool {
        return lhs.variable1==rhs.variable1 && lhs.variable2==rhs.variable2
    }
    
    static func <(lhs: CRUDObject, rhs: CRUDObject) -> Bool {
        guard lhs.variable1 != rhs.variable1 else {
            return lhs.variable2 < lhs.variable2
        }
        return lhs.variable1 < rhs.variable1
    }
}
