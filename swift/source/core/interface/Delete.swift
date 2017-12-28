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

/// Chain call for deleting
public final class Delete {
    private var core: Core
    private let statement = StatementDelete()

    /// The number of changed rows in the most recent call.
    /// It should be called after executing successfully
    public var changes: Int?

    init(with core: Core, andTableName tableName: String) throws {
        guard tableName.count > 0 else {
            throw Error.reportInterface(tag: core.tag,
                                        path: core.path,
                                        operation: .delete,
                                        code: .misuse,
                                        message: "Nil table name")
        }
        statement.delete(fromTable: tableName)
        self.core = core
    }

    /// WINQ interface for SQL
    ///
    /// - Parameter condition: Expression convertible
    /// - Returns: `self`
    @discardableResult
    public func `where`(_ condition: Condition) -> Delete {
        statement.where(condition)
        return self
    }

    /// WINQ interface for SQL
    ///
    /// - Parameter orderList: Order convertible list
    /// - Returns: `self`
    @discardableResult
    public func order(by orderList: OrderBy...) -> Delete {
        return order(by: orderList)
    }

    /// WINQ interface for SQL
    ///
    /// - Parameter orderList: Order convertible list
    /// - Returns: `self`
    @discardableResult
    public func order(by orderList: [OrderBy]) -> Delete {
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
    public func limit(from begin: Limit, to end: Limit) -> Delete {
        statement.limit(from: begin, to: end)
        return self
    }

    /// WINQ interface for SQL
    ///
    /// - Parameter limit: Expression convertible
    /// - Returns: `self`
    @discardableResult
    public func limit(_ limit: Limit) -> Delete {
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
    public func limit(_ limit: Limit, offset: Offset) -> Delete {
        statement.limit(limit, offset: offset)
        return self
    }

    /// Execute the delete chain call.
    ///
    /// - Throws: Error
    public func execute() throws {
        let recyclableHandleStatement: RecyclableHandleStatement = try core.prepare(statement)
        let handleStatement = recyclableHandleStatement.raw
        try handleStatement.step()
        changes = handleStatement.changes
    }
}

extension Delete: CoreRepresentable {

    /// The tag of the related database.
    public var tag: Tag? {
        return core.tag
    }

    /// The path of the related database.
    public var path: String {
        return core.path
    }
}
