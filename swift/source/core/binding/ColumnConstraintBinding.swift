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

public class ColumnConstraintBinding: AnyBinding {
    let isPrimary: Bool
    let isAutoIncrement: Bool
    let defaultValue: ColumnDef.DefaultType?
    let conflict: Conflict?
    let isNotNull: Bool
    let isUnique: Bool
    let term: OrderTerm?
    
    init(isPrimary: Bool = false, orderBy term: OrderTerm? = nil, isAutoIncrement: Bool = false, onConflict conflict: Conflict? = nil, isNotNull: Bool = false, isUnique: Bool = false, defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        self.isPrimary = isPrimary
        self.isAutoIncrement = isAutoIncrement
        self.isNotNull = isNotNull
        self.isUnique = isUnique
        self.defaultValue = defaultValue
        self.term = term
        self.conflict = conflict
        super.init(with: .ColumnConstraint)
    }
    
    func generateColumnDef(with columnConvertible: ColumnConvertible) -> ColumnDef {
        let columnDef = columnConvertible.asDef()
        if isPrimary {
            columnDef.makePrimary(orderBy: term, isAutoIncrement: isAutoIncrement, onConflict: conflict)
        }
        if isNotNull {
            columnDef.makeNotNull()
        }
        if isUnique {
            columnDef.makeUnique()
        }
        if defaultValue != nil {
            columnDef.makeDefault(to: defaultValue!)
        }
        return columnDef
    }
}

