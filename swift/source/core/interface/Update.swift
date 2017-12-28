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

/// The chain call for updating
public final class Update {
    private var core: Core
    private let statement = StatementUpdate()
    private let keys: [CodingTableKeyBase]

    /// The number of changed rows in the most recent call.
    /// It should be called after executing successfully
    public var changes: Int?

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
            return (propertyConvertible, Expression.bindParameter)
        })
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
        statement.limit(limit, offset: offset)
        return self
    }

    /// Execute the update chain call with object.
    ///
    /// - Parameter object: Table encodable object
    /// - Throws: `Error`
    public func execute<Object: TableEncodable>(with object: Object) throws {
        let recyclableHandleStatement: RecyclableHandleStatement = try core.prepare(statement)
        let handleStatement = recyclableHandleStatement.raw
        let encoder = TableEncoder(keys, on: recyclableHandleStatement)
        try object.encode(to: encoder)
        try handleStatement.step()
        changes = handleStatement.changes
    }

    /// Execute the update chain call with row.
    ///
    /// - Parameter row: Column encodable row
    /// - Throws: `Error`
    public func execute(with row: [ColumnEncodableBase?]) throws {
        let recyclableHandleStatement: RecyclableHandleStatement = try core.prepare(statement)
        let handleStatement = recyclableHandleStatement.raw
        for (index, value) in row.enumerated() {
            let bindingIndex = index + 1
            handleStatement.bind(value?.archivedFundamentalValue(), toIndex: bindingIndex)
        }
        try handleStatement.step()
        changes = handleStatement.changes
    }
}

extension Update: CoreRepresentable {

    /// The tag of the related database.
    public var tag: Tag? {
        return core.tag
    }

    /// The path of the related database.
    public var path: String {
        return core.path
    }
}
