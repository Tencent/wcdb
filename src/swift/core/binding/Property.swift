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

public protocol PropertyConvertible: ColumnConvertible, PropertyRedirectable {
    func isSwiftProperty() -> Bool

    var codingTableKey: CodingTableKeyBase? {get}
#if WCDB_SWIFT_BRIDGE_OBJC
    var wctProperty: WCTBridgeProperty? {get}
#endif
    func asProperty() -> Property

    func `in`(table: String) -> Property

    func of(schema schemaConvertible: SchemaConvertible) -> Property
}

public typealias PropertyOperable = PropertyConvertible & ExpressionOperable

public final class Property: Describable {
    public private(set) var description: String
    public private(set) var codingTableKey: CodingTableKeyBase?
    internal var tableBinding: UnsafeRawPointer?
    internal var tableName: String?
    internal var schema: SchemaConvertible?
#if WCDB_SWIFT_BRIDGE_OBJC
    public private(set) var wctProperty: WCTBridgeProperty?

    public init(named name: String, with codingTableKey: CodingTableKeyBase?, with wctProperty: WCTBridgeProperty?) {
        self.wctProperty = wctProperty
        self.codingTableKey = codingTableKey
        self.description = name
    }

    public init(named name: String, with wctProperty: WCTBridgeProperty?) {
        self.wctProperty = wctProperty
        self.description = name
    }
#endif
    public init(named name: String, with codingTableKey: CodingTableKeyBase?) {
        self.codingTableKey = codingTableKey
        self.description = name
    }

    public init(with codingTableKey: CodingTableKeyBase) {
        self.codingTableKey = codingTableKey
        self.description = codingTableKey.stringValue
    }

    public var name: String {
        return description
    }

    internal func copyAsNewProperty() -> Property {
#if WCDB_SWIFT_BRIDGE_OBJC
        let property = Property(named: description, with: codingTableKey, with: wctProperty)
#else
        let property = Property(named: description, with: codingTableKey)
#endif
        property.tableBinding = tableBinding
        property.tableName = tableName
        property.schema = schema
        return property
    }
}

extension Property: PropertyOperable {
    public func isSwiftProperty() -> Bool {
        return codingTableKey != nil
    }

    public func asProperty() -> Property {
        return self
    }

    public func `in`(table: String) -> Property {
        let property = copyAsNewProperty()
        property.tableName = table
        return property
    }

    public func of(schema schemaConvertible: SchemaConvertible) -> Property {
        let property = copyAsNewProperty()
        property.schema = schemaConvertible
        return property
    }

    public func asColumn() -> Column {
        let column = Column(named: name, binded: tableBinding)
        if let tableName = tableName {
            column.in(table: tableName)
        }
        if let schema = schema {
            column.of(schema: schema)
        }
        return column
    }
}
