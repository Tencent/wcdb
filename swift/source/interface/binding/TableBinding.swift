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

public class TableBinding {
    private static let spin = Spin()
    private static var collection: [String:TableBinding] = [:]
    
    static func from(_ type: CodableTable.Type) -> TableBinding {
        let description = String(describing: type)
        spin.lock(); defer { spin.unlock() }
        var tableBinding: TableBinding! = collection[description]
        if tableBinding == nil  {
            tableBinding = TableBinding(type)
            collection[description] = tableBinding
        }
        return tableBinding
    }
    
    let bindingClass: CodableTable.Type
    let accessors2Properties: [AnyColumnBinding.AnyAccessor:Property]
    let properties: [Property]
    let columnBindings: [String:AnyColumnBinding]
    
    public init(_ bindingClass: CodableTable.Type) {
        var columnBindings: [String:AnyColumnBinding] = [:]
        var accessors2Properties: [AnyColumnBinding.AnyAccessor:Property] = [:]
        var properties: [Property] = []
        for columnBinding in bindingClass.columnBindings() {
            let property = Property(columnBinding)
            
            properties.append(property)
            accessors2Properties[columnBinding.accessor] = property
            columnBindings[columnBinding.columnName] = columnBinding
        }
        self.accessors2Properties = accessors2Properties
        self.properties = properties
        self.columnBindings = columnBindings
        self.bindingClass = bindingClass
    }
    
    private var indexBindings: [IndexBinding]? {
        return bindingClass.indexBindings()
    }
    
    private var constraintBindings: [ConstraintBinding]? {
        return bindingClass.constraintBindings()
    }
    
    private var virtualTableBinding: VirtualTableBinding? {
        return bindingClass.virtualTableBinding()
    }

    func columnBinding(fromAccessor accessor: AnyKeyPath) -> AnyColumnBinding {
        return accessors2Properties[accessor]!.columnBinding!
    }
    
    func generateCreateVirtualTableStatement(named table: String) -> StatementCreateVirtualTable {
        guard virtualTableBinding != nil else {
            Error.abort("Virtual table binding is not defined")
        }
        var moduleArguments: [ModuleArgument] = virtualTableBinding?.arguments ?? []
        for property in properties {
            moduleArguments.append(ModuleArgument(with: property.columnBinding!.columnDef))
        }
        if constraintBindings != nil {
            for constraintBinding in constraintBindings! {
                moduleArguments.append(ModuleArgument(with:constraintBinding.generateConstraint()))
            }
        }
        return StatementCreateVirtualTable().create(virtualTable: table).using(module: virtualTableBinding!.module, arguments: moduleArguments)
    }
    
    func generateCreateTableStatement(named table: String) -> StatementCreateTable {
        var columnDefList: [ColumnDef] = []
        for property in properties {
            columnDefList.append(property.columnBinding!.columnDef)
        }
        var tableConstraints: [TableConstraint]? = nil
        if constraintBindings != nil {
            tableConstraints = []
            for constraintBinding in constraintBindings! {
                tableConstraints!.append(constraintBinding.generateConstraint())
            }
        }
        return StatementCreateTable().create(table: table, with: columnDefList, and: tableConstraints)
    }
    
    func generateCreateIndexStatements(onTable table: String) -> [StatementCreateIndex]? {
        guard indexBindings != nil else {
            return nil
        }
        var statementIndexes: [StatementCreateIndex] = []
        for indexBinding in indexBindings! {
            statementIndexes.append(indexBinding.generateCreateIndexStatement(prefix: table))
        }
        return statementIndexes
    }
}

