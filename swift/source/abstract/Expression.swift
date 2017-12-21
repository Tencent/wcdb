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

public struct Expression: Describable {
    public private(set) var description: String

    public static let bindParameter = Expression(withRaw: "?")

    public init(with columnConvertible: ColumnConvertible) {
        description = columnConvertible.asColumn().description
    }

    public init(with literalValueConvertible: LiteralValueConvertible) {
        description = literalValueConvertible.asLiteralValue().description
    }

    public init(with statementSelect: StatementSelect) {
        description = statementSelect.description
    }

    init(withRaw raw: String) {
        description = raw
    }
}

extension Expression: ExpressibleByNilLiteral {
    public init(nilLiteral value: ()) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByBooleanLiteral {
    public init(booleanLiteral value: Bool) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByIntegerLiteral {
    public init(integerLiteral value: Int) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByFloatLiteral {
    public init(floatLiteral value: Double) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByStringLiteral {
    public init(stringLiteral value: String) {
        self.init(with: LiteralValue(value))
    }
}

extension Expression: ExpressionOperable {
    public static func exists(_ statementSelect: StatementSelect) -> Expression {
        return Expression.operate(prefix: "EXISTS ", operand: statementSelect)
    }
    public static func notExists(_ statementSelect: StatementSelect) -> Expression {
        return Expression.operate(prefix: "NOT EXISTS ", operand: statementSelect)
    }

    public static func combine(_ expressionConvertibleList: ExpressionConvertible...) -> Expression {
        return combine(expressionConvertibleList)
    }
    public static func combine(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression {
        return Expression(withRaw: "(\(expressionConvertibleList.joined()))")
    }

    //Function
    public static func function(named name: String,
                                _ expressions: ExpressionConvertible...,
                                isDistinct: Bool = false) -> Expression {
        return function(named: name, expressions, isDistinct: isDistinct)
    }
    public static func function(named name: String,
                                _ expressions: [ExpressionConvertible],
                                isDistinct: Bool = false) -> Expression {
        return Expression.operate(title: name, infix: isDistinct ? "DISTINCT" : nil, operands: expressions)
    }

    public static func `case`(_ expressionConvertible: ExpressionConvertible,
                              _ flows: (when: ExpressionConvertible, then: ExpressionConvertible)...,
                              `else`: ExpressionConvertible) -> Expression {
        return `case`(expressionConvertible.asExpression(), flows, else: `else`.asExpression())
    }
    public static func `case`(_ `case`: ExpressionConvertible,
                              _ flows: [(when: ExpressionConvertible, then: ExpressionConvertible)],
                              `else`: ExpressionConvertible) -> Expression {
        var descrption = "CASE \(`case`.asExpression().description) "
        descrption.append(flows.joined({ "WHEN \($0.when) THEN \($0.then) " }))
        descrption.append("ELSE \(`else`.asExpression().description) END")
        return Expression(withRaw: descrption)
    }

    public func asExpression() -> Expression {
        return self
    }
}
