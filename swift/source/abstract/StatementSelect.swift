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
public final class StatementSelect: Statement {
    public private(set) var description: String = ""
    public var statementType: StatementType {
        return .select
    }

    public init() {}

    @discardableResult
    public func select(distinct: Bool = false,
                       _ columnResultConvertibleList: ColumnResultConvertible...) -> StatementSelect {
        return select(distinct: distinct, columnResultConvertibleList)
    }

    @discardableResult
    public func select(distinct: Bool = false,
                       _ columnResultConvertibleList: [ColumnResultConvertible]) -> StatementSelect {
        description.append("SELECT ")
        if distinct {
            description.append("DISTINCT ")
        }
        description.append(columnResultConvertibleList.joined())
        return self
    }

    @discardableResult
    public func from(_ tableOrSubqueryConvertibleList: TableOrSubqueryConvertible...) -> StatementSelect {
        return from(tableOrSubqueryConvertibleList)
    }

    @discardableResult
    public func from<T>(_ tableOrSubqueryConvertibleList: T...) -> StatementSelect
        where T : RawRepresentable, T.RawValue == String {
            return from(tableOrSubqueryConvertibleList.flatMap{ Optional($0.rawValue) })
    }

    @discardableResult
    public func from(_ tableOrSubqueryConvertibleList: [TableOrSubqueryConvertible]) -> StatementSelect {
        description.append(" FROM \(tableOrSubqueryConvertibleList.joined())")
        return self
    }

    @discardableResult
    public func from<T>(_ tableOrSubqueryConvertibleList: [T]) -> StatementSelect
        where T : RawRepresentable, T.RawValue == String {
        return from(tableOrSubqueryConvertibleList.flatMap{ Optional($0.rawValue) })
    }

    @discardableResult
    public func `where`(_ expressionConvertible: ExpressionConvertible) -> StatementSelect {
        let expression = expressionConvertible.asExpression()
        if !expression.description.isEmpty {
            description.append(" WHERE \(expression.description)")
        }
        return self
    }

    @discardableResult
    public func order(by orderConvertibleList: OrderConvertible...) -> StatementSelect {
        return order(by: orderConvertibleList)
    }

    @discardableResult
    public func order(by orderConvertibleList: [OrderConvertible]) -> StatementSelect {
        if !orderConvertibleList.isEmpty {
            description.append(" ORDER BY \(orderConvertibleList.joined())")
        }
        return self
    }

    @discardableResult
    public func limit(from expressionConvertibleFrom: ExpressionConvertible,
                      to expressionConvertibleTo: ExpressionConvertible) -> StatementSelect {
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
    public func limit(_ expressionConvertibleLimit: ExpressionConvertible) -> StatementSelect {
        let limit = expressionConvertibleLimit.asExpression()
        if !limit.description.isEmpty {
            description.append(" LIMIT \(limit.description)")
        }
        return self
    }

    @discardableResult
    public func limit(_ expressionConvertibleLimit: ExpressionConvertible,
                      offset expressionConvertibleOffset: ExpressionConvertible) -> StatementSelect {
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

    @discardableResult
    public func group(by expressionConvertibleGroupList: ExpressionConvertible...) -> StatementSelect {
        return group(by: expressionConvertibleGroupList)
    }

    @discardableResult
    public func group(by expressionConvertibleGroupList: [ExpressionConvertible]) -> StatementSelect {
        if !expressionConvertibleGroupList.isEmpty {
            description.append(" GROUP BY \(expressionConvertibleGroupList.joined())")
        }
        return self
    }

    @discardableResult
    public func having(_ expressionConvertibleHaving: ExpressionConvertible) -> StatementSelect {
        let having = expressionConvertibleHaving.asExpression()
        if !having.description.isEmpty {
            description.append(" HAVING \(having.description)")
        }
        return self
    }
}

extension StatementSelect: ExpressionConvertible, TableOrSubqueryConvertible {
    public func asExpression() -> Expression {
        return Expression(with: self)
    }
    public func asTableOrSubquery() -> Subquery {
        return Subquery(with: self)
    }
}
