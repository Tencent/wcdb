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

internal enum UnaryOperateType {
    case negative
    case positive
    case tilde
    case not

    var cValue: WCDBExpressionUnaryOperateType {
        switch self {
        case .negative:
            return WCDBExpressionUnaryOperateType_Negative
        case .positive:
            return WCDBExpressionUnaryOperateType_Positive
        case .tilde:
            return WCDBExpressionUnaryOperateType_Tilde
        case .not:
            return WCDBExpressionUnaryOperateType_Not
        }
    }
}

public protocol ExpressionUnaryOperable: ExpressionConvertible {
    prefix static func ! (operand: Self) -> Expression
    prefix static func + (operand: Self) -> Expression
    prefix static func - (operand: Self) -> Expression
    prefix static func ~ (operand: Self) -> Expression
    func isNull() -> Expression
    func isNotNull() -> Expression
}

extension ExpressionUnaryOperable {
    public prefix static func ! (operand: Self) -> Expression {
        return operate(.not, operand: operand)
    }
    public prefix static func + (operand: Self) -> Expression {
        return operate(.positive, operand: operand)
    }
    public prefix static func - (operand: Self) -> Expression {
        return operate(.negative, operand: operand)
    }
    public prefix static func ~ (operand: Self) -> Expression {
        return operate(.tilde, operand: operand)
    }
    public func isNull() -> Expression {
        self.isNull(isNot: false)
    }
    public func isNotNull() -> Expression {
        self.isNull(isNot: true)
    }
    internal func isNull(isNot: Bool) -> Expression {
        let expression = asExpression()
        let cppExpression = withExtendedLifetime(expression) {
            WCDBExpressionNullOperate($0.cppObj, isNot)
        }
        return Expression(with: cppExpression)
    }
}

internal enum BinaryOperateType {
    case Concatenate
    case Multiply
    case Divide
    case Modulo
    case Plus
    case Minus
    case LeftShift
    case RightShift
    case BitwiseAnd
    case BitwiseOr
    case Less
    case LessOrEqual
    case Greater
    case GreaterOrEqual
    case Equal
    case NotEqual
    case Is
    case And
    case Or
    case Like
    case GLOB
    case RegExp
    case Match

    var cValue: WCDBExpressionBinaryOperatorType {
        switch self {
        case .Concatenate:
            return WCDBExpressionBinaryOperatorType_Concatenate
        case .Multiply:
            return WCDBExpressionBinaryOperatorType_Multiply
        case .Divide:
            return WCDBExpressionBinaryOperatorType_Divide
        case .Modulo:
            return WCDBExpressionBinaryOperatorType_Modulo
        case .Plus:
            return WCDBExpressionBinaryOperatorType_Plus
        case .Minus:
            return WCDBExpressionBinaryOperatorType_Minus
        case .LeftShift:
            return WCDBExpressionBinaryOperatorType_LeftShift
        case .RightShift:
            return WCDBExpressionBinaryOperatorType_RightShift
        case .BitwiseAnd:
            return WCDBExpressionBinaryOperatorType_BitwiseAnd
        case .BitwiseOr:
            return WCDBExpressionBinaryOperatorType_BitwiseOr
        case .Less:
            return WCDBExpressionBinaryOperatorType_Less
        case .LessOrEqual:
            return WCDBExpressionBinaryOperatorType_LessOrEqual
        case .Greater:
            return WCDBExpressionBinaryOperatorType_Greater
        case .GreaterOrEqual:
            return WCDBExpressionBinaryOperatorType_GreaterOrEqual
        case .Equal:
            return WCDBExpressionBinaryOperatorType_Equal
        case .NotEqual:
            return WCDBExpressionBinaryOperatorType_NotEqual
        case .Is:
            return WCDBExpressionBinaryOperatorType_Is
        case .And:
            return WCDBExpressionBinaryOperatorType_And
        case .Or:
            return WCDBExpressionBinaryOperatorType_Or
        case .Like:
            return WCDBExpressionBinaryOperatorType_Like
        case .GLOB:
            return WCDBExpressionBinaryOperatorType_GLOB
        case .RegExp:
            return WCDBExpressionBinaryOperatorType_RegExp
        case .Match:
            return WCDBExpressionBinaryOperatorType_Match
        }
    }
}

