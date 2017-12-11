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

public class Update: CoreRepresentable {
    var core: Core
    private let statement = StatementUpdate()
    private let keys: [CodingTableKeyBase]
    public var changes: Int = Int.max

    init(with core: Core, on propertyConvertibleList: [PropertyConvertible], andTable table: String) throws {
        guard propertyConvertibleList.count > 0 else {
            throw Error.reportInterface(tag: core.tag,
                                        path: core.path,
                                        operation: .update,
                                        code: .misuse,
                                        message: "Updating \(table) with empty property")
        }
        guard table.count > 0 else {
            throw Error.reportInterface(tag: core.tag,
                                        path: core.path,
                                        operation: .update,
                                        code: .misuse,
                                        message: "Nil table name")
        }
        self.keys = propertyConvertibleList.asCodingTableKeys()
        self.core = core
        self.statement
            .update(table: table)
            .set(propertyConvertibleList.map { (propertyConvertible) -> StatementUpdate.ValueType in
            return (propertyConvertible, Expression.bindingParameter)
        })
    }

    @discardableResult
    public func `where`(_ condition: Condition) -> Update {
        statement.where(condition)
        return self
    }

    @discardableResult
    public func order(by orderList: OrderBy...) -> Update {
        statement.order(by: orderList)
        return self
    }

    @discardableResult
    public func order(by orderList: [OrderBy]) -> Update {
        statement.order(by: orderList)
        return self
    }

    @discardableResult
    public func limit(from begin: Limit, to end: Limit) -> Update {
        statement.limit(from: begin, to: end)
        return self
    }

    @discardableResult
    public func limit(_ limit: Limit) -> Update {
        statement.limit(limit)
        return self
    }

    @discardableResult
    public func limit(_ limit: Limit, offset: Offset) -> Update {
        statement.limit(limit, offset: offset)
        return self
    }

    public func execute<Object: TableEncodable>(with object: Object) throws {
        let coreStatement = try core.prepare(statement)
        let encoder = TableEncoder(keys, on: coreStatement)
        try encoder.bind(object)
        try coreStatement.step()
        changes = coreStatement.changes
    }

    public func execute(with row: [ColumnEncodableBase?]) throws {
        let coreStatement = try core.prepare(statement)
        for (index, value) in row.enumerated() {
            let bindingIndex = index + 1
            coreStatement.bind(value, toIndex: bindingIndex)
        }
        try coreStatement.step()
        changes = coreStatement.changes
    }
}
