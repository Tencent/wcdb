//
// Created by qiuwenchen on 2022/11/10.
//

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
import WCDB_Private

public final class ColumnConstraintConfig<CodingTableKeyType: CodingTableKey>: TableConfiguration {
    let codingKey: CodingTableKeyType
    private var columnConstraints: [ColumnConstraint] = []
    private let enableAutoIncrementForExistingTable: Bool

    public func config(with tableBinding: TableBindingBase) {
        for constraint in columnConstraints {
            WCDBBindingAddColumnConstraint(tableBinding.cppBinding, codingKey.rawValue.cString, constraint.cppObj)
        }
        if enableAutoIncrementForExistingTable {
            WCDBBindingEnableAutoIncrementForExistingTable(tableBinding.cppBinding)
        }
    }

    required public init(_ codingKey: CodingTableKeyType,
                isPrimary: Bool = false,
                orderBy term: Order? = nil,
                isAutoIncrement: Bool = false,
                enableAutoIncrementForExistingTable: Bool = false,
                onConflict conflict: ConflictAction? = nil,
                isNotNull: Bool = false,
                isUnique: Bool = false,
                defaultTo defaultValue: LiteralValue? = nil,
                isNotIndexed: Bool = false) {
        self.codingKey = codingKey
        if isPrimary {
            let constrant = ColumnConstraint()
            constrant.primaryKey()
            if let term = term {
                constrant.order(term)
            }
            if isAutoIncrement {
                constrant.autoIncrement()
            }
            if let conflict = conflict {
                constrant.conflict(action: conflict)
            }
            columnConstraints.append(constrant)
        }
        if isNotNull {
            columnConstraints.append(ColumnConstraint().notNull())
        }
        if isUnique {
            columnConstraints.append(ColumnConstraint().unique())
        }
        if let defaultValue = defaultValue {
            columnConstraints.append(ColumnConstraint().default(with: defaultValue))
        }
        if isNotIndexed {
            columnConstraints.append(ColumnConstraint().unIndexed())
        }
        self.enableAutoIncrementForExistingTable = enableAutoIncrementForExistingTable
    }

    public convenience init<T: ColumnEncodable>(
        _ codingKey: CodingTableKeyType,
        isPrimary: Bool = false,
        orderBy term: Order? = nil,
        isAutoIncrement: Bool = false,
        enableAutoIncrementForExistingTable: Bool = false,
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
        self.init(codingKey,
                  isPrimary: isPrimary,
                  orderBy: term,
                  isAutoIncrement: isAutoIncrement,
                  onConflict: conflict,
                  isNotNull: isNotNull,
                  isUnique: isUnique,
                  defaultTo: defaultValue,
                  isNotIndexed: isNotIndexed)
    }
}