public protocol ExpressionBinaryOperable: ExpressionConvertible {
    static func || <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self,
        right: ExpressionConvertibleType) -> Expression

    static func && <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func * <ExpressionConvertibleType: ExpressionConvertible>
        (left: Self, right: ExpressionConvertibleType) -> Expression

    static func / <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func % <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func + <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func - <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func << <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func >> <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func & <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func | <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func < <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func <= <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func > <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func >= <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func == <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    static func != <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression

    func concat(_ operand: ExpressionConvertible) -> Expression
}

extension ExpressionBinaryOperable {
    public static func || <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .Or, right: right)
    }

    public static func && <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .And, right: right)
    }

    public static func * <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .Multiply, right: right)
    }

    public static func / <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .Divide, right: right)
    }

    public static func % <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .Modulo, right: right)
    }

    public static func + <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .Plus, right: right)
    }

    public static func - <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .Minus, right: right)
    }

    public static func << <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .LeftShift, right: right)
    }

    public static func >> <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .RightShift, right: right)
    }

    public static func & <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .BitwiseAnd, right: right)
    }

    public static func | <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .BitwiseOr, right: right)
    }

    public static func < <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .Less, right: right)
    }

    public static func <= <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .LessOrEqual, right: right)
    }

    public static func > <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .Greater, right: right)
    }

    public static func >= <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .GreaterOrEqual, right: right)
    }

    public static func == <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .Equal, right: right)
    }

    public static func != <ExpressionConvertibleType: ExpressionConvertible>(
        left: Self, right: ExpressionConvertibleType) -> Expression {
            return operate(left: left, operator: .NotEqual, right: right)
    }

    public func concat(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .Concatenate, right: operand)
    }
}

public protocol ExpressionBetweenOperable: ExpressionConvertible {
    func between(_ begin: ExpressionConvertible, _ end: ExpressionConvertible) -> Expression
    func notBetween(_ begin: ExpressionConvertible, _ end: ExpressionConvertible) -> Expression
}

extension ExpressionBetweenOperable {
    public func between(_ begin: ExpressionConvertible, _ end: ExpressionConvertible) -> Expression {
        self.between(begin, end, isNot: false)

    }
    public func notBetween(_ begin: ExpressionConvertible, _ end: ExpressionConvertible) -> Expression {
        self.between(begin, end, isNot: true)
    }

    public func between(_ begin: ExpressionConvertible, _ end: ExpressionConvertible, isNot: Bool) -> Expression {
        let selfExp = self.asExpression()
        let beginExp = begin.asExpression()
        let endExp = end.asExpression()
        let cppExpression = ObjectBridge.extendLifetime(selfExp, beginExp, endExp) {
            WCDBExpressionBetweenOperate(selfExp.cppObj,
                                         beginExp.asExpression().cppObj,
                                         endExp.asExpression().cppObj,
                                         isNot)
        }
        return Expression(with: cppExpression)

    }
}

public protocol ExpressionInOperable: ExpressionConvertible {
    func `in`(function: String) -> Expression
    func notIn(function: String) -> Expression

    func inTable(_ name: String) -> Expression
    func notInTable(_ name: String) -> Expression

    func `in`(_ statementSelect: StatementSelect) -> Expression
    func notIn(_ statementSelect: StatementSelect) -> Expression

    func `in`(_ expressionConvertibleList: ExpressionConvertible...) -> Expression
    func notIn(_ expressionConvertibleList: ExpressionConvertible...) -> Expression

