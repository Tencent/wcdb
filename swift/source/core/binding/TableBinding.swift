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
    private static var atomicCollection = Atomic<[Int:TableBinding]>([:])
    static func from<CodableTableType: CodableTable>(_ type: CodableTableType.Type) -> TableBinding {
        let hash = unsafeBitCast(type, to: Int.self)
        var tableBinding: TableBinding! 
        atomicCollection.withValue { (collection) -> Void in
            tableBinding = collection[hash]
            if tableBinding == nil {
                tableBinding = TableBinding(type)
                collection[hash] = tableBinding
            }
        }
        return tableBinding
    }
    
    typealias Accessor = AnyColumnBinding.AnyAccessor
    
    let bindingClass: CodableTable.Type
    private let properties: [Accessor:Property]
    lazy var allProperties: [Property] = Array(properties.values)
    
    init<CodableTableType: CodableTable>(_ type: CodableTableType.Type) {
        self.bindingClass = type
        self.properties = bindingClass.columnBindings().reduce(into: [AnyColumnBinding.AnyAccessor:Property]()) {
            guard $1.class == type else {
                Error.abort("")
            }
            $0[$1.accessor] = Property(with: $1)
        }
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
    
    var columnBindings: [String:AnyColumnBinding] {
        return properties.values.reduce(into: [String:AnyColumnBinding]()) { (dic, property) in
            dic[property.name] = property.columnBinding
        }
    }

    func property(from accessor: Accessor) -> Property {
        guard let property = properties[accessor] else {
            Error.abort("")
        } 
        return property 
    }
    
    public func generateCreateVirtualTableStatement(named table: String) -> StatementCreateVirtualTable {
        guard let virtualTableBinding = self.virtualTableBinding else {
            Error.abort("Virtual table binding is not defined")
        }
        var moduleArguments = properties.values.reduce(into: [ModuleArgument]()) { 
            $0.append(ModuleArgument(with: $1.columnBinding.columnDef))
        }
        if let constraintBindings = self.constraintBindings {
            moduleArguments = constraintBindings.reduce(into: moduleArguments, { 
                $0.append(ModuleArgument(with:$1.generateConstraint()))
            })
        }
        moduleArguments += virtualTableBinding.arguments
        return StatementCreateVirtualTable().create(virtualTable: table).using(module: virtualTableBinding.module, arguments: moduleArguments)
    }
    
    public func generateCreateTableStatement(named table: String) -> StatementCreateTable {
        let columnDefList = properties.values.reduce(into: [ColumnDef]()) { 
            $0.append($1.columnBinding.columnDef)
        }
        let tableConstraints = constraintBindings?.reduce(into: [TableConstraint](), { 
            $0.append($1.generateConstraint())
        })
        return StatementCreateTable().create(table: table, with: columnDefList, and: tableConstraints)
    }
    
    public func generateCreateIndexStatements(onTable table: String) -> [StatementCreateIndex]? {
        guard let indexBindings = self.indexBindings else {
            return nil
        }
        return indexBindings.reduce(into: [StatementCreateIndex]()) { 
            $0.append($1.generateCreateIndexStatement(prefix: table))
        }
    }
}

