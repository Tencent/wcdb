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
public class Update : CoreRepresentable {
    var core: Core
    private let statement = StatementUpdate()
    private let properties: [Property]
    public var changes: Int = Int.max
    
    init(with core: Core, on propertyConvertibleList: [PropertyConvertible], andTable table: String) throws {
        guard propertyConvertibleList.count > 0 else {
            throw Error.reportInterface(tag: core.tag, path: core.path, operation: .Update, code: .Misuse, message: "Updating \(table) with empty property")
        }
        guard table.count > 0 else {
            throw Error.reportInterface(tag: core.tag, path: core.path, operation: .Update, code: .Misuse, message: "Nil table name")
        }
        var updateValueList: [StatementUpdate.ValueType] = []
        var properties: [Property] = []
        for propertyConvertible in propertyConvertibleList {
            let property = propertyConvertible.asProperty()
            updateValueList.append((property, Expression.bindingParameter))
            properties.append(property)
        }
        statement.update(table: table).set(updateValueList)
        self.properties = properties
        self.core = core
    }
    
    @discardableResult
    public func `where`(_ condition: Condition) -> Update {
        statement.where(condition)
        return self
    }
    
    @discardableResult
    public func order(by orderList: [OrderBy]) -> Update {
        statement.order(by: orderList) 
        return self
    }
    
    @discardableResult
    public func limit(_ limit: Limit) -> Update {
        statement.limit(limit)
        return self
    }
    
    @discardableResult
    public func offset(_ offset: Offset) -> Update {
        statement.offset(offset)
        return self
    }
    
    public func execute(with object: CodableTable) throws {
        let coreStatement = try core.prepare(statement)
        for (index, property) in properties.enumerated() {
            let bindingIndex = index + 1
            coreStatement.bind(property.columnBinding, of: object, toIndex: bindingIndex)
        }
        try coreStatement.step()
        changes = coreStatement.changes
    }
    
    public func execute(with row: [CodableColumnBase?]) throws {
        let coreStatement = try core.prepare(statement)
        for (index, value) in row.enumerated() {
            let bindingIndex = index + 1
            coreStatement.bind(value, toIndex: bindingIndex)
        }
        try coreStatement.step()
        changes = coreStatement.changes
    }
}
