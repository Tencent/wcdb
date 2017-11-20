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
public class SelectBase: CoreRepresentable {
    var core: Core
    let statementSelect: StatementSelect = StatementSelect()
    private var optionalCoreStatement: CoreStatement? = nil
    
    init(with core: Core) {
        self.core = core
    }
    
    func lazyCoreStatement() throws -> CoreStatement {
        if optionalCoreStatement == nil {
            optionalCoreStatement = try core.prepare(statementSelect)
        }
        return optionalCoreStatement!
    }
    
    private func finalize() throws {
        if let coreStatement = optionalCoreStatement {
            try coreStatement.finalize()
            optionalCoreStatement = nil
        }
    }
    
    @discardableResult
    public func `where`(_ condition: Condition) -> Self {
        statementSelect.where(condition)
        return self
    }
    
    @discardableResult
    public func order(by orderConvertibleList: OrderBy...) -> Self {
        return order(by: orderConvertibleList)
    }
    
    @discardableResult
    public func order(by orderConvertibleList: [OrderBy]) -> Self {
        statementSelect.order(by: orderConvertibleList)
        return self
    }
    
    @discardableResult
    public func limit(from expressionConvertibleFrom: Limit, to expressionConvertibleTo: Limit) -> Self {
        statementSelect.limit(from: expressionConvertibleFrom, to: expressionConvertibleTo)
        return self
    }
    
    @discardableResult
    public func limit(_ expressionConvertibleLimit: Limit) -> Self {
        statementSelect.limit(expressionConvertibleLimit)
        return self
    }
    
    @discardableResult
    public func offset(_ expressionConvertibleOffset: Offset) -> Self {
        statementSelect.offset(expressionConvertibleOffset)
        return self
    }
    
    @discardableResult
    public func group(by expressionConvertibleGroupList: GroupBy...) -> Self {
        return group(by: expressionConvertibleGroupList)
    }
    
    @discardableResult
    public func group(by expressionConvertibleGroupList: [GroupBy]) -> Self {
        statementSelect.group(by: expressionConvertibleGroupList)
        return self
    }
    
    @discardableResult
    public func having(_ expressionConvertibleHaving: Having) -> Self {
        statementSelect.having(expressionConvertibleHaving)
        return self
    }
    
    @discardableResult
    public func next() throws -> Bool {
        do {
            return try self.lazyCoreStatement().step()
        }catch let error {
            try! finalize()
            throw error
        }
    }
        
    func extractMultiObject(from properties: [Property]) throws -> [String:CodableTable] {
        var multiObject: [String:CodableTable] = [:]
        for (index, property) in properties.enumerated() {
            let tableName = try self.lazyCoreStatement().columnTableName(atIndex: index)
            let columnBinding = property.columnBinding
            let cls = columnBinding.`class` as! CodableTable.Type
            var object = multiObject[tableName]
            if object == nil {
                object = cls.init()
                multiObject[tableName] = object
            }
            try extract(from: property, at: index, into: &object!)
        }
        return multiObject
    }
    
    func extract<T: CodableTable>(from properties: [Property]) throws -> T {
        assert((properties.first!.columnBinding.`class` == T.self))
        let object = try extract(from: properties)
        return object as! T
    }
    
    func extract(from properties: [Property]) throws -> Any {
        let cls = (properties.first!.columnBinding.`class` as! CodableTable.Type)
        var object = cls.init()
        for (index, property) in properties.enumerated() {
            try extract(from: property, at: index, into: &object)
        }
        return object
    }
    
    func extract(from property: Property, at index: Int, into object: inout CodableTable) throws {
        let columnBinding = property.columnBinding 
        let coreStatement = try self.lazyCoreStatement()
        switch columnBinding.columnType {
        case .Integer32:
            let value: Int32 = coreStatement.value(atIndex: index)
            columnBinding.access(setFundamentalValue: value, forObject: &object)
        case .Integer64:
            let value: Int64 = coreStatement.value(atIndex: index)
            columnBinding.access(setFundamentalValue: value, forObject: &object)
        case .Float:
            let value: Double = coreStatement.value(atIndex: index)
            columnBinding.access(setFundamentalValue: value, forObject: &object)
        case .Text:
            let value: String = coreStatement.value(atIndex: index)
            columnBinding.access(setFundamentalValue: value, forObject: &object)
        case .BLOB:
            let value: Data = coreStatement.value(atIndex: index)
            columnBinding.access(setFundamentalValue: value, forObject: &object) 
        default:
            throw Error.reportInterface(tag: tag, path: path, operation: .Select, code: .Misuse, message: "Extracting column \(columnBinding.columnName) with unknown type \(columnBinding.columnType.description)")
        }
    }
    
    func extract(atIndex index: Int) throws -> FundamentalValue {
        let coreStatement = try self.lazyCoreStatement()
        switch coreStatement.type(atIndex: index) {
        case .Integer32:
            return coreStatement.value(atIndex: index) as Int32
        case .Integer64:
            return coreStatement.value(atIndex: index) as Int64
        case .Float:
            return coreStatement.value(atIndex: index) as Double
        case .Text:
            return coreStatement.value(atIndex: index) as String
        case .BLOB:
            return coreStatement.value(atIndex: index) as Data
        case .Null:
            return nil
        }
    }
    
    func extract() throws -> FundamentalRow {
        var row: FundamentalRow = []
        let coreStatement = try self.lazyCoreStatement()
        for index in 0..<coreStatement.columnCount() {
            row.append(try extract(atIndex: index))
        }
        return row
    }
}
