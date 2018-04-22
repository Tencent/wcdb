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
public final class StatementDelete: Statement {
    public private(set) var description: String = ""
    public var statementType: StatementType {
        return .delete
    }

    public init() {}

    @discardableResult
    public func delete(fromTable table: String) -> StatementDelete {
        description.append("DELETE FROM \(table)")
        return self
    }

    @discardableResult
    public func `where`(_ expressionConvertible: ExpressionConvertible) -> StatementDelete {
        let expression = expressionConvertible.asExpression()
        if !expression.description.isEmpty {
            description.append(" WHERE \(expression.description)")
        }
        return self
    }

    @discardableResult
    public func order(by orderConvertibleList: OrderConvertible...) -> StatementDelete {
        return order(by: orderConvertibleList)
    }

    @discardableResult
    public func order(by orderConvertibleList: [OrderConvertible]) -> StatementDelete {
        if !orderConvertibleList.isEmpty {
            description.append(" ORDER BY \(orderConvertibleList.joined())")
        }
        return self
    }

    @discardableResult
    public func limit(from expressionConvertibleFrom: ExpressionConvertible,
                      to expressionConvertibleTo: ExpressionConvertible) -> StatementDelete {
        let from = expressionConvertibleFrom.asExpression()
        if !from.description.isEmpty {
            description.append(" LIMIT \(from.description)")
            let to = expressionConvertibleTo.asExpression()
            if !to.description.isEmpty {
                description.append(", \(to.description)")
            }
        }
        return self
    }

    @discardableResult
    public func limit(_ expressionConvertibleLimit: ExpressionConvertible) -> StatementDelete {
        let limit = expressionConvertibleLimit.asExpression()
        if !limit.description.isEmpty {
            description.append(" LIMIT \(limit.description)")
        }
        return self
    }

    @discardableResult
    public func limit(_ expressionConvertibleLimit: ExpressionConvertible,
                      offset expressionConvertibleOffset: ExpressionConvertible) -> StatementDelete {
        let limit = expressionConvertibleLimit.asExpression()
        if !limit.description.isEmpty {
            description.append(" LIMIT \(limit.description)")
            let offset = expressionConvertibleOffset.asExpression()
            if !offset.description.isEmpty {
                description.append(" OFFSET \(offset.description)")
            }
        }
        return self
    }
}
