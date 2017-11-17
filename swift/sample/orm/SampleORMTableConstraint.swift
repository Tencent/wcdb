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

class SampleORMTableConstraint: WCDB.CodableTable {
    var primaryKeyPart1: Int? = nil
    var primaryKeyPart2: String? = nil
    
    var uniqueKeyPart1: Int? = nil
    var uniqueKeyPart2: Float? = nil
    
    required init() {}

    //WCDB
    static var objectRelationalMapping: TableBinding = TableBinding(SampleORMTableConstraint.self)

    var isAutoIncrement: Bool = false
    var lastInsertedRowID: Int64 = 0
}

//WCDB
extension SampleORMTableConstraint {
    static func columnBindings() -> [AnyColumnBinding] {
        return [
            ColumnBinding(\SampleORMTableConstraint.primaryKeyPart1),
            ColumnBinding(\SampleORMTableConstraint.primaryKeyPart2),
            ColumnBinding(\SampleORMTableConstraint.uniqueKeyPart1),
            ColumnBinding(\SampleORMTableConstraint.uniqueKeyPart2)]
    }
    
    static func constraintBindings() -> [ConstraintBinding]? {
        return [
            MultiUniqueBinding(named: "MultiPrimaryConstraint",
                               indexesBy: (\SampleORMTableConstraint.primaryKeyPart1).asIndex(orderBy: .Descending), \SampleORMTableConstraint.primaryKeyPart2),
            MultiPrimaryBinding(named: "MultiUniqueConstraint",
                                indexesBy: \SampleORMTableConstraint.uniqueKeyPart1, (\SampleORMTableConstraint.uniqueKeyPart2).asIndex(orderBy: .Ascending))]
    }
}
