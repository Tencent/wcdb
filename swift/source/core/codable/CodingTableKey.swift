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

public protocol CodingTableKeyBase: CodingKey {
    var rootType: TableCodableBase.Type {get}
}

public protocol CodingTableKey: CodingTableKeyBase,
    Hashable,
    PropertyOperable,
    RawRepresentable where RawValue == String {
    associatedtype Root: TableCodableBase

    static var all: [Property] {get}
    static var any: Column {get}

    static var objectRelationalMapping: TableBinding<Self> {get}
    static var columnConstraintBindings: [Self: ColumnConstraintBinding]? {get}
    static var indexBindings: [IndexBinding.Subfix: IndexBinding]? {get}
    static var tableConstraintBindings: [TableConstraintBinding.Name: TableConstraintBinding]? {get}
    static var virtualTableBinding: VirtualTableBinding? {get}
}

extension CodingTableKey {
    public var rootType: TableCodableBase.Type {
        return Root.self
    }
}

extension CodingTableKey {
    public static var all: [Property] {
        return objectRelationalMapping.allProperties
    }
    public static var any: Column {
        return Column.all
    }
}

extension CodingTableKey {
    public static var columnConstraintBindings: [Self: ColumnConstraintBinding]? {
        return nil
    }

    public static var indexBindings: [IndexBinding.Subfix: IndexBinding]? {
        return nil
    }

    public static var tableConstraintBindings: [TableConstraintBinding.Name: TableConstraintBinding]? {
        return nil
    }

    public static var virtualTableBinding: VirtualTableBinding? {
        return nil
    }
}

extension CodingTableKey {
    public var codingTableKey: CodingTableKeyBase {
        return self
    }

    public func `as`(_ propertyConvertible: PropertyConvertible) -> Property {
        return Property(named: stringValue, with: propertyConvertible.codingTableKey)
    }

    public func asProperty() -> Property {
        return Self.objectRelationalMapping.property(from: self)
    }

    public func `in`(table: String) -> Property {
        return asProperty().`in`(table: table)
    }

    public func asExpression() -> Expression {
        return asColumn().asExpression()
    }

    public func asColumn() -> Column {
        return Column(named: stringValue)
    }
}
