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

class SampleORMTableConstraint: WCDB.TableCodable {
    var primaryKeyPart1: Int? = nil
    var primaryKeyPart2: String? = nil
    
    var uniqueKeyPart1: Int? = nil
    var uniqueKeyPart2: Float? = nil
    enum CodingKeys: String, CodingTableKey {
        typealias Root = SampleORMTableConstraint    
        static let __objectRelationalMapping = TableBinding(CodingKeys.self)    
        case primaryKeyPart1    
        case primaryKeyPart2    
        case uniqueKeyPart1     
        case uniqueKeyPart2    
        static var __tableConstraintBindings: [TableConstraintBinding.Name:TableConstraintBinding]? {
            return [
                "MultiPrimaryConstraint":MultiPrimaryBinding(indexesBy: CodingKeys.primaryKeyPart1.asIndex(orderBy: .Descending), CodingKeys.primaryKeyPart2),
                "MultiUniqueConstraint":MultiUniqueBinding(indexesBy: CodingKeys.primaryKeyPart1, CodingKeys.primaryKeyPart2.asIndex(orderBy: .Ascending)),
            ]        
        }
    }
    required init() {}

    var isAutoIncrement: Bool = false
    var lastInsertedRowID: Int64 = 0
}

