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

public protocol OrderConvertible {
    func asOrder() -> Order 
}

public protocol SpecificOrderConvertible: OrderConvertible {
    func asOrder(by term: OrderTerm?) -> Order
}

extension SpecificOrderConvertible {
    public func asOrder() -> Order {
        return asOrder(by: nil)
    }
}

public protocol SpecificColumnDefConvertible {
    func asDef(with columnType: ColumnType?) -> ColumnDef
}

public protocol ColumnIndexConvertible {
    func asIndex() -> ColumnIndex
}

public protocol SpecificColumnIndexConvertible: ColumnIndexConvertible {
    func asIndex(orderBy term: OrderTerm?) -> ColumnIndex 
}

extension SpecificColumnIndexConvertible {
    public func asIndex() -> ColumnIndex {
        return asIndex(orderBy: nil)
    }
}

public protocol ColumnResultConvertible {
    func asColumnResult() -> ColumnResult
}

public protocol TableOrSubqueryConvertible {
    func asTableOrSubquery() -> Subquery
}

public protocol ExpressionConvertible: ColumnResultConvertible, SpecificOrderConvertible {
    func asExpression() -> Expression 
}

extension ExpressionConvertible {
    public func asColumnResult() -> ColumnResult {
        return ColumnResult(with: asExpression())
    }
    public func asOrder(by term: OrderTerm?) -> Order {
        return Order(with: asExpression(), by: term)
    }
}

public protocol ColumnConvertible: ExpressionConvertible, SpecificColumnIndexConvertible, SpecificColumnDefConvertible {
    func asColumn() -> Column 
    func `in`(table: String) -> Column 
}

extension ColumnConvertible {
    public func asExpression() -> Expression {
        return Expression(with: self)
    }
    
    public func asIndex(orderBy term: OrderTerm?) -> ColumnIndex {
        return ColumnIndex(with: self, orderBy: term)
    }
    
    public func asDef(with columnType: ColumnType? = nil) -> ColumnDef {
        return ColumnDef(with: self, and: columnType)
    }
    
    public func `in`(table: String) -> Column {
        return asColumn().in(table: table)
    }
}

public protocol ExpressionOperable: ExpressionConvertible {
    //Unary
    prefix static func !(operand: Self) -> Expression 
    prefix static func +(operand: Self) -> Expression 
    prefix static func -(operand: Self) -> Expression 
    prefix static func ~(operand: Self) -> Expression 
    
    //Binary
    static func ||(left: Self, right: ExpressionConvertible) -> Expression 
    static func ||(left: ExpressionConvertible, right: Self) -> Expression 
    static func ||(left: Self, right: Self) -> Expression
    
    static func &&(left: Self, right: ExpressionConvertible) -> Expression 
    static func &&(left: ExpressionConvertible, right: Self) -> Expression 
    static func &&(left: Self, right: Self) -> Expression
    
    static func *(left: ExpressionConvertible, right: Self) -> Expression 
    static func *(left: Self, right: ExpressionConvertible) -> Expression 
    static func *(left: Self, right: Self) -> Expression
    
    static func /(left: ExpressionConvertible, right: Self) -> Expression 
    static func /(left: Self, right: ExpressionConvertible) -> Expression 
    static func /(left: Self, right: Self) -> Expression
    
    static func %(left: ExpressionConvertible, right: Self) -> Expression 
    static func %(left: Self, right: ExpressionConvertible) -> Expression 
    static func %(left: Self, right: Self) -> Expression 
    
    static func +(left: ExpressionConvertible, right: Self) -> Expression 
    static func +(left: Self, right: ExpressionConvertible) -> Expression 
    static func +(left: Self, right: Self) -> Expression
    
    static func -(left: ExpressionConvertible, right: Self) -> Expression 
    static func -(left: Self, right: ExpressionConvertible) -> Expression 
    static func -(left: Self, right: Self) -> Expression
    
    static func <<(left: ExpressionConvertible, right: Self) -> Expression 
    static func <<(left: Self, right: ExpressionConvertible) -> Expression 
    static func <<(left: Self, right: Self) -> Expression
    
