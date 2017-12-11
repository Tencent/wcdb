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

public class RowSelect: SelectBase {
    init(with core: Core,
         results columnResultConvertibleList: [ColumnResultConvertible],
         tables: [String],
         isDistinct: Bool) throws {
        guard columnResultConvertibleList.count > 0 else {
            throw Error.reportInterface(tag: core.tag,
                                        path: core.path,
                                        operation: .select,
                                        code: .misuse,
                                        message: "Selecting nothing from \(tables) is invalid")
        }
        guard tables.count > 0 else {
            throw Error.reportInterface(tag: core.tag,
                                        path: core.path,
                                        operation: .select,
                                        code: .misuse,
                                        message: "Empty table")
        }
        super.init(with: core)
        statement.select(distinct: isDistinct, columnResultConvertibleList).from(tables)
    }

    private func extract(atIndex index: Int) throws -> FundamentalValue {
        let coreStatement = try self.lazyCoreStatement()
        switch coreStatement.columnType(atIndex: index) {
        case .integer32:
            let value: Int32 = coreStatement.value(atIndex: index) ?? 0
            return FundamentalValue(value)
        case .integer64:
            let value: Int64 = coreStatement.value(atIndex: index) ?? 0
            return FundamentalValue(value)
        case .float:
            let value: Double = coreStatement.value(atIndex: index)  ?? 0
            return FundamentalValue(value)
        case .text:
            let value: String = coreStatement.value(atIndex: index)  ?? ""
            return FundamentalValue(value)
        case .BLOB:
            let value: Data = coreStatement.value(atIndex: index)  ?? Data()
            return FundamentalValue(value)
        case .null:
            return nil
        }
    }

    private func extract() throws -> FundamentalRow {
        var row: FundamentalRow = []
        let coreStatement = try self.lazyCoreStatement()
        for index in 0..<coreStatement.columnCount() {
            row.append(try extract(atIndex: index))
        }
        return row
    }

    public func nextRow() throws -> FundamentalRow? {
        guard try next() else {
            return nil
        }
        return try extract()
    }

    public func allRows() throws -> FundamentalRowXColumn {
        var rows: [[FundamentalValue]] = []
        while try next() {
            rows.append(try extract())
        }
        return rows
    }

    public func nextValue() throws -> FundamentalValue? {
        guard try next() else {
            return nil
        }
        return try extract(atIndex: 0)
    }

    public func allValues() throws -> FundamentalColumn {
        var values: [FundamentalValue] = []
        while try next() {
            values.append(try extract(atIndex: 0))
        }
        return values
    }
}
