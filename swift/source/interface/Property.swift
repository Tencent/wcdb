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

public protocol PropertyRedirectable {
    func `as`(_ propertyColumnBinding: ColumnBindingRepresentable) -> Property
}

public protocol PropertyConvertible: ColumnConvertible, PropertyRedirectable {
    func asProperty() -> Property
}

extension PropertyConvertible {
    public func asColumn() -> Column {
        return self.asProperty()
    }
    
    public func `as`(_ propertyColumnBinding: ColumnBindingRepresentable) -> Property {
        let property = self.asProperty()
        property.columnBinding = propertyColumnBinding.columnBinding
        return property
    }
}

public protocol PropertyOperable: PropertyConvertible, ExpressionOperable {
    func def(withType type: ColumnType, isPrimary: Bool, order term: OrderTerm?, isAutoIncrement: Bool) -> ColumnDef 
    func `in`(table: String) -> Property 
}

extension PropertyOperable {
    public func def(withType type: ColumnType, isPrimary: Bool = false, order term: OrderTerm? = nil, isAutoIncrement: Bool = false) -> ColumnDef {
        let columnDef = ColumnDef(with: asProperty(), and: type)
        if isPrimary {
            return columnDef.makePrimary(orderBy: term, isAutoIncrement: isAutoIncrement)
        }
        return columnDef
    }
    
    //FIXME: remove it before beta release
    public func `in`(table: String) -> Property {
        let property = asProperty()
        return Property(describle: "\(property.description).\(table)", bindWith: property.columnBinding)
    }
}

public class Property: Column {
    public internal(set) var columnBinding: AnyColumnBinding?
    //FIXME: fix it before beta release
    public static let anyProperty: Property = Property(named: "*")

    public override init(named name: String) {
        self.columnBinding = nil
        super.init(named: name)
    }
    
    public init(_ columnBinding: AnyColumnBinding) {
        self.columnBinding = columnBinding
        super.init(named: columnBinding.columnName)
    }
    
    public convenience init(_ name: String) {
        self.init(named: name)
    }    
    
    //FIXME: remove it before beta release
    init(describle description: String, bindWith columnBinding: AnyColumnBinding?) {
        self.columnBinding = columnBinding
        super.init(named: description)
    }
}

extension Property: PropertyOperable {
    public func asProperty() -> Property {
        return self
    }
}

extension Property: ColumnBindingRepresentable {}

//FIXME: remove it before beta release
extension Expression: PropertyRedirectable {
    public func `as`(_ propertyColumnBinding: ColumnBindingRepresentable) -> Property {
        return Property(describle: description, bindWith: propertyColumnBinding.columnBinding)
    }
}

//FIXME: remove it before beta release
extension ColumnResult: PropertyRedirectable {
    public func `as`(_ propertyColumnBinding: ColumnBindingRepresentable) -> Property {
        return Property(describle: description, bindWith: propertyColumnBinding.columnBinding)
    }
}
