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
    let defaultValue: LiteralValue?
    let conflict: ConflictAction?
    let isNotNull: Bool
    let isUnique: Bool
    let term: Order?
    let isNotIndexed: Bool

    public init(isPrimary: Bool = false,
                orderBy term: Order? = nil,
                isAutoIncrement: Bool = false,
                onConflict conflict: ConflictAction? = nil,
                isNotNull: Bool = false,
                isUnique: Bool = false,
                defaultTo defaultValue: LiteralValue? = nil,
                isNotIndexed: Bool = false) {
        self.isPrimary = isPrimary
        self.isAutoIncrement = isAutoIncrement
        self.isNotNull = isNotNull
        self.isUnique = isUnique
        self.defaultValue = defaultValue
        self.term = term
        self.conflict = conflict
        self.isNotIndexed = isNotIndexed
    }

    public init<T: ColumnEncodable>(
        isPrimary: Bool = false,
        orderBy term: Order? = nil,
        isAutoIncrement: Bool = false,
        onConflict conflict: ConflictAction? = nil,
        isNotNull: Bool = false,
        isUnique: Bool = false,
        defaultTo defaultEncodedValue: T,
        isNotIndexed: Bool = false) {
        var defaultValue: LiteralValue!
        let value = defaultEncodedValue.archivedValue()
        switch T.columnType {
        case .integer32:
            defaultValue = LiteralValue(value.int32Value)
        case .integer64:
            defaultValue = LiteralValue(value.int64Value)
        case .text:
            defaultValue = LiteralValue(value.stringValue)
        case .float:
            defaultValue = LiteralValue(value.doubleValue)
        case .BLOB:
            defaultValue = LiteralValue(value.dataValue)
        case .null:
            defaultValue = LiteralValue(nil)
        }
        self.init(isPrimary: isPrimary,
                  orderBy: term,
                  isAutoIncrement: isAutoIncrement,
                  onConflict: conflict,
                  isNotNull: isNotNull,
                  isUnique: isUnique,
                  defaultTo: defaultValue,
                  isNotIndexed: isNotIndexed)
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
        if let defaultValue = defaultValue {
            columnDef.makeDefault(to: defaultValue)
        }
        if isNotIndexed {
            columnDef.makeNotIndexed()
        }
        return columnDef
    }
}
