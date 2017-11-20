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

public protocol PropertyConvertible {
    func asProperty() -> Property
    
    func `in`(table: String) -> Property
}

public class Property: Describable, ColumnBindingRepresentable {
    public let columnBinding: AnyColumnBinding
    
    public init(named name: String, with columnBinding: AnyColumnBinding) {
        self.columnBinding = columnBinding
        super.init(name)
    }    
    
    public init(with columnBinding: AnyColumnBinding) {
        self.columnBinding = columnBinding
        super.init(columnBinding.columnName)
    }
    
    public var name: String {
        return description
    }    
}

extension Property: PropertyConvertible {
    public func asProperty() -> Property {
        return self
    }
    
    public func `in`(table: String) -> Property {
        return Property(named: asColumn().in(table: table).description, with: columnBinding)
    }
}

extension Property: ColumnConvertible {
    public func asColumn() -> Column {
        return Column(named: name)
    }
}

extension Property: ExpressionOperable {}