    static func >>(left: ExpressionConvertible, right: Self) -> Expression 
    static func >>(left: Self, right: ExpressionConvertible) -> Expression 
    static func >>(left: Self, right: Self) -> Expression
    
    static func &(left: ExpressionConvertible, right: Self) -> Expression 
    static func &(left: Self, right: ExpressionConvertible) -> Expression 
    static func &(left: Self, right: Self) -> Expression
    
    static func |(left: ExpressionConvertible, right: Self) -> Expression 
    static func |(left: Self, right: ExpressionConvertible) -> Expression 
    static func |(left: Self, right: Self) -> Expression
    
    static func <(left: ExpressionConvertible, right: Self) -> Expression 
    static func <(left: Self, right: ExpressionConvertible) -> Expression 
    static func <(left: Self, right: Self) -> Expression
    
    static func <=(left: ExpressionConvertible, right: Self) -> Expression
    static func <=(left: Self, right: ExpressionConvertible) -> Expression
    static func <=(left: Self, right: Self) -> Expression
    
    static func >(left: ExpressionConvertible, right: Self) -> Expression 
    static func >(left: Self, right: ExpressionConvertible) -> Expression 
    static func >(left: Self, right: Self) -> Expression 
    
    static func >=(left: ExpressionConvertible, right: Self) -> Expression
    static func >=(left: Self, right: ExpressionConvertible) -> Expression
    static func >=(left: Self, right: Self) -> Expression
    
    static func ==(left: ExpressionConvertible, right: Self) -> Expression 
    static func ==(left: Self, right: ExpressionConvertible) -> Expression 
    static func ==(left: Self, right: Self) -> Expression 
    
    static func !=(left: ExpressionConvertible, right: Self) -> Expression 
    static func !=(left: Self, right: ExpressionConvertible) -> Expression 
    static func !=(left: Self, right: Self) -> Expression 
    
    func concat(_ operand: ExpressionConvertible) -> Expression 
    func substr(start: ExpressionConvertible, length: ExpressionConvertible) -> Expression 
    
    func like(_ operand: ExpressionConvertible) -> Expression 
    func glob(_ operand: ExpressionConvertible) -> Expression 
    func match(_ operand: ExpressionConvertible) -> Expression
    func regexp(_ operand: ExpressionConvertible) -> Expression
    func notLike(_ operand: ExpressionConvertible) -> Expression
    func notGlob(_ operand: ExpressionConvertible) -> Expression
    func notMatch(_ operand: ExpressionConvertible) -> Expression
    func notRegexp(_ operand: ExpressionConvertible) -> Expression
    