    func `in`(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression
    func notIn(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression
}

extension ExpressionInOperable {
    public func `in`(function: String) -> Expression {
        self.in(function: function, isNot: false)
    }
    public func notIn(function: String) -> Expression {
        self.in(function: function, isNot: true)
    }
    internal func `in`(function: String, isNot: Bool) -> Expression {
        let expression = asExpression()
        let cppExpression = withExtendedLifetime(expression) {
            WCDBExpressionInFunctionOperate(expression.cppObj, function.cString, isNot)
        }
        return Expression(with: cppExpression)
    }

    public func inTable(_ name: String) -> Expression {
        self.inTable(name, isNot: false)
    }
    public func notInTable(_ name: String) -> Expression {
        self.inTable(name, isNot: true)
    }
    internal func inTable(_ name: String, isNot: Bool) -> Expression {
        let expression = asExpression()
        let cppExpression = withExtendedLifetime(expression) {
            WCDBExpressionInTableOperate(expression.cppObj, name.cString, isNot)
        }
        return Expression(with: cppExpression)
    }

    public func `in`(_ statementSelect: StatementSelect) -> Expression {
        self.in(statementSelect, isNot: false)
    }
    public func notIn(_ statementSelect: StatementSelect) -> Expression {
        self.in(statementSelect, isNot: true)
    }
    internal func `in`(_ statementSelect: StatementSelect, isNot: Bool) -> Expression {
        let expression = asExpression()
        let cppExpression = ObjectBridge.extendLifetime(statementSelect, expression) {
            WCDBExpressionInSelectionOperate(expression.cppObj, statementSelect.cppObj, isNot)
        }
        return Expression(with: cppExpression)
    }

    public func `in`(_ expressionConvertibleList: ExpressionConvertible...) -> Expression {
        return self.`in`(expressionConvertibleList)
    }
    public func notIn(_ expressionConvertibleList: ExpressionConvertible...) -> Expression {
        return self.notIn(expressionConvertibleList)
    }
    public func `in`(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression {
        self.in(expressionConvertibleList, isNot: false)
    }
    public func notIn(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression {
        self.in(expressionConvertibleList, isNot: true)
    }
    internal func `in`(_ expressionConvertibleList: [ExpressionConvertible], isNot: Bool) -> Expression {
        let expressions = expressionConvertibleList.map { $0.asExpression() }
        let selfExp = asExpression()
        let cppExpression = ObjectBridge.extendLifetime(selfExp, expressions) { () -> CPPExpression in
            let cppExpressions = expressions.map { $0.cppObj }
            return cppExpressions.withUnsafeBufferPointer { cppExps in
                return WCDBExpressionInExpressionOperate(selfExp.cppObj, cppExps.baseAddress, Int32(cppExps.count), isNot)
            }
        }
        return Expression(with: cppExpression)
    }
}

public protocol ExpressionCollateOperable: ExpressionConvertible {
    func collate(_ collation: String) -> Expression
}

extension ExpressionCollateOperable {
    public func collate(_ collation: String) -> Expression {
        let expression = asExpression()
        let cppExpression = withExtendedLifetime(expression) {
            WCDBExpressionCollateOperate(expression.cppObj, collation.cString)
        }
        return Expression(with: cppExpression)
    }
}

public protocol ExpressionFunctionOperable: ExpressionConvertible {
    func substr(start: ExpressionConvertible, length: ExpressionConvertible) -> Expression

    func like(_ operand: ExpressionConvertible) -> Expression
    func glob(_ operand: ExpressionConvertible) -> Expression
    func match(_ operand: ExpressionConvertible) -> Expression
    func regexp(_ operand: ExpressionConvertible) -> Expression

    func notLike(_ operand: ExpressionConvertible) -> Expression
    func notGlob(_ operand: ExpressionConvertible) -> Expression
    func notMatch(_ operand: ExpressionConvertible) -> Expression
    func notRegexp(_ operand: ExpressionConvertible) -> Expression

    func `is`(_ operand: ExpressionConvertible) -> Expression
    func isNot(_ operand: ExpressionConvertible) -> Expression

    func collate(_ collation: String) -> Expression

    // aggregate functions
    func avg(isDistinct: Bool) -> Expression
    func count(isDistinct: Bool) -> Expression
    func groupConcat(isDistinct: Bool) -> Expression
    func groupConcat(isDistinct: Bool, separateBy seperator: String) -> Expression
    func max(isDistinct: Bool) -> Expression
    func min(isDistinct: Bool) -> Expression
    func sum(isDistinct: Bool) -> Expression
    func total(isDistinct: Bool) -> Expression

    // core functions
    func abs() -> Expression
    func hex() -> Expression
    func length() -> Expression
    func lower() -> Expression
    func upper() -> Expression
    func round() -> Expression

    // FTS3/4
    func matchinfo() -> Expression
    func offsets() -> Expression
    func snippet() -> Expression
    // FTS5
    func bm25() -> Expression
    func highlight() -> Expression
    func substringMatchInfo() -> Expression
}

extension ExpressionFunctionOperable {
    public func substr(start: ExpressionConvertible, length: ExpressionConvertible) -> Expression {
        return Expression.function(named: "SUBSTR", asExpression(), start, length)
    }

    public func like(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .Like, right: operand, isNot: false)
    }
    public func glob(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .GLOB, right: operand, isNot: false)
    }
    public func match(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .Match, right: operand, isNot: false)
    }
    public func regexp(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .RegExp, right: operand, isNot: false)
    }
    public func notLike(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .Like, right: operand, isNot: true)
    }
    public func notGlob(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .GLOB, right: operand, isNot: true)
    }
    public func notMatch(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .Match, right: operand, isNot: true)
    }
    public func notRegexp(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .RegExp, right: operand, isNot: true)
    }

    public func `is`(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .Is, right: operand, isNot: false)
    }
    public func isNot(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: .Is, right: operand, isNot: true)
    }

    // aggregate functions
    public func avg(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "AVG", self, isDistinct: isDistinct)
    }
    public func count(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "COUNT", self, isDistinct: isDistinct)
    }
    public func groupConcat(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "GROUP_CONCAT", self, isDistinct: isDistinct)
    }
    public func groupConcat(isDistinct: Bool = false, separateBy seperator: String) -> Expression {
        return Expression.function(named: "GROUP_CONCAT", self, seperator, isDistinct: isDistinct)
    }
    public func max(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "MAX", self, isDistinct: isDistinct)
    }
    public func min(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "MIN", self, isDistinct: isDistinct)
    }
    public func sum(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "SUM", self, isDistinct: isDistinct)
    }
    public func total(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "TOTAL", self, isDistinct: isDistinct)
    }

    // core functions
    public func abs() -> Expression {
        return Expression.function(named: "ABS", self)
    }
    public func hex() -> Expression {
        return Expression.function(named: "HEX", self)
    }
    public func length() -> Expression {
        return Expression.function(named: "LENGTH", self)
    }
    public func lower() -> Expression {
        return Expression.function(named: "LOWER", self)
    }
    public func upper() -> Expression {
        return Expression.function(named: "UPPER", self)
    }
    public func round() -> Expression {
        return Expression.function(named: "ROUND", self)
    }

    // FTS3/4
    public func matchinfo() -> Expression {
        return Expression.function(named: "matchinfo", self)
    }
    public func offsets() -> Expression {
        return Expression.function(named: "offsets", self)
    }
    public func snippet() -> Expression {
        return Expression.function(named: "snippet", self)
    }
    // FTS5
    public func bm25() -> Expression {
        return Expression.function(named: "bm25", self)
    }
    public func highlight() -> Expression {
        return Expression.function(named: "highlight", self)
    }
    public func substringMatchInfo() -> Expression {
        return Expression.function(named: BuiltinAuxiliaryFunction.SubstringMatchInfo, self)
    }
}

