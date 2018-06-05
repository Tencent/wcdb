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

public struct ColumnConstraintBinding {
    let isPrimary: Bool
    let isAutoIncrement: Bool
    let defaultValue: ColumnDef.DefaultType?
    let conflict: Conflict?
    let isNotNull: Bool
    let isUnique: Bool
    let term: OrderTerm?

    public init(isPrimary: Bool = false,
                orderBy term: OrderTerm? = nil,
                isAutoIncrement: Bool = false,
                onConflict conflict: Conflict? = nil,
                isNotNull: Bool = false,
                isUnique: Bool = false,
                defaultTo defaultValue: ColumnDef.DefaultType? = nil) {
        self.isPrimary = isPrimary
        self.isAutoIncrement = isAutoIncrement
        self.isNotNull = isNotNull
        self.isUnique = isUnique
        self.defaultValue = defaultValue
        self.term = term
        self.conflict = conflict
    }

    public init<T: ColumnEncodable>(
        isPrimary: Bool = false,
        orderBy term: OrderTerm? = nil,
        isAutoIncrement: Bool = false,
        onConflict conflict: Conflict? = nil,
        isNotNull: Bool = false,
        isUnique: Bool = false,
        defaultTo defaultEncodedValue: T) {
        var defaultValue: ColumnDef.DefaultType!
        let value = defaultEncodedValue.archivedValue()
        switch T.columnType {
        case .integer32:
            defaultValue = .int32(value.int32Value)
        case .integer64:
            defaultValue = .int64(value.int64Value)
        case .text:
            defaultValue = .text(value.stringValue)
        case .float:
            defaultValue = .float(value.doubleValue)
        case .BLOB:
            defaultValue = .BLOB(value.dataValue)
        case .null:
            defaultValue = .null
        }
        self.init(isPrimary: isPrimary,
                  orderBy: term,
                  isAutoIncrement: isAutoIncrement,
                  onConflict: conflict,
                  isNotNull: isNotNull,
                  isUnique: isUnique,
                  defaultTo: defaultValue)
    }

    func generateColumnDef(with rawColumnDef: ColumnDef) -> ColumnDef {
        let columnDef = rawColumnDef
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
