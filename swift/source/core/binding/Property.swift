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

public protocol PropertyConvertible: ColumnConvertible, PropertyRedirectable {
    var codingTableKey: CodingTableKeyBase {get}

    func asProperty() -> Property

    func `in`(table: String) -> Property
}

public typealias PropertyOperable = PropertyConvertible & ExpressionOperable

public final class Property: Describable {
    public private(set) var description: String
    public private(set) var codingTableKey: CodingTableKeyBase

    public init(named name: String, with codingTableKey: CodingTableKeyBase) {
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
}

extension Property: PropertyOperable {
    public func asProperty() -> Property {
        return self
    }

    public func `in`(table: String) -> Property {
        let column: Column = self.in(table: table)
        return Property(named: column.description, with: codingTableKey)
    }

    public func asColumn() -> Column {
        return Column(named: name)
    }
}