extension ExpressionConvertible {
    static func operate(_ `operator`: UnaryOperateType, operand: ExpressionConvertible) -> Expression {
        let expression = operand.asExpression()
        let cppExpression = withExtendedLifetime(expression) {
            WCDBExpressionUnaryOperate(expression.cppObj, `operator`.cValue)
        }
        return Expression(with: cppExpression)
    }
    static func operate(left: ExpressionConvertible,
                        `operator`: BinaryOperateType,
                        right: ExpressionConvertible, isNot: Bool? = nil) -> Expression {
        let leftExp = left.asExpression()
        let rightExp = right.asExpression()
        if let isNot = isNot {
            let cppExpression = ObjectBridge.extendLifetime(leftExp, rightExp) {
                WCDBExpressionBinaryOperate(leftExp.cppObj, rightExp.cppObj, `operator`.cValue, isNot)
            }
            return Expression(with: cppExpression)
        } else {
            let cppExpression = ObjectBridge.extendLifetime(leftExp, rightExp) {
                WCDBExpressionBinaryOperate(leftExp.cppObj, rightExp.cppObj, `operator`.cValue, false)
            }
            return Expression(with: cppExpression)
        }
    }
}

public typealias ExpressionOperable = ExpressionUnaryOperable & ExpressionBinaryOperable & ExpressionBetweenOperable & ExpressionInOperable & ExpressionCollateOperable & ExpressionFunctionOperable
