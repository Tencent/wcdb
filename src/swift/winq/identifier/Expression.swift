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

public final class Expression: Identifier<CPPExpression> {

    public convenience init(with literalValueConvertible: LiteralValueConvertible) {
        let literalValue = literalValueConvertible.asLiteralValue()
        let cppExpression = withExtendedLifetime(literalValue) {
            WCDBExpressionCreateWithLiteralValue($0.cppObj)
        }
        self.init(with: cppExpression)
    }

    public convenience init(with bindParameter: BindParameter) {
        let cppExpression = withExtendedLifetime(bindParameter) {
            WCDBExpressionCreateWithBindParameter($0.cppObj)
        }
        self.init(with: cppExpression)
    }

    public convenience init(with columnConvertible: ColumnConvertible) {
        let column = columnConvertible.asColumn()
        let cppExpression = withExtendedLifetime(column) {
            WCDBExpressionCreateWithColumn($0.cppObj)
        }
        self.init(with: cppExpression)
    }

    public convenience init(with statementSelect: StatementSelect) {
        let cppExpression = withExtendedLifetime(statementSelect) {
            WCDBExpressionCreateWithSelection($0.cppObj)
        }
        self.init(with: cppExpression)
    }

    public convenience init(with raiseFunc: RaiseFunction) {
        let cppExpression = withExtendedLifetime(raiseFunc) {
            WCDBExpressionCreateWithRaiseFunction($0.cppObj)
        }
        self.init(with: cppExpression)
    }

    public static func function(_ name: String) -> Expression {
        Expression(with: WCDBExpressionCreateWithFunction(name.cString))
    }

    @discardableResult
    public func schema(_ schemaConvertible: SchemaConvertible) -> Expression {
        let schema = schemaConvertible.asSchema()
        withExtendedLifetime(schema) {
            WCDBExpressionSetWithSchema(cppObj, schema.cppObj)
        }
        return self
    }

    @discardableResult
    public func distinct() -> Expression {
        WCDBExpressionDistinct(cppObj)
        return self
    }

    @discardableResult
    public func arguments(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression {
        let expressions = expressionConvertibleList.map { $0.asExpression() }
        withExtendedLifetime(expressions) {
            let arguments = $0.map { $0.cppObj }
            arguments.withUnsafeBufferPointer { cppArgs in
                WCDBExpressionSetArguments(cppObj, cppArgs.baseAddress, Int32(cppArgs.count))
            }
        }
        return self
    }

    @discardableResult
    public func arguments(_ expressionConvertibleList: ExpressionConvertible...) -> Expression {
        return self.arguments(expressionConvertibleList)
    }

    @discardableResult
    public func invoke() -> Expression {
        WCDBExpressionInvoke(cppObj)
        return self
    }

    @discardableResult
    public func invokeAll() -> Expression {
        WCDBExpressionInvokeAll(cppObj)
        return self
    }

    public static func cast(_ expressionConvertible: ExpressionConvertible) -> Expression {
        let expression = expressionConvertible.asExpression()
        let cppExpression = withExtendedLifetime(expression) {
            WCDBExpressionCast($0.cppObj)
        }
        return Expression(with: cppExpression)
    }

    @discardableResult
    public func `as`(_ type: ColumnType) -> Expression {
        WCDBExpressionAs(cppObj, type.cValue)
        return self
    }

    public func `as`(_ alias: String) -> ResultColumn {
        return ResultColumn(with: WCDBExpressionConfigAlias(cppObj, alias.cString))
    }

    public static func `case`(_ expressionConvertible: ExpressionConvertible? = nil) -> Expression {
        if let expressionConvertible = expressionConvertible {
            let expression = expressionConvertible.asExpression()
            let cppExpression = withExtendedLifetime(expression) {
                WCDBExpressionCaseWithExp($0.cppObj)
            }
            return Expression(with: cppExpression)
        } else {
            return Expression(with: WCDBExpressionCase())
        }
    }

    @discardableResult
    public func when(_ expressionConvertible: ExpressionConvertible) -> Expression {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBExpressionSetWithWhenExp(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func then(_ expressionConvertible: ExpressionConvertible) -> Expression {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBExpressionSetWithThenExp(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func `else`(_ expressionConvertible: ExpressionConvertible) -> Expression {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBExpressionSetWithElseExp(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func escape(_ expressionConvertible: ExpressionConvertible) -> Expression {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBExpressionEscapeWith(cppObj, $0.cppObj)
        }
        return self
    }

    public static func windowFunction(_ funcName: String) -> Expression {
        return Expression(with: WCDBExpressionCreateWithWindowFunction(funcName.cString))
    }

    @discardableResult
    public func filter(_ expressionConvertible: ExpressionConvertible) -> Expression {
        let expression = expressionConvertible.asExpression()
        withExtendedLifetime(expression) {
            WCDBExpressionFilter(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func over(_ window: WindowDef) -> Expression {
        withExtendedLifetime(window) {
            WCDBExpressionOverWindowDef(cppObj, $0.cppObj)
        }
        return self
    }

    @discardableResult
    public func over(_ window: String) -> Expression {
        WCDBExpressionOverWindow(cppObj, window.cString)
        return self
    }
}

extension Expression: ExpressibleByNilLiteral {
    public convenience init(nilLiteral value: ()) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByBooleanLiteral {
    public convenience init(booleanLiteral value: Bool) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByIntegerLiteral {
    public convenience init(integerLiteral value: Int) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByFloatLiteral {
    public convenience init(floatLiteral value: Double) {
        self.init(with: LiteralValue(value))
    }
}
extension Expression: ExpressibleByStringLiteral {
    public convenience init(stringLiteral value: String) {
        self.init(with: LiteralValue(value))
    }
}

extension Expression: ExpressionOperable {

    public static func exists(_ select: StatementSelect) -> Expression {
        let cppExpression = withExtendedLifetime(select) {
            WCDBExpressionCreateWithExistStatement($0.cppObj)
        }
        return Expression(with: cppExpression)
    }

    public static func notExists(_ select: StatementSelect) -> Expression {
        let cppExpression = withExtendedLifetime(select) {
            WCDBExpressionCreateWithNotExistStatement($0.cppObj)
        }
        return Expression(with: cppExpression)
    }

    // Function
    static func function(named name: String,
                         _ arguments: ExpressionConvertible...,
                         isDistinct: Bool = false) -> Expression {
        return function(named: name, arguments, isDistinct: isDistinct)
    }
    static func function(named name: String,
                         _ arguments: [ExpressionConvertible],
                         isDistinct: Bool = false) -> Expression {
        let expression = Expression.function(name)
        expression.invoke()
        if isDistinct {
            expression.distinct()
        }
        if arguments.count > 0 {
            expression.arguments(arguments)
        }
        return expression
    }

    public static func `case`(_ expressionConvertible: ExpressionConvertible,
                              _ flows: (when: ExpressionConvertible, then: ExpressionConvertible)...,
                              `else`: ExpressionConvertible) -> Expression {
        return `case`(expressionConvertible, flows, else: `else`)
    }
    public static func `case`(_ `case`: ExpressionConvertible,
                              _ flows: [(when: ExpressionConvertible, then: ExpressionConvertible)],
                              `else`: ExpressionConvertible) -> Expression {
        let expression = Expression.case(`case`)
        for flow in flows {
            expression.when(flow.when).then(flow.then)
        }
        expression.else(`else`)
        return expression
    }

    public func asExpression() -> Expression {
        return self
    }
}
