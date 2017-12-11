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

public class TableBindingBase {
    let allProperties: [Property]

    init(with properties: [Property]) {
        allProperties = properties
    }

    func property<CodingTableKeyType: CodingTableKey>(from codingTableKey: CodingTableKeyType) -> Property {
        Error.abort("")
    }

    public func generateCreateVirtualTableStatement(named table: String) -> StatementCreateVirtualTable {
        Error.abort("")
    }

    public func generateCreateTableStatement(named table: String) -> StatementCreateTable {
        Error.abort("")
    }

    public func generateCreateIndexStatements(onTable table: String) -> [StatementCreateIndex]? {
        Error.abort("")
    }

    func generateColumnDef(with key: CodingTableKeyBase) -> ColumnDef {
        Error.abort("")
    }

    func getPrimaryKey() -> CodingTableKeyBase? {
        Error.abort("")
    }
}

public class TableBinding<CodingTableKeyType: CodingTableKey>: TableBindingBase {
    private let properties: [CodingTableKeyType: Property]
    private let type: CodingTableKeyType.Type
    let allKeys: [CodingTableKeyType] = {
        var i = 0
        var allKeys: [CodingTableKeyType] = []
        while true {
            guard let key = (withUnsafePointer(to: &i) {
                return $0.withMemoryRebound(to: CodingTableKeyType?.self, capacity: 1, { return $0.pointee })
            }) else {
                break
            }
            allKeys.append(key)
            i += 1
        }
        return allKeys
    }()

    private lazy var columnTypes: [String: ColumnType] = {
        guard let tableEncodableType = CodingTableKeyType.Root.self as? TableEncodableBase.Type else {
            Error.abort("")
        }
        return ColumnTypeEncoder.getColumnTypes(of: tableEncodableType)
    }()

    private lazy var allColumnDef: [ColumnDef] = allKeys.map { (key) -> ColumnDef in
        return generateColumnDef(with: key)
    }

    private typealias ColumnConstraintBindingMap = [CodingTableKeyType: ColumnConstraintBinding]
    private lazy var columnConstraintBindings: ColumnConstraintBindingMap? = type.columnConstraintBindings

    private typealias IndexBindingMap = [IndexBinding.Subfix: IndexBinding]
    private lazy var indexBindings: IndexBindingMap? = type.indexBindings

    private typealias TableConstraintBindingMap = [TableConstraintBinding.Name: TableConstraintBinding]
    private lazy var tableConstraintBindings: TableConstraintBindingMap? = type.tableConstraintBindings
    private lazy var virtualTableBinding: VirtualTableBinding? = type.virtualTableBinding

    private lazy var primaryKey: CodingTableKeyType? = {
        guard let filtered = columnConstraintBindings?.filter({ (args) -> Bool in
            return args.value.isPrimary
        }) else {
            return nil
        }
        guard filtered.count == 1 else {
            if filtered.count == 0 {
                return nil
            } else {
                Error.abort("")
            }
        }
        return filtered.first!.key
    }()

    public init(_ type: CodingTableKeyType.Type) {
        self.type = type
        var allProperties: [Property] = []
        var properties: [CodingTableKeyType: Property] = [:]
        for key in allKeys {
            let property = Property(with: key)
            properties[key] = property
            allProperties.append(property)
        }
        self.properties = properties
        super.init(with: allProperties)
    }

    typealias TypedCodingTableKeyType = CodingTableKeyType
    override func property<CodingTableKeyType: CodingTableKey>(from codingTableKey: CodingTableKeyType) -> Property {
        guard let typedCodingTableKey = codingTableKey as? TypedCodingTableKeyType else {
            Error.abort("")
        }
        guard let typedProperty = properties[typedCodingTableKey] else {
            Error.abort("")
        }
        return typedProperty
    }

    override func generateColumnDef(with key: CodingTableKeyBase) -> ColumnDef {
        guard let codingTableKey = key as? CodingTableKeyType else {
            Error.abort("")
        }
        guard let columnType = columnTypes[codingTableKey.stringValue] else {
            Error.abort("")
        }
        var columnDef = ColumnDef(with: codingTableKey, and: columnType)
        if let index = columnConstraintBindings?.index(forKey: codingTableKey) {
            columnDef = columnConstraintBindings![index].value.generateColumnDef(with: columnDef)
        }
        return columnDef
    }

    public override func generateCreateVirtualTableStatement(named table: String) -> StatementCreateVirtualTable {
        guard let virtualTableBinding = self.virtualTableBinding else {
            Error.abort("Virtual table binding is not defined")
        }
        let columnModuleArguments = allColumnDef.map { ModuleArgument(with: $0) }
        let tableCostraintArguments = tableConstraintBindings?.map { (tableConstraintBinding) -> ModuleArgument in
            let key = tableConstraintBinding.key
            return ModuleArgument(with: tableConstraintBinding.value.generateConstraint(withName: key))
        } ?? []
        let arguments = columnModuleArguments + tableCostraintArguments + virtualTableBinding.arguments
        return StatementCreateVirtualTable()
            .create(virtualTable: table)
            .using(module: virtualTableBinding.module,
                   arguments: arguments)
    }

    public override func generateCreateTableStatement(named table: String) -> StatementCreateTable {
        let tableConstraints = tableConstraintBindings?.map { $0.value.generateConstraint(withName: $0.key) }
        return StatementCreateTable().create(table: table,
                                             with: allColumnDef,
                                             and: tableConstraints)
    }

    public override func generateCreateIndexStatements(onTable table: String) -> [StatementCreateIndex]? {
        guard let indexBindings = self.indexBindings else {
            return nil
        }
        return indexBindings.map { $0.value.generateCreateIndexStatement(onTable: table, withIndexSubfix: $0.key)}
    }

    override func getPrimaryKey() -> CodingTableKeyBase? {
        return primaryKey
    }
}
