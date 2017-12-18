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

protocol Selectable: CoreRepresentable {
    var statement: StatementSelect {get}

    func lazyCoreStatement() throws -> CoreStatement

    func finalize() throws

    @discardableResult
    func `where`(_ condition: Condition) -> Self

    @discardableResult
    func order(by orderConvertibleList: OrderBy...) -> Self

    @discardableResult
    func order(by orderConvertibleList: [OrderBy]) -> Self

    @discardableResult
    func limit(from expressionConvertibleFrom: Limit, to expressionConvertibleTo: Limit) -> Self

    @discardableResult
    func limit(_ expressionConvertibleLimit: Limit) -> Self

    @discardableResult
    func limit(_ expressionConvertibleLimit: Limit, offset expressionConvertibleOffset: Offset) -> Self

    @discardableResult
    func group(by expressionConvertibleGroupList: GroupBy...) -> Self

    @discardableResult
    func group(by expressionConvertibleGroupList: [GroupBy]) -> Self

    @discardableResult
    func having(_ expressionConvertibleHaving: Having) -> Self

    @discardableResult
    func next() throws -> Bool
}

extension Selectable {
    /// WINQ interface
    ///
    /// - Parameter condition: Expression convertible
    /// - Returns: `self`
    @discardableResult
    public func `where`(_ condition: Condition) -> Self {
        statement.where(condition)
        return self
    }

    @discardableResult
    public func order(by orderConvertibleList: OrderBy...) -> Self {
        return order(by: orderConvertibleList)
    }

    @discardableResult
    public func order(by orderConvertibleList: [OrderBy]) -> Self {
        statement.order(by: orderConvertibleList)
        return self
    }

    @discardableResult
    public func limit(from expressionConvertibleFrom: Limit, to expressionConvertibleTo: Limit) -> Self {
        statement.limit(from: expressionConvertibleFrom, to: expressionConvertibleTo)
        return self
    }

    @discardableResult
    public func limit(_ expressionConvertibleLimit: Limit) -> Self {
        statement.limit(expressionConvertibleLimit)
        return self
    }

    @discardableResult
    public func limit(_ expressionConvertibleLimit: Limit, offset expressionConvertibleOffset: Offset) -> Self {
        statement.limit(expressionConvertibleLimit, offset: expressionConvertibleOffset)
        return self
    }

    @discardableResult
    public func group(by expressionConvertibleGroupList: GroupBy...) -> Self {
        return group(by: expressionConvertibleGroupList)
    }

    @discardableResult
    public func group(by expressionConvertibleGroupList: [GroupBy]) -> Self {
        statement.group(by: expressionConvertibleGroupList)
        return self
    }

    @discardableResult
    public func having(_ expressionConvertibleHaving: Having) -> Self {
        statement.having(expressionConvertibleHaving)
        return self
    }

    //Since `next()` may throw errors, it can't conform to `Sequence` protocol to fit a `for in` loop.
    @discardableResult
    public func next() throws -> Bool {
        do {
            return try lazyCoreStatement().step()
        } catch let error {
            try? finalize()
            throw error
        }
    }
}
