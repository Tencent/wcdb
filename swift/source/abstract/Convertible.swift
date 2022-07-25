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

public protocol LiteralValueConvertible: ExpressionConvertible {
    func asLiteralValue() -> LiteralValue
}

extension LiteralValueConvertible {
    public func asExpression() -> Expression {
        return Expression(with: self)
    }
}
