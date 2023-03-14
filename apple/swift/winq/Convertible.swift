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

public protocol OrderingTermConvertible {
    func asOrder() -> OrderingTerm
    func order(_ order: Order?) -> OrderingTerm
    func collate(_ collation: String) -> OrderingTerm
}

public protocol SpecificColumnDefConvertible {
    func asDef(with columnType: ColumnType?) -> ColumnDef
}

public protocol IndexedColumnConvertible {
    func asIndex() -> IndexedColumn
}

public protocol SpecificColumnIndexConvertible: IndexedColumnConvertible {
    func asIndex(orderBy term: Order?) -> IndexedColumn
}

extension SpecificColumnIndexConvertible {
    public func asIndex() -> IndexedColumn {
        return asIndex(orderBy: nil)
    }
}

public protocol ResultColumnConvertible {
    func asResultColumn() -> ResultColumn
}

public protocol SchemaConvertible {
    func asSchema() -> Schema
}

public protocol QualifiedTableConvertible {
    func asQualifiedTable() -> QualifiedTable
}

public protocol TableOrSubqueryConvertible {
    func asTableOrSubquery() -> TableOrSubquery
}

public protocol ExpressionConvertible: ResultColumnConvertible, OrderingTermConvertible {
    func asExpression() -> Expression
}

extension ExpressionConvertible {
    public func asResultColumn() -> ResultColumn {
        return ResultColumn(with: asExpression())
    }
    public func asOrder() -> OrderingTerm {
        return OrderingTerm(with: self)
    }
    public func order(_ order: Order?) -> OrderingTerm {
        return asOrder().order(order)
    }

    public func collate(_ collation: String) -> OrderingTerm {
        return asOrder().collate(collation)
    }
}

public protocol ColumnConvertible: ExpressionConvertible, SpecificColumnIndexConvertible, SpecificColumnDefConvertible {
    func asColumn() -> Column
    func `in`(table: String) -> Column
    func of(schema schemaConvertible: SchemaConvertible) -> Column
}

extension ColumnConvertible {
    public func asExpression() -> Expression {
        return Expression(with: self)
    }

    public func asIndex(orderBy term: Order?) -> IndexedColumn {
        let indexedColumn = IndexedColumn(with: self)
        if let term = term {
            indexedColumn.order(term)
        }
        return indexedColumn
    }

    public func asDef(with columnType: ColumnType? = nil) -> ColumnDef {
        return ColumnDef(with: self, and: columnType)
    }

    public func `in`(table: String) -> Column {
        return asColumn().in(table: table)
    }
    public func of(schema schemaConvertible: SchemaConvertible) -> Column {
        return asColumn().of(schema: schemaConvertible)
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