    func like(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression 
    func glob(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression 
    func match(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression
    func regexp(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression
    func notLike(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression
    func notGlob(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression
    func notMatch(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression
    func notRegexp(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression
    
    func isNull() -> Expression 
    func isNotNull() -> Expression
    func `is`(_ operand: ExpressionConvertible) -> Expression 
    func isNot(_ operand: ExpressionConvertible) -> Expression
    func between(from: ExpressionConvertible, to: ExpressionConvertible) -> Expression 
    func notBetween(from: ExpressionConvertible, to: ExpressionConvertible) -> Expression 
    static func exists(_ statementSelect: StatementSelect) -> Expression 
    static func notExists(_ statementSelect: StatementSelect) -> Expression 
    func `in`(_ statementSelect: StatementSelect) -> Expression 
    func notIn(_ statementSelect: StatementSelect) -> Expression 
    func `in`(_ expressionConvertibleList: ExpressionConvertible...) -> Expression 
    func notIn(_ expressionConvertibleList: ExpressionConvertible...) -> Expression
    func `in`(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression 
    func notIn(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression
    static func combine(_ expressionConvertibleList: ExpressionConvertible...) -> Expression
    static func combine(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression
    
    //Function
    static func function(named name: String, _ expressions: ExpressionConvertible..., isDistinct: Bool) -> Expression 
    static func function(named name: String, _ expressions: [ExpressionConvertible], isDistinct: Bool) -> Expression 
    
    //aggregate functions
    func avg(isDistinct: Bool) -> Expression 
    func count(isDistinct: Bool) -> Expression
    func groupConcat(isDistinct: Bool) -> Expression
    func groupConcat(isDistinct: Bool, separateBy seperator: String) -> Expression
    func max(isDistinct: Bool) -> Expression 
    func min(isDistinct: Bool) -> Expression 
    func sum(isDistinct: Bool) -> Expression 
    func total(isDistinct: Bool) -> Expression
    
    //core functions
    func abs(isDistinct: Bool) -> Expression 
    func hex(isDistinct: Bool) -> Expression 
    func length(isDistinct: Bool) -> Expression
    func lower(isDistinct: Bool) -> Expression 
    func upper(isDistinct: Bool) -> Expression 
    func round(isDistinct: Bool) -> Expression 
    
    static func `case`(_ `case`: ExpressionConvertible, _ flows: (when: ExpressionConvertible, then: ExpressionConvertible)..., `else`: ExpressionConvertible) -> Expression 
    static func `case`(_ `case`: ExpressionConvertible, _ flows: [(when: ExpressionConvertible, then: ExpressionConvertible)], `else`: ExpressionConvertible) -> Expression 
    
    //FTS3
    func matchinfo() -> Expression 
    func offsets() -> Expression 
    func snippet() -> Expression 
}

extension ExpressionOperable {
    private static func operate(prefix: String, operand: ExpressionConvertible) -> Expression {
        return Expression(withRaw: "(\(prefix) \(operand.asExpression().description))")
    }
    private static func operate(title: String, infix: String?, operands: [ExpressionConvertible]) -> Expression {
        return Expression(withRaw: "\(title)(\(infix != nil ? infix!+" " : "")\(operands.joined()))")
    }
    private static func operate(operand: ExpressionConvertible, postfix: String) -> Expression {
        return Expression(withRaw: "(\(operand.asExpression().description) \(postfix))")
    }
    private static func operate(left: ExpressionConvertible, `operator`: String, right: ExpressionConvertible) -> Expression {
        return Expression(withRaw: "(\(left.asExpression().description) \(`operator`) \(right.asExpression().description))")
    }
    private static func operate(one: ExpressionConvertible, operator1: String, two: ExpressionConvertible, operator2: String, three: ExpressionConvertible) -> Expression {
        return Expression(withRaw: "(\(one.asExpression().description) \(operator1) \(two.asExpression().description) \(operator2) \(three.asExpression().description))")
    }
    
    //Unary
    public prefix static func !(operand: Self) -> Expression {
        return operate(prefix: "NOT", operand: operand)
    }
    public prefix static func +(operand: Self) -> Expression {
        return operate(prefix: "", operand: operand)
    }
    public prefix static func -(operand: Self) -> Expression {
        return operate(prefix: "-", operand: operand)
    }
    public prefix static func ~(operand: Self) -> Expression {
        return operate(prefix: "~", operand: operand)
    }
    
    //Binary
    public static func ||(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left as ExpressionConvertible, operator: "OR", right: right as ExpressionConvertible)
    }
    public static func ||(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "OR", right: right)
    }
    public static func ||(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "OR", right: right)
    }
    
    public static func &&(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "AND", right: right)
    }
    public static func &&(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "AND", right: right)
    }
    public static func &&(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "AND", right: right)
    }

    public static func *(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "*", right: right)
    }
    public static func *(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "*", right: right)
    }
    public static func *(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "*", right: right)
    }

    public static func /(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "/", right: right)
    }
    public static func /(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "/", right: right)
    }
    public static func /(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "/", right: right)
    }
    
    public static func %(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "%", right: right)
    }
    public static func %(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "%", right: right)
    }
    public static func %(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "%", right: right)
    }
    
    public static func +(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "+", right: right)
    }
    public static func +(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "+", right: right)
    }
    public static func +(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "+", right: right)
    }

    public static func -(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "-", right: right)
    }
    public static func -(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "-", right: right)
    }
    public static func -(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "-", right: right)
    }

    public static func <<(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "<<", right: right)
    }
    public static func <<(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "<<", right: right)
    }
    public static func <<(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "<<", right: right)
    }

    public static func >>(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: ">>", right: right)
    }
    public static func >>(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: ">>", right: right)
    }
    public static func >>(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: ">>", right: right)
    }
    
    public static func &(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "&", right: right)
    }
    public static func &(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "&", right: right)
    }
    public static func &(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "&", right: right)
    }

    public static func |(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "|", right: right)
    }
    public static func |(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "|", right: right)
    }
    public static func |(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "|", right: right)
    }

    public static func <(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "<", right: right)
    }
    public static func <(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "<", right: right)
    }
    public static func <(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "<", right: right)
    }
    
    public static func <=(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "<=", right: right)
    }
    public static func <=(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "<=", right: right)
    }
    public static func <=(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "<=", right: right)
    }

    public static func >(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: ">", right: right)
    }
    public static func >(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: ">", right: right)
    }
    public static func >(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: ">", right: right)
    }

    public static func >=(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: ">=", right: right)
    }
    public static func >=(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: ">=", right: right)
    }
    public static func >=(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: ">=", right: right)
    }

    public static func ==(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "=", right: right)
    }
    public static func ==(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "=", right: right)
    }
    public static func ==(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "=", right: right)
    }
    
    public static func !=(left: ExpressionConvertible, right: Self) -> Expression {
        return operate(left: left, operator: "!=", right: right)
    }
    public static func !=(left: Self, right: ExpressionConvertible) -> Expression {
        return operate(left: left, operator: "!=", right: right)
    }
    public static func !=(left: Self, right: Self) -> Expression {
        return operate(left: left, operator: "!=", right: right)
    }
    
    public func concat(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "||", right: operand)
    }
    public func substr(start: ExpressionConvertible, length: ExpressionConvertible) -> Expression {
        return Expression(withRaw: "SUBSTR(\(self.asExpression().description), \(start.asExpression().description), \(length.asExpression().description))")
    }
    
    public func like(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "LIKE", right: operand)
    }
    public func glob(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "GLOB", right: operand)
    }
    public func match(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "MATCH", right: operand)
    }
    public func regexp(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "REGEXP", right: operand)
    }
    public func notLike(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "NOT LIKE", right: operand)
    }
    public func notGlob(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "NOT GLOB", right: operand)
    }
    public func notMatch(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "NOT MATCH", right: operand)
    }
    public func notRegexp(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "NOT REGEXP", right: operand)
    }
    
    public func like(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression {
        return Self.operate(one: self, operator1: "LIKE", two: operand, operator2: "ESCAPE", three: escape)
    }
    public func glob(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression {
        return Self.operate(one: self, operator1: "GLOB", two: operand, operator2: "ESCAPE", three: escape)
    }
    public func match(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression {
        return Self.operate(one: self, operator1: "MATCH", two: operand, operator2: "ESCAPE", three: escape)
    }
    public func regexp(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression {
        return Self.operate(one: self, operator1: "REGEXP", two: operand, operator2: "ESCAPE", three: escape)
    }
    public func notLike(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression {
        return Self.operate(one: self, operator1: "NOT LIKE", two: operand, operator2: "ESCAPE", three: escape)
    }
    public func notGlob(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression {
        return Self.operate(one: self, operator1: "NOT GLOB", two: operand, operator2: "ESCAPE", three: escape)
    }
    public func notMatch(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression {
        return Self.operate(one: self, operator1: "NOT MATCH", two: operand, operator2: "ESCAPE", three: escape)
    }
    public func notRegexp(_ operand: ExpressionConvertible, escape: ExpressionConvertible) -> Expression {
        return Self.operate(one: self, operator1: "NOT REGEXP", two: operand, operator2: "ESCAPE", three: escape)
    }
    
    public func isNull() -> Expression {
        return Self.operate(operand: self, postfix: "ISNULL")
    }
    public func isNotNull() -> Expression {
        return Self.operate(operand: self, postfix: "NOTNULL")
    }
    public func `is`(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "IS", right: operand)
    }
    public func isNot(_ operand: ExpressionConvertible) -> Expression {
        return Self.operate(left: self, operator: "IS NOT", right: operand)
    }
    public func between(from: ExpressionConvertible, to: ExpressionConvertible) -> Expression {
        return Self.operate(one: self, operator1: "BETWEEN", two: from, operator2: "AND", three: to)
    }
    public func notBetween(from: ExpressionConvertible, to: ExpressionConvertible) -> Expression {
        return Self.operate(one: self, operator1: "NOT BETWEEN", two: from, operator2: "AND", three: to)
    }
    public static func exists(_ statementSelect: StatementSelect) -> Expression {
        return Self.operate(prefix: "EXISTS", operand: statementSelect)
    }
    public static func notExists(_ statementSelect: StatementSelect) -> Expression {
        return Self.operate(prefix: "NOT EXISTS", operand: statementSelect)
    }
    
    public func `in`(_ statementSelect: StatementSelect) -> Expression {
        return Self.operate(prefix: "IN", operand: statementSelect)
    }
    public func notIn(_ statementSelect: StatementSelect) -> Expression {
        return Self.operate(prefix: "NOT IN", operand: statementSelect)
    }
    public func `in`(_ expressionConvertibleList: ExpressionConvertible...) -> Expression {
        return self.`in`(expressionConvertibleList)
    } 
    public func notIn(_ expressionConvertibleList: ExpressionConvertible...) -> Expression {
        return self.notIn(expressionConvertibleList)
    }
    public func `in`(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression {
        return Self.operate(operand: self, postfix: "IN(\(expressionConvertibleList.joined()))")
    } 
    public func notIn(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression {
        return Self.operate(operand: self, postfix: "NOT IN(\(expressionConvertibleList.joined()))")
    }
    
    public static func combine(_ expressionConvertibleList: ExpressionConvertible...) -> Expression {
        return combine(expressionConvertibleList)
    }
    public static func combine(_ expressionConvertibleList: [ExpressionConvertible]) -> Expression {
        return Expression(withRaw: "(\(expressionConvertibleList.joined()))")
    }
    
    //Function
    public static func function(named name: String, _ expressions: ExpressionConvertible..., isDistinct: Bool = false) -> Expression {
        return function(named: name, expressions, isDistinct: isDistinct)
    }
    public static func function(named name: String, _ expressions: [ExpressionConvertible], isDistinct: Bool = false) -> Expression {
        return Expression.operate(title: name, infix: isDistinct ? "DISTINCT" : nil, operands: expressions)
    }
    
    //aggregate functions
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
    //core functions
    public func abs(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "ABS", self, isDistinct: isDistinct)
    }
    public func hex(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "HEX", self, isDistinct: isDistinct)
    }
    public func length(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "LENGTH", self, isDistinct: isDistinct)
    }
    public func lower(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "LOWER", self, isDistinct: isDistinct)
    }
    public func upper(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "UPPER", self, isDistinct: isDistinct)
    }
    public func round(isDistinct: Bool = false) -> Expression {
        return Expression.function(named: "ROUND", self, isDistinct: isDistinct)
    }
    
    public static func `case`(_ expressionConvertible: ExpressionConvertible, _ flows: (when: ExpressionConvertible, then: ExpressionConvertible)..., `else`: ExpressionConvertible) -> Expression {
        return `case`(expressionConvertible.asExpression(), flows, else: `else`.asExpression())
    }
    public static func `case`(_ `case`: ExpressionConvertible, _ flows: [(when: ExpressionConvertible, then: ExpressionConvertible)], `else`: ExpressionConvertible) -> Expression {
        var descrption = "CASE \(`case`.asExpression().description) "
        descrption.append(flows.joined({ "WHEN \($0.when) THEN \($0.then) " }))
        descrption.append("ELSE \(`else`.asExpression().description) END")
        return Expression(withRaw: descrption)
    }
    
    //FTS3
    public func matchinfo() -> Expression {
        return Expression.function(named: "MATCHINFO", self)
    }
    public func offsets() -> Expression {
        return Expression.function(named: "OFFSETS", self)
    }
    public func snippet() -> Expression {
        return Expression.function(named: "SNIPPET", self)
    }
}

public protocol LiteralValueConvertible: ExpressionConvertible {
    func asLiteralValue() -> LiteralValue
}

extension LiteralValueConvertible {
    public func asExpression() -> Expression {
        return Expression(with: self)
    }
}

