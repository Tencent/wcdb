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

/// Chain call for row-selecting
public final class RowSelect: Selectable {
    init(with handle: Handle,
         results resultColumnConvertibleList: [ResultColumnConvertible],
         tables: [String],
         isDistinct: Bool) {
        let statement = StatementSelect().select(distinct: isDistinct, resultColumnConvertibleList).from(tables)
        super.init(with: handle, statement: statement)
    }

    /// Get next selected row. You can do an iteration using it.
    ///
    ///     while let row = try rowSelect.nextRow() {
    ///         print(row[0].int32Value)
    ///         print(row[1].int64Value)
    ///         print(row[2].doubleValue)
    ///         print(row[3].stringValue)
    ///     }
    ///
    /// - Returns: Array with `Value`. Nil means the end of iteration.
    /// - Throws: `Error`
    public func nextRow() throws -> OneRowValue? {
        try lazyPrepareStatement()
        guard try next() else {
            return nil
        }
        return handle.oneRowValue()
    }

    /// Get all selected row.
    ///
    /// - Returns: Array with `Array<Value>`
    /// - Throws: `Error`
    public func allRows() throws -> MultiRowsValue {
        var rows: [[Value]] = []
        try lazyPrepareStatement()
        while try next() {
            rows.append(handle.oneRowValue())
        }
        return rows
    }

    /// Get next selected value. You can do an iteration using it.
    ///
    ///     while let value = try nextValue() {
    ///         print(value.int32Value)
    ///     }
    ///
    /// - Returns: `Value`. Nil means the end of iteration.
    /// - Throws: `Error`
    public func nextValue() throws -> Value? {
        try lazyPrepareStatement()
        guard try next() else {
            return nil
        }
        return handle.value(atIndex: 0)
    }

    /// Get all selected values.
    ///
    /// - Returns: Array with `Value`.
    /// - Throws: `Error`
    public func allValues() throws -> OneColumnValue {
        try lazyPrepareStatement()
        return try handle.oneColumnValue(atIndex: 0)
    }
}
