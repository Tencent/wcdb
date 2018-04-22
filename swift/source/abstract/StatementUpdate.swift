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
public final class StatementUpdate: Statement {
    public private(set) var description: String = ""
    public var statementType: StatementType {
        return .update
    }

    public init() {}

    @discardableResult
    public func update(table: String, onConflict conflict: Conflict? = nil) -> StatementUpdate {
        description.append("UPDATE ")
        if conflict != nil {
            description.append("OR \(conflict!.description) ")
        }
        description.append(table)
        return self
    }

    public typealias ValueType = (_: ColumnConvertible, _: ExpressionConvertible)

    @discardableResult
    public func set(_ values: [ValueType]) -> StatementUpdate {
        let valueString = values.joined({
            $0.0.asColumn().description + "=" + $0.1.asExpression().description
        })
        description.append(" SET \(valueString)")
        return self
    }

    @discardableResult
    public func set(_ values: ValueType...) -> StatementUpdate {
        return set(values)
    }

    @discardableResult
    public func `where`(_ expressionConvertible: ExpressionConvertible) -> StatementUpdate {
        let expression = expressionConvertible.asExpression()
        if !expression.description.isEmpty {
            description.append(" WHERE \(expression.description)")
        }
        return self
    }

    @discardableResult
    public func order(by orderConvertibleList: OrderConvertible...) -> StatementUpdate {
        return order(by: orderConvertibleList)
    }

    @discardableResult
    public func order(by orderConvertibleList: [OrderConvertible]) -> StatementUpdate {
        if !orderConvertibleList.isEmpty {
            description.append(" ORDER BY \(orderConvertibleList.joined())")
        }
        return self
    }

    @discardableResult
    public func limit(from expressionConvertibleFrom: ExpressionConvertible,
                      to expressionConvertibleTo: ExpressionConvertible) -> StatementUpdate {
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
    public func limit(_ expressionConvertibleLimit: ExpressionConvertible) -> StatementUpdate {
        let limit = expressionConvertibleLimit.asExpression()
        if !limit.description.isEmpty {
            description.append(" LIMIT \(limit.description)")
        }
        return self
    }

    @discardableResult
    public func limit(_ expressionConvertibleLimit: ExpressionConvertible,
                      offset expressionConvertibleOffset: ExpressionConvertible) -> StatementUpdate {
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
