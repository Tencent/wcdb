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

/// The chain call for updating
public final class Update {
    internal var handle: Handle
    public final let statement = StatementUpdate()
    internal let properties: [PropertyConvertible]
    private lazy var keys: [CodingTableKeyBase] = {
        self.properties.asCodingTableKeys()
    }()

    /// The number of changed rows in the most recent call.
    /// It should be called after executing successfully
    public var changes: Int?

    init(with handle: Handle, on propertyConvertibleList: [PropertyConvertible], andTable table: String) {
        self.properties = propertyConvertibleList
        self.handle = handle
        self.statement.update(table: table)
        var bindIndex: Int32 = 1
        for property in properties {
            self.statement.set(property).to(BindParameter(bindIndex))
            bindIndex += 1
        }
    }

    /// WINQ interface for SQL
    ///
    /// - Parameter condition: Expression convertible
    /// - Returns: `self`
    @discardableResult
    public func `where`(_ condition: Condition) -> Update {
        statement.where(condition)
        return self
    }

    /// WINQ interface for SQL
    ///
    /// - Parameter orderList: Order convertible list
    /// - Returns: `self`
    @discardableResult
    public func order(by orderList: OrderBy...) -> Update {
        statement.order(by: orderList)
        return self
    }

    /// WINQ interface for SQL
    ///
    /// - Parameter orderList: Order convertible list
    /// - Returns: `self`
    @discardableResult
    public func order(by orderList: [OrderBy]) -> Update {
        statement.order(by: orderList)
        return self
    }

    /// WINQ interface for SQL
    ///
    /// - Parameters:
    ///   - begin: Expression convertible
    ///   - end: Expression convertible
    /// - Returns: `self`
    @discardableResult
    public func limit(from begin: Limit, to end: Limit) -> Update {
        statement.limit(from: begin, to: end)
        return self
    }

    /// WINQ interface for SQL
    ///
    /// - Parameter limit: Expression convertible
    /// - Returns: `self`
    @discardableResult
    public func limit(_ limit: Limit) -> Update {
        statement.limit(limit)
        return self
    }

    /// WINQ interface for SQL
    ///
    /// - Parameters:
    ///   - limit: Expression convertible
    ///   - offset: Expression convertible
    /// - Returns: `self`
    @discardableResult
    public func limit(_ limit: Limit, offset: Offset) -> Update {
        statement.limit(limit).offset(offset)
        return self
    }

    /// Execute the update chain call with object.
    ///
    /// - Parameter object: Table encodable object
    /// - Throws: `Error`
    public func execute<Object: TableEncodable>(with object: Object) throws {
        try handle.prepare(statement)
        let encoder = TableEncoder(keys, on: handle)
        try object.encode(to: encoder)
        try handle.step()
        changes = handle.changes
        handle.finalize()
    }

    /// Execute the update chain call with row.
    ///
    /// - Parameter row: Column encodable row
    /// - Throws: `Error`
    public func execute(with row: [ColumnEncodable?]) throws {
        try handle.prepare(statement)
        for (index, value) in row.enumerated() {
            let bindingIndex = index + 1
            if let archivedValue = value?.archivedValue() {
                handle.bind(archivedValue, toIndex: bindingIndex)
            } else {
                handle.bind(nil, toIndex: bindingIndex)
            }
        }
        try handle.step()
        changes = handle.changes
        handle.finalize()
    }
}
