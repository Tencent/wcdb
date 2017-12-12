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
public final class JoinClause: Describable {
    public private(set) var description: String

    public enum JoinClauseType: Describable {
        case left
        case leftOuter
        case inner
        case cross
        public var description: String {
            switch self {
            case .left:
                return "LEFT"
            case .leftOuter:
                return "LEFT OUTER"
            case .inner:
                return "INNER"
            case .cross:
                return "CROSS"
            }
        }
    }

    public init(with subqueryConvertible: TableOrSubqueryConvertible) {
        description = subqueryConvertible.asTableOrSubquery().description
    }

    @discardableResult
    private func join(_ subqueryConvertible: TableOrSubqueryConvertible,
                      with type: JoinClauseType? = nil,
                      isNatural: Bool = false) -> JoinClause {
        if isNatural {
            description.append(" NATURAL")
        }
        if type != nil {
            description.append(" \(type!.description)")
        }
        description.append(" JOIN \(subqueryConvertible.asTableOrSubquery().description)")
        return self
    }

    @discardableResult
    public func join(_ subqueryConvertible: TableOrSubqueryConvertible,
                     with type: JoinClauseType? = nil) -> JoinClause {
        return self.join(subqueryConvertible, with: type, isNatural: false)
    }

    @discardableResult
    public func natureJoin(_ subqueryConvertible: TableOrSubqueryConvertible,
                           with type: JoinClauseType? = nil) -> JoinClause {
        return self.join(subqueryConvertible, with: type, isNatural: true)
    }

    @discardableResult
    public func on(_ expressionConvertible: ExpressionConvertible) -> JoinClause {
        description.append(" ON \(expressionConvertible.asExpression().description)")
        return self
    }

    @discardableResult
    public func using(_ columnConvertibleList: ColumnConvertible...) -> JoinClause {
        return using(columnConvertibleList)
    }

    @discardableResult
    public func using(_ columnConvertibleList: [ColumnConvertible]) -> JoinClause {
        description.append(" USING \(columnConvertibleList.joined())")
        return self
    }
}

extension JoinClause: TableOrSubqueryConvertible {
    public func asTableOrSubquery() -> Subquery {
        return Subquery(with: self)
    }
}
