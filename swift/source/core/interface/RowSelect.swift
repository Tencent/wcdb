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

/// Chain call for row-selecting
public final class RowSelect: Selectable {
    init(with core: Core,
         results columnResultConvertibleList: [ColumnResultConvertible],
         tables: [String],
         isDistinct: Bool) throws {
        guard tables.count > 0 else {
            throw Error.reportInterface(tag: core.tag,
                                        path: core.path,
                                        operation: .select,
                                        code: .misuse,
                                        message: "Empty table")
        }
        let statement = StatementSelect().select(distinct: isDistinct, columnResultConvertibleList).from(tables)
        super.init(with: core, statement: statement)
    }

    private func extract(atIndex index: Int) throws -> FundamentalValue {
        let handleStatement = try self.lazyHandleStatement()
        switch handleStatement.columnType(atIndex: index) {
        case .integer32:
            return FundamentalValue(handleStatement.columnValue(atIndex: index, of: Int32.self))
        case .integer64:
            return FundamentalValue(handleStatement.columnValue(atIndex: index, of: Int64.self))
        case .float:
            return FundamentalValue(handleStatement.columnValue(atIndex: index, of: Double.self))
        case .text:
            return FundamentalValue(handleStatement.columnValue(atIndex: index, of: String.self))
        case .BLOB:
            return FundamentalValue(handleStatement.columnValue(atIndex: index, of: Data.self))
        case .null:
            return FundamentalValue(nil)
        }
    }

    private func extract() throws -> FundamentalRow {
        var row: FundamentalRow = []
        let handleStatement = try self.lazyHandleStatement()
        for index in 0..<handleStatement.columnCount() {
            row.append(try extract(atIndex: index))
        }
        return row
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
    /// - Returns: Array with `FundamentalValue`. Nil means the end of iteration.
    /// - Throws: `Error`
    public func nextRow() throws -> FundamentalRow? {
        guard try next() else {
            return nil
        }
        return try extract()
    }

    /// Get all selected row.
    ///
    /// - Returns: Array with `Array<FundamentalValue>`
    /// - Throws: `Error`
    public func allRows() throws -> FundamentalRowXColumn {
        var rows: [[FundamentalValue]] = []
        while try next() {
            rows.append(try extract())
        }
        return rows
    }

    /// Get next selected value. You can do an iteration using it.
    ///
    ///     while let value = try nextValue() {
    ///         print(value.int32Value)
    ///     }
    ///
    /// - Returns: `FundamentalValue`. Nil means the end of iteration.
    /// - Throws: `Error`
    public func nextValue() throws -> FundamentalValue? {
        guard try next() else {
            return nil
        }
        return try extract(atIndex: 0)
    }

    /// Get all selected values.
    ///
    /// - Returns: Array with `FundamentalValue`.
    /// - Throws: `Error`
    public func allValues() throws -> FundamentalColumn {
        var values: [FundamentalValue] = []
        while try next() {
            values.append(try extract(atIndex: 0))
        }
        return values
    }
}
